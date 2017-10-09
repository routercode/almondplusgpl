/******************************************************************************
     Copyright (c) 2010, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : pktgen.c
   Date        : 2010-11-05
   Description : Cortina Systems Packet Translator
                 To translate one of following files to FPGA simulation packet files
                 (1) Ethereal captured file
                 (2) Storlink Packet Description File
   Author      : Axl Lee <axl.lee@cortina-systems.com>
   Remarks     : <Txt Edit: UltraEdit, Tab: 4>
                 ----------------------------------------------------
                 ----------------------------------------------------
   Syntax: 
        pktgen [input file] [output file] 
        pktgen -s [Server Interface Name] -c [Client Interface Name] -i [input file] -o [output file]
            -s: Server Interface Name, for ex. "eth0"
            -c: Client Interface Name, for ex. "eth1"
            -i: input file: Cortina Systems Packet Description File
            -o: output file: Cortina Systems Packet Description File

   History     :
 
        2010/11/05  Axl Lee         Develop for GoldenGate.

 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "pktgen.h"
#include "fileProcess.h"
#include "rawSocketHdr.h"


#define SVERSION            "1.0.0"
char VersionInfo[] = "Cortina GoldenGate Packet Generator Utility "SVERSION" ["__DATE__" "__TIME__"]\n";
char syntax[]=  "\nUsage:\n"
                "   pktgen -s [Server Interface Name] -c [Client Interface Name] -i [input file] -o [output file] \n"
                "\nOption:\n"
                "   -s: Server Interface Name, for ex. \"eth0\"\n"
                "   -c: Client Interface Name, for ex. \"eth1\"\n"
                "   -i: input file: Cortina Systems Packet Description File\n"
                "   -o: output file: Cortina Systems Packet Description File\n";


DESCRIPTION_INFO_T  description_info;
DESCRIPTION_INFO_T	*pInfo = &description_info;

typedef struct _thread_info_{
	pthread_t       server_thread;
	int 			server_thread_state;
	void 			*server_thread_result;

//	pthread_t       client_thread;
//	int 			client_thread_state;
//	void 			*client_thread_result;
//
} thread_info_s;

thread_info_s   threadInfo;
thread_info_s   *pThreadInfo = &threadInfo;

void *server_thread(void *arg)
{
	unsigned char   buf[MAX_JUMBO_PKT_SIZE];
	unsigned long   PktLen, PktLeninFile=0;
	unsigned long   *pCSMagic, *pSeqNo, SequenceNumber;
	unsigned char   RcvBuffer[MAX_JUMBO_PKT_SIZE];
	unsigned char   status = CS_STATUS_SUCCESS;
    
	printf("server_thread Enter\n");

    pThreadInfo->server_thread_state = 1;
    
	while(pThreadInfo->server_thread_state)
	{
	    bzero(buf, sizeof(buf));
	    PktLen = 0;
	    recvRawData(buf, &PktLen);
	    
	    pCSMagic = (unsigned long*)(&buf[PAYLOAD_CS_MAGIC_OFFSET]);
        pSeqNo = (unsigned long*)(&buf[PAYLOAD_SEQUENCE_NUMBER_OFFSET]);
        
 //       printf("CSMagic 0x%8.8x\n", (int)SWAP_DWORD(*pCSMagic));
//	    printf("receive: %d\n", (int)PktLen);
	    if(SWAP_DWORD(*pCSMagic) == CORTINA_MAGIC_NUMBER)
	    {
//{
//    int i;
//    for(i=0; i<PktLen; i++)
//    {
//        printf("%2.2x ", buf[i]);
//        if((i!=0) && ((i+1)%16)==0)
//            printf("\n");
//    }
//    printf("\n");
//    
//}
            SequenceNumber = SWAP_DWORD(*pSeqNo);
//            printf("SeqNo 0x%8.8x\n", (int)SequenceNumber);
	        status = read_a_pkt_hex_file(SequenceNumber, RcvBuffer, &PktLeninFile);
        	if(status == CS_STATUS_SUCCESS)
        	{
//	            printf("Packet Found\n");
	            if(PktLeninFile != PktLen)
	            {
	                printf("ERROR: Packet Length NOT Match\n");
	            }else
	            {
    	            if(memcmp(buf, RcvBuffer, PktLeninFile) == 0)
    	            {
    	                printf("success: %d \n", (int)SequenceNumber);
    	            }else
    	            {
    	                printf("ERROR: Sequence %d\n", (int)SequenceNumber);
    	            }
	            }
        	}else
        	{
        	    printf("Packet NOT Found\n");
        	}
        }
//	    usleep(10000);
	}//end while(pThreadInfo->server_thread_state)

	printf("server_thread Left\n");
	pthread_exit(NULL);
}/* server_thread */

//void *client_thread(void *arg)
//{
//    
//	printf("client_thread Enter\n");
//    pThreadInfo->client_thread_state = 1;
//    
//	while(pThreadInfo->client_thread_state)
//	{
//	    usleep(10000);
//	}//end while(pThreadInfo->client_thread_state)
//	
//	printf("client_thread Left\n");
//	pthread_exit(NULL);
//}/* client_thread */

unsigned char threadOpen(thread_info_s *pThreadInfo)
{
	pthread_create(&(pThreadInfo->server_thread), NULL, &server_thread, NULL);
//	pthread_create(&(pThreadInfo->client_thread), NULL, &client_thread, NULL);
	
	return 0;
}/* threadOpen */

void threadClose(thread_info_s *pThreadInfo)
{
	pThreadInfo->server_thread_state = 0;
	pthread_join(pThreadInfo->server_thread, &(pThreadInfo->server_thread_result));

//	pThreadInfo->client_thread_state = 0;
//	pthread_join(pThreadInfo->client_thread, &(pThreadInfo->client_thread_result));
    
    return;
}/* threadClose */

//=================================================================
//                        Main Functions
//=================================================================
const char *AllOpts = "Hhs:c:i:o:";
int main(int argc, char *argv[])
{
    int                 oc;
    char                inputName[128], outputName[128];

    printf(VersionInfo);
    if(argc < 2)
    {
        printf(syntax);
        exit(1);
    }
    
    bzero(inputName, sizeof(inputName));
    bzero(outputName, sizeof(outputName));
    bzero(prawSockInfo, sizeof(raw_sock_info_s));
    // Give default serverName and clientName
    strcpy(prawSockInfo->serverName, DEFAULT_SERVER_INTERFACE_NAME);
    strcpy(prawSockInfo->clientName, DEFAULT_CLIENT_INTERFACE_NAME);

	// Don't print error message
	opterr = 0;
	// Parameter get
	while((oc = getopt(argc, argv, AllOpts)) != -1)
	{
    	switch(oc)
    	{
            case 's':
                bzero(prawSockInfo->serverName, sizeof(prawSockInfo->serverName));
                strcpy(prawSockInfo->serverName, optarg);
                break;
    
            case 'c':
                bzero(prawSockInfo->clientName, sizeof(prawSockInfo->clientName));
                strcpy(prawSockInfo->clientName, optarg);
                break;

            case 'i':
                if(strlen(optarg) > sizeof(inputName))
                {
                    printf("input file name too long\n");
                    return 1;
                }
                strcpy(inputName, optarg);
                break;
    
            case 'o':
                if(strlen(optarg) > sizeof(outputName))
                {
                    printf("output file name too long\n");
                    return 1;
                }
                strcpy(outputName, optarg);
                break;
    
            case 'h':
            case 'H':
            default:
                printf(syntax);
                return 1;
    	}//end switch()
    }//end while()
    
    if((0 == strlen(inputName)) || (0 == strlen(outputName)))
    {
        printf(syntax);
        return 1;
    }
    
    if (0 != rawSockOpen(prawSockInfo))
        return 1;
    threadOpen(pThreadInfo);
    sleep(1);
    cs_process_specify_file(inputName, outputName, syntax);
    sleep(1);
    threadClose(pThreadInfo);
    rawSockClose(prawSockInfo);
   
    return 0;
}/* main */
