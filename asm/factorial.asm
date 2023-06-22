;godbolt.org

bits 64
default rel

segment .data
	factorial_message db "Factorial is: %d",0xd,0xa,0

segment .text

extern printf

global main
main:
	push rbp
	mov rbp,rsp
	sub rsp,32

	mov ecx,5
	call factorial

	mov edx,eax
	lea rax,[factorial_message]
	mov rcx,rax
	call printf
	mov eax,0

	add rsp,32
	pop rbp
	ret

factorial:
	push rbp
	mov rbp,rsp
	sub rsp,16
	mov dword 16[rbp],ecx
	cmp dword 16[rbp],0
	jne .factorial_varinit
	mov eax,1
	jmp .factorial_return
.factorial_varinit:
	mov dword -4[rbp],1
	mov dword -8[rbp],1
	jmp .factorial_jle
.factorial_loop:
	mov eax,dword -4[rbp]
	imul eax,dword -8[rbp]
	mov dword -4[rbp],eax
	add dword -8[rbp],1
.factorial_jle:
	mov eax,dword -8[rbp]
	cmp eax,dword 16[rbp]
	jle .factorial_loop
	mov eax,dword -4[rbp]
.factorial_return:
	add rsp,16
	pop rbp
	ret
