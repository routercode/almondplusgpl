#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "ddr_stats.h"
#include "post.h"
#include "registers.h"
#include "general.h"
#include "errorlog.h"
#include "ni_linked_list_test.h"
#include "dct/dct.h"

#define READ 0
#define WRITE 1
#define READM 2
#define WRITEM 3

#define MAP_SIZE  0X8000000  // 32 MB
#define PAGE_SIZE 0X1000  // 4KB

#define RRAM0_ADDR 0xf6200000
#define RRAM0_SIZE 32768

#define RRAM1_ADDR 0xf6400000
#define RRAM1_SIZE 32768

#define PKTBUF_ADDR 0xf6a00000
#define PKTBUF_SIZE 262144

#define RCPU0_IRAM_ADDR 0xF6008000
#define RCPU0_IRAM_SIZE 24576

#define RCPU1_IRAM_ADDR 0xF6028000
#define RCPU1_IRAM_SIZE 24576

#define RCPU0_DRAM_ADDR 0xF6000000
#define RCPU0_DRAM_SIZE 8192

#define RCPU1_DRAM_ADDR 0xF6020000
#define RCPU1_DRAM_SIZE 8192

#define BASE_ADDR 0xF0000000
#define G2_CPU_VOQ_START 48
#define G2_CPU_VOQ_COUNT 64

#define CS_PRINT_MAGIC		0xbabeface
#define CS_PRINT_BUF_SIZE	(0x8000 - sizeof(cs_print_ring_t))

int debug_access=2;
unsigned int *gp_value=NULL;
int mem_fd = 0;

typedef struct cs_print_ring_s {
	unsigned int pmagic;
	unsigned int w_off;
	unsigned int buf_size;
}cs_print_ring_t;

void printhelp()
{
   printf("G2 Diags\r\n");
   printf("=>g2_diags <test #>\r\n");
   printf(" <test #> - \r\n");
   printf("        1 - QM Linked List Check - Autodetects INTERNAL/EXTERNAL queue settings\r\n");
   printf("        2 - NI Linked List Check\r\n");
   printf("        3 - RRAM0/1 Memory Tests\r\n");
   printf("        4 - PKTBUF Memory Test\r\n");
   printf("        5 - RCPU0 I-RAM Memory Test\r\n");
   printf("        6 - RCPU0 D-RAM Memory Test\r\n");
   printf("        7 - RCPU1 I-RAM Memory Test\r\n");
   printf("        8 - RCPU1 D-RAM Memory Test\r\n");
   printf("        9 - Arbitrary memory address: give start & size in hex\r\n");
   printf("       10 - Disk/RAMdisk stress test - multithreaded\r\n");
   printf("       11 - QM Linked List Check - Do not halt traffic -  same as #1\r\n");
   printf("       12 - Run all internal memory tests\r\n");
   printf("        ------------ FE Tests/Statistics\r\n");
   printf("       13 - FE Hash and Overflow Statistics - no debug output\r\n");
   printf("       14 - FE Hash and Overflow Statistics - with debug output\r\n");
   printf("       ------------- Generic Read/Write Commands\r\n");
   printf("       15 - read          32-bit data,  e.g. g2_diags -15 r 0xf0000000\r\n");
   printf("       15 - read multiple 32-bit data,  e.g. g2_diags -15 rm 0xf0000000 <count>\r\n");
   printf("       15 - write 32-bit data,  e.g. g2_diags -15 w 0xf0000000 0xbeef\r\n");
   printf("       15 - write 32-bit data,  e.g. g2_diags -15 wm 0x000000000 <data> <count> \r\n");
   printf("       ------------- DDR Commands\r\n");
   printf("       30 - Read config & bandwidth over 'x' samples e.g. g2_diags -30 <sample_count>\r\n");
   printf("       31 - Check DDR Interrupt status and read bad accesses e.g. g2_diags -31 <count>\r\n");
   printf("       ------------- PL-301 Commands\r\n");
   printf("       40 - Dump Fabric Configuration at address: e.g. g2_diags -40 0xf4923000\r\n");
   printf("       41 - Set Fabric Configuration at address: e.g. g2_diags -40 0xf49230000 <master_id> <config_fname>\r\n");
   printf("                 <config_fname> - first pair of values are QoS Tidmark, QoS Access Control Reg\r\n");
   printf("                                - subsequent pairs are AR, AW Master IDs.  For example:\r\n");
   printf("                                - 1 0       ; tidemark, access control\r\n");
   printf("                                - 3 3       ; AR/AW values of 3,3 for slot 0\r\n");
   printf("                                - 2 2       ; AR/AW values of 2,2 for slot 1\r\n");
   printf("                                - 0 0       ; AR/AW values of 0,0 for slot 2\r\n");
   printf("       ------------- PE DEBUG Commands\r\n");
   printf("       50 - [-c] Dump WFO RCPU0 debug message, '-c' resets the buffer g2_diags -50 [-c]\r\n");
   printf("       51 - [-c] Dump WFO RCPU1 debug message, '-c' resets the buffer g2_diags -51 [-c]\r\n");
   printf("       52 - Dump IPsec RCPU0 debug message\r\n");
   printf("       53 - Dump IPsec RCPU1 debug message\r\n");

  return;
}

unsigned int rd_access(unsigned  int *p_value, unsigned int addr, unsigned int *rd_data)
{
  unsigned int offset;
  unsigned int *rd_ptr;


  if((addr%4) != 0) {
     LOG(ERR("rd_access: Address should be word aligned \r\n"));
     return 0;
  }

  offset  = addr - 0xf0000000;
  rd_ptr = p_value + (offset/4);
  //rd_ptr=(unsigned int *)offset;

  //LOG(DBG("rd_access: Read Address 0x%x\n",rd_ptr));
  
  *rd_data = *rd_ptr;
  //LOG(DBG("rd_Access: Read Data 0x%x\n",*rd_data));
  return (*rd_data);
}

int wr_access(unsigned  int *p_value, unsigned int addr, unsigned int wr_data)
{
  unsigned  int *wr_ptr;
  unsigned int offset;

  if((addr%4) != 0) {
     LOG(ERR("rd_access: Address should be word aligned \r\n"));
     return 1;
  }

  offset  = addr - 0xf0000000;
  wr_ptr = p_value + (offset/4);

  //LOG(DBG("wr_access: Write Address 0x%8.8x with data:%8.8x\n",wr_ptr,wr_data));
  *wr_ptr = wr_data;

  return 0;
}

QM_STATUS_0_t                          qm_status_0;
QM_STATUS_1_t                          qm_status_1;
QM_INGRESS_STATUS_PRIMARY_PATH0_t      qm_ingress_status_primary_path0;
QM_INGRESS_STATUS_PRIMARY_PATH1_t      qm_ingress_status_primary_path1;
QM_INGRESS_STATUS_CPU_PATH0_t          qm_ingress_status_cpu_path0;
QM_INGRESS_STATUS_CPU_PATH1_t          qm_ingress_status_cpu_path1;
QM_EGRESS_STATUS_0_t                   qm_egress_status_0;
QM_EGRESS_STATUS_1_t                   qm_egress_status_1;
QM_CPU_PATH_CONFIG_0_t                 qm_cpu_path_config_0;
QM_CPU_PATH_CONFIG_1_t                 qm_cpu_path_config_1;

// Internal Buffer Information
QM_INT_BUF_CONFIG_0_t			qm_int_buf_config_0;
QM_INT_BUF_CONFIG_1_t			qm_int_buf_config_1;
QM_INT_BUF_STATUS_0_t			qm_int_buf_status_0;

// By default we don't try to do internal linked list checking
// However in later releases we will read the QM Internal memory status
// to see if it has been configured and allow or disallow it based on that
// status
int int_test=0;

unsigned int total_internal_buffer_count=0;
unsigned int use_internal_for_recirc=0;
unsigned int internal_buffer_size=0;
unsigned int internal_free_buffer_count=0;

unsigned int buf_access=0;
unsigned int buf_data=0;


int check_voq_internal(int voq)
{
	if ((int_test==1 && use_internal_for_recirc==1 && voq<48) || 
			(int_test==1 && use_internal_for_recirc==0 && (voq<24 || (voq>=40 && voq<48))))
			return 1;
	else
			return 0;
}

int memory_test_region(
		int type,
		unsigned int start_address,
		unsigned int size,
		char *test_string)
{
	char test_str[128];
	void *usr_mapmem = NULL;
	unsigned  int *p_value;

	strcpy(test_str,test_string);

	usr_mapmem = (unsigned char*)mmap((void *)0x0,MAP_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,mem_fd,start_address);
	LOG(DBG("Peripherals base_addr =0x%4.4x virt_addr = 0x%x\n", start_address,usr_mapmem));
	p_value = usr_mapmem;
	gp_value = p_value;

	g2_mem_test(type, (void*) p_value, size, test_str);

}


int main(int argc, char **argv)
{
	void *usr_mapmem = NULL;
	char file_name[80];
	unsigned int addr;
	int rw_op;
	unsigned int value;
	unsigned int count;
	int s_count;
	unsigned long length;
	unsigned  int *p_value;
	unsigned int offset;
	unsigned int reg_temp, reg_temp2;
	unsigned int my_timer_set, my_timer_val;
	unsigned int lo_addr_lim, hi_addr_lim;
	unsigned int bypass_data, bypass_cpu;
	unsigned int parsed_start_addr;
	unsigned int parsed_addr_length;

	unsigned int mem_op=READ;
	unsigned int address=0, data=0;
	unsigned int temp_addr;

	int ports;
	int stop_traffic=1;
	int total_buf_count=0;
	int loop;
	int next_buffer;
	char mem_test[80];


	bypass_data= 0;
	bypass_cpu=0;

	/* open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR) ) < 0) {
		LOG(ERR("FATAL: can't open /dev/mem\n"));
		exit (-1);
	}

	if ((argc<2) || ( argc==2 && strcmp(argv[1],"-h")==0)) {
		printhelp();
		return 1;
	}

	lo_addr_lim = 0x00000000;
	hi_addr_lim = 0x40000000;

	if ((argc >= 2) && ((strcmp(argv[1], "-50") == 0) ||
				(strcmp(argv[1], "-51") == 0))) {
		int i, off;
		char *ptr;

		if (strcmp(argv[1], "-50") == 0) {
			printf("Dump RCPU0 message\n");
			address = 0xf6404000;
		} else {
			printf("Dump RCPU1 message\n");
			address = 0xf6404800;
		}
		temp_addr = address & 0xFFFFF000;
		usr_mapmem = (unsigned char*)mmap((void *)0x0, 0x1000,
				PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd,
				temp_addr);
		if (usr_mapmem == 0) {
			printf("Unable to map memory for address: 0x%8.8x\n",
					address);
			exit(0);
		}

		p_value = usr_mapmem + (address & 0xFFF);

		/* Check magic */
		if (*p_value != CS_PRINT_MAGIC) {
			printf("Magic is not match\n");
			exit(0);
		}

		off = *(p_value+1);
		ptr = p_value+3;

		while(ptr < (char *)p_value + off) {
			printf("%c", *ptr);
			ptr++;
		}
		if ((argc == 3) && ((strcmp(argv[2], "-c") == 0))) {
			/* Clean buffer */
			ptr = p_value+3;
			*ptr = 0;
			*(p_value+1) = 12;
		}
		exit(0);
	}

	if ((argc == 2) && ((strcmp(argv[1], "-52") == 0) || 
				(strcmp(argv[1], "-53") == 0))) {
		int i, off;
		char *ptr;

		if (strcmp(argv[1], "-52") == 0) {
			printf("Dump RCPU0 message\n");
			address = 0x01f00000;
		} else {
			printf("Dump RCPU1 message\n");
			address = 0x01f08000;
		}
		temp_addr = address & 0xffff0000;
		usr_mapmem = (unsigned char*)mmap((void *)0x0, 0x10000, 
				PROT_READ | PROT_WRITE, MAP_SHARED, mem_fd, 
				temp_addr);
		if (usr_mapmem == 0) {
			printf("Unable to map memory for address: 0x%8.8x\n", 
				address);
			exit(0);
		}

		p_value = usr_mapmem + (address & 0xffff);

		/* Check magic */
		if (*p_value != CS_PRINT_MAGIC) {
			printf("Magic is not match\n");
			exit(0);
		}

		off = *(p_value+1);
		ptr = p_value+3;

		while(ptr < (char *)p_value + off) {
			printf("%c", *ptr);
			ptr++;
		}

		/* Clean buffer */
		ptr = p_value+3;
		*ptr = 0;
		*(p_value+1) = 12;

		exit(0);
	}
	
	// Memory read/write 32-bit only
	if ((argc>=2) && (strcmp(argv[1],"-15")==0)) {
		if (argc>=4) {
			sscanf(argv[3],"%x",&address);

			if (strcmp(argv[2],"r")==0) 
				mem_op=READ;
			else if (strcmp(argv[2],"w")==0) {
				sscanf(argv[4],"%x",&data);
				mem_op=WRITE;
			} else if (strcmp(argv[2],"rm")==0) {
				mem_op=READM;
				sscanf(argv[4],"%d",&length);
			} else if (strcmp(argv[2],"wm")==0) {
				mem_op=WRITEM;
				sscanf(argv[4],"%x",&data);
				sscanf(argv[5],"%d",&length);
			} else {
				printf("Invalid operation, must be r for read, w for write, rm for read multiple and wm for write multiple\r\n");
				exit(0);
			}
		} else {
			printf("Invalid number of commands for reading or writing memory!\r\n");
			exit(0);
		}

		temp_addr=address & 0xffff0000;
		usr_mapmem = (unsigned char*)mmap((void *)0x0,0x10000,PROT_READ | PROT_WRITE,MAP_SHARED,mem_fd,temp_addr);
		if (usr_mapmem==0) {
			printf("Unable to map memory for address: 0x%8.8x\n",address);
			exit(0);
		}

		p_value = usr_mapmem + (address & 0xffff);

		switch (mem_op) {

			case WRITE:  
				*p_value=data;
				printf("Wrote addr=>data  0x%8.8x==>0x%8.8x\n", address, data);
				exit(0);
				break;

			case READ:
				printf("Read addr=>data  0x%8.8x==>0x%8.8x\n", address, *p_value);
				exit(0);
				break;

			case READM:
				s_count = length;

				if (length <= 0) {
					printf("count must be a positive number\n");
					exit(0);
				}
				while (s_count-- > 0) {
					printf("0x%8.8x: 0x%8.8x\n", address, *p_value);
					p_value++;
					address += 4;
				}
				exit(0);
				break;

			case WRITEM:
				s_count = length;

				if (length <= 0) {
					printf("count must be a positive number\n");
					exit(0);
				}
				while (s_count-- > 0) {
					*p_value=data;
					p_value++;
				}
				exit(0);
				break;
		}
	}

	usr_mapmem = (unsigned char*)mmap((void *)0x0,MAP_SIZE,PROT_READ | PROT_WRITE,MAP_SHARED,mem_fd,0xf0000000);
	printf("Peripherals base_addr =0xf0000000 virt_addr = 0x%x\n", usr_mapmem);
	p_value = usr_mapmem;
	gp_value = p_value;

	if ((argc==3) && (strcmp(argv[1],"-30")==0)) {
		sscanf(argv[2],"%d",&count);
		cs_ddr_status(count,count);
		exit(0);
	}

	if ((argc==3) && (strcmp(argv[1],"-31")==0)) {
		sscanf(argv[2],"%d",&count);
		cs_ddr_error_read(count);
		exit(0);
	}


	// Open logs to the screen
	open_logs(stdout,0,0);

	if ((argc>=2) && (strcmp(argv[1],"-40")==0)) {
		sscanf(argv[2],"%x",&parsed_start_addr);
		cs_pl301_dump(parsed_start_addr);
		exit(0);
	}

	if ((argc>=3) && (strcmp(argv[1],"-41")==0)) {
		sscanf(argv[2],"%x",&parsed_start_addr);
		sscanf(argv[3],"%d",&count);
		sscanf(argv[4],"%s",&file_name);
		printf("%x %d %s\n",parsed_start_addr,count,file_name);
		cs_pl301_write(parsed_start_addr,count,file_name);
		exit(0);
	}


	if (argc==2) {

		if (strcmp(argv[1],"-13")==0) {
			printf("FE Statistics:\n");
			cs_fe_stats(0);
			exit(0);
		}

		if (strcmp(argv[1],"-14")==0) {
			printf("FE Statistics:\n");
			cs_fe_stats(1);
			exit(0);
		}

		if (strcmp(argv[1],"-12")==0) {
			printf("Running all internal memory tests.\r\n");
			memory_test_region( 1, RRAM0_ADDR, RRAM0_SIZE, "Memory testing of RRAM0");
			memory_test_region( 1, RRAM1_ADDR, RRAM1_SIZE, "Memory testing of RRAM1");

			memory_test_region( 1, PKTBUF_ADDR, PKTBUF_SIZE, "Memory testing of Packet Buffer 256kB");
			// Put the RCPUs in run/stall
			wr_access( p_value, GLOBAL_RECIRC_CPU_CTL, 0x129);

			// Pull the RCPUs out of reset
			rd_access( p_value, GLOBAL_BLOCK_RESET, &reg_temp);
			reg_temp&=0xff9fffff;
			wr_access( p_value, GLOBAL_BLOCK_RESET, reg_temp);

			memory_test_region( 2, RCPU0_IRAM_ADDR, RCPU0_IRAM_SIZE, "Memory testing of RCPU0 I-RAM");
			memory_test_region( 1, RCPU0_DRAM_ADDR, RCPU0_DRAM_SIZE, "Memory testing of RCPU0 D-RAM");
			memory_test_region( 2, RCPU1_IRAM_ADDR, RCPU1_IRAM_SIZE, "Memory testing of RCPU1 I-RAM");
			memory_test_region( 1, RCPU1_DRAM_ADDR, RCPU1_DRAM_SIZE, "Memory testing of RCPU1 D-RAM");
			close_logs();
			return 0;			

		}
		if ((strcmp(argv[1],"-1")==0) || (strcmp(argv[1],"-11")==0)) {
			printf("QM External Queue Diagnostic Link List Checker\r\n");
      			my_timer_set=0x1;
			
			if (strcmp(argv[1],"-11")==0) {
				stop_traffic=0;
			}
		}

		if (strcmp(argv[1],"-2")==0) {
			printf("NI Buffer Link Test Checker\r\n");
			cs_ni_link_list_diag();
		}
		
		if (strcmp(argv[1],"-3")==0) {
			memory_test_region( 1, RRAM0_ADDR, RRAM0_SIZE, "Memory testing of RRAM0");
			memory_test_region( 1, RRAM1_ADDR, RRAM1_SIZE, "Memory testing of RRAM1");
			close_logs();
			return 0;
		}

		if (strcmp(argv[1],"-4")==0) {
			memory_test_region( 1, PKTBUF_ADDR, PKTBUF_SIZE, "Memory testing of Packet Buffer 256kB");
			close_logs();
			return 0;
		}

		if (strcmp(argv[1],"-5")==0) {

			// Put the RCPUs in run/stall
			wr_access( p_value, GLOBAL_RECIRC_CPU_CTL, 0x129);

			// Pull the RCPUs out of reset
			rd_access( p_value, GLOBAL_BLOCK_RESET, &reg_temp);
			reg_temp&=0xff9fffff;
			wr_access( p_value, GLOBAL_BLOCK_RESET, reg_temp);

			memory_test_region( 2, RCPU0_IRAM_ADDR, RCPU0_IRAM_SIZE, "Memory testing of RCPU0 I-RAM");
			close_logs();
			return 0;
		}
		if (strcmp(argv[1],"-6")==0) {
			// Put the RCPUs in run/stall
			wr_access( p_value, GLOBAL_RECIRC_CPU_CTL, 0x129);

			// Pull the RCPUs out of reset
			rd_access( p_value, GLOBAL_BLOCK_RESET, &reg_temp);
			reg_temp&=0xff9fffff;
			wr_access( p_value, GLOBAL_BLOCK_RESET, reg_temp);

			memory_test_region( 1, RCPU0_DRAM_ADDR, RCPU0_DRAM_SIZE, "Memory testing of RCPU0 D-RAM");
			close_logs();
			return 0;
		}

		if (strcmp(argv[1],"-7")==0) {
			// Put the RCPUs in run/stall
			wr_access( p_value, GLOBAL_RECIRC_CPU_CTL, 0x129);

			// Pull the RCPUs out of reset
			rd_access( p_value, GLOBAL_BLOCK_RESET, &reg_temp);
			reg_temp&=0xff9fffff;
			wr_access( p_value, GLOBAL_BLOCK_RESET, reg_temp);

			memory_test_region( 2, RCPU1_IRAM_ADDR, RCPU1_IRAM_SIZE, "Memory testing of RCPU1 I-RAM");
			close_logs();
			return 0;
		}
		if (strcmp(argv[1],"-8")==0) {
			// Put the RCPUs in run/stall
			wr_access( p_value, GLOBAL_RECIRC_CPU_CTL, 0x129);

			// Pull the RCPUs out of reset
			rd_access( p_value, GLOBAL_BLOCK_RESET, &reg_temp);
			reg_temp&=0xff9fffff;
			wr_access( p_value, GLOBAL_BLOCK_RESET, reg_temp);

			memory_test_region( 1, RCPU1_DRAM_ADDR, RCPU1_DRAM_SIZE, "Memory testing of RCPU1 D-RAM");
			close_logs();
		}

		if (strcmp(argv[1],"-10")==0) {
			dct_main();
			close_logs();
			return 0;
		}

	} else if (argc==4) {
		if (strcmp(argv[1],"-9")==0) {
			parsed_start_addr=sscanf(argv[2],"%x");
			parsed_addr_length=sscanf(argv[3],"%x");

			printf("Beginning memory test of address %8.8x with size of %8.8x\r\n",parsed_start_addr,parsed_addr_length);
			memory_test_region( 1, parsed_start_addr, parsed_addr_length, "Arbitrary region");
			close_logs();
			return 0;
		}
	} 
	my_timer_set=0x1;

	unsigned int int_free_bank_cnt=0;
	unsigned int int_free_bank_head=0;
	unsigned int int_free_bank_tail=0;

	unsigned int free_buf_reg;
	unsigned int cpu_free_buf_reg;
	unsigned int free_bank_cnts[8];
	unsigned int free_bank_heads[8];
	unsigned int free_bank_tails[8];
	unsigned int cpu_free_cnts[8];
	unsigned int cpu_free_heads[8]; 

	unsigned int int_sum_cnt, sum_bank_cnt, cpu_sum_free_cnt;
	unsigned int hdr_err_cnt_val, hdr_err_cnt_check;
	unsigned int error_flag, cpu_error_flag;
	unsigned int qm_cpu_path_total_linux_buffers;
	unsigned int allocated_cpu_buffers;

	int b, i, j;
  
	error_flag = 0;
	cpu_error_flag = 0;
	/* 
	 * determine sampling event
	 * either detect new header CRC errors OR
	 * Timeout
	 */

	rd_access(p_value,QM_STATUS_0,&(qm_status_0.wrd));
	LOG(DBG("QM Status word0:%8.8x\n",qm_status_0.wrd));
	LOG(DBG("QM Status word0: init done:%d, pkt_banks:%d, cpu_banks:%d, buffers:%d, free_buffers:%d\n",
		qm_status_0.bf.init_done, 
		qm_status_0.bf.pkt_banks, 
		qm_status_0.bf.cpu_banks,
		qm_status_0.bf.buffers,
		qm_status_0.bf.free_buffers));

	rd_access( p_value, QM_EGRESS_STATUS_0, &(qm_egress_status_0.wrd));  /* get the current count */
	hdr_err_cnt_val= qm_egress_status_0.bf.pkt_hdr_uncorrected_errs + qm_egress_status_0.bf.pkt_hdr_corrected_errs;
	hdr_err_cnt_check = hdr_err_cnt_val;
	my_timer_val = my_timer_set;

	rd_access( p_value, QM_EGRESS_STATUS_0, &(qm_egress_status_0.wrd));  /* get the current count */
	hdr_err_cnt_check=qm_egress_status_0.bf.pkt_hdr_uncorrected_errs + qm_egress_status_0.bf.pkt_hdr_corrected_errs;

	if ( hdr_err_cnt_check != hdr_err_cnt_val ) {
		LOG(DBG("Header CRC Trigger: old %d, new %d \r\n",hdr_err_cnt_val, hdr_err_cnt_check));
	}

	/* NI: Stop Traffic from Rx FIFO for all eight ports*/
	if (stop_traffic)
		for (ports=0;ports<8;ports++) {
			rd_access( p_value, NI_TOP_NI_RX_CNTRL_CONFIG0_0+(8*ports), &reg_temp);
			wr_access( p_value, NI_TOP_NI_RX_CNTRL_CONFIG0_0+(8*ports), (reg_temp | 0x2) );
		}
 
	/* insert delay to allow queues to drain before cripliing the QM */
	for( my_timer_val=0; my_timer_val<0x2000000; my_timer_val++) {
		b++;
	}

	/* QM: need to turn on SW access to the buffer list */
	rd_access( p_value, QM_CONFIG_0, &reg_temp);
	// MSB turns on CPU access to buffer lists
	wr_access( p_value, QM_CONFIG_0, (reg_temp|0x80000000) );

	/* QM: first read the total free buffer count */
	rd_access( p_value, QM_STATUS_0, &free_buf_reg);
	free_buf_reg = free_buf_reg>>16;
	printf("Total Free Count 0x%x, %d\r\n",free_buf_reg,free_buf_reg);

	/* QM: Check to see if we have internal queue status checking enabled and if
	* we do check to see if it is actually enabled
	*/
	rd_access( p_value, QM_INT_BUF_CONFIG_0, &qm_int_buf_config_0.wrd);
	printf("QM Internal mode is: %s\n", (qm_int_buf_config_0.bf.use_internal) ? "ON" : "OFF");
	int_test=qm_int_buf_config_0.bf.use_internal;

	if (int_test)
		printf("\t Since internal queues are on the accounting of internal buffers will take place.\r\n");

	// We add one to the total buffer count because zero or
	// wherever the starting position is a usable buffer
	total_internal_buffer_count=
					qm_int_buf_config_0.bf.last_buffer_addr-
					qm_int_buf_config_0.bf.first_buffer_addr+1;

	use_internal_for_recirc=qm_int_buf_config_0.bf.use_internal_for_recirc;
	internal_buffer_size=qm_int_buf_config_0.bf.buffer_size;
	printf("Total internal buffer count:%d\n",total_internal_buffer_count);
	printf("Use internal memory for recirculation CPU:%s\n",(use_internal_for_recirc) ?
		"YES" : "NO");
			  
	rd_access( p_value, QM_INT_BUF_STATUS_0, &qm_int_buf_status_0.wrd);
	internal_free_buffer_count=qm_int_buf_status_0.bf.free_buffers;
  
	/* QM:QM_CPU_PATH_CONFIG_0  */
  	rd_access( p_value, QM_CPU_PATH_CONFIG_0, &qm_cpu_path_config_0.wrd);
  	qm_cpu_path_total_linux_buffers = qm_cpu_path_config_0.bf.cpu_buffers;
	printf("Maximum allocatable CPU buffers in linux mode is 0x%x %d\r\n",
		qm_cpu_path_total_linux_buffers,
		qm_cpu_path_total_linux_buffers);

	/* QM: first read the total CPU free buffer count */
	rd_access( p_value, QM_CPU_PATH_STATUS_0, &cpu_free_buf_reg);
	cpu_free_buf_reg = cpu_free_buf_reg>>16;
	printf("Total CPU allocatable or empty CPU pointers 0x%x %d\r\n",cpu_free_buf_reg,cpu_free_buf_reg);

	allocated_cpu_buffers=qm_cpu_path_total_linux_buffers-cpu_free_buf_reg;
	printf("Allocated CPU Buffers is then:%d\n",
		allocated_cpu_buffers);

	/* QM: scan internal buffer counts */
	int_sum_cnt= 0;

	/* QM: read bank buffer count */
	reg_temp = (unsigned int) 128; // Read the free buffers for internal packet free buffers
	reg_temp |= 0x80000000;
	wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);

	while ( reg_temp & 0x80000000 ){
		rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	}
	rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
  	int_free_bank_cnt = reg_temp;
  	int_sum_cnt += reg_temp;

	/* QM: read bank heads - internal */
	reg_temp = (unsigned int) 129; // Read head which is the next value over from bank buffer count
	reg_temp |= 0x80000000;
	wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);

	while ( reg_temp & 0x80000000 ){
		rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	}
	rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
	int_free_bank_head = reg_temp;

	/* QM: read bank tails - internal */
	reg_temp = (unsigned int) 130;
	reg_temp |= 0x80000000;
	wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);
	while ( reg_temp & 0x80000000 ){
		rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	}

	rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
	int_free_bank_tail = reg_temp;

	printf("INTERNAL Free Count %d, Bank Aggregate Free Count %d \r\n", internal_free_buffer_count, int_sum_cnt);
	if ( internal_free_buffer_count != int_sum_cnt ) {
		error_flag++;
		LOG(ERR("ERROR: VoQ stats error, free count mismatch INTERNAL %d but Aggr %d\r\n", internal_free_buffer_count, int_sum_cnt));
	}

	/* QM: scan per bank buffer counts */
	sum_bank_cnt = 0;
	for( b=0; b<8; b++) {

		/* QM: read bank buffer count */
		reg_temp = (unsigned int) b*4; // Read the free buffers from each bank bank*4 for free buffers
		reg_temp |= 0x80000000;
		wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);

		while ( reg_temp & 0x80000000 ){
			rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
		}
		rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
		free_bank_cnts[b] = reg_temp;
		sum_bank_cnt += reg_temp;

		/* QM: read bank heads */
		reg_temp = (unsigned int) b*4+1; // Read head which is the next value over from bank buffer count
		reg_temp |= 0x80000000;
		wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);

		while ( reg_temp & 0x80000000 ){
			rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
		}

		rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
		free_bank_heads[b] = reg_temp;

		/* QM: read bank tails */
		reg_temp = (unsigned int) b*4+2;
		reg_temp |= 0x80000000;
		wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);
		while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
		}
		rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
		free_bank_tails[b] = reg_temp;
  	}

  printf("Global Free Count %d, Bank Aggregate Free Count %d \r\n", free_buf_reg, sum_bank_cnt);
  if ( free_buf_reg != sum_bank_cnt ) {
    error_flag++;
    LOG(ERR("ERROR: VoQ stats error, free count mismatch Global %d but Aggr %d\r\n", free_buf_reg, sum_bank_cnt));
  }

  /* scan CPU Free lists buffer counts, linux0, linux1, non-specified */
  cpu_sum_free_cnt = 0;
  for( b=0; b<8; b++) {

	  reg_temp = (unsigned int) b*4 + 64;
	  reg_temp |= 0x80000000;
	  wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);
	  while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	  }
	  rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
	  cpu_free_cnts[b] = reg_temp;
	  cpu_sum_free_cnt += reg_temp;

 	  reg_temp = (unsigned int) b*4+1+64;
	  reg_temp |= 0x80000000;
	  wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);
	  while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	  }
	  rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
	  cpu_free_heads[b] = reg_temp;
 }

  // Need to fix this up to add the number of allocated buffers + free buffers  = total of QM_CPU_PATH_CONFIG_0
  printf("Global CPU Available Allocatable Count %d, CPU Aggregate Allocated Unused Count %d \r\n", cpu_free_buf_reg, cpu_sum_free_cnt);
  if ( (cpu_free_buf_reg+cpu_sum_free_cnt) != qm_cpu_path_total_linux_buffers) {
    LOG(ERR("ERROR: CPU stats error, free count mismatch Global %d but Aggr %d\r\n", cpu_free_buf_reg, cpu_sum_free_cnt));
    /*cpu_error_flag++; */
  }

 /* now loop through and get all the buffer states */ 
  unsigned int buffer_links[16384];
  unsigned int cpu_buffer_links[4096];
  unsigned int int_buffer_links[16384];

  unsigned int access_addr;
  unsigned int buf_index, num_bufs, int_voq_buf_sum, voq_buf_sum, tail_index;
  unsigned int cpu_voq_buf_sum;
  unsigned int cpu_lin0_buf_used_sum, cpu_lin1_buf_used_sum;
  unsigned int cpu_lin0_buf_free_sum, cpu_lin1_buf_free_sum;

  unsigned int cpu_lin2_buf_used_sum, cpu_lin3_buf_used_sum;
  unsigned int cpu_lin2_buf_free_sum, cpu_lin3_buf_free_sum;

  unsigned int cpu_lin4_buf_used_sum, cpu_lin5_buf_used_sum;
  unsigned int cpu_lin4_buf_free_sum, cpu_lin5_buf_free_sum;

  unsigned int cpu_lin6_buf_used_sum, cpu_lin7_buf_used_sum;
  unsigned int cpu_lin6_buf_free_sum, cpu_lin7_buf_free_sum;




  unsigned int cpu_empty_buf_free_sum;  
  unsigned int cpu_list=0;
  unsigned int cpu_linux_buf_used_sum[8];

  /* clear histograms */
  voq_buf_sum = 0;
  for(i=0;i<16384;i++) {
	  buffer_links[i]=0;
	  int_buffer_links[i]=0;

  }
  for(i=0;i<4096;i++) {
	  cpu_buffer_links[i]=0;
  }
  /* need to walk the normal packet VoQ buffer lists, starting at Rd_buffer for buffers_used
   * rd_buffer is [46:33], buffers_used is [54:47]
   * rd_buffer is [40:26], buffers_used is [65:53]
   */
  printf("============ Walking normal buffer lists - VoQs 0-47\n");
  for (j=0; j<48; j++) {
	  reg_temp = (unsigned int) j;
	  reg_temp |= 0x80000000;
	  wr_access (p_value, QM_QUE_STATUS_MEM_ACCESS, reg_temp);
	  while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_QUE_STATUS_MEM_ACCESS, &reg_temp);
	  }

	  /* goto queue status mem data2 */
	  rd_access (p_value, QM_QUE_STATUS_MEM_DATA2, &reg_temp );

	  // Was reg_temp>>1 but now reg_temp
	  // Left shift was 13 now 12
	  num_bufs = ((reg_temp) & 0x3)<<12;

      LOG(DBG("DATA2: %8.8x\n",reg_temp));

	/* goto queue status mem data1 */
	rd_access (p_value, QM_QUE_STATUS_MEM_DATA1, &reg_temp );
    
	LOG(DBG("DATA1: %8.8x\n",reg_temp));
	buf_index = (reg_temp>>9) & 0xfff;

	// mask was 0x3ff and now is 0x7ff
	num_bufs += (reg_temp>>21) & 0x7ff;

	LOG(DBG("Number of buffers used for this %s VoQ:%d is %d\n",(check_voq_internal(j)) ? "INTERNAL" : "EXTERNAL",
				j,num_bufs));

	if (check_voq_internal(j))
		int_voq_buf_sum+=num_bufs;
	else
		voq_buf_sum += num_bufs;

	  /* QM: WR_BUFFER Index - goto queue status mem data0 */
	  rd_access (p_value, QM_QUE_STATUS_MEM_DATA0, &reg_temp );
      LOG(DBG("DATA0: %8.8x\n",reg_temp));
	  tail_index = (reg_temp>>14) & 0xfff; // 12-bit field

	  while ( num_bufs > 0 ) {
  	    // This pretty complicated status decides if we are out of bounds
	    // with a buffer link depending on the memory configuration
	    if      ((int_test==1 && use_internal_for_recirc==1 && j<48 && buf_index>4096) ||
		    (int_test==1 && use_internal_for_recirc==0 && (j<24 || (j>=40 && j<48)) && buf_index>4096) ||
		    (buf_index>16384)) {
		    	LOG(ERR("ERRORVoQ List %d, Head Buffer Link has illegal ptr %d Buffers Lost=%d : Walk Aborted \r\n",j,buf_index,num_bufs));
	      break;
	    }	     
	
	    if (check_voq_internal(j))
		    int_buffer_links[buf_index]++;  /* count this buffer usage */
	    else
		    buffer_links[buf_index]++;  /* count this buffer usage */

	    LOG(DBG("VoQ %d Status: Head Rd Buffer %d, Tail Wr Buffer %d, Buffers used %d \r\n",j,buf_index, tail_index,num_bufs));
	    while ( num_bufs > 1) {  /* walk buffers used */
		    LOG(DBG("============ Number of buffers >1 wakling index:%d for VoQ %d\n",buf_index,j));

		    reg_temp = buf_index;
		    reg_temp |= 0x80000000;
		    if (check_voq_internal(j)) {
			    buf_access=QM_INT_BUFFER_LIST_MEM_ACCESS;
			    buf_data=QM_INT_BUFFER_LIST_MEM_DATA;

		    } else {
			    buf_access=QM_BUFFER_LIST_MEM_ACCESS;
			    buf_data=QM_BUFFER_LIST_MEM_DATA;
		    }

		    wr_access (p_value, buf_access, reg_temp);
		    while ( reg_temp & 0x80000000 ){
			    rd_access( p_value, buf_access, &reg_temp);
		    }

		  rd_access (p_value, buf_data, &reg_temp );
      LOG(DBG("Buffer at index %d in_free_list:%d next_index:%d\n",
                      buf_index,
                      (reg_temp>>12) & 1,
                      reg_temp & 0xfff));
      reg_temp=reg_temp & 0xfff;

		  if ( reg_temp > 16384) {
		    LOG(ERR("VoQ List %d, Buffer Link %d has illegal next ptr %d : Walk Aborted (check is >16384) \r\n",j,buf_index,reg_temp));
		    break;
		  } else {
			  if (check_voq_internal(j))
				  int_buffer_links[reg_temp]++;
			  else
				  buffer_links[reg_temp]++;
		      buf_index = reg_temp;
          num_bufs--;
		    }
	    }
    num_bufs--;
	  }
	}

	printf("\nAggregate EXTERNAL VoQ Buffer Usage %d summary_bank_count:%d\r\n\n", voq_buf_sum,sum_bank_cnt);
	printf("\nAggregate INTERNAL VoQ Buffer Usage %d summary_bank_count:%d\r\n\n", int_voq_buf_sum,int_sum_cnt);

	// Internal buffer check
	if ( (int_voq_buf_sum + int_sum_cnt) != total_internal_buffer_count) {
		LOG(ERR("ERROR: Oustanding INTERNAL buffers + free aggregate buffers != total buffer count of %d\n",
					total_internal_buffer_count));
		error_flag++;
  	}

	// Extneral buffer check
	total_buf_count=qm_status_0.bf.pkt_banks*2048;
	if ( (voq_buf_sum + sum_bank_cnt) != total_buf_count) {
		LOG(ERR("ERROR: Oustanding EXTERNAL buffers + free aggregate buffers != total buffer count of %d\n",total_buf_count));
		error_flag++;
  	}

	cpu_voq_buf_sum=0;
	cpu_lin0_buf_used_sum=0;
	cpu_lin1_buf_used_sum=0;
	cpu_lin2_buf_used_sum=0;
	cpu_lin3_buf_used_sum=0;
	cpu_lin4_buf_used_sum=0;
	cpu_lin5_buf_used_sum=0;
	cpu_lin6_buf_used_sum=0;
	cpu_lin7_buf_used_sum=0;

  for (loop=0;loop<8;loop++)
  	cpu_linux_buf_used_sum[loop]=0;

  //return 0;

/* slightly different for CPU VoQs in Linux Mode */

  for (j=G2_CPU_VOQ_START; j<(G2_CPU_VOQ_START+G2_CPU_VOQ_COUNT); j++) {
	  reg_temp = (unsigned int) j;
	  reg_temp |= 0x80000000;
	  wr_access (p_value, QM_QUE_STATUS_MEM_ACCESS, reg_temp);
	  while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_QUE_STATUS_MEM_ACCESS, &reg_temp);
	  }

	  /* goto queue status mem data2 */
	  rd_access (p_value, QM_QUE_STATUS_MEM_DATA2, &reg_temp );

	  // Was reg_temp>>1 but now reg_temp
	  // Left shift was 13 now 12
	  num_bufs = ((reg_temp) & 0x3)<<12;

      LOG(DBG("DATA2: %8.8x\n",reg_temp));

	  /* goto queue status mem data1 */
	  rd_access (p_value, QM_QUE_STATUS_MEM_DATA1, &reg_temp );
    
      LOG(DBG("DATA1: %8.8x\n",reg_temp));
	  buf_index = (reg_temp>>9) & 0xfff;

	  // mask was 0x3ff and now is 0x7ff
	  num_bufs += (reg_temp>>21) & 0x7ff;

	  /*
	  rd_access (p_value, 0x50c00dc, &reg_temp );
	  buf_index = (reg_temp>>1) & 0x3fff;  // only 9 bit buffer index range for CPU VoQs 
	  num_bufs = (reg_temp>>15) & 0xff;
	  */

	  cpu_voq_buf_sum += num_bufs;
	  while ( num_bufs > 0 ) {
	     if ( buf_index > 4096 ) {
		 	LOG(ERR("CPU VoQ List %d, Head Buffer Link has illegal ptr %d Buffers Lost=%d: Walk Aborted \r\n",j,buf_index,num_bufs));
		 cpu_error_flag++;
		 break;
	     }	     
	     cpu_buffer_links[buf_index]++;  /* count this buffer usage */
	     LOG(DBG("CPU VoQ %d Status: Rd Buffer %d, Buffers used %d \r\n",j,buf_index, num_bufs));
	     /* read the main CPU buffer list to get Linux0 vs Linux1 status only if SINGLE Buffer in VoQ*/
	     reg_temp = buf_index;
	     reg_temp |= 0x80000000;
	     wr_access (p_value, QM_CPU_BUFFER_LIST_MEM_ACCESS, reg_temp);
	     while ( reg_temp & 0x80000000 ){
		    rd_access( p_value, QM_CPU_BUFFER_LIST_MEM_ACCESS, &reg_temp);
	     }
	     rd_access (p_value, QM_CPU_BUFFER_LIST_MEM_DATA1, &reg_temp2);
	     rd_access (p_value, QM_CPU_BUFFER_LIST_MEM_DATA0, &reg_temp );

	     // Combine and form the address of the linux CPU buffer location
	     addr = ((reg_temp >> 16) | ((reg_temp2 << 16) & 0x07ffffff))<<3;
	     if ( (addr >= hi_addr_lim) || (addr < lo_addr_lim) ) {
	          LOG(ERR("Walking VoQ %d, CPU Buffer %d has Illegal Physical Address 0x%x \r\n",j,buf_index,addr));
		  cpu_error_flag++;
	     } 

	     cpu_list=(reg_temp>>13) & 0x7;
	     cpu_linux_buf_used_sum[cpu_list]++;
	     
	     buf_index = reg_temp & 0xfff; /* isoloate the next buffer index */
  	     while ( num_bufs > 1) {    /* walk CPU buffers used */
	          cpu_buffer_links[buf_index]++;  /* count this buffer usage */
	     	  reg_temp = buf_index;
		  reg_temp |= 0x80000000;
		  wr_access (p_value, QM_CPU_BUFFER_LIST_MEM_ACCESS, reg_temp);
		  while ( reg_temp & 0x80000000 ){
			  rd_access( p_value, QM_CPU_BUFFER_LIST_MEM_ACCESS, &reg_temp);
		  }

	         rd_access (p_value, QM_CPU_BUFFER_LIST_MEM_DATA1, &reg_temp2);
		 rd_access (p_value, QM_CPU_BUFFER_LIST_MEM_DATA0, &reg_temp );
	         addr = ((reg_temp >> 16) | (reg_temp2 << 16))<<3;

	         if ( (addr >= hi_addr_lim) || (addr < lo_addr_lim) ) {
	             LOG(ERR("ERROR: Walking VoQ %d, CPU Buffer %d has Illegal Physical Address 0x%x \r\n",j,buf_index,addr));
		     cpu_error_flag++;
	         } 
	     	  b=(reg_temp>>13) & 0x7;
	          cpu_linux_buf_used_sum[b]++;

		  buf_index = b;
		  num_bufs--;
	        }
	     num_bufs--;
	   }
        }
	
  	for (loop=0;loop<8;loop++) 
		LOG(DBG("\nAggregate CPU VoQ Buffer Usage %d , with Linux list %d=%d\r\n\n", 
			cpu_voq_buf_sum,
			loop,
			cpu_linux_buf_used_sum[loop]));

	if ( (cpu_voq_buf_sum + cpu_sum_free_cnt) != 4096) {
	   LOG(ERR("The sum of CPU buffers in VoQs (%d) and Free CPU Buffers (%d) is not 4096\r\n",cpu_voq_buf_sum, cpu_sum_free_cnt));
	   }

	j=0;
	printf("================= Walking INTERNAL bank free buffer lists\n");
	/* now walk each bank free buffer list */
	num_bufs = int_free_bank_cnt;
	buf_index = int_free_bank_head;

	if ( num_bufs > 0 ) {
		int_buffer_links[buf_index]++;  /* count this buffer usage */
		LOG(DBG("INTERNAL Status: Head Buffer %d, Tail Buffer %d, Free Buffers %d \r\n",buf_index,int_free_bank_tail,num_bufs));
	}

	while (( int_test==1) && (num_bufs > 1)) {  /* walk buffers used in the PACKET Buffer List*/
		reg_temp = buf_index;
		reg_temp |= 0x80000000;

		buf_access=QM_INT_BUFFER_LIST_MEM_ACCESS;
		buf_data=QM_INT_BUFFER_LIST_MEM_DATA;

		wr_access (p_value, buf_access, reg_temp);
		while ( reg_temp & 0x80000000 ) {
			rd_access( p_value, buf_access, &reg_temp);
		}

		rd_access (p_value, buf_data, &reg_temp );
		next_buffer=reg_temp & 0x7ff;

		if (num_bufs == int_free_bank_cnt) {
			LOG(DBG("INTERNAL:   bufferHead=%d, bufferNext=%d, bufferHeadUsed=%d, numBufs=%d\r\n",buf_index,next_buffer,int_buffer_links[buf_index],num_bufs));
        	}

		if (1024<next_buffer) {
			LOG(ERR("INTERNAL: Next Buffer index out of range! buffer=%d, bufferNext=%d, bufferNextUsed=%d, numBufs=%d\r\n",buf_index,next_buffer,int_buffer_links[next_buffer],num_bufs));
		}

		if (int_buffer_links[next_buffer]>0) {
			LOG(ERR("INTERNAL: Next Buffer List Walk Error, buffer=%d, bufferNext=%d, bufferUsed=%d, numBufs=%d\r\n",buf_index,next_buffer,int_buffer_links[next_buffer],num_bufs));
		}

		if ( next_buffer> 1024) {
			LOG(ERR("INTERNAL: Buffer Link %d has illegal next ptr %d : Walk Aborted \r\n",buf_index,next_buffer));
			error_flag++;
			break;
		} else {
			int_buffer_links[next_buffer]++;
			buf_index = next_buffer;
			num_bufs--;
		}

		if ( int_free_bank_cnt > 0) 
			LOG(DBG("INTERNAL: bufferTail=%d, bufferTailUsed=%d, numBufs=1\r\n",next_buffer,int_buffer_links[next_buffer]));
	}



	printf("================= Walking EXTERNAL bank free buffer lists\n");
	/* now walk each bank free buffer list */
	for (j=0; j<8; j++) {
		num_bufs = free_bank_cnts[j];
		buf_index = free_bank_heads[j];
		if ( num_bufs > 0 ) {
			buffer_links[buf_index]++;  /* count this buffer usage */
			LOG(DBG("Bank Free Head %d Status: Head Buffer %d, Tail Buffer %d, Free Buffers %d \r\n",j,buf_index,free_bank_tails[j],num_bufs));
		}

		while ( num_bufs > 1) {  /* walk buffers used in the PACKET Buffer List*/
			reg_temp = buf_index;
			reg_temp |= 0x80000000;

			buf_access=QM_BUFFER_LIST_MEM_ACCESS;
			buf_data=QM_BUFFER_LIST_MEM_DATA;

			wr_access (p_value, buf_access, reg_temp);

			while ( reg_temp & 0x80000000 ){
				rd_access( p_value, buf_access, &reg_temp);
			}

			rd_access (p_value, buf_data, &reg_temp );
			next_buffer=reg_temp & 0x7ff;

			if (num_bufs == free_bank_cnts[j]) {
				LOG(DBG("  Head Buffer bank=%d, bufferHead=%d, bufferNext=%d, bufferHeadUsed=%d, numBufs=%d\r\n",j,buf_index,next_buffer,buffer_links[buf_index],num_bufs));
			}

			if ((j*2048>next_buffer) || ((j+1)*2048-1)<next_buffer) {
				LOG(ERR("  Next Buffer index out of range! bank=%d, buffer=%d, bufferNext=%d, bufferNextUsed=%d, numBufs=%d\r\n",j,buf_index,next_buffer,buffer_links[next_buffer],num_bufs));
			}

			if (buffer_links[next_buffer]>0) {
				LOG(ERR("  Next Buffer List Walk Error, bank=%d, buffer=%d, bufferNext=%d, bufferUsed=%d, numBufs=%d\r\n",j,buf_index,next_buffer,buffer_links[next_buffer],num_bufs));
			}

			if ( next_buffer> 16384) {
				LOG(ERR("Bank Free List %d, Buffer Link %d has illegal next ptr %d : Walk Aborted \r\n",j,buf_index,next_buffer));
				error_flag++;
				break;
			} else {
				buffer_links[next_buffer]++;
				buf_index = next_buffer;
				num_bufs--;
			}
		} 

		if ( free_bank_cnts[j] > 0) {
			LOG(DBG("  Tail Buffer bank=%d, bufferTail=%d, bufferTailUsed=%d, numBufs=1\r\n",j,next_buffer,buffer_links[next_buffer]));
		}
	}

	printf("================= Walking CPU bank free buffer lists\n");
	/* now walk the free lists for Linux buffers to account for all buffers not linked into the CPU VoQ lists*/
      for (j=0; j<8; j++) {
         num_bufs = cpu_free_cnts[j];
	 buf_index = cpu_free_heads[j];
 	  if ( num_bufs > 0 ) {
		  cpu_buffer_links[buf_index]++;  /* count this buffer usage */
	          LOG(DBG("CPU Bank Free Head %d Status: Head Buffer %d, Free Buffers %d \r\n",j,buf_index, num_bufs));
		  }
	  while ( num_bufs > 1) {  /* walk buffers used in the CPU Buffer list*/
		  reg_temp = buf_index;
		  reg_temp |= 0x80000000;
		  wr_access (p_value, QM_CPU_BUFFER_LIST_MEM_ACCESS, reg_temp);
		  while ( reg_temp & 0x80000000 ){
			  rd_access( p_value, QM_CPU_BUFFER_LIST_MEM_ACCESS, &reg_temp);
		  }

	         rd_access (p_value, QM_CPU_BUFFER_LIST_MEM_DATA1, &reg_temp2);
		 rd_access (p_value, QM_CPU_BUFFER_LIST_MEM_DATA0, &reg_temp );
	         addr = ((reg_temp >> 16) | (reg_temp2 << 16))<<3;

	         //rd_access (p_value, 0x50c00b4, &reg_temp2);
	         //rd_access (p_value, 0x50c00b8, &reg_temp );
	         //addr = ((reg_temp >> 10) | (reg_temp2 << 22))<<3;
		 /*printf("Walking Free List %d, CPU Buffer %d Physical Address 0x%x\r\n",j,buf_index,addr);*/

	         if ( (addr >= hi_addr_lim) || (addr < lo_addr_lim) ) {
	             LOG(ERR("Walking Free List %d, CPU Buffer %d has Illegal Physical Address 0x%x \r\n",j,buf_index,addr));
		     cpu_error_flag++;
	         } 

		  b=reg_temp & 0xfff;
	     	  cpu_list=(reg_temp>>13) & 0x7;


		  //b = reg_temp & 0x1ff;   /* index value is 9 bits wide */
		  //if ( j==0 && ((reg_temp&0x200)==0x200) ) {
		  if (j!=cpu_list) {
		      LOG(ERR("Linux list %d found in free list %d, buffer link %d has illegal Linux List bit ptr %d : Walk Aborted \r\n",
				      cpu_list,
				      j,
				      buf_index,
				      b));
		      error_flag++;
		      break;
		  } else {
	             LOG(DBG("==> going from link %d-->%d on linux list %d/free list %d\n",
				     buf_index,b,
				     cpu_list, j));
		     cpu_buffer_links[b]++;
		     buf_index = b;
		     num_bufs--;
		  }
	        }
 
        }

	/* all stats collected, now scan for errors
	* any value=0 or any value>1 is an error
	*/

	if (int_test==1)
		for( i=0; i<total_internal_buffer_count; i++){
			if ( int_buffer_links[i] != 1) {
				LOG(ERR("INTERNAL: Buffer Link Error at %d, value is %d\r\n", i, int_buffer_links[i]));
				error_flag++;
			}
		}

	for( i=0; i<qm_status_0.bf.pkt_banks*2048; i++){
		if ( buffer_links[i] != 1) {
			LOG(ERR("EXTERNAL: Buffer Link Error at %d, value is %d\r\n", i, buffer_links[i]));
			error_flag++;
		}
 	}

	for(i=0; i<allocated_cpu_buffers; i++) {
		if ( cpu_buffer_links[i] != 1) {
			LOG(ERR("CPU: Buffer Link Error at %d, value is %d\r\n", i, cpu_buffer_links[i]));
			cpu_error_flag++;
		}
	}

	/* re-scan per bank buffer counts */
	sum_bank_cnt = 0;
	for( b=0; b<8; b++) {
     /* read bank buffer count */
	  reg_temp = (unsigned int) b*4;
	  reg_temp |= 0x80000000;
	  wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);
	  while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	  }
	  rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
	  free_bank_cnts[b] = reg_temp;
	  sum_bank_cnt += reg_temp;
     /* read bank heads */
 	  reg_temp = (unsigned int) b*4+1;
	  reg_temp |= 0x80000000;
	  wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);
	  while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	  }
	  rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
	  free_bank_heads[b] = reg_temp;
     /* read bank tails */
 	  reg_temp = (unsigned int) b*4+2;
	  reg_temp |= 0x80000000;
	  wr_access (p_value, QM_DV_REGS_ACCESS, reg_temp);
	  while ( reg_temp & 0x80000000 ){
		  rd_access( p_value, QM_DV_REGS_ACCESS, &reg_temp);
	  }
	  rd_access (p_value, QM_DV_REGS_DATA, &reg_temp );
	  free_bank_tails[b] = reg_temp;
  }

	close_logs();

  for (j=0; j<8; j++) {
     printf("Bank Free Head %d Status: Head Buffer %d, Tail Buffer %d, Free Buffers %d \r\n",j,free_bank_heads[j],free_bank_tails[j],free_bank_cnts[j]);
  }


  if( (bypass_data==1||(error_flag == 0))  && (bypass_cpu==1 || (cpu_error_flag==0)) ) {
    printf("PASS: No errors detected\n");
    /* need to turn off SW access to the buffer list */
    //rd_access( p_value, 0x50c0000, &reg_temp);
    //wr_access( p_value, 0x50c0000, (reg_temp & 0x7fffffff) );
    /* Start Traffic from Rx FIFO */
    //rd_access( p_value, 0x5000334, &reg_temp);
    //wr_access( p_value, 0x5000334, (reg_temp & 0xfffffffd) );
  } else {
    printf("FAIL: Traffic Stopped with Error Counts %d, and CPU %d \r\n", error_flag, cpu_error_flag);
    return 1;
   }
  


  /* QM: need to turn on SW access to the buffer list */
  rd_access( p_value, QM_CONFIG_0, &reg_temp);

  // MSB turns on CPU access to buffer lists
  wr_access( p_value, QM_CONFIG_0, (reg_temp&0x7fffffff) );

  /* NI: Turn on the traffic for Rx FIFO for all eight ports*/
  if (stop_traffic)
  	for (ports=0;ports<8;ports++) {
    	rd_access( p_value, NI_TOP_NI_RX_CNTRL_CONFIG0_0+(8*ports), &reg_temp);
    	wr_access( p_value, NI_TOP_NI_RX_CNTRL_CONFIG0_0+(8*ports), (reg_temp &  0xfffffffd) );
  	}

  return 0;
}

