#include <stdlib.h>
#include <stdio.h>
#include <execinfo.h>

void test_dump_bt(void)
{
    void *func[81920];
    int size;
    char **symb = NULL;

    size = backtrace(func, 81920);
    symb = backtrace_symbols(func, size);
    while (size > 0) {
        printf("%d: %s\n", size, symb[size - 1]);
        size--;
    }
    if (symb)
        free(symb);
}

int main()
{
    test_dump_bt();
    return 0;
}
