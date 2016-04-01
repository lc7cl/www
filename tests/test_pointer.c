#include <stdio.h>
#include <stdlib.h>

struct info {
    int a;
    char* b;
};

void test(char *p)
{
    char *ptr;

    ptr = malloc(32);
    printf("ptr:%p\n", ptr);
    *(p + 4) = ptr - (char*)0;
}


int main()
{
    struct info obj;

    printf("before test info.b:%p\n", (char*)&obj + 4);
    //*obj.b = 'a';
    test((char*)&obj);
    //*obj.b = 'a';
    printf("after test info.b:%p\n", (char*)&obj + 4);

    return 0;
}
