ASoC--ALSA System on Chip ，是建立在標準ALSA驅動層上，為了更好地支援嵌入式處理器和移動設備中的音訊Codec的一套軟體體系。目前已經被整合至核心的程式碼樹中：sound/soc。ASoC不能單獨存在，他只是建立在標準ALSA驅動上的一個它必須和標準的ALSA驅動框架相結合才能工作。

驅動分析:

![](https://github.com/CheweiChan/Mini2440-linux2.6.29/blob/master/IMG/ASOC_Flow.png)

snd_soc_device结构体是对ASoC设备的整体封装，包括了封装板用的snd_soc_machine(machine成员)[此结构体在linux 2.6.32.2中是snd_soc_card(card成员)]、封装ASoC CODEC设备用的snd_soc_codec_device(codec_devc成员)、封装ASoC平台设备用snd_soc_platform(platform成员)。
在CODEC驱动中提供给板驱动两个结构体：一个是关于codec 的DAI和对PCM配置的结构体snd_soc_dai另一个是封装ASoC CODEC设备用的snd_soc_codec_device结构体；
在ASoC平台设备驱动中也提供两个：一个是关于CPU相关的DAI操作设置另一个是对ASoC平台设备的封装snd_soc_platform结构体;在板驱动中，将CODEC驱动和平台驱动中的snd_soc_dai进行绑定，用snd_soc_dai_link结构体实现，同时把板的硬件的板流操作snd_soc_ops结构体的实现填充进去，snd_soc_card结构体对板驱动的封装，把snd_soc_dai_link和snd_soc_platform填充进去，snd_soc_device结构体对ASoC设备一个整体封装，把snd_soc_card和snd_soc_codec_device uda134x_platform_data进行填充。

 
ASoC驱动有以下三部分组成：

（1）CODEC驱动：由内核源代码sound/soc/codecs/uda134x.c实现實現audio controls/audio interface/capabilities/DAOM definition/codec IO functions
 
（2）平台驱动：由内核源代码sound/soc/s3c24xx/s3c24xx-i2s.c实现CPU端的DAI驱动，由sound/soc/s3c24xx/s3c24xx_pcm.c实现CPU端的DMA驱动

（3）板驱动：由内核源代码sound/soc/s3c24xx/s3c24xx_uda134x.c实现，它将第一部分和第二部分进行绑定。
     在板驱动的模块初始化函数中，会通过platform_device_add()注册一个名为“soc-audio”的platform设备，这是因为soc-core.c注册了一个名为“soc-audio”的platform驱动，因此，在板驱动中注册“soc-audio”设备会引起两者的匹配，从而引发一系列的初始化操作。

在以上三部分之上的是ASoC核心层，由内核源代码中的sound/soc/soc-core.c实现，查看其源代码发现它完全是一个传统的ALSA驱动。
