/********************************************************************/
/*File: pwr-test.c                                               */
/*                                                                  */
/*Descriptions:                                                     */
/*   To configure CIR and dump the input from the remote controller */
/*                                                                  */
/*Author:                                                           */
/*  Y.J. Wu 2010-06-09                                              */
/*                                                                  */
/*                                                                  */
/*History:                                                          */
/*   2010-06-09         init version                                */
/********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mach/cs_types.h>
#include <linux/apm_bios.h>
//#include "cs_types.h"
//#include "apm_bios.h"

enum diagOption {
	PWR_SHUT,
	PWR_WAIT,
	PWR_HALT,
	PWR_HELP
};

static char optionStr[][16] = {
	"-shut",
	"-wait",
	"-halt",
	"--help"
};

static char helpStr[] = {
	"Usage: pwr-test [OPTION] [ARGUMENT]...\n"
	/* Control Path Test */
	"    -shut <secs>       force shut down time(3 ~ 6 seconds)\n" \
	"    -wait              wait for power(button/cir) interrupt\n" \
	"    -halt              halt the system for force power down test\n" \
	/* Data Path Test */
	"    --help             Print Help (this message) and exit\n"
};

static char pwrDevice[] = "/dev/cs75xx-pwr";

int main(int argc, char **argv)
{
  	int fd, i, index, option = -1;
	struct pwc_ioctl_data ioctl_data;
	char usageStr[] = "Try `pwr-test --help' for more information.";

	if (argc < 2) {
		printf("%s\n", usageStr);
		return -1;
	}

	if (strncmp(argv[1], optionStr[PWR_HELP], strlen(optionStr[PWR_HELP])) == 0) {
		printf("%s\n", helpStr);
		return -1;
	}

	for (i = 0; i < PWR_HELP; i++)
		if (strncmp(argv[1], optionStr[i], strlen(optionStr[i])) == 0) {
			option = i;
		}

	if ((fd = open(pwrDevice, O_RDONLY)) < 0) {
		printf("Open device %s failed !!!\r\n", pwrDevice);
		return -1;
	}

	switch (option) {
	case PWR_SHUT:
		if (argc < 3) {
			printf("Usage: pwr-test -shut [secs]");
			return -1;
		}
		ioctl_data.data = atoi(argv[2]);
		ioctl(fd, PWC_SET_SHUT_TIME, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, PWC_GET_SHUT_TIME, &ioctl_data);
		printf("New shut down count: %d\r\n", ioctl_data.data);
		break;
	case PWR_WAIT:
		ioctl(fd, PWC_WAIT_BTN, &ioctl_data);
		switch (ioctl_data.action) {
		case POWEROFF:
			printf("Power Off event is occurred !!\r\n");
			system("poweroff");
			break;
		case RESTORE_DEFAULT:
			printf("Restore Default event is occurred !!\r\n");
			break;
		case SYSTEM_REBOOT:
			printf("System Reboot event is occurred !!\r\n");
			break;
		default:
			printf("Ambiguous event is occurred !!\r\n");
			break;
		}
		break;
	case PWR_HALT:
		ioctl(fd, PWC_SHUTDOWN, NULL);
	}

	close(fd);
	return 0;
}

