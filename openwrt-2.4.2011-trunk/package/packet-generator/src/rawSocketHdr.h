#ifndef __RAW_SOCKET_HDR_H__
#define __RAW_SOCKET_HDR_H__

#include <netpacket/packet.h>

#define DEFAULT_SERVER_INTERFACE_NAME         "eth0"
#define DEFAULT_CLIENT_INTERFACE_NAME         "eth1"

#define PCKT_LEN 8192

typedef struct _raw_sock_info_{
    // definition for Server
    char                serverName[32];
    struct sockaddr_ll	srvaddr;
    int                 rawSockServer;

    // definition for Client
    char                clientName[32];
    struct sockaddr_ll	cliaddr;
    int                 rawSockClient;
} raw_sock_info_s;

extern raw_sock_info_s *prawSockInfo;

unsigned char rawSockOpen(raw_sock_info_s *prawSockInfo);
void rawSockClose(raw_sock_info_s *prawSockInfo);
unsigned char sendRawData(unsigned char* pBuffer, unsigned long PktLen);
unsigned char recvRawData(unsigned char* pBuffer, unsigned long *pPktLen);

#endif //__RAW_SOCKET_HDR_H__
