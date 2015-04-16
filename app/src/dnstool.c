#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "uio.h"
#include "edns.h"
#include "dnstool.h"

int main(int argc, char ** argv)
{
    int c, ret = 0;
    int action = 0;
    char *ip_str = NULL, *in = NULL, *out = NULL;
	edns_context_t ctx;
	FILE *f = NULL;

	edns_command_register();

#if 1
    if (open_uio())
        return -1;
#endif
	INIT_EDNS_CTX(ctx);
    opterr = 0;
    while ((c = getopt(argc, argv, "hads:f:o:")) != -1)
        switch (c)
        {
        case 'a':
            ctx.action = ACTION_ADD;
            break;
        case 's':
            ctx.action = ACTION_SEARCH;
            break;
        case 'd':
            ctx.action = ACTION_DEL;
            break;
        case 'f':
            if (optarg)
            {
	            f = fopen(optarg, "r");
				if (f == NULL)
		        {
		            kerror("invalid file %s\n", file);
		            goto out;
		        }
                ctx.input = f;
            }
            break;
		case 'o':			
            if (optarg)
            {
                f = fopen(optarg, "r");
				if (f == NULL)
		        {
		            kerror("invalid file %s\n", file);
		            goto out;
		        }
                ctx.input = f;
            }
			break;
		case 'h':
			break;
        case '?':
            break;
        default:
            
            ;
        }

	if (optind < argc)
	{
	    size_t str_len = 16 * (argc - optind);
		char *ip_str;
		ctx.ip = malloc(str_len);
		if (!ctx.ip)
			goto out;			

		while (optind < argc && str_len) 
		{
			strncat(ctx.ip,argv[optind],str_len);			
			str_len -= strlen(argv[optind]);
			if (optind != argc)
			{
				strncat(ctx.ip, "", str_len);
				str_len--;
			}
			optind++;
		}
	}
	edns_setting(&ctx);
out:
	DESTROY_EDNS_CTX(ctx);
#if 1
    close_uio();
#endif
    edns_command_unregister();
    return 0;
}
