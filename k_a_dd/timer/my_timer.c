#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/timer.h>

static struct timer_list my_timer;

static void callback_func(unsigned long __data)
{
	printk(KERN_ALERT "%s(%ld)", (char *)__data, jiffies);
}

static int __init mytimer_init(void)
{
	u8 ret;
	char *data = "I'm the callee...";

	setup_timer(&my_timer, callback_func, (unsigned long)data);
	ret = mod_timer(&my_timer, msecs_to_jiffies(200));
	if(ret)
		printk(KERN_ERR "Oops, mod_timer err in func-> %s\n", __func__);

	return ret;
}

static void __exit mytimer_exit(void)
{
	printk(KERN_ALERT "my_timer module exit!\n");
	del_timer_sync(&my_timer);
}

module_init(mytimer_init);
module_exit(mytimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sudo.lee <desoxydate@gmail.com>");
