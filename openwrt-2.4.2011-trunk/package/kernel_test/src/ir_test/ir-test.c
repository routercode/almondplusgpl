#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <event.h>

static int debug = 0;

void keypadhandler(int fd, short event, void *arg)
{
	struct event *ev = arg;
	struct input_event buff;
	read(fd, &buff, sizeof(struct input_event));

	if (buff.type == EV_KEY)
		printf("%s key:%d\n", buff.value ? "Press" : "Release", buff.code);
	else {
		if (debug)
			printf("type:%d, code:%d, value:%d\n", buff.type, buff.code, buff.value);
	}
}

int main (int argc, char * argv[])
{
	struct event ev;
	int fd;

	if (argv[1]) {
		fd = open(argv[1], O_RDWR);
		if (argv[2])
			debug = atoi(argv[2]);
	} else {
		fd = open("/dev/input/event0", O_RDWR);
	}

	if (fd < 0){
		puts("can not open device event0");
		return -1;
	}

	event_init();

	event_set(&ev, fd, EV_READ | EV_PERSIST, keypadhandler, &ev);
	event_add(&ev, NULL);

	event_dispatch();

	close(fd);

	return 0;
}
