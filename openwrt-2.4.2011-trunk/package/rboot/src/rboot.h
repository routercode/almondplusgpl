#ifndef __BOOTRCPU_H__
#define __BOOTRCPU_H__

#ifdef __cplusplus
extern "C" {
#endif

#define G2_GLOBAL_REG_BASE    0xf0000000
#define G2_MAP_SIZE           4096
#define MAX_BUF_SIZE          0x100000
#define MAX_SHNAME_BUF_SIZE   0x1000
#define MAX_FNAME_LEN         32

// Register offsets

#define GLOBAL_JTAG_ID          0x00
#define GLOBAL_BLOCK_RESET      0x04
#define GLOBAL_RECIRC_CPU_CTL   0x84

#define RCPU0_RUNSTALL        (1<<3)
#define RCPU1_RUNSTALL        (1<<8)
#define RCPU0_RESET           (1<<21)
#define RCPU1_RESET           (1<<22)


typedef struct {
  uint mem_start;
  uint mem_end;
  uint mem_size;
}g2_mem_map_t;


#define CS_OK       0
#define CS_ERROR    1

#ifdef __cplusplus
}
#endif

#endif /* __BOOTRCPU_H__ */
