#include <iostream>
#include <Windows.h>

WNDCLASSEXA wcx = { };
RECT rect = {};
HWND hwnd_main;
HWND hwnd_textbox;
MSG msg;
CHAR filename[MAX_PATH];
HANDLE file_handle;
DWORD file_size;
HANDLE heap_handle;
LPVOID file_data_ptr;
DWORD num_bytes_read;
OPENFILENAMEA ofn = {};

const char wste_class[] = "wildcat";
const char window_name[] = "wtf is WildCat doin ?";
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
    file_size = GetWindowTextLength(hwnd_textbox) + 1;
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
        SetWindowTextA(hwnd_main, filename);
    }
}

void resize_textbox() {
    GetClientRect(hwnd_main, &rect);
    MoveWindow(hwnd_textbox, rect.left, rect.top, rect.right, rect.bottom, TRUE);
}

void create_textbox() {
    hwnd_textbox = CreateWindowExA(WS_EX_LEFT, edit_class, NULL,
        ES_LEFT + ES_MULTILINE + ES_WANTRETURN + WS_VISIBLE + WS_CHILD + WS_HSCROLL + WS_VSCROLL,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        hwnd_main,
        NULL,
        wcx.hInstance, NULL);
    SetFocus(hwnd_textbox);
    HFONT font = CreateFontA(14, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, textbox_font);
    SendMessageA(hwnd_textbox, WM_SETFONT, (WPARAM)font, TRUE);
    resize_textbox();
}

LRESULT CALLBACK window_proc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
    }
    else if (uMsg == WM_SIZE) {
        DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    else {
        resize_textbox();
    }
    return 0;
}

int main()
{   
    WORD keystate = 0;

    ofn.lStructSize = 88;
    ofn.lpstrFilter = file_filters;
    ofn.nFilterIndex = 1;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_HIDEREADONLY;

    wcx.cbSize = sizeof(WNDCLASSEXA);
    wcx.lpfnWndProc = window_proc;
    wcx.hInstance = GetModuleHandle(0);
    wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcx.lpszClassName = wste_class;
    RegisterClassExA(&wcx);
    std::cout << "RegisterClassExA error " << GetLastError() << " " << sizeof(WNDCLASSEXA) << std::endl;
    hwnd_main = CreateWindowExA(0, window_name, wste_class,
        WS_OVERLAPPEDWINDOW + WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
        640, 480, 0, 0, wcx.hInstance, 0);
    if (hwnd_main == NULL) {
        std::cout << "CreateWindowExA error " << GetLastError() << std::endl;
        ExitProcess(0);
    }
    create_textbox();

    // message_loop:
    while (1) {
        if (GetMessageA(&msg, NULL, 0, 0)) {
            break;
        }
        if (msg.message == WM_KEYDOWN) {
            keystate = GetKeyState(VK_CONTROL);
            if (keystate & 0b1000) {
                TranslateMessage(&msg);
                DispatchMessageA(&msg);
            }
            else {
                if ((byte)msg.wParam == 0x4f) { // O
                    std::cout << "open_file()" << std::endl;
                    // open_file()
                }
                else if ((byte)msg.wParam == 0x53) { // S
                    keystate = GetKeyState(VK_SHIFT);
                    if (keystate & 0b1000) {    // Shift pressed
                        std::cout << "save_file()" << std::endl;
                        // save_file()
                    }
                    else {
                        if (filename[0] == 0) {
                            std::cout << "save_file()" << std::endl;
                            // save_file()
                        }
                        else {
                            std::cout << "write_file()" << std::endl;
                            // write_file()
                        }
                    }
                }
                else {
                    TranslateMessage(&msg);
                    DispatchMessageA(&msg);
                }
            }
        }

    }

    ExitProcess(0);
    return 0;
}