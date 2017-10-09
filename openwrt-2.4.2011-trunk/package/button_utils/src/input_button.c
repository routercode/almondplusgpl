#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>
#include <event.h>

#define BUTTON_CODE_WPS		238
#define BUTTON_CODE_RADIO	529	
void keypadhandler(int fd, short event, void *arg)
{
	struct event *ev = arg;
	struct input_event buff;

	read(fd, &buff, sizeof(struct input_event));

	if (buff.code == BUTTON_CODE_WPS && buff.value == 0)
	{
		//printf("%s: Enter WPS mode\r\n", __func__);
		system("/sbin/dowps.sh");
	}	
	if (buff.code == BUTTON_CODE_RADIO && buff.value == 0)
	{
		//printf("%s: Turn ON/OFF Wifi radio\r\n", __func__);
                system("/sbin/wifi_radio");
	}
}

int main (int argc, char * argv[])
{
	struct event ev;
	int fd;

	fd = open("/dev/input/event0", O_RDWR);
	if (fd < 0) {
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

