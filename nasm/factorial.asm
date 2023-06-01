default rel
bits 64

segment .data
	fmt db "Factorial is: %d",0xd,0xa,0

segment .text

global main
global factorial

extern _CRT_INIT
extern ExitProcess
extern printf

factorial:
	push rbp
	mov rbp,rsp
	sub rsp,32
	test ecx,ecx
	jz .zero
	mov ebx,1
	mov eax,1
	inc ecx

.for_loop:
	cmp ebx,ecx
	je .end_loop
	mul ebx
	inc ebx
	jmp .for_loop

.zero:
	mov eax,1

.end_loop:
	leave
	ret

main:
	push rbp
	mov rbp,rsp
	sub rsp,32

	mov rcx,5
	call factorial

	lea rcx,[fmt]
	mov rdx,rax
	call printf

	xor rax,rax
	call ExitProcess
