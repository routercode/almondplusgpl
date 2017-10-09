/*
 *  Based on spidev_test.c, spidev_fdx.c
 */

#include <stdint.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/types.h>
#include <linux/spi/spidev.h>

#define SPI_CONFIG_FLAG_SPEED	0x00000001
#define SPI_CONFIG_FLAG_DELAY	0x00000002
#define SPI_CONFIG_FLAG_BPW	0x00000004
//#define SPI_CONFIG_FLAG_IOCTL	0x00000008
//#define SPI_CONFIG_FLAG_FOPT	0x00000010
#define SPI_CONFIG_FLAG_LOOP	0x00000008
#define SPI_CONFIG_FLAG_CPHA	0x00000010
#define SPI_CONFIG_FLAG_CPOL	0x00000020
#define SPI_CONFIG_FLAG_LSB	0x00000040
#define SPI_CONFIG_FLAG_CSHIGH	0x00000080
#define SPI_CONFIG_FLAG_3WIRE	0x00000100
#define SPI_CONFIG_FLAG_NOCS	0x00000200
#define SPI_CONFIG_FLAG_READY	0x00000400
//#define SPI_CONFIG_FLAG_ITER	0x00000800


static void pabort(const char *s)
{
	perror(s);
	abort();
}

static __u32 conf_flag = 0x0;
static const char *device = NULL;
static __u8 mode;
static __u8 bits;
static __u32 speed;
static __u16 delay;
//static int verbose;
static unsigned char tx_buf[96];
static int rx_len, tx_len, option_io, option_file;
static __u32 iter = 1;

static int conv(char *input, int *rx_len, int *tx_len, unsigned char *tx_buf)
{
	int i;
	char *output = input;
	char *p = NULL;

	/* get rx length */
	p = strchr(output, '_');
	if (p) {
		*p = '\0';
		*rx_len = atoi(output);
		output = p + 1;
	}
	else {
		*rx_len = atoi(output);
		*tx_len = 0;
		return 0;
	}

	/* get tx length and data */
	*tx_len = 0;
	p = strtok(output, ":");
	if (p) {
		*(tx_buf++) = strtoul(p, NULL, 16);
		(*tx_len)++;
	}
	while (p = strtok(NULL, ":")) {
		*(tx_buf++) = strtoul(p, NULL, 16);
		(*tx_len)++;
	}

	return 0;
}

static void do_read(int fd, int rx_len, int tx_len, unsigned char *tx_buf)
{
	unsigned char	rx_buf[96];
	int		i, status;

	if (tx_len) {
		status = write(fd, tx_buf, tx_len);
		if (status < 0) {
			perror("write");
			return;
		}
		printf("write(%2d, %2d):\n", tx_len, status);
		while (tx_len-- > 0)
			printf(" %02x", *tx_buf++);
		printf("\n");
	}

	if (rx_len) {
		memset(rx_buf, 0, sizeof rx_buf);

		status = read(fd, rx_buf, rx_len);
		if (status < 0) {
			perror("read");
			return;
		}
//		if (status != rx_len) {
//			fprintf(stderr, "short read\n");
//			return;
//		}
		printf("read(%2d, %2d):\n", rx_len, status);
		for (i = 0; i < rx_len; i++)
			printf(" %02x", rx_buf[i]);
		printf("\n");
	}
}

static void do_msg(int fd, int rx_len, int tx_len, unsigned char *tx_buf)
{
	int i;
	struct spi_ioc_transfer	xfer[2];
	unsigned char		buf[96], *bp, tmp_buf[96];
	int			status;

	struct timeval tvs, tve;
	struct timezone tz;

	gettimeofday(&tvs, &tz);

	memset(tmp_buf, 0, 96);
	for (i = 0; i < iter; i++) {

		memset(xfer, 0, sizeof xfer);
		memset(buf, 0, sizeof buf);

		if (rx_len > sizeof buf)
			rx_len = sizeof buf;

		memcpy(buf, tx_buf, tx_len);
		xfer[0].tx_buf = (unsigned long)buf;
		xfer[0].len = tx_len;
		if (conf_flag & SPI_CONFIG_FLAG_DELAY)
			xfer[0].delay_usecs = delay;

		xfer[1].rx_buf = (unsigned long)buf;
		xfer[1].len = rx_len;
		if (conf_flag & SPI_CONFIG_FLAG_DELAY)
			xfer[1].delay_usecs = delay;

		status = ioctl(fd, SPI_IOC_MESSAGE(2), xfer);
		if (status < 0) {
			printf("at %d time, SPI_IOC_MESSAGE", i);
			return;
		}

		if ((i == 0) && rx_len) {
			memcpy(tmp_buf, buf, rx_len);
		} else {
			if (memcmp(tmp_buf, buf, rx_len)) {
				printf("at %d time, the result is not the same as before!\n", i + 1);
				break;
			}
		}
	}

	printf("response(%2d, %2d): ", rx_len, status);
	for (bp = buf; rx_len; rx_len--)
		printf(" %02x", *bp++);
	printf("\n");

	if (iter > 1) {
		gettimeofday(&tve, &tz);
		printf("%d times message took %d us\n", iter, tve.tv_sec * 1000000 + tve.tv_usec - (tvs.tv_sec * 1000000 + tvs.tv_usec));
	}
}

static void dumpstat(const char *name, int fd)
{
	__u8	org_mode, org_lsb, org_bits;
	__u32	org_speed;

	if (ioctl(fd, SPI_IOC_RD_MODE, &org_mode) < 0) {
		perror("SPI rd_mode");
		return;
	}
	if (ioctl(fd, SPI_IOC_RD_LSB_FIRST, &org_lsb) < 0) {
		perror("SPI rd_lsb_fist");
		return;
	}
	if (ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &org_bits) < 0) {
		perror("SPI bits_per_word");
		return;
	}
	if (ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &org_speed) < 0) {
		perror("SPI max_speed_hz");
		return;
	}

	printf("%s: spi mode %d, %d bits %sper word, %d Hz max\n",
		name, org_mode, org_bits, org_lsb ? "(lsb first) " : "", org_speed);
}

static int updatestat(const char *name, int fd)
{
	__u8	org_mode;
	int	ret = 0;

	/*
	 * spi mode
	 */
	ret = ioctl(fd, SPI_IOC_RD_MODE, &org_mode);
	if (ret == -1)
		pabort("can't get spi mode");

	if (conf_flag & SPI_CONFIG_FLAG_LOOP) {
		if (mode & SPI_LOOP)
			org_mode |= SPI_LOOP;
		else
			org_mode &= ~SPI_LOOP;
	}

	if (conf_flag & SPI_CONFIG_FLAG_CPHA) {
		if (mode & SPI_CPHA)
			org_mode |= SPI_CPHA;
		else
			org_mode &= ~SPI_CPHA;
	}

	if (conf_flag & SPI_CONFIG_FLAG_CPOL) {
		if (mode & SPI_CPOL)
			org_mode |= SPI_CPOL;
		else
			org_mode &= ~SPI_CPOL;
	}

	if (conf_flag & SPI_CONFIG_FLAG_LSB) {
		if (mode & SPI_LSB_FIRST)
			org_mode |= SPI_LSB_FIRST;
		else
			org_mode &= ~SPI_LSB_FIRST;
	}

	if (conf_flag & SPI_CONFIG_FLAG_CSHIGH) {
		if (mode & SPI_CS_HIGH)
			org_mode |= SPI_CS_HIGH;
		else
			org_mode &= ~SPI_CS_HIGH;
	}

	if (conf_flag & SPI_CONFIG_FLAG_3WIRE) {
		if (mode & SPI_3WIRE)
			org_mode |= SPI_3WIRE;
		else
			org_mode &= ~SPI_3WIRE;
	}

	if (conf_flag & SPI_CONFIG_FLAG_NOCS) {
		if (mode & SPI_NO_CS)
			org_mode |= SPI_NO_CS;
		else
			org_mode &= ~SPI_NO_CS;
	}

	if (conf_flag & SPI_CONFIG_FLAG_READY) {
		if (mode & SPI_READY)
			org_mode |= SPI_READY;
		else
			org_mode &= ~SPI_READY;
	}

	ret = ioctl(fd, SPI_IOC_WR_MODE, &org_mode);
	if (ret == -1)
		pabort("can't set spi mode");
	ret = ioctl(fd, SPI_IOC_RD_MODE, &org_mode);
	if (ret == -1)
		pabort("can't get spi mode");

	/*
	 * bits per word
	 */
	if (conf_flag & SPI_CONFIG_FLAG_BPW) {
		ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
		if (ret == -1)
			pabort("can't set bits per word");

		ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
		if (ret == -1)
			pabort("can't get bits per word");
	}

	/*
	 * max speed hz
	 */
	if (conf_flag & SPI_CONFIG_FLAG_SPEED) {
		ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
		if (ret == -1)
			pabort("can't set max speed hz");

		ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
		if (ret == -1)
			pabort("can't get max speed hz");
	}

	dumpstat(name, fd);

}

static void print_usage(const char *prog)
{
	printf("Usage: %s -D [-sbdIFlHOLC3i] <param>\n", prog);
	puts("  -D --device       device to use ex:/dev/spidev0.0)\n"
	     "  -s --speed        max speed (Hz)\n"
	     "  -d --delay        delay (usec)\n"
	     "  -b --bpw          bits per word\n"
	     "  -I --ioctl        ioctl tx/rx, ex:2_4a:1:73\n"
	     "  -F --file         file write/read, ex:2_4a:1:73\n"
	     "  -l --loop         loopback\n"
	     "  -H --cpha         clock phase\n"
	     "  -O --cpol         clock polarity\n"
	     "  -L --lsb          least significant bit first\n"
	     "  -C --cs-high      chip select active high\n"
	     "  -3 --3wire        SI/SO signals shared\n"
	     "  -i --iteration    iteration number\n"
	     );
	exit(1);
}

static void parse_opts(int argc, char *argv[])
{
	static const struct option lopts[] = {
		{ "device",      1, 0, 'D' },
		{ "speed",       1, 0, 's' },
		{ "delay",       1, 0, 'd' },
		{ "bpw",         1, 0, 'b' },
		{ "ioctl",       1, 0, 'I' },
		{ "fopt",        1, 0, 'F' },
		{ "loop",        1, 0, 'l' },
		{ "cpha",        1, 0, 'H' },
		{ "cpol",        1, 0, 'O' },
		{ "lsb",         1, 0, 'L' },
		{ "cs-high",     1, 0, 'C' },
		{ "3wire",       1, 0, '3' },
		{ "no-cs",       1, 0, 'N' },
		{ "ready",       1, 0, 'R' },
		{ "iteration",   1, 0, 'i' },
		{ NULL, 0, 0, 0 },
	};
	int c;
	
	while (1) {
		c = getopt_long(argc, argv, "D:s:d:b:I:F:l:H:O:L:C:3:N:R:i:v", lopts, NULL);

		if (c == -1)
			break;

		switch (c) {
		case 'D':
			device = optarg;
			break;
		case 's':
			speed = atoi(optarg);
			conf_flag |= SPI_CONFIG_FLAG_SPEED;
			break;
		case 'd':
			delay = atoi(optarg);
			conf_flag |= SPI_CONFIG_FLAG_DELAY;
			break;
		case 'b':
			bits = atoi(optarg);
			conf_flag |= SPI_CONFIG_FLAG_BPW;
			break;
		case 'I':
			conv(optarg, &rx_len, &tx_len, tx_buf);
			if (rx_len == 0 && tx_len == 0)
				break;
			printf("i - r:%d, t:%d ", rx_len, tx_len);
			printf("\n");
			option_io = 1;
			//conf_flag |= SPI_CONFIG_FLAG_BPW;
			break;
		case 'F':
			conv(optarg, &rx_len, &tx_len, tx_buf);
			if (rx_len == 0 && tx_len == 0)
				break;
			printf("f - r:%d, t:%d ", rx_len, tx_len);
			printf("\n");
			option_file = 1;
			//conf_flag |= SPI_CONFIG_FLAG_BPW;
			break;
		case 'l':
			if (atoi(optarg))
				mode |= SPI_LOOP;
			conf_flag |= SPI_CONFIG_FLAG_LOOP;
			break;
		case 'H':
			if (atoi(optarg))
				mode |= SPI_CPHA;
			conf_flag |= SPI_CONFIG_FLAG_CPHA;
			break;
		case 'O':
			if (atoi(optarg))
				mode |= SPI_CPOL;
			conf_flag |= SPI_CONFIG_FLAG_CPOL;
			break;
		case 'L':
			if (atoi(optarg))
				mode |= SPI_LSB_FIRST;
			conf_flag |= SPI_CONFIG_FLAG_LSB;
			break;
		case 'C':
			if (atoi(optarg))
				mode |= SPI_CS_HIGH;
			conf_flag |= SPI_CONFIG_FLAG_CSHIGH;
			break;
		case '3':
			if (atoi(optarg))
				mode |= SPI_3WIRE;
			conf_flag |= SPI_CONFIG_FLAG_3WIRE;
			break;
		case 'N':
			if (atoi(optarg))
				mode |= SPI_NO_CS;
			conf_flag |= SPI_CONFIG_FLAG_NOCS;
			break;
		case 'R':
			if (atoi(optarg))
				mode |= SPI_READY;
			conf_flag |= SPI_CONFIG_FLAG_READY;
			break;
		case 'i':
			iter = strtoul(optarg, NULL, 16);
			if (!iter)
				iter = 1;
			break;
		default:
			print_usage(argv[0]);
			break;
		}
	}
}


int main(int argc, char **argv)
{
	int		fd;

	memset(tx_buf, 0, sizeof(tx_buf));
	rx_len = tx_len = option_io = option_file = 0;

	parse_opts(argc, argv);

	if (device == NULL) {
		print_usage(argv[0]);
		return -1;
	}

	fd = open(device, O_RDWR);
	if (fd < 0)
		pabort("can't open device");

	printf("OLD Config - ");
	dumpstat(device, fd);
	printf("NEW Config - ");
	updatestat(device, fd);

	if (option_io && (rx_len || tx_len))
		do_msg(fd, rx_len, tx_len, tx_buf);

	if (option_file && (rx_len || tx_len))
		do_read(fd, rx_len, tx_len, tx_buf);

	close(fd);

	return 0;
}
