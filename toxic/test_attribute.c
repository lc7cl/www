#include <stdio.h>

void __attribute__((constructor)) foo(void)
{
    printf("enter...\n");
}

void __attribute__((destructor)) bar(void)
{
    printf("exit...\n");
}

void __attribute__((section("TEST"))) baz(void)
{
    printf("%s %d\n", __func__, __LINE__);
}

void __attribute__((destructor)) qux(void)
{
    printf("%s %d\n", __func__, __LINE__);
}

int main()
{
    printf("...\n");
    baz();

    return 0;
}
