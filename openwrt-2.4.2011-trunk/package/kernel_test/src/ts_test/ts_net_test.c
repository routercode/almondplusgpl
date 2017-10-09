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

#include <linux/dvb/dmx.h>


#define MB 					(1024*1024)
#define KB  				(1024)
#define MAX_FILE_SIZE 		(50*MB)

#define	MXA_DMUX_DEVICE		(12)

#define MAX_PES_SIZE   		(188*348)
//#define MAX_PES_SIZE   		(188*7)//(188*348)

#define SKIP_COUNT 			(100)

#define MAX_DMX_BUFFER_SIZE (188*1024*200) //(192*4096)

typedef struct {
	int		fd;
  	int   	id;
  	char	outfile[64];
  	char  	string[128];
} thread_parm_t;

typedef struct {
	unsigned int	id;
	char			filename[32];
	unsigned int	pid_num;
	unsigned int	pid[32];
} G2_TS_PID_T;

static G2_TS_PID_T	ts_rxq_pid[12];
static int dmxfd[MXA_DMUX_DEVICE];
static unsigned int	ts_num = 0;

static pthread_mutex_t mutex;

//debug_Aaron
int cs_UdpSocketInit(char *szRecipient, int Port, struct sockaddr_in *recipient);
int cs_UdpSend(int s, struct sockaddr_in *recipient, char *buf, int len);
struct sockaddr_in    recipient;
int snd_sock;
#define PEER_IP "192.168.61.196"
#define PEER_PORT 8888

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
	int		fd;
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
	int read_len;
	int	total_read_len;
	unsigned char	read_char;
	unsigned char *p;

	p = line_buf;
	total_read_len = 0;

	while ( 1 )
	{
		read_len  = read(fd, &read_char, 1);
		if ( read_len <= 0 ) //EOF
		{
			return ( -1 );
		}
		else if ( read_char == '\r' )
		{
			continue;
		}
		else if ( read_char == '\n' )
		{
			break;
		}

		*p = read_char;
		p++;
		total_read_len++;

		if ( total_read_len >= line_buf_size )
		{
			break;
		}
	}

	*p = '\0';

	return total_read_len;
}

/************************************************/



/************************************************/

int iii=0;
int process_pes0(int fd, FILE *out, unsigned char *buf)
{
	int len;
	struct timeb marker2,marker3;
	int marker_count;

//printf("*** fd = %x ***1\n",fd);
// 	pthread_mutex_lock(&mutex);
//printf("*** fd = %x ***2\n",fd);

	ftime(&marker2);
	len = read(fd, buf, MAX_PES_SIZE);
	ftime(&marker3);

//        printf("%s(): fd=%d, buf=0x%x, MAX_PES_SIZE=%d\n",__func__, fd, buf, MAX_PES_SIZE);
	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);


	if (len < 0) {
		if (errno == EOVERFLOW) {
			fprintf(stderr, "iii=%d--read error: buffer overflow (%d)\n",iii,EOVERFLOW);
			return 0;
		}
		else {
            printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,len, errno);
			perror("read");
			exit(1);
		}
	}

	//verizon_demo_parse_ts(buf);

	if ( (iii%SKIP_COUNT)==0)
    	printf("fd= %x iii=%d-read len=%d--time=%d msec\n",fd,iii,len, marker_count);

    if ( (iii%SKIP_COUNT)==0)
	   	printf("fd= %x iii=%d--length=%d first byte=%x.%x.%x.%x\n",fd,iii, len,buf[0],buf[1],buf[2],buf[3]);


	if (out !=stdout)
	{
		 ftime(&marker2);
	   	fwrite(buf,len,1,out);
	   	ftime(&marker3);
	   	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);
	 	if ( (iii%SKIP_COUNT)==0)
	   		printf("fd= %x iii=%d, write len=%d--time=%d msec\n",fd,iii,len, marker_count);
	}

    iii++;

// 	pthread_mutex_unlock(&mutex);

	return len;
}

int process_pes1(int fd, FILE *out, unsigned char *buf)
{
	int len;
	struct timeb marker2,marker3;
	int marker_count;
	static unsigned int counter = 0;

// 	pthread_mutex_lock(&mutex);

	ftime(&marker2);
	len = read(fd, buf, MAX_PES_SIZE);
	ftime(&marker3);

//        printf("%s(): fd=%d, buf=0x%x, MAX_PES_SIZE=%d\n",__func__, fd, buf, MAX_PES_SIZE);
	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);

	if (len < 0) {
		if (errno == EOVERFLOW) {
			fprintf(stderr, "counter=%d--read error: buffer overflow (%d)\n",counter,EOVERFLOW);
			return 0;
		}
		else {
            printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,len, errno);
			perror("read");
			exit(1);
		}
	}

	if ( (counter%SKIP_COUNT)==0) {
    	printf("fd= %x counter=%d-read len=%d--time=%d msec\n",fd,counter,len, marker_count);
	   	printf("fd= %x counter=%d-first byte=%x.%x.%x.%x\n",fd,counter,buf[0],buf[1],buf[2],buf[3]);
	}

	if (out !=stdout)
	{
		ftime(&marker2);
	   	fwrite(buf,len,1,out);
	   	ftime(&marker3);
	   	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);
	 	if ((counter%SKIP_COUNT)==0)
	   		printf("fd= %x counter=%d, write len=%d--time=%d msec\n",fd,counter,len, marker_count);
	}
    counter++;

// 	pthread_mutex_unlock(&mutex);

	return len;
}

int process_pes2(int fd, FILE *out, unsigned char *buf)
{
	int len;
	struct timeb marker2,marker3;
	int marker_count;
	static unsigned int counter = 0;

// 	pthread_mutex_lock(&mutex);

	ftime(&marker2);
	len = read(fd, buf, MAX_PES_SIZE);
	ftime(&marker3);

//        printf("%s(): fd=%d, buf=0x%x, MAX_PES_SIZE=%d\n",__func__, fd, buf, MAX_PES_SIZE);
	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);


	if (len < 0) {
		if (errno == EOVERFLOW) {
			fprintf(stderr, "counter=%d--read error: buffer overflow (%d)\n",counter,EOVERFLOW);
			return 0;
		}
		else {
            printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,len, errno);
			perror("read");
			exit(1);
		}
	}

	if ( (counter%SKIP_COUNT)==0) {
    	printf("fd= %x counter=%d-read len=%d--time=%d msec\n",fd,counter,len, marker_count);
	   	printf("fd= %x counter=%d-first byte=%x.%x.%x.%x\n",fd,counter,buf[0],buf[1],buf[2],buf[3]);
	}

	if (out !=stdout)
	{
		ftime(&marker2);
	   	fwrite(buf,len,1,out);
	   	ftime(&marker3);
	   	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);
	 	if ( (counter%SKIP_COUNT)==0)
	   		printf("fd= %x counter=%d, write len=%d--time=%d msec\n",fd,counter,len, marker_count);
	}
    counter++;

// 	pthread_mutex_unlock(&mutex);

	return len;
}

int process_pes3(int fd, FILE *out, unsigned char *buf)
{
	int len;
	struct timeb marker2,marker3;
	int marker_count;
	static unsigned int counter = 0;

// 	pthread_mutex_lock(&mutex);

	ftime(&marker2);
	len = read(fd, buf, MAX_PES_SIZE);
	ftime(&marker3);

//        printf("%s(): fd=%d, buf=0x%x, MAX_PES_SIZE=%d\n",__func__, fd, buf, MAX_PES_SIZE);
	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);


	if (len < 0) {
		if (errno == EOVERFLOW) {
			fprintf(stderr, "counter=%d--read error: buffer overflow (%d)\n",counter,EOVERFLOW);
			return 0;
		}
		else {
            printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,len, errno);
			perror("read");
			exit(1);
		}
	}

	if ( (counter%SKIP_COUNT)==0) {
    	printf("fd= %x counter=%d-read len=%d--time=%d msec\n",fd,counter,len, marker_count);
	   	printf("fd= %x counter=%d-first byte=%x.%x.%x.%x\n",fd,counter,buf[0],buf[1],buf[2],buf[3]);
	}

	if (out !=stdout)
	{
		ftime(&marker2);
	   	fwrite(buf,len,1,out);
	   	ftime(&marker3);
	   	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);
	 	if ( (counter%SKIP_COUNT)==0)
	   		printf("fd= %x counter=%d, write len=%d--time=%d msec\n",fd,counter,len, marker_count);
	}
    counter++;

// 	pthread_mutex_unlock(&mutex);

	return len;
}

int process_pes4(int fd, FILE *out, unsigned char *buf)
{
	int len;
	struct timeb marker2,marker3;
	int marker_count;
	static unsigned int counter = 0;

// 	pthread_mutex_lock(&mutex);

	ftime(&marker2);
	len = read(fd, buf, MAX_PES_SIZE);
	ftime(&marker3);

//        printf("%s(): fd=%d, buf=0x%x, MAX_PES_SIZE=%d\n",__func__, fd, buf, MAX_PES_SIZE);
	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);


	if (len < 0) {
		if (errno == EOVERFLOW) {
			fprintf(stderr, "counter=%d--read error: buffer overflow (%d)\n",counter,EOVERFLOW);
			return 0;
		}
		else {
            printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,len, errno);
			perror("read");
			exit(1);
		}
	}

	if ( (counter%SKIP_COUNT)==0) {
    	printf("fd= %x counter=%d-read len=%d--time=%d msec\n",fd,counter,len, marker_count);
	   	printf("fd= %x counter=%d-first byte=%x.%x.%x.%x\n",fd,counter,buf[0],buf[1],buf[2],buf[3]);
	}

	if (out !=stdout)
	{
		ftime(&marker2);
	   	fwrite(buf,len,1,out);
	   	ftime(&marker3);
	   	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);
	 	if ( (counter%SKIP_COUNT)==0)
	   		printf("fd= %x counter=%d, write len=%d--time=%d msec\n",fd,counter,len, marker_count);
	}
    counter++;

// 	pthread_mutex_unlock(&mutex);

	return len;
}

int process_pes5(int fd, FILE *out, unsigned char *buf)
{
	int len;
	struct timeb marker2,marker3;
	int marker_count;
	static unsigned int counter = 0;

// 	pthread_mutex_lock(&mutex);

	ftime(&marker2);
	len = read(fd, buf, MAX_PES_SIZE);
	ftime(&marker3);

//        printf("%s(): fd=%d, buf=0x%x, MAX_PES_SIZE=%d\n",__func__, fd, buf, MAX_PES_SIZE);
	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);


	if (len < 0) {
		if (errno == EOVERFLOW) {
			fprintf(stderr, "counter=%d--read error: buffer overflow (%d)\n",counter,EOVERFLOW);
			return 0;
		}
		else {
            printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,len, errno);
			perror("read");
			exit(1);
		}
	}

	if ( (counter%SKIP_COUNT)==0) {
    	printf("fd= %x counter=%d-read len=%d--time=%d msec\n",fd,counter,len, marker_count);
	   	printf("fd= %x counter=%d-first byte=%x.%x.%x.%x\n",fd,counter,buf[0],buf[1],buf[2],buf[3]);
	}

	if (out !=stdout)
	{
		ftime(&marker2);
	   	fwrite(buf,len,1,out);
	   	ftime(&marker3);
	   	marker_count = (marker3.time-marker2.time)*1000 + (marker3.millitm-marker2.millitm);
	 	if ( (counter%SKIP_COUNT)==0)
	   		printf("fd= %x counter=%d, write len=%d--time=%d msec\n",fd,counter,len, marker_count);
	}
    counter++;

// 	pthread_mutex_unlock(&mutex);

	return len;
}

int set_pes_buffer_size( int fd)
{
		return ioctl(fd, DMX_SET_BUFFER_SIZE, MAX_DMX_BUFFER_SIZE);
}

int set_filter(int fd, unsigned int pid)
{
	struct dmx_pes_filter_params f;

	f.pid = (unsigned short) pid;
	f.input = DMX_IN_FRONTEND;
	f.output = DMX_OUT_TAP;
	f.pes_type = DMX_PES_OTHER;
	f.flags = DMX_IMMEDIATE_START;


//printf("%s():f->flags=0x%x\n",__func__, DMX_SET_PES_FILTER);

	if (ioctl(fd, DMX_SET_PES_FILTER, &f) == -1) {
		perror("ioctl DMX_SET_PES_FILTER");
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
}/**/

int flush_pids(int fd)
{
    unsigned short flush_pids=1;

//	ioctl(fd,DMX_FLUSH_PES_PIDS , flush_pids);

    return 0;
}

void *threadfunc0(void *parm)
{
  	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	//unsigned char	*tmpfile,*fptr;
	int written=0;
    int bytes;
	FILE *out = stdout;
	static unsigned int counter = 0;

	//debug_Aaron
	int ret;

 	printf("%s, id=%d fd=%d output file=%s\n", p->string, p->id, p->fd, p->outfile);

	buf=malloc(MAX_PES_SIZE);
  	if (buf==NULL)
  	{
	    printf("malloc error\n");
		exit(1);
    }

#if 0
	tmpfile=malloc(MAX_FILE_SIZE);
  	if (tmpfile==NULL)
  	{
	    printf("malloc error\n");
		exit(1);
    }

	fptr = tmpfile;

	out = fopen(p->outfile, "wb");
	if (!out) {
		perror("open out file ");
//		exit(1);
	}
#endif

	for (;;) {
		//bytes=process_pes0(p->fd, stdout, buf);
		bytes = read(p->fd, buf, MAX_PES_SIZE);
		if (bytes < 0) {
			if (errno == EOVERFLOW) {
				fprintf(stderr, "iii=%d--read error: buffer overflow (%d)\n",iii,EOVERFLOW);
			}
			else {
	            printf("%s(): read TS device error rd_count=%d, errno=%d\n",__func__,bytes, errno);
				perror("read");
			}
//			return 0;
		}
		else {
		    if ( (counter%SKIP_COUNT)==0) {
	   			printf("fd= %x iii=%d--length=%d first byte=%x.%x.%x.%x\n",
	   			        p->fd,counter,bytes,buf[0],buf[1],buf[2],buf[3]);
	   		}
		}

#if 1 
		//debug_aaron
		 ret = cs_UdpSend(snd_sock, &recipient, buf, bytes);
         if (ret == -1)
         {
               printf("sendto() failed; %d\n", errno);
               break;
         }
#endif

        written+=bytes;
		counter++;
#if 0 
        if ((out>0) && (bytes>0))
        {
	   		fwrite(buf,bytes,1,out);
            if ( (counter%SKIP_COUNT)==0 ) {
		        printf("%s():decive id =%d =>counter=%d-written=%d bytes (%d MB), bytes=%d\n",
		        		__func__,p->id,counter, written,written/MB, bytes);
		    }

			if (written >(MAX_FILE_SIZE))
	        {
	            printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
				break;
	        }
        }
		counter++;
		if (written >(MAX_FILE_SIZE))
	    {
	        printf("Read file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
			break;
	    }
#endif

//		memcpy(fptr,buf,bytes);
//		fptr= fptr + bytes;


	}


	if (out)
	{
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
	static int 		written=0;
    int 			bytes;
	FILE 			*out = stdout;
	static unsigned int counter = 0;

 	printf("%s, id=%d fd=%d\n", p->string, p->id, p->fd);

	buf=malloc(MAX_PES_SIZE);
  	if (buf==NULL)
  	{
		printf("malloc error\n");
	    exit(1);
    }

	for (;;) {
//		process_pes(dmxfd[p->id],stdout,buf);
		bytes=process_pes1(p->fd, stdout, buf);
        if (bytes>0)
        {
            written+=bytes;
            if ( (counter%SKIP_COUNT)==0 && out!=stdout) {
		        printf("%s():decive id =%d =>counter=%d-written=%d bytes (%d MB), bytes=%d\n",
		        		__func__,p->id,counter, written,written/MB, bytes);
		    }
        }
		if (written >(MAX_FILE_SIZE) && out!=stdout)
        {
            printf("file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
			break;
        }/*if*/
		counter++;
	}

	if (out !=stdout)
	{
	    fflush(out);
	    fclose(out);
	}//if

  	free(p);

  	return 0;
}

void *threadfunc2(void *parm)
{
  	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	static int 		written=0;
    int 			bytes;
	FILE 			*out = stdout;
	static unsigned int counter = 0;

 	printf("%s, id=%d fd=%d\n", p->string, p->id, p->fd);

	buf=malloc(MAX_PES_SIZE);
  	if (buf==NULL)
  	{
		printf("malloc error\n");
	    exit(1);
    }

	for (;;) {
//		process_pes(dmxfd[p->id],stdout,buf);
		bytes=process_pes2(p->fd, stdout, buf);
        if (bytes>0)
        {
            written+=bytes;
            if ( (counter%SKIP_COUNT)==0 && out!=stdout) {
		        printf("%s():decive id =%d =>counter=%d-written=%d bytes (%d MB), bytes=%d\n",
		        		__func__,p->id,counter, written,written/MB, bytes);
		    }
        }
		if (written >(MAX_FILE_SIZE) && out!=stdout)
        {
            printf("file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
			break;
        }/*if*/
		counter++;
	}

	if (out !=stdout)
	{
	    fflush(out);
	    fclose(out);
	}//if

  	free(p);

  	return 0;
}

void *threadfunc3(void *parm)
{
  	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	static int 		written=0;
    int 			bytes;
	FILE 			*out = stdout;
	static unsigned int counter = 0;

 	printf("%s, id=%d fd=%d\n", p->string, p->id, p->fd);

	buf=malloc(MAX_PES_SIZE);
  	if (buf==NULL)
  	{
		printf("malloc error\n");
	    exit(1);
    }

	for (;;) {
//		process_pes(dmxfd[p->id],stdout,buf);
		bytes=process_pes3(p->fd, stdout, buf);
        if (bytes>0)
        {
            written+=bytes;
            if ( (counter%SKIP_COUNT)==0 && out!=stdout) {
		        printf("%s():decive id =%d =>counter=%d-written=%d bytes (%d MB), bytes=%d\n",
		        		__func__,p->id,counter, written,written/MB, bytes);
		    }
        }
		if (written >(MAX_FILE_SIZE) && out!=stdout)
        {
            printf("file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
			break;
        }/*if*/
		counter++;
	}

	if (out !=stdout)
	{
	    fflush(out);
	    fclose(out);
	}//if

  	free(p);

  	return 0;
}

void *threadfunc4(void *parm)
{
  	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	static int 		written=0;
    int 			bytes;
	FILE 			*out = stdout;
	static unsigned int counter = 0;

 	printf("%s, id=%d fd=%d\n", p->string, p->id, p->fd);

	buf=malloc(MAX_PES_SIZE);
  	if (buf==NULL)
  	{
		printf("malloc error\n");
	    exit(1);
    }

	for (;;) {
//		process_pes(dmxfd[p->id],stdout,buf);
		bytes=process_pes4(p->fd, stdout, buf);
        if (bytes>0)
        {
            written+=bytes;
            if ( (counter%SKIP_COUNT)==0 && out!=stdout) {
		        printf("%s():decive id =%d =>counter=%d-written=%d bytes (%d MB), bytes=%d\n",
		        		__func__,p->id,counter, written,written/MB, bytes);
		    }
        }
		if (written >(MAX_FILE_SIZE) && out!=stdout)
        {
            printf("file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
			break;
        }/*if*/
		counter++;
	}

	if (out !=stdout)
	{
	    fflush(out);
	    fclose(out);
	}//if

  	free(p);

  	return 0;
}

void *threadfunc5(void *parm)
{
  	thread_parm_t   *p = (thread_parm_t *)parm;
	unsigned char	*buf;
	static int 		written=0;
    int 			bytes;
	FILE 			*out = stdout;
	static unsigned int counter = 0;

 	printf("%s, id=%d fd=%d\n", p->string, p->id, p->fd);

	buf=malloc(MAX_PES_SIZE);
  	if (buf==NULL)
  	{
		printf("malloc error\n");
	    exit(1);
    }

	for (;;) {
//		process_pes(dmxfd[p->id],stdout,buf);
		bytes=process_pes5(p->fd, stdout, buf);
        if (bytes>0)
        {
            written+=bytes;
            if ( (counter%SKIP_COUNT)==0 && out!=stdout) {
		        printf("%s():decive id =%d =>counter=%d-written=%d bytes (%d MB), bytes=%d\n",
		        		__func__,p->id,counter, written,written/MB, bytes);
		    }
        }
		if (written >(MAX_FILE_SIZE) && out!=stdout)
        {
            printf("file size: %d Mbytes--Done\n",(MAX_FILE_SIZE)/MB);
			break;
        }/*if*/
		counter++;
	}

	if (out !=stdout)
	{
	    fflush(out);
	    fclose(out);
	}//if

  	free(p);

  	return 0;
}

int main(int argc, char *argv[])
{
	int cfg_fd;
	int i,j;
	unsigned int pid;
    unsigned char *buf;
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
	FILE *out = stdout;
	char filename[128];
	int write_to_file=0;
	int written=0;
    int bytes;
    int read_ts=0;
	unsigned char	line_buf[128];
	int	ret;
	pthread_t             thread[12];
	int                   rc=0;
//	pthread_attr_t        pta;
	thread_parm_t         *parm=NULL;


    printf("test_pes v0.99-(date:20101031)\n");
	printf("buffer length=%d\n", MAX_PES_SIZE);

	/* open DEMUX device */
	for (i=0; i<MXA_DMUX_DEVICE; i++) {
		fprintf(stderr, "open device: %s\n", dmxdev[i]);
		if ((dmxfd[i] = open(dmxdev[i], O_RDWR)) < 0)
		{
			perror("open");
			return 1;
		}
		printf("test_pes: using '%s' fd=%d\n", dmxdev[i],dmxfd[i]);
	}

	/* open TS configuration file */
	if ((cfg_fd = config_file_open()) < 0)
	{
			perror("open");
			return 1;
	}


	/* parsing TS configuration file */
	ts_num =0;
	while (1)
	{
		char *token;


		ret = config_file_read_line(cfg_fd, line_buf, sizeof(line_buf));
		if ( ret < 0 )
		{
			break;
		}
        token = strtok((char *)line_buf," "); /*There are two delimiters here*/
       	if (memcmp(token,"#",1)==0)
       		continue;

		if (token != NULL)
			ts_rxq_pid[ts_num].id = str2int(token);

        token = strtok(NULL," ");
		if (token != NULL)
			strcpy(ts_rxq_pid[ts_num].filename,token);

        token = strtok(NULL," ");

		i = 0;
        while (token != NULL){
//        	printf("The token is:  %s\n", token);
			ts_rxq_pid[ts_num].pid[i] = str2int(token);
            token = strtok(NULL," ");
            i++;
        }
		ts_rxq_pid[ts_num].pid_num = i;
		ts_num++;
	}

	/* display the content of TS configuration file */
	for (i=0; i<ts_num; i++)
	{
		printf("id = %d ",ts_rxq_pid[i].id);
		printf("file name = %s  PID=",ts_rxq_pid[i].filename);
		for (j=0; j<ts_rxq_pid[i].pid_num; j++)
		{
			printf(" %d ",ts_rxq_pid[i].pid[j]);
		}
		printf("\n");
	}

	/* input arguments from command line */
	if (cfg_fd < 0) {
		if (argc == 1)
	 		usage();

		for (i=1; i<argc; i++)
		{

			// ----------------------------------------------------
			// -h, --help, -v, --version:
			// ----------------------------------------------------
			if ( (strcmp(argv[i], "-h")== 0) ||
			 (strcmp(argv[i], "--help")== 0) ||
			 (strcmp(argv[i], "-v")== 0) ||
			 (strcmp(argv[i], "--version")== 0))
			{
				usage();
				exit( 0 );
			}
			else if (strcmp(argv[i], "-r")== 0)
			{
				read_ts = 1;
			}
			else if (strcmp(argv[i], "-pid")== 0)
			{
			    //flush all of pids when we want to set it
			    flush_pids(dmxfd[0]);
				while( (++i<argc) && (pid = strtoul(argv[i],NULL, 0))>=0)
				{
					if (argv[i][0]=='-')
					{
						i--;
						break;
					}
					printf("set PID=[0x%04x]-",pid);
					if (pid > 0x1fff)
						usage();
					if (set_filter(dmxfd[0], pid) != 0)
					{
						printf("FAIL\n");
						return 1;
					}
					else
						printf("OK\n");
				}/*while*/
			}
			else if (strcmp(argv[i], "-f")==0)
			{
				i++;
				write_to_file=1;
				strcpy(filename, argv[i]);
				printf("Using file name %s\n", filename);
			}
			else if (strcmp(argv[i], "-d")==0)
			{
				printf("Delete all PID..\n");
			}
			else if (strcmp(argv[i], "-l")==0)
			{
				printf("list all PID..\n");
				get_pes_pids(dmxfd[0]);
			}/*if-else*/
		}/*while*/

		if (write_to_file)
		{
			out = fopen(filename, "wb");
			if (!out) {
				perror("open output file");
				exit(1);
			}
		}/*if*/


	    if (read_ts==0)
	    	exit(0);

	}

	/* set receive buffer size for each TS port */
	for (i=0; i<1; i++) {
		set_pes_buffer_size(dmxfd[i]);
	}

	/* set filter pids */
	for (i=0; i<6; i++) {
		if (ts_rxq_pid[i].pid_num <= 0)
			continue;

		for (j=0; j<ts_rxq_pid[i].pid_num; j++) {
			if (set_filter(dmxfd[ts_rxq_pid[i].id], ts_rxq_pid[i].pid[j]) != 0)
			{
				printf("Queue id=%d   pid=%d ... Fail.\n",ts_rxq_pid[i].id,ts_rxq_pid[i].pid[j]);
				return 1;
			}
		}
	}

//	for (i=0; i<12; i++) {
//	  	printf("Queue id = %d allocating read buffer %d bytes\n",i,MAX_PES_SIZE);
	  	buf=malloc(MAX_PES_SIZE);
	  	if (buf==NULL)
	  	{
	      printf("malloc error\n");
	      exit(1);
	    }
//	}

	 //debug_Aaron
        snd_sock = cs_UdpSocketInit(PEER_IP, PEER_PORT, &recipient);
        if (snd_sock < 0)
        {
                printf("UDP socket initialize failed!!!\r\n");
                exit(-1);
        }
	
	/* create threads to receive TS packets */
//  	rc = pthread_attr_init(&pta);
	parm = malloc(sizeof(thread_parm_t));
	parm->fd = dmxfd[0];
	parm->id = 0;
	strcpy(parm->outfile,ts_rxq_pid[parm->id].filename);
	strcpy(parm->string, "TS Channel 0 Read......");
	rc = pthread_create( &thread[0], NULL, threadfunc0, (void *)parm);

#if 0
	parm = malloc(sizeof(thread_parm_t));
	parm->fd = dmxfd[1];
	parm->id = 1;
	strcpy(parm->string, "TS Channel 1 Read......");
	rc = pthread_create( &thread[1], NULL, threadfunc1, (void *)parm);

	parm = malloc(sizeof(thread_parm_t));
	parm->fd = dmxfd[2];
	parm->id = 2;
	strcpy(parm->string, "TS Channel 2 Read......");
	rc = pthread_create( &thread[2], NULL, threadfunc2, (void *)parm);

	parm = malloc(sizeof(thread_parm_t));
	parm->fd = dmxfd[3];
	parm->id = 3;
	strcpy(parm->string, "TS Channel 3 Read......");
	rc = pthread_create( &thread[3], NULL, threadfunc3, (void *)parm);

	parm = malloc(sizeof(thread_parm_t));
	parm->fd = dmxfd[4];
	parm->id = 4;
	strcpy(parm->string, "TS Channel 4 Read......");
	rc = pthread_create( &thread[4], NULL, threadfunc4, (void *)parm);

	parm = malloc(sizeof(thread_parm_t));
	parm->fd = dmxfd[5];
	parm->id = 5;
	strcpy(parm->string, "TS Channel 5 Read......");
	rc = pthread_create( &thread[5], NULL, threadfunc5, (void *)parm);
#endif
	while (1);

	for (i=0; i<MXA_DMUX_DEVICE; i++) {
		close(dmxfd[0]);
	}

	return 0;
}
