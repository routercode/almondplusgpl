#include <stdio.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#define O_RDWR				2
#define CS_QOS_SET_MINOR	246
#define CS_QOS_SET_DEV		"/dev/cs_qos_set"

typedef enum {
	CS_QOS_SET_CMD_ADD,
	CS_QOS_SET_CMD_DEL,
	CS_QOS_SET_CMD_PRINT,
	CS_QOS_SET_CMD_TEST,
	CS_QOS_SET_CMD_TOTAL
} cs_qos_set_cmd_e;

typedef struct cs_qos_mapping {
	// use cs_int8 instead of cs_uint8 for cmd and ingress_port is
	// to use negative nubmer to indicate error
	u_int8_t  cmd;
	int8_t  ingress_port;
	int8_t	ingress_pri;
	int8_t	ingress_dscp;
	int8_t	egress_pri;
	int8_t	egress_dscp;
} cs_qos_mapping_s;


int main(int argc, char *argv[])
{
	int i = 1;
	int dev_fd;
	int retval;
	unsigned int cmd;

	cs_qos_mapping_s qos_entry;
	qos_entry.cmd = -1;
	qos_entry.ingress_port = -1;
	qos_entry.ingress_pri = -1;
	qos_entry.ingress_dscp = -1;
	qos_entry.egress_pri = -1;
	qos_entry.egress_dscp = -1;

	if (argc <2) {
		fprintf(stderr, "no input argument!\n");
		return -1;
	}
	dev_fd = open(CS_QOS_SET_DEV, O_RDWR);

	if (dev_fd < 0) {
		fprintf(stderr, "Open dev %s fail!\n", CS_QOS_SET_DEV);
		return -1;
	}

	while (i<argc) {
		if (strcmp(argv[i], "-a") == 0) {
			qos_entry.cmd = CS_QOS_SET_CMD_ADD;
			i++;
		} else if (strcmp(argv[i], "-d") == 0) {
			qos_entry.cmd = CS_QOS_SET_CMD_DEL;
			i++;
		} else if (strcmp(argv[i], "-s") == 0) {
			qos_entry.cmd = CS_QOS_SET_CMD_PRINT;
			i++;
		} else if (strcmp(argv[i], "-i_p") == 0) {
			qos_entry.ingress_pri = atoi(argv[i+1]);
			i+=2;
		} else if (strcmp(argv[i], "-i_d") == 0) {
			qos_entry.ingress_dscp = atoi(argv[i+1]);
			i+=2;
		} else if (strcmp(argv[i], "-p") == 0) {
			qos_entry.ingress_port = atoi(argv[i+1]);
			i+=2;
		} else if (strcmp(argv[i], "-e_p") == 0) {
			qos_entry.egress_pri = atoi(argv[i+1]);
			i+=2;
		} else if (strcmp(argv[i], "-e_d") == 0) {
			qos_entry.egress_dscp = atoi(argv[i+1]);
			i+=2;
		} else
			i++;
	}

	if (qos_entry.cmd <0 || qos_entry.ingress_port <0) {
		fprintf(stderr, "Ingress command or port is incorrect!\n");
		close(dev_fd);
		return 0;
	}
	cmd =qos_entry.cmd;

	retval = ioctl(dev_fd, cmd, &qos_entry);
	if (retval)
		fprintf(stderr, "\nioctl ret %d\n", retval);
	close(dev_fd);
	return 0;
}

