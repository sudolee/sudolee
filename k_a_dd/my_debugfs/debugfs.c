#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/debugfs.h>
//#include <linux/slab.h>
#include <asm/uaccess.h>

#define LEN 32

struct dentry *debugfs_root;

u8 u8_a = 12;
char hello[LEN] = "hello...\n";
struct debugfs_blob_wrapper blob;

static int
debugfs_open(struct inode *inode, struct file *filp)
{
	filp->private_data = inode->i_private;
	return 0;
}

static ssize_t
debugfs_read(struct file *filp, char __user * buffer, size_t len, loff_t * pos)
{
	if (*pos >= LEN)
		return 0;
	if (*pos + len > LEN)
		len = LEN - *pos;

	if (copy_to_user(buffer, hello + *pos, len)) {
		printk(KERN_ALERT ">>> copy_to_user failed.\n");
		return -EFAULT;
	}

	*pos += len;
	return len;
}

static ssize_t
debugfs_write(struct file *filp, const char __user * buffer, size_t len,
	      loff_t * pos)
{
	if (*pos >= LEN)
		return 0;
	if (*pos + len > LEN)
		len = LEN - *pos;

	if (copy_from_user(hello + *pos, buffer, len)) {
		printk(KERN_ALERT ">>> copy_from_user failed.\n");
		return -EFAULT;
	}

	*pos += len;
	return len;
}

struct file_operations debugfs_fops = {
	.owner = THIS_MODULE,
	.open = debugfs_open,
	.read = debugfs_read,
	.write = debugfs_write,
};

static int __init
mydebugfs_init(void)
{
	struct dentry *r_a, *r_b;

	printk(KERN_ALERT ">>> mydebugfs_init...\n");

	debugfs_root = debugfs_create_dir("my_debugfs", NULL);
	if (IS_ERR(debugfs_root)) {
		printk(KERN_ALERT ">>> kernel without debugfs support...\n");
		return -ENOENT;
	}
	if (!debugfs_root) {
		printk(KERN_ALERT
		       ">>> mydebugfs: debugfs_create_dir failed.\n");
		return -ENOENT;
	}

	r_a = debugfs_create_u8("u8_a", S_IRUSR | S_IWUSR, debugfs_root, &u8_a);
	if (!r_a) {
		printk(KERN_ALERT ">>> mydebugfs: debugfs_create_u8 failed.\n");
		goto Fail;
	}

	blob.data = (void *) hello;
	blob.size = strlen(hello) + 1;
	r_b =
	    debugfs_create_blob("blob", S_IRUSR | S_IWUSR, debugfs_root, &blob);
	if (!r_b) {
		printk(KERN_ALERT
		       ">>> mydebugfs: debugfs_create_blob failed.\n");
		goto Fail;
	}

	return 0;

      Fail:
	debugfs_remove_recursive(debugfs_root);
	debugfs_root = NULL;
	return -ENOENT;
}

static void __exit
mydebugfs_exit(void)
{
	printk(KERN_ALERT ">>> mydebugfs_exit...\n");

	debugfs_remove_recursive(debugfs_root);
}

module_init(mydebugfs_init);
module_exit(mydebugfs_exit);
MODULE_LICENSE("GPL");
