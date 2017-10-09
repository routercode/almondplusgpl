#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <syslog.h>
#include <termios.h>
#include <unistd.h>
#include <assert.h>
#include <linux/soundcard.h>
#include <errno.h>
#include "cs75xx_spdif.h"

#define SPDIF_DIAG_MAJOR_STR	"0"
#define SPDIF_DIAG_MINOR_STR	"0"

enum diagOption {
	SPDIF_DIAG,
	SPDIF_ICLK,
	SPDIF_PLAY,
	SPDIF_HELP
};

char optionStr[][16] = {
	"-diag",
	"-iclk",
	"-play",
	"--help"
};

char helpStr[] = {
	"G2 SPDIF Module Diagnostic" SPDIF_DIAG_MAJOR_STR "." SPDIF_DIAG_MINOR_STR "\n" \
	"Usage: spdif-test [OPTION] [PARAM]...\n"
	"    -diag                       diagnostic command\n" \
	"    -iclk                       play <1: internal clock, 0: external clock>\n" \
	"    -play                       play [PARAM1: wave file] [PARAM2: times] or test.wav\n" \
	/* Data Path Test */
	"    --help                      Print Help (this message) and exit\n"
};

#define SUPPORT_AC3
#ifdef SUPPORT_AC3
static char fscod_str[][16] = {
	"48 kHz",
	"44.1 kHz",
	"32 kHz",
	"reserved"
};

static char bsmod_str[][64] = {
	"main audio service: complete main (CM)",
	"main audio service: music and effects (ME)",
	"associated service: visually impaired (VI)",
	"associated service: hearing impaired (HI)",
	"associated service: dialogue (D)",
	"associated service: commentary (C)",
	"associated service: emergency (E)",
	"others"
};

static char ac3_acmod_str[][32] = {
	"1+1 (Ch1, Ch2)",
	"1/0 (C)",
	"2/0 (L, R)",
	"3/0 (L, C, R)",
	"2/1 (L, R, S)",
	"3/1 (L, C, R, S)",
	"2/2 (L, R, SL, SR)",
	"3/2 (L, C, R, SL, SR)"
};

static char cmixlev_str[][16] = {
	"0.707 (-3.0 dB)",
	"0.595 (-4.5 dB)",
	"0.500 (-6.0 dB)",
	"reserved"
};

static char surmixlev_str[][16] = {
	"0.707 (-3 dB)",
	"0.500 (-6 dB)",
	"0",
	"reserved"
};

static char dsurmod_str[][32] = {
	"not indicated",
	"Not Dolby Surround encoded",
	"Dolby Surround encoded",
	"reserved"
};

static const unsigned int ac3_frame_size_code_table[][3] = {
                       /* 48 kHz,    44.1 kHz,  32 kHz */
/* 0x00 - 32 kbps */   {  64,        69,        96   },
/* 0x01 - 32 kbps */   {  64,        70,        96   },
/* 0x02 - 40 kbps */   {  80,        87,        120  },
/* 0x03 - 40 kbps */   {  80,        88,        120  },
/* 0x04 - 48 kbps */   {  96,        104,       144  },
/* 0x05 - 48 kbps */   {  96,        105,       144  },
/* 0x06 - 56 kbps */   {  112,       121,       168  },
/* 0x07 - 56 kbps */   {  112,       122,       168  },
/* 0x08 - 64 kbps */   {  128,       139,       192  },
/* 0x09 - 64 kbps */   {  128,       140,       192  },
/* 0x0A - 80 kbps */   {  160,       174,       240  },
/* 0x0B - 80 kbps */   {  160,       175,       240  },
/* 0x0C - 96 kbps */   {  192,       208,       288  },
/* 0x0D - 96 kbps */   {  192,       209,       288  },
/* 0x0E - 112 kbps */  {  224,       243,       336  },
/* 0x0F - 112 kbps */  {  224,       244,       336  },
/* 0x10 - 128 kbps */  {  256,       278,       384  },
/* 0x11 - 128 kbps */  {  256,       279,       384  },
/* 0x12 - 160 kbps */  {  320,       348,       480  },
/* 0x13 - 160 kbps */  {  320,       349,       480  },
/* 0x14 - 192 kbps */  {  384,       417,       576  },
/* 0x15 - 192 kbps */  {  384,       418,       576  },
/* 0x16 - 224 kbps */  {  448,       487,       672  },
/* 0x17 - 224 kbps */  {  448,       488,       672  },
/* 0x18 - 256 kbps */  {  512,       557,       768  },
/* 0x19 - 256 kbps */  {  512,       558,       768  },
/* 0x1A - 320 kbps */  {  640,       696,       960  },
/* 0x1B - 320 kbps */  {  640,       697,       960  },
/* 0x1C - 384 kbps */  {  768,       835,       1152 },
/* 0x1D - 384 kbps */  {  768,       836,       1152 },
/* 0x1E - 448 kbps */  {  896,       975,       1344 },
/* 0x1F - 448 kbps */  {  896,       976,       1344 },
/* 0x20 - 512 kbps */  {  1024,      1114,      1536 },
/* 0x21 - 512 kbps */  {  1024,      1115,      1536 },
/* 0x22 - 576 kbps */  {  1152,      1253,      1728 },
/* 0x23 - 576 kbps */  {  1152,      1254,      1728 },
/* 0x24 - 640 kbps */  {  1280,      1393,      1920 },
/* 0x25 - 640 kbps */  {  1280,      1394,      1920 },
};
#endif

#define SUPPORT_DTS
#ifdef SUPPORT_DTS
static const char amode_str[][64] = {
	"A",
	"A + B (dual mono)",
	"L + R (stereo)",
	"(L+R) + (L-R) (sum-difference)",
	"LT +RT (left and right total)",
	"C + L + R",
	"L + R+ S",
	"C + L + R+ S",
	"L + R+ SL+SR",
	"C + L + R+ SL+SR",
	"CL + CR + L + R + SL + SR",
	"C + L + R+ LR + RR + OV",
	"CF+ CR+LF+ RF+LR + RR",
	"CL + C + CR + L + R + SL + SR",
	"CL + CR + L + R + SL1 + SL2+ SR1 + SR2",
	"CL + C+ CR + L + R + SL + S+ SR"
};

static const int dts_sample_rates[] =
{
	0,
	8000,
	16000,
	32000,
	0,
	0,
	11025,
	22050,
	44100,
	0,
	0,
	12000,
	24000,
	48000,
	96000,
	192000
};
#endif

int main(int argc, char *argv[])
{
	FILE *fp;
	int fd, i, j, k, m, err, index, option = -1, count = 1;
	char spdifDevice[16] = "/dev/dsp";
	char defaultAudiofile[] = "/sbin/test.wav";
	char usageStr[] = "Try `spdif_test --help' for more information.";
	spdif_diag_cmd_t diag_cmd;
	struct stat fileStat;
	int handle, num;
	char *imgBuff;
	int fileLen, buf_size, cksize;
	char ifunc;
	spdif_audio_type_t audio_type = AUDIO_UNDEF;
	wave_file_info_t fileInfo;
#ifdef SUPPORT_AC3
	ac3_file_info_t ac3_header;
#endif
#ifdef SUPPORT_DTS
	int nblks, fsize, amode, sfreq, lff;
	unsigned char hdr[12];
	unsigned short info[4];
	dts_file_info_t dts_header;
#endif

	if (argc < 2) {
		printf("%s\n", usageStr);
		return -1;
	}

	if (strncmp(argv[1], optionStr[SPDIF_HELP], strlen(optionStr[SPDIF_HELP])) == 0) {
		printf("%s\n", helpStr);
		return -1;
	}

	for (i = 0; i < SPDIF_HELP; i++)
		if (strncmp(argv[1], optionStr[i], strlen(optionStr[i])) == 0) {
			option = i;
		}

	if (option == -1) {
		printf("%s\n", usageStr);
		return -1;
	}

	if ((fd = open(spdifDevice, O_SYNC | O_RDWR)) < 0) {
		printf("Open device %s failed !!!r\n", spdifDevice);
		return -1;
	}

	switch (option) {
	case SPDIF_DIAG:
		if (argc < 4) {
			printf("%s\n", usageStr);
			break;
		}

		diag_cmd.cmd = (unsigned int)strtoul(argv[2], NULL, 0);
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 0);
		ioctl(fd, SPDIF_DIAG_CMD, &diag_cmd);
		break;

	case SPDIF_ICLK:
		if (argc < 4) {
			printf("%s\n", usageStr);
			break;
		}

		diag_cmd.cmd = DIAG_INTER_CLOCK;
		diag_cmd.data = (unsigned int)strtoul(argv[3], NULL, 0);
		ioctl(fd, SPDIF_DIAG_CMD, &diag_cmd);
		break;

	case SPDIF_PLAY:
		if (argc >= 3) {
			if ((fp = fopen(argv[2], "rb")) == NULL) {
				printf("open file error: %s\n", argv[2]);
				break;
			}
			if (argc >= 4) {
				count = strtoul(argv[3], NULL, 0);
				if (count <= 1)
					count = 1;
			}
		}
/*
		else {
			if ((fp = fopen(defaultAudiofile, "rb")) == NULL) {
				printf("open file error: %s\n", defaultAudiofile);
				break;
			}
		}
*/
		handle = fileno(fp);
		if (fstat(handle, &fileStat) < 0) {
			printf("read file state error\n");
			break;
		}

		for (m = 0; m < count; m++) {
#ifdef SUPPORT_AC3
			/* AC-3 */
			fseek(fp, 0, SEEK_SET);
			fread(&ac3_header, sizeof(ac3_header), 1, fp);
			fileLen = fileStat.st_size;
			printf("fileLen = %d (0x%x)\n", fileLen, fileLen);

			if (ac3_header.syncword == 0x770B) {
				audio_type = AUDIO_AC3;
#if 0
				printf("syncword = %02X%02X\n", (ac3_header.syncword & 0x00FF), (ac3_header.syncword >> 8));
				printf("CRC = %02X%02X\n", (ac3_header.crc1 & 0x00), (ac3_header.crc1 >> 8));
				printf("Sampling frequency = 0x%X - %s\n", ac3_header.fscod, fscod_str[ac3_header.fscod]);
				printf("Frame Size Code = 0x%X\n", ac3_header.frmsizecod);
				printf("Bit Stream Identification = 0x%X\n", ac3_header.bsid);
				printf("Bit Stream Mode = 0x%X - %s\n", ac3_header.bsmod, bsmod_str[ac3_header.bsmod]);
				printf("Audio Coding Mode = 0x%X - %s\n", ac3_header.acmod, ac3_acmod_str[ac3_header.acmod]);
				printf("Center Mix level = 0x%X - %s\n", ac3_header.cmixlev, cmixlev_str[ac3_header.cmixlev]);
				printf("Surround Mix Level = 0x%X - %s\n", ac3_header.surmixlev, surmixlev_str[ac3_header.surmixlev]);
				printf("Dolby Surround Mode = 0x%X - %s\n", ac3_header.dsurmod, dsurmod_str[ac3_header.dsurmod]);
#endif
				ioctl(fd, SPDIF_AC3_FILEINFO, &ac3_header);

				fileLen = fileStat.st_size;

				buf_size = ac3_frame_size_code_table[ac3_header.frmsizecod][ac3_header.fscod] * 2;;

				goto SUPPORT_FMT;
			}
#endif
#ifdef SUPPORT_DTS
			/* DTS */
			fseek(fp, 0, SEEK_SET);
			fread(hdr, 12, 1, fp);
			if (hdr[0] == 0x7f && hdr[1] == 0xfe &&
				hdr[2] == 0x80 && hdr[3] == 0x01) {

				audio_type = AUDIO_DTS;

				info[0] = hdr[4] << 8 | hdr[5];
				info[1] = hdr[6] << 8 | hdr[7];
				info[2] = hdr[8] << 8 | hdr[9];
				info[3] = hdr[10] << 8 | hdr[11];

				nblks = (info[0] >> 2)	& 0x7f;
				fsize = ((info[0] & 0x0003) << 12 | info[1] >> 4) + 1;
				amode = (info[1] << 2)	& 0x3c |
						(info[2] >> 14) & 0x03;
				sfreq = (info[2] >> 10) & 0x0f;
				lff   = (info[3] >> 9)	& 0x03;
				nblks++;


				dts_header.samplerate = dts_sample_rates[sfreq];
				//dts_header.framesize = fsize;
				dts_header.framesize = 2013;
				//if (fsize == 2012)
				//	buf_size = 2048;
				//else {
				//	printf("Not support DTS frame size %d!\n", fsize);
				//	goto UNSUPPORT_FMT;
				//}
				nblks <<= 5;
				if (nblks == 512)
					dts_header.type = 11;	// DTS type I
				else if (nblks == 1024)
					dts_header.type = 12;	// DTS type II
				else if (nblks == 2048)
					dts_header.type = 13;	// DTS type III
				else {
					printf("Invalid DTS block size %d!\n", nblks >> 5);
					goto UNSUPPORT_FMT;
				}
				dts_header.format = 0;

				//printf("\n\n#### samplerate = %d, framesize = %d, type = %d###\n\n", dts_header.samplerate, dts_header.framesize, dts_header.type);
				ioctl(fd, SPDIF_DTS_FILEINFO, &dts_header);

				fileLen = fileStat.st_size;
				printf("DTS fileLen = %d\n", fileLen);

				buf_size = dts_header.framesize;

				goto SUPPORT_FMT;
			}
#endif

			/* WAVE */
			audio_type = AUDIO_WAV;
			fseek(fp, 16, SEEK_SET);
			fread(&cksize, 4, 1, fp);
			//printf("fmt cksize = %d\n", cksize);
			fread(&fileInfo.format, 2, 1, fp);
			//printf("format = %d\n", fileInfo.format);
			fread(&fileInfo.nChannels, 2, 1, fp);
			//printf("nChannels = %d\n", fileInfo.nChannels);
			fread(&fileInfo.nSamplesPerSec, 4, 1, fp);
			//printf("nSamplesPerSec = %d\n", fileInfo.nSamplesPerSec);
			fseek(fp, 6, SEEK_CUR);
			fread(&fileInfo.wBitsPerSample, 2, 1, fp);
			//printf("nSamplesPerSec = %d\n", fileInfo.wBitsPerSample);
			fseek(fp, 20+cksize+4, SEEK_SET);
			fread(&fileInfo.nDataLen, 4, 1, fp);
			//printf("nDataLen = %d\n", fileInfo.nDataLen);
			ioctl(fd, SPDIF_WAVE_FILEINFO, &fileInfo);

			fileLen = fileInfo.nDataLen;
			printf("fileLen = %d (0x%x)\n", fileLen, fileLen);
			ioctl(fd, SPDIF_FILE_LEN, &fileLen);

			if (fileInfo.wBitsPerSample == 16)
				buf_size = DEF_BUF_SIZE;
			else
				buf_size = 3*DEF_BUF_SIZE;
		  	printf("buf_size = %d\n", buf_size);

SUPPORT_FMT:
			ioctl(fd, SPDIF_INIT_BUF, NULL);
			ioctl(fd, SPDIF_FILE_LEN, &fileLen);

			imgBuff = malloc(buf_size);

			if (!imgBuff) {
				printf("imgBuff buffer alloc error.\n");
				break;
			}

			i = fileLen / buf_size;
			j = fileLen % buf_size;
			printf("SPDIF test start --> i:%d  j:%d\n", i, j);

			if (audio_type != AUDIO_WAV)
				fseek(fp, 0, SEEK_SET);
			for (k = 0; k < i; k++) {
				fread(imgBuff, buf_size, 1, fp);

  #if 0
				if (k == 0) {
					int q;

					for (q = 0; q < 128; q++) {
						printf("%02X ", *(imgBuff + q));
						if ((q+1)%16 == 0)
							printf("\n");
					}
				}
  #endif
			wt_again:
				err = write(fd, imgBuff, buf_size);
				if (err <= 0) {
					printf("write err!!\n");
					break;
				}
				if (err != buf_size) {
					printf("len = %d\n", err);
					goto wt_again;
				}
				printf(".");
			}
			if (j != 0) {
				memset(imgBuff, 0x0, buf_size);
				fread(imgBuff, j, 1, fp);
			wt_again1:
				err = write(fd, imgBuff, buf_size);
				if (err < 0) {
					printf("write err!!\n");
					break;
				}
				if (err != buf_size) {
					printf("len = %d\n", err);
					goto wt_again1;
				}
				printf("*");
			}
			printf("\nSPDIF test end <--\n");
			ioctl(fd, SPDIF_STOP_PLAY, NULL);
			free(imgBuff);
		}

		printf("close file ctrl \n");
		fclose(fp);
		break;

	default:
		printf("%s\n", usageStr);
	}

UNSUPPORT_FMT:
	close(fd);
	return 0;
}

