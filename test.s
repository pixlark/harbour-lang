	.arch armv4t
	.global main
	.text
	.arm
main:
	push {fp, lr}
	sub sp, #9 @ Creating stack frame
	
	ldr r0, =string
	mov r1, #15
	bl printf

	add sp, #9 @ Removing stack frame
	pop {fp, lr}
	bx lr

	.data
string:
	.ascii "test: %d\n\0"
