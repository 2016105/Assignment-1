/*
 *  main2.c - Create a random no from given input range 
 */
#include <linux/init.h>		/*macros used to mark up functions e.g __init __exit*/
#include <linux/kernel.h>	/*contains macros, types, functions for kernel*/
#include <linux/module.h>	/*Core hear for loading LKMs into kernel*/
#include <linux/device.h>	/*header to support the kernel driver model*/
#include <linux/fs.h>		/*header to support linux file system support*/
#include <linux/random.h>	/*to access system entropy pool through function get_random_bytes*/
#include "extra.h"		/*to include defined ioctl calls*/


#define DEVICE_NAME	"trng"		/*the device will appear at /dev/trng using this value*/
#define CLASS_NAME	"trng_c"	/*the device class -- this is a trng device driver*/

/*
 *macros defined to implement park-miller-cart
 *algorithm to generate random no
 */
#define A 16807
#define M ((1U << 31) - 1)
#define Q 1227773L
#define R 2836

/*
 * general description
 */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("TANYA");
MODULE_DESCRIPTION("RANDOM NO. GENERATOR");

/*
 * global variable declaration
 */
static unsigned long	range_temp_min		= 0;
static unsigned long	range_temp_max		= 0;
static int		number_opens		= 0;
static struct		class* trng_class	= NULL;
static struct		device* trng_device	= NULL;

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
		printk(KERN_ALERT "TRNG:failed to create class or destroying the created class\n\n");
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
	printk(KERN_INFO "TRNG: I'm leaving :) :)\n");
}

/*
 * TRNG: device open() function
 */
static int dev_open(struct inode *inodep, struct file *filep)
{
	number_opens++;
	printk(KERN_INFO "TRNG: Device successfully open\n");
	printk(KERN_INFO "TRNG: device has been opened %d times",number_opens);
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
  	static unsigned long n, temp, lo ,hi;		/*temporary variables used*/
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
		
		get_random_bytes(&buf_ent, sizeof(int));	/*getting random no as random seed*/
		printk(KERN_INFO "\n random value: %lu\n\n",buf_ent);
		temp = buf_ent;
		
		/*PARK-MILLER-CARTA RANDOM NO GENERATION ALGORITHM*/
		lo = A * temp;
		hi = A * (temp >> 16);
		lo += (hi & 0x7FFF) << 16;
		if (lo > M)
		{
			lo &= M;
			++lo;
		}
		
		if (lo > M)
		{
			lo &= M;
			++lo;
		}
		
		temp = lo/10000;
		printk(KERN_INFO "\n temp value: %lu\n",temp);
		
		/*algorithm to fit random no in user range*/
		n = temp;		
		while(n != 0 )
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
