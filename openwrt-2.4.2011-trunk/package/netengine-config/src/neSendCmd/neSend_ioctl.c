/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : neSend_ioctl.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration IOCTL file
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <net/if.h>

#include <linux/cs_ne_ioctl.h>
#include <ne_defs.h>

#define     IOCTL_INTERFACE     "eth0"

/*-----------------------------------------------------------------
* ROUTINE NAME - IOCTL_NetEngine
*----------------------------------------------------------------*/
int IOCTL_NetEngine(void *pBuffer)
{
	int sockfd;
	struct ifreq ifr;
	int retStatus = STATUS_SUCCESS;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) {
		printf("<IOCTL> IOCTL_NetEngine: open socket fail, %s\n",
		       strerror(errno));
		return STATUS_ERR_SOCKET;
	}
	strncpy(ifr.ifr_name, IOCTL_INTERFACE, IFNAMSIZ - 1);
	ifr.ifr_data = (void *)pBuffer;
	if (ioctl(sockfd, SIOCDNEPRIVATE, &ifr) == -1) {
		printf("<IOCTL> IOCTL IOCTL_feFVLAN ERROR!\n");
		retStatus = STATUS_ERR_IOCTL;
	}
	close(sockfd);
	return retStatus;
} /*end IOCTL_NetEngine(); */
