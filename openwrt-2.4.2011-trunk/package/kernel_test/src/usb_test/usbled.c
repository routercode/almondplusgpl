#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>

// usbled -i 0 -1
// usbled -i 0 -0
// usbled --id 0 --start
// usbled --id 0 --stop

#define USBLED_START			0
#define USBLED_STOP			1

static const struct option long_options[] = {
	{ "id",		1, NULL,  'i' },
	{ "start",	0, NULL,  '1' },
	{ "stop",	0, NULL,  '0' },
	{ 0,		0, NULL,  0 }
};

int main(int argc, char *argv[])
{
	int option;

	int usbled_id;
	int usbled_start;

	int fd;
	int ret;

	usbled_id = -1;
	usbled_start = -1;

	while (1) {
		option = getopt_long(argc, argv, "i:10", long_options, NULL);
		if (option == -1)
			break;

		switch (option) {
		case 'i':
			usbled_id = strtoul(optarg, NULL, 10);
			printf("usbled_id=%d\n", usbled_id);
			break;

		case '1':
			usbled_start = 1;
			printf("usbled_start=%d\n", usbled_start);
			break;

		case '0':
			usbled_start = 0;
			printf("usbled_start=%d\n", usbled_start);
			break;
		}
	}

	if ((usbled_id == -1) &&
	    (usbled_start == -1))
	{
		printf("error command\n");
		return -1;
	}

	if (usbled_id == 0) {
		fd = open("/dev/usbled0", O_RDWR);
		if (fd < 0)
		{
			printf("/dev/usbled0 not found (%d)\n", errno);
			return -1;
		}

		if (usbled_start) {
			ret = ioctl(fd, USBLED_START);
			if (ret < 0) {
				printf("ioctl(USBLED_START) fail for /dev/usbled0 (%d)\n", errno);
				return -1;
			}
		} else {
			ret = ioctl(fd, USBLED_STOP);
			if (ret < 0) {
				printf("ioctl(USBLED_STOP) fail for /dev/usbled0 (%d)\n", errno);
				return -1;
			}
		}

		close(fd);
	} else {
		fd = open("/dev/usbled1", O_RDWR);
		if (fd < 0)
		{
			printf("/dev/usbled1 not found (%d)\n", errno);
			return -1;
		}

		if (usbled_start) {
			ret = ioctl(fd, USBLED_START);
			if (ret < 0) {
				printf("ioctl(USBLED_START) fail for /dev/usbled1 (%d)\n", errno);
				return -1;
			}
		} else {
			ret = ioctl(fd, USBLED_STOP);
			if (ret < 0) {
				printf("ioctl(USBLED_STOP) fail for /dev/usbled1 (%d)\n", errno);
				return -1;
			}
		}

		close(fd);
	}
}
