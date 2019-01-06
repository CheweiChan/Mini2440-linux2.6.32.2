#linux 
1.开始安装（建议大家新建一个文件夹），然后将文件复制到新建的文件夹中 
下載arm-linux-gcc 壓縮檔 (交叉工具鏈) 
下載linux-2.6.32.2 壓縮檔 (linux源代碼) 

 
2.安裝arm-linux-gcc 交叉工具鏈 
cd到下載目錄 输入命令： sudo tar xvzf arm-linux-gcc-4.5.1-v6-vfp-20120301.tgz 
安装完成后，输入ls命令，可以看到一个opt文件夹 
 
tar xvzf arm-linux-gcc-4.5.1-v6-vfp-20120301.tgz -C /abc (-C /abc 解壓縮到目錄abc) 
 
 
 
3.修改环境变量，把交叉编译器的路径加入到PATH： 
到opt/FriendlyARM/toolschain/4.5.1/bin输入pwd命令，查看当前路径的绝对路径 
接下來輸入cmd增加路徑 
export PATH=$PATH:/home/chewei/desktop_/opt/FriendlyARM/toolschain/4.4.3/bin 
 
永久增加路徑：修改.bashrc 
/home ~/.bashrc ：在.bashrc最下面增加export PATH=$PATH:/home/chewei/desktop_/opt/FriendlyARM/toolschain/4.4.3/bin 
运行脚本，让环境变量生效 
source ~/.bashrc 
 
也可以修改/etc/profile 來加入路徑 
在#Path manipulation下面加入路徑/home/chewei/desktop_/opt/FriendlyARM/toolschain/4.4.3/bin 
运行脚本，让环境变量生效 > source /etc/profile 
 
 
 
輸入cmd 確認路徑：echo $PATH 
輸入cmd 確認編譯器: arm-linux-gcc hello.c -o hello(編譯hello.c檔) 
輸入cmd 確認版本: arm-linux-readefl -a hello(查看編譯完後的hello檔) 
 
 
#cp config_mini2440_P43 .config 
#make mini2440_defconfig ;使用 Linux 官方自带的 mini2440 配置 
#make menuconfig 
#make zImage (arch/arm/boot目录下生成linux内核映象文件) 
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////// 

 
缺少 （build-essential） 编译库引起的编译问题。 
 
HOSTCC scripts/basic/fixdep 
scripts/basic/fixdep.c:106:23: fatal error: sys/types.h: No such file or directory 
compilation terminated. 
make[1]: *** [scripts/basic/fixdep] Error 1 
make: *** [scripts_basic] Error 2 
 
解决方法如下： 
apt-get install build-essential  
 
sudo apt-get install build-essential linux-headers-$(uname -r) 安裝linux drv 開發套件 

 
///////////////////////////////////////////////////////////////////////////////////////////////////////////// 

 
编译内核出现以下错误 
 
Can't use 'defined(@array)' (Maybe you should just omit the defined()?) at kernel/timeconst.pl line 373. 
 
/root/working/Hi3520D_SDK_V2.0.3.0/osdrv/kernel/linux-3.0.y/kernel/Makefile:140: recipe for target 'kernel/timeconst.h' failed 
 
make[1]: *** [kernel/timeconst.h] Error 255 
 
Makefile:945: recipe for target 'kernel' failed 
 
make: *** [kernel] Error 2 
 
解决办法： 
将kernel/timeconst.pl中第373行的defined()去掉只留下@val就可以了 
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////// 

 
yaffs2 文件系统 移植問題 
https://blog.csdn.net/baijinglei12/article/details/7686528 
 

///////////////////////////////////////////////////////////////////////////////////////////////////////////// 
Error 1 Makefile:454: recipe for target 'menuconfig' failed make: *** [menuconfig] Error 2  
解决办法： 
sudo apt-get install libncurses5-dev 
 
///////////////////////////////////////////////////////////////////////////////////////////////////////////// 
 
查看rs232 port 
cmd:dmesg | grep tty 
 

///////////////////////////////////////////////////////////////////////////////////////////////////////////// 
 
linux 按 minitool 沒反應 
安裝--> apt install libqtwebkit4 

 
///////////////////////////////////////////////////////////////////////////////////////////////////////////// 
执行make menuconfig可能看如这样的错误： 
 
*** Unable to find the ncurses libraries or the 
*** required header files. 
*** ‘make menuconfig’ requires the ncurses libraries. 
*** 
*** Install ncurses (ncurses-devel) and try again. 
 
解决办法如下： 
CentOS： 
yum install -y ncurses-devel 
 
Ubuntu： 
sudo apt-get install ncurses-dev 
///////////////////////////////////////////////////////////////////////////////////////////////////////////// 

 