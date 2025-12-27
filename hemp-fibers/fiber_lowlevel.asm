
.code 

PUBLIC swap_registers
PUBLIC start_fiber

; assume register context in rcx
swap_registers PROC

mov rax, [rsp] ; We called to get here, so rsp is on the stack and use later

; r15
mov r8, [rcx + 000h]
mov [rcx + 000h], r15
mov r15, r8

; r14
mov r8, [rcx + 008h]
mov [rcx + 008h], r14
mov r14, r8

; r13
mov r8, [rcx + 010h]
mov [rcx + 010h], r13
mov r13, r8

; r12
mov r8, [rcx + 018h]
mov [rcx + 018h], r12
mov r12, r8

; rbx
mov r8, [rcx + 020h]
mov [rcx + 020h], rbx
mov rbx, r8

; rbp
mov r8, [rcx + 028h]
mov [rcx + 028h], rbp
mov rbp, r8

; rsp
add rsp, 8
mov r8, [rcx + 030h]
mov [rcx + 030h], rsp
mov rsp, r8

mov r8, [rcx + 038h]
mov [rcx + 038h], rax ; Store rip
jmp r8
swap_registers ENDP

start_fiber PROC
mov rax, [rsp] ; We called to get here, so rsp is on the stack and use later

; r15
mov r8, [rcx + 000h]
mov [rcx + 000h], r15
mov r15, r8

; r14
mov r8, [rcx + 008h]
mov [rcx + 008h], r14
mov r14, r8

; r13
mov r8, [rcx + 010h]
mov [rcx + 010h], r13
mov r13, r8

; r12
mov r8, [rcx + 018h]
mov [rcx + 018h], r12
mov r12, r8

; rbx
mov r8, [rcx + 020h]
mov [rcx + 020h], rbx
mov rbx, r8

; rbp
mov r8, [rcx + 028h]
mov [rcx + 028h], rbp
mov rbp, r8

; rsp
add rsp, 8
mov r8, [rcx + 030h]
mov [rcx + 030h], rsp
mov rsp, r8

mov r8, [rcx + 038h]
mov [rcx + 038h], rax ; Store rip

mov rcx, rdx
jmp r8
start_fiber ENDP


END

