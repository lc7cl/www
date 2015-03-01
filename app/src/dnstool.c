#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "uio.h"
#include "edns.h"
#include "dnstool.h"

int main(int argc, char ** argv)
{
    int c;
    int action = 0;
    char *ip_str = NULL;
    char *file = NULL;

    edns_command_register();

#if 1
    if (open_uio())
        return -1;
    if (edns_init())
        return -1;
#endif

    opterr = 0;
    while ((c = getopt(argc, argv, "ads:f:")) != -1)
        switch (c)
        {
        case 'a':
            action = ACTION_ADD;
            break;
        case 's':
            action = ACTION_SEARCH;
            break;
        case 'd':
            action = ACTION_DEL;
            break;
        case 'f':
            if (optarg)
            {
                file = strdup(optarg);
            }
            printf("%s\n", optarg);
            break;
        case '?':
            break;
        default:
            
            ;
        }
    
    if (file)
    {
#define LINENUM 1024
        FILE *f;
        char buf[LINENUM], *p;

        memset(buf, 0, sizeof(buf));
        f = fopen(file, "r");
        if (f == NULL)
            return -1;

        while (p = fgets(buf, sizeof(buf), f))
        {
            edns_setting(action, p, strlen(p));
        }
        fclose(f);    
    } 
    else
    {
        while (optind < argc) 
        {
            ip_str = argv[optind];
            edns_setting(action, ip_str, strlen(ip_str));
            optind++;
        }
    }
#if 1
    edns_close();
    close_uio();
#endif

    edns_command_unregister();

    return 0;
}
