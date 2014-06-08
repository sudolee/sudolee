#include "minihd.h"

static int __init mti_init(void)
{
//    printk("%s : Hello world, i'm %s, :)\n", __func__, MTI_MODULE_VERSION);
    say_hello();

    return 0;
}

static void __exit mti_exit(void)
{
    printk("%s : Goodbye, the cruel world, :(\n", __func__);
}

module_init(mti_init);
module_exit(mti_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matti Lee <desoxydate@gmail.com>");
