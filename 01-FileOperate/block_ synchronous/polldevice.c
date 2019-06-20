#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/miscdevice.h>
struct poll_dev{
int flag;
wait_queue_head_t wq; 
struct timer_list timer;
};


#define DEVICE_NAME "polldev"

struct poll_dev dev;

void timer_func(unsigned long para)
{
    wake_up_interruptible(&dev.wq);
    dev.flag=1;
    mod_timer(&dev.timer,jiffies + HZ);
	printk("timer\n");
}

static int poll_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &dev;
	printk ("poll open\n");
	return 0;
}
static int poll_release(struct inode *inode, struct file *filp)
{
	printk ("poll release\n");
	return 0;
}

static ssize_t poll_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
      printk ("poll read\n");
	  return 0;
}

static ssize_t poll_write(struct file *filp, const char *buf, size_t size, loff_t *f_pos) {

     printk("poll write\n");
	return size;
}

static unsigned int poll_poll(struct file *filp,poll_table *wait)
{
	struct poll_dev *devpoll = filp->private_data;
	unsigned int mask=0;
	printk("poll\n");
	poll_wait(filp,&dev.wq,wait);
	mask |=POLLIN|POLLRDNORM;
	wait_event_interruptible(dev.wq,dev.flag);
	dev.flag=0;
	return mask;

}


static struct file_operations poll_fops =
{
    .owner   = THIS_MODULE,
    .read    = poll_read,
    .open    = poll_open,
    .write = poll_write,
    .release = poll_release,
    .poll = poll_poll,
};

static struct miscdevice poll_test = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME, //此名称将显示在/dev目录下面
	.fops = &poll_fops,
};

static int __init poll_init(void)
{
    int ret = misc_register(&poll_test);
    init_timer(&dev.timer);
    dev.timer.expires = jiffies+HZ;
    dev.timer.function = timer_func;
    add_timer(&dev.timer);
    dev.flag=0;
    init_waitqueue_head(&dev.wq);	
    printk ("poll test initialized\n");
	
    return ret;
}

static void __exit poll_exit(void)
{
    misc_deregister(&poll_test);
	del_timer(&dev.timer);
    printk("poll test unloaded\n");
}

module_init(poll_init);
module_exit(poll_exit);
MODULE_LICENSE("GPL");











