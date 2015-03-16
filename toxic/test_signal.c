#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

void sighandle(int sno)
{
    printf("0\n");
}

int main()
{
#if 0
    sigset_t *set;


    set = (sigset_t*)malloc(sizeof(set));
    sigemptyset(set);
    sigaddset(set, SIGINT);
    //sigprocmask(SIG_SETMASK, set, NULL);

    while (1);
#endif

    int i;
    sigset_t set, empty;

    sigemptyset(&set);
    sigaddset(&set, SIGINT);
    signal(SIGINT, sighandle);

    for (;;) {
        if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
            printf("%s\n", strerror(errno));
            return -1;
        }

        for (i = 0; i < 5; i++) {
            //printf("*");
            write(1, "*", 1);
            sleep(1);
        }
        printf("\n");

        sigemptyset(&empty);
#if 0
        sigsuspend(&empty);
#else
        if (sigprocmask(SIG_UNBLOCK, &set, NULL) == -1) {
            printf("%s\n", strerror(errno));
            return -1;
        }
        pause();
#endif
    }

    return 0;
}
