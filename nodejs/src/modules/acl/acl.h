#ifndef _ACL_H_
#define _ACL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "radix.h"
struct region;

typedef struct acl_s {
    isc_radix_tree_t *radix;
    struct region *region;
}acl_t;

#define __unused __attribute__((__unused__))

acl_t *acl_create();
void acl_free(acl_t *acl);
int acl_add(acl_t *acl, uint32_t ip, uint8_t depth, char* acl_name);
int acl_delete(acl_t *acl, uint32_t ip, uint8_t depth);
int acl_lookup2(acl_t *acl, uint32_t ip, uint32_t ecs, uint8_t ecslen, uint8_t *scope, char **acl_name);
static inline int acl_lookup(acl_t *acl, uint32_t ip, char **acl_name)
{
    return acl_lookup2(acl, ip, 0, 0, NULL, acl_name);
}

#ifdef __cplusplus
}
#endif

#endif

