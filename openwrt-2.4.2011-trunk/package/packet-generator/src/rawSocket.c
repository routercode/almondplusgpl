/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <net/ethernet.h>
#include <arpa/inet.h>
#include <errno.h>

#include "rawSocketHdr.h"

raw_sock_info_s     rawSockInfo;
raw_sock_info_s     *prawSockInfo = &rawSockInfo;

unsigned char rawSockOpen(raw_sock_info_s *prawSockInfo)
{
	socklen_t		        len;
   
    // Create a raw socket: server
	prawSockInfo->rawSockServer = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(prawSockInfo->rawSockServer < 0)
    {
        printf("ERROR: socket() error");
        return 1;
    }

    prawSockInfo->srvaddr.sll_protocol = htons(ETH_P_ALL);
    prawSockInfo->srvaddr.sll_ifindex = if_nametoindex(prawSockInfo->serverName);
    printf("prawSockInfo->srvaddr.sll_ifindex %d, %s\n", prawSockInfo->srvaddr.sll_ifindex, prawSockInfo->serverName);
	len = sizeof(struct sockaddr);
	bind (prawSockInfo->rawSockServer, (struct sockaddr*) &(prawSockInfo->srvaddr), len);

	printf ("Socket Bound to %d\n", prawSockInfo->srvaddr.sll_ifindex);

    // Create a raw socket: client
	prawSockInfo->rawSockClient = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if(prawSockInfo->rawSockClient < 0)
    {
        printf("ERROR: socket() error\n");
        close(prawSockInfo->rawSockClient);
        return 1;
    }

	prawSockInfo->cliaddr.sll_protocol = htons(ETH_P_ALL);
	prawSockInfo->cliaddr.sll_ifindex  = if_nametoindex(prawSockInfo->clientName);

    return 0;
}/* rawSockOpen */

void rawSockClose(raw_sock_info_s *prawSockInfo)
{
    if (prawSockInfo->rawSockServer)
        close(prawSockInfo->rawSockServer);
    
    if (prawSockInfo->rawSockClient)
        close(prawSockInfo->rawSockClient);
    
    return;
}/* rawSockClose */

unsigned char sendRawData(unsigned char* pBuffer, unsigned long PktLen)
{
	struct sockaddr_ll	dstaddr;
	socklen_t		    len = sizeof(struct sockaddr_ll);;

    dstaddr.sll_protocol = prawSockInfo->cliaddr.sll_protocol;
    dstaddr.sll_ifindex  = prawSockInfo->cliaddr.sll_ifindex;
	memcpy(dstaddr.sll_addr, pBuffer, ETH_ALEN); 

    if(sendto(prawSockInfo->rawSockClient, pBuffer, PktLen, 0, (struct sockaddr*)&dstaddr, len) < 0)
    {
        return 1;
    }

    return 0;
}/* sendRawData */

unsigned char recvRawData(unsigned char* pBuffer, unsigned long *pPktLen)
{
    int			        InBufLen = 0;
	socklen_t		    len = sizeof(struct sockaddr_ll);;
    
	InBufLen = recvfrom(prawSockInfo->rawSockServer, pBuffer, PCKT_LEN, 0, (struct sockaddr*) &(prawSockInfo->srvaddr), &len);
	if(InBufLen < 0)
	{
	    return -1;
	}
    *pPktLen = InBufLen;
    
    return 0 ;
}/* recvRawData */
