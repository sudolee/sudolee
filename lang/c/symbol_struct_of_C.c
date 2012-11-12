#include <stdio.h>
#define THIS_MODULE "MY_MODULE"

struct file_operations {
	char *owner;
	void (*llseek) (void);
	void (*read) (void);
	void (*write) (void);
	void (*ioctl) (void);
	void (*open) (void);
	void (*release) (void);
};

void my_llseek(void)
{
	puts(">>>my_llseek...");
}

void my_read(void)
{
	puts(">>>my_read...");
}

void my_write(void)
{
	puts(">>>my_write...");
}

void my_ioctl(void)
{
	puts(">>>my_ioctl...");
}

void my_open(void)
{
	puts(">>>my_open...");
}

void my_release(void)
{
	puts(">>>my_release...");
}

struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.llseek = my_llseek,
	.write = my_write,
	.read = my_read,
	.ioctl = my_ioctl,
	.open = my_open,
	.release = my_release,
};

int main(int argc, char *argv[])
{
	puts(my_fops.owner);

	my_fops.open();
	my_fops.read();
	my_fops.write();
	my_fops.ioctl();
	my_fops.release();

	return 0;
}
