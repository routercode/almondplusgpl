#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>

#include <linux/telephony.h>
#include <mach/cs752x_telephony.h>
#include <errno.h>

#define RANDOM_CID
/*
typedef enum {
	DIAG_ZSLIC_TALK,
	DIAG_ZSLIC_INIT,
	DIAG_ZSLIC_HOOKSTATE,
	DIAG_ZSLIC_TONE,
	DIAG_ZSLIC_CID,
	DIAG_ZSLIC_LOOPBACK,
	DIAG_ZSLIC_DEBUG,
	DIAG_ZSLIC_DMABUSY,
	DIAG_ZSLIC_CPUBUSY
} diag_cmd_func_t;
*/

#define TOTAL_LINES 2

#define ZPHONE_DIAG_MAJOR_STR	"0"
#define ZPHONE_DIAG_MINOR_STR	"0"

#define OFF_HOOK_STR		"Off Hook"
#define ON_HOOK_STR		"On Hook"

enum diagOption {
	ZP_TALK,
	ZP_INIT,
	ZP_HOOKSTATE,
	ZP_TONE,
	ZP_CID,
	ZP_LOOPBACK,
	ZP_DEBUG,
	ZP_DMABUSY,
	ZP_CPUBUSY,
	ZP_HELP
};

char optionStr[][16] = {
	"-talk",
	"-init",
	"-hookstat",
	"-tone",
	"-cid",
	"-loopback",
	"-debug",
	"-dmabusy",
	"-cpubusy",
	"--help"
};

char helpStr[] = {
	"Zarlink VE880 Module Diagnostic" ZPHONE_DIAG_MAJOR_STR"."ZPHONE_DIAG_MINOR_STR"\n" \
	"Usage: zphone-test [phone_idx] [OPTION] [ARGUMENT]...\n"
	/* Control Path Test */
	"    -talk <action>              <action:0 stop, 1 start> two phone talks\n" \
	"    -init                       SLIC init\n" \
	"    -hookstat                   get hook state\n" \
	"    -tone <tone#>               play tone <tone#>\n" \
	"    -cid <action>               <action:0 stop, 1 start> play cid\n" \
	"    -loopback <action>          <action:0 stop, 1 start> one phone loopback\n" \
	"    -debug                      debug message\n" \
	"    -dmabusy                    debug commands\n" \
	"    -cpubusy                    debug commands\n" \
	/* Data Path Test */
	"    --help                      Print Help (this message) and exit\n"
};


int main(int argc, char *argv[])
{
	int fd, i, index, option = -1;
	char zphoneDevice[16];
	char usageStr[] = "Try `zphone-test --help' for more information.";
	phone_diag_cmd_t diag_cmd;

	if (argc < 2) {
		printf("%s\n", usageStr);
		return -1;
	}

	if (strncmp(argv[1], optionStr[ZP_HELP], strlen(optionStr[ZP_HELP])) == 0) {
		printf("%s\n", helpStr);
		return -1;
	}

	for (i = 0; i < ZP_HELP; i++)
		if (strncmp(argv[2], optionStr[i], strlen(optionStr[i])) == 0) {
			option = i;
		}

	if (option == -1) {
		printf("%s\n", usageStr);
		return -1;
	}

	index = (int)strtol(argv[1], NULL, 0);
	if ((index < 0) || (index >= TOTAL_LINES)) {
		printf("<phone_idex> is 0 ~ %d\n", TOTAL_LINES - 1);
		return -1;
	}

	sprintf(zphoneDevice, "/dev/phone%d", index);
	if ((fd = open(zphoneDevice, O_RDONLY)) < 0) {
		printf("Open device %s failed !!!r\n", zphoneDevice);
		return -1;
	}

	diag_cmd.chan = index;
	switch (option) {
	case ZP_LOOPBACK:
		if (argc < 4) {
			printf("Usage: zphone-test [phone_idx] [OPTION] [ARGUMENT]");
		}

		diag_cmd.cmd = 5;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 0);
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	case ZP_TALK:
		if (argc < 4) {
			printf("Usage: zphone-test [phone_idx] [OPTION] [ARGUMENT]");
		}

		diag_cmd.cmd = 0;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 0);
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	case ZP_INIT:
		diag_cmd.cmd = 1;
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	case ZP_HOOKSTATE:
		diag_cmd.cmd = 2;
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		printf("hoot state: %s\n", diag_cmd.data ? "on-hook" : "off-hook");
		break;
	case ZP_TONE:
		diag_cmd.cmd = 3;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 0);
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	case ZP_CID:
		diag_cmd.cmd = 4;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 0);
#ifdef RANDOM_CID
		if (diag_cmd.data) {
			PHONE_CID cid;
			char *wday[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
			char *name[] = {"David", "Mary", "John", "Floria"};
			time_t timep;
			struct tm *p;
			int num;

			time(&timep);
			p = localtime(&timep);
			printf("%04d-%02d-%02d ", (1900+p->tm_year), (1+p->tm_mon), p->tm_mday);
			printf("%s %d:%d:%d\n", wday[p->tm_wday],p->tm_hour, p->tm_min, p->tm_sec);

			srand((int)time(0));
			num = (int)(999999999.0*rand()/(RAND_MAX+1.0));
			printf("num: %03d, name = %s\n", num, name[num % 4]);

			sprintf(cid.month, "%02d", (1+p->tm_mon));
			sprintf(cid.day, "%02d", p->tm_mday);
			sprintf(cid.hour, "%02d", p->tm_hour);
			sprintf(cid.min, "%02d", p->tm_min);
			sprintf(cid.number, "%010d", num);
			cid.numlen = strlen(cid.number);
			sprintf(cid.name, "%s", name[num % 4]);
			cid.namelen = strlen(cid.name);

			ioctl(fd, PHONE_RING_START, &cid);
		}
#endif
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	case ZP_DEBUG:
		diag_cmd.cmd = 6;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 16);
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	case ZP_DMABUSY:
		diag_cmd.cmd = 7;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 16);
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	case ZP_CPUBUSY:
		diag_cmd.cmd = 8;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 16);
		ioctl(fd, SSP_DIAG_FUNC_TEST, &diag_cmd);
		break;
	}

	close(fd);
	return 0;
}

