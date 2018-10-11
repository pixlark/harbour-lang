; ModuleID = 'harbour_module'

define i32 @main() {
main_block:
  %x = alloca i32
  store i32 15, i32* %x
  %0 = load i32, i32* %x
  %1 = add i32 %0, 2
  %y = alloca i32
  store i32 %1, i32* %y
  ret i32 0
}
