#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/module.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/mm.h>
#include<linux/device.h>
#include<linux/miscdevice.h>

#define DEV_NAME "mmapled"

int mmapled_open(struct inode *inode,struce file *filp)
{
	return 0;
}

int mmapled_close(struct inode *inode,struct file *filp)
{
	return 0;
}

int mmapled_mmap(struct file *filp,struct vm_area_struct *vma)
{
	int ret;
	vma->vna_flag |=(VM_RESERVED+VM_IO);
	vma->vm_page_port = pgport_noncached(vma->vm_page_port);
	ret=remap_pfn_range(vma,vma->vm_start,vma->vm_pgoff,vma->vm_end-vma->vm_start,vma->vm_page_port);
	printk("in %s,vm_pgoff=%x,vm_end=%x,vm_start=%x,vm_page_port=%x",__func__,vma->vm_pgoff,vma->vm_end,vma->vm_start,vma->vm_page_port);
	return 0;
}

struct file_operations mmapled_fops =
{
	.owner = THIS_MODULE,
	.open = mmapled_open,
	.release = mmapled_close,
	.mmap = mmapled_mmap,
};

static struct miscdevice miscled =
{
		.minor = MISC_DYNAMIC_MINOR,
		.name = DEV_NAME,
		.fops = &mmapled_fops,
};


int mmapled_init(void)
{
	int ret;
	misc_register(&miseled);
	return 0;
}


int mmapled_exit(void)
{
	misc_deregister(&miscled);
}

module_init(mmapled_init);
module_exit(mmapled_exit);
MODULE_LICENSE("GPL");






