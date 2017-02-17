/*
 *  main1.c - Create a random no from given input range
 */
#include <linux/init.h>		/*macros used to mark up functions e.g __init __exit*/
#include <linux/kernel.h>	/*contains macros, types, functions for kernel*/
#include <linux/module.h>	/*Core hear for loading LKMs into kernel*/
#include <linux/device.h>	/*header to support the kernel driver model*/
#include <linux/fs.h>		/*header to support linux file system support*/
#include <linux/time.h>		/*to access system time*/
#include <linux/types.h>	/*to hold major and minor no in kernel*/
#include "extra.h"		/*to include defined ioctl calls*/


#define DEVICE_NAME	"trng"		/*the device will appear at /dev/trng using this value*/
#define CLASS_NAME	"trng_c"	/*the device class -- this is a trng device driver*/

#define MY_LONG_NO (((1U << 31) - 1)-1)	/*macro to be used in algorithm*/

/*
 * general description
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TANYA");
MODULE_DESCRIPTION("RANDOM NO. GENERATOR version 1");

/*
 * global variable declaration
 */
static unsigned long	range_temp_min		= 0;
static unsigned long	range_temp_max		= 0;
static int		number_opens		= 0;	/*counter for no. of times device is open*/
static struct 		class* trng_class	= NULL;
static struct 		device* trng_device	= NULL;
static struct		timespec ts;
static struct		timeval tv;

/*
 * functions declaration
 */
static int	dev_open(struct inode *, struct file *);
static int	dev_release(struct inode *, struct file *);
static long	device_ioctl(struct file *, unsigned int , unsigned long );

/* Module Declarations */
static struct file_operations fops = {
	.owner 			= THIS_MODULE,
	.open			= dev_open,
	.release		= dev_release,
	.unlocked_ioctl		= device_ioctl,
};

/*
 * Initialize the module - Register the trng device
 */
static int __init trng_init(void)
{
	int ret_val;
	printk(KERN_INFO "****TRNG: initialing the true random no generator driver****\n");

	ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
	if(ret_val < 0)
	{
		printk(KERN_ALERT "TRNG FAILED to register a major no\n");
		return ret_val;
	}
	printk(KERN_INFO "TRNG: registerd correctly with major number:%d\n",MAJOR_NUM);

	/*creating class*/
	trng_class = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(trng_class))
	{
		unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
		printk(KERN_ALERT "FAILED to register device class\n\n");
		return PTR_ERR(trng_class);
	}
	printk(KERN_INFO "TRNG: device class registerd correctly\n\n");

	
	trng_device = device_create(trng_class, NULL, MKDEV(MAJOR_NUM, 0), NULL, DEVICE_NAME);
	if(IS_ERR(trng_device))
	{
		class_destroy(trng_class);
		unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
		printk(KERN_ALERT "FAILED to create device\n\n");
		return PTR_ERR(trng_class);
	}
	printk(KERN_INFO "TRNG:class device  registerd correctly\n\n");

	return 0;
}

/*
 * Cleanup - unregister the appropriate file from /proc
 */
static void __exit trng_exit(void)
{
	device_destroy(trng_class, MKDEV(MAJOR_NUM, 0));     
	class_unregister(trng_class);                          
	class_destroy(trng_class);                             
	unregister_chrdev(MAJOR_NUM, DEVICE_NAME);             
	printk(KERN_INFO "\nTRNG: I'm leaving :) :)\n");
}

/*
 * TRNG: device open() function
 */
static int dev_open(struct inode *inodep, struct file *filep)
{
	number_opens++;
	printk(KERN_INFO "\nTRNG: Device successfully open\n");
	printk(KERN_INFO "TRNG: device has been opened %d times\n",number_opens);
	return 0;
}

/*
 * TRNG: device close() function
 */
static int dev_release(struct inode *inodep, struct file *filep)
{
	printk(KERN_INFO "TRNG: Device successfully closed\n");
	return 0;
}

/*
 * TRNG: device ioctl() function
 */
static long device_ioctl(struct file *filep, unsigned int ioctl_num, unsigned long buf_ent)
{
  	unsigned long n, temp;		/*temporary variables used*/
	int digit = 0, power=1;				

	/*
	 * Switch according to the ioctl called
	 */
	switch (ioctl_num)
	{
		case IOCTL_GET_MAX:

		range_temp_max = buf_ent;
		printk(KERN_INFO "\n max value :%lu",range_temp_max);
		break;

		case IOCTL_GET_MIN:

		range_temp_min = buf_ent;
		printk(KERN_INFO "\n min value :%lu",range_temp_min);
		break;

		case IOCTL_PUT_RANDOM:
		
		do_gettimeofday(&tv);			/*to get time in microsecond*/
		ts = current_kernel_time();		/*to get time in nanosecond*/
		buf_ent = ts.tv_nsec;

		temp = buf_ent;
		
		printk(KERN_INFO "\n My long macro: %u\n\n",MY_LONG_NO);
		printk(KERN_INFO "\n random in microsecond:%ld ", tv.tv_usec );
		printk(KERN_INFO "\n random in nanosecond: %lu\n\n",temp);

		temp = temp * (tv.tv_usec);	/*random seed for the algorithm implementation*/
		
		/*LINEAR CONGRUENTIAL RANDOM NO GENERATION ALGORITHM*/
		temp = (temp * 1100355245 + 985632) & (MY_LONG_NO);
		temp = temp/10000;
		printk(KERN_INFO "\n random value from algorithm: %lu\n", temp);

		/*algorithm to fit random no in user range*/
		n = temp;		
		while(n !=0 )
		{
			n=n/10;
			digit++;
		}
		printk(KERN_INFO "\n digit value: %d\n",digit);

		while(digit !=0 )
		{
			power = 10*power;
			digit--;
		}
		printk(KERN_INFO "\n power: %d",power);
	
		temp = ((temp*(range_temp_max-range_temp_min))+(range_temp_min*power))/power;
		printk(KERN_INFO "\n random value: %lu\n",temp);
		return(temp);

		break;

		default:	printk(KERN_INFO "\n invalid case\n\n");		

	}
	return 0;

}


module_init(trng_init);
module_exit(trng_exit);
