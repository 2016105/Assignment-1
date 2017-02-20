# Assignment-1

TRUE RANDOM NO GENERATION

Introduction:

The objective of this assignment is to create a character driver that generates true random number. This includes two kernel modules to implement two different algorithms to generate random number.

main1.c:

This kernel module works on "Park-Miller-Carta" algorithm to generate random number. It requires an initial value for its implementation which is called SEED VALUE (random in nature). This SEED value is calucalated using time values in microseconds and miliseconds.

main2.c:

This kernel module works on "Linear Congruential Generator" algorithm to generate random number. The SEED value for this algorithm is obtained from function get_random_bytes() which is defined in <linux/random.h>. Linux provides a special feature for the generation of random number. It creates an entropy pool for its kernel by gathering computer's environmental noise.  Sources of randomness from the environment includes inter-keyboard timings, inter-interrupt timings from interrupts, etc. The entropy pool created can be accessed in three ways:
a)	/dev/random	
b)	/dev/(u)random
c)	get_random_bytes()

extra.h:
This header file contains definition for IOCTL calls.

user.c:
This is a user program that helps to interact with the kernel module.

How to build:

Type the following commands in order:
$ make
$ sudo insmod ./main1.ko	or		$ sudo insmod ./main2.ko
$ gcc -g -o user user.c
$ sudo ./user
