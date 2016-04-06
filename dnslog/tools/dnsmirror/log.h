#ifndef _LOG_H_
#define _LOG_H_

#ifdef __cplusplus
extern "C" {
#endif

void log_msg(const char* fmt, ...);
int open_log(const char*);
void close_log();

#ifdef __cplusplus
}
#endif

#endif
