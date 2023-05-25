global main
extern puts
section .text

main:
	sub rsp,28h
	mov rcx,message
	call puts
	add rsp,28h
	ret
message:
	db	'Hello World',0	;C strings require a zero byte at the end
