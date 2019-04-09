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
int main(void)
{
 int fd;
 int key_value,i=0,count;
 struct input_event ev_key;
 fd = open("/dev/event1", 666);//®Úcheck /sys/class/unput dir num
 if (fd < 0) {
  perror("open device buttons");
  exit(1);
 }
 for (;;) {
  count = read(fd,&ev_key,sizeof(struct input_event));
  for(i=0; i<(int)count/sizeof(struct input_event); i++)
   if(EV_KEY==ev_key.type)
    printf("type:%d,code:%d,value:%d\n", ev_key.type,ev_key.code,ev_key.value);
   if(EV_SYN==ev_key.type)
    printf("syn event\n\n");
 }
 close(fd);
 return 0;
}

