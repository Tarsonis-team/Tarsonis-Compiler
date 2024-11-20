; ModuleID = 'I_module'
source_filename = "I_module"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @two(i32 %0) {
entry:
  %a = alloca i32, align 4
  store i32 %0, i32* %a, align 4
  store i32 9, i32* %a, align 4
  %a1 = load i32, i32* %a, align 4
  %addtmp = add i32 2, %a1
  ret i32 %addtmp
}

define void @main() {
entry:
  %a = alloca i32, align 4
  store i32 5, i32* %a, align 4
  %c = alloca i32, align 4
  %a1 = load i32, i32* %a, align 4
  %call_two = call i32 @two(i32 %a1)
  store i32 %call_two, i32* %c, align 4
  %c2 = load i32, i32* %c, align 4
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %c2)
  %a3 = load i32, i32* %a, align 4
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0), i32 %a3)
  ret void
}

declare i32 @printf(i8*, ...)
