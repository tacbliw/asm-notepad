#include <iostream>
#include <Windows.h>

WNDCLASSEXA gWndClsEx;
RECT gRect = {};

CHAR wste_class[] = "wildcat";
CHAR window_name[] = "WTF is WildCat doin ?";



LRESULT CALLBACK window_proc(_In_ HWND hwnd, _In_ UINT uMsg, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    if (uMsg == WM_DESTROY) {
        PostQuitMessage(0);
    }
    else if (uMsg == WM_SIZE) {
        DefWindowProcA(hwnd, uMsg, wParam, lParam);
    }
    else {
        return 0;
    }
    return 0;
}

int main()
{   
    gWndClsEx.cbSize = 48;
    gWndClsEx.lpfnWndProc = &window_proc;
    gWndClsEx.hInstance = GetModuleHandleA(0);
    gWndClsEx.hbrBackground = (HBRUSH)COLOR_WINDOW;
    gWndClsEx.lpszClassName = wste_class;
    RegisterClassExA(&gWndClsEx);
    CreateWindowExA(0, window_name, wste_class, WS_OVERLAPPEDWINDOW + WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, 0, 0, gWndClsEx.hInstance, 0);
    return 0;

}