#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include <linux/input.h>
/*
ev_key.code = KEY_1~KEY_6(1~6);
ev_key.type = EV_KEY
ev_key.value = up or down (0 or 1)
*/
int main(void)
{
    int fd;
    int key_value,i=0,count;
    struct input_event ev_key;
    fd = open("/dev/input/event1", 666);//check device node use:cat /proc/bus/input/devices
    if (fd < 0) 
    {
        perror("open device buttons");
        exit(1);
    }
    while(1)
    {
        count = read(fd,&ev_key,sizeof(struct input_event)); //block (wait input_event)
        if(EV_KEY==ev_key.type)
            printf("type:%d,code:%d,value:%d\n", ev_key.type,ev_key.code,ev_key.value);
        if(EV_SYN==ev_key.type)
            printf("syn event\n\n");

    }
    close(fd);
    return 0;
}

