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

#define OK 1
#define FALSE 0

void toSleepMilliSec(int msec)
{
long timeout =(msec)*HZ/1000;

while(timeout>0)
{

timeout = schedule_timeout(timeout);
}

}

int w_databuffer[100];
int r_databuffer[100]={0};

static struct task_struct *kfileThread = NULL;
struct file *file = NULL;

static int fileThread(void *data)
{
    int cmd,count,i;
    mm_srgment_t old_fa=get_fs();
    loff_t posWrite,posRead;
    count=0;
    w_databuffer[0]=0x12345678;
    w_databuffer[1]=0xaaaaaaaa;
    w_databuffer[2]=0x55555555;

    filp = filp_open("/home/testfile",O_RDWR|O_CREAT,777);

    if(IS_ERR(filp))
    {
        printk("can't open file");
        return -1;
    }
    set_fs(KERNEL_DS);
    posWrite=0;
    podRead=0;

    while(!(kthread_should_stop()))
    {
        toSleepMilliSec(100);
        vfs_write(filp,(char *)w_databuffer,12,&posWrite);
        vfs_read(filp,(char *)r_databuffer,6,&posRead);
        for(i=0;i<10;i++)
            printk("read[%d],%x",posRead,r_databuffer[i]);
        vfs_read(filp,(char *)r_databuffer,6,&posRead);
        for(i=0;i<10;i++)
            printk("read[%d],%x",posRead,r_databuffer[i]);

        filp_close(filp,0);
        set_fs(old_fs);

    }
}

static int kfile_init(void)
{
    kfileThread = kthread_run(fileThread,"test","filethradf");
    return 0;
}

static void kfile_cleanup(void)
{
if(kfileThread)
{
printf("stop filethread \n");
kthread_stop(kfileThread);

}

module_init(kfile_init);
module_exit(kfile_cleanup);

}




















