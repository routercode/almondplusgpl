/*-
 * Copyright (C) 2011 Encore Interactive Inc.
 * All rights reserved.
 */

#include <sys/types.h>
#include <sys/ioctl.h>

#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef __NetBSD__
#include <dev/dtv/dtvio.h>
#else
#include <linux/dvb/frontend.h>
#include <linux/dvb/dmx.h>
#endif

static void
usage(const char *pn)
{
	fprintf(stderr, "usage: %s frontend demux pid1 [pid2 ...]\n", pn);
	exit(EXIT_FAILURE);
}

static void
tune_atsc(int fd, uint32_t freq, fe_modulation_t modulation)
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

//		printf("%s: status = 0x%x\n", __func__, status);
		if (status & FE_HAS_LOCK)
			break;
		sleep(1);
	}
}

static void
set_filter(int fd, int argc, char *argv[])
{
	struct dmx_pes_filter_params params;
	int n = 0;

	memset(&params, 0, sizeof(params));
	params.pid = strtoul(argv[n++], NULL, 0);
	params.input = DMX_IN_FRONTEND;
	params.output = DMX_OUT_TSDEMUX_TAP;
	params.pes_type = DMX_PES_OTHER;
	params.flags = 0;

	ioctl(fd, DMX_SET_BUFFER_SIZE, 1024*1024);

	if (ioctl(fd, DMX_SET_PES_FILTER, &params) == -1) {
		fprintf(stderr, "ioctl DMX_SET_PES_FILTER failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (n < argc) {
		uint16_t pid;

		pid = strtoul(argv[n++], NULL, 0);
		if (ioctl(fd, DMX_ADD_PID, &pid) == -1) {
			fprintf(stderr, "ioctl DMX_ADD_PID failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}

	if (ioctl(fd, DMX_START, NULL) == -1) {
		fprintf(stderr, "ioctl DMX_START failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

static void
ratetest(int fd)
{
	struct timeval now, last;
	uint64_t nbytes;
	uint8_t *buf;
	size_t bufsz = 188*1024;
	ssize_t len;

	buf = malloc(bufsz);
	if (buf == NULL) {
		fprintf(stderr, "malloc failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	gettimeofday(&now, NULL);
	last = now;
	nbytes = 0;

	for (;;) {
		len = read(fd, buf, bufsz);
		if (len < 0) {
			if (errno == EOVERFLOW) {
				printf(" [overflow]\n");
				continue;
			}
			perror("read");
			break;
		}
		gettimeofday(&now, NULL);
		if (now.tv_sec > last.tv_sec) {
			double d = ((double)nbytes * 8) / (1024 * 1024);
			printf("~%.04f Mbps\n", d);
			nbytes = 0;
		}

		nbytes += len;
		last = now;
	}
}

int
main(int argc, char *argv[])
{
	const char *pn = argv[0];
	int demux_fd = -1, frontend_fd = -1;

	if (argc < 4)
		usage(pn);
		/* NOTREACHED */

	frontend_fd = open(argv[1], O_RDWR);
	if (frontend_fd == -1) {
		fprintf(stderr, "couldn't open %s: %s\n", argv[1], strerror(errno));
		return EXIT_FAILURE;
	}
	demux_fd = open(argv[2], O_RDWR);
	if (demux_fd == -1) {
		fprintf(stderr, "couldn't open %s: %s\n", argv[2], strerror(errno));
		return EXIT_FAILURE;
	}
	argc -= 3;
	argv += 3;

	tune_atsc(frontend_fd, 63000000, QAM_256);
	set_filter(demux_fd, argc, argv);

	ratetest(demux_fd);

	return EXIT_SUCCESS;
}
