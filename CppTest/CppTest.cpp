#include <iostream>
#include <Windows.h>

WNDCLASSEXA wcx = { };
RECT rect = {};
HWND hwnd_main = NULL;
HWND hwnd_textbox;
MSG msg;
CHAR filename[MAX_PATH];
HANDLE file_handle;
DWORD file_size;
HANDLE heap_handle;
LPVOID file_data_ptr;
DWORD num_bytes_read;
OPENFILENAMEA ofn = {};

const char wste_class[] = "main_class";
const char window_name[] = "C++ text editor - wildcat";
const char edit_class[] = "EDIT";
const char error_message[] = "Error.";
const char file_filters[] = "Any file (*.*)\0*.*";
const char textbox_font[] = "Consolas";

void error_msgbox() {
    MessageBoxA(HWND_DESKTOP, error_message, error_message, MB_OK);
}

void read_file() {
    HANDLE result = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (result == INVALID_HANDLE_VALUE) {
        error_msgbox();
        return;
    }
    file_handle = result;
    file_size = GetFileSize(file_handle, NULL) + 1;
    heap_handle = GetProcessHeap();
    file_data_ptr = HeapAlloc(heap_handle, HEAP_ZERO_MEMORY, file_size);
    BOOL success = ReadFile(file_handle, file_data_ptr, file_size, &num_bytes_read, NULL);
    SetWindowTextA(hwnd_textbox, (LPCSTR)file_data_ptr);
    CloseHandle(file_handle);
    HeapFree(heap_handle, 0, file_data_ptr);
}

void write_file() {
    heap_handle = GetProcessHeap();
    file_size = GetWindowTextLengthA(hwnd_textbox) + 1;
    LPVOID  file_data_ptr = HeapAlloc(heap_handle, HEAP_ZERO_MEMORY, file_size);
    GetWindowTextA(hwnd_textbox, (LPSTR)file_data_ptr, file_size);
    HANDLE result = CreateFileA(filename, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (result == INVALID_HANDLE_VALUE) {
        error_msgbox();
    }
    file_handle = result;
    WriteFile(file_handle, file_data_ptr, file_size - 1, &num_bytes_read, NULL);
    CloseHandle(file_handle);
    HeapFree(heap_handle, 0, file_data_ptr);
}

void open_file() {
    ofn.hwndOwner = hwnd_main;
    if (GetOpenFileNameA(&ofn)) {
        read_file();
        SetWindowTextA(hwnd_main, filename);
    }
}

void save_file() {
    ofn.hwndOwner = hwnd_main;
    if (GetSaveFileNameA(&ofn)) {
        write_file();
        SetWindowTextA(hwnd_main, filename);
    }
}

void resize_textbox() {
    GetClientRect(hwnd_main, &rect);
    MoveWindow(hwnd_textbox, rect.left, rect.top, rect.right, rect.bottom, TRUE);
}

void create_textbox() {
    hwnd_textbox = CreateWindowExA(0, edit_class, NULL,
        ES_LEFT + ES_MULTILINE + ES_WANTRETURN + WS_VISIBLE + WS_CHILD + WS_HSCROLL + WS_VSCROLL,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hwnd_main,
        NULL,
        wcx.hInstance, NULL);
    if (hwnd_textbox == NULL) {
        std::cout << "CreateWindowExA at create_textbox() error " << GetLastError() << std::endl;
    }
    SetFocus(hwnd_textbox);
    HFONT font = CreateFontA(14, 0, 0, 0, 0, FALSE, FALSE, FALSE,
        ANSI_CHARSET,
        OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS,
        PROOF_QUALITY,
        DEFAULT_PITCH,
        textbox_font
    );
    SendMessageA(hwnd_textbox, WM_SETFONT, (WPARAM)font, TRUE);
    resize_textbox();
}

static LRESULT CALLBACK window_proc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_SIZE:
        resize_textbox();
        break;
    default:
        return DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    return FALSE;
}

int main()
{   
    WORD keystate = 0;

    ofn.lStructSize = sizeof ofn;
    ofn.lpstrFilter = file_filters;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_HIDEREADONLY;

    wcx.cbSize = sizeof(WNDCLASSEXA);
    wcx.lpfnWndProc = window_proc;
    wcx.hInstance = GetModuleHandleA(0);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = wste_class;
    RegisterClassExA(&wcx);
    hwnd_main = CreateWindowExA(0, wste_class, window_name,
        WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480, NULL, NULL, wcx.hInstance, 0);
    if (hwnd_main == NULL) {
        std::cout << "CreateWindowExA error " << GetLastError() << std::endl;
        ExitProcess(0);
    }
    create_textbox();

    // message_loop:
    while (1) {
        if (GetMessageA(&msg, NULL, 0, 0) == 0) {
            std::cout << "GetMessageA returns 0 -> error code " << GetLastError() << std::endl;
            break;
        }
        if (msg.message == WM_KEYDOWN) {
            keystate = GetKeyState(VK_CONTROL);
            if ((keystate & 0b10000000) != 0) {
                if ((byte)msg.wParam == 0x4f) { // O
                    open_file();
                }
                else if ((byte)msg.wParam == 0x53) { // S
                    keystate = GetKeyState(VK_SHIFT);
                    if (keystate & 0b10000000) {    // Shift pressed
                        save_file();
                    }
                    else {
                        if (filename[0] == 0) {
                            save_file();
                        }
                        else {
                            write_file();
                        }
                    }
                }
                continue;
            }
        }
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    ExitProcess(0);
    return 0;
}