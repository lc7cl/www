#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

int main()
{
    FILE *fp;
    char buf[MAX_LINE_LENGTH];

    fp = fopen("res/111", "r");
    if (fp == NULL) {
        printf("%s\n", strerror(errno));
        return -1;
    }

    while (1) {
        if (fgets(buf, MAX_LINE_LENGTH, fp)) {
            printf("%d content:%s\n", __LINE__, buf);
        } else {
            printf("%d %s\n", __LINE__, strerror(errno));
        }

        if (feof(fp)) {
            rewind(fp);
            sleep(1);
            if (fgets(buf, MAX_LINE_LENGTH, fp)) {
                printf("%d content:%s\n", __LINE__, buf);
            } else {
                printf("%d %s\n", __LINE__, strerror(errno));
                break;
            }
        }
        sleep(1);
    }


    fclose(fp);

    return 0;
}
