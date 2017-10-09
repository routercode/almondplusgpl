/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : ne_ipc.c
   Date        : 2010-08-20
   Description : Cortina GoldenGate NetEngine configuration utility IPC module
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : 

 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#include <ipc.h>
unsigned long SendCommand(char *pBuffer, unsigned int SendLen)
{
	int sockfd;
	struct sockaddr_in addr;
	int addr_len = sizeof(struct sockaddr_in);
	fd_set Recvfds;
	struct timeval TimeVal;

	/*create socket */
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("IOCTL: socket error");
		return 1;
	}

	/*build address data structure */
	bzero((char *)&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(IPC_SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr(IPC_SERVER_IP);
	if (sendto(sockfd, pBuffer, SendLen, 0, (struct sockaddr *)&addr,
	           addr_len) == -1) {
		close(sockfd);
		return 1;
	}

	/*rcv packet */
	FD_ZERO(&Recvfds);
	FD_SET(sockfd, &Recvfds);
	TimeVal.tv_sec = 3;
	TimeVal.tv_usec = 0;
	if (select(sockfd + 1, &Recvfds, (fd_set *) NULL, 
		   (fd_set *) NULL, &TimeVal)) {
		recvfrom(sockfd, pBuffer, SendLen, 0, 
			 (struct sockaddr *)&addr, &addr_len);
		close(sockfd);
		return 0;
	} else {
		close(sockfd);
		return 1;
	}
} /*end SendCommand() */
