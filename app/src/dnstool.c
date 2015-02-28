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

#if 0
    if(open_uio())
        return -1;
    if (edns_init())
        return -1;
#endif

    opterr = 0;
    while ((c = getopt(argc, argv, "a:d:s:f:")) != -1)
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

    if (optind >= argc)
        return -1;

    ip_str = argv[optind];
    printf("%s\n", ip_str);
    
    if (file)
    {
    
    } 
    else
    {
        edns_setting(action, ip_str, strlen(ip_str));
        if (ip_str)
            free(ip_str);
    }
#if 0
    edns_close();
    close_uio();
#endif

    edns_command_unregister();

    return 0;
}
