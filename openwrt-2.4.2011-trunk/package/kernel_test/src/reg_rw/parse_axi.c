#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>	

//#define MAX_BUF_SIZE	(64*1024)
int MAX_BUF_SIZE;
unsigned char data_pattern[] = {0x00, 0xFF, 0x55, 0xAA};
static unsigned char *data_buf1; //[MAX_BUF_SIZE];	
static unsigned char *data_buf2; //[MAX_BUF_SIZE];	

#define REG_DMA_D_AXI_CONFIG	   0xf0090300  
#define REG_DMA_D_AXI_READ_ARB_ID  0xf0090304
#define REG_DMA_D_AXI_WRITE_ARB_ID 0xf0090308
#define REG_DMA_D_AXI_READ_CHANNEL_0_3_DDR_WEIGHT 0xf009030c
#define REG_DMA_D_AXI_READ_CHANNEL_4_7_DDR_WEIGHT 0xf0090310
#define REG_DMA_D_AXI_WRITE_CHANNEL_0_3_DDR_WEIGHT 0xf0090314
#define REG_DMA_D_AXI_WRITE_CHANNEL_4_7_DDR_WEIGHT 0xf0090318
#define REG_DMA_D_AXI_RWD_VALUE 0xf009031c
#define REG_DMA_D_AXI_READ_TIMEOUT_THRESHOLD  0xf0090320
#define REG_DMA_D_AXI_WRITE_TIMEOUT_THRESHOLD 0xf0090324

#define DMA_DMA_LSO_RXDMA_CONTROL  0xf0090000   
#define DMA_DMA_LSO_TXDMA_CONTROL  0xf0090004   


struct dma_d_axi_config_register {
	union {
        	struct {
			unsigned long axi_config_reserved: 4;				/* lowest bit */
			unsigned long axi_write_channel_fixed_id_option: 1;
			unsigned long axi_read_channel_fixed_id_option: 1;
			unsigned long axi_write_allow_normal_req_EXOK_response: 1;
			unsigned long axi_read_allow_normal_req_EXOK_response: 1;
			unsigned long axi_config_reserved1: 2;
			unsigned long axi_write_channel5_arbscheme: 1;
			unsigned long axi_write_channel4_arbscheme: 1;
			unsigned long axi_write_channel3_arbscheme: 1;
			unsigned long axi_write_channel2_arbscheme: 1;
			unsigned long axi_write_channel1_arbscheme: 1;
			unsigned long axi_write_channel0_arbscheme: 1;
			unsigned long axi_reserved2: 2;
			unsigned long axi_read_channel5_arbscheme: 1;
                        unsigned long axi_read_channel4_arbscheme: 1;
                        unsigned long axi_read_channel3_arbscheme: 1;
                        unsigned long axi_read_channel2_arbscheme: 1;
                        unsigned long axi_read_channel1_arbscheme: 1;
                        unsigned long axi_read_channel0_arbscheme: 1;
			unsigned long axi_write_outtrans_nums: 2;
			unsigned long axi_read_outtrans_nums: 2;
			unsigned long axi_func_reserved: 2;
			unsigned long axi_xbus_len: 2;					/* highest bit */
        	}b;
		unsigned long value;
	}a;
};

struct dma_d_axi_read_arb_id_register {
        union {
                struct {
                        unsigned long axi_rchannel0_SPpri_or_DRRid: 3;
                        unsigned long axi_rchannel1_SPpri_or_DRRid: 3;
                        unsigned long axi_rchannel2_SPpri_or_DRRid: 3;
                        unsigned long axi_rchannel3_SPpri_or_DRRid: 3;
                        unsigned long axi_rchannel4_SPpri_or_DRRid: 3;
                        unsigned long axi_rchannel5_SPpri_or_DRRid: 3;
			unsigned long reserved: 14;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_write_arb_id_register {
        union {
                struct {
                        unsigned long axi_wchannel0_SPpri_or_DRRid: 3;
                        unsigned long axi_wchannel1_SPpri_or_DRRid: 3;
                        unsigned long axi_wchannel2_SPpri_or_DRRid: 3;
                        unsigned long axi_wchannel3_SPpri_or_DRRid: 3;
                        unsigned long axi_wchannel4_SPpri_or_DRRid: 3;
                        unsigned long axi_wchannel5_SPpri_or_DRRid: 3;
                        unsigned long reserved: 14;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_read_channel_0_3_drr_weight_register {
        union {
                struct {
                        unsigned long axi_rchannel0_3_DRRweight_0_7: 8;
                        unsigned long axi_rchannel0_3_DRRweight_8_15: 8;
                        unsigned long axi_rchannel0_3_DRRweight_16_23: 8;
                        unsigned long axi_rchannel0_3_DRRweight_24_31: 8;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_read_channel_4_7_drr_weight_register {
        union {
                struct {
                        unsigned long axi_rchannel4_7_DRRweight_0_7: 8;
                        unsigned long axi_rchannel4_7_DRRweight_8_15: 8;
                        unsigned long axi_rchannel4_7_DRRweight_16_23: 8;
                        unsigned long axi_rchannel4_7_DRRweight_24_31: 8;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_write_channel_0_3_drr_weight_register {
        union {
                struct {
                        unsigned long axi_wchannel0_3_DRRweight_0_7: 8;
                        unsigned long axi_wchannel0_3_DRRweight_8_15: 8;
                        unsigned long axi_wchannel0_3_DRRweight_16_23: 8;
                        unsigned long axi_wchannel0_3_DRRweight_24_31: 8;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_write_channel_4_7_drr_weight_register {
        union {
                struct {
                        unsigned long axi_wchannel4_7_DRRweight_0_7: 8;
                        unsigned long axi_wchannel4_7_DRRweight_8_15: 8;
                        unsigned long axi_wchannel4_7_DRRweight_16_23: 8;
                        unsigned long axi_wchannel4_7_DRRweight_24_31: 8;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_rwd_value_register {
        union {
                struct {
                        unsigned long axi_rid: 4;
                        unsigned long axi_wid: 4;
                        unsigned long reserved: 24;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_read_timeout_threshold_register {
        union {
                struct {
                        unsigned long reserved: 8;
			unsigned long axi_rchannel5_timeout_threshold: 4;
			unsigned long axi_rchannel4_timeout_threshold: 4;
			unsigned long axi_rchannel3_timeout_threshold: 4;
			unsigned long axi_rchannel2_timeout_threshold: 4;
			unsigned long axi_rchannel1_timeout_threshold: 4;
			unsigned long axi_rchannel0_timeout_threshold: 4;
                }b;
                unsigned long value;
        }a;
};

struct dma_d_axi_write_timeout_threshold_register {
        union {
                struct {
                        unsigned long reserved: 8;
                        unsigned long axi_wchannel5_timeout_threshold: 4;
                        unsigned long axi_wchannel4_timeout_threshold: 4;
                        unsigned long axi_wchannel3_timeout_threshold: 4;
                        unsigned long axi_wchannel2_timeout_threshold: 4;
                        unsigned long axi_wchannel1_timeout_threshold: 4;
                        unsigned long axi_wchannel0_timeout_threshold: 4;
                }b;
                unsigned long value;
        }a;
};

struct axi_register_1 {
        union {
                struct {
                        unsigned long field1: 12 ;
                        unsigned long field2: 10 ;
                        unsigned long field3: 10 ;
                }b;
                unsigned long value;
        }a;
};

//REGREAD
typedef struct {
        u_int16_t               cmd;    // command ID
        u_int16_t               len;    // data length, excluding this header
        u_int32_t               location;
        u_int32_t               length;
        u_int32_t               size;
        u_int32_t               value;
} NAT_REGREAD;

// Common Header
struct {
        u_int16_t               cmd;    // command ID
        u_int16_t               len;    // data length, excluding this header
} NATCMD_HDR_T;

//REGWRITE
typedef struct {
        u_int16_t               cmd;    // command ID
        u_int16_t               len;    // data length, excluding this header
        u_int32_t               location;
        u_int32_t               data;
        u_int32_t               size;
} NAT_REGWRITE;

#define REGREAD                 26
#define REGWRITE                27
#define MEM_READ    100
#define MEM_WRITE   110

static unsigned int read_register(unsigned int reg_index)
{
#if 0
	char buf[512+1];
	FILE *file1;
	unsigned int value1, value2;
	
	sprintf(buf, "reg_rw read mem -b 0x%x -l 1 -4 > reg_value\n", reg_index);
        system(buf);
        file1 = fopen("reg_value", "rw");
        if (file1 == NULL)
        {
                printf("%s: open reg_value failed!!!\r\n", __func__);
                exit(-1);
        }
        fseek(file1, 0L, SEEK_SET);
        fgets(buf, 512, file1);
        printf("%s: buf=%s\n", __func__, buf);
        sscanf(buf, "0x%8x: %x", &value1, &value2);
        fclose(file1);

        printf("%s: value1=0x%x, value2=0x%x\r\n", __func__, value1, value2);
	return value2;
#endif
	int fd, ret;
	unsigned int data;
	NAT_REGREAD reg_read;

	reg_read.cmd = REGREAD;
        reg_read.len = sizeof(NAT_REGREAD) - sizeof(NATCMD_HDR_T);
        reg_read.location = reg_index;
        reg_read.length = 1;
        reg_read.size = 4;

	//printf("reg_read.len=%d\n", reg_read.len);	

        if( (fd = open("/dev/reg_rw", O_SYNC|O_RDWR)) < 0)
        {
            printf("ERROR: Open RW /Read device error: \n");
            exit(-1);
        }

        ret = ioctl(fd, MEM_READ, &reg_read, &data) ;
        if (ret)
        {
        	fprintf(stderr,"REGREAD Invalid argument \n");
                close(fd);
		exit(-1);	
        }
        close(fd);

	//printf("%s: reg_read.value=0x%x\n", __func__, reg_read.value);
        return reg_read.value;
}

static void write_register(unsigned int reg_index, unsigned int value)
{
#if 0
        FILE *file1;
        unsigned int value1, value2;

	printf("%s: before write reg_index=0x%x, value=0x%x\r\n", __func__, reg_index, value);
        //sprintf(buf, "reg_rw write mem -b 0x%x -d 0x%x-l 1 -4\n", reg_index, value);
        //system(buf);
	value = read_register(reg_index);
	printf("%s: after write reg_index=0x%x, value=0x%x\r\n", __func__, reg_index, value);
#endif

	
	int fd, ret;
	unsigned int data;
        NAT_REGWRITE reg_write;

        reg_write.cmd = REGWRITE;
        reg_write.len = sizeof(NAT_REGWRITE) - sizeof(NATCMD_HDR_T);
        reg_write.location = reg_index;
        reg_write.size = 4;
        reg_write.data = value;

        if( (fd = open("/dev/reg_rw", O_SYNC|O_RDWR)) < 0)
        {
            printf("ERROR: Open RW /Read device error: \n");
            exit(-1);
        }

        ret = ioctl(fd, MEM_WRITE, &reg_write, &data);
        if (ret)
        {
                fprintf(stderr,"REGWRITE Invalid argument \n");
                close(fd);
                exit(-1);
        }
        close(fd);

}

int main(int argc, char *argv[])
{
	unsigned long  size;	
	char *ret;
	unsigned long bsize;
	char *ptr, *src_ptr, *target_ptr;
	FILE *file1, *file2;
	int fd1, fd2;
	char buf[512+1];
	int first_time;
	int i,j;
	int line_number;
	int  axi_xbus_len,
             axi_read_outtrans_nums,
             axi_write_outtrans_nums,
             axi_read_channel0_arbscheme,
             axi_read_channel1_arbscheme,
             axi_read_channel2_arbscheme,
             axi_read_channel3_arbscheme,
             axi_read_channel4_arbscheme,
             axi_read_channel5_arbscheme,
             axi_write_channel0_arbscheme,
             axi_write_channel1_arbscheme,
             axi_write_channel2_arbscheme,
             axi_write_channel3_arbscheme,
             axi_write_channel4_arbscheme,
             axi_write_channel5_arbscheme,
             axi_read_channel_fixed_id_option,
             axi_write_channel_fixed_id_option,
             axi_rchannel0_SPpri_or_DRRid,
             axi_rchannel1_SPpri_or_DRRid,
             axi_rchannel2_SPpri_or_DRRid,
             axi_rchannel3_SPpri_or_DRRid,
             axi_rchannel4_SPpri_or_DRRid,
             axi_rchannel5_SPpri_or_DRRid,
             axi_wchannel0_SPpri_or_DRRid,
             axi_wchannel1_SPpri_or_DRRid,
             axi_wchannel2_SPpri_or_DRRid,
             axi_wchannel3_SPpri_or_DRRid,
             axi_wchannel4_SPpri_or_DRRid,
             axi_wchannel5_SPpri_or_DRRid,
             axi_rchannel0_3_DRRweight_0_7,
             axi_rchannel0_3_DRRweight_8_15,
             axi_rchannel0_3_DRRweight_16_23,
             axi_rchannel0_3_DRRweight_24_31,
             axi_rchannel4_7_DRRweight_0_7,
             axi_rchannel4_7_DRRweight_8_15,
	     axi_wchannel0_3_DRRweight_0_7,
             axi_wchannel0_3_DRRweight_8_15,
             axi_wchannel0_3_DRRweight_16_23,
             axi_wchannel0_3_DRRweight_24_31,
             axi_wchannel4_7_DRRweight_0_7,
             axi_wchannel4_7_DRRweight_8_15,
             axi_wid,
             axi_rid;

	struct axi_register_1 reg1;
	unsigned int value, value1, value2;

	struct dma_d_axi_config_register dma_a_axi_config;
	struct dma_d_axi_read_arb_id_register dma_d_axi_read_arb_id;
	struct dma_d_axi_write_arb_id_register dma_d_axi_write_arb_id;
	struct dma_d_axi_rwd_value_register dma_d_axi_rwd_value;
	struct dma_d_axi_read_timeout_threshold_register dma_d_axi_read_timeout_threshold;
	struct dma_d_axi_write_timeout_threshold_register dma_d_axi_write_timeout_threshold;
	struct dma_d_axi_read_channel_0_3_drr_weight_register dma_d_axi_read_channel_0_3_drr_weight;
	struct dma_d_axi_read_channel_4_7_drr_weight_register dma_d_axi_read_channel_4_7_drr_weight;
	struct dma_d_axi_write_channel_0_3_drr_weight_register dma_d_axi_write_channel_0_3_drr_weight;
        struct dma_d_axi_write_channel_4_7_drr_weight_register dma_d_axi_write_channel_4_7_drr_weight;

#if 0
	reg1.a.b.field1 = 5;
	reg1.a.b.field2 = 4;
	reg1.a.b.field3 = 3;
	printf("reg1=0x%x\r\n", reg1.a.value);
#endif
	

	if (argc < 4)
	{
		printf("%s {filename1} {line number} {block_size}\r\n", argv[0]);
		exit (-1);
	}

	MAX_BUF_SIZE = atoi(argv[3]);
	bsize = MAX_BUF_SIZE;
	data_buf1 = malloc(MAX_BUF_SIZE);
  	if (data_buf1==NULL)
  	{
	    printf("data_buf1 malloc error\n");
		exit(1);
    	}

	//printf("%s: filename1=%s\r\n", __func__, argv[1]);
	//printf("Comparing...%d\r\n",bsize);

	file1 = fopen(argv[1], "rw");
	if (file1 == NULL)
	{
		printf("%s: create %s failed!!!\r\n", __func__, argv[1]);
                exit(-1);
	}

    	fseek(file1, 0L, SEEK_SET);

			ret = fread(data_buf1, 1, bsize, file1);
			if (ret == NULL) 
			{
				printf("read failed!!!\n");
				exit(-1);
			}
			fclose(file1);

			data_buf1[bsize] = 0;
			//printf("read OK\n");
	     		line_number = atoi(argv[2]);
			//printf("line_number=%d\n", line_number);
			target_ptr = src_ptr = data_buf1;
			for (i = 0; i < line_number; i++)
			{
				target_ptr= src_ptr;
				//printf("src_ptr=0x%x\n", src_ptr);
				ptr = strchr(src_ptr, '\n');	
				if (ptr == NULL)
				{
					printf("strchr failed!!\n");
					exit(-1);
				}
				//printf("ptr=0x%x\n", ptr);
				*ptr = 0;
				//printf("src_ptr=%s\r\n", src_ptr);
				src_ptr = ptr + 1;
				//printf("src_ptr=%s\n", src_ptr);
			}	 
			//printf("target_ptr=0x%x\n", target_ptr);
			//printf("target ptr=%s\r\n", target_ptr);

			sscanf(target_ptr, "%1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x "
				"%1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x "
				"%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %1x %1x", 
				&axi_xbus_len,
             			&axi_read_outtrans_nums,
            			&axi_write_outtrans_nums,
            			&axi_read_channel0_arbscheme,
            			&axi_read_channel1_arbscheme,
            &axi_read_channel2_arbscheme,
             &axi_read_channel3_arbscheme,
             &axi_read_channel4_arbscheme,
             &axi_read_channel5_arbscheme,
             &axi_write_channel0_arbscheme,
             &axi_write_channel1_arbscheme,
             &axi_write_channel2_arbscheme,
             &axi_write_channel3_arbscheme,
             &axi_write_channel4_arbscheme,
             &axi_write_channel5_arbscheme,
             &axi_read_channel_fixed_id_option,
             &axi_write_channel_fixed_id_option,
             &axi_rchannel0_SPpri_or_DRRid,
             &axi_rchannel1_SPpri_or_DRRid,
             &axi_rchannel2_SPpri_or_DRRid,
             &axi_rchannel3_SPpri_or_DRRid,
             &axi_rchannel4_SPpri_or_DRRid,
             &axi_rchannel5_SPpri_or_DRRid,
             &axi_wchannel0_SPpri_or_DRRid,
             &axi_wchannel1_SPpri_or_DRRid,
             &axi_wchannel2_SPpri_or_DRRid,
             &axi_wchannel3_SPpri_or_DRRid,
             &axi_wchannel4_SPpri_or_DRRid,
             &axi_wchannel5_SPpri_or_DRRid,
             &axi_rchannel0_3_DRRweight_0_7,
             &axi_rchannel0_3_DRRweight_8_15,
             &axi_rchannel0_3_DRRweight_16_23,
             &axi_rchannel0_3_DRRweight_24_31,
             &axi_rchannel4_7_DRRweight_0_7,
             &axi_rchannel4_7_DRRweight_8_15,
            &axi_wchannel0_3_DRRweight_0_7,
             &axi_wchannel0_3_DRRweight_8_15,
             &axi_wchannel0_3_DRRweight_16_23,
             &axi_wchannel0_3_DRRweight_24_31,
             &axi_wchannel4_7_DRRweight_0_7,
             &axi_wchannel4_7_DRRweight_8_15,
             &axi_wid,
             &axi_rid
	);

			printf("data=%1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x "
                                "%1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x %1x "
                                "%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %1x %1x\n",	
				axi_xbus_len,
             axi_read_outtrans_nums,
             axi_write_outtrans_nums,
             axi_read_channel0_arbscheme,
             axi_read_channel1_arbscheme,
             axi_read_channel2_arbscheme,
             axi_read_channel3_arbscheme,
             axi_read_channel4_arbscheme,
             axi_read_channel5_arbscheme,
             axi_write_channel0_arbscheme,
             axi_write_channel1_arbscheme,
             axi_write_channel2_arbscheme,
             axi_write_channel3_arbscheme,
             axi_write_channel4_arbscheme,
             axi_write_channel5_arbscheme,
             axi_read_channel_fixed_id_option,
             axi_write_channel_fixed_id_option,
             axi_rchannel0_SPpri_or_DRRid,
             axi_rchannel1_SPpri_or_DRRid,
             axi_rchannel2_SPpri_or_DRRid,
             axi_rchannel3_SPpri_or_DRRid,
             axi_rchannel4_SPpri_or_DRRid,
             axi_rchannel5_SPpri_or_DRRid,
             axi_wchannel0_SPpri_or_DRRid,
             axi_wchannel1_SPpri_or_DRRid,
             axi_wchannel2_SPpri_or_DRRid,
             axi_wchannel3_SPpri_or_DRRid,
             axi_wchannel4_SPpri_or_DRRid,
             axi_wchannel5_SPpri_or_DRRid,
             axi_rchannel0_3_DRRweight_0_7,
             axi_rchannel0_3_DRRweight_8_15,
             axi_rchannel0_3_DRRweight_16_23,
             axi_rchannel0_3_DRRweight_24_31,
             axi_rchannel4_7_DRRweight_0_7,
             axi_rchannel4_7_DRRweight_8_15,
             axi_wchannel0_3_DRRweight_0_7,
             axi_wchannel0_3_DRRweight_8_15,
             axi_wchannel0_3_DRRweight_16_23,
             axi_wchannel0_3_DRRweight_24_31,
             axi_wchannel4_7_DRRweight_0_7,
             axi_wchannel4_7_DRRweight_8_15,
             axi_wid,
             axi_rid);



	//dma_a_axi_config	
	dma_a_axi_config.a.value = read_register(REG_DMA_D_AXI_CONFIG);
	dma_a_axi_config.a.b.axi_xbus_len = axi_xbus_len;
	dma_a_axi_config.a.b.axi_read_outtrans_nums = axi_read_outtrans_nums;
	dma_a_axi_config.a.b.axi_write_outtrans_nums = axi_write_outtrans_nums;
	dma_a_axi_config.a.b.axi_read_channel0_arbscheme = axi_read_channel0_arbscheme;
	dma_a_axi_config.a.b.axi_read_channel1_arbscheme = axi_read_channel1_arbscheme;
	dma_a_axi_config.a.b.axi_read_channel2_arbscheme = axi_read_channel2_arbscheme;
	dma_a_axi_config.a.b.axi_read_channel3_arbscheme = axi_read_channel3_arbscheme;
	dma_a_axi_config.a.b.axi_read_channel4_arbscheme = axi_read_channel4_arbscheme;
	dma_a_axi_config.a.b.axi_read_channel5_arbscheme = axi_read_channel5_arbscheme;
	dma_a_axi_config.a.b.axi_write_channel0_arbscheme = axi_write_channel0_arbscheme;
	dma_a_axi_config.a.b.axi_write_channel1_arbscheme = axi_write_channel1_arbscheme;
	dma_a_axi_config.a.b.axi_write_channel2_arbscheme = axi_write_channel2_arbscheme;
	dma_a_axi_config.a.b.axi_write_channel3_arbscheme = axi_write_channel3_arbscheme;
	dma_a_axi_config.a.b.axi_write_channel4_arbscheme = axi_write_channel4_arbscheme;
	dma_a_axi_config.a.b.axi_write_channel5_arbscheme = axi_write_channel5_arbscheme;
	dma_a_axi_config.a.b.axi_read_channel_fixed_id_option = axi_read_channel_fixed_id_option;
	dma_a_axi_config.a.b.axi_write_channel_fixed_id_option = axi_write_channel_fixed_id_option;
	write_register(REG_DMA_D_AXI_CONFIG, dma_a_axi_config.a.value);
	value = read_register(REG_DMA_D_AXI_CONFIG);
	//printf("dma_a_axi_config.a.value=0x%x\n",  dma_a_axi_config.a.value);
	//printf("read value=0x%x\n", value);

	//dma_d_axi_read_arb_id
	dma_d_axi_read_arb_id.a.value = read_register(REG_DMA_D_AXI_READ_ARB_ID);
        dma_d_axi_read_arb_id.a.b.axi_rchannel0_SPpri_or_DRRid = axi_rchannel0_SPpri_or_DRRid;
        dma_d_axi_read_arb_id.a.b.axi_rchannel1_SPpri_or_DRRid = axi_rchannel1_SPpri_or_DRRid;
        dma_d_axi_read_arb_id.a.b.axi_rchannel2_SPpri_or_DRRid = axi_rchannel2_SPpri_or_DRRid;
        dma_d_axi_read_arb_id.a.b.axi_rchannel3_SPpri_or_DRRid = axi_rchannel3_SPpri_or_DRRid;
        dma_d_axi_read_arb_id.a.b.axi_rchannel4_SPpri_or_DRRid = axi_rchannel4_SPpri_or_DRRid;
        dma_d_axi_read_arb_id.a.b.axi_rchannel5_SPpri_or_DRRid = axi_rchannel5_SPpri_or_DRRid;
	write_register(REG_DMA_D_AXI_READ_ARB_ID, dma_d_axi_read_arb_id.a.value);
	value = read_register(REG_DMA_D_AXI_READ_ARB_ID);
	//printf("dma_d_axi_read_arb_id.a.value=0x%x\n", dma_d_axi_read_arb_id.a.value);
	//printf("read value=0x%x\n", value);
	
	//dma_d_axi_write_arb_id
	dma_d_axi_write_arb_id.a.value = read_register(REG_DMA_D_AXI_WRITE_ARB_ID);
        dma_d_axi_write_arb_id.a.b.axi_wchannel0_SPpri_or_DRRid = axi_wchannel0_SPpri_or_DRRid;
        dma_d_axi_write_arb_id.a.b.axi_wchannel1_SPpri_or_DRRid = axi_wchannel1_SPpri_or_DRRid;
        dma_d_axi_write_arb_id.a.b.axi_wchannel2_SPpri_or_DRRid = axi_wchannel2_SPpri_or_DRRid;
        dma_d_axi_write_arb_id.a.b.axi_wchannel3_SPpri_or_DRRid = axi_wchannel3_SPpri_or_DRRid;
        dma_d_axi_write_arb_id.a.b.axi_wchannel4_SPpri_or_DRRid = axi_wchannel4_SPpri_or_DRRid;
        dma_d_axi_write_arb_id.a.b.axi_wchannel5_SPpri_or_DRRid = axi_wchannel5_SPpri_or_DRRid;
	write_register(REG_DMA_D_AXI_WRITE_ARB_ID, dma_d_axi_write_arb_id.a.value);
	value = read_register(REG_DMA_D_AXI_WRITE_ARB_ID);
	//printf("dma_d_axi_write_arb_id.a.value=0x%x\n", dma_d_axi_write_arb_id.a.value);
	//printf("read value=0x%x\n", value);

	//dma_d_axi_read_channel_0_3_drr_weight
	dma_d_axi_read_channel_0_3_drr_weight.a.value = read_register(REG_DMA_D_AXI_READ_CHANNEL_0_3_DDR_WEIGHT);
	dma_d_axi_read_channel_0_3_drr_weight.a.b.axi_rchannel0_3_DRRweight_0_7 = axi_rchannel0_3_DRRweight_0_7;
	dma_d_axi_read_channel_0_3_drr_weight.a.b.axi_rchannel0_3_DRRweight_8_15 = axi_rchannel0_3_DRRweight_8_15;
	dma_d_axi_read_channel_0_3_drr_weight.a.b.axi_rchannel0_3_DRRweight_16_23 = axi_rchannel0_3_DRRweight_16_23;
	dma_d_axi_read_channel_0_3_drr_weight.a.b.axi_rchannel0_3_DRRweight_24_31 = axi_rchannel0_3_DRRweight_24_31;
	write_register(REG_DMA_D_AXI_READ_CHANNEL_0_3_DDR_WEIGHT, dma_d_axi_read_channel_0_3_drr_weight.a.value);
	value =  read_register(REG_DMA_D_AXI_READ_CHANNEL_0_3_DDR_WEIGHT);
	//printf("dma_d_axi_read_channel_0_3_drr_weight.a.value=0x%x\n", dma_d_axi_read_channel_0_3_drr_weight.a.value);
	//printf("read value=0x%x\n", value);

	//dma_d_axi_read_channel_4_7_drr_weight
        dma_d_axi_read_channel_4_7_drr_weight.a.value = read_register(REG_DMA_D_AXI_READ_CHANNEL_4_7_DDR_WEIGHT);
        dma_d_axi_read_channel_4_7_drr_weight.a.b.axi_rchannel4_7_DRRweight_0_7 = axi_rchannel4_7_DRRweight_0_7;
        dma_d_axi_read_channel_4_7_drr_weight.a.b.axi_rchannel4_7_DRRweight_8_15 = axi_rchannel4_7_DRRweight_8_15;
        write_register(REG_DMA_D_AXI_READ_CHANNEL_4_7_DDR_WEIGHT, dma_d_axi_read_channel_4_7_drr_weight.a.value);
	value = read_register(REG_DMA_D_AXI_READ_CHANNEL_4_7_DDR_WEIGHT);
	//printf("dma_d_axi_read_channel_4_7_drr_weight.a.value=0x%x\n", dma_d_axi_read_channel_4_7_drr_weight.a.value);
	//printf("read value=0x%x\n", value);
	

	//dma_d_axi_write_channel_0_3_drr_weight
        dma_d_axi_write_channel_0_3_drr_weight.a.value = read_register(REG_DMA_D_AXI_WRITE_CHANNEL_0_3_DDR_WEIGHT);
        dma_d_axi_write_channel_0_3_drr_weight.a.b.axi_wchannel0_3_DRRweight_0_7 = axi_wchannel0_3_DRRweight_0_7;
        dma_d_axi_write_channel_0_3_drr_weight.a.b.axi_wchannel0_3_DRRweight_8_15 = axi_wchannel0_3_DRRweight_8_15;
        dma_d_axi_write_channel_0_3_drr_weight.a.b.axi_wchannel0_3_DRRweight_16_23 = axi_wchannel0_3_DRRweight_16_23;
        dma_d_axi_write_channel_0_3_drr_weight.a.b.axi_wchannel0_3_DRRweight_24_31 = axi_wchannel0_3_DRRweight_24_31;
        write_register(REG_DMA_D_AXI_WRITE_CHANNEL_0_3_DDR_WEIGHT, dma_d_axi_write_channel_0_3_drr_weight.a.value);
	value = read_register(REG_DMA_D_AXI_WRITE_CHANNEL_0_3_DDR_WEIGHT);
	//printf("dma_d_axi_write_channel_0_3_drr_weight.a.value=0x%x\n", dma_d_axi_write_channel_0_3_drr_weight.a.value);
	//printf("read value=0x%x\n", value);

        //dma_d_axi_write_channel_4_7_drr_weight
        dma_d_axi_write_channel_4_7_drr_weight.a.value = read_register(REG_DMA_D_AXI_WRITE_CHANNEL_4_7_DDR_WEIGHT);
        dma_d_axi_write_channel_4_7_drr_weight.a.b.axi_wchannel4_7_DRRweight_0_7 = axi_wchannel4_7_DRRweight_0_7;
        dma_d_axi_write_channel_4_7_drr_weight.a.b.axi_wchannel4_7_DRRweight_8_15 = axi_wchannel4_7_DRRweight_8_15;
        write_register(REG_DMA_D_AXI_WRITE_CHANNEL_4_7_DDR_WEIGHT, dma_d_axi_write_channel_4_7_drr_weight.a.value);
	value = read_register(REG_DMA_D_AXI_WRITE_CHANNEL_4_7_DDR_WEIGHT);
	//printf("dma_d_axi_write_channel_4_7_drr_weight.a.value=0x%x\n", dma_d_axi_write_channel_4_7_drr_weight.a.value);
	//printf("read value=0x%x\n", value);

	//dma_d_axi_rwd_value
	dma_d_axi_rwd_value.a.value = read_register(REG_DMA_D_AXI_RWD_VALUE);
	dma_d_axi_rwd_value.a.b.axi_rid = axi_rid;
	dma_d_axi_rwd_value.a.b.axi_wid = axi_wid;
	write_register(REG_DMA_D_AXI_RWD_VALUE, dma_d_axi_rwd_value.a.value);
	value = read_register(REG_DMA_D_AXI_RWD_VALUE);
	//printf("dma_d_axi_rwd_value.a.value=0x%x\n", dma_d_axi_rwd_value.a.value);
	//printf("read value=0x%x\n", value);

	//RX DMA
	value = read_register(DMA_DMA_LSO_RXDMA_CONTROL);
	value |= 0x0c;
	write_register(DMA_DMA_LSO_RXDMA_CONTROL, value);

 	//TX DMA
        value = read_register(DMA_DMA_LSO_TXDMA_CONTROL);
        value |= 0x0c;
        write_register(DMA_DMA_LSO_TXDMA_CONTROL, value);

	return 0;

}

