; ModuleID = 'I_module'
source_filename = "I_module"

%Point = type { i32, i32 }

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define %Point @upd_record(%Point %0) {
entry:
  %old_point = alloca %Point, align 8
  store %Point %0, %Point* %old_point, align 4
  %new_point = alloca %Point, align 8
  %x = getelementptr inbounds %Point, %Point* %old_point, i32 0, i32 0
  %accessed_value = load i32, i32* %x, align 4
  %y = getelementptr inbounds %Point, %Point* %old_point, i32 0, i32 1
  %accessed_value1 = load i32, i32* %y, align 4
  %addtmp = add i32 %accessed_value, %accessed_value1
  %x2 = getelementptr inbounds %Point, %Point* %new_point, i32 0, i32 0
  store i32 %addtmp, i32* %x2, align 4
  %new_point3 = load %Point, %Point* %new_point, align 4
  ret %Point %new_point3
}

define [3 x i32] @upd_arr([3 x i32] %0) {
entry:
  %old_arr = alloca [3 x i32], align 4
  store [3 x i32] %0, [3 x i32]* %old_arr, align 4
  %new_arr = alloca [3 x i32], align 4
  %arr_index = getelementptr [3 x i32], [3 x i32]* %old_arr, i32 0, i32 2
  %accessed_value = load i32, i32* %arr_index, align 4
  %addtmp = add i32 %accessed_value, 1
  %arr_index1 = getelementptr [3 x i32], [3 x i32]* %new_arr, i32 0, i32 1
  store i32 %addtmp, i32* %arr_index1, align 4
  %new_arr2 = load [3 x i32], [3 x i32]* %new_arr, align 4
  ret [3 x i32] %new_arr2
}

define void @main() {
entry:
  %a = alloca [3 x i32], align 4
  %b = alloca [3 x i32], align 4
  %arr_index = getelementptr [3 x i32], [3 x i32]* %b, i32 0, i32 2
  store i32 7, i32* %arr_index, align 4
  %b1 = load [3 x i32], [3 x i32]* %b, align 4
  %call_upd_arr = call [3 x i32] @upd_arr([3 x i32] %b1)
  store [3 x i32] %call_upd_arr, [3 x i32]* %a, align 4
  %p = alloca %Point, align 8
  %x = getelementptr inbounds %Point, %Point* %p, i32 0, i32 0
  store i32 2, i32* %x, align 4
  %y = getelementptr inbounds %Point, %Point* %p, i32 0, i32 1
  store i32 3, i32* %y, align 4
  %p2 = load %Point, %Point* %p, align 4
  %call_upd_record = call %Point @upd_record(%Point %p2)
  store %Point %call_upd_record, %Point* %p, align 4
  %x3 = getelementptr inbounds %Point, %Point* %p, i32 0, i32 0
  %accessed_value = load i32, i32* %x3, align 4
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %accessed_value)
  ret void
}

declare i32 @printf(i8*, ...)
