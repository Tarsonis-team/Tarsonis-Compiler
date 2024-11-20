; ModuleID = 'I_module'
source_filename = "I_module"

%Point = type { i32, i32 }

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define void @main() {
entry:
  %point = alloca %Point, align 8
  %x = getelementptr inbounds %Point, %Point* %point, i32 0, i32 0
  store i32 5, i32* %x, align 4
  %y = getelementptr inbounds %Point, %Point* %point, i32 0, i32 1
  store i32 3, i32* %y, align 4
  %x1 = getelementptr inbounds %Point, %Point* %point, i32 0, i32 0
  %accessed_value = load i32, i32* %x1, align 4
  %y2 = getelementptr inbounds %Point, %Point* %point, i32 0, i32 1
  %accessed_value3 = load i32, i32* %y2, align 4
  %addtmp = add i32 %accessed_value, %accessed_value3
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %addtmp)
  ret void
}

declare i32 @printf(i8*, ...)
