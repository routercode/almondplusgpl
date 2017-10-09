/*
 * GPIO user space helpers
 *
 * Copyright 2009 Analog Devices Inc.
 * Michael Hennerich (hennerich@blackfin.uclinux.org)
 *
 * Licensed under the GPL-2 or later
 */

/*
GPIO1_BIT13 : 1*32+13=45

echo 45 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio45/direction

on :
echo 0 > /sys/class/gpio/gpio45/value

off :
echo 1 > /sys/class/gpio/gpio45/value

echo 45 > /sys/class/gpio/unexport
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

#define GPIO_DIR_IN			0
#define GPIO_DIR_OUT			1

int gpio_export(unsigned gpio)
{
	int fd, len;
	char buf[11];

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);

	return 0;
}

int gpio_unexport(unsigned gpio)
{
	int fd, len;
	char buf[11];

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		perror("gpio/export");
		return fd;
	}

	len = snprintf(buf, sizeof(buf), "%d", gpio);
	write(fd, buf, len);
	close(fd);
	return 0;
}

int gpio_dir(unsigned gpio, unsigned dir)
{
	int fd, len;
	char buf[60];

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/direction", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/direction");
		return fd;
	}

	if (dir == GPIO_DIR_OUT)
		write(fd, "out", 4);
	else
		write(fd, "in", 3);

	close(fd);
	return 0;
}

int gpio_dir_out(unsigned gpio)
{
	return gpio_dir(gpio, GPIO_DIR_OUT);
}

int gpio_dir_in(unsigned gpio)
{
	return gpio_dir(gpio, GPIO_DIR_IN);
}

int gpio_value(unsigned gpio, unsigned value)
{
	int fd, len;
	char buf[60];

	len = snprintf(buf, sizeof(buf), "/sys/class/gpio/gpio%d/value", gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		perror("gpio/value");
		return fd;
	}

	if (value)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);

	close(fd);
	return 0;
}

static const struct option long_options[] = {
	{ "on",		1, 0, 'n' },
	{ "off",	1, 0, 'f' },
	{ "gpio",	1, 0, 'g' },
	{ "range",	1, 0, 'r' },
	{ 0,		0, 0,  0  }
};

int main(int argc, char *argv[])
{
	int iteration;
	int option;

	int on_msec, on_usec;
	int off_msec, off_usec;
	int gpio;
	int range_msec;

	int temp;

	on_msec = 1000;
	on_usec = on_msec*1000;
	off_msec = 100;
	off_usec = on_msec*1000;
	gpio = 45;
	range_msec=1000;

	while (1) {
		option = getopt_long(argc, argv, "n:f:g:r:", long_options, NULL);
		if (option == -1)
			break;

		switch (option) {
		case 'n':
			on_msec = strtoul(optarg, NULL, 10);
			on_usec = on_msec * 1000;
			printf("on_msec=%d, on_usec=%d\n", on_msec, on_usec);
			break;

		case 'f':
			off_msec = strtoul(optarg, NULL, 10);
			off_usec = off_msec * 1000;
			printf("off_msec=%d, off_usec=%d\n", off_msec, off_usec);
			break;

		case 'g':
			gpio = strtoul(optarg, NULL, 10);
			printf("gpio=%d\n", gpio);
			break;

		case 'r':
			range_msec = strtoul(optarg, NULL, 10);
			printf("range_msec=%d\n", range_msec);
			break;
		}
	}

	gpio_export(gpio);
	gpio_dir_out(gpio);

	iteration=0;
	while (1) {
		if (iteration & 1) {
			// off
			temp = 1 + (int) (range_msec * (rand() / (RAND_MAX + 1.0)));
			off_usec=(off_msec+temp)*1000;
			printf("%d,X>%d\n", iteration, off_usec/1000);

			gpio_value(gpio, iteration & 1);
			usleep(off_usec);
		} else {
			// on
			temp = 1 + (int) (range_msec * (rand() / (RAND_MAX + 1.0)));
			on_usec=(on_msec+temp)*1000;
			printf("%d,V>%d\n", iteration, on_usec/1000);

			gpio_value(gpio, iteration & 1);
			usleep(on_usec);
		}

		iteration++;
	}

	gpio_unexport(gpio);
}
