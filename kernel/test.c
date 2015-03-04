#include <linux/module.h>
#include <linux/gfp.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

static char *ptr;

static int test_init(void)
{
    ptr = kmalloc(2^23, GFP_KERNEL);
    if (!ptr)
        printk("%s %d\n", __func__, __LINE__);
    return 0;
}

static void test_exit(void)
{
    if (ptr)
        kfree(ptr);
}

module_init(test_init);
module_exit(test_exit);
