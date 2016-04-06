#include <stdio.h>
#include <stdarg.h>

static FILE* log;

void log_msg(const char* fmt, ...) 
{
    char message[1024];
    va_list list;

    va_start(list, fmt);
    vsnprintf(message, 1024, fmt, list);
    fprintf(log, "%s", message);
    fflush(log);
    va_end(list);    
}

int open_log(const char* f)
{
    log = fopen(f, "w");
    if (log == NULL)
        return -1;
    return 0;
}

void close_log()
{
    if (log)
        fclose(log);
}
