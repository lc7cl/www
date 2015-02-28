#ifndef _EDNS_H
#define _EDNS_H

#define ACTION_ADD 1
#define ACTION_DEL 2
#define ACTION_SEARCH 3

extern int edns_init();
extern void edns_close();
extern int edns_setting(int action, char *ip, size_t length);
int edns_command_register();
int edns_command_unregister();

#endif
