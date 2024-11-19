; ModuleID = 'I_module'
source_filename = "I_module"

@0 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define void @main() {
entry:
  %c = alloca i32, align 4
  store i32 4, i32* %c, align 4
  %a = alloca i32, align 4
  store i32 1, i32* %a, align 4
  %c1 = load i32, i32* %c, align 4
  %gretmp = icmp sgt i32 %c1, 3
  br i1 %gretmp, label %then, label %else

then:                                             ; preds = %entry
  store i32 3, i32* %a, align 4
  br label %ifskip

else:                                             ; preds = %entry
  %a2 = load i32, i32* %a, align 4
  %addtmp = add i32 4, %a2
  store i32 %addtmp, i32* %a, align 4
  br label %ifskip

ifskip:                                           ; preds = %else, %then
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), i32 %addtmp)
  ret void
}

declare i32 @printf(i8*, ...)
