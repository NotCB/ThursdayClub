bits 64
default rel

segment .data
	msg db "Hello World!",0xd,0xa,0

segment .text
global main
extern printf

main:
	lea rcx,[msg]
	call printf
