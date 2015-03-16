#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <errno.h>

static int test_exit;
static int once;

static void sighandler(int signo)
{
    static int count;
    printf("count:%d\n", ++count);
    if (once)
        test_exit = 1;
}

int main()
{
    struct sigaction sa;
    struct itimerval timer;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler =  sighandler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) == -1) {
        printf("%s %d error!\n", __func__, __LINE__);
        return 1;
    }

    memset(&timer, 0, sizeof(timer));
    //timer.it_interval.tv_sec = 10;
    timer.it_value.tv_sec = 1;
    //timer.it_value.tv_usec = 1000 * 1000;
    if (setitimer(ITIMER_REAL, &timer, NULL) == -1) {
        printf("%s %d error %s!\n", __func__, __LINE__, strerror(errno));
        return 1;
    }

    while (!test_exit){};

    return 0;
}
