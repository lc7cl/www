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
    
    if(open_uio())
        return -1;
    if (edns_init())
        return -1;

    opterr = 0;
    while ((c = getopt(argc, argv, "a:d:s:f:")) != -1)
        switch (c)
        {
        case 'a':
            action = ACTION_ADD;
            ip_str = malloc(128);
            snprintf(ip_str, 128, "%s", optarg);
            break;
        case 's':
            action = ACTION_SEARCH;
            ip_str = malloc(128);
            snprintf(ip_str, 128, "%s", optarg);
            break;
        case 'd':
            action = ACTION_DEL;
			ip_str = malloc(128);
            snprintf(ip_str, 128, "%s", optarg);
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
    
    } 
    else
    {
        edns_setting(action, ip_str, strlen(ip_str));
        if (ip_str)
            free(ip_str);
    }

    edns_close();
    close_uio();

    edns_command_unregister();

    return 0;
}
