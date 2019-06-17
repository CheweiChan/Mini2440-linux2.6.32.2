#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<sys/mman.h>

#define DEV_NAME "/dev/mmapled"

int main()
{
	int fd,k;
	void *start,*reg=NULL;
	fd = open(DEV_NAME,O_RDWR);
	if(fd<0)
	{
		printf("\n open device fail")
		retuen -1;
	}
	
	start = mmap(NULL,1024*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0x56000000);
	if(start = NULL)
	{
		printf("\nmmap error");
		return -1;
	}
	reg=start + 0x10;
	*(unsigned *)reg &= 0xfffc04ff;
	*(unsigned *)reg |=0x00015400;

	k=25;
	while(k--)
	{
		*(unsigned *)reg &= ~(0x1e0);
		sleep(1);
		*(unsigned *)reg |= 0x1e0;		
		sleep(1);	
	}
	munmap(start,4096);
	close(fd);
	return 0;
		
}
