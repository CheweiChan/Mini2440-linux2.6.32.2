#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/*
    ./i2c_test w address data  (w 1 7654321)
    ./i2c_test r address len   (r 1 7)
*/
int main(int argc,char **argv)
{
int fd,res,i;
char register_addr = 0x01;//要写入的地址
char wbuf[2];//写缓冲区
char rbuf[2];//读缓冲区
char *p_buf;
char RX_buffer[20]={0};
char TX_buffer[20]={0};
fd = open("/dev/at24c08", O_RDWR);
if (fd < 0)
{
perror("open error\n");
exit(1);
}
if(strcmp(argv[1],"w") == 0)
{   

RX_buffer[0]=atoi(argv[2]);// read address
p_buf=argv[3];

while(p_buf[i] != '\0')
{
	RX_buffer[i+1]=p_buf[i]-'0';
printf("%d ",RX_buffer[i]);
i++;
}
printf("%d ",RX_buffer[i]);
write(fd, &RX_buffer[0], i+1);
}
else
{	
register_addr =atoi(argv[2]);
wbuf[1] = atoi(argv[3]);//len
write(fd, &register_addr, 1);
read(fd, &TX_buffer[0], wbuf[1]);
i=0;
while(i<wbuf[1])
	printf("%d ",TX_buffer[i++]);
}

printf("\n");

return 0;
}

