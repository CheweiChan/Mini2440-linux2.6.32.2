#include <linux/device.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/string.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/i2c/at24.h>

MODULE_LICENSE("GPL");

static struct platform_device *my_device;
struct i2c_client *client;
struct i2c_adapter *adapter;


struct i2c_board_info info = {
	I2C_BOARD_INFO("at24c08b", 0x50),
	};

static int __init my_device_init(void)
{
    int ret = 0;
    adapter = i2c_get_adapter(0);
    client=i2c_new_device(adapter,&info);
    i2c_put_adapter(adapter);
    my_device = platform_device_alloc("at24c08b", -1);  
    ret = platform_device_add(my_device);
    if (ret)
	platform_device_put(my_device);
	
    printk("iic device module init");
	return ret;	
}

static void my_device_exit(void)
{
    platform_device_unregister(my_device);
}

module_init(my_device_init);
module_exit(my_device_exit);
