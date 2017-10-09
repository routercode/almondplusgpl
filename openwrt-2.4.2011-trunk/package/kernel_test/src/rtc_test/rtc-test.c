/*
 *      Real Time Clock Driver Test/Example Program
 *
 *      Compile with:
 *		     gcc -s -Wall -Wstrict-prototypes rtctest.c -o rtctest
 *
 *      Copyright (C) 1996, Paul Gortmaker.
 *
 *      Released under the GNU General Public License, version 2,
 *      included herein by reference.
 *
 */

#include <stdio.h>
#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>


/*
 * This expects the new RTC class driver framework, working with
 * clocks that will often not be clones of what the PC-AT had.
 * Use the command line to specify another RTC if you need one.
 */
static const char default_rtc[] = "/dev/rtc0";


int main(int argc, char **argv)
{
	int i, fd, retval, irqcount = 0;
	unsigned long tmp, data;
	struct rtc_time rtc_tm, rtc_tm1;
	const char *rtc = default_rtc;

	switch (argc) {
	case 2:
		rtc = argv[1];
		/* FALLTHROUGH */
	case 1:
		break;
	default:
		fprintf(stderr, "usage:  rtctest [rtcdev]\n");
		return 1;
	}

	fd = open(rtc, O_RDONLY);

	if (fd ==  -1) {
		perror(rtc);
		exit(errno);
	}

	fprintf(stderr, "\n\t\t\tRTC Driver Test.\n\n");


test_READ:
	/***************************/
	/* Write the RTC time/date */
	/***************************/
	rtc_tm.tm_year = 111;
	rtc_tm.tm_mon = 6;
	rtc_tm.tm_mday = 11;
	rtc_tm.tm_hour = 18;
	rtc_tm.tm_min = 50;
	rtc_tm.tm_sec = 11;
	fprintf(stderr, "\n\nSet RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);
	retval = ioctl(fd, RTC_SET_TIME, &rtc_tm);
	if (retval == -1) {
		perror("RTC_SET_TIME ioctl");
		exit(errno);
	}
		
	/**************************/
	/* Read the RTC time/date */
	/**************************/
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm);
	if (retval == -1) {
		perror("RTC_RD_TIME ioctl");
		exit(errno);
	}

	fprintf(stderr, "\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm.tm_mday, rtc_tm.tm_mon + 1, rtc_tm.tm_year + 1900,
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	printf("Wait 5 seconds......\n");
	sleep(5);
	
	retval = ioctl(fd, RTC_RD_TIME, &rtc_tm1);
	if (retval == -1) {
		perror("RTC_RD_TIME ioctl");
		exit(errno);
	}

	fprintf(stderr, "\n\nCurrent RTC date/time is %d-%d-%d, %02d:%02d:%02d.\n",
		rtc_tm1.tm_mday, rtc_tm1.tm_mon + 1, rtc_tm1.tm_year + 1900,
		rtc_tm1.tm_hour, rtc_tm1.tm_min, rtc_tm1.tm_sec);

	if (rtc_tm1.tm_sec > rtc_tm.tm_sec) {
		if (rtc_tm1.tm_sec == (rtc_tm.tm_sec+5))
			printf("Read Time Test OK!\n");
		else
			printf("Read Time Test FAIL!\n");		
	} else {
		if ((rtc_tm1.tm_sec+60) == (rtc_tm.tm_sec+5))
			printf("Read Time Test OK!\n");
		else
			printf("Read Time Test FAIL!\n");	
	}		
		
	/***************************/
	/* Set the ALARM time/date */
	/***************************/
	/* Set the alarm to 5 sec in the future, and check for rollover */
	rtc_tm.tm_sec += 5;
	if (rtc_tm.tm_sec >= 60) {
		rtc_tm.tm_sec %= 60;
		rtc_tm.tm_min++;
	}
	if (rtc_tm.tm_min == 60) {
		rtc_tm.tm_min = 0;
		rtc_tm.tm_hour++;
	}
	if (rtc_tm.tm_hour == 24)
		rtc_tm.tm_hour = 0;

	rtc_tm.tm_year = 0;
	rtc_tm.tm_mon = 0;
	rtc_tm.tm_mday = 0;

	retval = ioctl(fd, RTC_ALM_SET, &rtc_tm);
	if (retval == -1) {
		if (errno == ENOTTY) {
			fprintf(stderr,
				"\n...Alarm IRQs not supported.\n");
			goto test_PIE;
		}
		perror("RTC_ALM_SET ioctl");
		exit(errno);
	}

	/* Read the current alarm settings */
	retval = ioctl(fd, RTC_ALM_READ, &rtc_tm);
	if (retval == -1) {
		perror("RTC_ALM_READ ioctl");
		exit(errno);
	}

	fprintf(stderr, "Alarm time now set to %02d:%02d:%02d.\n",
		rtc_tm.tm_hour, rtc_tm.tm_min, rtc_tm.tm_sec);

	/* Enable alarm interrupts */
	retval = ioctl(fd, RTC_AIE_ON, 0);
	if (retval == -1) {
		perror("RTC_AIE_ON ioctl");
		exit(errno);
	}

	fprintf(stderr, "Waiting 5 seconds for alarm...");
	fflush(stderr);
	/* This blocks until the alarm ring causes an interrupt */
//	retval = read(fd, &data, sizeof(unsigned long));
//	if (retval == -1) {
//		perror("read");
//		exit(errno);
//	}
	irqcount++;
	fprintf(stderr, " okay. Alarm rang.\n");

	/* Disable alarm interrupts */
	retval = ioctl(fd, RTC_AIE_OFF, 0);
	if (retval == -1) {
		perror("RTC_AIE_OFF ioctl");
		exit(errno);
	}

test_PIE:

	/* The periodic is 1/256(1), 1/64(2), 1/16(3), 1/4(4), 1/2(5), 1(6) second. */
	for (tmp=1; tmp<=6; tmp++) {

		retval = ioctl(fd, RTC_IRQP_SET, tmp);
		if (retval == -1) {
			/* not all RTCs can change their periodic IRQ rate */
			if (errno == ENOTTY) {
				fprintf(stderr,
					"\n...Periodic IRQ rate is fixed\n");
				goto done;
			}
			perror("RTC_IRQP_SET ioctl");
			exit(errno);
		}

		fflush(stderr);

		/* Enable periodic interrupts */
		retval = ioctl(fd, RTC_PIE_ON, 0);
		if (retval == -1) {
			perror("RTC_PIE_ON ioctl");
			exit(errno);
		}

		for (i=1; i<21; i++) {
			/* This blocks */
			retval = read(fd, &data, sizeof(unsigned long));
			if (retval == -1) {
				perror("read");
				exit(errno);
			}
			fprintf(stderr, " %d",i);
			fflush(stderr);
			irqcount++;
		}
		printf("\n");
		
		/* Disable periodic interrupts */
		retval = ioctl(fd, RTC_PIE_OFF, 0);
		if (retval == -1) {
			perror("RTC_PIE_OFF ioctl");
			exit(errno);
		}
	}

done:
	fprintf(stderr, "\n\n\t\t\t *** Test complete ***\n");

	close(fd);

	return 0;
}
