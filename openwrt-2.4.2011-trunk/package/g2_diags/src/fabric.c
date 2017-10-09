#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "registers.h"

#define BANKS 6
#define BANKS_ITERATION BANKS/2
#define BANKS_PER_ADDRESS 2
#define HASH_SIZE 4096
#define OVERFLOW_COUNT 64
#define RESULT_COUNT 8192

extern unsigned int *gp_value;

void cs_pl301_dump(unsigned int base_address)
{
	unsigned int i;
	
	unsigned int qos_tidemark;
	unsigned int qos_access;
	unsigned int qos_ar;
	unsigned int qos_aw;
	unsigned int *p_value;
	unsigned int num_si;
	unsigned int num_mi;
	unsigned int ii;
	unsigned int mcount;

        cs_uint32       value;
	unsigned int reg_temp;
	int bank_count[6];

	unsigned int offset=base_address+0x400;

	printf("PL-301 Base address:%x\n", base_address);
	rd_access( gp_value, base_address+0xfc0,	&num_si);
	rd_access( gp_value, base_address+0xfc4,	&num_mi);
	printf("PL-301 Number of SI (input):%d Number of MI (output):%d\n",
			num_si,num_mi);

	mcount=num_mi;

	for (i=0;i<mcount;i++) {

		rd_access( gp_value, offset+i*0x20, 	&qos_tidemark);
		rd_access( gp_value, offset+i*0x20+0x4, &qos_access);

		printf("Master:%d: QoS Tidemark:%d QoS Access Control:%d\n", 
			i, qos_tidemark, qos_access);

		for (ii=0;ii<32;ii++) {
			reg_temp=(0xFF<<24) | (ii & 0xFF);

			wr_access( gp_value, offset+i*0x20+0x8, reg_temp);
			rd_access( gp_value, offset+i*0x20+0x8, &qos_ar);

			wr_access( gp_value, offset+i*0x20+0xc, reg_temp);
			rd_access( gp_value, offset+i*0x20+0xc, &qos_aw);

			printf("\tSlot:%d, AR MID:%d AW MID:%d\n",
					ii, qos_ar, qos_aw);

		}
	}

}

void cs_pl301_write (unsigned int base_address, unsigned int master, char *fname)
{
	unsigned int i=0;
	
	unsigned int qos_tidemark;
	unsigned int qos_access;
	unsigned int qos_ar;
	unsigned int qos_aw;
	unsigned int *p_value;
	unsigned int num_si;
	unsigned int num_mi;
	unsigned int ii;
	FILE *in;

        cs_uint32       value;
	unsigned int reg_temp;

	unsigned int offset=base_address+0x400;

	printf("PL-301 Base address:%x\n", base_address);

	in=fopen(fname,"r");
	if (in==NULL) {
		printf("Error opening filename %s\n",fname);
		exit(0);
	}

	fscanf(in,"%d %d\n",&qos_tidemark,&qos_access);
	printf("Setting QoS Tidemark of:%d and QoS Access Value of:%d\n",
			qos_tidemark,qos_access);

	wr_access( gp_value, offset+master*0x20,   qos_tidemark);
	wr_access( gp_value, offset+master*0x20+4, qos_access);

	while (!feof(in)) {
		fscanf(in,"%d %d\n",&qos_ar,&qos_aw);
		wr_access( gp_value, offset+master*0x20+0x8, (i & 0xFF) <<24 | (qos_ar & 0xFF));
		wr_access( gp_value, offset+master*0x20+0xc, (i & 0xFF) <<24 | (qos_aw & 0xFF));
		printf("Set slot %d to AR:%d and AW:%d\n",
				i,qos_ar,qos_aw);
		i++;

	}

	fclose(in);
}
