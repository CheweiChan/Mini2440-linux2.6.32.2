#include<sys/types.h>
#include<sys/stat.h>
#include<stdio.h>
#include<fcntl.h>
#include<signal.h>
#include<unistd.h>

int fd,g_running;

void sync_handler(int num)
{
    printf("receive sync signal \n");
}

void stop_handler(int num)
{
    close(fd);
    g_running=0;
    printf("stop process \n");
}

int main(void)
{
    int oflags;
    g_running=1;
    signal(SIGINT,stop_handler);

    fd =open("/dev/sync_123",O_RDWR);
    if(fd>0) 
	printf("open ok \n");
    else 
	printf("open fail \n");

    signal(SIGIO,sync_handler);
    fcntl(fd,F_SETOWN,getpid());
    oflags = fcntl(fd,F_GETFL);
    fcntl(fd,F_SETFL,oflags|FASYNC);
    while(g_running)
    {
	printf("go\n");
        sleep(100);
    }
    
    return 0;
}













