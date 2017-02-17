
#include "extra.h"		/*to include defined ioctl calls*/
#include <stdio.h>		
#include <stdlib.h>		/*to include exit function*/
#include <fcntl.h>              /* open file */
#include <unistd.h>             /* exit the function if invalid*/
#include <sys/ioctl.h>          /* ioctl */

#define DEVICE_NAME	"trng"

static unsigned long d;

/*
 *function to get upper limit of the range from the user
 */
long ioctl_get_max(unsigned long file_desc, unsigned long max_me)
{
        d = ioctl(file_desc, IOCTL_GET_MAX, max_me);
        if (d < 0) 
	{
		printf("ioctl_get_max: function failed\n");
		exit(-1);
        }
	return 0;
}


/*
 *function to get lower limit of the range from the user
 */
long ioctl_get_min(unsigned long file_desc, unsigned long min_me)
{
        d = ioctl(file_desc, IOCTL_GET_MIN, min_me);
        if (d < 0) 
	{
        	printf("ioctl_get_min: function failed\n");
        	exit(-1);
        }
	return 0;
}

/*
 *function to get output from kernel to user
 */
long ioctl_put_output(unsigned long file_desc)
{
        d = ioctl(file_desc, IOCTL_PUT_RANDOM,0);
        if (d < 0) 
	{
		printf("ioctl_put_output function failed\n");
		exit(-1);
        }

        printf("Output of my TRUE RANDOM NO. GENERATOR = %lu \n",d);
	return 0;
}

/*
 *main function
 */
int main(void)
{
	int file_desc;
	unsigned long max, min;
	file_desc = open(DEVICE_FILE_NAME, 0);
	
	if (file_desc < 0) 
	{
	        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
	        exit(-1);
	}
	printf("\n ****TRUE RANDOM NUMBER GENERATOR: TRNG****\n");
	printf("\n Please your range");
	printf("\n Please enter upper limit: ");
	scanf("%lu",&max); 
	
	if (max < 0)
	{
		printf("*****invalid max no. entered*****\n");
		exit(-1);
	}
	else
	ioctl_get_max(file_desc,max);

	printf("\n Please enter lower limit: ");
	scanf("%lu",&min); 
	if (min < 0)
	{
		printf("*****invalid min no. entered*****\n");
		exit(-1);
	}
	else
	ioctl_get_min(file_desc,min);

	ioctl_put_output(file_desc);

    	close(file_desc);
    	return 0;
}
