#include "minihd.h"

void say_hello(void)
{
    printk("%s : Hello world, i'm %s, :)\n", __func__, MTI_MODULE_VERSION);
}
