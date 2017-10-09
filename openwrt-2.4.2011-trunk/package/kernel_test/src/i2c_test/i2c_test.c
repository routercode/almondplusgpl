/*
 * FILE NAME i2c_test.c
 *
 *  i2c_test.c - A user-space program to read/write an I2C register.
 *
 *  Copyright 2011 Cortina , Corp.
 *
 *  Based on i2cget.c - A user-space program to read an I2C register.
 *
 *  This program is free software; you can redistribute  it and/or modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#define ENDURANCE_EN

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef ENDURANCE_EN
#include <sys/time.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

static int i2c_register_read(int file, int phy_addr, int size, int reg_idx,
	unsigned int *reg_val, unsigned int iter, int idx_size, int val_size)
{
	int i;
	unsigned int tmp = 0;
	unsigned char idx_buf[4], val_buf[4];
	struct i2c_msg msg[2];
	struct i2c_ioctl_rdwr_data {
 		struct i2c_msg *msgs;  /* ptr to array of simple messages */
	    	int nmsgs;             /* number of messages to exchange */
	} msgst;

#ifdef ENDURANCE_EN
	struct timeval tvs, tve;
	struct timezone tz;

	gettimeofday(&tvs, &tz);
#endif

	//bar();

	for (i = 0; i < iter; i++) {
		memset(val_buf, 0, 4);

		if (idx_size < 0)
			idx_size = size;

		switch (idx_size) {
		case 1:
			idx_buf[0] = reg_idx & 0x000000FF;
			break;
		case 2:
			idx_buf[0] = (reg_idx & 0x0000FF00) >> 8;
			idx_buf[1] = (reg_idx & 0x000000FF);
			break;
		case 3:
			idx_buf[0] = (reg_idx & 0x00FF0000) >> 16;
			idx_buf[1] = (reg_idx & 0x0000FF00) >> 8;
			idx_buf[2] = (reg_idx & 0x000000FF);
			break;
		case 4:
			idx_buf[0] = (reg_idx & 0xFF000000) >> 24;
			idx_buf[1] = (reg_idx & 0x00FF0000) >> 16;
			idx_buf[2] = (reg_idx & 0x0000FF00) >> 8;
			idx_buf[3] = (reg_idx & 0x000000FF);
			break;
		}

		msg[0].addr = phy_addr;
		msg[0].flags = 0;
		if (phy_addr > 0x7F) msg[0].flags |= I2C_M_TEN;
		msg[0].len = idx_size;
		msg[0].buf = idx_buf;

		msg[1].addr = phy_addr;
		msg[1].flags = I2C_M_RD | I2C_M_NOSTART;
		if (phy_addr > 0x7F) msg[1].flags |= I2C_M_TEN;
		msg[1].len = (val_size >= 0) ? val_size : size;
		msg[1].buf = val_buf;

		msgst.msgs = msg;
		msgst.nmsgs = 2;

		if (ioctl(file, I2C_RDWR, &msgst) < 0) {
			fprintf(stderr, "Error: Transaction failed: %s\n", strerror(errno));
			return 1;
		}

		if (val_size < 0)
			val_size = size;

		switch (val_size) {
		case 1:
			*reg_val = val_buf[0];
			break;
		case 2:
			*reg_val = ((unsigned int)val_buf[0]) << 8;
			*reg_val |= (unsigned int)val_buf[1];
			break;
		case 3:
			*reg_val = ((unsigned int)val_buf[0]) << 16;
			*reg_val |= ((unsigned int)val_buf[1]) << 8;
			*reg_val |= (unsigned int)val_buf[2];
			break;
		case 4:
			*reg_val = ((unsigned int)val_buf[0]) << 24;
			*reg_val |= ((unsigned int)val_buf[1]) << 16;
			*reg_val |= ((unsigned int)val_buf[2]) << 8;
			*reg_val |= (unsigned int)val_buf[3];
			break;
		}

		if (i == 0) {
			printf("read phy 0x%X register 0x%X = 0x%X\n", phy_addr,
								reg_idx, *reg_val);
			tmp = *reg_val;
		} else {
			if (*reg_val != tmp) {
				printf("at %d time, read phy 0x%X register 0x%X = 0x%X(!= 0x%X)\n",
						i + 1, phy_addr, reg_idx, *reg_val, tmp);
				break;
			}
		}
	}

#ifdef ENDURANCE_EN
	if ((iter > 1) && (i == iter)) {
		gettimeofday(&tve, &tz);
		printf("%d times read took %d us\n", iter, tve.tv_sec * 1000000
			+ tve.tv_usec - (tvs.tv_sec * 1000000 + tvs.tv_usec));
	}
#endif

	return 0;
}

static int i2c_register_write(int file, int phy_addr, int size, int reg_idx,
	unsigned int reg_val, unsigned int iter, int idx_size, int val_size)
{
	int i;
	unsigned char idx_buf[4], val_buf[4];
	struct i2c_msg msg[2];
	struct i2c_ioctl_rdwr_data {
 		struct i2c_msg *msgs;  /* ptr to array of simple messages */
	    	int nmsgs;             /* number of messages to exchange */
	} msgst;

#ifdef ENDURANCE_EN
	struct timeval tvs, tve;
	struct timezone tz;

	gettimeofday(&tvs, &tz);
#endif

	//bar();

	for (i = 0; i < iter; i++) {

		if (idx_size < 0)
			idx_size = size;

		switch (idx_size) {
		case 1:
			idx_buf[0] = reg_idx & 0x000000FF;
			break;
		case 2:
			idx_buf[0] = (reg_idx & 0x0000FF00) >> 8;
			idx_buf[1] = (reg_idx & 0x000000FF);
			break;
		case 3:
			idx_buf[0] = (reg_idx & 0x00FF0000) >> 16;
			idx_buf[1] = (reg_idx & 0x0000FF00) >> 8;
			idx_buf[2] = (reg_idx & 0x000000FF);
			break;
		case 4:
			idx_buf[0] = (reg_idx & 0xFF000000) >> 24;
			idx_buf[1] = (reg_idx & 0x00FF0000) >> 16;
			idx_buf[2] = (reg_idx & 0x0000FF00) >> 8;
			idx_buf[3] = (reg_idx & 0x000000FF);
			break;
		}

		if (val_size < 0)
			val_size = size;

		switch (val_size) {
		case 1:
			val_buf[0] = reg_val & 0x000000FF;
			break;
		case 2:
			val_buf[0] = (reg_val & 0x0000FF00) >> 8;
			val_buf[1] = (reg_val & 0x000000FF);
			break;
		case 3:
			val_buf[0] = (reg_val & 0x00FF0000) >> 16;
			val_buf[1] = (reg_val & 0x0000FF00) >> 8;
			val_buf[2] = (reg_val & 0x000000FF);
			break;
		case 4:
			val_buf[0] = (reg_val & 0xFF000000) >> 24;
			val_buf[1] = (reg_val & 0x00FF0000) >> 16;
			val_buf[2] = (reg_val & 0x0000FF00) >> 8;
			val_buf[3] = (reg_val & 0x000000FF);
			break;
		}

		msg[0].addr = phy_addr;
		msg[0].flags = 0;
		if (phy_addr > 0x7F) msg[0].flags |= I2C_M_TEN;
		msg[0].len = idx_size;
		msg[0].buf = idx_buf;

		msg[1].addr = phy_addr;
		msg[1].flags = I2C_M_NOSTART;
		if (phy_addr > 0x7F) msg[1].flags |= I2C_M_TEN;
		msg[1].len = (val_size >= 0) ? val_size : size;
		msg[1].buf = val_buf;

		msgst.msgs = msg;
		msgst.nmsgs = 2;

		if (ioctl(file, I2C_RDWR, &msgst) < 0){
			fprintf(stderr, "Error: Transaction failed: %s\n", strerror(errno));
			return 1;
		}
	}


#ifdef ENDURANCE_EN
	if (iter > 1) {
		gettimeofday(&tve, &tz);
		printf("%d times write took %d us\n", iter, tve.tv_sec * 1000000
			+ tve.tv_usec - (tvs.tv_sec * 1000000 + tvs.tv_usec));
	}
	else
#endif
	printf("write phy 0x%X register 0x%X to 0x%X\n", phy_addr, reg_idx, reg_val);

	return 0;
}

static void print_usage(const char *prog)
{
	printf("Usage: %s -asrw [-bIVi] <param>\n", prog);
	puts("  Ex: i2c_test -a 59 -s 3 -o 0 -r\n"
	     "  -b --bus        bus(adapter) device\n"
	     "  -a --address    device phy address\n"
	     "  -s --size       register index/value size(byte)\n"
	     "  -o --offset     register offset\n"
	     "  -w --write      write value to register\n"
	     "  -r --read       read value from register\n"
	     "  -I --idx_size   register index size\n"
	     "  -V --val_size   register value size\n"
#ifdef ENDURANCE_EN
	     "  -i --iteration  iteration number\n"
#endif
	     "  Note: all input param value should be HEX format\n"
	     );
	exit(1);
}

static const struct option lopts[] = {
	{ "bus",        1, 0, 'b' },
	{ "address",    1, 0, 'a' },
	{ "size",       1, 0, 's' },
	{ "offset",     1, 0, 'o' },
	{ "idx_size",   1, 0, 'I' },
	{ "val_size",   1, 0, 'V' },
#ifdef ENDURANCE_EN
	{ "iteration",  1, 0, 'i' },
#endif
	{ "write",      1, 0, 'w' },
	{ "read",       0, 0, 'r' },
	{ NULL,         0, 0,  0  },
};

int main(int argc, char **argv)
{
	int c, res, address = -1, size = -1, index = -1, iter = 1, file;
	int idx_size = -1, val_size = -1;
	unsigned int value, write = 0xFF;
	char *filename = "/dev/i2c-0";

	/* parse parameters */
	while (1) {
#ifdef ENDURANCE_EN
		c = getopt_long(argc, argv, "b:a:s:o:I:V:i:w:r", lopts, NULL);
#else
		c = getopt_long(argc, argv, "b:a:s:o:I:V:w:r", lopts, NULL);
#endif

		if (c == -1)
			break;

		switch (c) {
		case 'b':
			filename = optarg;
			break;
		case 'a':
			address = strtoul(optarg, NULL, 16);
			break;
		case 's':
			size = atoi(optarg);
			break;
		case 'o':
			index = strtoul(optarg, NULL, 16);
			break;
		case 'I':
			idx_size = strtoul(optarg, NULL, 16);
			break;
		case 'V':
			val_size = strtoul(optarg, NULL, 16);
			break;
#ifdef ENDURANCE_EN
		case 'i':
			iter = strtoul(optarg, NULL, 16);
			break;
#endif
		case 'w':
			write = 1;
			value = strtoul(optarg, NULL, 16);
			break;
		case 'r':
			write = 0;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}

	if ((address < 0) || (write == 0xFF)) {
		print_usage(argv[0]);
		return -1;
	}
	if ((size <= 0 || size > 4) && (idx_size < 0) && (val_size < 0)) {
		print_usage(argv[0]);
		return -1;
	}

	file = open(filename, O_RDWR);

	if (file < 0)
		exit(1);

	if (write == 0)
		res = i2c_register_read(file, address, size, index, &value, iter,
								idx_size, val_size);
	else if (write == 1)
		res = i2c_register_write(file, address, size, index, value, iter,
								idx_size, val_size);
	close(file);

	return res;
}

