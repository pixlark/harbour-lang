@emit_header
.arch armv4t
.global main
.text
.arm
@emit_label
main:
@emit_func_start
push {fp, lr}
mov fp, sp
sub  sp, #8
@emit_push_i32
mov r12, #15
@emit_push
push {r12}
@emit_pop
pop {r0}
@emit_stack_load
str r0, [fp, #8]
@emit_stack_load
ldr r0, [fp, #8]
@emit_push
push {r0}
@emit_push_i32
mov r12, #15
@emit_push
push {r12}
@emit_pop
pop {r1}
@emit_pop
pop {r0}
@emit_op
add r0, r1
@emit_push
push {r0}
@emit_pop
pop {r0}
@emit_stack_load
str r0, [fp, #4]
@emit_stack_load
ldr r0, [fp, #4]
@emit_push
push {r0}
@emit_pop
pop {r1}
ldr r0, =D__fmt_string
bl printf
@emit_func_end
add sp, #8
pop {fp, lr}
bx  lr
.data
D__fmt_string:
.ascii "%d\n"
