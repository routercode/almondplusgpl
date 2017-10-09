/* test_pes.c - Test for PES filters.
 * usage: DEMUX=/dev/dvb/adapterX/demuxX test_pes PID
 *
 * Copyright (C) 2002 convergence GmbH
 * Johannes Stezenbach <js@convergence.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <sys/timeb.h>
#include <pthread.h>

//debug_Aaron
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <linux/dvb/dmx.h>
#include <linux/dvb/frontend.h>

#define MB 			(1024*1024)
#define KB  			(1024)
#define MAX_FILE_SIZE 		(20*MB)

#define	G2_TS_MAX_CHANNEL	(6) /* Maximum channel number of TS module. */

#define	MXA_DMUX_DEVICE		(12)

#define MAX_PES_SIZE   		(31584) //(32712)//(188*348)

//#define SKIP_COUNT 		(100)
static unsigned int		skip_count = 100;

#define MAX_DMX_BUFFER_SIZE 	(188*16*1024) //(192*4096)

#define UDP_SEND_SIZE		(7*188)

typedef struct {
	int	fd;
  	int   	id;
  	char	outfile[64];
  	char  	string[128];
} thread_parm_t;

typedef struct {
	unsigned int	id;
	char		filename[32];
	unsigned int	pid_num;
	unsigned int	pid[32];
} G2_TS_PID_T;

static G2_TS_PID_T	ts_rxq_pid[12];
static int 		fefd[G2_TS_MAX_CHANNEL];
static int 		dmxfd[MXA_DMUX_DEVICE];
static unsigned char	remote_ip[16];
static pthread_mutex_t 	mutex;

static int		udp_send = 0;

//debug_Aaron
struct sockaddr_in  	recipient[G2_TS_MAX_CHANNEL];
int 			snd_sock[G2_TS_MAX_CHANNEL];

#define PEER_PORT 	1234

static unsigned char peer_ip[G2_TS_MAX_CHANNEL][16]={
				"227.1.1.1","227.2.2.2","227.3.3.3",
				"227.4.4.4","227.5.5.5","227.6.6.6"};
int cs_UdpSocketInit(char *szRecipient, int Port, struct sockaddr_in *recipient);
int cs_UdpSend(int s, struct sockaddr_in *recipient, char *buf, int len);

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
	
	/* Create the socket */
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s == -1) {
		printf("socket() failed; %d\n", errno);
		return -1;
	}
	
	/* Resolve the recipient's IP address or hostname */
	recipient->sin_family = AF_INET;
	recipient->sin_port = htons((short)Port);
	if ((recipient->sin_addr.s_addr = inet_addr(szRecipient))
		== INADDR_NONE)	{
		host = gethostbyname(szRecipient);
		if (host) {
	    		memcpy(&recipient->sin_addr, host->h_addr_list[0],
	        		host->h_length);
		} else {
			printf("gethostbyname() failed: %d\n", errno);
			return -1;
		}
	}

	return s;
}


int cs_UdpSend(int s, struct sockaddr_in *recipient, char *buf, int len)
{
	int ret;

	ret = sendto(s, buf, len, 0,
		(struct sockaddr *) recipient, sizeof(struct sockaddr));

	return ret;
}

void cs_UdpClose(int s)
{
	close(s);
}

void usage(void)
{
	fprintf(stderr, "Usage: test_pes [-r] [-pid <PID1 PID2..>] [-f <FILENAME>] [-l]\n");
	fprintf(stderr, "\t-pid PID1 PID2 PID3.. : write PIDs\n");
	fprintf(stderr, "\t-r                    : read TS driver\n");
	fprintf(stderr, "\t-f FILENAME           : write to file name\n");
    	fprintf(stderr, "\t-l                    : list PIDs\n");
	exit(1);
}

int str2int(char *str)
{
	long result = 0;
 	int tmp;
 	int len;

 	len = strlen(str);
 	while ( len > 0 ) {
 		tmp = *str++ - '0';
 		if ( 9 < tmp )  {
 			printf( "\"%c\" is not a number!\n" , tmp + '0' );
 			break;
 		}
 		len--;
 		result = result*10 + tmp;
 	}
 	return result;
}

static int config_file_open(void)
{
	int	fd;
	char 	*config_file="/etc/ts.cfg";

	fd = open(config_file, O_RDONLY);
	if (fd >= 0) {
		printf("config '%s' open.\n", config_file);
		return fd;
	} else {
		printf("Could not open config file %s\n", config_file);
		return(-1);
	}
}

static int config_file_read_line( int fd, unsigned char *line_buf, int line_buf_size)
{
	unsigned int 	read_len;
	unsigned int	total_read_len;
	unsigned char	read_char;
	unsigned char 	*p;

	p = line_buf;
	total_read_len = 0;

	while ( 1 ) {
		read_len  = read(fd, &read_char, 1);
		if ( read_len <= 0 ) {
			return ( -1 );
		} else if ( read_char == '\r' )	{
			continue;
		} else if ( read_char == '\n' )	{
			break;
		}

		*p = read_char;
		p++;
		total_read_len++;

		if ( total_read_len >= line_buf_size )
			break;
	}

	*p = '\0';

	return total_read_len;
}


int set_pes_buffer_size( int fd)
{
	return ioctl(fd, DMX_SET_BUFFER_SIZE, MAX_DMX_BUFFER_SIZE);
}


int ts_set_filter_pids(unsigned int dev_id)
{
	unsigned int	fd,i;
	unsigned long	pid;
	struct dmx_pes_filter_params f;

	if (ts_rxq_pid[dev_id].pid_num <= 0)
		return 0;

	fd = dmxfd[ts_rxq_pid[dev_id].id];
	
	memset(&f, 0, sizeof(f));
	f.pid = (unsigned short) ts_rxq_pid[dev_id].pid[0];
	f.input = DMX_IN_FRONTEND;
	f.output = DMX_OUT_TSDEMUX_TAP;
	f.pes_type = DMX_PES_OTHER;
	f.flags = 0;
	if (ioctl(fd, DMX_SET_PES_FILTER, &f) == -1) {
		perror("ioctl DMX_SET_PES_FILTER");
		return 1;
	}

	for (i=1; i<ts_rxq_pid[dev_id].pid_num; i++) {
		pid = ts_rxq_pid[dev_id].pid[i];
		if (ioctl(fd, DMX_ADD_PID, &pid) == -1) {
			printf("Queue id=%d   pid=%d ... Fail.\n",ts_rxq_pid[dev_id].id,pid);
			return 1;
		}
	}

	if (ioctl(fd, DMX_START) == -1) {
		printf("fd = %d...",fd);
		perror("DMX_START");
		close(dmxfd);
		return 1;
	}

	return 0;
}

int get_pes_pids(int fd)
{
	int i;
	unsigned short pids[32];
	int retval=0;

	memset(pids, 0xff, sizeof(pids));

	ioctl(fd, DMX_GET_PES_PIDS, pids);

	for (i=0;i<32;i++)
	{
		if (pids[i]==0xffff)
			break;
		printf("pid %d =0x%04x (%d)\n",i,  pids[i], pids[i]);

	}/*for*/

	return retval;
}


void *threadfunc0(void *parm)
{
  	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int 		written=0;
	int 		bytes;
	int		ret;
	FILE 		*out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL)
	{
		printf("malloc error\n");
		exit(1);
	}
	
	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
	  	if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	}
	else {
		perror("open out file ");
	}


	for (;;) {
		//bytes=process_pes0(p->fd, stdout, buf);
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			}
			else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			//debug_aaron
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;
		         		}
		         		remain = remain - UDP_SEND_SIZE;
		         		index = index + UDP_SEND_SIZE;
	        		}
	        		if (remain) {
					ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;
		         		}
//		         		printf("remain=%d \n",remain);
				}
			}

			if ( (counter%skip_count)==0) {
	   			printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
	   			        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
	   		}
		}

		counter++;

		if (out) {
	        	written+=bytes;

			if (written >(MAX_FILE_SIZE)) {
		        	printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
		    	}

			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

    	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}

	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}

  	free(p);

  	return 0;
}

void *threadfunc1(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int 		written=0;
	int 		bytes;
	int		ret;
	FILE 		*out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL)	{
		printf("malloc error\n");
		exit(1);
	}
	
	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
	  	if (tmpfile==NULL) {
		 	printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}


	for (;;) {
		//bytes=process_pes0(p->fd, stdout, buf);
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			}
			else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			//debug_aaron
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;
		         		}
		         		remain = remain - UDP_SEND_SIZE;
		         		index = index + UDP_SEND_SIZE;
	        		}
	        		if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;
		         		}
				}
			}
	
			if ( (counter%skip_count)==0) {
	   			printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
	   			        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
	   		}
		}
	
		counter++;
	
		if (out) {
	        	written+=bytes;
	
			if (written >(MAX_FILE_SIZE)) {
		        	printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
		    	}
	
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

unsigned long total = 0;
void *threadfunc2(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int 		written=0;
	int 		bytes;
	int		ret;
	FILE 		*out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}
	
	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
	  	if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
		}
		fptr = tmpfile;
	}
	else {
		perror("open out file ");
	}


	for (;;) {
		//bytes=process_pes0(p->fd, stdout, buf);
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		total += bytes;
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			}
			else {
	            		printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			//debug_aaron
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;		         	
		               		}
		         		remain = remain - UDP_SEND_SIZE;
		         		index = index + UDP_SEND_SIZE;
	        		}
	        		if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;
		         		}
				}
			}
	
			if ( (counter%skip_count)==0) {
	   			printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
	   			        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
	   		}
		}

		counter++;

		if (out) {
	        	written+=bytes;

			if (written >(MAX_FILE_SIZE)) {
		        	printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
		    	}

			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
//		printf("total = %d...\n",total);
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc3(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int 		written=0;
	int 		bytes;
	int		ret;
	FILE 		*out = stdout;
	static unsigned int counter = 0;
	
	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
	  	if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}


	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
		    		printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
			 		ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
		 			if (ret == -1) {
		       				printf("sendto() failed; %d\n", errno);
		       				break;
		 			}
		 			remain = remain - UDP_SEND_SIZE;
		 			index = index + UDP_SEND_SIZE;
				}
				if (remain) {
					ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
			 		if (ret == -1){
			       			printf("sendto() failed; %d\n", errno);
			       			break;
			 		}
				}
			}
		
			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
			        	p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
	        	written+=bytes;

			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
		    	}

			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}

	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}

  	free(p);

  	return 0;
}

void *threadfunc4(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int 		written=0;
	int 		bytes;
	int		ret;
	FILE 		*out = stdout;
	static unsigned int counter = 0;
	
	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
	  	if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	}
	else {
		perror("open out file ");
	}


	for (;;) {
		//bytes=process_pes0(p->fd, stdout, buf);
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
	            		printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			//debug_aaron
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;
		         		}
		         		remain = remain - UDP_SEND_SIZE;
		         		index = index + UDP_SEND_SIZE;
	        		}
	        		if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
		         		if (ret == -1) {
		               			printf("sendto() failed; %d\n", errno);
		               			break;
		         		}
				}
			}

			if ( (counter%skip_count)==0) {
	   			printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
	   			        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
	   		}
		}

		counter++;

		if (out) {
	        	written+=bytes;

			if (written >(MAX_FILE_SIZE)){
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}

			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc5(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int written=0;
	int bytes;
	int	ret;
	FILE *out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
		if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}

	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
					remain = remain - UDP_SEND_SIZE;
					index = index + UDP_SEND_SIZE;
				}
				if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
				}
			}

			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
				        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
			written+=bytes;
		
			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}
		
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc6(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int written=0;
	int bytes;
	int	ret;
	FILE *out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
		if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}

	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
					remain = remain - UDP_SEND_SIZE;
					index = index + UDP_SEND_SIZE;
				}
				if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
				}
			}

			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
				        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
			written+=bytes;
		
			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}
		
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc7(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int written=0;
	int bytes;
	int	ret;
	FILE *out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
		if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}

	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
					remain = remain - UDP_SEND_SIZE;
					index = index + UDP_SEND_SIZE;
				}
				if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
				}
			}

			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
				        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
			written+=bytes;
		
			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}
		
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc8(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int written=0;
	int bytes;
	int	ret;
	FILE *out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
		if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}

	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
					remain = remain - UDP_SEND_SIZE;
					index = index + UDP_SEND_SIZE;
				}
				if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
				}
			}

			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
				        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
			written+=bytes;
		
			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}
		
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc9(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int written=0;
	int bytes;
	int	ret;
	FILE *out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
		if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}

	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
					remain = remain - UDP_SEND_SIZE;
					index = index + UDP_SEND_SIZE;
				}
				if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
				}
			}

			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
				        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
			written+=bytes;
		
			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}
		
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc10(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int written=0;
	int bytes;
	int	ret;
	FILE *out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
		if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}

	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
					remain = remain - UDP_SEND_SIZE;
					index = index + UDP_SEND_SIZE;
				}
				if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
				}
			}

			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
				        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
			written+=bytes;
		
			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}
		
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

void *threadfunc11(void *parm)
{
	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	unsigned char	*tmpfile,*fptr;
	unsigned int	index;
	unsigned int	remain;
	int written=0;
	int bytes;
	int	ret;
	FILE *out = stdout;
	static unsigned int counter = 0;

	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);
	
	buf=malloc(MAX_PES_SIZE);	/* TS read buffer */
	if (buf==NULL) {
		printf("malloc error\n");
		exit(1);
	}

	out = fopen(p->outfile, "wb");
	if (out) {
		tmpfile=malloc(MAX_FILE_SIZE);	/* TS write buffer */
		if (tmpfile==NULL) {
			printf("malloc error\n");
			exit(1);
	    	}
		fptr = tmpfile;
	} else {
		perror("open out file ");
	}

	for (;;) {
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "ID=%d counter=%d--read error: buffer overflow (%d)\n",p->id,counter,EOVERFLOW);
			} else {
				printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
		} else {
			if (udp_send > 0) {
				index = 0;
				remain = bytes;
				while (remain > UDP_SEND_SIZE) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], UDP_SEND_SIZE);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
					remain = remain - UDP_SEND_SIZE;
					index = index + UDP_SEND_SIZE;
				}
				if (remain) {
				 	ret = cs_UdpSend(snd_sock[p->id], &recipient[p->id], &buf[index], remain);
					if (ret == -1) {
						printf("sendto() failed; %d\n", errno);
						break;
					}
				}
			}

			if ( (counter%skip_count)==0) {
				printf("fd= %x counter=%d--length=%d first byte=%x.%x.%x.%x\n",
				        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
			}
		}

		counter++;

		if (out) {
			written+=bytes;
		
			if (written >(MAX_FILE_SIZE)) {
				printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
			}
		
			memcpy(fptr,buf,bytes);
			fptr= fptr + bytes;
		}
	}

	if ((out>0) && (bytes>0)) {
		fwrite(tmpfile,written,1,out);
	}
	
	if (out) {
		fflush(out);
		fclose(out);
		printf("Write file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
	}
	
	free(p);
	
	return 0;
}

int parse_cfg_file(void)
{
	int 		cfg_fd;
	unsigned char	line_buf[256];
	unsigned int	ts_num;
	char 		*token;
	unsigned int	i,j;
	int		ret;

	/* open TS configuration file */
	if ((cfg_fd = config_file_open()) < 0) {
		perror("open");
		return 1;
	}

	/* parsing TS configuration file */
	ts_num =0;
	while (1)
	{
		ret = config_file_read_line(cfg_fd, line_buf, sizeof(line_buf));
		if ( ret < 0 ) {
			break;
		}
		token = strtok((char *)line_buf," "); /*There are two delimiters here*/
		if (memcmp(token,"#",1)==0)
			continue;
	
		if (strcmp(token,"UDP_SEND") == 0) {
	       		token = strtok(NULL," ");
			udp_send = str2int(token);
			printf("*** udp_send = %d ***\n",udp_send);
			continue;
		}

		if (token != NULL)
			ts_rxq_pid[ts_num].id = str2int(token);
		
		token = strtok(NULL," ");
		if (token != NULL)
			strcpy(ts_rxq_pid[ts_num].filename,token);
		
		token = strtok(NULL," ");
		
		i = 0;
		while (token != NULL){
			ts_rxq_pid[ts_num].pid[i] = str2int(token);
			token = strtok(NULL," ");
			i++;
		}
		ts_rxq_pid[ts_num].pid_num = i;
		ts_num++;
	}

#ifdef DEBUG
	/* display the content of TS configuration file */
	for (i=0; i<ts_num; i++) {
		printf("id = %d ",ts_rxq_pid[i].id);
		printf("file name = %s  count = %d \n\nPID=",ts_rxq_pid[i].filename,ts_rxq_pid[i].pid_num);
		for (j=0; j<ts_rxq_pid[i].pid_num; j++)
		{
			printf(" %d ",ts_rxq_pid[i].pid[j]);
		}
		printf("\n\n\n");
	}
#endif	
}

static void tune_atsc(int fd, uint32_t freq, fe_modulation_t modulation)
{
	struct dvb_frontend_parameters params;

	memset(&params, 0, sizeof(params));
	params.frequency = freq;
	params.inversion = INVERSION_AUTO;
	params.u.vsb.modulation = modulation;

	if (ioctl(fd, FE_SET_FRONTEND, &params) == -1) {
		fprintf(stderr, "ioctl FE_SET_FRONTEND failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	for (;;) {
		fe_status_t status;

		if (ioctl(fd, FE_READ_STATUS, &status) == -1) {
			fprintf(stderr, "ioctl FE_READ_STATUS failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		/* printf("%s: status = 0x%x\n", __func__, status); */
		if (status & FE_HAS_LOCK)
			break;
		sleep(1);
	}
}

int main(int argc, char *argv[])
{
	int i,j;
	unsigned int	pid;
	unsigned char	*buf;
	FILE		*out = stdout;
	char		filename[128];
	int		write_to_file = 0;
	int		written = 0;
	int		bytes;
	int		read_ts = 0;
	int		ret;
	pthread_t	thread[MXA_DMUX_DEVICE];
	int		rc=0;
	thread_parm_t	*parm=NULL;
	unsigned int	nport = 1;
#if 0
	char dmxdev[MXA_DMUX_DEVICE][32] = {
			"/dev/dvb/adapter0/demux0",
			"/dev/dvb/adapter0/demux1",
			"/dev/dvb/adapter0/demux2",
			"/dev/dvb/adapter0/demux3",
			"/dev/dvb/adapter0/demux4",
			"/dev/dvb/adapter0/demux5",
			"/dev/dvb/adapter0/demux6",
			"/dev/dvb/adapter0/demux7",
			"/dev/dvb/adapter0/demux8",
			"/dev/dvb/adapter0/demux9",
			"/dev/dvb/adapter0/demux10",
			"/dev/dvb/adapter0/demux11",
			};
	char frontend[G2_TS_MAX_CHANNEL][32] = {
			"/dev/dvb/adapter0/frontend0",
			"/dev/dvb/adapter0/frontend1",
			"/dev/dvb/adapter0/frontend2",
			"/dev/dvb/adapter0/frontend3",
			"/dev/dvb/adapter0/frontend4",
			"/dev/dvb/adapter0/frontend5",			
			};
#else
	char dmxdev[MXA_DMUX_DEVICE][32] = {
			"/dev/dvb0.demux0",
			"/dev/dvb0.demux1",
			"/dev/dvb0.demux2",
			"/dev/dvb0.demux3",
			"/dev/dvb0.demux4",
			"/dev/dvb0.demux5",
			"/dev/dvb0.demux6",
			"/dev/dvb0.demux7",
			"/dev/dvb0.demux8",
			"/dev/dvb0.demux9",
			"/dev/dvb0.demux10",
			"/dev/dvb0.demux11",
			};
	char frontend[G2_TS_MAX_CHANNEL][32] = {
			"/dev/dvb0.frontend0",
			"/dev/dvb0.frontend1",
			"/dev/dvb0.frontend2",
			"/dev/dvb0.frontend3",
			"/dev/dvb0.frontend4",
			"/dev/dvb0.frontend5",			
			};
#endif

	if (argc > 1) {
		nport = strtoul(argv[1],NULL, 0);
	}

	if (argc > 2)
		skip_count = strtoul(argv[2],NULL, 0);

	printf("test_pes v0.99-(date:20110930)\n");
	printf("buffer length=%d\n", MAX_PES_SIZE);

	/* open frontend device */
	for (i=0; i<nport; i++) {
		if (i >= 6)
			break;
			
		fprintf(stderr, "open frontend device: %s\n", frontend[i]);
		if ((fefd[i] = open(frontend[i], O_RDWR)) < 0) {
			perror("open");
			return 1;
		}
		printf("test_pes: using '%s' fd=%d\n", frontend[i],fefd[i]);
		tune_atsc(fefd[i], 63000000, QAM_256);
	}

	/* open DEMUX device */
	for (i=0; i<MXA_DMUX_DEVICE; i++) {
		fprintf(stderr, "open DEMUX device: %s\n", dmxdev[i]);
		if ((dmxfd[i] = open(dmxdev[i], O_RDWR)) < 0) {
			perror("open");
			return 1;
		}
		printf("test_pes: using '%s' fd=%d\n", dmxdev[i],dmxfd[i]);
	}

	ret = parse_cfg_file();

	/* set receive buffer size for each TS port */
	for (i=0; i<MXA_DMUX_DEVICE; i++) {
		set_pes_buffer_size(dmxfd[i]);
	}

	if (udp_send > 0) {
		for (i=0; i<G2_TS_MAX_CHANNEL; i++) {
			snd_sock[i] = cs_UdpSocketInit(peer_ip[i], PEER_PORT, &recipient[i]);
			if (snd_sock[i] < 0) {
				printf("UDP socket #%d initialize failed!!!\r\n",i);
				exit(-1);
			}
		}
	}

	if (nport > 0) {
		/* create threads to receive TS packets */
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[0];
		parm->id = 0;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 0 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[0], NULL, threadfunc0, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n",	parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 1) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[1];
		parm->id = 1;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 1 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[1], NULL, threadfunc1, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n",	parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 2) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[2];
		parm->id = 2;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 2 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[2], NULL, threadfunc2, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n",	parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 3) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[3];
		parm->id = 3;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 3 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[3], NULL, threadfunc3, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n",	parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 4) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[4];
		parm->id = 4;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 4 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[4], NULL, threadfunc4, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n",	parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 5) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[5];
		parm->id = 5;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 5 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[5], NULL, threadfunc5, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n",	parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 6) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[6];
		parm->id = 6;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 6 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[6], NULL, threadfunc6, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n", parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 7) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[7];
		parm->id = 7;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 7 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[7], NULL, threadfunc7, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n", parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 8) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[8];
		parm->id = 8;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 8 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[8], NULL, threadfunc8, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n", parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}
	
	if (nport > 9) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[9];
		parm->id = 9;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 9 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[9], NULL, threadfunc9, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n", parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 10) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[10];
		parm->id = 10;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 10 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[10], NULL, threadfunc10, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n", parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	if (nport > 11) {
		parm = malloc(sizeof(thread_parm_t));
		parm->fd = dmxfd[11];
		parm->id = 11;
		strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
		strcpy(parm->string, "TS Channel 11 Read......");
		ts_set_filter_pids(parm->id);
		rc = pthread_create( &thread[11], NULL, threadfunc11, (void *)parm);
		if (rc)	{
			printf("Create thread %d fail ......\n", parm->id);
			exit(-1);
		}
		printf("\n\n\n");
		sleep(1);
	}

	while (1);

	for (i=0; i<MXA_DMUX_DEVICE; i++) {
		close(dmxfd[i]);
	}

	return 0;
}

