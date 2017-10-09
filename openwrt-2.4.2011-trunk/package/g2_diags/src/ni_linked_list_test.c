#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#include "registers.h"

extern unsigned int *gp_value;

void cs_ni_link_list_diag(void)
{
#define NI_LINK_LIST_CNT                1024
        int i;
        NI_TOP_NI_RXFIFO_DBG_ACCESS_t   rxfifo_dbg_reg;
        cs_uint32       value;
	unsigned int reg_temp;

        rxfifo_dbg_reg.bf.access = 1;
        rxfifo_dbg_reg.bf.debug_mode = 1;
        rxfifo_dbg_reg.bf.mem_sel = 0x01;
        rxfifo_dbg_reg.bf.address = 0x3e;
  	wr_access( gp_value, NI_TOP_NI_RXFIFO_DBG_ACCESS, rxfifo_dbg_reg.wrd);
        // original - writel(rxfifo_dbg_reg.wrd, NI_TOP_NI_RXFIFO_DBG_ACCESS);
	
        // original - while (((NI_TOP_NI_RXFIFO_DBG_ACCESS) & 0x80000000) == 0x80000000)
	while ((rd_access( gp_value, NI_TOP_NI_RXFIFO_DBG_ACCESS, &reg_temp) & 0x80000000)==0x80000000);
                //udelay(1);

        //value = readl(NI_TOP_NI_RXFIFO_DBG_DBG_RDATA);
	value=rd_access( gp_value, NI_TOP_NI_RXFIFO_DBG_DATA, &reg_temp);

        printf("%s::RXFIFO buffer start pointer %08x\n", __func__, value);

        for (i=0; i<NI_LINK_LIST_CNT; i++) {
                rxfifo_dbg_reg.bf.access = 1;
                rxfifo_dbg_reg.bf.debug_mode = 1;
                rxfifo_dbg_reg.bf.mem_sel = 0x00;
                rxfifo_dbg_reg.bf.address = value;

  		wr_access( gp_value, NI_TOP_NI_RXFIFO_DBG_ACCESS, rxfifo_dbg_reg.wrd);
                //writel(rxfifo_dbg_reg.wrd, NI_TOP_NI_RXFIFO_DBG_ACCESS);

                // original - while ((readl(NI_TOP_NI_RXFIFO_DBG_ACCESS) & 0x80000000) == 0x80000000)
		while ((rd_access( gp_value, NI_TOP_NI_RXFIFO_DBG_ACCESS, &reg_temp) & 0x80000000)==0x80000000);
                        //udelay(1);

                //value = readl(NI_TOP_NI_RXFIFO_DBG_DATA);
		value=rd_access( gp_value, NI_TOP_NI_RXFIFO_DBG_DATA, &reg_temp);

                printf("%s::RXFIFO buffer %d, %08x\n", __func__, i, value);
        }
        return;
}
