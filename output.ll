; ModuleID = 'I_module'
source_filename = "I_module"

%Point = type { [5 x i32], i32, %Color }
%Color = type { i32, i32, i32 }

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define i32 @two(i32 %0) {
entry:
  %t = alloca i32, align 4
  store i32 %0, i32* %t, align 4
  %t1 = load i32, i32* %t, align 4
  %addtmp = add i32 2, %t1
  ret i32 %addtmp
}

define void @main() {
entry:
  %arr1 = alloca [3 x i32], align 4
  %arr2 = alloca [2 x [5 x i32]], align 4
  %arr_rec = alloca [3 x %Point], align 8
  %x = alloca i32, align 4
  store i32 3, i32* %x, align 4
  %x1 = load i32, i32* %x, align 4
  %arr_index = getelementptr [2 x [5 x i32]], [2 x [5 x i32]]* %arr2, i32 0, i32 1
  %arr_index2 = getelementptr [5 x i32], [5 x i32]* %arr_index, i32 0, i32 2
  store i32 %x1, i32* %arr_index2, align 4
  %x3 = load i32, i32* %x, align 4
  %arr_index4 = getelementptr [3 x i32], [3 x i32]* %arr1, i32 0, i32 0
  store i32 %x3, i32* %arr_index4, align 4
  %arr_index5 = getelementptr [2 x [5 x i32]], [2 x [5 x i32]]* %arr2, i32 0, i32 1
  %arr_index6 = getelementptr [5 x i32], [5 x i32]* %arr_index5, i32 0, i32 2
  %accessed_value = load i32, i32* %arr_index6, align 4
  %arr_index7 = getelementptr [3 x i32], [3 x i32]* %arr1, i32 0, i32 0
  %accessed_value8 = load i32, i32* %arr_index7, align 4
  %addtmp = add i32 %accessed_value, %accessed_value8
  store i32 %addtmp, i32* %x, align 4
  %arr_index9 = getelementptr [3 x %Point], [3 x %Point]* %arr_rec, i32 0, i32 2
  %y = getelementptr inbounds %Point, %Point* %arr_index9, i32 0, i32 1
  store i32 1, i32* %y, align 4
  %x10 = load i32, i32* %x, align 4
  %arr_index11 = getelementptr [3 x %Point], [3 x %Point]* %arr_rec, i32 0, i32 2
  %y12 = getelementptr inbounds %Point, %Point* %arr_index11, i32 0, i32 1
  %accessed_value13 = load i32, i32* %y12, align 4
  %subtmp = sub i32 %x10, %accessed_value13
  store i32 %subtmp, i32* %x, align 4
  %arr_index14 = getelementptr [3 x %Point], [3 x %Point]* %arr_rec, i32 0, i32 1
  %x15 = getelementptr inbounds %Point, %Point* %arr_index14, i32 0, i32 0
  %arr_index16 = getelementptr [5 x i32], [5 x i32]* %x15, i32 0, i32 0
  store i32 2, i32* %arr_index16, align 4
  %x17 = load i32, i32* %x, align 4
  %arr_index18 = getelementptr [3 x %Point], [3 x %Point]* %arr_rec, i32 0, i32 1
  %x19 = getelementptr inbounds %Point, %Point* %arr_index18, i32 0, i32 0
  %arr_index20 = getelementptr [5 x i32], [5 x i32]* %x19, i32 0, i32 0
  %accessed_value21 = load i32, i32* %arr_index20, align 4
  %addtmp22 = add i32 %x17, %accessed_value21
  store i32 %addtmp22, i32* %x, align 4
  %arr_index23 = getelementptr [3 x %Point], [3 x %Point]* %arr_rec, i32 0, i32 0
  %c = getelementptr inbounds %Point, %Point* %arr_index23, i32 0, i32 2
  %b = getelementptr inbounds %Color, %Color* %c, i32 0, i32 2
  store i32 10, i32* %b, align 4
  %x24 = load i32, i32* %x, align 4
  %arr_index25 = getelementptr [3 x %Point], [3 x %Point]* %arr_rec, i32 0, i32 0
  %c26 = getelementptr inbounds %Point, %Point* %arr_index25, i32 0, i32 2
  %b27 = getelementptr inbounds %Color, %Color* %c26, i32 0, i32 2
  %accessed_value28 = load i32, i32* %b27, align 4
  %multmp = mul i32 %x24, %accessed_value28
  store i32 %multmp, i32* %x, align 4
  %x29 = load i32, i32* %x, align 4
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %x29)
  ret void
}

declare i32 @printf(i8*, ...)
