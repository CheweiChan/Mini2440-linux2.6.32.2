#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/poll.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
#include <linux/gpio.h>
#include <asm/unistd.h>
#include <linux/sched.h>
#include <linux/wait.h>

#define MEM_CPY_NO_DMA  0

#define MEM_CPY_DMA     1

#define BUF_SIZE  (8*1024)

#define DMA0_BASE_ADDR  0x4B000000

#define DMA1_BASE_ADDR  0x4B000040

#define DMA2_BASE_ADDR  0x4B000080

#define DMA3_BASE_ADDR  0x4B0000C0

struct s3c_dma_regs {

unsigned long disrc;

unsigned long disrcc;

unsigned long didst;

unsigned long didstc;

unsigned long dcon;

unsigned long dstat;

unsigned long dcsrc;

unsigned long dcdst;

unsigned long dmasktrig;

};

static char *src;

static u32 src_phys;

static char *dst;

static u32 dst_phys;

static volatile struct s3c_dma_regs *dma_regs;

static DECLARE_WAIT_QUEUE_HEAD(dma_waitq);

/* 中断事件标志, 中断服务程序将它置1，ioctl将它清0 */

static volatile int ev_dma = 0;

static int s3c_dma_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	int i;

	memset(src, 0x77, BUF_SIZE);
	memset(dst, 0x88, BUF_SIZE);
	switch (cmd)
	{
		//非DMA模式
		case MEM_CPY_NO_DMA :
		{
			for (i = 0; i < BUF_SIZE; i++)
				dst[i] = src[i];  //CPU直接将源拷贝到目的
			if (memcmp(src, dst, BUF_SIZE) == 0)
				printk("MEM_CPY_NO_DMA OK\n");
			else
				printk("MEM_CPY_DMA ERROR\n");
			break;
		}

		//DMA模式
		case MEM_CPY_DMA :
		{
			ev_dma = 0;
			/* 把源,目的,长度告诉DMA */
			/* 关于下面寄存器的具体情况，我们在注释3里面来详细讲一下 */
			dma_regs->disrc      = src_phys;        /* 源的物理地址 */
			dma_regs->disrcc     = (0<<1) | (0<<0); /* 源位于AHB总线, 源地址递增 */
			dma_regs->didst      = dst_phys;        /* 目的的物理地址 */
			dma_regs->didstc     = (0<<2) | (0<<1) | (0<<0); /* 目的位于AHB总线, 目的地址递增 */
			dma_regs->dcon       = (1<<30)|(1<<29)|(0<<28)|(1<<27)|(0<<23)|(0<<20)|(BUF_SIZE<<0);  /* 使能中断,单个传输,软件触发, */
			/* 启动DMA */
			dma_regs->dmasktrig  = (1<<1) | (1<<0);
			/* 休眠 */
			wait_event_interruptible(dma_waitq, ev_dma);
			if (memcmp(src, dst, BUF_SIZE) == 0)
				printk("MEM_CPY_DMA OK\n");
			else
				printk("MEM_CPY_DMA ERROR\n");
			break;
		}
	}
	return 0;

}
static irqreturn_t s3c_dma_irq(int irq, void *devid)
{
	ev_dma = 1;
	wake_up_interruptible(&dma_waitq);   /* 唤醒休眠的进程 */
	printk("DMA ISR\n");
	return IRQ_HANDLED;
}


static int s3c_dma_open(struct inode *inode, struct file *filp) {
/* 这里注册一个中断，当DMA数据传输完毕之后会发生此中断 */
	if (request_irq(IRQ_DMA3, s3c_dma_irq, 0, "s3c_dma", (void *)0))
	{
		disable_irq(IRQ_DMA3);
		free_irq(IRQ_DMA3, (void *)0);
		printk("can't request_irq for DMA\n");
		return -EBUSY;
	}

    return 0;
}

static int s3c_dma_close(struct inode *inode, struct file *filp) {
    free_irq(IRQ_DMA3, (void *)0);
    return 0;
}




static struct file_operations misc_fops =
{
    .owner  = THIS_MODULE,
    .ioctl  = s3c_dma_ioctl,
    .open   = s3c_dma_open,
    .release  = s3c_dma_close,
};

static struct miscdevice DMA_TEST = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "dma_",
	.fops = &misc_fops,
};



static int s3c_dma_init(void)
{
	int ret;

	src = dma_alloc_writecombine(NULL, BUF_SIZE, &src_phys, GFP_KERNEL);//源
	if (NULL == src)
	{
		printk("can't alloc buffer for src\n");
		return -ENOMEM;
	}

	dst = dma_alloc_writecombine(NULL, BUF_SIZE, &dst_phys, GFP_KERNEL);//目的
	if (NULL == dst)
	{
		dma_free_writecombine(NULL, BUF_SIZE, src, src_phys);
		printk("can't alloc buffer for dst\n");
		return -ENOMEM;
	}


	ret = misc_register(&DMA_TEST);
   	if (ret < 0) 
	{
        printk("Failed to register character device\n");
        return ret;
    }
	dma_regs = ioremap(DMA3_BASE_ADDR, sizeof(struct s3c_dma_regs));//这边是将DMA控制寄存器映射到内核空间

	printk("Dma init success");
	return 0;

}

static void s3c_dma_exit(void)

{

	iounmap(dma_regs);

	misc_deregister(&DMA_TEST);

	dma_free_writecombine(NULL, BUF_SIZE, src, src_phys);

	dma_free_writecombine(NULL, BUF_SIZE, dst, dst_phys);

	printk("Dma exit");
}

module_init(s3c_dma_init);

module_exit(s3c_dma_exit);

MODULE_LICENSE("GPL");


