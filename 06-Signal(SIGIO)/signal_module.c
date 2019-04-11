#include<linux/module.h>
#include<linux/miscdevice.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/mm.h>
#include<linux/sched.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/poll.h>
#include<linux/timer.h>
#include<asm/io.h>
//#include<asm/system.h>
#include<asm/uaccess.h>
#include<asm/atomic.h>
#include <asm/unistd.h>
#include <linux/slab.h>

struct sync_dev
{
struct cdev cdev;
struct timer_list timer;
struct fasync_struct *fasync_queue;
};

struct sync_dev *sync_devp;

void timer_function(unsigned long para)
{
    if(&sync_devp->fasync_queue)
    {
        kill_fasync(&sync_devp->fasync_queue,SIGIO,POLL_IN);
        printk("send sync!\n");
    }
    mod_timer(&sync_devp->timer,jiffies +HZ*2);
}


static int sync_open(struct inode *inode,struct file *filp)
{
    filp->private_data = sync_devp;
    printk("open\n");
    return 0;
}

static int sync_fasync(int fd,struct file *filp,int on)
{
    int retval;
    struct sync_dev *dev = filp->private_data;
    printk("fasync\n");
    retval = fasync_helper(fd,filp,on,&dev->fasync_queue);

    if(retval<0) 
        return retval;

    return 0;

}

int sync_release(struct inode *inode,struct file *filp)
{
    struct sync_dev *dev = filp->private_data;
    sync_fasync(-1,filp,0);
    return 0;
}


static const struct file_operations sync_fops =
{
    .owner = THIS_MODULE,
    .open = sync_open,
    .release = sync_release,
    .fasync = sync_fasync,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "signal_test",
	.fops = &sync_fops,
};


static int __init syncint_init(void)
{
    int result;
    result =misc_register(&misc);
    sync_devp = kmalloc(sizeof(struct sync_dev),GFP_KERNEL);
    if(!sync_devp)
        goto fail_malloc;
    memset(sync_devp, 0, sizeof(struct sync_dev));//need to initial
    init_timer(&sync_devp->timer);
    sync_devp->timer.expires = jiffies + HZ*2;
    sync_devp->timer.function = timer_function;
    add_timer(&sync_devp->timer);
    printk("\nsync_init");
    return 0;   

    fail_malloc:
        misc_deregister(&misc);
        return result;
}

static void __exit sync_exit(void)
{
    kfree(sync_devp);
    del_timer(&sync_devp->timer);
    misc_deregister(&misc);
    printk("\nsync_close");
}


MODULE_LICENSE("GPL");
module_init(syncint_init);
module_exit(sync_exit);

















