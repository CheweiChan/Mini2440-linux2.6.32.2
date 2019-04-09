/*mini-2440 input device test*/
#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/hardware.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>
#include <linux/device.h>
 
static struct input_dev *button_dev;
static irqreturn_t buttons_interrupt(int irq, void *dummy);
static int button_open(struct input_dev *dev);
static void button_close(struct input_dev *dev);
struct button_irq_desc {   //定义按键中断的结构体
    int irq;     //中断号 
    int pin;     //按键引脚
    int pin_setting;    // 按键引脚设置（设置为中断）
    int number;            //按键编号
    char *name;     //按键名称 
};
/*利用结构体struct button_irq_desc 定义6个按键的信息，把按键的资源进行了组织*/
static struct button_irq_desc button_irqs [] = {
    {IRQ_EINT8 , S3C2410_GPG0 ,  S3C2410_GPG0_EINT8  , 1, "KEY1"},
    {IRQ_EINT11, S3C2410_GPG3 ,  S3C2410_GPG3_EINT11 , 2, "KEY2"},
    {IRQ_EINT13, S3C2410_GPG5 ,  S3C2410_GPG5_EINT13 , 3, "KEY3"},
    {IRQ_EINT14, S3C2410_GPG6 ,  S3C2410_GPG6_EINT14 , 4, "KEY4"},
    {IRQ_EINT15, S3C2410_GPG7 ,  S3C2410_GPG7_EINT15 , 5, "KEY5"},
    {IRQ_EINT19, S3C2410_GPG11,  S3C2410_GPG11_EINT19, 6, "KEY6"},
};
/*定义按键open函数，也可以将open函数的内容放到init函数中去做*/
static int button_open(struct input_dev *dev)
{
    int i;
    int err = 0;
    
    /*依次设置六个按键对应的中断为双沿触发，即上升沿和下降沿都会触发中断*/
    set_irq_type(IRQ_EINT8, IRQ_TYPE_EDGE_BOTH);  
    set_irq_type(IRQ_EINT11, IRQ_TYPE_EDGE_BOTH);  
    set_irq_type(IRQ_EINT13, IRQ_TYPE_EDGE_BOTH);  
    set_irq_type(IRQ_EINT14, IRQ_TYPE_EDGE_BOTH);  
    set_irq_type(IRQ_EINT15, IRQ_TYPE_EDGE_BOTH);  
    set_irq_type(IRQ_EINT19, IRQ_TYPE_EDGE_BOTH);
    
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++)
    {
        if (button_irqs[i].irq < 0) 
            continue; 
 /*依次申请中断，并设置六个中断类型标识为IRQF_SAMPLE_RANDOM，
 IRQF_SAMPLE_RANDOM表示设备可以被看做是事件随机的发生源
 request_irq其它几个参数分别是中断号；中断处理函数；设备名；最后一个参数为dev_id,
 共享中断时使用，必须唯一，用每个结构体的起始地址来表示能够保证id的唯一性。
 */
        err = request_irq(button_irqs[i].irq, buttons_interrupt, IRQF_SAMPLE_RANDOM, 
                          button_irqs[i].name, (void *)&button_irqs[i]);
        if (err)
            break;
    }
    /*出错处理，禁止中断并释放中断号*/
    if (err) 
    {
         i--;
         for (; i >= 0; i--)
         {
             if (button_irqs[i].irq < 0) 
             {
                continue;
             }
             disable_irq(button_irqs[i].irq);
             free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
         }
            return -EBUSY;
    }
        return 0;
}



static void button_close(struct input_dev *dev)
{
    int i;
     
    /*禁止中断并释放中断号*/
    for (i = 0; i < sizeof(button_irqs)/sizeof(button_irqs[0]); i++)
    {
         if (button_irqs[i].irq < 0)
         {
             continue;
         }
        disable_irq(button_irqs[i].irq);
        free_irq(button_irqs[i].irq, (void *)&button_irqs[i]);
    }
}

static irqreturn_t buttons_interrupt(int irq, void *dummy)
{
 /*当中断发生时，上报六个按键的状态，本来按键按下时键值是0，由于本人想让按键按下时显示键值为  1，故将从gpio端口获得的值取反。注意：六个按键都需要上报，在这里将你所有支持的按键都 上报，将
第二个参数按键号和第三个参数键值（从具体的GPIO端口去取按键的值）关联了起来，从而让你
 开发板上的硬件（即按键）能够跟系统衔接起来；input_sync函数告知系统你已经上报完毕*/
     input_report_key(button_dev, KEY_ESC, !s3c2410_gpio_getpin(S3C2410_GPG0));
     input_report_key(button_dev, KEY_1, !s3c2410_gpio_getpin(S3C2410_GPG3));
     input_report_key(button_dev, KEY_2, !s3c2410_gpio_getpin(S3C2410_GPG5));
     input_report_key(button_dev, KEY_3, !s3c2410_gpio_getpin(S3C2410_GPG6));
     input_report_key(button_dev, KEY_4, !s3c2410_gpio_getpin(S3C2410_GPG7));
     input_report_key(button_dev, KEY_5, !s3c2410_gpio_getpin(S3C2410_GPG11));
     input_sync(button_dev);
     return IRQ_HANDLED;
}
static int __init button_init(void)
{
    int error;
    struct input_dev *input_dev; 
    printk("s3c2440 my_keyboard module start/n");     
     /*给input_dev这个结构体指针分配空间*/
     input_dev = input_allocate_device();  
     if(!input_dev)  
     {  
      printk(KERN_ERR "Unable to allocate the input device!!/n");  
      return -ENOMEM;  
     }  
     button_dev = input_dev;  
 
 /*设置按键的类型和所支持的键，EV_KEY为按键类型，KEY_ESC、KEY_1-5为所支持的键，由于我希望我的按键
 取1-6，从include/linux/input.h中查询相关的宏可以知道我选择支持的键为1-6，当然你也可以选择你自己喜
 欢的键值，如果你选择的不是1-6，应用程序取得的按键号ev_key.code也会根据你的设置而改变*/
     set_bit(EV_KEY, button_dev->evbit);
     set_bit(KEY_ESC, button_dev->keybit);
     set_bit(KEY_1, button_dev->keybit);
     set_bit(KEY_2, button_dev->keybit);
     set_bit(KEY_3, button_dev->keybit);
     set_bit(KEY_4, button_dev->keybit);
     set_bit(KEY_5, button_dev->keybit);
 
 /*填充设备的名字，设置此名字可以让用户查询该设备的用途，该名字将存入在input/event*目录下的name
 中，cat name将会显示你设置的名字*/
     button_dev->name = "buttons_yzx";
 /*设置我的input子系统的名字，加载模块后，/sys/class/input目录下将会有你所设置的名字*/
     button_dev->dev.init_name = "input_yzx";
 
 /*设置结构体中的回调函数*/
     button_dev->open = button_open;
     button_dev->close = button_close;
 
    printk("input device has allocated/n");
 
 /*注册设备*/
    error = input_register_device(button_dev);
    if (error)     
    {
         printk(KERN_ERR "button.c: Failed to register device\n");
         input_free_device(button_dev);
         
         return error;
    }
 
    printk("register device has success\n");
 
    return 0;
}
static void __exit button_exit(void)
{
        /*删除设备*/
    input_unregister_device(button_dev); 
}
module_init(button_init);
module_exit(button_exit);
MODULE_LICENSE("GPL");









































