	.arch armv4t
	.global main
	.text
	.arm
main:
	push {fp, lr}
	mov fp, sp
	sub  sp, #8
	mov r12, #15
	push {r12}
	pop {r0}
	str r0, [fp, #8]
	ldr r0, [fp, #8]
	push {r0}
	mov r12, #15
	push {r12}
	pop {r1}
	pop {r0}
	add r0, r1
	push {r0}
	pop {r0}
	str r0, [fp, #4]
	ldr r0, [fp, #4]
	push {r0}
	pop {r1}
	ldr r0, =D__fmt_string
	bl printf
	add sp, #8
	pop {fp, lr}
	bx  lr
	@mov r7, #1
	@mov r0, #101
	@swi #0
	
	.data
D__fmt_string:
	.ascii "%d\n\0"
