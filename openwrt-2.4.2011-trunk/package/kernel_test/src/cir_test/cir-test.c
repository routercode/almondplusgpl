/********************************************************************/
/*File: cir-test.c                                                  */
/*                                                                  */
/*Descriptions:                                                     */
/*   To configure CIR and dump the input from the remote controller */
/*                                                                  */
/*Author:                                                           */
/*  Y.J. Wu 2010-06-09                                              */
/*                                                                  */
/*                                                                  */
/*History:                                                          */
/*   2010-06-09         init version                                */
/********************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <mach/cs75xx_cir.h>

#define CIR_DIAG_MAJOR_STR		"0"
#define CIR_DIAG_MINOR_STR		"0"
//#define TYPE_TV1_26		0
//#define TYPE_VCR_33		1
//#define DVB_T				2

#define TV1_26_PREFIX 0x40040100	/* TV1-26 Input(48 bits) prefix + 0xXXXX */
#define VCR_33_PREFIX 0x613E		/* VCR-33 Input(32 bits) prefix + 0xXXXX */

/* Power Key */
#define POWER		"Power"
/* Digit Key */
#define DIGIT_0 	"Digit 0"
#define DIGIT_1 	"Digit 1"
#define DIGIT_2 	"Digit 2"
#define DIGIT_3 	"Digit 3"
#define DIGIT_4 	"Digit 4"
#define DIGIT_5 	"Digit 5"
#define DIGIT_6 	"Digit 6"
#define DIGIT_7 	"Digit 7"
#define DIGIT_8 	"Digit 8"
#define DIGIT_9 	"Digit 9"
/* Function Key */
#define CATV		"CATV"
#define TVAV		"TV/AV"
#define DISP		"Disp"
#define MUTE		"Mute"
#define CH_UP		"CH Up"
#define CH_DOWN		"CH Down"
#define VOL_UP		"VOL +"
#define VOL_DOWN	"VOL -"
#define ENTER		"ENT"
#define PA			"P/A"
#define SEL_UP		"SEL Up"
#define SEL_DOWN	"SEL Down"
#define RET			"RET"
#define SOUND		"Sound"
#define STOP		"Stop"
#define PLAY		"Play"
#define REV			"Reverse"
#define FWD			"Forward"
#define REC			"Record"
#define PAUSE		"Pause"
#define MENU		"Menu"
#define MTS			"MTS"

#define CATCH		"Catch"
#define ENLARGE		"Enlarge"
#define FLUSH		"Flush"


struct cir_input {
	char str[12];
	unsigned int value;
};

struct cir_input tv1_26_table[] = {   // not inlcude prefix
    /* Power */
    {POWER,     0xBCBD},
    /* Digit Key */
    {DIGIT_0,   0x5859},
    {DIGIT_1,   0x0809},
    {DIGIT_2,   0x8889},
    {DIGIT_3,   0x4849},
    {DIGIT_4,   0xC8C9},
    {DIGIT_5,   0x2829},
    {DIGIT_6,   0xA8A9},
    {DIGIT_7,   0x6869},
    {DIGIT_8,   0xE8E9},
    {DIGIT_9,   0x1819},
    /* Func Key */
    {CATV,      0x8A8B},
    {TVAV,      0xA0A1},
    //{Disp,      0x0},
    {MUTE,      0x4C4D},
    {CH_UP,     0x2C2D},
    {CH_DOWN,   0xACAD},
    {VOL_UP,    0x0405},
    {VOL_DOWN,  0x8485},
    {ENTER,     0x9899},
    {PA,        0x0A0B},
    {SEL_UP,    0x5253},
    {SEL_DOWN,  0xD2D3},
    {RET,       0x4140},
    {SOUND,     0x8C8D},
    //{STOP,      0x0},
    //{PLAY,      0x0},
    //{REV,       0x0},
    //{FWD,       0x0},
    //{REC,       0x0},
    //{PAUSE,     0x0},
    {MENU,      0x4A4B},
    {MTS,       0xCCCD},
};

struct cir_input vcr_33_table[] = {   // not inlcude prefix
    /* Power */
    {POWER,     0x609F},
    /* Digit Key */
    {DIGIT_0,   0x08f7},
    {DIGIT_1,   0x8877},
    {DIGIT_2,   0x48B7},
    {DIGIT_3,   0xC837},
    {DIGIT_4,   0x28D7},
    {DIGIT_5,   0xA857},
    {DIGIT_6,   0x6897},
    {DIGIT_7,   0xE817},
    {DIGIT_8,   0x18E7},
    {DIGIT_9,   0x9867},
    /* Func Key */
    {CATV,      0x906F},
    {TVAV,      0x00FF},
    {DISP,      0xD827},
    //{MUTE,    0x0},
    {CH_UP,     0xE01F},
    {CH_DOWN,   0x807F},
    //{VOL_UP,    0x0},
    //{VOL_DOWN,  0x0},
    {ENTER,     0xD02F},
    {PA,        0xF00F},
    {SEL_UP,    0xE01F},
    {SEL_DOWN,  0x807F},
    {RET,   0x30CF},
    //{SOUND,     0x0},
    {STOP,      0x20DF},
    {PLAY,      0xC03F},
    {REV,       0x40BF},
    {FWD,       0x10EF},
    {REC,       0x50AF},
    {PAUSE,     0xA05F},
    {MENU,      0xF807},
    {MTS,       0xB04F},
};

struct cir_input dvb_t_table[] = {
    /* Power */
    {POWER,     0x38000000},
    /* Digit Key */
    {DIGIT_0,   0x39100000},
    {DIGIT_1,   0x38400000},
    {DIGIT_2,   0x38500000},
    {DIGIT_3,   0x38600000},
    {DIGIT_4,   0x38800000},
    {DIGIT_5,   0x38900000},
    {DIGIT_6,   0x38A00000},
    {DIGIT_7,   0x38C00000},
    {DIGIT_8,   0x38D00000},
    {DIGIT_9,   0x38E00000},
    /* Func Key */
    {CATCH,     0x39200000},
    {MUTE,      0x38300000},
    {ENLARGE,   0x38200000},
    {FLUSH,     0x38100000},
    {CH_UP,     0x38F00000},
    {CH_DOWN,   0x39300000},
    {VOL_UP,    0x38700000},
    {VOL_DOWN,  0x38B00000},
    {SEL_UP,    0x39000000},
    {SEL_DOWN,  0x39400000},
    {ENTER,     0x39500000},
    {REC,       0x39600000},
    {STOP,      0x39700000},
};

enum diagOption {
	SET_BAUD_DIV,
	SET_HIGH_T,
	SET_LOW_T,
	SET_RC5_EXTEND,
	SET_RC5_STOP,
	SET_PROTOCOL,
	SET_POS,
	SET_DEMOD,
	SET_PWR_CPU,
	SET_DATA_COMP,
	SET_DATA_LEN,
	SET_PWR_KEY,
	SET_PRECISION,	
	SEL_CONF,
	DISP_CONF,
	GET_OLD_RX,
	GET_NEW_RX,
	CIR_HELP,
};

char optionStr[][8] = {
	"-sbd",
	"-shp",
	"-slp",
	"-sre",
	"-srs",
	"-spr",
	"-spo",
	"-sdm",
	"-spc",
	"-sdc",
	"-sdl",
	"-spk",
	"-si",
	"-sc",
	"-dc",
	"-god",
	"-gnd",
	"--help",
};

char helpStr[] = {
	"Golden Gate CIR Module Diagnostic" CIR_DIAG_MAJOR_STR"."CIR_DIAG_MINOR_STR"\n" \
	"Usage: cir-test [OPTION] [ARGUMENT]\n"
	"    -sbd <baud_div>      Set baud rate division\n" \
	"    -shp <high_period>   Set high period of frame header for NEC protocol\n" \
	"    -slp <low_period>    Set low period of frame header for NEC protocol\n" \
	"    -sre <rc5_extend>    Set if decode received frame with second start bit\n"
	"                         as command bit[6], RC-5 protocl only\n" \
	"    -srs <rc5_stopbit>   Set if decode received frame with stop bit, RC-5\n" \
	"                         protocl only\n" \
	"    -spr <cir_protocol>  Set CIR protocol, 0 is RC-5, 1 is NEC\n" \
	"    -spo <pos>           Set polarity of received signal from received module\n" \
	"    -sdm <demod>         Set demodulation\n" \
	"    -spc <pwrirqcpu>     Set if interrupt of CIR received power key to CPU\n" \
	"                         when system power is on\n" \
	"    -sdc <data_compare>  Set if  received frame's data and data bar comparing,\n"
	"                         NEC protocol only\n" \
	"    -sdl <data_len>      Set the data length of received frame from 8 bits to\n"
	"                         48 bits long, NEC protocol only\n"
	"    -spk <pwr_code>      Set power key code for comparing CIR receive data,\n"
	"                         <pwr_code> format is x x\n"
	"    -si <interval>       Set receive precision interval\n"	
	"    -sc <select>         Select predefined configuration, 1 is TV1-26, 2 is\n"
	"                         VCR-33, 3 is DVB-T\n"
	"    -dc                  Display configuration\n"
	"    -god                 Get previous receiving data\n"
	"    -gnd <timeout>       Get new receiving data for <timeout> time\n"
	"    --help               Print Help (this message) and exit\n"
};

int inputparse(struct cir_ioctl_data48 ioctl_data48)
{
	int i;

	for (i = 0; i < sizeof(tv1_26_table)/sizeof(struct cir_input); i++)
		if ((ioctl_data48.data == TV1_26_PREFIX) && (ioctl_data48.data_ext == tv1_26_table[i].value)) {
			printf("%s\n", tv1_26_table[i].str);
			return 0;
		}

	for (i = 0; i < sizeof(vcr_33_table)/sizeof(struct cir_input); i++)
		if (ioctl_data48.data == (vcr_33_table[i].value | (VCR_33_PREFIX << 16))) {
			printf("%s\n", vcr_33_table[i].str);
			return 0;
		}

	for (i = 0; i < sizeof(dvb_t_table)/sizeof(struct cir_input); i++)
		if (ioctl_data48.data  == dvb_t_table[i].value) {
			printf("%s\n", dvb_t_table[i].str);
			return 0;
		}

	printf("Unknown\n");
	return -1;
}

int main(int argc, char *argv[])
{
	struct cir_ioctl_data ioctl_data;
	struct cir_ioctl_data48	ioctl_data48;
	int i, fd, option = -1, sel;
	char cirDevice[32];
	char usageStr[] = "Try `cir-test --help' for more information.";

	if (argc < 2 ) {
		printf("%s\n", usageStr);
		return -1;
	}

	for (i = 0; i <= CIR_HELP; i++) {
		if (strncmp(argv[1], optionStr[i], strlen(optionStr[i])) == 0)
			option = i;
	}

	//printf("option = %d\n", option);

	if (option == -1) {
		printf("%s\n", usageStr);
		return -1;
	}

	if ((option != DISP_CONF) && (option != GET_OLD_RX) && (option != CIR_HELP)) {
		if (argc < 3) {
			printf("%s\n", usageStr);
			return -1;
		}
		else {
			if (option == SET_PWR_KEY) {
				if (argc < 4) {
					printf("%s\n", usageStr);
					return -1;
				}
			}
		}
	}

	sprintf(cirDevice, "/dev/%s", CS75XX_CIR_NAME);
	if ((fd = open(cirDevice, O_RDONLY)) < 0) {
		printf("Open device %s failed !!!\n", cirDevice);
		return -1;
	}

	switch (option) {
	case SET_BAUD_DIV:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_BAUDRATE, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_BAUDRATE, &ioctl_data);
		printf("Set new baud rate division to %d\n", ioctl_data.data);
		break;

	case SET_HIGH_T:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_HIGH_PERIOD, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_HIGH_PERIOD, &ioctl_data);
		printf("Set new high period to %d\n", ioctl_data.data);
		break;

	case SET_LOW_T:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_LOW_PERIOD, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_LOW_PERIOD, &ioctl_data);
		printf("Set new low period to %d\n", ioctl_data.data);
		break;

	case SET_RC5_EXTEND:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_RC5_EXTEND, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_RC5_EXTEND, &ioctl_data);
		printf("Set new RC-5 extend to %s\n", ioctl_data.data ? "enable" : "disable");
		break;

	case SET_RC5_STOP:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_RC5_STOPBIT, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_RC5_STOPBIT, &ioctl_data);
		printf("Set new RC-5 stop bit to %s\n", ioctl_data.data ? "enable" : "disable");
		break;

	case SET_PROTOCOL:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_PROTOCOL, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_PROTOCOL, &ioctl_data);
		printf("Set new CIR protocol to %s\n", ioctl_data.data ? "NEC" : "RC-5");
		break;

	case SET_POS:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_POSITIVE_POLARITY, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_POSITIVE_POLARITY, &ioctl_data);
		printf("Set new polarity of received signal to %s for idle\n", ioctl_data.data ? "low" : "high");
		break;

	case SET_DEMOD:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_ENABLE_DEMOD, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_ENABLE_DEMOD, &ioctl_data);
		printf("Set new demodulation to %s\n", ioctl_data.data ? "enable" : "disable");
		break;

	case SET_PWR_CPU:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_POWER_KEY_HANDLE, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_POWER_KEY_HANDLE, &ioctl_data);
		printf("Set interrupt of CIR received power key to CPU to %s\n", ioctl_data.data ? "enable" : "disable");
		break;

	case SET_DATA_COMP:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_ENABLE_COMPARE, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_ENABLE_COMPARE, &ioctl_data);
		printf("Set received frame's data and data bar comparing to %s\n", ioctl_data.data ? "enable" : "disable");
		break;

	case SET_DATA_LEN:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_DATA_LEN, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_DATA_LEN, &ioctl_data);
		printf("Set data length of received frame to %d\n", ioctl_data.data);
		break;

	case SET_PWR_KEY:
		ioctl_data48.data = (unsigned int)strtoul(argv[2], NULL, 16);
		ioctl_data48.data_ext =(unsigned int)strtoul(argv[3], NULL, 16);
		ioctl(fd, CIR_SET_POWER_KEY, &ioctl_data48);
		ioctl_data48.data = -1;
		ioctl_data48.data_ext = -1;
		ioctl(fd, CIR_GET_POWER_KEY, &ioctl_data48);
		printf("Set new power key coce to %X-%X\n", ioctl_data48.data, ioctl_data48.data_ext);
		break;

	case SET_PRECISION:
		ioctl_data.data = (unsigned int)strtoul(argv[2], NULL, 0);
		ioctl(fd, CIR_SET_PRECISION, &ioctl_data);
		ioctl_data.data = -1;
		ioctl(fd, CIR_GET_PRECISION, &ioctl_data);
		printf("Set new precision interval to %d(=%dms)\n", ioctl_data.data, ioctl_data.data*100);
		break;
		
	case SEL_CONF:
		sel = (unsigned int)strtoul(argv[2], NULL, 0);
		if (sel == 1)
			ioctl_data.data = TV1_PROTOCOL;
		else if (sel == 2)
			ioctl_data.data = VCR_PROTOCOL;
		else if (sel == 3)
			ioctl_data.data = DVB_PROTOCOL;
		else {
			printf("invalid select %d\n", sel);
			break;
		}

		ioctl(fd, CIR_SET_CONFIGURATION_SEL, &ioctl_data);
		break;
	
	case DISP_CONF:
		ioctl(fd, CIR_GET_BAUDRATE, &ioctl_data);
		printf("baud rate division: %d\n", ioctl_data.data);
		ioctl(fd, CIR_GET_HIGH_PERIOD, &ioctl_data);
		printf("high period: %d\n", ioctl_data.data);
		ioctl(fd, CIR_GET_LOW_PERIOD, &ioctl_data);
		printf("low period: %d\n", ioctl_data.data);
		ioctl(fd, CIR_GET_RC5_EXTEND, &ioctl_data);
		printf("RC-5 extend: %s\n", ioctl_data.data ? "enable" : "disable");
		ioctl(fd, CIR_GET_RC5_STOPBIT, &ioctl_data);
		printf("RC-5 stop bit: %s\n", ioctl_data.data ? "enable" : "disable");
		ioctl(fd, CIR_GET_PROTOCOL, &ioctl_data);
		printf("CIR protocol: %s\n", ioctl_data.data ? "NEC" : "RC-5");
		ioctl(fd, CIR_GET_POSITIVE_POLARITY, &ioctl_data);
		printf("polarity of received signal: %s for idle\n", ioctl_data.data ? "low" : "high");
		ioctl(fd, CIR_GET_ENABLE_DEMOD, &ioctl_data);
		printf("demodulation to %s\n", ioctl_data.data ? "enable" : "disable");
		ioctl(fd, CIR_GET_POWER_KEY_HANDLE, &ioctl_data);
		printf("interrupt of CIR received power key to CPU: %s\n", ioctl_data.data ? "enable" : "disable");
		ioctl(fd, CIR_GET_ENABLE_COMPARE, &ioctl_data);
		printf("received frame's data and data bar comparing: %s\n", ioctl_data.data ? "enable" : "disable");
		ioctl(fd, CIR_GET_DATA_LEN, &ioctl_data);
		printf("data length of received frame: %d\n", ioctl_data.data);
		ioctl(fd, CIR_GET_POWER_KEY, &ioctl_data48);
		printf("power key coce: %X-%X\n", ioctl_data48.data, ioctl_data48.data_ext);
		ioctl(fd, CIR_GET_PRECISION, &ioctl_data);
		printf("precision interval: %d(=%dms)\n", ioctl_data.data, ioctl_data.data*100);		
		break;

	case GET_OLD_RX:
		ioctl(fd, CIR_GET_DATA, &ioctl_data48);
		printf("Old Rx is %X-%X\r\n", ioctl_data48.data, ioctl_data48.data_ext);
		break;

	case GET_NEW_RX:
		if (strtoul(argv[2], NULL, 0) <= 60)
			ioctl_data48.timeout = strtoul(argv[2], NULL, 0);
		else
			ioctl_data48.timeout = 10;
		do {
			ioctl(fd, CIR_WAIT_INT_DATA, &ioctl_data48);
			if (ioctl_data48.data != 0)
				inputparse(ioctl_data48);
		} while(ioctl_data48.data != 0);	// not timeout

		break;

	case CIR_HELP:
		printf("%s\n", helpStr);
		break;
	}

	close(fd);
	return 0;
}

