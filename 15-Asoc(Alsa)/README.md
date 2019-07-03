ASoC驱动有以下三部分组成：

 

（1）CODEC驱动：由内核源代码sound/soc/codecs/uda134x.c实现


（2）平台驱动：由内核源代码sound/soc/s3c24xx/s3c24xx-i2s.c实现CPU端的DAI驱动，由sound/soc/s3c24xx/s3c24xx_pcm.c实现CPU端的DMA驱动

（3）板驱动：由内核源代码sound/soc/s3c24xx/s3c24xx_uda134x.c实现，它将第一部分和第二部分进行绑定。


在以上三部分之上的是ASoC核心层，由内核源代码中的sound/soc/soc-core.c实现，查看其源代码发现它完全是一个传统的ALSA驱动。
