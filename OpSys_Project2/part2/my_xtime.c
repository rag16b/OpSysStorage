#include <linux/init.h>
#include <linux/module.h>
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/uaccess.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Proc to give the user the current kernel time");

#define ENTRY_NAME "timed"
#define PERMS 0644
#define PARENT NULL
static struct file_operations fops;
struct timespec ts;
struct timespec eTime;
struct timespec buffTs;

static char *message;
static int read_p;
int counter = 0;


int timed_proc_open(struct inode *sp_inode, struct file *sp_file)
{
	printk(KERN_INFO "proc called open\n");

	ts.tv_sec 	= current_kernel_time().tv_sec;
	ts.tv_nsec 	= current_kernel_time().tv_nsec;
	read_p = 1;
	message = kmalloc(sizeof(char)*100,__GFP_RECLAIM | __GFP_IO | __GFP_FS);

	if (message == NULL)
	{
		printk(KERN_WARNING "timed_proc_open");
		return -ENOMEM;
	}

	if(counter == 0)
	{
		sprintf(message, "Current Time: %lld.%.9ld\n", (long long)ts.tv_sec, ts.tv_nsec);
	}
	else
	{
		// subtract times
		eTime.tv_sec 	= ts.tv_sec - buffTs.tv_sec;
		eTime.tv_nsec 	= ts.tv_nsec - buffTs.tv_nsec;

		// negative nanosecond handler
		if(eTime.tv_nsec < 0)
		{
			eTime.tv_sec 	= eTime.tv_sec - 1;
			eTime.tv_nsec 	= eTime.tv_nsec + 1000000000;
		}
		sprintf(message, "Current Time: %lld.%.9ld\nElapsed Time: %lld.%.9ld\n",
		(long long)ts.tv_sec, ts.tv_nsec, (long long) eTime.tv_sec, eTime.tv_nsec);
	}

	buffTs.tv_sec 	= ts.tv_sec;
	buffTs.tv_nsec	= ts.tv_nsec;

	counter++;
	return 0;
}

ssize_t timed_proc_read(struct file *sp_file, char __user *buf, size_t size, loff_t *offset)
{
	int len = strlen(message);

	read_p = !read_p;
	if (read_p)
		return 0;

	printk(KERN_INFO "proc called read\n");
	copy_to_user(buf, message, len);
	return len;
}

int timed_proc_release(struct inode *sp_inode, struct file *sp_file)
{
	printk(KERN_INFO "prc called release\n");
	kfree(message);
	return 0;
}

static int call_kernel_time(void)
{
	printk(KERN_NOTICE "/proc/%s create\n",ENTRY_NAME);
	fops.open = timed_proc_open;
	fops.read = timed_proc_read;
	fops.release = timed_proc_release;

	if (!proc_create(ENTRY_NAME,PERMS,NULL,&fops))
	{
		printk("ERROR! proc_create\n");
		remove_proc_entry(ENTRY_NAME,NULL);
		return -ENOMEM;
	}
	return 0;
}

static void hello_exit(void)
{
	remove_proc_entry(ENTRY_NAME,NULL);
	printk(KERN_NOTICE "Removing /proc/%s.\n",ENTRY_NAME);
}

module_init(call_kernel_time);
module_exit(hello_exit);
