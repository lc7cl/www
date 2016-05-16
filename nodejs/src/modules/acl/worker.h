#ifndef _WORKER_H_
#define _WORKER_H_

#ifdef __cplusplus
extern "C" {
#endif

int build_acl(const char *file);
char* get_acl(uint32_t ip);
void release_acl();

#ifdef __cplusplus
}
#endif

#endif
