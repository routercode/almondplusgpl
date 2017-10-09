#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>

#define PROC_STAT	"/proc/stat"
#define NF_CONNTRACK	"/proc/net/nf_conntrack"
#define LINE_BUF_SIZE	2048
#define MAX_CPU		2
#define THRES_L		5
#define THRES_H		10
#define THRES_L_ACT	10
#define THRES_H_ACT	1

typedef struct jiffy_counts_t {
	/* Linux 2.4.x has only first four */
	unsigned long long usr, nic, sys, idle;
	unsigned long long iowait, irq, softirq, steal;
	unsigned long long total;
	unsigned long long busy;
} jiffy_counts_t;

jiffy_counts_t curr_jif, prev_jif;
int num_cpus, active_cpus, idle_duration, busy_duration;


static int read_cpu_jiffy(FILE *fp, jiffy_counts_t *p_jif)
{

	unsigned char line_buf[LINE_BUF_SIZE];
	static const char fmt[] = "cp%*s %llu %llu %llu %llu %llu %llu %llu %llu";
	int ret;

	if (!fgets(line_buf, LINE_BUF_SIZE, fp) || line_buf[0] != 'c' /* not "cpu" */)
		return 0;
	ret = sscanf(line_buf, fmt,
			&p_jif->usr, &p_jif->nic, &p_jif->sys, &p_jif->idle,
			&p_jif->iowait, &p_jif->irq, &p_jif->softirq,
			&p_jif->steal);
	if (ret >= 4) {
		p_jif->total = p_jif->usr + p_jif->nic + p_jif->sys + p_jif->idle
			+ p_jif->iowait + p_jif->irq + p_jif->softirq + p_jif->steal;
		/* procps 2.x does not count iowait as busy time */
		p_jif->busy = p_jif->total - p_jif->idle - p_jif->iowait;
	}

	return ret;
}

int get_cpu_number(void)
{
	int count=0;
	FILE* fp;
	jiffy_counts_t tmp_jif;

	fp = fopen(PROC_STAT, "r");

	/* Read 1st line cpu */
	if (read_cpu_jiffy(fp, &tmp_jif) < 4){
		perror("can't read /proc/stat !!");
		return -1;
	}

	/* Count cpu # */
	while (1) {
		if (read_cpu_jiffy(fp, &tmp_jif) <= 4)
			break;
		count++;
	}
	fclose(fp);

	return count;
}

int check_pattern(char *str)
{
	char *pch;

	pch = strstr (str,"ASSURED");
	if(pch != NULL)
		return 1;

	pch = strstr (str,"ESTABLISHED");
	if(pch != NULL)
		return 1;

	return 0;
}

int get_connection_no(void)
{
	FILE *fp;
#define BUFFER_SIZE	300
	unsigned char tmp_buff[BUFFER_SIZE];
	int ret=0, line=0;


	fp = fopen(NF_CONNTRACK, "r");
	if(fp == NULL){
		perror ("Can't open /proc/net/nf_conntrack\n");
		return -1;
	}

	while(!feof(fp) && (line < 20)){
		memset(tmp_buff, 0, BUFFER_SIZE);
		fgets( tmp_buff, BUFFER_SIZE, fp);
		ret += check_pattern(tmp_buff);
		line++;
	}

	fclose(fp);

	return ret;
}


int main(int argc, char **argv)
{
	FILE* fp;
	unsigned char line_buf[LINE_BUF_SIZE];
	int jiff, busy_jiff, load, interval=1;
	int i, arg_cnt = argc;
	int thresh_low, thresh_high, duration1, duration2, nf_conntrack;

	thresh_low = THRES_L;
	thresh_high = THRES_H;
	duration1 = THRES_L_ACT;
	duration2 = 60;

	if((argc%2 == 0) || (argc > 9)){
		printf("\nUsage:\n");
		printf("\t -L : Low threshold\n");
		printf("\t -H : High threshold\n");
		printf("\t -D : Idle duration to remove 2nd CPU\n");
		printf("\t -S : Idle duration to standby\n");
		return 0;
	}

	i = 1;
	while(i < argc){
		switch (argv[i][1]){
		case 'L':
			thresh_low = atol(argv[i+1]);
			break;
		case 'H':
			thresh_high = atol(argv[i+1]);
			break;
		case 'D':
			duration1 = atol(argv[i+1]);
			break;
		case 'S':
			duration2 = atol(argv[i+1]);
			break;
		default:
			return 0;
		}
		i += 2;
	}
	printf("thresh_low: %d\n",thresh_low);
	printf("thresh_high: %d\n",thresh_high);
	printf("duration1: %d\n",duration1);
	printf("duration2: %d\n",duration2);


	num_cpus = 0;
	memset(&curr_jif, 0, sizeof(jiffy_counts_t));
	memset(&prev_jif, 0, sizeof(jiffy_counts_t));

	active_cpus = get_cpu_number();
	printf("Active CPU:%d\n",active_cpus);

	/*======== First read =========*/
	/* Read 1st line cpu */
	fp = fopen(PROC_STAT, "r");
	if (read_cpu_jiffy(fp, &curr_jif) < 4){
		printf("can't read /proc/stat !!");
		return -1;
	}

	fclose(fp);

	sleep(1);

	while(1){
		active_cpus = get_cpu_number();
		/* Open /proc/stat */
		fp = fopen(PROC_STAT, "r");
		if(!fp)
			perror("Open fail\n");

		/* Keep previous number */
		prev_jif = curr_jif;

		if (read_cpu_jiffy(fp, &curr_jif) < 4){
			perror("can't read /proc/stat !!");
			return -1;
		}

		sleep(1);
		fclose(fp);

		jiff = curr_jif.total - prev_jif.total;
		busy_jiff = curr_jif.busy - prev_jif.busy;
		load = (busy_jiff * 100) / jiff ;
		//printf("load:%d\n", load);

		if(load < thresh_low){
			idle_duration++;
			busy_duration = 0;
		}
		else{
			idle_duration = 0;
		}

		if(load > thresh_high){
			busy_duration++;
			idle_duration = 0;
		}
		else{
			busy_duration = 0;
		}

		if((idle_duration > duration1) && (active_cpus > 1))
			system("echo 0 > /sys/devices/system/cpu/cpu1/online");
		if((busy_duration > THRES_H_ACT) && (active_cpus < MAX_CPU))
			system("echo 1 > /sys/devices/system/cpu/cpu1/online");

		nf_conntrack = get_connection_no();
		//printf("nf_conntrack=%d\n",nf_conntrack);

		if((idle_duration > duration2) && (active_cpus > 0) && (nf_conntrack==0)){
			idle_duration = 0;
			busy_duration = 0;
			system("ethtool -s eth0 autoneg off");
			system("ethtool -s eth0 speed 10");
			system("ethtool -s eth1 autoneg off");
			system("ethtool -s eth1 speed 10");

			system("echo \"standby\" > /sys/power/state");
			sleep(5);
			system("ethtool -s eth0 autoneg on");
			system("ethtool -s eth1 autoneg on");
		}
	}

	return 0;
}
