.data
    HelloWorld db "Hello World!", 0

extern MessageBoxA: proc
extern ExitProcess: proc

.code
start proc
    push rbp
    mov rbp, rsp
    sub rsp, 10h
    mov r9, 0 ; MB_OK
    mov r8, offset HelloWorld
    mov rdx, offset HelloWorld
    mov rcx, 0
    call MessageBoxA

    mov rcx, 0
    call ExitProcess
start endp  

end