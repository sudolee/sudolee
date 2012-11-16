/*
 * Copyright (c) 2011 sudo.lee <desoxydate@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>

#define DEBUG
#define DEVICE_NAME "scdev"	/*simple char device */
#define MAX_BUFFER_LEN 32

struct simple_cdev {
	struct semaphore sema;
	struct cdev cdev;
	struct class *class;
	unsigned char *buffer;
};

static struct simple_cdev *__sdev;

static int simple_release(struct inode *inode, struct file *file)
{
	struct simple_cdev *sdev = file->private_data;

	kfree(sdev->buffer);
	file->private_data = NULL;
	return 0;
}

static ssize_t simple_write(struct file *file, const char __user * buf,
			    size_t count, loff_t * fpos)
{
	struct simple_cdev *sdev = file->private_data;
	ssize_t res;

	if (*fpos > MAX_BUFFER_LEN)
		*fpos = 0;
	if (count > MAX_BUFFER_LEN - *fpos)
		count = MAX_BUFFER_LEN - *fpos;

	down_interruptible(&sdev->sema);
	res = copy_from_user(sdev->buffer + *fpos, buf, count);
	up(&sdev->sema);
#ifdef DEBUG
	printk(KERN_INFO "copy from user[%s]", buf);
#endif

	return count - res;
}

static ssize_t simple_read(struct file *file, char __user * buf,
			   size_t count, loff_t * fpos)
{
	struct simple_cdev *sdev = file->private_data;
	ssize_t res;

	if (*fpos > MAX_BUFFER_LEN)
		*fpos = 0;
	if (count > MAX_BUFFER_LEN - *fpos)
		count = MAX_BUFFER_LEN - *fpos;

	down_interruptible(&sdev->sema);
	res = copy_to_user(buf, sdev->buffer + *fpos, count);
	up(&sdev->sema);

	return count - res;
}

static int simple_open(struct inode *inode, struct file *file)
{
	struct simple_cdev *sdev =
	    container_of(inode->i_cdev, struct simple_cdev, cdev);
	file->private_data = sdev;

	sdev->buffer = kmalloc(MAX_BUFFER_LEN, GFP_KERNEL);
	if (!sdev->buffer) {
		printk(KERN_ERR "can't alloc memory for buffer.");
		return -ENOMEM;
	}
	/* default string in buffer */
	sprintf(sdev->buffer, "%s", "hello world...");

	return 0;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = simple_open,
	.read = simple_read,
	.write = simple_write,
	.release = simple_release,
};

static int __init simple_init(void)
{
	int res;
	dev_t cdev_region;
	struct simple_cdev *simple_dev;
	struct device *device;

	printk(KERN_INFO "Simple char device driver init...");

	simple_dev = kmalloc(sizeof(struct simple_cdev), GFP_KERNEL);
	if (!simple_dev) {
		printk(KERN_ERR "can't alloc space to simple_cdev.");
		return -ENOMEM;
	}

	__sdev = simple_dev;
	//sema_init(&simple_dev->sema, 1);
	init_MUTEX(&simple_dev->sema);

	res = register_chrdev_region(MKDEV(251, 0), 1, DEVICE_NAME);
	if (!res) {
		cdev_region = MKDEV(251, 0);
	} else {
		res = alloc_chrdev_region(&cdev_region, 0, 1, DEVICE_NAME);
		if (res < 0) {
			printk(KERN_ERR "alloc char device region ERROR.");
			goto err_mem;
		}
	}

	simple_dev->class = class_create(THIS_MODULE, DEVICE_NAME);
	if (IS_ERR(simple_dev->class)) {
		res = PTR_ERR(simple_dev->class);
		printk(KERN_ERR "class create ERROR(%d).", res);
		goto err_region;
	}

	cdev_init(&simple_dev->cdev, &fops);

	res = cdev_add(&simple_dev->cdev, cdev_region, 1);
	if (res < 0) {
		printk(KERN_ERR "char device add ERROR.");
		goto err_class;
	}

	device =
	    device_create(simple_dev->class, NULL, cdev_region, NULL, "%s%d",
			  DEVICE_NAME, MINOR(cdev_region));
	if (IS_ERR(device)) {
		res = PTR_ERR(device);
		printk(KERN_ERR "device create ERROR(%d).", res);
		goto err_cdev;
	}

	return 0;
err_cdev:
	cdev_del(&simple_dev->cdev);
err_class:
	class_destroy(simple_dev->class);
err_region:
	unregister_chrdev_region(cdev_region, 1);
err_mem:
	kfree(simple_dev);
	return res;
}

static void __exit simple_exit(void)
{
	device_destroy(__sdev->class, __sdev->cdev.dev);
	cdev_del(&__sdev->cdev);
	class_destroy(__sdev->class);
	unregister_chrdev_region(__sdev->cdev.dev, 1);
	kfree(__sdev);
	printk(KERN_INFO "Simple char device driver exit.");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("sudolee <desoxydate@gmail.com>");
MODULE_DESCRIPTION("Driver of simple char device");
MODULE_LICENSE("GPL");
