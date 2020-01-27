#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include<linux/sysfs.h> 
#include<linux/kobject.h> 

#define DEVICE_NAME "Driver1"
#define CLASS_NAME "Driver"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ayush Agarwal");
MODULE_DESCRIPTION("Demo character driver");
MODULE_VERSION("0.1");

static int majorNumber;
static char message[256] = {0};
static short size_of_message;
static int numberOpens = 0;
static struct class* charClass = NULL;
static struct device* charDevice = NULL;

static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *,char *,size_t ,loff_t *);
static ssize_t dev_write(struct file *,const char *,size_t,loff_t *);

struct kobject *kobj_ref;
static int sysFs;

//Initialise file_operations structure
static struct file_operations fops =
{
	.open = dev_open,
	.read = dev_read,
	.write = dev_write,
	.release = dev_release,
};

//syfs functions

static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf);
static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count);
struct kobj_attribute etx_attr = __ATTR(charDeviceDriverOpened_value, 0660, sysfs_show, sysfs_store);

//sysfs initialization function
static ssize_t sysfs_show(struct kobject *kobj, 
                struct kobj_attribute *attr, char *buf)
{
        printk(KERN_INFO "Sysfs - Read!!!\n");
        return sprintf(buf, "number of times CharacterDeviceDriver opened : %d\n", numberOpens);
}
 
static ssize_t sysfs_store(struct kobject *kobj, 
                struct kobj_attribute *attr,const char *buf, size_t count)
{
        printk(KERN_INFO "Sysfs - Write!!!\n");
        sscanf(buf,"%d",&numberOpens);
        return count;
}
 


//driver Initialisation function 
static int __init char_init(void)
{
	printk(KERN_INFO"Driver Loaded Successfully");
	majorNumber = register_chrdev(0,DEVICE_NAME,&fops);

	if (majorNumber<0)
	{
		printk(KERN_ALERT"Failed to register a major number");
	    return majorNumber;
	}

	printk(KERN_INFO"Regestered correctly with major number %d",majorNumber);
	charClass = class_create(THIS_MODULE , CLASS_NAME);

	if (IS_ERR(charClass))
	{
		unregister_chrdev(majorNumber,DEVICE_NAME);
		printk(KERN_ALERT"Failed to register device class\n");
		return PTR_ERR(charClass);
	}

	printk(KERN_INFO"Device class successfully register");
	charDevice = device_create(charClass,NULL,MKDEV(majorNumber,0),NULL,DEVICE_NAME);
	if (IS_ERR(charDevice))
	{
		class_destroy(charClass);
		unregister_chrdev(majorNumber,DEVICE_NAME);
		//printk(KERN_A ret=read(fd,receive,BUFFER_LENGTH));
		return PTR_ERR(charDevice);
	}

	printk(KERN_INFO"Device class created successfully");

	/*Creating a directory in /sys/kernel/ */
        kobj_ref = kobject_create_and_add("Driver1_sysfs",kernel_kobj);
	sysFs=sysfs_create_file(kobj_ref,&etx_attr.attr);
	/*Creating sysfs file for etx_value*/
	if(sysFs){
	    printk(KERN_INFO"Cannot create sysfs file......\n");
	    kobject_put(kobj_ref); 
            sysfs_remove_file(kernel_kobj, &etx_attr.attr);
	    return PTR_ERR(sysFs);
	}
	printk(KERN_INFO "Sysfs created sucessfully in sys/kernel!!!\n");
	printk(KERN_INFO "Device Driver Insert...Done!!!\n");
	return 0;
}

static void __exit char_exit(void)
{
	device_destroy(charClass,MKDEV(majorNumber,0));
	class_unregister((charClass));
	class_destroy(charClass);
	unregister_chrdev(majorNumber,DEVICE_NAME);

	kobject_put(kobj_ref); 
	sysfs_remove_file(kernel_kobj, &etx_attr.attr);
	printk(KERN_INFO"Goodbye from oue driver \n");
}

static int dev_open(struct inode *inodep,struct file *filep)
{
	numberOpens++;
	printk(KERN_INFO"Device has been opened %d times(s)\n",numberOpens);
	return 0;
}

static ssize_t dev_read(struct file *filep,char *buffer,size_t len,loff_t *offset)
{
	int error_count=0;

	error_count=copy_to_user(buffer,message,size_of_message);
	if (error_count==0)
	{
		printk(KERN_INFO"Send %d characters to the user",size_of_message);
		return(size_of_message=0);
	}
	else
	{
		printk(KERN_INFO"Failed to Send %d character to the user \n",error_count);
		return -EFAULT;
	}
}

static ssize_t dev_write(struct file *filep,const char *buffer,size_t len,loff_t *offset)
{
	//sprintf(message,"%s(%d letters)",buffer,len);
	copy_from_user(message,buffer,len);
	size_of_message = strlen(message);
	printk(KERN_INFO"Received %d characters from user \n",len);
	return len;
}


static int dev_release(struct inode *inodep,struct file *filep)
{
	printk(KERN_INFO"Device successfully closed\n");
	return 0;
}

module_init(char_init);
module_exit(char_exit);
