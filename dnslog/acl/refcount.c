#include "refcount.h"

void isc_refcount_init(isc_refcount_t *ref, unsigned int n)
{
    ref->refs = n;
}
