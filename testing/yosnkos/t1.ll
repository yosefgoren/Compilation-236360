declare i32 @printf(i8*, ...)
declare void @exit(i32)
@.int_specifier = constant [4 x i8] c"%d\0A\00"
@.str_specifier = constant [4 x i8] c"%s\0A\00"
define void @printi(i32) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.int_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i32 %0)
    ret void
}
define void @print(i8*) {
    %spec_ptr = getelementptr [4 x i8], [4 x i8]* @.str_specifier, i32 0, i32 0
    call i32 (i8*, ...) @printf(i8* %spec_ptr, i8* %0)
    ret void
}
define i32 @main(){
%sp = alloca [50 x i32]
br label %parse_label_3
parse_label_3:
br label %parse_label_10
br label %parse_label_6
parse_label_6:
%reg1 = getelementptr [50 x i32], [50 x i32]* %sp, i32 0, i32 0
store i32 0, i32* %reg1
br label %parse_label_10
parse_label_10:
%reg2 = add i32 0, 3
call void(i32) @printi(i32%reg2)
ret i32 0
}
