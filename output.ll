; ModuleID = 'I_module'
source_filename = "I_module"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define void @main() {
entry:
  %a = alloca [5 x [3 x i32]], align 4
  %b = alloca [3 x i32], align 4
  %arr_index = getelementptr [3 x i32], [3 x i32]* %b, i32 0, i32 3
  store i32 7, i32* %arr_index, align 4
  %b1 = load [3 x i32], [3 x i32]* %b, align 4
  %arr_index2 = getelementptr [5 x [3 x i32]], [5 x [3 x i32]]* %a, i32 0, i32 1
  store [3 x i32] %b1, [3 x i32]* %arr_index2, align 4
  %arr_index3 = getelementptr [5 x [3 x i32]], [5 x [3 x i32]]* %a, i32 0, i32 1
  %arr_index4 = getelementptr [3 x i32], [3 x i32]* %arr_index2, i32 0, i32 3
  %accessed_value = load i32, i32* %arr_index4, align 4
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %accessed_value)
  ret void
}

declare i32 @printf(i8*, ...)
