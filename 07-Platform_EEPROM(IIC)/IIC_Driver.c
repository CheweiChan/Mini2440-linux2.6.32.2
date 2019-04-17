#include<linux/init.h>
#include<linux/module.h>
#include<linux/i2c.h>
#include<linux/fs.h>
#include<linux/device.h>
#include<linux/slab.h>
#include<asm/uaccess.h>
#define E2PROM_MAJOR 250//cat /proc/devices  to check major number
MODULE_LICENSE("GPL");
struct e2prom_device{
    struct i2c_client *at24c02_client;
    struct class *at24c02_class;
    struct device *at24c02_device;
};
struct e2prom_device *e2prom_dev;
struct i2c_device_id e2prom_table[]={
	[0]={
		.name		="at24c02b",
		.driver_data		=0,
	},
	[1]={
		.name		="at24c08b",
		.driver_data	=0,
	}
};

static int i2c_read_byte(char *buf,int count)
{
	int ret=0;
	struct i2c_msg msg;
	msg.addr	=e2prom_dev->at24c02_client->addr;
	msg.flags	=1;//read=1,write=0
	msg.len		=count;
	msg.buf		=buf;

	ret=i2c_transfer(e2prom_dev->at24c02_client->adapter,&msg,1);
	if(ret<0){
		printk("i2c transfer failed!\n");
		return	-EINVAL;
	}
	return ret;
}
static int i2c_write_byte(char *buf,int count)
{
	int ret=0;
	struct i2c_msg msg;
	msg.addr	=e2prom_dev->at24c02_client->addr;//device address 0x50
	msg.flags	=0;	//write
	msg.len		=count;
	msg.buf		=buf;

	ret=i2c_transfer(e2prom_dev->at24c02_client->adapter,&msg,1);
	if(ret<0){
		printk("i2c transfer failed!\n");
		return	-EINVAL;
	}
	return ret;
}
static int e2prom_open(struct inode *inode, struct file *file)
{
return 0;
}
static ssize_t e2prom_read(struct file *filep, char *buf, size_t size,loff_t *ppos)
{
	int ret = 0;
	char *tmp;
	tmp = kmalloc(size,GFP_KERNEL);
	if(tmp==NULL){
		printk("malloc failed!\n");
		return -ENOMEM;
	}
	ret = i2c_read_byte(tmp,size);
	if(ret<0){
		printk("read byte failed!\n");
		ret = -EINVAL;
		goto err0;
	}
	
	ret = copy_to_user(buf,tmp,size);
	if(ret){
		printk("copy data failed!\n");
		ret =-EINVAL;
		goto err0;
	}
	kfree(tmp);
	return size;
    err0:
    kfree(tmp);
	return ret;		
}
static ssize_t e2prom_write(struct file *filep, const char *buf, size_t size,loff_t *ppos)
{
	int ret = 0;
	char *tmp;
	tmp = kmalloc(size,GFP_KERNEL);
	if(tmp == NULL){
		printk("malloc failed!\n");
		return -ENOMEM;
		goto err0;
	}
    ret = copy_from_user(tmp,buf,size);
	if(ret){
		printk("copy data failed!\n");
		ret =-EFAULT;
		goto err0;
	}
	ret = i2c_write_byte(tmp,size);
	if(ret<0){
		printk("write byte failed!\n");
		ret = -EINVAL;
		goto err0;
	}
	
	kfree(tmp);
	return size;
	err0:
		kfree(tmp);
		return ret;
}
struct file_operations e2prom_fops = {
.owner=THIS_MODULE,
.open=e2prom_open,
.read=e2prom_read,
.write=e2prom_write,
};
static int e2prom_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
	int ret;
	printk("enter probe!\n");
	e2prom_dev = kmalloc(sizeof(struct e2prom_device),GFP_KERNEL);
	if(e2prom_dev == NULL){
		printk("malloc failed\n");
		return -ENOMEM;
	}
	memset(e2prom_dev,0,sizeof(struct e2prom_device));
	e2prom_dev->at24c02_client = client;
	/*create char device that provide user file operate interface*/
	ret = register_chrdev(E2PROM_MAJOR,"e2prom_module",&e2prom_fops);
	if(ret < 0)
	{
		printk("register major failded\n");
		ret =-EINVAL;
		goto err0;
	}
	/*create class*/
	e2prom_dev->at24c02_class	= class_create(THIS_MODULE,"e2prom_class");
	if(IS_ERR(e2prom_dev->at24c02_class)){
		printk("class create failed\n");
		ret =PTR_ERR(e2prom_dev->at24c02_client);
		goto err1;
	}	
	
	/*create device file*/
	e2prom_dev->at24c02_device=device_create(e2prom_dev->at24c02_class,NULL,MKDEV(E2PROM_MAJOR,0),NULL,"at24c08");
	if(IS_ERR(e2prom_dev->at24c02_device)){
		printk("class create failed\n");
		ret =PTR_ERR(e2prom_dev->at24c02_device);
		goto err1;
	}
	return 0;
	err1:
		unregister_chrdev(E2PROM_MAJOR,"e2prom_module");
	err0:
		kfree(e2prom_dev);
	return ret;
}
static int e2prom_remove(struct i2c_client *client)
{
	unregister_chrdev(E2PROM_MAJOR,"e2prom_module");
	device_destroy(e2prom_dev->at24c02_class, MKDEV(E2PROM_MAJOR,0));
	class_destroy(e2prom_dev->at24c02_class);
	kfree(e2prom_dev);
	return 0;
	
}
/*struct i2c_driver*/
static struct i2c_driver e2prom_driver={
	.probe		= e2prom_probe,
	.remove		= __devexit_p(e2prom_remove),
    .id_table   = e2prom_table,//Record which devices this driver serves
	.driver     =
    {
     .name	="at24c08b",
     .owner =THIS_MODULE,
	},
};
static int __init e2prom_init(void)
{
	/*register platform driver
	*add iic driver to iic bus list
	*seach id_table and match device name and drive name
	*/
	i2c_add_driver(&e2prom_driver);

    printk("\niic drive module init");
	return 0;
}
static void __exit e2prom_exit(void)
{
	i2c_del_driver(&e2prom_driver);
}
module_init(e2prom_init);
module_exit(e2prom_exit);