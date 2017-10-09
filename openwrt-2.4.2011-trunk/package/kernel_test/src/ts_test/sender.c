// File Name: sender.c
//
// Description:

//
// Compile:
//    gcc -o sender sender.c
//
// Command line options:
//    sender [-p:int] [-r:IP] [-n:x] [-b:x] [-d:c]
//           -p:int   Remote port
//           -r:IP    Recipient's IP address or hostname
//           -n:x     Number of times to send message
//           -b:x     Size of buffer to send
//           -d:c     Character to fill buffer with
//
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <netdb.h>
#include <errno.h>

#define DEFAULT_PORT            5150
#define DEFAULT_COUNT           25
#define DEFAULT_CHAR            'a'
#define DEFAULT_BUFFER_LENGTH   64

#define TRUE  1
#define FALSE 0

typedef unsigned int DWORD;

int   bConnect = FALSE;                 // Connect to recipient first
int   iPort    = DEFAULT_PORT;          // Port to send data to
char  cChar    = DEFAULT_CHAR;          // Character to fill buffer
DWORD dwCount  = DEFAULT_COUNT,         // Number of messages to send
      dwLength = DEFAULT_BUFFER_LENGTH; // Length of buffer to send
char  szRecipient[128];                 // Recipient's IP or hostname

//
// Function: usage
//
// Description:
//    Print usage information and exit
//
#ifdef STANDALONE
void usage()
{
    printf("usage: sender [-p:int] [-r:IP] "
		   "[-c] [-n:x] [-b:x] [-d:c]\n\n");
    printf("       -p:int   Remote port\n");
    printf("       -r:IP    Recipients IP address or hostname\n");
    printf("       -n:x     Number of times to send message\n");
    printf("       -b:x     Size of buffer to send\n");
    printf("       -d:c     Character to fill buffer with\n\n");
    exit(1);
}

//
// Function: ValidateArgs
//
// Description:
//    Parse the command line arguments, and set some global flags to
//    indicate what actions to perform
//
void ValidateArgs(int argc, char **argv)
{
    int i;
    
    if (argc == 1)
		usage();
    for(i = 1; i < argc; i++)
    {
        if ((argv[i][0] == '-') || (argv[i][0] == '/'))
        {
            switch (argv[i][1])
            {
                case 'p':        // Remote port
                    if (strlen(argv[i]) > 3)
                        iPort = atoi(&argv[i][3]);
                    break;
                case 'r':        // Recipient's IP addr
                    if (strlen(argv[i]) > 3)
                    {
                        strcpy(szRecipient, &argv[i][3]);
                        printf("Recepient's IP addr=%s\n", szRecipient);
                    }
                    break;
                case 'c':        // Connect to recipients IP addr
                    bConnect = TRUE;
                    break;
                case 'n':        // Number of times to send message
                    if (strlen(argv[i]) > 3)
                        dwCount = atol(&argv[i][3]);
                    break;
                case 'b':        // Buffer size
                    if (strlen(argv[i]) > 3)
                        dwLength = atol(&argv[i][3]);
                    break;
                case 'd':		// Character to fill buffer
                    cChar = argv[i][3];
                    break;
                default:
                    usage();
                    break;
            }
        }
    }
}
#endif

//Input:
//    szRecipient : Remote side's IP Address,like "192.168.65.99" 
//    Port        : Remote side's Port number, like 5150
//Return:
//    s           : socket number
//    recipient   : return buffer sockaddr_in used by sendto()

int cs_UdpSocketInit(char *szRecipient, int Port, struct sockaddr_in *recipient)
{
    int           s;
    struct hostent *host=NULL;
    
    // Create the socket
    //
    s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == -1)
    {
        printf("socket() failed; %d\n", errno);
        return -1;
    }
    // Resolve the recipient's IP address or hostname
    //
    recipient->sin_family = AF_INET;
    recipient->sin_port = htons((short)Port);
    if ((recipient->sin_addr.s_addr = inet_addr(szRecipient))
		== INADDR_NONE)
    {
        host = gethostbyname(szRecipient);
        if (host)
            memcpy(&recipient->sin_addr, host->h_addr_list[0],
                host->h_length);
        else
        {
            printf("gethostbyname() failed: %d\n", errno);
            return -1;
        }
    }	
	
	return s;
}


int cs_UdpSend(int s, struct sockaddr_in *recipient, char *buf, int len)
{
	int ret;

//debug_Aaron
//printf("%s: len=%d\r\n", __func__, len);
	ret = sendto(s, buf, len, 0,
           (struct sockaddr *) recipient, sizeof(struct sockaddr));
	
	return ret;	
}

void cs_UdpClose(int s)
{
	close(s);	
}

#ifdef STANDALONE
//
// Function: main
//
// Description:
//    Main thread of execution. Initialize Winsock, parse the command
//    line arguments, create a socket, connect to the remote IP
//    address if specified, and then send datagram messages to the
//    recipient.
//
int main(int argc, char **argv)
{
    int           s;
    char          *sendbuf = NULL;
    int            ret, i;
    int            out_bytes=0;
    struct sockaddr_in    recipient;
        
    // Parse the command line and load Winsock
    //
    ValidateArgs(argc, argv);

    s = cs_UdpSocketInit(szRecipient, iPort, &recipient);
    if (s<0)
    	return 1;
    	
    // Allocate the send buffer
    //
    sendbuf = malloc(dwLength);
    if (!sendbuf)
    {
        printf("malloc() failed: %d\n", errno);
        return 1;
    }
    memset(sendbuf, cChar, dwLength);
    // Otherwise, use the sendto() function
    //
    for(i = 0; i < dwCount; i++)
    {
        ret = cs_UdpSend(s, &recipient, sendbuf, dwLength);
            if (ret == -1)
            {
                printf("sendto() failed; %d\n", errno);
                break;
            }
            out_bytes+=ret;
            // sendto() succeeded!
    }/*for*/
       printf("sendto() success ! byteout=%d\n",out_bytes);

	cs_UdpClose(s);
	
    free(sendbuf);
    
    return 0;
}
#endif
/************************* End of Sender ***********************/
