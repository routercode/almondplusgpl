#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <unistd.h>

int main(int argc, char **argv) {
	char *str;
	signed char buf[3];
	int err;
	int fd;
	fd_set rfds;
	struct timeval tv;

	if (argc != 2) {
		printf("please input device node path\n");
		return -1;
	}

	str=argv[1];
	fd=open(str, O_RDONLY);
	printf("fd is %d\n", fd);
	if (fd == -1) {
		printf("cannot open %s\n", str);
		return -1;
	}

	while (1) {
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);

		tv.tv_sec = 10;
		tv.tv_usec = 0;

		err=select(fd+1, &rfds, NULL, NULL, &tv);
		if (err == -1) {
			printf("(%s) select() return error (%d)\n", str, err);
			close(fd);
			return -1;
		}

		if ((err & 0x8) == 0x8) {
			printf("(%s) usb mouse disconnect (%d)\n", str, err);
			close(fd);
			return -1;
		}

		if (FD_ISSET(fd, &rfds)) {
			memset(buf, 0 , sizeof(buf));
			err=read(fd, buf, sizeof(buf));
			if (err == -1) {
				printf("(%s) read() return error (%d)\n", str, err);
				close(fd);
				return -1;
			} else {
				if (buf[0] & 0x01)
					printf("(%s) left key pressed\n", str);
				if (buf[0] & 0x02)
					printf("(%s) right key pressed\n", str);
				if (buf[0] & 0x04)
					printf("(%s) middle key pressed\n", str);
				if (buf[1])
					printf("(%s) relative x move : %d\n", str, buf[1]);
				if (buf[2])
					printf("(%s) relative y move : %d\n", str, buf[2]);
			}
		} else {
			printf("(%s) no data available\n", str);
		}
	}

	return 1;
}
