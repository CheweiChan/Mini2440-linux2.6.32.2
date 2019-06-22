相關檔案:

spidev_test.c-->spi test

spi_s3c24xx.c-->spi control driver

devs.c-->spi control device

spidev.c -->spi device (create user space fileoperate node)

![](https://github.com/CheweiChan/Mini2440-linux2.6.29/blob/master/IMG/spi_IOdefine.png)

SPI 子系統框架:
1.注册平台设备

2.注册平台驱动
  .probe()函数中注册描述spi控制器的spi_master
  
3.注册spi设备：spi_board_info

4.注册spi_driver
  .probe()函数中注册描述外设的设备结构体，如char设备
  
5.用户操作API实现。read()/write()

-------------------------------------------------------------------------------------
在Linux Source Code中修改arch/arm/mach-s3c2440/mach-mini2440.c文件，加入头文件：
```c
#include <linux/spi/spi.h>
#include <../mach-s3c2410/include/mach/spi.h>
```
然后加入如下代码：
```c
static struct spi_board_info s3c2410_spi0_board[] =
{  
        [0] = {  
                .modalias = "spidev",  //需與spi device driver名字一致
                .bus_num = 0,  
                .chip_select = 0,  
                .irq = IRQ_EINT9,  
                .max_speed_hz = 500 * 1000,  
                }  
};  
  
static struct s3c2410_spi_info s3c2410_spi0_platdata = {  
        .pin_cs = S3C2410_GPG(2),  
        .num_cs = 1,  
        .bus_num = 0,  
        .gpio_setup = s3c24xx_spi_gpiocfg_bus0_gpe11_12_13,  
};  
  
static struct spi_board_info s3c2410_spi1_board[] =  
{  
        [0] = {  
                .modalias = "spidev",  
                .bus_num = 1,  
                .chip_select = 0,  
                .irq = IRQ_EINT2,  
                .max_speed_hz = 500 * 1000,  
                }  
};  

  
static struct s3c2410_spi_info s3c2410_spi1_platdata = {  
        .pin_cs = S3C2410_GPG(3),  
        .num_cs = 1,  
        .bus_num = 1,  
        .gpio_setup = s3c24xx_spi_gpiocfg_bus1_gpg5_6_7,  
}; 
```  
这里需要了解驱动架构，其中移植过程中容易出问题的地方时S3C2410_GPG(2)和S3C2410_GPG(3)两处地方，网上一般给的源代码是 S3C2410_GPG2，这在2.6.29中可行，但是在2.6.32源代码中没有定义S3C2410_GPG2宏定义，要使用 S3C2410_GPG(2)宏定义。
在mini2440_devices[]平台数组中添加如下代码：
```c
&s3c_device_spi0,  
&s3c_device_spi1,  
```
最后在mini2440_machine_init函数中加入如下代码：
```c
s3c_device_spi0.dev.platform_data= &s3c2410_spi0_platdata;  
spi_register_board_info(s3c2410_spi0_board, ARRAY_SIZE(s3c2410_spi0_board));  
s3c_device_spi1.dev.platform_data= &s3c2410_spi1_platdata;  
spi_register_board_info(s3c2410_spi1_board, ARRAY_SIZE(s3c2410_spi1_board)); 
```
最后需要修改arch/arm/plat-s3c24xx/KConfig文件

找到如下代码段：
```shell
config S3C24XX_SPI_BUS0_GPE11_GPE12_GPE13  
        bool   
        help  
          SPI GPIO configuration code for BUS0 when connected to  
          GPE11, GPE12 and GPE13.  
  
config S3C24XX_SPI_BUS1_GPG5_GPG6_GPG7  
        bool   
        help  
          SPI GPIO configuration code for BUS 1 when connected to  
          GPG5, GPG6 and GPG7.  
修改为
config S3C24XX_SPI_BUS0_GPE11_GPE12_GPE13  
        bool "S3C24XX_SPI_BUS0_GPE11_GPE12_GPE13"  
        help  
          SPI GPIO configuration code for BUS0 when connected to  
          GPE11, GPE12 and GPE13.  
  
config S3C24XX_SPI_BUS1_GPG5_GPG6_GPG7  
        bool "S3C24XX_SPI_BUS1_GPG5_GPG6_GPG7"  
        help  
          SPI GPIO configuration code for BUS 1 when connected to  
          GPG5, GPG6 and GPG7. 
最后我们配置编译文件：
make menuconfig

system type-->[*]S3C24XX_SPI_BUS0_GPE11_GPE12_GPE13/[*]S3C24XX_SPI_BUS1_GPG5_GPG6_GPG7

Device driver -->SPI support-->[*]samsung s3c23xx series SPI/[*]user mode SPI device driver support
```

--------------------------------------------------------------------------------------
  SPI子系统从上到下分为：spi设备驱动层，核心层和master驱动层。其中master驱动抽象出spi控制器的相关操作，而spi设备驱动层抽象出了用户空间API。

  platform_device结构中描述了SPI控制器的相关资源，同时在板级信息中将会添加spi设备的相关信息。master驱动将以platform_driver形式体现出来，也就是说

在主控制器(master)和主控制器驱动将挂载到platform总线上。platform_driver的probe函数中将注册spi_master，同时将会获取在板级信息中添加的spi设备，将该

信息转换成spi_device，然后注册spi_device到spi总线上。spi_driver结构用于描述spi设备驱动，也将挂载到spi总线上。连同spi_driver一起注册的是字符设备，该

字符设备将提供5个API给用户空间。通过API，用户空间可以执行半双工读、半双工写和全双工读


當spi controller driver match spi controller device 調用probe function 由spi_bitbang_start调用一連串函數註冊device
随后调用了scan_boardinfo,该函数中，遍历spi_board_info，通过bus_num完成SPI设备和SPI控制器的匹配，匹配成功则开始建立spi_device设备，该过程通过调用spi_new_device实现。

![](https://github.com/CheweiChan/Mini2440-linux2.6.29/blob/master/IMG/spi_bitbang_star.png)

---------------------------------------------------------------------------------------------------
註冊spi device driver創建read/write/ioctrl

調用read function 流程:

![](https://github.com/CheweiChan/Mini2440-linux2.6.29/blob/master/IMG/SPI_READ.jpg)


調用write function 流程:

![](https://github.com/CheweiChan/Mini2440-linux2.6.29/blob/master/IMG/SPI_WRITE.jpg)


調用ioctrl function 流程:

![](https://github.com/CheweiChan/Mini2440-linux2.6.29/blob/master/IMG/SPI_ioctrl.jpg)
