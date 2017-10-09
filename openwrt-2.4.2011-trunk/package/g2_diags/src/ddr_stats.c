#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <strings.h>

#include "registers.h"


extern unsigned int *gp_value;

void gather_bw_stats(int count, int *port_bw)
{
	unsigned int sum;
	SDRAM_DENALI_CTL_400_t ddr_400;
	SDRAM_DENALI_CTL_401_t ddr_401;
	SDRAM_DENALI_CTL_410_t ddr_410;

	int i,p;


	port_bw[0]=0;
	port_bw[1]=0;
	port_bw[2]=0;
	port_bw[3]=0;

	for (i=0;i<count;i++) {
		rd_access(gp_value,SDRAM_DENALI_CTL_400,&ddr_400.wrd);
		port_bw[0]+=ddr_400.bf.AXI0_CURRENT_BDW0;
		port_bw[1]+=ddr_400.bf.AXI1_CURRENT_BDW0;

		rd_access(gp_value,SDRAM_DENALI_CTL_401,&ddr_401.wrd);
		port_bw[2]+=ddr_401.bf.AXI2_CURRENT_BDW1;

		rd_access(gp_value,SDRAM_DENALI_CTL_410,&ddr_410.wrd);
		port_bw[3]+=ddr_410.bf.AXI3_CURRENT_BDW0;
	}
}

void cs_ddr_status(int output, int sample_count)
{
	unsigned int i;

	SDRAM_DENALI_CTL_000_t ddr_000;
	unsigned int version; 		// Offset 0
	unsigned int dram_class;
	unsigned int started;
	unsigned int eight_bank_mode; 	// Offset register 0x70 CTL 0x140

	SDRAM_DENALI_CTL_150_t ddr_150;
	// Denali CTL15_0 register 
	unsigned int priority_en;
	unsigned int placement_en;
	unsigned int bank_split_en;
	unsigned int addr_cmp_en;

	SDRAM_DENALI_CTL_151_t ddr_151;
	// Denali CTL15_1 register 
	unsigned int burst_on_fly_bit;
	unsigned int swap_en;
	unsigned int swap_port_rw_same_en;
	unsigned int rw_same_en;

	SDRAM_DENALI_CTL_161_t ddr_161;
	// Denali CTL16_1 register 
	unsigned int int_status; // Interrupt status

	SDRAM_DENALI_CTL_370_t ddr_370;
	// Denali CTL_37_0: AXI Port configurations
	unsigned int axi0_wr_priority;
	unsigned int axi0_rd_priority;
	unsigned int axi0_narrow_transactions;

	SDRAM_DENALI_CTL_371_t ddr_371;
	// Denali CTL_37_1: AXI Port configurations
	unsigned int axi1_rd_priority;
	unsigned int axi1_narrow_transactions;
	unsigned int axi0_fifo_type;

	SDRAM_DENALI_CTL_380_t ddr_380;
	// Denali CTL_38_0: AXI Port configurations
	unsigned int axi1_wr_priority;
	unsigned int axi2_narrow_transactions;
	unsigned int axi1_fifo_type;

	SDRAM_DENALI_CTL_381_t ddr_381;
	// Denali CTL_38_1: AXI Port configurations
	unsigned int axi2_rd_priority;
	unsigned int axi2_wr_priority;
	unsigned int axi2_fifo_type;

	SDRAM_DENALI_CTL_390_t ddr_390;
	// Denali CTL_39_0: AXI Port configurations
	unsigned int axi3_rd_priority;
	unsigned int axi3_wr_priority;
	unsigned int axi3_narrow_transactions;

	SDRAM_DENALI_CTL_391_t ddr_391;
	// Denali CTL_39_1: AXI Port configurations
	unsigned int axi0_bdw_ovflow;
	unsigned int axi0_bdw;
	unsigned int arb_cmd_q_threshold;
	unsigned int axi3_fifo_type;

	SDRAM_DENALI_CTL_400_t ddr_400;
	// Denali CTL_40_0: AXI Port configurations
	unsigned int axi0_current_bdw;
	unsigned int axi1_current_bdw;
	unsigned int axi1_bdw;
	unsigned int axi1_bdw_ovflow;

	SDRAM_DENALI_CTL_401_t ddr_401;
	// Denali CTL_40_1: AXI Port configurations
	unsigned int axi2_current_bdw;
	unsigned int axi2_bdw;
	unsigned int axi2_bdw_ovflow;
	unsigned int axi3_bdw;

	SDRAM_DENALI_CTL_410_t ddr_410;
	// Denali CTL_41_0: AXI Port configurations
	unsigned int axi3_current_bdw;
	unsigned int axi3_bdw_ovflow;
	unsigned int mem_rst_valid;
	unsigned int cke_status;

	SDRAM_DENALI_CTL_531_t ddr_531;
	// Denali CTL_53_1: Interrupt acknowledgement etc.
	unsigned int int_ack;
	unsigned int update_error_status;

	SDRAM_DENALI_CTL_541_t ddr_541;
	// Denali CTL_54_1: Error status
	unsigned int out_of_range_source_id;
	unsigned int port_cmd_error_id;
	

	unsigned  int *p_value;
        cs_uint32       value;
	unsigned int reg_temp;
	int bank_count[6];
	unsigned int port_bw[4];

	p_value=gp_value;

	rd_access(p_value,SDRAM_DENALI_CTL_000,&ddr_000.wrd);
	printf("DDR Version: %4.4x DDR Class: %4.4x DDR Controlled Enabled:%s\n",
			ddr_000.bf.version0,
			ddr_000.bf.dram_class0,
			(ddr_000.bf.start0) ? "ON" : "OFF");

	rd_access(p_value,SDRAM_DENALI_CTL_150,&ddr_150.wrd);
	printf("Arbitration and command placement\n");
	printf("\tPrioritization: %s Placement: %s Bank Splitting:%s Address Compare: %s\n",
			(ddr_150.bf.PRIORITY_EN0) ? "ON" : "OFF",
			(ddr_150.bf.PLACEMENT_EN0) ? "ON" : "OFF",
			(ddr_150.bf.BANK_SPLIT_EN0) ? "ON" : "OFF",
			(ddr_150.bf.ADDR_CMP_EN0) ? "ON" : "OFF");

	rd_access(p_value,SDRAM_DENALI_CTL_151,&ddr_151.wrd);
	printf("\tSWAP_EN: %s SWAP_PORT_RW_SAME_EN: %s RW_SAME_EN:%s BURST_ON_FLY: %4.4x\n",
			(ddr_151.bf.SWAP_EN1) ? "ON" : "OFF",
			(ddr_151.bf.SWAP_PORT_RW_SAME_EN1) ? "ON" : "OFF",
			(ddr_151.bf.RW_SAME_EN1) ? "ON" : "OFF",
			ddr_151.bf.BURST_ON_FLY_BIT1);

	rd_access(p_value,SDRAM_DENALI_CTL_161,&ddr_161.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_370,&ddr_370.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_371,&ddr_371.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_380,&ddr_380.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_381,&ddr_381.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_390,&ddr_390.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_391,&ddr_391.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_400,&ddr_400.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_401,&ddr_401.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_410,&ddr_410.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_531,&ddr_531.wrd);
	rd_access(p_value,SDRAM_DENALI_CTL_541,&ddr_541.wrd);


	printf("\nAXI priorities are 0== highest, 3==lowest\n");
	printf("\tCPU-> AXI0 Wr Pri:%d AXI0 Rd:%d AXI0 Narrow:%4.4x AXI0_FIFO Type:%s/%d\n",
			ddr_370.bf.AXI0_W_PRIORITY0,
			ddr_370.bf.AXI0_R_PRIORITY0,
			ddr_370.bf.AXI0_EN_SIZE_LT_WIDTH_INSTR0,
			(ddr_371.bf.AXI0_FIFO_TYPE_REG1) ? "Asynchronous" : "Synchronous Ratio",
			ddr_371.bf.AXI0_FIFO_TYPE_REG1);

	printf("\tPER-> AXI1 Wr Pri:%d AXI1 Rd:%d AXI1 Narrow:%4.4x AXI1_FIFO Type:%s/%d\n",
			ddr_380.bf.AXI1_W_PRIORITY0,
			ddr_371.bf.AXI1_R_PRIORITY1,
			ddr_371.bf.AXI1_EN_SIZE_LT_WIDTH_INSTR1,
			(ddr_380.bf.AXI1_FIFO_TYPE_REG0) ? "Asynchronous" : "Synchronous Ratio",
			ddr_380.bf.AXI1_FIFO_TYPE_REG0);


	printf("\tREC-> AXI2 Wr Pri:%d AXI2 Rd:%d AXI2 Narrow:%4.4x AXI2_FIFO Type:%s/%d\n",
			ddr_381.bf.AXI2_W_PRIORITY1,
			ddr_381.bf.AXI2_R_PRIORITY1,
			ddr_380.bf.AXI2_EN_SIZE_LT_WIDTH_INSTR0,
			(ddr_381.bf.AXI2_FIFO_TYPE_REG1) ? "Asynchronous" : "Synchronous Ratio",
			ddr_381.bf.AXI2_FIFO_TYPE_REG1);

	printf("\tQM -> AXI3 Wr Pri:%d AXI3 Rd:%d AXI3 Narrow:%4.4x AXI3_FIFO Type:%s/%d\n",
			ddr_390.bf.AXI3_W_PRIORITY0,
			ddr_390.bf.AXI3_R_PRIORITY0,
			ddr_390.bf.AXI3_EN_SIZE_LT_WIDTH_INSTR0,
			(ddr_391.bf.AXI3_FIFO_TYPE_REG1==0) ? "Asynchronous" : "Synchronous Ratio",
			ddr_391.bf.AXI3_FIFO_TYPE_REG1);



	printf("\nCommand Queue Threshold (ARB_CMD_Q_THRESHOLD):%d\n",
			ddr_391.bf.ARB_CMD_Q_THRESHOLD1);

	printf("\nAXI bandwidth setting in percentages\n");
	printf("\tCPU-> AXI0 Overflow:%s AXI0 BW Setting %%%d\n",
			(ddr_391.bf.AXI0_BDW_OVFLOW1) ? "ON" : "OFF",
			ddr_391.bf.AXI0_BDW1);

	printf("\tPER-> AXI1 Overflow:%s AXI1 BW Setting %%%d\n",
			(ddr_400.bf.AXI1_BDW_OVFLOW0) ? "ON" : "OFF",
			ddr_400.bf.AXI1_BDW0);

	printf("\tREC-> AXI2 Overflow:%s AXI2 BW Setting %%%d\n",
			(ddr_401.bf.AXI2_BDW_OVFLOW1) ? "ON" : "OFF",
			ddr_401.bf.AXI2_BDW1);

	printf("\tREC-> AXI3 Overflow:%s AXI3 BW Setting %%%d\n",
			(ddr_410.bf.AXI3_BDW_OVFLOW0) ? "ON" : "OFF",
			ddr_401.bf.AXI3_BDW1);


	printf("Gather port bandwidth over %d cycles...\n", sample_count);
	gather_bw_stats(sample_count,port_bw);
	printf("  CPU AXI0 %d/100\n", port_bw[0]/sample_count);
	printf("  CPU AXI1 %d/100\n", port_bw[1]/sample_count);
	printf("  CPU AXI2 %d/100\n", port_bw[2]/sample_count);
	printf("  CPU AXI3 %d/100\n", port_bw[3]/sample_count);
}

void cs_ddr_error_read(int count)
{
	unsigned int i;

	unsigned  int *p_value;
        cs_uint32       value;
	unsigned int reg_temp;
	int newly_out_of_range=0;

	char source_port[80];
	char access_type[80];

	SDRAM_DENALI_CTL_000_t ddr_000;
	SDRAM_DENALI_CTL_161_t ddr_161;
	SDRAM_DENALI_CTL_180_t ddr_180;
	SDRAM_DENALI_CTL_181_t ddr_181;
	SDRAM_DENALI_CTL_531_t ddr_531;
	SDRAM_DENALI_CTL_541_t ddr_541;
	// Denali CTL16_1 register 
	unsigned int int_status; // Interrupt status

	p_value=gp_value;

	rd_access(p_value,SDRAM_DENALI_CTL_000,&ddr_000.wrd);
	printf("DDR Version: %4.4x DDR Class: %4.4x DDR Controlled Enabled:%s\n",
			ddr_000.bf.version0,
			ddr_000.bf.dram_class0,
			(ddr_000.bf.start0) ? "ON" : "OFF");

	for (i=0;i<count;i++) {
		rd_access(p_value,SDRAM_DENALI_CTL_161,&ddr_161.wrd);
		rd_access(p_value,SDRAM_DENALI_CTL_180,&ddr_180.wrd);
		rd_access(p_value,SDRAM_DENALI_CTL_181,&ddr_181.wrd);
		rd_access(p_value,SDRAM_DENALI_CTL_541,&ddr_541.wrd);
		rd_access(p_value,SDRAM_DENALI_CTL_531,&ddr_531.wrd);





		printf("Interrupt status: %s\n",
				(ddr_161.bf.INT_STATUS1>>21 & 1) ? "Triggered" : "Not Triggered");

		int_status=ddr_161.bf.INT_STATUS1;

		ddr_531.wrd=0;
		ddr_531.bf.INT_ACK1=int_status;
		wr_access(p_value,SDRAM_DENALI_CTL_531,ddr_531.wrd);

		printf("%s%s%s%s%s%s",
				(ddr_161.bf.INT_STATUS1>>20 & 1) ? "20: DLL Resync Done\n" : "",
				(ddr_161.bf.INT_STATUS1>>19 & 1) ? "19: dfi_init state change\n" : "",
				(ddr_161.bf.INT_STATUS1>>18 & 1) ? "18: INHIBIT_DRAM_CMD set\n" : "",
				(ddr_161.bf.INT_STATUS1>>17 & 1) ? "17: Register write/mode register done\n" : "",
				(ddr_161.bf.INT_STATUS1>>16 & 1) ? "16: Temperature Alert\n" : "",
				(ddr_161.bf.INT_STATUS1>>15 & 1) ? "15: MRR of MR4 Change/(TUF bit set)\n" : "");

		printf("%s%s%s%s%s%s",
				(ddr_161.bf.INT_STATUS1>>14 & 1) ? "14: Register read/mode register done\n" : "",
				(ddr_161.bf.INT_STATUS1>>13 & 1) ? "13: Leveling operation done\n" : "",
				(ddr_161.bf.INT_STATUS1>>12 & 1) ? "12: Leveling operation requested\n" : "",
				(ddr_161.bf.INT_STATUS1>>11 & 1) ? "11: DFI Update Error - check UPDATE_ERROR_STATUS\n" : "",
				(ddr_161.bf.INT_STATUS1>>10 & 1) ? "10: Write leveling error - check WRLVL_ERROR STATUS\n" : "",
				(ddr_161.bf.INT_STATUS1>>9  & 1) ? "9:  Read leveling gate training error - Check RDLVL_ERROR STATUS\n," : "");
		printf("%s%s%s%s%s%s",
				(ddr_161.bf.INT_STATUS1>>8 & 1) ? "8: Read leveling error - check RDLVL_ERROR_STATUS\n" : "",
				(ddr_161.bf.INT_STATUS1>>7 & 1) ? "7: ODT & CAS 3 - Invalid Configuration\n" : "",
				(ddr_161.bf.INT_STATUS1>>6 & 1) ? "6: BIST operation completed\n" : "",
				(ddr_161.bf.INT_STATUS1>>5 & 1) ? "5: MC init completed\n" : "",
				(ddr_161.bf.INT_STATUS1>>4 & 1) ? "4: Error occured on port data channel\n" : "",
				(ddr_161.bf.INT_STATUS1>>3 & 1) ? "3: Error occured on port command channel\n," : "");

		printf("%s%s%s\n",
				(ddr_161.bf.INT_STATUS1>>2 & 1) ? "2: Multiple memory out of physical range errors\n" : "",
				(ddr_161.bf.INT_STATUS1>>1 & 1) ? "1: Memory of out physical range error\n" : "",
				(ddr_161.bf.INT_STATUS1>>0 & 1) ? "0: Memory reset valid on DFI bus\n" : "");

		if ((ddr_161.bf.INT_STATUS1>>2 & 1) || (ddr_161.bf.INT_STATUS1>>1 & 1)) {
			if (!newly_out_of_range) {
				newly_out_of_range=1;
				printf("Out of range access found!  Display faults for each sample below:\n");
			}

			source_port[0]=0;
			switch (ddr_541.bf.OUT_OF_RANGE_SOURCE_ID1>>12 & 3) {
				case 0: strcpy(source_port,"CPU Port"); break;
				case 1: strcpy(source_port,"Peripheral Port"); break;
				case 2: strcpy(source_port,"Recirculation Port"); break;
				case 3: strcpy(source_port,"QM Port"); break;
				default: break;
			}

			access_type[0]=0;
			switch (ddr_181.bf.OUT_OF_RANGE_TYPE1 & 0x1f) {
				case 0: 	strcpy(access_type,"Write: Non-Exclusive"); break;
				case 1: 	strcpy(access_type,"Read:  Non-Exclusive"); break;
				case 2: 	strcpy(access_type,"Write: Non-Exclusive Masked Write"); break;
				case 4: 	strcpy(access_type,"Write: Wrapped"); break;
				case 5: 	strcpy(access_type,"Read:  Wrapped"); break;
				case 6: 	strcpy(access_type,"Write: Wrapped Masked"); break;
				case 8: 	strcpy(access_type,"Write: Exclusive"); break;
				case 9: 	strcpy(access_type,"Read:  Exclusive"); break;
				case 10: 	strcpy(access_type,"Write: Exclusive Masked Write"); break;
				case 16: 	strcpy(access_type,"Write: Flushed"); break;

				default:	strcpy(access_type,"Reserved ORT"); break;

			}

			if (ddr_181.bf.OUT_OF_RANGE_TYPE1>>8 & 1)
				strcat(source_port,"-Auto-precharge");

			printf("Addr:%8.8x,Port_ID:%d,Port Name:%s,Access Type:%s,Length:%d,AXI_ID_thread:%d\n",
					ddr_180.bf.OUT_OF_RANGE_ADDR0,
					ddr_541.bf.OUT_OF_RANGE_SOURCE_ID1>>12 & 0x3,
					source_port,
					access_type,
					ddr_181.bf.OUT_OF_RANGE_LENGTH1,
					ddr_541.bf.OUT_OF_RANGE_SOURCE_ID1 & 0xfff);
		}
	}
}

