/*
 * getimeofday.c
 *
 * Copyright (c) Cortina-Systems Limited 2011.  All rights reserved.
 *
 * Author: Joe Hsu <joe.hsu@cortina-systems.com>
 *
 *  Get minisecond time of current time and mask the seconds with 0xfffff.
 *
 * usage:
 *   getimeofday
 *
 * return masked time format:
 *   (tv.tv_sec & 0xfffff) * 1000 + (tv.tv_usec/1000)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	struct timeval	tv;

	gettimeofday(&tv, (void *)0);
	printf("%d%d", tv.tv_sec & 0x3ffff, (tv.tv_usec/1000));

	return 0;
}
