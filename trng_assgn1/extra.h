/*
 *  extra.h - the header file with the ioctl definitions.
 *
 *  The declarations here have to be in a header file, because
 *  they need to be known both to the kernel module
 *  (in main1.c & main2.c) and the process calling ioctl (user.c)
 */

#ifndef CHARDEV_H
#define CHARDEV_H

#include <linux/ioctl.h>

/*
 * The major device number. We can't rely on dynamic
 * registration any more, because ioctls need to know
 * it.
 */
#define MAJOR_NUM 100

/*
 * Get the upper limit of range
 */
#define IOCTL_GET_MAX _IOW(MAJOR_NUM, 0, unsigned long)
/*
 * The IOCTL is used to get input. It
 * receives a number from the user.
 */

/*
 * Get the lower limit of range
 */
#define IOCTL_GET_MIN _IOW(MAJOR_NUM, 1, unsigned long)
/*
 * The IOCTL is used to get input. It
 * receives a number from the user.
 */

/*
 * Return the random number
 */
#define IOCTL_PUT_RANDOM _IOR(MAJOR_NUM, 2, unsigned long)
/*
 * The IOCTL is used for output. It
 * returns generated random no to the user.
 */

/*
 * The name of the device file
 */

#define DEVICE_FILE_NAME "/dev/trng"
#endif
