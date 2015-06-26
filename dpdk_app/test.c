#include <stdlib.h>
#include <stdio.h>
#include <rte_cycles.h>

int main(int argc, char ** argv)
{
    int ret;
    
    ret = rte_eal_init(argc, argv);
    if (ret < 0)
        return -1;

    argc -= ret;
    argv += ret;


    rte_get_hpet_cycles();
    printf("hello\n");
    return 0;
}
