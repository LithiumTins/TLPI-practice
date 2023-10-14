#include <setjmp.h>
#include <stdio.h>

jmp_buf buf;

void f1(void)
{
    printf("this is f1");
    longjmp(buf, 114514);
}

void f2(void)
{
    setjmp(buf);
    printf("this is f2");
}

int main(int argc, char *argv[])
{
    f2();
    f1();
    return 0;
}