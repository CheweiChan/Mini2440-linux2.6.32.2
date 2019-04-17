#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/*
./app w address data ex: w 1 7654321
./app r address len  ex: r 1 7
*/
int main(int argc,char **argv)
{
    int fd,i=0;
    char register_addr = 0;//start address
    char wbuf[2]={0};//command buffer
    char *p_buf;
    char Rx_buffer[256]={0};
    char Tx_buffer[256]={0};
    fd = open("/dev/at24c08", O_RDWR);
    if (fd < 0)
    {
        perror("open error\n");
        exit(1);
    }
    if(strcmp(argv[1],"w") == 0)
    {   
        Rx_buffer[0]=atoi(argv[2]);// idx 0= write start address
        p_buf=argv[3];
        while(p_buf[i] != '\0')
        {
        	Rx_buffer[i+1]=p_buf[i++]-'0';
        }
        write(fd, &Rx_buffer[0], i+1);
    }
    else
    {	
        register_addr =atoi(argv[2]);
        wbuf[1] = atoi(argv[3]);//read len
        write(fd, &register_addr, 1);//write start address
        read(fd, &Tx_buffer[0], wbuf[1]);
        while(i<wbuf[1])
        	printf("%d ",Tx_buffer[i++]);
        printf("\n");
    }
    close(fd);
    return 0;
}

