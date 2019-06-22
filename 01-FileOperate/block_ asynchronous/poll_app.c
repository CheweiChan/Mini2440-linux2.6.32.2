#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>
#define MEMDEV_SIZE 256

int main(void)
{
    int fd1,ret;
    fd_set fds;
    struct timespec timeout;
    fd1=open("/dev/polldev",O_RDWR);
    FD_ZERO(&fds);
    FD_SET(fd1,&fds);
    timeout.tv_sec=5;
    timeout.tv_nsec=0;
    printf("\ntest for select");

    while(1)
    {
    	printf("\nselect");
	ret = select(fd1+1,&fds,NULL,NULL,&timeout);
    	if(ret == -1)
            printf("\nselect error");
    	else if(ret == 0)
            printf("\n time out");
    	else
    	{
       	    FD_ZERO(&fds);
            FD_SET(fd1,&fds);
    	}
    }
}














