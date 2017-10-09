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
#include "dac.h"
#include <errno.h>

#define DAC_DIAG_MAJOR_STR	"0"
#define DAC_DIAG_MINOR_STR	"0"

enum diagOption {
	DAC_INIT,
	DAC_PLAY,
	DAC_LEVEL,
//	DAC_STEREO,
//	DAC_SAMPLE,
//	DAC_FORMAT,
	DAC_HELP,
};

char optionStr[][16] = {
	"-init",
	"-play",
	"-level",
//	"-stereo",
//	"-sample",
//	"-format",
	"--help"
};

char helpStr[] = {
	"DAC CS4341 with G2 SSP Diagnostic" DAC_DIAG_MAJOR_STR "." DAC_DIAG_MINOR_STR "\n" \
	"Usage: spdif_test [OPTION] [ARGUMENT]...\n"
	"    -init                       init CS4341\n" \
	"    -play                       play specific wav file\n" \
	"    -level                      adjust volume, '+' to up, '-' to down\n" \
//	"    -stereo                     1: stereo, 0: mono\n" \
//	"    -sample                     sampling rate\n" \
//	"    -format                     audio format\n" \
	/* Data Path Test */
	"    --help                      Print Help (this message) and exit\n"
};

int main(int argc, char *argv[])
{
	FILE *fp;
	int fd, i, j, k,  m, err, index, option = -1, count = 1;
	char dacDevice[16] = "/dev/dsp1";
	char defaultAudiofile[] = "/sbin/test.wav";
	char usageStr[] = "Try `dac_test --help' for more information.";
	struct stat fileStat;
	int handle, num;
	char *imgBuff;
	int fileLen, buf_size, cksize;
	char ifunc;
	char tmpbuf[8];
	wave_file_info_t fileInfo;

	if (argc < 2) {
		printf("%s\n", usageStr);
		return -1;
	}

	if (strncmp(argv[1], optionStr[DAC_HELP], strlen(optionStr[DAC_HELP])) == 0) {
		printf("%s\n", helpStr);
		return -1;
	}

	for (i = 0; i < DAC_HELP; i++)
		if (strncmp(argv[1], optionStr[i], strlen(optionStr[i])) == 0) {
			option = i;
		}

	if (option == -1) {
		printf("%s\n", usageStr);
		return -1;
	}

	if ((fd = open(dacDevice, O_SYNC | O_RDWR)) < 0) {
		printf("Open device %s failed !!!r\n", dacDevice);
		return -1;
	}

	switch (option) {
	case DAC_INIT:
		ioctl(fd, SSP_I2S_INIT_MIXER, NULL);
		break;

	case DAC_LEVEL:
		if (argc >= 3) {
			if (strncmp(argv[2], "+", 1) == 0)
				ioctl(fd, SSP_I2S_INC_LEVEL, NULL);
			if (strncmp(argv[2], "-", 1) == 0)
				ioctl(fd, SSP_I2S_DEC_LEVEL, NULL);
		}
		break;
#if 0
	case DAC_STEREO:
		ioctl(fd, SSP_I2S_STEREO, &num);
		break;

	case DAC_SAMPLE:
		ioctl(fd, SSP_I2S_SETSPEED, &num);
		break;

	case DAC_FORMAT:
		ioctl(fd, SSP_I2S_SETFMT, &num);
		break;
#endif
	case DAC_PLAY:
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
		} else {
			if ((fp = fopen(defaultAudiofile, "rb")) == NULL) {
				printf("open file error: %s\n", defaultAudiofile);
				break;
			}
		}
		handle = fileno(fp);
		if (fstat(handle, &fileStat) < 0) {
			printf("read file state error\n");
			break;
		}

		for (m = 0; m < count; m++) {
			ioctl(fd, SSP_I2S_INIT_BUF, NULL);

			fseek(fp, 0, SEEK_SET);
			fread(tmpbuf, 4, 1, fp);
			tmpbuf[4] = '\0';
			printf("tmpbuf = %s\n", tmpbuf);
			if (strncmp(tmpbuf, "RIFF", strlen("RIFF")) == 0)
				fileInfo.endian = 0;	// little
			else
				fileInfo.endian = 1;	// big
			printf("fileInfo.endian = %d\n", fileInfo.endian);
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
			ioctl(fd, SSP_I2S_FILE_INFO, &fileInfo);

			fileLen = fileInfo.nDataLen;
			printf("fileLen = %d (0x%x)\n", fileLen, fileLen);
			ioctl(fd, SSP_I2S_FILE_LEN, &fileLen);

			if (fileInfo.wBitsPerSample == 24 && fileInfo.endian == 0)
				buf_size = 3*SBUF_SIZE;
			else
			buf_size = SBUF_SIZE;
			imgBuff = malloc(buf_size);
			if (!imgBuff) {
				printf("imgBuff(0x%x) buffer alloc error.\n", SBUF_SIZE);
				break;
			}

			i = fileLen / buf_size;
			j = fileLen % buf_size;
			printf("DAC test start --> i:%d  j:%d\n", i, j);
			for (k = 0; k < i; k++) {
				fread(imgBuff, buf_size, 1, fp);

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
				//printf(".");
			}
			if (j != 0) {
				memset(imgBuff, 0x0, buf_size);
				fread(imgBuff, j, 1, fp);
			wt_again1:
				err = write(fd, imgBuff, buf_size);
				if (err <= 0) {
					printf("write err!!\n");
					break;
				}
				if (err != buf_size) {
					printf("len = %d\n", err);
					goto wt_again1;
				}
				printf("*");
			}
			printf("\nDAC test end <--\n");
			ioctl(fd, SSP_I2S_STOP_DMA, NULL);
			free(imgBuff);
		}
		printf("close file ctrl \n");
		fclose(fp);
		break;

	default:
		printf("%s\n", usageStr);
	}

	close(fd);
	return 0;
}

