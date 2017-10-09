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

void cs_fe_stats(int output)
{
	unsigned int i;
        unsigned int bank;
	unsigned int hash_addr;

	unsigned int crc32_0=0;
	unsigned int result_index_0=0;
	unsigned int mask_ptr_0=0;
	unsigned int crc32_1=0;
	unsigned int result_index_1=0;
	unsigned int mask_ptr_1=0;

	unsigned int hash_allocation=0;
	unsigned int overflow_allocation=0;
	unsigned int overflow_crc32;
	unsigned int overflow_crc16;
	unsigned int overflow_result_index;
	unsigned int overflow_mask_ptr;
	unsigned  int *p_value;

        cs_uint32       value;
	unsigned int reg_temp;
	int bank_count[6];

	for (i=0;i<BANKS;i++)
		bank_count[i]=0;

	FETOP_HASH_HASH_MEM_DATA3_t hash_data3;
	FETOP_HASH_HASH_MEM_DATA2_t hash_data2;
	FETOP_HASH_HASH_MEM_DATA1_t hash_data1;
	FETOP_HASH_HASH_MEM_DATA0_t hash_data0;

	FETOP_HASH_OVERFLOW_MEM_DATA0_t overflow_data0;
	FETOP_HASH_OVERFLOW_MEM_DATA1_t overflow_data1;

	for (bank=0;bank<BANKS_ITERATION; bank++)
		for (hash_addr=0;hash_addr<HASH_SIZE;hash_addr++) {
			
			// Access the hash memory
			reg_temp = hash_addr;
			reg_temp += (unsigned int) bank<<12; // The banks are the upper two bits starting at [13:12]
			reg_temp |= 0x80000000;
			wr_access (gp_value, FETOP_HASH_HASH_MEM_ACCESS, reg_temp);

			while ( reg_temp & 0x80000000 ){
				rd_access( gp_value, FETOP_HASH_HASH_MEM_ACCESS, &reg_temp);
			}
			
			// Extract the hash data at that location
			rd_access (gp_value, FETOP_HASH_HASH_MEM_DATA3, &hash_data3.wrd);
			rd_access (gp_value, FETOP_HASH_HASH_MEM_DATA2, &hash_data2.wrd);
			rd_access (gp_value, FETOP_HASH_HASH_MEM_DATA1, &hash_data1.wrd);
			rd_access (gp_value, FETOP_HASH_HASH_MEM_DATA0, &hash_data0.wrd);


			crc32_0=hash_data0.bf.crc32_0;
			result_index_0=hash_data1.bf.result_index_0;
			mask_ptr_0=hash_data1.bf.mask_ptr_0;


			if (mask_ptr_0!=0)
				bank_count[bank*2]++;

			crc32_1=hash_data1.bf.crc32_1;
			crc32_1=hash_data2.bf.crc32_1<<13 | crc32_1;
			result_index_1=hash_data2.bf.result_index_1;
			mask_ptr_1=hash_data3.bf.mask_ptr_1;

			if (mask_ptr_1!=0)
				bank_count[bank*2+1]++;

			if (output) {
				printf("Address/upper bank addr:%d/%d, crc32_0: %4.4x result_idx0:%d mask_ptr0:%d\n",
						hash_addr,bank,crc32_0, result_index_0, mask_ptr_0);
				printf("Address/upper bank addr:%d/%d, crc32_1: %4.4x result_idx1:%d mask_ptr1:%d\n",
						hash_addr,bank,crc32_1, result_index_1, mask_ptr_1);
			}
		}

	for (i=0;i<BANKS;i++) {
		printf("HASH Allocation: bank:%d count:%d\n",
				i, bank_count[i]);
		hash_allocation+=bank_count[i];
	}

	printf("HASH total:%d\n", hash_allocation);


	for (i=0;i<OVERFLOW_COUNT; i++) {
			
			// Access the hash memory
			reg_temp = i;
			reg_temp |= 0x80000000;
			wr_access (gp_value, FETOP_HASH_OVERFLOW_MEM_ACCESS, reg_temp);

			while ( reg_temp & 0x80000000 ){
				rd_access( gp_value, FETOP_HASH_OVERFLOW_MEM_ACCESS, &reg_temp);
			}
			
			// Extract the hash data at that location
			rd_access (gp_value, FETOP_HASH_OVERFLOW_MEM_DATA1, &overflow_data1.wrd);
			rd_access (gp_value, FETOP_HASH_OVERFLOW_MEM_DATA0, &overflow_data0.wrd);


			overflow_crc32=overflow_data0.bf.crc32;
			overflow_crc16=overflow_data1.bf.crc16;
			overflow_result_index=overflow_data1.bf.result_index;
			overflow_mask_ptr=overflow_data1.bf.mask_ptr;

			if (output) {
				printf("Overflow address:%d crc32:%4.4x result_idx:%d mask_ptr:%d\n",
						i,overflow_crc32, overflow_result_index, overflow_mask_ptr);
			}


			if (overflow_mask_ptr!=0)
				overflow_allocation++;

	}

	printf("Overflow allocation:%d\n",
			overflow_allocation);

}
		

