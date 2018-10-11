; ModuleID = 'harbour_module'

define i32 @main() {
main_block:
  %x = alloca i32
  store i32 15, i32* %x
  ret i32 0
}
