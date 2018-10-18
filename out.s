@emit_header
.arch armv4t
.global main
.text
.arm
@emit_label
main:
@emit_func_save
push {fp, lr}
add  fp, sp, #4
@emit_push_i32
mov r12, #10
@emit_push
push {r12}
@emit_push_i32
mov r12, #2
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
@emit_push_i32
mov r12, #4
@emit_push
push {r12}
@emit_pop
pop {r1}
@emit_pop
pop {r0}
@emit_op
mul r0, r1
@emit_push
push {r0}
@emit_push_i32
mov r12, #6
@emit_push
push {r12}
@emit_pop
pop {r1}
@emit_pop
pop {r0}
@emit_op
push {r0, r1}
pop  {r0, r1}
bl __aeabi_idiv
@emit_push
push {r0}
@emit_pop
pop {r0}
@emit_func_load
pop {fp, lr}
bx  lr
