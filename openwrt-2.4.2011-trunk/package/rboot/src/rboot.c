/****************************************************************************
            Software License for Customer Use of Cortina Software
                          Grant Terms and Conditions

IMPORTANT NOTICE - READ CAREFULLY: This Software License for Customer Use
of Cortina Software ("LICENSE") is the agreement which governs use of
software of Cortina Systems, Inc. and its subsidiaries ("CORTINA"),
including computer software (source code and object code) and associated
printed materials ("SOFTWARE").  The SOFTWARE is protected by copyright laws
and international copyright treaties, as well as other intellectual property
laws and treaties.  The SOFTWARE is not sold, and instead is only licensed
for use, strictly in accordance with this document.  Any hardware sold by
CORTINA is protected by various patents, and is sold but this LICENSE does
not cover that sale, since it may not necessarily be sold as a package with
the SOFTWARE.  This LICENSE sets forth the terms and conditions of the
SOFTWARE LICENSE only.  By downloading, installing, copying, or otherwise
using the SOFTWARE, you agree to be bound by the terms of this LICENSE.
If you do not agree to the terms of this LICENSE, then do not download the
SOFTWARE.

DEFINITIONS:  "DEVICE" means the Cortina Systems(TM) Daytona SDK product.
"You" or "CUSTOMER" means the entity or individual that uses the SOFTWARE.
"SOFTWARE" means the Cortina Systems(TM) SDK software.

GRANT OF LICENSE:  Subject to the restrictions below, CORTINA hereby grants
CUSTOMER a non-exclusive, non-assignable, non-transferable, royalty-free,
perpetual copyright license to (1) install and use the SOFTWARE for
reference only with the DEVICE; and (2) copy the SOFTWARE for your internal
use only for use with the DEVICE.

RESTRICTIONS:  The SOFTWARE must be used solely in conjunction with the
DEVICE and solely with Your own products that incorporate the DEVICE.  You
may not distribute the SOFTWARE to any third party.  You may not modify
the SOFTWARE or make derivatives of the SOFTWARE without assigning any and
all rights in such modifications and derivatives to CORTINA.  You shall not
through incorporation, modification or distribution of the SOFTWARE cause
it to become subject to any open source licenses.  You may not
reverse-assemble, reverse-compile, or otherwise reverse-engineer any
SOFTWARE provided in binary or machine readable form.  You may not
distribute the SOFTWARE to your customers without written permission
from CORTINA.

OWNERSHIP OF SOFTWARE AND COPYRIGHTS. All title and copyrights in and the
SOFTWARE and any accompanying printed materials, and copies of the SOFTWARE,
are owned by CORTINA. The SOFTWARE protected by the copyright laws of the
United States and other countries, and international treaty provisions.
You may not remove any copyright notices from the SOFTWARE.  Except as
otherwise expressly provided, CORTINA grants no express or implied right
under CORTINA patents, copyrights, trademarks, or other intellectual
property rights.

DISCLAIMER OF WARRANTIES. THE SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY
EXPRESS OR IMPLIED WARRANTY OF ANY KIND, INCLUDING ANY IMPLIED WARRANTIES
OF MERCHANTABILITY, NONINFRINGEMENT, OR FITNESS FOR A PARTICULAR PURPOSE,
TITLE, AND NON-INFRINGEMENT.  CORTINA does not warrant or assume
responsibility for the accuracy or completeness of any information, text,
graphics, links or other items contained within the SOFTWARE.  Without
limiting the foregoing, you are solely responsible for determining and
verifying that the SOFTWARE that you obtain and install is the appropriate
version for your purpose.

LIMITATION OF LIABILITY. IN NO EVENT SHALL CORTINA OR ITS SUPPLIERS BE
LIABLE FOR ANY DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION, LOST
PROFITS, BUSINESS INTERRUPTION, OR LOST INFORMATION) OR ANY LOSS ARISING OUT
OF THE USE OF OR INABILITY TO USE OF OR INABILITY TO USE THE SOFTWARE, EVEN
IF CORTINA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
TERMINATION OF THIS LICENSE. This LICENSE will automatically terminate if
You fail to comply with any of the terms and conditions hereof. Upon
termination, You will immediately cease use of the SOFTWARE and destroy all
copies of the SOFTWARE or return all copies of the SOFTWARE in your control
to CORTINA.  IF you commence or participate in any legal proceeding against
CORTINA, then CORTINA may, in its sole discretion, suspend or terminate all
license grants and any other rights provided under this LICENSE during the
pendency of such legal proceedings.
APPLICABLE LAWS. Claims arising under this LICENSE shall be governed by the
laws of the State of California, excluding its principles of conflict of
laws.  The United Nations Convention on Contracts for the International Sale
of Goods is specifically disclaimed.  You shall not export the SOFTWARE
without first obtaining any required export license or other approval from
the applicable governmental entity, if required.  This is the entire
agreement and understanding between You and CORTINA relating to this subject
matter.
GOVERNMENT RESTRICTED RIGHTS. The SOFTWARE is provided with "RESTRICTED
RIGHTS." Use, duplication, or disclosure by the Government is subject to
restrictions as set forth in FAR52.227-14 and DFAR252.227-7013 et seq. or
its successor. Use of the SOFTWARE by the Government constitutes
acknowledgment of CORTINA's proprietary rights therein. Contractor or
Manufacturer is CORTINA.

Copyright (c) 2009 by Cortina Systems Incorporated
****************************************************************************/
/***********************************************************************/
/* This file contains unpublished documentation and software           */
/* proprietary to Cortina Systems Incorporated. Any use or disclosure, */
/* in whole or in part, of the information in this file without a      */
/* written consent of an officer of Cortina Systems Incorporated is    */
/* strictly prohibited.                                                */
/* Copyright (c) 2010 by Cortina Systems Incorporated.                 */
/***********************************************************************/


#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "rboot.h"

#undef DEBUG

/* Memory map of RCPU's, the array should be sorted */

g2_mem_map_t g2_mem_map[]={
  {0x01E08000, 0x01E0Bfff, 0x4000},    // DDR
  {0x01E0C000, 0x01E0ffff, 0x4000},    // DDR
  {0x01EFC000, 0x01EFFFFF, 0x4000},    // DDR
  {0xf6000000, 0xf6001fff, 0x2000},   // DRAM0
  {0xf6008000, 0xf600dfff, 0x6000},   // IRAM0
  {0xf6020000, 0xf6021fff, 0x2000},   // DRAM1
  {0xf6028000, 0xf602dfff, 0x6000},   // IRAM1
  {0xf6200000, 0xf6207fff, 0x8000},   // RRAM0
  {0xf6400000, 0xf6407fff, 0x8000},    // RRAM1
  {0xF6A3C000, 0xF6A3ffff, 0x4000}    // PKTBUFF
};

int dev_fd;
char rcpu0_img[MAX_FNAME_LEN] = "rcpu_0";
char rcpu1_img[MAX_FNAME_LEN] = "rcpu_1";

char elf_magic[EI_NIDENT]= {0x7f, 0x45, 0x4c, 0x46,
                            0x01, 0x01, 0x01, 0x00,
                            0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00};

void print_help(void)
{
  printf("\nrboot [rcpu_0] [rcpu_1]\n");
  printf("\t Bootloader for RCPU - Accepts ELF images only\n");
  printf("\t default images rcpu_0 for RCPU0 and rcpu_1 for RCPU1\n\n");
}


int copy_image(FILE *fp)
{
  int status = CS_ERROR;
  Elf32_Ehdr ehdr;
  uint shdr_sz, shdr_num, shdr_off;
  uint mem_elemts;
  Elf32_Shdr *shdr, *tshdr;
  char *shdr_name, *rcpu_mem, *pmem;
  size_t readb = 0;

  if( fread(&ehdr, sizeof(Elf32_Ehdr), 1, fp) != 1) {
    printf("error reading file\n");
    return CS_ERROR;
  }

  if( (strncmp(ehdr.e_ident, elf_magic, EI_NIDENT) != 0) ||
                              (ehdr.e_machine != EM_XTENSA)){
    printf("Invalid Tensilica image !!!\n");
    return CS_ERROR;
  }

  shdr_sz = ehdr.e_shentsize;
  shdr_num = ehdr.e_shnum;
  shdr_off = ehdr.e_shoff;

  if( (shdr = (Elf32_Shdr *) malloc(shdr_sz * shdr_num)) == NULL) {
    printf("error Allocating memory\n");
    return CS_ERROR;
  }

  fseek(fp, shdr_off, SEEK_SET);
  if(fread(shdr, (shdr_sz * shdr_num), 1, fp) != 1 ) {
    printf("error reading section header from file\n");
    goto copy_err;
  }

  tshdr = (shdr+ ehdr.e_shstrndx);
  if((shdr_name = (char *) malloc(tshdr->sh_size)) == NULL ) {
    printf("error Allocating memory\n");
    goto copy_err;
  }

  fseek(fp, tshdr->sh_offset, SEEK_SET);
  if( (fread(shdr_name, tshdr->sh_size,1,fp) != 1) ) {
    printf("error reading file\n");
    goto copy_err;
  }

  for( tshdr=shdr; shdr_num > 0; shdr_num--, tshdr++) {
    if( tshdr->sh_addr == 0 ) {
      // Ignore non text and data sections (address is 0)
      continue;
    }

#ifdef DEBUG
    printf("Copying %-20s to Address 0x%08x of Size 0x%08x\n",
           (shdr_name+tshdr->sh_name), tshdr->sh_addr, tshdr->sh_size);
#endif

    mem_elemts = (sizeof(g2_mem_map)/sizeof(g2_mem_map_t));
    // Check if contents of given section overflow the physical memory
    while((mem_elemts--) && (tshdr->sh_addr < g2_mem_map[mem_elemts].mem_start));

    if( (tshdr->sh_addr + tshdr->sh_size) > g2_mem_map[mem_elemts].mem_end ) {
      printf("Section %s , exceeds physical memory!!!\n",
                          (shdr_name + tshdr->sh_name));
      goto copy_err;
    }

#ifdef DEBUG
    printf("Memory Remap region [0x%08x - 0x%08x] \n",
              g2_mem_map[mem_elemts].mem_start,g2_mem_map[mem_elemts].mem_end);
#endif

    rcpu_mem = mmap(0, g2_mem_map[mem_elemts].mem_size,
                        PROT_READ|PROT_WRITE, MAP_SHARED, dev_fd,
                                    g2_mem_map[mem_elemts].mem_start);
    if( rcpu_mem == MAP_FAILED ) {
      printf("Unable to map 0x%08x size 0x%x\n",
          g2_mem_map[mem_elemts].mem_start, g2_mem_map[mem_elemts].mem_size);
      goto copy_err;
    }

    fseek(fp, tshdr->sh_offset, SEEK_SET);
    pmem = rcpu_mem + (tshdr->sh_addr - g2_mem_map[mem_elemts].mem_start);
    //if(fread(pmem, tshdr->sh_size, 1, fp) != 1) {
    if((readb = fread(pmem, sizeof(char), (tshdr->sh_size + 3) & 0xfffffffc, fp))
       != (tshdr->sh_size + 3) & 0xfffffffc) {
      printf("error reading file!!!\n");
      munmap(rcpu_mem, g2_mem_map[mem_elemts].mem_size);
      goto copy_err;
    }
    munmap(rcpu_mem, g2_mem_map[mem_elemts].mem_size);
  }

  status = CS_OK;

copy_err:
  if(shdr_name != NULL)
    free(shdr_name);
  if(shdr != NULL)
    free(shdr);
  return status;
}

/*
 * 1. Open /dev/mem interface to access g2 registers
 * 2. Configure RCPUs in run/stall mode and release reset.
 * 3. Copy Image to appropriate locations
 * 4. Start RCPU's
 *
 */

int main(int argc, char *argv[])
{
  FILE *fp0=NULL, *fp1=NULL;
  struct stat fstat;
  char *reg_mem = MAP_FAILED;
  uint *reg;
  volatile unsigned int tmp;


  if((argc > 3) || ((argc > 1) && !(strncmp(argv[1], "-h", 2)))){
    print_help();
    return 0;
  }

  if( argc >= 2) {
    strcpy(rcpu0_img, argv[1]);
    printf("RCPU0 image file : %s\n",rcpu0_img);
  }

  if( argc == 3) {
    strcpy(rcpu1_img, argv[2]);
  printf("RCPU1 image file : %s\n",rcpu1_img);
  }

  if((dev_fd=open("/dev/mem", O_RDWR|O_SYNC)) < 0 ) {
    printf("Unable to open /dev/mem \n");
    return 1;
  }

  /* Map G2 register space */
  reg_mem = mmap(0, G2_MAP_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED,
                                          dev_fd, G2_GLOBAL_REG_BASE);
  if( reg_mem == MAP_FAILED ) {
    printf("mmap : failed to map global register space\n");
    return 1;
  }

  /*
   * Reset RCPUs and keep at stall mode then release from reset.
   */

  printf("Reset RCPU...\n");
  reg = (uint *)(reg_mem + GLOBAL_BLOCK_RESET);
  *reg |= (RCPU0_RESET|RCPU1_RESET);

  printf("Pushing RCPU's into Run-Stall and releasing from reset\n");
  reg = (uint *)(reg_mem + GLOBAL_RECIRC_CPU_CTL);
  *reg |= (RCPU0_RUNSTALL | RCPU1_RUNSTALL);

  reg = (uint *)(reg_mem + GLOBAL_BLOCK_RESET);
  *reg &= ~(RCPU0_RESET|RCPU1_RESET);

  /*
   * Copy code to destined locations
   */

  if( ((fp0 = fopen(rcpu0_img, "rb")) == NULL ) ||
        ((fp1 = fopen(rcpu1_img, "rb")) == NULL ) &&
	(argc == 3)) {
    printf("error Opening RCPU image files\n");
    goto error;
  }

  printf("Copying RCPU0 image to destination memory...");
  if( copy_image(fp0) != 0 ) {
    goto error;
  }
  printf("done\n");


  reg = (uint *)(reg_mem + GLOBAL_RECIRC_CPU_CTL);
  *reg &= ~(RCPU0_RUNSTALL);

  printf("Released RCPU from Run-Stall\n");

  for(tmp=0;tmp<0x100;tmp++);

  if( argc == 3) {
  printf("Copying RCPU1 image to destination memory...");
  if( copy_image(fp1) != 0 ) {
    goto error;
  }
  printf("done\n");
  }

  /*
   * Start RCPUs
   */

  reg = (uint *)(reg_mem + GLOBAL_RECIRC_CPU_CTL);
  if( argc == 3) {
    *reg &= ~RCPU1_RUNSTALL;
  }

  printf("Released RCPU from Run-Stall\n");

error:
  if(fp1 != NULL ) fclose(fp1);
  if(fp0 != NULL ) fclose(fp0);
  if(reg_mem != MAP_FAILED) munmap(reg_mem, G2_MAP_SIZE);
  close(dev_fd);
  return 0;
}
