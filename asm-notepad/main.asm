; Comdlg32
extern GetSaveFileNameA: proc
extern GetOpenFileNameA: proc

; Gdi32
extern CreateFontA: proc

; Kernel32
extern CloseHandle: proc
extern CreateFileA: proc
extern ExitProcess: proc
extern GetFileSize: proc
extern GetModuleHandleA: proc
extern GetProcessHeap: proc
extern HeapAlloc: proc
extern HeapFree: proc
extern ReadFile: proc
extern WriteFile: proc

; User32
extern CreateWindowExA: proc
extern DefWindowProcA: proc
extern DispatchMessageA: proc
extern GetClientRect: proc
extern GetKeyState: proc
extern GetMessageA: proc
extern GetWindowTextA: proc
extern GetWindowTextLengthA: proc
extern MessageBoxA: proc
extern MoveWindow: proc
extern PostQuitMessage: proc
extern RegisterClassExA: proc
extern SendMessageA: proc
extern SetFocus: proc
extern SetWindowTextA: proc
extern TranslateMessage: proc

; macros
startfunc macro
   push rbp
   mov rbp, rsp
endm


endfunc macro
    mov rsp, rbp
    pop rbp
    ret
endm

; structs

; typedef struct tagRECT {
;   LONG left;
;   LONG top;
;   LONG right;
;   LONG bottom;
; } RECT, *PRECT, *NPRECT, *LPRECT;
RECT struct
    left        dq ?
    top         dq ?
    right       dq ?
    bottom      dq ?
RECT ends

; typedef struct tagWNDCLASSEXA {
;   UINT      cbSize;
;   UINT      style;
;   WNDPROC   lpfnWndProc;
;   int       cbClsExtra;
;   int       cbWndExtra;
;   HINSTANCE hInstance;
;   HICON     hIcon;
;   HCURSOR   hCursor;
;   HBRUSH    hbrBackground;
;   LPCSTR    lpszMenuName;
;   LPCSTR    lpszClassName;
;   HICON     hIconSm;
; } WNDCLASSEXA, *PWNDCLASSEXA, *NPWNDCLASSEXA, *LPWNDCLASSEXA;
WNDCLASSEXA struct
    cbSize          dd  ?
    style           dd  ?
    lpfnWndProc     dq  ?
    cbClsExtra      dd  ?
    cbWndExtra      dd  ?
    hInstance       dq  ?
    hIcon           dq  ?
    hCursor         dq  ?
    hbrBackground   dq  ?
    lpszMenuName    dq  ?
    lpszClassName   dq  ?
    hIconSm         dq  ?
WNDCLASSEXA ends
; globals
.data
    HelloWorld      db "Hello World", 0
    edit_class      db "EDIT", 0
    wste_class      db "WSTE_Main", 0
    textbox_font    db "Consolas", 0
    window_title    db "WildCat's Text Editor", 0
    file_filters    db "Any file (*.*)", 0, "*.*", 0, 0
    error_msg       db "Error.", 0

    file_data_ptr   dq ?
    heap_handle     dq ?
    file_handle     dq ?
    file_size       dq ?
    num_bytes_read  dq ?
    hwnd_textbox    dq ?
    hwnd_main       dq ?

    gRect           RECT <?, ?, ?, ?>
    gWndClsEx        WNDCLASSEXA <>


.code
resize_textbox proc
    startfunc
    mov rdx, offset gRect
    mov rcx, hwnd_main
    call GetClientRect
    mov rbx, offset gRect
    push 1
    push [rbx+18h]
    mov r9, [rbx+10h]
    mov r8, [rbx+8]
    mov rdx, [rbx]
    mov rcx, hwnd_main
    call MoveWindow
    endfunc
resize_textbox endp

; LRESULT CALLBACK WindowProc(
;   _In_ HWND   hwnd,       ; rcx
;   _In_ UINT   uMsg,       ; rdx
;   _In_ WPARAM wParam,     ; r8
;   _In_ LPARAM lParam      ; r9
; )
window_proc proc
    startfunc
    cmp rdx, 2h     ; WM_DESTROY
    jne neq_wm_destroy
    mov rdx, 0
    call PostQuitMessage
    jmp window_proc_end
neq_wm_destroy:
    cmp rdx, 5h     ; WM_SIZE
    jne neq_wm_size
    call resize_textbox
    jmp window_proc_end
neq_wm_size:
    call DefWindowProcA
window_proc_end:
    endfunc
window_proc endp

start proc
    startfunc
    sub rsp, 100h
    mov rcx, 0
    call GetModuleHandleA
    mov gWndClsEx.cbSize, 48
    mov rbx, offset window_proc
    mov gWndClsEx.lpfnWndProc, rbx
    mov gWndClsEx.hInstance, rax        ; ModuleHandle
    mov gWndClsEx.hbrBackground, 6      ; COLOR_WINDOW + 1
    mov rbx, offset wste_class
    mov gWndClsEx.lpszClassName, rbx
    mov rcx, offset gWndClsEx
    call RegisterClassExA

    push 0
    push [gWndClsEx.hInstance]
    push 0
    push 0
    push 480
    push 640
    push 80000000h              ; CW_USEDEFAULT
    push 80000000h              ; CW_USEDEFAULT
    mov r9, 10CF0000h           ; WS_OVERLAPPEDWINDOW + WS_VISIBLE
    mov r8, offset window_title
    mov rdx, offset wste_class
    mov rcx, 0
    push 0
    push 0
    push 0
    push 0
    call CreateWindowExA

    mov rcx, 0
    call ExitProcess
    endfunc
start endp

end