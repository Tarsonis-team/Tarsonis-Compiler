; ModuleID = 'I_module'
source_filename = "I_module"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define void @main() {
entry:
  %a = alloca i32, align 4
  store i32 10, ptr %a, align 4
  %b = alloca i32, align 4
  store i32 8, ptr %b, align 4
  br i1 true, label %then, label %ifskip6

then:                                             ; preds = %entry
  %b1 = alloca i32, align 4
  store i32 11, ptr %b1, align 4
  %b2 = load i32, ptr %b1, align 4
  %0 = call i32 (ptr, ...) @printf(ptr @0, i32 %b2)
  br i1 true, label %then3, label %ifskip

then3:                                            ; preds = %then
  %a4 = alloca i32, align 4
  store i32 13, ptr %a4, align 4
  %a5 = load i32, ptr %a4, align 4
  %1 = call i32 (ptr, ...) @printf(ptr @1, i32 %a5)
  br label %ifskip

ifskip:                                           ; preds = %then3, %then
  br label %ifskip6

ifskip6:                                          ; preds = %ifskip, %entry
  %c = alloca i32, align 4
  store i32 42, ptr %c, align 4
  %a7 = load i32, ptr %a, align 4
  %b8 = load i32, ptr %b, align 4
  %subtmp = sub i32 %a7, %b8
  store i32 %subtmp, ptr %c, align 4
  %c9 = load i32, ptr %c, align 4
  %2 = call i32 (ptr, ...) @printf(ptr @2, i32 %c9)
  ret void
}

declare i32 @printf(ptr, ...)
