#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/type.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <time.h>
#define MEMDEV_SIZE 256

int main(void)
{
    int fd1,fd2,maxfdp,ret;
    fd_set fds;
    struct timespec tv;
    char buf[MEMDEV_SIZE];
    FD_ZERO(&fds);

    FD_SET(fd1,&fds);
    tv.tv_sec=0;
    tv.tv_nsec=0;
    printf("\ntest for select");

    while(1)
    {

    ret = select(fd1+1,&fds,NULL,NULL,NULL);
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














