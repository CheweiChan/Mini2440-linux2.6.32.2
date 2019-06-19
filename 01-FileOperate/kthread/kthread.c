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
#include <linux/kthread.h>

void toSleepMilliSec(int msec)
{
    long timeout =(msec)*HZ/1000;
    while(timeout>0)
        timeout = schedule_timeout(timeout);
}

char w_databuffer[20]="abcdefghijklmn";
char r_databuffer[12];

static struct task_struct *kfileThread = NULL;
struct file *filp = NULL;

static int fileThread(void *data)
{
    int count;
    mm_segment_t old_fs=get_fs();
    loff_t posWrite,posRead;
    count=0;


    filp = filp_open("/home/testfile",O_RDWR|O_CREAT,777);

    if(IS_ERR(filp))
    {
        printk("can't open file");
        return -1;
    }
    set_fs(KERNEL_DS);
    posWrite=0;
    posRead=0;

    while(!(kthread_should_stop()))
    {
        toSleepMilliSec(100);
		count++;
	if(count<10)
	{
		vfs_write(filp,(char *)w_databuffer,12,&posWrite);
		vfs_read(filp,(char *)r_databuffer,12,&posRead);
		printk("read[%d]=%s\n",(int)posRead,r_databuffer);
	}
	else if(count == 10)
	{ 
		filp_close(filp,0);
		set_fs(old_fs);
	}
	else
	{
		continue;
	}
     }

}

static int __init kfile_init(void)
{
    kfileThread = kthread_run(fileThread,"test","filethradf");
    printk("initthread=%x\n",kfileThread);
    return 0;
}

static  void __exit kfile_cleanup(void)
{
	if(kfileThread)
	{
		printk("stop filethread \n");
		kthread_stop(kfileThread);
	}
}

module_init(kfile_init);
module_exit(kfile_cleanup);
MODULE_LICENSE("GPL");


















