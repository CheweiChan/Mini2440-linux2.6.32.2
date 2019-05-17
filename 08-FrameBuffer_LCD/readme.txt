fbmem  (path:\linux-2.6.29\drivers\video)
s3c2410fb  (path:\linux-2.6.29\drivers\video)
devs (path:\linux-2.6.29\linux-2.6.29\arch\arm\plat-s3c24xx)

platform_device s3c_device_lcd 定義在devs裡面然後mach-smdk2440.c裡面會創建platform_Lcd_device
fbmem.c在模塊加載的時候會呼叫fbmem_init註冊一個chrdev(FB_MAJOR)包含了userspace所會呼叫到的fb_fops
s3c2410fb為lcd驅動,裡面包含了register_framebuffer(函數在fbmem.c裡),此函數會呼叫device_create創建chrdev(FB_MAJOR)的操作節點
                            s3c2410fb_map_video_memory會初始化framebuffer內存空間

C:\linux-2.6.29\arch\arm\mach-s3c2440\mach-smdk2440.c 增加:
platform_device *smdk2440_devices[] add (&s3c_device_lcd)
/* LCD driver info */

static struct s3c2410fb_display smdk2440_lcd_cfg __initdata = {

	.lcdcon5	= S3C2410_LCDCON5_FRM565 |
			  S3C2410_LCDCON5_INVVLINE |
			  S3C2410_LCDCON5_INVVFRAME |
			  S3C2410_LCDCON5_PWREN |
			  S3C2410_LCDCON5_HWSWP,

	.type		= S3C2410_LCDCON1_TFT,

	.width		= 240,
	.height		= 320,

	.pixclock	= 166667, /* HCLK 60 MHz, divisor 10 */
	.xres		= 240,
	.yres		= 320,
	.bpp		= 16,
	.left_margin	= 20,
	.right_margin	= 8,
	.hsync_len	= 4,
	.upper_margin	= 8,
	.lower_margin	= 7,
	.vsync_len	= 4,
};

static struct s3c2410fb_mach_info smdk2440_fb_info __initdata = {
	.displays	= &smdk2440_lcd_cfg,
	.num_displays	= 1,
	.default_display = 0,
	.lpcsel		= ((0xCE6) & ~7) | 1<<4,
};


static void __init smdk2440_machine_init(void)
{
	s3c24xx_fb_set_platdata(&smdk2440_fb_info);
	s3c_i2c0_set_platdata(NULL);

	platform_add_devices(smdk2440_devices, ARRAY_SIZE(smdk2440_devices));
	smdk_machine_init();
}
