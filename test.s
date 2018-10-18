	.arch armv4t
	.global main
	.text
	.arm
main:
	push {fp, lr}

	mov fp, sp
	sub sp, #4

	mov r0, #101
	str r0, [fp, #-4]

	ldr r0, =string
	ldr r1, [fp, #-4]
	bl printf
	
	add sp, #4

	pop {fp, lr}
	bx lr

	.data
string:
	.ascii "test: %d\n\0"
