#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/gfp.h>
#include <linux/time.h>

typedef unsigned long *  compare_t;

struct object 
{
	int a;
    long b;
    long  c;
    char d;
    int e;
    char f;
    long g;
} __attribute__((__aligned__(BITS_PER_LONG/8)));


static void test_memcmp(struct object *obj, int count, struct object *o, int size)
{
    int i;
    struct object *objp;
    unsigned start = jiffies, end = jiffies;
    int n = 0;

    for (i = 0; i < count; i++) {
        objp = obj +  i;
        if (memcmp(objp, o, size)) 
        {
            n++;
        }
          //  printk("%s %d [%d]\n", __func__, __LINE__, i);
    }
    end = jiffies;
    printk("%s total : %u n:%d \n", __func__, (end-start), n);

}

static void test_wordcmp(struct object *obj, int count, struct object *o, int size)
{
    int i, j, n = 0;
    unsigned start, end;
    struct object *objp;
    compare_t p1, p2;

    if (size % (sizeof(unsigned long)))
        printk("********size\n");

    size = size / (sizeof(unsigned long));

    start = jiffies;
    for (i = 0; i < count; i++, j = 0) {
        objp = obj + i;
        p1 = (compare_t)obj;
        p2 = (compare_t)o;
        while(j < size) {
            if (*(p1 + j) != *(p2 + j))
                break;
            j++;
        }
        if (j != size) {n++;}
          //  printk("%s %d [%d]\n", __func__, __LINE__, i);

    }

    end = jiffies;
    printk("%s total : %u n:%d\n", __func__, (end-start), n);
}


static int test_init(void)
{
    char *mem = (char*)kmalloc(sizeof(struct object) * 1024 * 1024 * 10, GFP_KERNEL);
    struct object o;

    test_wordcmp((struct object*)mem, 800000, &o, sizeof(struct object));
    test_memcmp((struct object*)mem, 800000, &o, sizeof(struct object));

    kfree(mem);
    return 0;

}

static void test_exit(void)
{
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");


