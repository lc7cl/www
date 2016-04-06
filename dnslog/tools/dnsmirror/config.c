#include <stdio.h>
#include <string.h>

#include "config.h"
#include "log.h"

struct config g_config;

#define LINE_MAX_LENGTH 2048
int parse_cfg(const char* fname)
{
    int ret;
    FILE* f;
    char line_buf[LINE_MAX_LENGTH], *p;
    int size;

    f = fopen(fname, "r");
    if (f == NULL)
        printf("cannot open config file %s\n", fname);

    while (fgets(line_buf, LINE_MAX_LENGTH, f) != NULL) {
        p = strtok(line_buf, "=\n");
        if (p == NULL) {
            printf("invalid config %s\n", line_buf);
            ret = -1;
            goto out;
        }
        if (strcmp(p, "device") == 0) {
            p = strtok(NULL, "=\n");
            if (p == NULL) {
                printf("%s needs value\n", line_buf);
                ret = -1;
                goto out;
            }
            snprintf(g_config.device, 512, "%s", p);         
        } else if (strcmp(p, "filter") == 0) {
            p = strtok(NULL, "=\n");
            if (p == NULL) {
                printf("%s needs value\n", line_buf);
                ret = -1;
                goto out;
            }
            snprintf(g_config.filter, 2048, "%s", p);      
        } else if (strcmp(p, "log_file") == 0) {
            p = strtok(NULL, "=\n");
            if (p == NULL) {
                printf("%s needs value\n", line_buf);
                ret = -1;
                goto out;
            }
            if (open_log(p) == -1) {
                printf("cannot open file %s\n", p);
                ret = -1;
                goto out;
            }
            snprintf(g_config.log_file, 512, "%s", p);      
        } else if (strcmp(p, "output_dir") == 0) {
            p = strtok(NULL, "=\n");
            if (p == NULL) {
                printf("%s needs value\n", line_buf);
                ret = -1;
                goto out;
            }
            snprintf(g_config.output_path, 512, "%s", p);      
        }

    }
    ret = 0;

out:
    if (f)
        fclose(f);
    return ret;
}

int chk_config()
{
    return 0;
}

