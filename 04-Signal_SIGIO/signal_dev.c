#include<linux/module.h>
#include<linux/types.h>
#include<linux/fs.h>
#include<linux/mm.h>
#include<linux/sched.h>
#include<linux/init.h>
#include<linux/cdev.h>
#include<linux/poll.h>
#include<linux/timer.h>
#include<asm/io.h>
#include<asm/system.h>
#include<asm/uaccess.h>
#include<asm/atomic.h>

#define SYNC_MAJOR 200
typedef struct 
{
struct cdev cdev;
struct timer_list timer;
struct fasync_struct *fasync_queue;
}sync_dev;


static int sync_major =SYNC_MAJOR;
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


int sync_open(struct inode *inode,struct file *filp)
{
    filp->private_data = sync_devp;
    printk("open\n");
}

static int sync_fasync(int fd,struct file *filp,int on)
{
    int retval;
    struct sync_dev *dev = filp->private_data;

    retval = fasync_helper(fd,filp,on,&dev->fasync_queue);

    if(retval<0) 
        return retval;

    return 0;

}

int sync_release(struct inode *inode,struct file *filp)
{
    struct sync_dev *dev = filp->private_data;
    unregister_chrdev_region(MKDEV(sync_major,0),1);
    sync_fasync(-1,filp,0);
    return 0;
}


static const struct file_operations sync_fops =
{
    .owner = THIS_MODULE,
    .open = sync_open,
    .release = sync_release,
    .fasync = sync_fasync,
}



static int sync_init(void)
{
    int result,err,i;

    dev_t devno =MKDEV(sync_major,0);

    result = register_chrdev_region(devno,1,"syncdev");
    if(result < 0)
        return result;
    sync_devp = kmalloc(sizeof(struct sync_dev),GFP_KERNEL);
    if(!sync_devp)
        goto fail_malloc;

    memset(sync_devp,0,sizeof(struct sync_dev));
    cdev_init(&sync_devp->cdev,&sync_fops);
    sync_devp->cdev.owner = THIS_MODULE;
    sync_devp->cdev.ops = &sync_fops;
    err = cdev_add(&sync_devp->cdev,devno,1);
    if(err)
    {
        printk("module add error\n");
        goto fail_malloc;
    }
    init_timer(&sync_devp->timer);
    sync_devp->timer.expires = jiffies + HZ*2;
    sync_devp->timer.function = timer_function;
    add_timer(&sync_devp->timer);
    return 0;

    fail_malloc:
        unregister_chrdev_region(devno,1);
        return result;
}

static void sync_exit(void)
{
    cdev_del(sync_devp->cdev);
    kfree(sync_devp);
    del_timer(&sync_devp->timer);
    unregister_chrdev_region(MKDEV(sync_major,0),1);
}


MODULE_LICENSE("GPL");
MODULE_init(sync_init);
MODULE_exit(sync_exit);

















