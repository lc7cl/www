#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

struct config {
    char device[512];
    char filter[1024];
    char log_file[512];
    char output_path[512];
}; 

extern struct config g_config;

int parse_cfg(const char* fname);
int chk_config();

#ifdef __cplusplus
}
#endif

#endif
