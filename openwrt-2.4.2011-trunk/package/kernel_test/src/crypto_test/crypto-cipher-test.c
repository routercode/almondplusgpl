/*
 *      Crypto Linux Driver Test/Example Program
 *
 *      Compile with:
 *		     gcc -s -Wall -Wstrict-prototypes crypto_test.c -o crypto-test
 *
 *      Copyright (C) 1996, Paul Gortmaker.
 *
 *      Released under the GNU General Public License, version 2,
 *      included herein by reference.
 *
 */
#define CONFIG_RWSEM_GENERIC_SPINLOCK


#include <stdio.h>
//#include <sys/ioctl.h>
//#include <sys/time.h>
//#include <sys/types.h>
//#include <fcntl.h>
//#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stddef.h>


#include <sys/socket.h>

//#include <linux/if_alg.h>

#include <linux/types.h>

struct sockaddr_alg {
	   __u16   salg_family;
	   __u8    salg_type[14];
	   __u32   salg_feat;
	   __u32   salg_mask;
	   __u8    salg_name[64];
};

struct af_alg_iv {
	   __u32   ivlen;
	   __u8    iv[0];
};

/* Socket options */
#define ALG_SET_KEY					   1
#define ALG_SET_IV					   2
#define ALG_SET_OP					   3

/* Operations */
#define ALG_OP_DECRYPT				   0
#define ALG_OP_ENCRYPT				   1


#ifndef AF_ALG
#  define AF_ALG 38
#endif

#ifndef SOL_ALG
#  define SOL_ALG 279
#endif


int main(int argc, char **argv)
{
	int opfd;
	int tfmfd;
	struct sockaddr_alg sa = {
		.salg_family = AF_ALG,
		.salg_type = "skcipher",
		.salg_name = "cbc(aes)"
	};
	struct msghdr msg = {};
	struct cmsghdr *cmsg;
	//char cbuf[CMSG_SPACE(4) + CMSG_SPACE(20)];
	u_int32_t type;
	char cbuf[CMSG_SPACE(sizeof(type)) +
				 CMSG_SPACE(offsetof(struct af_alg_iv, iv)+16)];
	char buf[128];
	struct af_alg_iv *iv;
	struct iovec iov;
	int i;

	tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0);

	bind(tfmfd, (struct sockaddr *)&sa, sizeof(sa));

	char key[128] ="\x06\xa9\x21\x40\x36\xb8\xa1\x5b"
		   "\x51\x2e\x03\xd5\x34\x12\x00\x06";


	setsockopt(tfmfd, SOL_ALG, ALG_SET_KEY,
		   key, 16);

	opfd = accept(tfmfd, NULL, 0);

	memset(buf, 0, sizeof(buf));
	memset(cbuf, 0, sizeof(cbuf));

	msg.msg_control = cbuf;
	msg.msg_controllen = sizeof(cbuf);

	cmsg = CMSG_FIRSTHDR(&msg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_OP;
	cmsg->cmsg_len = CMSG_LEN(sizeof(type));
	*(__u32 *)CMSG_DATA(cmsg) = ALG_OP_ENCRYPT;
	cmsg = CMSG_NXTHDR(&msg, cmsg);
	cmsg->cmsg_level = SOL_ALG;
	cmsg->cmsg_type = ALG_SET_IV;
	cmsg->cmsg_len = CMSG_LEN(
				offsetof(struct af_alg_iv, iv) + 16);
	iv = (void *)CMSG_DATA(cmsg);
	iv->ivlen = 16;
	memcpy(iv->iv, "\x3d\xaf\xba\x42\x9d\x9e\xb4\x30"
		       "\xb4\x22\xda\x80\x2c\x9f\xac\x41", 16);


	if (argc > 1)
	{
		if (((strlen(argv[1]) % 16)!=0)||(strlen(argv[1])>128))
		{
			printf("input string must be 16 bytes alignment or len must <128\n");
			close(opfd);
			close(tfmfd);
			return 1;
		}
		iov.iov_base = argv[1];
		iov.iov_len = strlen(argv[1]);
	}
	else
	{
		iov.iov_base = "Single block msg";
		iov.iov_len = 16;
	}

	msg.msg_iov = &iov;
	msg.msg_iovlen = 1;

	int len=sendmsg(opfd, &msg, 0);

	if ( (len > 128) || (iov.iov_len!=len))
	{
		close(opfd);
		close(tfmfd);
		printf("input len (%d) is not equal to return len (%d) \n",iov.iov_len,len);
		return 1;
	}

	read(opfd, buf, len);

	if (argc > 1) {

		for (i = 0; i < len; i++) {
			printf("%02x", (unsigned char)buf[i]);
		}
		printf("\n");
	}

	close(opfd);
	close(tfmfd);

	return 0;
}


