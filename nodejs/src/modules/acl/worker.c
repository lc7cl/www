#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <malloc.h>
#include <errno.h>
#include <syslog.h>

#include "acl.h"
#include "worker.h"
#define KSDNS_ACL_ALLOC_SIZE 16777216 // 2^24

/* global vars */
FILE *acl_file_stream = NULL;
const char *acl_conf_file = "";
#define MAXBUF	1024
static int global_log_level = LOG_INFO;

#define log_msg(priority, format, args...) fprintf(stdout, format, ##args)

enum acl_cfg_read_state {
    acl_state_start = 1,
    acl_state_ip_mask = 2,
    acl_state_end = 3
} state;

static int check_acl_buf(char *string, int *type) {
    char *cp = NULL;
    char *start = NULL;
    char *end = NULL;
    char token[MAXBUF];
    int str_len = 0;
    int current_type = 0;
    if (!string)
        return 1;
    cp = string;

    /* Skip white spaces */
    while (isspace((int) *cp) && *cp != '\0')
        cp++;

    /* Return if there is only white spaces */
    if (*cp == '\0')
        return 1;

    /* Return if string begin with a comment */
    if (*cp == '!' || *cp == '#' || *cp == ';')
        return 1;
    while (1) {
        start = cp;
        while (*cp != '\0' && *cp != ';') {
            cp++;
        }
        str_len = cp - start;

        memcpy(token, start, str_len);
        *(token + str_len) = '\0';
        if ((start = strstr(token, "acl")) != NULL) {
            if ((end = strstr(token, "{")) != NULL) {
                current_type = 1;
        } else 
            return -1;
        } else if ((end = strstr(token, "}")) != NULL) {
            current_type = 3;
        } else if ((end = strstr(token, "/")) != NULL) {
            current_type = 2;
        }
        break;
    }
    memcpy(string, token, str_len+1);
    *type = current_type;
    return 0;        
}

static int read_acl_line(char *buf, int size, int *type) {
    int ch;
    int ret = 1;
    while(ret == 1 && ch != EOF) {
        int count = 0;
        memset(buf, 0, MAXBUF);
        while ((ch = fgetc(acl_file_stream)) != EOF && (int) ch != '\n'
               && (int) ch != '\r') {
            if (count < size)
                buf[count] = (int) ch;
            else
                break;
            count++;
        }
        ret = check_acl_buf(buf, type);
    }

    buf[MAXBUF-1] = 0;
    if (ret < 0)
        return ret;
    return (ch == EOF) ? 0 : 1;
}

/*static int check_acl_conf(void) {
    
    return 0;
}*/

int check_ip_net24(char *buf, unsigned int *ip_net) {
    unsigned int ip_addr = 0;
    char *token = NULL;
    int ret = 0;
    int ip_mask = 0;
    strtok(buf, "/");
    token = strtok(NULL, "/"); 
    ip_mask = atoi(token);
    if (ip_mask <= 0 || ip_mask > 32) {
        log_msg(LOG_ERR, "acl conf:mask err \"%s:%s\".\n", buf, token);
        return -1;
    }
    ret = inet_pton(AF_INET, buf, &ip_addr);
    if(ret <= 0) {
        log_msg(LOG_ERR, "acl conf:inet_pton err \"%s\".\n", buf);
        return -1;
    }
    if (ip_mask <= 24) {
        *ip_net = 0;
        return 1;
    }
    ip_addr = ip_addr>>8;
    *ip_net = ip_addr;
    return 1;
}

/* load acl conf   */
int load_acl_conf(const char* conf_file, int *acl_count, int *net24_conut) {
    int ret = 1;
    acl_file_stream = NULL;
    acl_file_stream = fopen(conf_file, "r");
    if(!acl_file_stream) {
        log_msg(LOG_ERR, "open acl cfg file \"%s\" failed: %s\n", 
                conf_file, strerror(errno));
        ret = -1;
        return -1;//goto out;
    }
    char buf[MAXBUF];
    int type = acl_state_end;
    int current_type = acl_state_end; 
    int line = 0;
    int iacl_count = 0;
    int inet24_count = 0;
    unsigned int ip_net = 0;
    /* malloc 2^24 array to flag net24 count */
    char *net24_flag = (char *)malloc(KSDNS_ACL_ALLOC_SIZE + 1);
    memset(net24_flag, 0, KSDNS_ACL_ALLOC_SIZE + 1);
    while(ret > 0) {
        line++;  
        ret = read_acl_line(buf, MAXBUF, &current_type);
        if (ret < 0) {
            goto out;
        }
        switch(current_type) {
            case acl_state_start: 
                if(type != acl_state_end) 
                    goto out; 
                break; 
            case acl_state_ip_mask: 
                if(type != acl_state_start && type != acl_state_ip_mask) 
                    goto out; 
                else {
                    ip_net = 0;
                    ret = check_ip_net24(buf, &ip_net);
                    if (ret < 0) {
                        goto out;
                    } else if (ret > 0 && ip_net > 0) {
                        net24_flag[ip_net] = 1;
                    }
                }
                break;
            case acl_state_end: 
                if(type != acl_state_ip_mask) {
                    //ret = -1;
                    goto out; 
                }
                else iacl_count++; 
                break;
            default:   
                break;
        }
        type = current_type;
    }
out:
    if (ret != 0 && acl_file_stream) {
        log_msg(LOG_ERR, "parser acl cfg file \"%s\" failed: %d\n", 
                conf_file, line);
        fclose(acl_file_stream);
        acl_file_stream = NULL;
    }
    unsigned int i = 0;
    for (i = 0; i < KSDNS_ACL_ALLOC_SIZE; i++) {
        if (net24_flag[i] != 0) { 
            inet24_count++;
        }
    }
    *acl_count = iacl_count;
    *net24_conut = inet24_count;
    if (acl_file_stream) {
        rewind(acl_file_stream);
    }
    free(net24_flag);
    net24_flag = NULL;
    return ret;
}

void close_acl_config(void) {
    
    if (acl_file_stream != NULL) {
        fclose(acl_file_stream);
        acl_file_stream = NULL;
    }
}

int get_acl_name(char *acl_name, int size) {
    int ret = 1;
    char buf[MAXBUF];
    char *token = NULL;
    int current_type = acl_state_end; 
    while(ret > 0 && current_type != acl_state_start) {
        ret = read_acl_line(buf, MAXBUF, &current_type);
    }
    if (current_type != acl_state_start) {
        return -1;
    } else {
        strtok(buf, "\""); 
        token = strtok(NULL, "\""); 
        strncpy(acl_name, token, size);
    }
    return 0;
}

int get_ip_mask(unsigned int *ip, int *mask) {
    int ret = 1;
    unsigned int ip_addr;
    int ip_mask = 0;
    char buf[MAXBUF];
    char *token = NULL;
    int current_type = 0; 
    while(ret > 0 && (current_type != acl_state_ip_mask \
            && current_type != acl_state_end)) {
        ret = read_acl_line(buf, MAXBUF, &current_type);
    }
    if (current_type == acl_state_ip_mask) {
        strtok(buf, "/");
        ret = inet_pton(AF_INET, buf, &ip_addr);
        if(ret <= 0) {
            log_msg(LOG_ERR, "acl conf:inet_pton err \"%s\".\n", buf);
            return -1;
        }
        token = strtok(NULL, "/"); 
        ip_mask = atoi(token);
        if (ip_mask <= 0 || ip_mask >32) {
            log_msg(LOG_ERR, "acl conf:mask err \"%s:%s\".\n", buf, token);
            return -1;
        }
        *ip = ip_addr;
        *mask = ip_mask;
        ret = 0;
    } else if (current_type == acl_state_end) {
        *ip = 0;
        *mask = 0;
        ret = 1;
    }
    return ret;
} 


static acl_t *g_acl;

int build_acl(const char *file)
{
	int acl_cnt, net24_cnt, ret = 0, k;
	char *acl_name;
	
    uint32_t ip = 0;
    int mask = 0;
	
	ret = load_acl_conf(file, &acl_cnt, &net24_cnt);
    if(ret < 0) {
        log_msg(LOG_ERR, "load acl conf error: %s", file);
        return -1;
    }

	g_acl = acl_create();
	if (g_acl == NULL) {
		log_msg(LOG_ERR, "acl create error");
		return -1;
	}
	for(k = 0; k < acl_cnt; k++) {
		acl_name = (char*)malloc(128);
		if (acl_name == NULL) {
			log_msg(LOG_ERR, "cannot  alloc acl_name");
			return -1;
		}
			
		ret = get_acl_name(acl_name, 128 - 1);
		if(ret < 0) {
			log_msg(LOG_ERR, "get_acl_name error");
			return -1;
		}
		while(get_ip_mask(&ip, &mask) == 0) {
			ret = acl_add(g_acl, ip, (uint8_t)mask, acl_name);
			if(ret < 0) {
				log_msg(LOG_ERR, "acl_add error");
				return -1;
			}
		}
	}
	close_acl_config();
	return 0;
}

char* get_acl(uint32_t ip)
{
	char *acl_name;
	int ret;
	
	ret = acl_lookup(g_acl, ip, &acl_name);
	if (ret) {
		return NULL;
	}
	return acl_name;
}

void release_acl()
{
	
}
