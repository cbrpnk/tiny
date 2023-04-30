#include <stdio.h>

int main() {
    double a = 3.1;
    double b = 0;
    
    asm volatile (
        "fldl %1;"
        "fsin;"
        "fstpl %0;"
         : "=m"(b) : "m"(a)
    );
    
    printf("%f\n", b);
    
    // End
    asm volatile ("mov $0, %rax");
    asm volatile ("mov $60, %rdi");
    asm volatile ("syscall");
}
