; ModuleID = 'I_module'
source_filename = "I_module"

@0 = private unnamed_addr constant [4 x i8] c"%f\0A\00", align 1
@1 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1
@2 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

define void @main() {
entry:
  %socket = alloca i32, align 4
  store i32 8, i32* %socket, align 4
  %r = alloca double, align 8
  store double 1.400000e+00, double* %r, align 8
  %r1 = load double, double* %r, align 8
  %faddtmp = fadd double %r1, 1.500000e+00
  store double %faddtmp, double* %r, align 8
  %r2 = load double, double* %r, align 8
  %0 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @0, i32 0, i32 0), double %r2)
  %b = alloca i1, align 1
  store i1 true, i1* %b, align 1
  %b3 = load i1, i1* %b, align 1
  %andtmp = and i1 %b3, false
  store i1 %andtmp, i1* %b, align 1
  %b4 = load i1, i1* %b, align 1
  %1 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @1, i32 0, i32 0), i1 %b4)
  %socket5 = load i32, i32* %socket, align 4
  %2 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([4 x i8], [4 x i8]* @2, i32 0, i32 0), i32 %socket5)
  ret void
}

declare i32 @printf(i8*, ...)
