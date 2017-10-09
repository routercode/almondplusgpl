/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/
/*
 * pkt_fwd_ctl.c
 *
 * $Id: pkt_fwd_ctl.c,v 1.2 2012/06/21 07:55:17 ewang Exp $
 *
 * Device Control Commands for packet forwarding filter.
 * Purpose: dropt LAN-to-LAN, WAN-to-WAN, and WLAN-to-WLAN traffic
 *          maintain forwarding bit-map for LAN-to-WAN traffic per VLAN
 * $Log: pkt_fwd_ctl.c,v $
 * Revision 1.2  2012/06/21 07:55:17  ewang
 * BUG#31959: porting pkt_fwd_filter from R0.1 to R1.x trunk
 *
 *
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/fs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "pkt_fwd_ctl.h"		/* ioctl command IDs */




#define isdigit_1(c)		(c >= '0' && c <= '9') ? 1 : 0
#define ishex_1(c)		((c >= 'A' && c <= 'F') || \
				 (c >= 'a' && c <= 'f')) ? 1 : 0

int fwd_ctl_help_cmd( int argc, char ** argv);
int fwd_ctl_reset_cmd(int argc, char **argv);
int fwd_ctl_raw_cmd(int argc, char **argv);
int fwd_ctl_grp_cmd(int argc, char **argv);
int fwd_ctl_vid_mode_cmd(int argc, char **argv);

int fd;

CLICMD_T fwd_ctl_main_cmds[] = {
	{"?",		fwd_ctl_help_cmd,	"Help Commands"},
	{"help",	fwd_ctl_help_cmd,	"Help Commands"},
	{"reset",	fwd_ctl_reset_cmd,	"Reset forwarding group and bit-map"},
	{"raw",		fwd_ctl_raw_cmd,	"Get/set forwarding group and bit-map"},
	{"group",	fwd_ctl_grp_cmd,	"Get/set forwarding group"},
	{"vid_mode", 	fwd_ctl_vid_mode_cmd,	"Get/Set forwarding mode of the VID"},
	{0,0,0}
};


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

u_int32_t char2decimal_1(u_int8_t c)
{
	if (c >= '0' && c <= '9')
		return (c - '0');
	else
		return (0xffffffff);
}

u_int32_t str2hex_1(u_int8_t *cp)
{
	u_int32_t value, result;

	result = 0;
	if (*cp=='0' && toupper(*(cp+1))=='X')
		cp += 2;
	
	while ((value = char2hex_1(*cp)) != 0xffffffff)	{
		  result = result * 16 + value;
		  cp++;
	}

	return(result);
}

u_int32_t str2decimal_1(u_int8_t *cp)
{
	u_int32_t value, result;

	result=0;
	while ((value = char2decimal_1(*cp)) != 0xffffffff) {
		result= result * 10 + value;
		cp++;
	}

	return(result);
}


u_int32_t str2value_1(u_int8_t *strp)
{
	char	*cp;
	int		is_hex = 0;

	cp = strp;
	if (*cp=='0' && toupper(*(cp+1))=='X') {
		strp += 2;
		is_hex = 1;
	}
	
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
int cli_show_all_commands(CLICMD_T *cmd)
{

	fprintf(stderr,"[Command List]\n");

	while (cmd->name) {
		fprintf(stderr,"%s --- %s\n",cmd->name, cmd->help_msg);
		cmd++;
	}
	return 0;
}



CLICMD_T *cli_find_command(char *name, CLICMD_T *cmd)
{
	int i, total, len=strlen(name);
	CLICMD_T *me=NULL, *exact;
	
	exact=(CLICMD_T *)0;
	total=0;
	while (cmd->name) {
		char *srce=name;
		char *dest=cmd->name;
		for (i=0; i<len; i++) {
			if (tolower(*dest)!=tolower(*srce)) break;
			dest++; srce++;
		}
		
		if (i==len) {
			if (i==strlen(cmd->name))
				exact=cmd;
			total++;       
			me=cmd;
		}
		
		cmd++;
	}
	
	if (total>1)
	{
		return((CLICMD_T *)exact);   /* duplicate */
	}        
	if (total==1)
		return(me);
	else    
		return((CLICMD_T *)0);
}

static char * eth_group (int group)
{
	switch (group) {
	case 0: /* NETDEV_GRP_IGNORE */
		return "IGNORE";
	case 1: /* NETDEV_GRP_LAN */
		return "LAN";
	case 2: /* NETDEV_GRP_WAN */
		return "WAN";
	case 3: /* NETDEV_GRP_LAN | NETDEV_GRP_WAN */
		return "LAN|WAN";
	case 4: /* NETDEV_GRP_WLAN */
		return "WLAN";
	case 5: /* NETDEV_GRP_LAN | NETDEV_GRP_WLAN */
		return "LAN|WLAN";
	case 6: /* NETDEV_GRP_WAN | NETDEV_GRP_WLAN */
		return "WAN|WLAN";
	case 7: /* NETDEV_GRP_LAN | NETDEV_GRP_WAN | NETDEV_GRP_WLAN */
		return "LAN|WAN|WLAN";
	default:
		return "UNKNOWN";
	}
}

static char * eth_fwd_mode (int mode)
{
	switch (mode) {
	case 0:
		return "DROP";
	case 1:
		return "FORWARD";
	default:
		return "UNKNOWN";
	}
}


/*----------------------------------------------------------------------
* ROUTINE NAME - fwd_ctl_help_cmd
*-----------------------------------------------------------------------
* DESCRIPTION: 
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED 
*----------------------------------------------------------------------*/
int fwd_ctl_help_cmd( int argc, char ** argv)
{
	cli_show_all_commands((CLICMD_T *)&fwd_ctl_main_cmds[0]);
	printf("Syntax:\n"
	       " pkt_fwd_ctl [cmd] [device name] ([parameters]...)\n"
	       " pkt_fwd_ctl reset [device name]\n"
	       " pkt_fwd_ctl raw [device name] ([forwarding group] [VLAN bit-map] ...)\n"
	       " pkt_fwd_ctl group [device name] ([forwarding group])\n"
	       " pkt_fwd_ctl vid_mode [device name] [VID] ([mode])\n"
	       "Parameters:\n"
	       " [device name]: Name of net device (eth0, eth1_0, and etc.)\n"
	       " [forwarding group]: 0 (IGNORE), 1 (LAN), 2 (WAN), 4 (WLAN)\n"
	       " [VLAN bit-map]: 0 ~ 0xFF (up to 512 1-byte parameters)\n"
	       " [VID]: 0 ~ 4095\n"
	       " [mode]: 0 (DROP), 1 (FORWARD)\n");

	return 0;
}


/*----------------------------------------------------------------------
* ROUTINE NAME - fwd_ctl_reset_cmd
*-----------------------------------------------------------------------
* DESCRIPTION:  pkt_fwd_ctl reset [device name]
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int fwd_ctl_reset_cmd(int argc, char **argv)
{
	BR_FWD_PKT_FT_CMD_T br_cmd;
	int ret = -1;
	
	memset(&br_cmd, 0, sizeof(br_cmd));
	
	if (argc >= 2) {
		br_cmd.cmd = PKT_FWD_FT_RESET;
		strncpy(br_cmd.netdev_name, argv[1], BR_PKT_FWD_FT_NAME_LEN);
		br_cmd.netdev_name[BR_PKT_FWD_FT_NAME_LEN - 1] = '\0';

		ret = ioctl(fd, SIOCDEVPRIVATE, &br_cmd) ;
		if (ret) {
			fprintf(stderr, "Fail to reset, ret = %d\n", ret);
			return -1;
		}
	 }
	else
		fwd_ctl_help_cmd(argc-1,&argv[1]); /* skip the 1st argument */

	return ret ;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - fwd_ctl_raw_cmd
*-----------------------------------------------------------------------
* DESCRIPTION:  raw [device name] ([forwarding group] [VLAN bit-map] ...)
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int fwd_ctl_raw_cmd(int argc, char **argv)
{
	BR_FWD_PKT_FT_CMD_T br_cmd;
	int ret = -1;
	int i, j;
	
	memset(&br_cmd, 0, sizeof(br_cmd));
	
	if (argc == 2) { /* Get command */
		br_cmd.cmd = PKT_FWD_FT_RAW_GET;
		strncpy(br_cmd.netdev_name, argv[1], BR_PKT_FWD_FT_NAME_LEN);
		br_cmd.netdev_name[BR_PKT_FWD_FT_NAME_LEN - 1] = '\0';

		ret = ioctl(fd, SIOCDEVPRIVATE, &br_cmd) ;
		if (ret) {
			fprintf(stderr, "Fail to reset, ret = %d\n", ret);
			return -1;
		}
		
		printf("Forwarding group: %s (%d)\n", eth_group(br_cmd.fwd_grp),
			br_cmd.fwd_grp);
		printf("Offset  (VID     ) : VLAN filter (HEX)\n");
		for(i = 0; i < 68; i++)
			printf("-");
		for (i = 0; i < (BR_PKT_FWD_FT_VLAN_ARRAY_LEN/16); i++) {
			printf("\n0x%04XH (VID %4d) : ", i*16, i*128);
			for (j = 0; j < 16; j++) {
				printf("%02X ", br_cmd.vid_map[i*16+j]);
			}
		}
		printf("\n");
		
	} else if (argc >= 4) { /* Set command */
		br_cmd.cmd = PKT_FWD_FT_RAW_SET;
		strncpy(br_cmd.netdev_name, argv[1], BR_PKT_FWD_FT_NAME_LEN);
		br_cmd.netdev_name[BR_PKT_FWD_FT_NAME_LEN - 1] = '\0';

		br_cmd.fwd_grp = str2value_1(argv[2]);
		
		j = ((argc - 3) < BR_PKT_FWD_FT_VLAN_ARRAY_LEN) ?
			(argc - 3) : BR_PKT_FWD_FT_VLAN_ARRAY_LEN;
		for ( i = 0; i < j; i++ )
			br_cmd.vid_map[i] = str2value_1(argv[i + 3]);

		ret = ioctl(fd, SIOCDEVPRIVATE, &br_cmd) ;
		if (ret) {
			fprintf(stderr, "Fail to reset, ret = %d\n", ret);
			return -1;
		}
		
	} else
		fwd_ctl_help_cmd(argc-1,&argv[1]);  /* skip the 1st argument */

	return ret ;
}


/*----------------------------------------------------------------------
* ROUTINE NAME - fwd_ctl_grp_cmd
*-----------------------------------------------------------------------
* DESCRIPTION:  group [device name] ([forwarding group])
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int fwd_ctl_grp_cmd(int argc, char **argv)
{
	BR_FWD_PKT_FT_CMD_T br_cmd;
	int ret = -1;
	int i, j;
	
	memset(&br_cmd, 0, sizeof(br_cmd));
	
	if (argc == 2) { /* Get command */
		br_cmd.cmd = PKT_FWD_FT_ETH_GRP_GET;
		strncpy(br_cmd.netdev_name, argv[1], BR_PKT_FWD_FT_NAME_LEN);
		br_cmd.netdev_name[BR_PKT_FWD_FT_NAME_LEN - 1] = '\0';

		ret = ioctl(fd, SIOCDEVPRIVATE, &br_cmd) ;
		if (ret) {
			fprintf(stderr, "Fail to reset, ret = %d\n", ret);
			return -1;
		}
		
		printf("Forwarding group: %s (%d)\n", eth_group(br_cmd.fwd_grp),
			br_cmd.fwd_grp);
		
	} else if (argc >= 3) { /* Set command */
		br_cmd.cmd = PKT_FWD_FT_ETH_GRP_SET;
		strncpy(br_cmd.netdev_name, argv[1], BR_PKT_FWD_FT_NAME_LEN);
		br_cmd.netdev_name[BR_PKT_FWD_FT_NAME_LEN - 1] = '\0';

		br_cmd.fwd_grp = str2value_1(argv[2]);
		
		ret = ioctl(fd, SIOCDEVPRIVATE, &br_cmd) ;
		if (ret) {
			fprintf(stderr, "Fail to reset, ret = %d\n", ret);
			return -1;
		}
		
	} else
		fwd_ctl_help_cmd(argc-1,&argv[1]);  /* skip the 1st argument */

	return ret ;
}

/*----------------------------------------------------------------------
* ROUTINE NAME - fwd_ctl_vid_mode_cmd
*-----------------------------------------------------------------------
* DESCRIPTION:  vid_mode [device name] [VID] ([mode])
* INPUT      : int argc, char ** argv
* OUTPUT     : 0: SUCCESS 1:FAILED     
*----------------------------------------------------------------------*/
int fwd_ctl_vid_mode_cmd(int argc, char **argv)
{
	BR_FWD_PKT_FT_CMD_T br_cmd;
	int ret = -1;
	int i, j;
	
	memset(&br_cmd, 0, sizeof(br_cmd));
	
	if (argc == 3) { /* Get command */
		br_cmd.cmd = PKT_FWD_FT_ETH_VLAN_GET;
		strncpy(br_cmd.netdev_name, argv[1], BR_PKT_FWD_FT_NAME_LEN);
		br_cmd.netdev_name[BR_PKT_FWD_FT_NAME_LEN - 1] = '\0';
		
		br_cmd.vlan_mode.vid = str2value_1(argv[2]) & 0xFFF;

		ret = ioctl(fd, SIOCDEVPRIVATE, &br_cmd) ;
		if (ret) {
			fprintf(stderr, "Fail to reset, ret = %d\n", ret);
			return -1;
		}
		
		printf("VID %d: %s (%d)\n", br_cmd.vlan_mode.vid, 
			eth_fwd_mode(br_cmd.vlan_mode.mode),
			br_cmd.vlan_mode.mode);
		
	} else if (argc >= 4) { /* Set command */
		br_cmd.cmd = PKT_FWD_FT_ETH_VLAN_SET;
		strncpy(br_cmd.netdev_name, argv[1], BR_PKT_FWD_FT_NAME_LEN);
		br_cmd.netdev_name[BR_PKT_FWD_FT_NAME_LEN - 1] = '\0';

		br_cmd.vlan_mode.vid = str2value_1(argv[2]) & 0xFFF;
		br_cmd.vlan_mode.mode = str2value_1(argv[3]) & 0x1;
		
		ret = ioctl(fd, SIOCDEVPRIVATE, &br_cmd) ;
		if (ret) {
			fprintf(stderr, "Fail to reset, ret = %d\n", ret);
			return -1;
		}
		
	} else
		fwd_ctl_help_cmd(argc-1,&argv[1]);  /* skip the 1st argument */

	return ret ;
}



/*
 * pkt_fwd_ctl [cmd] [device name] ([parameters]...)
 * pkt_fwd_ctl reset [device name]
 * pkt_fwd_ctl raw [device name] ([forwarding group] [VLAN bit-map] ...)
 * pkt_fwd_ctl group [device name] ([forwarding group])
 * pkt_fwd_ctl vid_mode [device name] [VID] ([mode])
 */

int main(int argc, char **argv)
{
	CLICMD_T *cmd;
	int ret=-1;
	char file[] = "/dev/" BR_PKT_FWD_FT_DRIVER_NAME;

	if ((fd = open(file, O_SYNC|O_RDWR)) < 0) {
		fprintf(stderr, "ERROR: Open switch device (%s) error: \n", file);
		return -1;
	}

	if (argc < 3)
		fwd_ctl_help_cmd(argc, argv);
	else
	{
		cmd = cli_find_command(argv[1],(CLICMD_T *)&fwd_ctl_main_cmds[0]);
		if (cmd) {
			ret =cmd->action(argc-1, &argv[1]);
		} else {
			fprintf(stderr,"[%s] Command is not found!\n",argv[1]);
			fwd_ctl_help_cmd(argc, argv);
		}
	}

	close(fd);
	return ret;
}

