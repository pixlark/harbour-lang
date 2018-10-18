@emit_header
.arch armv4t
.global main
.text
.arm
@emit_label
main:
@emit_func_save
push {fp, lr}
@add  fp, sp, #4
@emit_push_i32
mov r12, #1
@emit_push
push {r12}
@emit_pop
pop {r1}
ldr r0, =D__fmt_string
bl printf
@emit_push_i32
mov r12, #2
@emit_push
push {r12}
@emit_pop
pop {r1}
ldr r0, =D__fmt_string
bl printf
@emit_push_i32
mov r12, #3
@emit_push
push {r12}
@emit_pop
pop {r1}
ldr r0, =D__fmt_string
bl printf
@emit_func_load
pop {fp, lr}
bx  lr
.data
D__fmt_string:
.ascii "%d\n"
