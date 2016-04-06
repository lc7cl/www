#include <stdio.h>
#include <stdlib.h>
#include "acl.h"
#include "region-allocator.h"

acl_t *acl_create()
{
    int ret = 0;
    acl_t *acl = NULL;
    isc_radix_tree_t *radix = NULL;
    region_type *region = NULL;

    region = region_create(malloc, free);
    if(!region) {
        ret = -1;
        goto done;
    }
    ret = isc_radix_create(region, &radix, RADIX_MAXBITS);
    if(ret < 0) {
        ret = -1;
        goto done;
    }
    radix->region = region;
    acl = (acl_t*)region_alloc(region, sizeof(acl_t));
    if(!acl) {
        ret = -1;
        goto done;
    }
    acl->radix = radix;
done:    
    if(ret < 0) {
        if(radix) {
            isc_radix_destroy(radix, NULL);
        }
        region_destroy(region);
    }
    return acl;
}

void acl_free(acl_t *acl)
{
    if(!acl) {
        return;
    }
    if(acl->radix) {
        isc_radix_destroy(acl->radix, NULL);
    }
    region_destroy(acl->region);
    return;
}

/**
 * @param ipÊÇÍøÂçÐòµÄ
 */
int acl_add(acl_t *acl, uint32_t ip, uint8_t depth, char* acl_name)
{
    isc_prefix_t pfx;
    isc_radix_node_t *node = NULL;
    isc_netaddr_t addr;
    int ret;

    if(!acl || depth > RADIX_MAXBITS) {
        return -1;
    }
    addr.family = AF_INET;
    *((uint32_t *)&addr.type.in) = ip;

    NETADDR_TO_PREFIX_T(&addr, pfx, depth);
    ret = isc_radix_insert(acl->radix, &node, NULL, &pfx);
    if(ret != 0) {
        //isc_refcount_destroy(&pfx.refcount);
        return -1;
    }
    node->data[ISC_IS6(family)] = 0;
    //node->view_idx = view_idx;
    node->acl_name = acl_name;
    return 0;
}

/**
 * no use, so not implemented
 */
int acl_delete(__unused acl_t *acl, __unused uint32_t ip, __unused uint8_t depth)
{
    return -1;
}

/**
 * for EDNS client subnet
 */
int acl_lookup2(acl_t *acl, uint32_t ip, uint32_t ecs, uint8_t ecslen, uint8_t *scope, char **acl_name)
{
    isc_netaddr_t addr;
    isc_prefix_t pfx;
    isc_radix_node_t *node = NULL;
    int ret;

    addr.family = AF_INET;

    if (ecs) {
        *((uint32_t *)&addr.type.in) = ecs;
        NETADDR_TO_PREFIX_T(&addr, pfx, ecslen);
        ret = isc_radix_search(acl->radix, &node, &pfx);
        if (ret == 0 && node != NULL) {
            if (scope != NULL)
                *scope = node->bit;
            *acl_name = node->acl_name;
            return 0;
        }
    } 
    
    *((uint32_t *)&addr.type.in) = ip;
    NETADDR_TO_PREFIX_T(&addr, pfx, 32);
    ret = isc_radix_search(acl->radix, &node, &pfx);
    if(ret == 0 && node != NULL) {
        *acl_name = node->acl_name;
        if (scope != NULL)
            *scope = node->bit;
        return 0;
    }

    return -1;
}

