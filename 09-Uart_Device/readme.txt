1.define struct of uart_driver variable and initial
2.use uart_register_driver() to register uart driver
3.initial struct of uart_port and struct of uart_ops function table
4.use uart_add_one_port() to add uart_port




linux-2.6.32.2\drivers\serial
s3c2400.c 中定義 s3c2400_serial_init()並且註冊 platform_driver_register()實現了uart_platform_driver
samsung.c 中定義 s3c24xx_serial_probe()並且定義了struct of s3c24xx_uart_port,struct of uart_driver variable
s3c24xx_serial_probe()中調用uart_add_one_port()註冊了uart device driver