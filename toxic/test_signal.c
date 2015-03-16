#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int main()
{
    sigset_t *set;


    set = (sigset_t*)malloc(sizeof(set));
    sigemptyset(set);
    sigaddset(set, SIGINT);
    //sigprocmask(SIG_SETMASK, set, NULL);

    while (1);

    return 0;
}
