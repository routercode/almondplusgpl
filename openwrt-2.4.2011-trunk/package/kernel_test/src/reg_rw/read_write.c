/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * read_write.c
 *
 * $Id: read_write.c,v 1.5 2012/11/29 01:50:03 ewang Exp $
 *
 * Device Control Commands for Regsiter Read/Write Utility.
 *
 * $Log: read_write.c,v $
 * Revision 1.5  2012/11/29 01:50:03  ewang
 * BUG#37776 dos2unix
 *
 * Revision 1.4  2011/07/14 14:50:25  chsu
 * Add ioctl for NI port calendar.
 *
 *
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include "read_write.h"
#include <linux/fs.h>
#include <sys/types.h>
#include <asm-generic/fcntl.h>
#define CONFIG_MAC_NUM 0x1

int cli_show_all_commands(CLICMD_T * cmd);
CLICMD_T *cli_find_command(char *name, CLICMD_T * cmd);
static int gen_reg_help_cmd(int argc, char **argv);
static int gen_reg_read_cmd(int argc, char **argv);
static int gen_reg_read_help_cmd(int argc, char **argv);
static int gen_reg_write_cmd(int argc, char **argv);
static int gen_reg_write_help_cmd(int argc, char **argv);
static int reg_read_cmd(char argc, char **argv);
static int reg_write_cmd(char argc, char **argv);

static int gen_reg_mii_read_cmd(int argc, char **argv);
static int gen_reg_mii_read_help_cmd(int argc, char **argv);
static int gen_reg_mii_write_cmd(int argc, char **argv);
static int gen_ni_pol_cal_cmd(int argc, char **argv);
static int gen_reg_mii_write_help_cmd(int argc, char **argv);
static int gen_ni_pol_cal_help_cmd(int argc, char **argv);
static int mii_read_cmd(char argc, char **argv);
static int mii_write_cmd(char argc, char **argv);
static int ni_get_cmd(char argc, char **argv);
static int ni_set_cmd(char argc, char **argv);

u_int32_t str2value_1(u_int8_t * strp);
u_int32_t str2value_2(u_int8_t * strp);
u_int32_t str2decimal_1(u_int8_t * cp);
u_int32_t str2hex_1(u_int8_t * cp);
u_int32_t char2decimal_1(u_int8_t c);
u_int32_t char2hex_1(u_int8_t c);

#define isdigit_1(c)		(c >= '0' && c <= '9') ? 1 : 0
#define ishex_1(c)		((c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')) ? 1 : 0
#define isva1id_mem(c)		((c == 'C' || c == 'F' || c == 'B') || (c == 'c' || c == 'f' || c == 'b')) ? 1 : 0
int not_valid = 0;

CLICMD_T gen_reg_main_cmds[] = {
	{"?", gen_reg_help_cmd, "Help Commands"},
	{"help", gen_reg_help_cmd, "Help Commands"},
	{"read", gen_reg_read_cmd, "Read Registers"},
	{"write", gen_reg_write_cmd, "Write Registers"},
	{"mii", gen_reg_mii_read_cmd, "MII Read/Write Registers"},
	//{"mii_write",   gen_reg_mii_write_cmd,     "MII Write Registers"},
	{"ni", gen_ni_pol_cal_cmd, "NI Port Calendar"},
	{0, 0, 0}
};

CLICMD_T gen_reg_read_cmds[] = {
	{"?", gen_reg_read_help_cmd, "Help commands"},
	{"help", gen_reg_read_help_cmd, "Help commands"},
	{"mem", (void *)reg_read_cmd, "Read Registers"},
	{0, 0, 0}
};

CLICMD_T gen_reg_write_cmds[] = {
	{"?", gen_reg_write_help_cmd, "Help commands"},
	{"help", gen_reg_write_help_cmd, "Help commands"},
	{"mem", (void *)reg_write_cmd, "Write Registers"},
	{0, 0, 0}
};

CLICMD_T gen_reg_mii_read_cmds[] = {
	{"?", gen_reg_mii_read_help_cmd, "Help commands"},
	{"help", gen_reg_mii_read_help_cmd, "Help commands"},
	{"read", (void *)mii_read_cmd, "Read MII value"},
	{"write", (void *)mii_write_cmd, "Write Registers"},
	{0, 0, 0}
};

CLICMD_T gen_reg_mii_write_cmds[] = {
	{"?", gen_reg_mii_write_help_cmd, "Help commands"},
	{"help", gen_reg_mii_write_help_cmd, "Help commands"},
	{"mii", (void *)mii_write_cmd, "Write Registers"},
	{0, 0, 0}
};

CLICMD_T gen_ni_pol_cal_cmds[] = {
	{"?", gen_ni_pol_cal_help_cmd, "Help commands"},
	{"help", gen_ni_pol_cal_help_cmd, "Help commands"},
	{"get", (void *)ni_get_cmd, "Get NI port calendar"},
	{"set", (void *)ni_set_cmd, "Set NI port calendar"},
	{0, 0, 0}
};

CLICMD_T *cli_find_command(char *name, CLICMD_T * cmd)
{
	int i, total, len = strlen(name);
	CLICMD_T *me = NULL, *exact;

	exact = (CLICMD_T *) 0;
	total = 0;
	while (cmd->name) {
		char *srce = name;
		char *dest = cmd->name;
		for (i = 0; i < len; i++) {
			if (tolower(*dest) != tolower(*srce))
				break;
			dest++;
			srce++;
		}
		if (i == len) {
			if (i == strlen(cmd->name))
				exact = cmd;
			total++;
			me = cmd;
		}
		cmd++;
	}

	if (total > 1) {
		return ((CLICMD_T *) exact);	// duplicate
	}
	if (total == 1)
		return (me);
	else
		return ((CLICMD_T *) 0);
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_read_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int gen_reg_read_cmd(int argc, char **argv)
{
	CLICMD_T *cmd;
	int ret = -1;

	if (argc > 1) {
		cmd =
		    cli_find_command(argv[1],
				     (CLICMD_T *) & gen_reg_read_cmds[0]);
		if (cmd && cmd->action)
			ret = cmd->action(argc - 1, &argv[1]);	// skip set argument
		else {
			fprintf(stderr, "[%s] Command is not found!\n",
				argv[1]);
			gen_reg_read_help_cmd(argc - 1, &argv[1]);
		}
	} else
		gen_reg_read_help_cmd(argc - 1, &argv[1]);	// skip set argument

	return ret;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_write_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int gen_reg_write_cmd(int argc, char **argv)
{
	CLICMD_T *cmd;
	int ret = -1;

	if (argc > 1) {
		cmd =
		    cli_find_command(argv[1],
				     (CLICMD_T *) & gen_reg_write_cmds[0]);
		if (cmd && cmd->action)
			ret = cmd->action(argc - 1, &argv[1]);	// skip set argument
		else {
			fprintf(stderr, "[%s] Command is not found!\n",
				argv[1]);
			gen_reg_write_help_cmd(argc - 1, &argv[1]);
		}
	} else
		gen_reg_write_help_cmd(argc - 1, &argv[1]);	// skip set argument

	return ret;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_mii_read_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int gen_reg_mii_read_cmd(int argc, char **argv)
{
	CLICMD_T *cmd;
	int ret = -1;

	if (argc > 1) {
		cmd = cli_find_command(argv[1],
				     (CLICMD_T *) & gen_reg_mii_read_cmds[0]);
		if (cmd && cmd->action)
			ret = cmd->action(argc - 1, &argv[1]);	// skip set argument
		else {
			fprintf(stderr, "[%s] Command is not found!\n",
				argv[1]);
			gen_reg_mii_read_help_cmd(argc - 1, &argv[1]);
		}
	} else
		gen_reg_mii_read_help_cmd(argc - 1, &argv[1]);	// skip set argument

	return ret;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_mii_write_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int gen_reg_mii_write_cmd(int argc, char **argv)
{
	CLICMD_T *cmd;
	int ret = -1;

	if (argc > 1) {
		cmd = cli_find_command(argv[1],
				     (CLICMD_T *) & gen_reg_mii_write_cmds[0]);
		if (cmd && cmd->action)
			ret = cmd->action(argc - 1, &argv[1]);	// skip set argument
		else {
			fprintf(stderr, "[%s] Command is not found!\n",
				argv[1]);
			gen_reg_mii_write_help_cmd(argc - 1, &argv[1]);
		}
	} else
		gen_reg_mii_write_help_cmd(argc - 1, &argv[1]);	// skip set argument

	return ret;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_ni_pol_cal_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int gen_ni_pol_cal_cmd(int argc, char **argv)
{
	CLICMD_T *cmd;
	int ret = -1;

	if (argc > 1) {
		cmd = cli_find_command(argv[1],
				     (CLICMD_T *) & gen_ni_pol_cal_cmds[0]);
		if (cmd && cmd->action)
			ret = cmd->action(argc - 1, &argv[1]);	// skip set argument
		else {
			fprintf(stderr, "[%s] Command is not found!\n",
				argv[1]);
			gen_ni_pol_cal_help_cmd(argc - 1, &argv[1]);
		}
	} else
		gen_ni_pol_cal_help_cmd(argc - 1, &argv[1]);	// skip set argument

	return ret;
}

static int gen_reg_help_cmd(int argc, char **argv)
{
	cli_show_all_commands((CLICMD_T *) & gen_reg_main_cmds[0]);
	return 0;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_read_help_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED 
*----------------------------------------------------------------------*/
int gen_reg_read_help_cmd(int argc, char **argv)
{
	cli_show_all_commands((CLICMD_T *) & gen_reg_read_cmds[0]);
	return 0;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_write_help_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED 
*----------------------------------------------------------------------*/
int gen_reg_write_help_cmd(int argc, char **argv)
{
	cli_show_all_commands((CLICMD_T *) & gen_reg_write_cmds[0]);
	return 0;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_mii_read_help_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED 
*----------------------------------------------------------------------*/
int gen_reg_mii_read_help_cmd(int argc, char **argv)
{
	cli_show_all_commands((CLICMD_T *) & gen_reg_mii_read_cmds[0]);
	return 0;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_reg_mii_mii_write_help_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED 
*----------------------------------------------------------------------*/
int gen_reg_mii_write_help_cmd(int argc, char **argv)
{
	cli_show_all_commands((CLICMD_T *) & gen_reg_mii_write_cmds[0]);
	return 0;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - gen_ni_pol_cal_help_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED 
*----------------------------------------------------------------------*/
int gen_ni_pol_cal_help_cmd(int argc, char **argv)
{
	cli_show_all_commands((CLICMD_T *) & gen_ni_pol_cal_cmds[0]);
	return 0;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - reg_read_cmd 
*-----------------------------------------------------------------------
* DESCRIPTION: 
* Syntax     : reg_rw read mem [-b <location>] [-l <length>] [-1|2|4]
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED  
*----------------------------------------------------------------------*/
static int reg_read_cmd(char argc, char *argv[])
{
	int fd;
	char imgPt[] = "/dev/reg_rw";
	CS_REGREAD reg_read;
	int s, ret, err = 0;
	u_int8_t *cp;
	u_int32_t location = -1, length = -1, size = -1;

	// parse options
	argc--;
	while (!err && argc > 0 && *argv[1] == '-') {
		argv++;
		cp = argv[0];
		cp++;
		switch (*cp) {
		case 'h':
			err = 1;
			argc -= 1;
			break;
		case 'b':
			if (argc >= 2)
				argc -= 2;
			else {
				err = 1;
				break;
			}
			argv++;
			location = str2value_2(argv[0]);
			if (not_valid)
				err = 1;
			break;
		case 'l':	//L
			if (argc >= 2)
				argc -= 2;
			else {
				err = 1;
				break;
			}
			argv++;
			length = str2value_1(argv[0]);
			break;
		case '1':	//number 1
			size = 1;
			argc--;
			argv++;
			break;
		case '2':
			size = 2;
			argc--;
			argv++;
			break;
		case '4':
			size = 4;
			argc--;
			argv++;
			break;
		default:
			fprintf(stderr, "Syntax error!\n");
			err = 1;
			break;
		}
	}

	if (err || location == -1 || length == -1) {
		fprintf(stderr,
			"Syntax:	read mem [-b <location>] [-l <length>] [-1|2|4]\n");
		fprintf(stderr, "Example:read mem -b 0xf600a000 -l 20 -4\n");
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "\t-b	Register Address\n");
		fprintf(stderr, "\t-l	Length\n");
		fprintf(stderr, "\t-1/2/4	Display How Many Bytes\n");
		return 1;
	}

	reg_read.cmd = REGREAD;
	reg_read.len = sizeof(CS_REGREAD) - sizeof(CS_REGCMD_HDR_T);
	reg_read.location = location;
	reg_read.length = length;
	reg_read.size = size;

	if ((fd = open(imgPt, O_SYNC | O_RDWR)) < 0) {
		printf("ERROR: Open RW /Read device error: \n");
		return -1;
	}

	ret = ioctl(fd, SIOCDEVCS75XX, &reg_read);
	if (ret) {
		fprintf(stderr, "REGREAD Invalid argument \n");
		close(fd);
		return errno;
	}

	close(fd);

	return 0;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - reg_write_cmd 
*-----------------------------------------------------------------------
* DESCRIPTION: 
* Syntax     : reg_rw write mem [-b <location>] [-d <data>] [-1|2|4]
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED  
*----------------------------------------------------------------------*/
static int reg_write_cmd(char argc, char *argv[])
{
	int fd;
	char imgPt[] = "/dev/reg_rw";
	CS_REGWRITE reg_write;
	int s, ret, err = 0;
	u_int8_t *cp;
	u_int32_t location = -1, data = -1, size = -1;

	// parse options
	argc--;
	while (!err && argc > 0 && *argv[1] == '-') {
		argv++;
		cp = argv[0];
		cp++;
		switch (*cp) {
		case 'h':
			err = 1;
			argc -= 1;
			break;
		case 'b':
			if (argc >= 2)
				argc -= 2;
			else {
				err = 1;
				break;
			}
			argv++;
			location = str2value_2(argv[0]);
			if (not_valid) {
				err = 1;
				not_valid = 0;
			}
			break;
		case 'd':
			if (argc >= 2)
				argc -= 2;
			else {
				err = 1;
				break;
			}
			argv++;
			data = str2value_1(argv[0]);
			break;
		case '1':	//number 1
			size = 1;
			argc--;
			argv++;
			break;
		case '2':
			size = 2;
			argc--;
			argv++;
			break;
		case '4':
			size = 4;
			argc--;
			argv++;
			break;
		default:
			fprintf(stderr, "Syntax error!\n");
			err = 1;
			break;
		}
	}

	if (err || location == 0xffffffff) {
		fprintf(stderr,
			"Syntax:	write mem [-b <location>] [-d <data>] [-1|2|4]\n");
		fprintf(stderr,
			"Example:write mem -b 0xf600a080 -d 0x8000c001 -4\n");
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "\t-b	Register Address\n");
		fprintf(stderr, "\t-d	Data Vaule\n");
		fprintf(stderr, "\t-1/2/4	Display How Many Bytes\n");
		return 1;
	}

	reg_write.cmd = REGWRITE;
	reg_write.len = sizeof(CS_REGWRITE) - sizeof(CS_REGCMD_HDR_T);
	reg_write.location = location;
	reg_write.data = data;
	reg_write.size = size;

	if ((fd = open(imgPt, O_SYNC | O_RDWR)) < 0) {
		printf("ERROR: Open RW device error: \n");
		return -1;
	}

	ret = ioctl(fd, SIOCDEVCS75XX, &reg_write);
	if (ret) {
		fprintf(stderr, "REGWRITE Invalid argument \n");
		close(fd);
		return errno;
	}

	close(fd);

	return 0;
}

/*----------------------------------------------------------------------
* 	mii_read_cmd
*	Syntax: MII read [-a phy addr] [-r phy reg] [-l length]
*----------------------------------------------------------------------*/
static int mii_read_cmd(char argc, char *argv[])
{
	NAT_GMIIREG_T r_mii;
	int fd;
	char imgPt[] = "/dev/reg_rw";
	int ret, err = 0;
	u_int16_t phy_addr = -1, phy_reg = -1, phy_len = 1;
	u_int8_t *cp;

	// parse options
	argc--;
	while (!err && argc > 0 && *argv[1] == '-') {
		argv++;
		cp = argv[0];
		cp++;
		switch (*cp) {
		case 'h':
			err = 1;
			argc -= 1;
			break;
		case 'a':
			argc -= 2;
			argv++;
			phy_addr = str2value_1(argv[0]);
			break;
		case 'r':
			argc -= 2;
			argv++;
			phy_reg = str2value_1(argv[0]);
			break;
		case 'l':
			argc -= 2;
			argv++;
			phy_len = str2value_1(argv[0]);
			break;
		default:
			fprintf(stderr, "Syntax error!\n");
			err = 1;
			break;
		}
	}

	if (err || phy_addr == -1 || phy_reg == -1) {
		fprintf(stderr,
			"Syntax: MII read [-a phy addr] [-r phy reg] [-l length]\n");
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "\t-a  Phy address\n");
		fprintf(stderr, "\t-r  Phy registers\n");
		fprintf(stderr, "\t-l  Display total registers\n");
		return 1;
	}
//              s = socket(AF_INET, SOCK_DGRAM, 0);
//      if (s < 0)
//      {
//                      fprintf(stderr,"open socket fail\n");
//              return 1;
//      }

	//sprintf(ifr.ifr_name, "eth0");
	r_mii.cmd = GMIIREG;
	r_mii.len = sizeof(NAT_GMIIREG_T) - sizeof(CS_REGCMD_HDR_T);
	r_mii.phy_addr = phy_addr;
	r_mii.phy_reg = phy_reg;
	r_mii.phy_len = phy_len;

	//ifr.ifr_data = (void *)&r_mii;
	if ((fd = open(imgPt, O_SYNC | O_RDWR)) < 0) {
		printf("ERROR: Open RW /Read device error: \n");
		return -1;
	}
	//ret = ioctl(s, SIOCDEVLEPUS, &ifr) ;
	ret = ioctl(fd, SIOCDEVCS75XX, &r_mii);
	if (ret) {
		fprintf(stderr, "GMIIREG Invalid argument \n");
		close(fd);
		return errno;
	}

	close(fd);

	return 0;
}

/*----------------------------------------------------------------------
* 	mii_write_cmd
*	Syntax: MII write [-a phy addr] [-r phy reg] [-d data]
*----------------------------------------------------------------------*/
static int mii_write_cmd(char argc, char *argv[])
{
	NAT_SMIIREG_T s_mii;
	int fd;
	char imgPt[] = "/dev/reg_rw";
	int ret;
	u_int16_t phy_addr = -1, phy_reg = -1;
	u_int32_t phy_data = 0, data_present = 0;
	int err = 0;
	u_int8_t *cp;

	// parse options
	argc--;
	while (!err && argc > 0 && *argv[1] == '-') {
		argv++;
		cp = argv[0];
		cp++;
		switch (*cp) {
		case 'h':
			err = 1;
			argc -= 1;
			break;
		case 'a':
			argc -= 2;
			argv++;
			phy_addr = str2value_1(argv[0]);
			break;
		case 'r':
			argc -= 2;
			argv++;
			phy_reg = str2value_1(argv[0]);
			break;
		case 'd':
			argc -= 2;
			argv++;
			data_present = 1;
			phy_data = str2value_1(argv[0]);
			break;
		default:
			fprintf(stderr, "Syntax error!\n");
			err = 1;
			break;
		}
	}

	if (err || phy_addr == -1 || phy_reg == -1 || !data_present) {
		fprintf(stderr,
			"Syntax: mii [-a phy addr] [-r phy reg] [-d data]\n");
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "\t-a  Phy address\n");
		fprintf(stderr, "\t-r  Phy registers\n");
		fprintf(stderr, "\t-d  date\n");
		return 1;
	}
//      s = socket(AF_INET, SOCK_DGRAM, 0);
//    if (s < 0)
//    {
//              fprintf(stderr,"open socket fail\n");
//      return 1;
//    }

	//sprintf(ifr.ifr_name, "eth0");
	s_mii.cmd = SMIIREG;
	s_mii.len = sizeof(NAT_SMIIREG_T) - sizeof(CS_REGCMD_HDR_T);
	s_mii.phy_addr = phy_addr;
	s_mii.phy_reg = phy_reg;
	s_mii.phy_data = phy_data;

	//ifr.ifr_data = (void *)&s_mii;
	if ((fd = open(imgPt, O_SYNC | O_RDWR)) < 0) {
		printf("ERROR: Open RW device error: \n");
		return -1;
	}
	//ret = ioctl(s, SIOCDEVLEPUS, &ifr) ;
	ret = ioctl(fd, SIOCDEVCS75XX, &s_mii);

	if (ret) {
		fprintf(stderr, "SMIIREG Invalid argument \n");
		close(fd);
		return ret;
	}

	close(fd);
	return 0;
}

/*
 * 	ni_get_cmd
 *	Syntax: ni get [-e flag]
 *		-e: 1: enable get port calendar
 */
static int ni_get_cmd(char argc, char *argv[])
{
	CS_NIGETPORTCAL_T g_ni_cal;
	int fd;
	char imgPt[] = "/dev/reg_rw";
	int ret;
	u_int16_t enable = 0;
	int err = 0;
	u_int8_t *cp;

	argc--;
	while (!err && argc > 0 && *argv[1] == '-') {
		argv++;
		cp = argv[0];
		cp++;
		switch (*cp) {
		case 'h':
			err = 1;
			argc -= 1;
			break;
		case 'e':
			argc -= 2;
			argv++;
			enable = str2value_1(argv[0]);
			break;
		default:
			fprintf(stderr, "Syntax error!\n");
			err = 1;
			break;
		}
	}
	if (err || enable == 0) {
		fprintf(stderr, "Syntax: ni get [-e flag]\n");
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "\t-e  1: get port calendar\n");
		fprintf(stderr, "\Example: reg_rw ni get -e 1\n");
		return 1;
	}
	g_ni_cal.cmd = NIGETPORTCAL;
	g_ni_cal.len = sizeof(CS_NIGETPORTCAL_T) - sizeof(CS_REGCMD_HDR_T);
	g_ni_cal.get_port_cal = enable;
	if ((fd = open(imgPt, O_SYNC | O_RDWR)) < 0) {
		printf("ERROR: Open RW device error: \n");
		return -1;
	}
	ret = ioctl(fd, SIOCDEVCS75XX, &g_ni_cal);
	if (ret) {
		fprintf(stderr, "NIGETPORTCAL Invalid argument \n");
		close(fd);
		return ret;
	}

	close(fd);
	return 0;
}

/*
 * 	ni_set_cmd
 *	Syntax: ni set [-t table_address] [-p pspid]
 *	    -t: table address 0 ~ 95
 *	    -p: pspid
 *		0 - GE0
 *		1 - GE1
 *		2 - GE2
 *		3 - CPU
 *		4 - Crypto
 *		5 - Encap
 *		6 - Mcast
 *		7 - Mirror
 */
static int ni_set_cmd(char argc, char *argv[])
{
	CS_NISETPORTCAL_T s_ni_cal;
	int fd;
	char imgPt[] = "/dev/reg_rw";
	int ret;
	u_int16_t pspid = 0;
	u_int16_t table = 0;
	int err = 0;
	u_int8_t *cp;

	argc--;
	while (!err && argc > 0 && *argv[1] == '-') {
		argv++;
		cp = argv[0];
		cp++;
		switch (*cp) {
		case 'h':
			err = 1;
			argc -= 1;
			break;
		case 't':
			if (argc >= 2)
				argc -= 2;
			else {
				err = 1;
				break;
			}
			argv++;
			table = str2value_1(argv[0]);
			break;
		case 'p':
			if (argc >= 2)
				argc -= 2;
			else {
				err = 1;
				break;
			}
			argv++;
			pspid = str2value_1(argv[0]);
			break;
		default:
			fprintf(stderr, "Syntax error!\n");
			err = 1;
			break;
		}
	}
	if (err) {
		fprintf(stderr, "Syntax: ni set [-t table] [-p pspid]\n");
		fprintf(stderr, "\Example: reg_rw ni set -t 95 -p 3\n");
		fprintf(stderr, "Options:\n");
		fprintf(stderr, "\t-t  table address 0 ~ 95\n");
		fprintf(stderr, "\t-p  pspid\n");
		fprintf(stderr, "\t	0 - GE0\n");
		fprintf(stderr, "\t	1 - GE1\n");
		fprintf(stderr, "\t	2 - GE2\n");
		fprintf(stderr, "\t	3 - CPU\n");
		fprintf(stderr, "\t	4 - Crypto\n");
		fprintf(stderr, "\t	5 - Encap\n");
		fprintf(stderr, "\t	6 - Mcast\n");
		fprintf(stderr, "\t	7 - Mirror\n");
		return 1;
	}
	s_ni_cal.cmd = NISETPORTCAL;
	s_ni_cal.len = sizeof(CS_NISETPORTCAL_T) - sizeof(CS_REGCMD_HDR_T);
	s_ni_cal.table_address = table;
	s_ni_cal.pspid_ts = pspid;

	if ((fd = open(imgPt, O_SYNC | O_RDWR)) < 0) {
		printf("ERROR: Open RW device error: \n");
		return -1;
	}
	ret = ioctl(fd, SIOCDEVCS75XX, &s_ni_cal);

	if (ret) {
		fprintf(stderr, "NISETPORTCAL Invalid argument \n");
		close(fd);
		return ret;
	}

	close(fd);

	return 0;
}

/*----------------------------------------------------------------------
* char2hex_1
*----------------------------------------------------------------------*/
u_int32_t char2hex_1(u_int8_t c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else if (c >= 'a' && c <= 'f')
		return (c - 'a' + 10);
	else if (c >= 'A' && c <= 'F')
		return (c - 'A' + 10);
	else
		return (0xffffffff);
}

/*----------------------------------------------------------------------
* char2decimal_1
*----------------------------------------------------------------------*/
u_int32_t char2decimal_1(u_int8_t c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else
		return (0xffffffff);
}

/*----------------------------------------------------------------------
* str2hex_1
*----------------------------------------------------------------------*/
u_int32_t str2hex_1(u_int8_t * cp)
{
	u_int32_t value, result;

	result = 0;
	if (*cp == '0' && toupper(*(cp + 1)) == 'X')
		cp += 2;

	while ((value = char2hex_1(*cp)) != 0xffffffff) {
		result = result * 16 + value;
		cp++;
	}

	return (result);
}

/*----------------------------------------------------------------------
* str2decimal_1
*----------------------------------------------------------------------*/
u_int32_t str2decimal_1(u_int8_t * cp)
{
	u_int32_t value, result;

	result = 0;
	while ((value = char2decimal_1(*cp)) != 0xffffffff) {
		result = result * 10 + value;
		cp++;
	}

	return (result);
}

/*----------------------------------------------------------------------
* str2value_1
*----------------------------------------------------------------------*/
u_int32_t str2value_1(u_int8_t * strp)
{
//    u_int32_t value, result;
	char *cp;
	int is_hex = 0;

	cp = strp;
	if (*cp == '0' && toupper(*(cp + 1)) == 'X') {
		strp += 2;
		is_hex = 1;
	}
	// check 
	cp = strp;
	while (*cp) {
		if (ishex_1(*cp))
			is_hex = 1;
		else if (!isdigit_1(*cp))
			return 0;
		cp++;
	}

	return (is_hex) ? str2hex_1(strp) : str2decimal_1(strp);

}

/*----------------------------------------------------------------------
* str2value_2
*----------------------------------------------------------------------*/
u_int32_t str2value_2(u_int8_t * strp)
{
//    u_int32_t value, result;
	char *cp;
	int is_hex = 0;

	cp = strp;
	if (*cp == '0' && toupper(*(cp + 1)) == 'X') {
		strp += 2;
		is_hex = 1;
	}
	// check 
	cp = strp;
	while (*cp) {
		if (ishex_1(*cp))
			is_hex = 1;
		else if (!isdigit_1(*cp))
			return 0;
		cp++;
	}

	return (is_hex) ? str2hex_1(strp) : str2decimal_1(strp);
}

/*----------------------------------------------------------------------
* ROUTINE NAME - cli_show_all_commands
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : LICMD_T *cmd: points the first command table
* OUTPUT     : 0: SUCCESS 1:FAILED    
*----------------------------------------------------------------------*/
int cli_show_all_commands(CLICMD_T * cmd)
{

	fprintf(stderr, "[Command List]\n");

	while (cmd->name) {
		fprintf(stderr, "%s --- %s\n", cmd->name, cmd->help_msg);
		cmd++;
	}
	return 0;
}

int main(int argc, char **argv)
{
	CLICMD_T *cmd;
	int ret = -1;

	if (argc == 1)
		cli_show_all_commands((CLICMD_T *) & gen_reg_main_cmds[0]);
	else {
		cmd =
		    cli_find_command(argv[1],
				     (CLICMD_T *) & gen_reg_main_cmds[0]);
		if (cmd) {
			ret = cmd->action(argc - 1, &argv[1]);
		} else {
			fprintf(stderr, "[%s] Command is not found!\n",
				argv[1]);
			cli_show_all_commands((CLICMD_T *) &
					      gen_reg_main_cmds[0]);
		}
	}
	return ret;
}
