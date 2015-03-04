#include <linux/kernel.h>
#include <linux/module.h>

static unsigned long foo;

static test_global_var(int circles)
{
    int i;
    int cpuid;

    cpuid = smp_processor_id()
    for (i = 0; i < circles; i++) {
        foo++;
    }

}


