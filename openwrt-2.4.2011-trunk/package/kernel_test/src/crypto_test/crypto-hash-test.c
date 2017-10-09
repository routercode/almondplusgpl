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
#include <stdlib.h>
#include <errno.h>


#include <sys/socket.h>

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

#define AF_ALG 38

#define SOL_ALG 279

#endif



int main(int argc, char **argv)

{

    int opfd;

    int tfmfd;

    struct sockaddr_alg sa = {

        .salg_family = AF_ALG,

        .salg_type = "hash",

        .salg_name = "sha1"

    };

    char buf[20];

    int i;



    tfmfd = socket(AF_ALG, SOCK_SEQPACKET, 0);



    bind(tfmfd, (struct sockaddr *)&sa, sizeof(sa));



    opfd = accept(tfmfd, NULL, 0);


	if (argc > 1) {

    	write(opfd, argv[1], strlen(argv[1]));
	}
	else
	{
		write(opfd, "abc", 3);
	}

    read(opfd, buf, 20);


	if (argc > 1) {

    	for (i = 0; i < 20; i++) {

 	       printf("%02x", (unsigned char)buf[i]);

    	}

    	printf("\n");
	}


    close(opfd);

    close(tfmfd);



    return 0;

}

