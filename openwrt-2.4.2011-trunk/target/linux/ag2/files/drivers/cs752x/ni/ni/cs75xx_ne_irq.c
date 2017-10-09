/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <mach/registers.h>
#include <asm/io.h>
#include <linux/kernel.h>
#include "cs75xx_ne_irq.h"
#include "cs752x_eth.h"

#define DBG(x) {}
#if 0
#ifdef CONFIG_CS752X_PROC
#define DBG(x) {if (cs_ni_debug == DBG_IRQ || cs_ni_debug == DBG_NI_IRQ) x;}
#else
#define DBG(x) {}
#endif
#endif

/*
 * ======= Interrupt Hierarchy ======
 * ni top
 *	ni_xram_rx
 *
 *	ni_xram_tx
 *
 *	ni
 *		ni_port 0~7
 *		ni_rxfifo
 *		ni_txem
 *		ni_pc 0~2
 *		ni_cpuxram_int_stat_cntr
 *		ni_cpuxram_int_stat_err
 *
 *	fe
 *		fe_fwd
 *		fe_lpm
 *		fe_hash
 *		fe_pe
 *		fe_dbg
 *
 *	tm
 *		tm_bm
 *		tm_pol
 *		tm_pm
 *
 *	qm
 *
 *	sch
 */

const static struct cs_ne_irq_info cs_ne_fedbg_irq_info = {
	.module_name = "FE_DBG",
	.intr_status = (FEDBG_HDR_D_BUF_OVF_INT_STATUS |
			FEDBG_CLASS_RSLT_BUF_INT_OVF_INT_STATUS |
			FEDBG_CLASS_RSLT_BUF_OVF_INT_STATUS |
			FEDBG_HLKP_RSLT_BUF_INT_OVF_INT_STATUS |
			FEDBG_HLKP_RSLT_BUF_OVF_INT_STATUS |
			FEDBG_LPM_RSLT_BUF_INT_OVF_INT_STATUS |
			FEDBG_LPM_RSLT_BUF_OVF_INT_STATUS |
			FEDBG_FWD_RSLT_BUF_INT_OVF_INT_STATUS |
			FEDBG_FWD_RSLT_BUF_OVF_INT_STATUS |
			FEDBG_QOS_RSLT_BUF_INT_OVF_INT_STATUS |
			FEDBG_QOS_RSLT_BUF_OVF_INT_STATUS |
			FEDBG_INBUF_OVF_INT_STATUS |
			FEDBG_TM_BUF_OVF_INT_STATUS |
			FEDBG_CLASS_HIT_INT_STATUS),

	.intr_enable = (FEDBG_HDR_D_BUF_OVF_INT_ENABLE |
			FEDBG_CLASS_RSLT_BUF_INT_OVF_INT_ENABLE |
			FEDBG_CLASS_RSLT_BUF_OVF_INT_ENABLE |
			FEDBG_HLKP_RSLT_BUF_INT_OVF_INT_ENABLE |
			FEDBG_HLKP_RSLT_BUF_OVF_INT_ENABLE |
			FEDBG_LPM_RSLT_BUF_INT_OVF_INT_ENABLE |
			FEDBG_LPM_RSLT_BUF_OVF_INT_ENABLE |
			FEDBG_FWD_RSLT_BUF_INT_OVF_INT_ENABLE |
			FEDBG_FWD_RSLT_BUF_OVF_INT_ENABLE |
			FEDBG_QOS_RSLT_BUF_INT_OVF_INT_ENABLE |
			FEDBG_QOS_RSLT_BUF_OVF_INT_ENABLE |
			FEDBG_INBUF_OVF_INT_ENABLE |
			FEDBG_TM_BUF_OVF_INT_ENABLE |
			FEDBG_CLASS_HIT_INT_ENABLE),

	.intr_e_reg  = FETOP_FE_DBG_INTENABLE_0,

	.intr_handle = cs_ne_fedbg_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_fehash_irq_info = {
	.module_name = "FE_HASH",
	.intr_status = (FEHASH_HASHMEM_CORR_ECC_ERR_STATUS |
			FEHASH_HASHMEM_UNCORR_ECC_ERR_STATUS |
			FEHASH_MASKMEM_CORR_ECC_ERR_STATUS |
			FEHASH_MASKMEM_UNCORR_ECC_ERR_STATUS |
			FEHASH_STATUSMEM_CORR_ECC_ERR_STATUS |
			FEHASH_STATUSMEM_UNCORR_ECC_ERR_STATUS |
			FEHASH_HASHMEM_MULT_HIT_STATUS |
			FEHASH_HASH_OVERFLOW_MULT_HIT_STATUS |
			FEHASH_CHECKMEM_PARITY_FAIL_STATUS),

	.intr_enable = (FEHASH_HASHMEM_CORR_ECC_ERR_ENABLE |
			FEHASH_HASHMEM_UNCORR_ECC_ERR_ENABLE |
			FEHASH_MASKMEM_CORR_ECC_ERR_ENABLE |
			FEHASH_MASKMEM_UNCORR_ECC_ERR_ENABLE |
			FEHASH_STATUSMEM_CORR_ECC_ERR_ENABLE |
			FEHASH_STATUSMEM_UNCORR_ECC_ERR_ENABLE |
			FEHASH_HASHMEM_MULT_HIT_ENABLE |
			FEHASH_HASH_OVERFLOW_MULT_HIT_ENABLE |
			FEHASH_CHECKMEM_PARITY_FAIL_ENABLE),

	.intr_e_reg  = FETOP_HASH_INTENABLE_0,

	.intr_handle = cs_ne_fehash_intr_handle,

};

const static struct cs_ne_irq_info cs_ne_felpm_irq_info = {
	.module_name = "FE_LPM",
	.intr_status = (FELPM_TABLE0_UPPER_PAR_ERR_INT_STATUS |
			FELPM_TABLE0_LOWER_PAR_ERR_INT_STATUS |
			FELPM_TABLE1_UPPER_PAR_ERR_INT_STATUS |
			FELPM_TABLE1_LOWER_PAR_ERR_INT_STATUS |
			FELPM_HC_TABLE0_UPPER_PAR_ERR_INT_STATUS |
			FELPM_HC_TABLE0_LOWER_PAR_ERR_INT_STATUS |
			FELPM_HC_TABLE1_UPPER_PAR_ERR_INT_STATUS |
			FELPM_HC_TABLE1_LOWER_PAR_ERR_INT_STATUS |
			FELPM_HC0_HALF_ROLL_INT_STATUS |
			FELPM_HC0_FULL_ROLL_INT_STATUS |
			FELPM_HC0_HALF_ROLL_INT_STATUS |
			FELPM_HC1_FULL_ROLL_INT_STATUS),

	.intr_enable = (FELPM_TABLE0_UPPER_PAR_ERR_INT_ENABLE |
			FELPM_TABLE0_LOWER_PAR_ERR_INT_ENABLE |
			FELPM_TABLE1_UPPER_PAR_ERR_INT_ENABLE |
			FELPM_TABLE1_LOWER_PAR_ERR_INT_ENABLE |
			FELPM_HC_TABLE0_UPPER_PAR_ERR_INT_ENABLE |
			FELPM_HC_TABLE0_LOWER_PAR_ERR_INT_ENABLE |
			FELPM_HC_TABLE1_UPPER_PAR_ERR_INT_ENABLE |
			FELPM_HC_TABLE1_LOWER_PAR_ERR_INT_ENABLE |
			FELPM_HC0_HALF_ROLL_INT_ENABLE |
			FELPM_HC0_FULL_ROLL_INT_ENABLE |
			FELPM_HC0_HALF_ROLL_INT_ENABLE |
			FELPM_HC1_FULL_ROLL_INT_ENABLE),

	.intr_e_reg  = FETOP_LPM_INTENABLE_0,

	.intr_handle = cs_ne_felpm_intr_handle,

};

const static struct cs_ne_irq_info cs_ne_fepe_irq_info = {
	.module_name = "FE_PE",
	.intr_status = (FEPE_DATA_FIFO_OVERFLOW_INT_STATUS |
			FEPE_CMD_FIFO_OVERFLOW_INT_STATUS |
			FEPE_FE_PKT_CNT_MSB_SETL_STATUS |
			FEPE_FE_PKT_DROP_MSB_SETL_STATUS |
			FEPE_HDRA_PKT_DROP_MSB_SETL_STATUS |
			FEPE_RUNT_PKT_DETL_STATUS |
			FEPE_IPV6_UDPCSUM_0_DETL_STATUS),

	.intr_enable = (FEPE_DATA_FIFO_OVERFLOW_INT_ENABLE |
			FEPE_CMD_FIFO_OVERFLOW_INT_ENABLE |
			FEPE_FE_PKT_CNT_MSB_SETL_ENABLE |
			FEPE_FE_PKT_DROP_MSB_SETL_ENABLE |
			FEPE_HDRA_PKT_DROP_MSB_SETL_ENABLE |
			FEPE_RUNT_PKT_DETL_ENABLE |
			FEPE_IPV6_UDPCSUM_0_DETL_ENABLE),

	.intr_e_reg  = FETOP_FE_PE_INTENABLE_0,

	.intr_handle = cs_ne_fepe_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_fefwd_irq_info = {
	.module_name = "FE_FWD",
	.intr_status = (FEFWD_DROP_INT_STATUS |
			FEFWD_CLASS0_PORT_CHK_FAIL_STATUS |
			FEFWD_CLASS1_PORT_CHK_FAIL_STATUS |
			FEFWD_CLASS2_PORT_CHK_FAIL_STATUS |
			FEFWD_CLASS3_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLRULE0_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLRULE1_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLRULE2_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLRULE3_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLRULE4_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLRULE5_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLACT0_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLACT1_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLACT2_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLACT3_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLACT4_PORT_CHK_FAIL_STATUS |
			FEFWD_ACLACT5_PORT_CHK_FAIL_STATUS |
			FEFWD_FWDRSLT_PORT_CHK_FAIL_STATUS |
			FEFWD_QOSRSLT_PORT_CHK_FAIL_STATUS |
			FEFWD_L2TABLE_PORT_CHK_FAIL_STATUS |
			FEFWD_L3TABLE_PORT_CHK_FAIL_STATUS |
			FEFWD_VOQPOL_PORT_CHK_FAIL_STATUS |
			FEFWD_FLOWVLAN_PORT_CHK_FAIL_STATUS |
			FEFWD_VLANTBL_PORT_CHK_FAIL_STATUS),

	.intr_status = (FEFWD_DROP_INT_ENABLE |
			FEFWD_CLASS0_PORT_CHK_FAIL_ENABLE |
			FEFWD_CLASS1_PORT_CHK_FAIL_ENABLE |
			FEFWD_CLASS2_PORT_CHK_FAIL_ENABLE |
			FEFWD_CLASS3_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLRULE0_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLRULE1_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLRULE2_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLRULE3_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLRULE4_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLRULE5_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLACT0_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLACT1_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLACT2_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLACT3_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLACT4_PORT_CHK_FAIL_ENABLE |
			FEFWD_ACLACT5_PORT_CHK_FAIL_ENABLE |
			FEFWD_FWDRSLT_PORT_CHK_FAIL_ENABLE |
			FEFWD_QOSRSLT_PORT_CHK_FAIL_ENABLE |
			FEFWD_L2TABLE_PORT_CHK_FAIL_ENABLE |
			FEFWD_L3TABLE_PORT_CHK_FAIL_ENABLE |
			FEFWD_VOQPOL_PORT_CHK_FAIL_ENABLE |
			FEFWD_FLOWVLAN_PORT_CHK_FAIL_ENABLE |
			FEFWD_VLANTBL_PORT_CHK_FAIL_ENABLE),

	.intr_e_reg  = FETOP_FE_FWD_INTENABLE_0,

	.intr_handle = cs_ne_fefwd_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_fetop_irq_info = {
	.module_name = "FE_TOP",
	.intr_status = (FETOP_FWD_INT_STATUS |
			FETOP_LPM_INT_STATUS |
			FETOP_HASH_INT_STATUS |
			FETOP_PE_INT_STATUS | FETOP_DBG_INT_STATUS),

	.intr_enable = (FETOP_FWD_INT_ENABLE |
			FETOP_LPM_INT_ENABLE |
			FETOP_HASH_INT_ENABLE |
			FETOP_PE_INT_ENABLE | FETOP_DBG_INT_ENABLE),

	.intr_e_reg  = FETOP_FE_INTENABLE_0,

	.intr_handle = cs_ne_fetop_intr_handle,

	.sub_module[0] = &cs_ne_fefwd_irq_info,
	.sub_module[1] = &cs_ne_felpm_irq_info,
	.sub_module[2] = &cs_ne_fehash_irq_info,
	.sub_module[3] = &cs_ne_fepe_irq_info,
	.sub_module[31] = &cs_ne_fedbg_irq_info,

	.sub_intr_reg[0] = FETOP_FE_FWD_INTERRUPT_0,
	.sub_intr_reg[1] = FETOP_LPM_INTERRUPT_0,
	.sub_intr_reg[2] = FETOP_HASH_INTERRUPT_0,
	.sub_intr_reg[3] = FETOP_FE_PE_INTERRUPT_0,
	.sub_intr_reg[31] = FETOP_FE_DBG_INTERRUPT_0,
};

/* TM Interrupts */
const static struct cs_ne_irq_info cs_ne_tmpm_irq_info = {
	.module_name = "TM_PM",
	.intr_status = (TM_PM_FLOW_CNTR_MSB0_STATUS |
			TM_PM_FLOW_CNTR_MSB1_STATUS |
			TM_PM_FLOW_CNTR_MSB2_STATUS |
			TM_PM_FLOW_CNTR_MSB3_STATUS |
			TM_PM_FLOW_CNTR_MSB4_STATUS |
			TM_PM_FLOW_CNTR_MSB5_STATUS |
			TM_PM_FLOW_CNTR_MSB6_STATUS |
			TM_PM_FLOW_CNTR_MSB7_STATUS |
			TM_PM_SPID_CNTR_MSB0_STATUS |
			TM_PM_VOQ_CNTR_MSB0_STATUS |
			TM_PM_VOQ_CNTR_MSB1_STATUS |
			TM_PM_CPU_CNTR_MSB0_STATUS |
			TM_PM_PKT_TYPE_CNTR_MSB0_STATUS |
			TM_PM_PKT_TYPE_CNTR_MSB1_STATUS |
			TM_PM_CPU_COPY_CNTR_MSB0_STATUS |
			TM_PM_CPU_COPY_CNTR_MSB1_STATUS |
			TM_PM_CPU_COPY_CNTR_MSB2_STATUS |
			TM_PM_CPU_COPY_CNTR_MSB3_STATUS |
			TM_PM_GLB_CNTR_MSB0_STATUS |
			TM_PM_ERR_CNTR_MEM_STATUS |
			TM_PM_ERR_GLB_CNTR_MEM_STATUS),

	.intr_enable = (TM_PM_FLOW_CNTR_MSB0_ENABLE |
			TM_PM_FLOW_CNTR_MSB1_ENABLE |
			TM_PM_FLOW_CNTR_MSB2_ENABLE |
			TM_PM_FLOW_CNTR_MSB3_ENABLE |
			TM_PM_FLOW_CNTR_MSB4_ENABLE |
			TM_PM_FLOW_CNTR_MSB5_ENABLE |
			TM_PM_FLOW_CNTR_MSB6_ENABLE |
			TM_PM_FLOW_CNTR_MSB7_ENABLE |
			TM_PM_SPID_CNTR_MSB0_ENABLE |
			TM_PM_VOQ_CNTR_MSB0_ENABLE |
			TM_PM_VOQ_CNTR_MSB1_ENABLE |
			TM_PM_CPU_CNTR_MSB0_ENABLE |
			TM_PM_PKT_TYPE_CNTR_MSB0_ENABLE |
			TM_PM_PKT_TYPE_CNTR_MSB1_ENABLE |
			TM_PM_CPU_COPY_CNTR_MSB0_ENABLE |
			TM_PM_CPU_COPY_CNTR_MSB1_ENABLE |
			TM_PM_CPU_COPY_CNTR_MSB2_ENABLE |
			TM_PM_CPU_COPY_CNTR_MSB3_ENABLE |
			TM_PM_GLB_CNTR_MSB0_ENABLE |
			TM_PM_ERR_CNTR_MEM_ENABLE |
			TM_PM_ERR_GLB_CNTR_MEM_ENABLE),

	.intr_e_reg  = TM_PM_INTENABLE_0,

	.intr_handle = cs_ne_tmpm_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_tmpol_irq_info = {
	.module_name = "TM_POL",
	.intr_status = (TM_POL_ERR_FLOW_PROFILE_MEM_STATUS |
			TM_POL_ERR_CORRECT_FLOW_PROFILE_MEM_STATUS |
			TM_POL_ERR_FLOW_STATUS_MEM_STATUS |
			TM_POL_ERR_CORRECT_FLOW_STATUS_MEM_STATUS |
			TM_POL_ERR_SPID_PROFILE_MEM_STATUS |
			TM_POL_ERR_CORRECT_SPID_PROFILE_MEM_STATUS |
			TM_POL_ERR_SPID_STATUS_MEM_STATUS |
			TM_POL_ERR_CORRECT_SPID_STATUS_MEM_STATUS |
			TM_POL_ERR_CPU_PROFILE_MEM_STATUS |
			TM_POL_ERR_CORRECT_CPU_PROFILE_MEM_STATUS |
			TM_POL_ERR_CPU_STATUS_MEM_STATUS |
			TM_POL_ERR_CORRECT_CPU_STATUS_MEM_STATUS |
			TM_POL_ERR_PKT_TYPE_PROFILE_MEM_STATUS |
			TM_POL_ERR_CORRECT_PKT_TYPE_PROFILE_MEM_STATUS |
			TM_POL_ERR_PKT_TYPE_STATUS_MEM_STATUS |
			TM_POL_ERR_CORRECT_PKT_TYPE_STATUS_MEM_STATUS),

	.intr_enable = (TM_POL_ERR_FLOW_PROFILE_MEM_ENABLE |
			TM_POL_ERR_CORRECT_FLOW_PROFILE_MEM_ENABLE |
			TM_POL_ERR_FLOW_STATUS_MEM_ENABLE |
			TM_POL_ERR_CORRECT_FLOW_STATUS_MEM_ENABLE |
			TM_POL_ERR_SPID_PROFILE_MEM_ENABLE |
			TM_POL_ERR_CORRECT_SPID_PROFILE_MEM_ENABLE |
			TM_POL_ERR_SPID_STATUS_MEM_ENABLE |
			TM_POL_ERR_CORRECT_SPID_STATUS_MEM_ENABLE |
			TM_POL_ERR_CPU_PROFILE_MEM_ENABLE |
			TM_POL_ERR_CORRECT_CPU_PROFILE_MEM_ENABLE |
			TM_POL_ERR_CPU_STATUS_MEM_ENABLE |
			TM_POL_ERR_CORRECT_CPU_STATUS_MEM_ENABLE |
			TM_POL_ERR_PKT_TYPE_PROFILE_MEM_ENABLE |
			TM_POL_ERR_CORRECT_PKT_TYPE_PROFILE_MEM_ENABLE |
			TM_POL_ERR_PKT_TYPE_STATUS_MEM_ENABLE |
			TM_POL_ERR_CORRECT_PKT_TYPE_STATUS_MEM_ENABLE),

	.intr_e_reg  = TM_POL_INTENABLE_0,

	.intr_handle = cs_ne_tmpol_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_tmbm_irq_info = {
	.module_name = "TM_BM",
	.intr_status = (TM_BM_ERR_VOQ_MEM_STATUS |
			TM_BM_ERR_CORRECT_VOQ_MEM_STATUS |
			TM_BM_ERR_VOQ_PROFILE_MEM_STATUS |
			TM_BM_ERR_CORRECT_VOQ_PROFILE_MEM_STATUS |
			TM_BM_ERR_VOQ_STATUS_MEM_STATUS |
			TM_BM_ERR_CORRECT_VOQ_STATUS_MEM_STATUS |
			TM_BM_ERR_DEST_PORT_MEM_STATUS |
			TM_BM_ERR_CORRECT_DEST_PORT_MEM_STATUS |
			TM_BM_ERR_DEST_PORT_STATUS_MEM_STATUS |
			TM_BM_ERR_CORRECT_DEST_PORT_STATUS_MEM_STATUS |
			TM_BM_ERR_WRED_PROFILE_MEM_STATUS |
			TM_BM_ERR_CORRECT_WRED_PROFILE_MEM_STATUS |
			TM_BM_ERR_GLOBAL_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_CPU_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX0_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX1_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX2_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX3_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX4_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX5_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX6_BUFFERS_USED_STATUS |
			TM_BM_ERR_GLOBAL_LINUX7_BUFFERS_USED_STATUS),

	.intr_enable = (TM_BM_ERR_VOQ_MEM_ENABLE |
			TM_BM_ERR_CORRECT_VOQ_MEM_ENABLE |
			TM_BM_ERR_VOQ_PROFILE_MEM_ENABLE |
			TM_BM_ERR_CORRECT_VOQ_PROFILE_MEM_ENABLE |
			TM_BM_ERR_VOQ_STATUS_MEM_ENABLE |
			TM_BM_ERR_CORRECT_VOQ_STATUS_MEM_ENABLE |
			TM_BM_ERR_DEST_PORT_MEM_ENABLE |
			TM_BM_ERR_CORRECT_DEST_PORT_MEM_ENABLE |
			TM_BM_ERR_DEST_PORT_STATUS_MEM_ENABLE |
			TM_BM_ERR_CORRECT_DEST_PORT_STATUS_MEM_ENABLE |
			TM_BM_ERR_WRED_PROFILE_MEM_ENABLE |
			TM_BM_ERR_CORRECT_WRED_PROFILE_MEM_ENABLE |
			TM_BM_ERR_GLOBAL_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_CPU_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX0_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX1_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX2_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX3_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX4_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX5_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX6_BUFFERS_USED_ENABLE |
			TM_BM_ERR_GLOBAL_LINUX7_BUFFERS_USED_ENABLE),

	.intr_e_reg  = TM_BM_INTENABLE_0,

	.intr_handle = cs_ne_tmbm_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_tmtop_irq_info = {
	.module_name = "TM_TOP",
	.intr_status = (TM_TC_BM_STATUS | TM_TC_POL_STATUS | TM_TC_PM_STATUS),

	.intr_enable = (TM_TC_BM_ENABLE | TM_TC_POL_ENABLE | TM_TC_PM_ENABLE),

	.intr_e_reg  = TM_TC_INTENABLE_0,

	.intr_handle = cs_ne_tmtop_intr_handle,
	.sub_module[0] = &cs_ne_tmbm_irq_info,
	.sub_module[1] = &cs_ne_tmpol_irq_info,
	.sub_module[2] = &cs_ne_tmpm_irq_info,

	.sub_intr_reg[0] = TM_BM_INTERRUPT_0,
	.sub_intr_reg[1] = TM_POL_INTERRUPT_0,
	.sub_intr_reg[2] = TM_PM_INTERRUPT_0,
};

/* QM Interrupts */
const static struct cs_ne_irq_info cs_ne_qmtop_irq_info = {
	.module_name = "QM_TOP",
	.intr_status = (QM_ERR_BUF_LIST_MEM_STATUS |
			QM_ERR_CORRECT_BUF_LIST_MEM_STATUS |
			QM_ERR_CPU_BUF_LIST_MEM_STATUS |
			QM_ERR_CORRECT_CPU_BUF_LIST_MEM_STATUS |
			QM_ERR_PROFILE_MEM_STATUS |
			QM_ERR_CORRECT_PROFILE_MEM_STATUS |
			QM_ERR_STATUS_MEM_STATUS |
			QM_ERR_CORRECT_STATUS_MEM_STATUS |
			QM_ERR_STATUS_SDRAM_ADDR_MEM_STATUS |
			QM_ERR_CORRECT_STATUS_SDRAM_ADDR_MEM_STATUS |
			QM_ERR_INT_BUF_LIST_MEM_STATUS |
			QM_ERR_CORRECT_INT_BUF_LIST_MEM_STATUS |
			QM_FLUSH_COMPLETE_STATUS |
			QM_ERR_AXI_INTMEM_READ_STATUS |
			QM_ERR_AXI_INTMEM_WRITE_STATUS |
			QM_ERR_BUF_UNDERRUN_STATUS |
			QM_VOQ_DISABLE_STATUS |
			QM_ERR_PKT_ENQUEUE_STATUS |
			QM_ERR_CPU_BUF_UNDERRUN_STATUS |
			QM_CPU_VOQ_DISABLE_STATUS |
			QM_ERR_CPU_PKT_ENQUEUE_STATUS |
			QM_PKT_AGE_OLD_STATUS |
			QM_QUE_AGE_OLD_STATUS |
			QM_ERR_PKT_HEADER_STATUS |
			QM_ERR_AXI_QM_READ_STATUS |
			QM_ERR_AXI_QM_WRITE_STATUS |
			QM_ERR_AXI_QMCPU_READ_STATUS |
			QM_ERR_AXI_QMCPU_WRITE_STATUS),

	.intr_enable = (QM_ERR_BUF_LIST_MEM_ENABLE |
			QM_ERR_CORRECT_BUF_LIST_MEM_ENABLE |
			QM_ERR_CPU_BUF_LIST_MEM_ENABLE |
			QM_ERR_CORRECT_CPU_BUF_LIST_MEM_ENABLE |
			QM_ERR_PROFILE_MEM_ENABLE |
			QM_ERR_CORRECT_PROFILE_MEM_ENABLE |
			QM_ERR_STATUS_MEM_ENABLE |
			QM_ERR_CORRECT_STATUS_MEM_ENABLE |
			QM_ERR_STATUS_SDRAM_ADDR_MEM_ENABLE |
			QM_ERR_CORRECT_STATUS_SDRAM_ADDR_MEM_ENABLE |
			QM_ERR_INT_BUF_LIST_MEM_ENABLE |
			QM_ERR_CORRECT_INT_BUF_LIST_MEM_ENABLE |
			QM_FLUSH_COMPLETE_ENABLE |
			QM_ERR_AXI_INTMEM_READ_ENABLE |
			QM_ERR_AXI_INTMEM_WRITE_ENABLE |
			QM_ERR_BUF_UNDERRUN_ENABLE |
			QM_VOQ_DISABLE_ENABLE |
			QM_ERR_PKT_ENQUEUE_ENABLE |
			QM_ERR_CPU_BUF_UNDERRUN_ENABLE |
			QM_CPU_VOQ_DISABLE_ENABLE |
			QM_ERR_CPU_PKT_ENQUEUE_ENABLE |
			QM_PKT_AGE_OLD_ENABLE |
			QM_QUE_AGE_OLD_ENABLE |
			QM_ERR_PKT_HEADER_ENABLE |
			QM_ERR_AXI_QM_READ_ENABLE |
			QM_ERR_AXI_QM_WRITE_ENABLE |
			QM_ERR_AXI_QMCPU_READ_ENABLE |
			QM_ERR_AXI_QMCPU_WRITE_ENABLE),

	.intr_e_reg  = QM_INTENABLE_0,

	.intr_handle = cs_ne_qmtop_intr_handle,
};

/* Scheduler Interrupts */
const static struct cs_ne_irq_info cs_ne_schcnfg_irq_info = {
	.module_name = "SCH_CNFG",
	.intr_status = (SCH_CNFG_PARERR_VOQ_STATUS(0) |
			SCH_CNFG_PARERR_VOQ_STATUS(1) |
			SCH_CNFG_PARERR_VOQ_STATUS(2) |
			SCH_CNFG_PARERR_VOQ_STATUS(3) |
			SCH_CNFG_PARERR_VOQ_STATUS(4) |
			SCH_CNFG_PARERR_VOQ_STATUS(5) |
			SCH_CNFG_PARERR_VOQ_STATUS(6) |
			SCH_CNFG_PARERR_VOQ_STATUS(7) |
			SCH_CNFG_PARERR_VOQ_STATUS(8) |
			SCH_CNFG_PARERR_VOQ_STATUS(9) |
			SCH_CNFG_PARERR_VOQ_STATUS(10) |
			SCH_CNFG_PARERR_VOQ_STATUS(11) |
			SCH_CNFG_PARERR_VOQ_STATUS(12) |
			SCH_CNFG_PARERR_VOQ_STATUS(13) |
			SCH_CNFG_PARERR_VOQ_STATUS(14) |
			SCH_CNFG_PARERR_VOQ_STATUS(15) |
			SCH_CNFG_PARERR_VOQ_STATUS(16) |
			SCH_CNFG_PARERR_VOQ_STATUS(17) |
			SCH_CNFG_PARERR_VOQ_STATUS(18) |
			SCH_CNFG_PARERR_VOQ_STATUS(19) |
			SCH_CNFG_PARERR_VOQ_STATUS(20) |
			SCH_CNFG_PARERR_VOQ_STATUS(21) |
			SCH_CNFG_PARERR_VOQ_STATUS(22) |
			SCH_CNFG_PARERR_VOQ_STATUS(23) |
			SCH_CNFG_PARERR_VOQ_STATUS(24) |
			SCH_CNFG_PARERR_VOQ_STATUS(25) |
			SCH_CNFG_PARERR_VOQ_STATUS(26) |
			SCH_CNFG_PARERR_VOQ_STATUS(27) |
			SCH_CNFG_PARERR_VOQ_STATUS(28) |
			SCH_CNFG_PARERR_VOQ_STATUS(29) |
			SCH_CNFG_PARERR_VOQ_STATUS(30) |
			SCH_CNFG_PARERR_VOQ_STATUS(31)),

	.intr_enable = (SCH_CNFG_PARERR_VOQ_ENABLE(0) |
			SCH_CNFG_PARERR_VOQ_ENABLE(1) |
			SCH_CNFG_PARERR_VOQ_ENABLE(2) |
			SCH_CNFG_PARERR_VOQ_ENABLE(3) |
			SCH_CNFG_PARERR_VOQ_ENABLE(4) |
			SCH_CNFG_PARERR_VOQ_ENABLE(5) |
			SCH_CNFG_PARERR_VOQ_ENABLE(6) |
			SCH_CNFG_PARERR_VOQ_ENABLE(7) |
			SCH_CNFG_PARERR_VOQ_ENABLE(8) |
			SCH_CNFG_PARERR_VOQ_ENABLE(9) |
			SCH_CNFG_PARERR_VOQ_ENABLE(10) |
			SCH_CNFG_PARERR_VOQ_ENABLE(11) |
			SCH_CNFG_PARERR_VOQ_ENABLE(12) |
			SCH_CNFG_PARERR_VOQ_ENABLE(13) |
			SCH_CNFG_PARERR_VOQ_ENABLE(14) |
			SCH_CNFG_PARERR_VOQ_ENABLE(15) |
			SCH_CNFG_PARERR_VOQ_ENABLE(16) |
			SCH_CNFG_PARERR_VOQ_ENABLE(17) |
			SCH_CNFG_PARERR_VOQ_ENABLE(18) |
			SCH_CNFG_PARERR_VOQ_ENABLE(19) |
			SCH_CNFG_PARERR_VOQ_ENABLE(20) |
			SCH_CNFG_PARERR_VOQ_ENABLE(21) |
			SCH_CNFG_PARERR_VOQ_ENABLE(22) |
			SCH_CNFG_PARERR_VOQ_ENABLE(23) |
			SCH_CNFG_PARERR_VOQ_ENABLE(24) |
			SCH_CNFG_PARERR_VOQ_ENABLE(25) |
			SCH_CNFG_PARERR_VOQ_ENABLE(26) |
			SCH_CNFG_PARERR_VOQ_ENABLE(27) |
			SCH_CNFG_PARERR_VOQ_ENABLE(28) |
			SCH_CNFG_PARERR_VOQ_ENABLE(29) |
			SCH_CNFG_PARERR_VOQ_ENABLE(30) |
			SCH_CNFG_PARERR_VOQ_ENABLE(31)),

	// need some trick here, four shaper parity err interrupts should use
	// different interrupt enable registers.
	.intr_e_reg  = SCH_SHAPER_PARITY_ERR_INTENABLE0_0,

	.intr_handle = &cs_ne_schcnfg_intr_handle,

};

const static struct cs_ne_irq_info cs_ne_schtop_irq_info = {
	.module_name = "SCH_TOP",
	.intr_status = (SCHTOP_CPU_CMD_EXEC_STATUS |
			SCHTOP_SHP_PAR_ERR_0_INT_STATUS |
			SCHTOP_SHP_PAR_ERR_1_INT_STATUS |
			SCHTOP_SHP_PAR_ERR_2_INT_STATUS |
			SCHTOP_SHP_PAR_ERR_3_INT_STATUS |
			SCHTOP_EXPRESS_MODE_ON_STATUS |
			SCHTOP_EXPRESS_MODE_OFF_STATUS |
			SCHTOP_SCH_EMYVOQ_REQGVN_STATUS),

	.intr_enable = (SCHTOP_CPU_CMD_EXEC_ENABLE |
			SCHTOP_SHP_PAR_ERR_0_INT_ENABLE |
			SCHTOP_SHP_PAR_ERR_1_INT_ENABLE |
			SCHTOP_SHP_PAR_ERR_2_INT_ENABLE |
			SCHTOP_SHP_PAR_ERR_3_INT_ENABLE |
			SCHTOP_EXPRESS_MODE_ON_ENABLE |
			SCHTOP_EXPRESS_MODE_OFF_ENABLE |
			SCHTOP_SCH_EMYVOQ_REQGVN_ENABLE),

	.intr_e_reg  = SCH_INTENABLE_0,

	.intr_handle = cs_ne_schtop_intr_handle,

	.sub_module[1] = &cs_ne_schcnfg_irq_info,
	.sub_module[2] = &cs_ne_schcnfg_irq_info,
	.sub_module[3] = &cs_ne_schcnfg_irq_info,
	.sub_module[4] = &cs_ne_schcnfg_irq_info,

	.sub_intr_reg[1] = SCH_SHAPER_PARITY_ERR_INTERRUPT0_0,
	.sub_intr_reg[2] = SCH_SHAPER_PARITY_ERR_INTERRUPT1_0,
	.sub_intr_reg[3] = SCH_SHAPER_PARITY_ERR_INTERRUPT2_0,
	.sub_intr_reg[4] = SCH_SHAPER_PARITY_ERR_INTERRUPT3_0,
};

/* NI Interrupts */
const static struct cs_ne_irq_info cs_ne_ni_xram_rx_irq_info = {
	.module_name = "NI_XRAM_RX",
	.intr_status = (NI_TOP_XRAM_RX_0_DONE |
			NI_TOP_XRAM_RX_1_DONE |
			NI_TOP_XRAM_RX_2_DONE |
			NI_TOP_XRAM_RX_3_DONE |
			NI_TOP_XRAM_RX_4_DONE |
			NI_TOP_XRAM_RX_5_DONE |
			NI_TOP_XRAM_RX_6_DONE |
			NI_TOP_XRAM_RX_7_DONE | NI_TOP_XRAM_RX_8_DONE),

	.intr_enable = (NI_TOP_XRAM_RX_0_DONE_ENABLE |
			NI_TOP_XRAM_RX_1_DONE_ENABLE |
			NI_TOP_XRAM_RX_2_DONE_ENABLE |
			NI_TOP_XRAM_RX_3_DONE_ENABLE |
			NI_TOP_XRAM_RX_4_DONE_ENABLE |
			NI_TOP_XRAM_RX_5_DONE_ENABLE |
			NI_TOP_XRAM_RX_6_DONE_ENABLE |
			NI_TOP_XRAM_RX_7_DONE_ENABLE |
			NI_TOP_XRAM_RX_8_DONE_ENABLE),
	// obsolete interrupts?
	// .intr_e_reg  = ,
	.intr_handle = cs_ne_ni_xram_rx_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_xram_tx_irq_info = {
	.module_name = "NI_XRAM_TX",
	.intr_status = (NI_TOP_XRAM_TX_0_DONE | NI_TOP_XRAM_TX_1_DONE),

	.intr_enable = (NI_TOP_XRAM_TX_0_DONE_ENABLE |
			NI_TOP_XRAM_TX_1_DONE_ENABLE),

	.intr_e_reg  = NI_TOP_NI_CPUXRAM_TXPKT_INTENABLE_0,

	.intr_handle = cs_ne_ni_xram_tx_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_port_info = {
	.module_name = "NI_PORT",
	.intr_status = (NI_PORT_LINK_STAT_CHANGE |
			NI_PORT_TXFIFO_UNDERRUN |
			NI_PORT_TXFIFO_OVERRUN |
			NI_PORT_RXCNTRL_OVERRUN |
			NI_PORT_RXCNTRL_USAGE_EXCEED |
			NI_PORT_RXMIB_CNTMSB_SET |
			NI_PORT_TXMIB_CNTMSB_SET |
			NI_PORT_TXEM_CRCERR_CNTMSB_SET |
			NI_PORT_RXCNTRL_RD_IDLE),

	.intr_enable = (NI_PORT_LINK_STAT_CHANGE_ENABLE |
			NI_PORT_TXFIFO_UNDERRUN_ENABLE |
			NI_PORT_TXFIFO_OVERRUN_ENABLE |
			NI_PORT_RXCNTRL_OVERRUN_ENABLE |
			NI_PORT_RXCNTRL_USAGE_EXCEED_ENABLE |
			NI_PORT_RXMIB_CNTMSB_SET_ENABLE |
			NI_PORT_TXMIB_CNTMSB_SET_ENABLE |
			NI_PORT_TXEM_CRCERR_CNTMSB_SET_ENABLE |
			NI_PORT_RXCNTRL_RD_IDLE_ENABLE),

	// trick for different ports?
	.intr_e_reg  = NI_TOP_NI_PORT_0_INTENABLE_0,

	.intr_handle = cs_ne_ni_port_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_rxfifo_info = {
	.module_name = "NI_RXFIFO",
	.intr_status = (NI_RXFIFO_FULL_STATUS |
			NI_RXFIFO_OVERRUN_STATUS |
			NI_RXFIFO_VOQ_FULL_STATUS |
			NI_RXFIFO_CONG_STATUS |
			NI_RXFIFO_NOEOP_STATUS |
			NI_RXFIFO_NOSOP_STATUS |
			NI_RXFIFO_NOEOP_AF_FL_STATUS |
			NI_RXFIFO_EOP_BF_FL_STATUS |
			NI_RXFIFO_LL_ECC_ERR_STATUS |
			NI_RXFIFO_LL_ECC_CORR_ERR_STATUS |
			NI_RXFIFO_MAL_DROP_PKT_CNT_MSB_SET_STATUS |
			NI_RXFIFO_MCAL_PKT_DROP_STATUS |
			NI_RXFIFO_VOQ_ECC_ERR_STATUS |
			NI_RXFIFO_VOQ_ECC_CORR_ERR_STATUS),

	.intr_enable = (NI_RXFIFO_FULL_ENABLE |
			NI_RXFIFO_OVERRUN_ENABLE |
			NI_RXFIFO_VOQ_FULL_ENABLE |
			NI_RXFIFO_CONG_ENABLE |
			NI_RXFIFO_NOEOP_ENABLE |
			NI_RXFIFO_NOSOP_ENABLE |
			NI_RXFIFO_NOEOP_AF_FL_ENABLE |
			NI_RXFIFO_EOP_BF_FL_ENABLE |
			NI_RXFIFO_LL_ECC_ERR_ENABLE |
			NI_RXFIFO_LL_ECC_CORR_ERR_ENABLE |
			NI_RXFIFO_MAL_DROP_PKT_CNT_MSB_SET_ENABLE |
			NI_RXFIFO_MCAL_PKT_DROP_ENABLE |
			NI_RXFIFO_VOQ_ECC_ERR_ENABLE |
			NI_RXFIFO_VOQ_ECC_CORR_ERR_ENABLE),

	.intr_e_reg  = NI_TOP_NI_RXFIFO_INTENABLE_0,

	.intr_handle = cs_ne_ni_rxfifo_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_txem_info = {
	.module_name = "NI_TXEM",
	.intr_status = (NI_TXEM_IFIFO_OVF_STATUS |
			NI_TXEM_CRC_ERR_STATUS |
			NI_TXEM_CNTOVF_STATUS |
			NI_TXEM_TXMIB_FIFO_OVF_STATUS |
			NI_TXEM_RXMIB_FIFO_OVF_STATUS |
			NI_TXEM_PTP_VOQCHG_ERR_STATUS |
			NI_TXEM_PTP_CACHEVOQ_ERR_STATUS |
			NI_TXEM_VOQ_LKUP_MEM_PERR_STATUS |
			NI_TXEM_MC_INDX_LKUP_MEM_PERR_STATUS |
			NI_TXEM_PTP_V6CSUM0_ERR_STATUS),

	.intr_enable = (NI_TXEM_IFIFO_OVF_ENABLE |
			NI_TXEM_CRC_ERR_ENABLE |
			NI_TXEM_CNTOVF_ENABLE |
			NI_TXEM_TXMIB_FIFO_OVF_ENABLE |
			NI_TXEM_RXMIB_FIFO_OVF_ENABLE |
			NI_TXEM_PTP_VOQCHG_ERR_ENABLE |
			NI_TXEM_PTP_CACHEVOQ_ERR_ENABLE |
			NI_TXEM_VOQ_LKUP_MEM_PERR_ENABLE |
			NI_TXEM_MC_INDX_LKUP_MEM_PERR_ENABLE |
			NI_TXEM_PTP_V6CSUM0_ERR_ENABLE),

	.intr_e_reg  = NI_TOP_NI_TXEM_INTENABLE_0,

	.intr_handle = cs_ne_ni_txem_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_pc_info = {
	.module_name = "NI_PC",
	.intr_status = (NI_PC0_DCHK_OUTOFSYNC_STATUS |
			NI_PC0_DST_ADDR_MISMATCH_STATUS |
			NI_PC0_SRC_ADDR_MISMATCH_STATUS |
			NI_PC0_VLAN1_FIELD_MISMATCH_STATUS |
			NI_PC0_SEQNUM_MISMATCH_STATUS |
			NI_PC0_DATA_MISMATCH_STATUS |
			NI_PC0_FRAME_LEN_MISMATCH_STATUS),

	.intr_enable = (NI_PC0_DCHK_OUTOFSYNC_ENABLE |
			NI_PC0_DST_ADDR_MISMATCH_ENABLE |
			NI_PC0_SRC_ADDR_MISMATCH_ENABLE |
			NI_PC0_VLAN1_FIELD_MISMATCH_ENABLE |
			NI_PC0_SEQNUM_MISMATCH_ENABLE |
			NI_PC0_DATA_MISMATCH_ENABLE |
			NI_PC0_FRAME_LEN_MISMATCH_ENABLE),

	.intr_e_reg  = NI_TOP_NI_PC0_INTENABLE_0,

	.intr_handle = cs_ne_ni_pc_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_xram_cnt_info = {
	.module_name = "NI_XRAM_CNT",
	.intr_status = (NI_CPUXRAM_PKT_DROP_ERR0_SET |
			NI_CPUXRAM_PKT_DROP_ERR1_SET |
			NI_CPUXRAM_PKT_DROP_ERR2_SET |
			NI_CPUXRAM_PKT_DROP_ERR3_SET |
			NI_CPUXRAM_PKT_DROP_ERR4_SET |
			NI_CPUXRAM_PKT_DROP_ERR5_SET |
			NI_CPUXRAM_PKT_DROP_ERR6_SET |
			NI_CPUXRAM_PKT_DROP_ERR7_SET |
			NI_CPUXRAM_PKT_DROP_ERR8_SET |
			NI_CPUXRAM_PKT_TO_XRAM0_SET |
			NI_CPUXRAM_PKT_TO_XRAM1_SET |
			NI_CPUXRAM_PKT_TO_XRAM2_SET |
			NI_CPUXRAM_PKT_TO_XRAM3_SET |
			NI_CPUXRAM_PKT_TO_XRAM4_SET |
			NI_CPUXRAM_PKT_TO_XRAM5_SET |
			NI_CPUXRAM_PKT_TO_XRAM6_SET |
			NI_CPUXRAM_PKT_TO_XRAM7_SET |
			NI_CPUXRAM_PKT_TO_XRAM8_SET |
			NI_CPUXRAM_PKT_DROP_OVERRUN_SET |
			NI_CPUXRAM_BYTE_TO_XRAM0_SET |
			NI_CPUXRAM_BYTE_TO_XRAM1_SET |
			NI_CPUXRAM_BYTE_TO_XRAM2_SET |
			NI_CPUXRAM_BYTE_TO_XRAM3_SET |
			NI_CPUXRAM_BYTE_TO_XRAM4_SET |
			NI_CPUXRAM_BYTE_TO_XRAM5_SET |
			NI_CPUXRAM_BYTE_TO_XRAM6_SET |
			NI_CPUXRAM_BYTE_TO_XRAM7_SET |
			NI_CPUXRAM_BYTE_TO_XRAM8_SET |
			NI_CPUXRAM_DMA_PKT_TO_CPU_SET |
			NI_CPUXRAM_DMA_BYTE_TO_CPU_SET |
			NI_CPUXRAM_PKT_DROP_OVERRUN_MGMT),

	.intr_enable = (NI_CPUXRAM_PKT_DROP_ERR0_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR1_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR2_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR3_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR4_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR5_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR6_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR7_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_ERR8_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM0_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM1_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM2_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM3_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM4_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM5_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM6_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM7_SET_ENABLE |
			NI_CPUXRAM_PKT_TO_XRAM8_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_OVERRUN_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM0_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM1_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM2_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM3_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM4_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM5_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM6_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM7_SET_ENABLE |
			NI_CPUXRAM_BYTE_TO_XRAM8_SET_ENABLE |
			NI_CPUXRAM_DMA_PKT_TO_CPU_SET_ENABLE |
			NI_CPUXRAM_DMA_BYTE_TO_CPU_SET_ENABLE |
			NI_CPUXRAM_PKT_DROP_OVERRUN_MGMT_ENABLE),

	.intr_e_reg  = NI_TOP_NI_CPUXRAM_CNTR_INTENABLE_0,

	.intr_handle = cs_ne_ni_xram_cntr_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_xram_err_info = {
	.module_name = "NI_XRAM_ERR",
	.intr_status = (NI_CPUXRAM_RX0_PTRBKUP |
			NI_CPUXRAM_RX1_PTRBKUP |
			NI_CPUXRAM_RX2_PTRBKUP |
			NI_CPUXRAM_RX3_PTRBKUP |
			NI_CPUXRAM_RX4_PTRBKUP |
			NI_CPUXRAM_RX5_PTRBKUP |
			NI_CPUXRAM_RX6_PTRBKUP |
			NI_CPUXRAM_RX7_PTRBKUP |
			NI_CPUXRAM_RX8_PTRBKUP |
			NI_CPUXRAM_RX0_DIS_PKT |
			NI_CPUXRAM_RX1_DIS_PKT |
			NI_CPUXRAM_RX2_DIS_PKT |
			NI_CPUXRAM_RX3_DIS_PKT |
			NI_CPUXRAM_RX4_DIS_PKT |
			NI_CPUXRAM_RX5_DIS_PKT |
			NI_CPUXRAM_RX6_DIS_PKT |
			NI_CPUXRAM_RX7_DIS_PKT |
			NI_CPUXRAM_RX8_DIS_PKT |
			NI_CPUXRAM_RX_QMFIFO_OVERRUN |
			NI_CPUXRAM_RX_PTPFIFO_OVERRUN |
			NI_CPUXRAM_RX_MGMTFIFO_OVERRUN |
			NI_CPUXRAM_DMA_FIFO_OVERRUN),

	.intr_enable = (NI_CPUXRAM_RX0_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX1_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX2_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX3_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX4_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX5_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX6_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX7_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX8_PTRBKUP_ENABLE |
			NI_CPUXRAM_RX0_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX1_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX2_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX3_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX4_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX5_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX6_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX7_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX8_DIS_PKT_ENABLE |
			NI_CPUXRAM_RX_QMFIFO_OVERRUN_ENABLE |
			NI_CPUXRAM_RX_PTPFIFO_OVERRUN_ENABLE |
			NI_CPUXRAM_RX_MGMTFIFO_OVERRUN_ENABLE |
			NI_CPUXRAM_DMA_FIFO_OVERRUN_ENABLE),

	.intr_e_reg  = NI_TOP_NI_CPUXRAM_ERR_INTENABLE_0,

	.intr_handle = cs_ne_ni_xram_err_intr_handle,
};

const static struct cs_ne_irq_info cs_ne_ni_irq_info = {
	.module_name = "NI_TOP",
	.intr_status = (NI_TOP_INT_STAT_PSPID_0 |
			NI_TOP_INT_STAT_PSPID_1 |
			NI_TOP_INT_STAT_PSPID_2 |
			NI_TOP_INT_STAT_PSPID_3 |
			NI_TOP_INT_STAT_PSPID_4 |
			NI_TOP_INT_STAT_PSPID_5 |
			NI_TOP_INT_STAT_PSPID_6 |
			NI_TOP_INT_STAT_PSPID_7 |
			NI_TOP_INT_STAT_RXFIFO |
			NI_TOP_INT_STAT_TXEM |
			NI_TOP_INT_STAT_PC0 |
			NI_TOP_INT_STAT_PC1 |
			NI_TOP_INT_STAT_PC2 |
			NI_TOP_CPUXRAM_STAT_CNTR | NI_TOP_CPUXRAM_STAT_ERR),

	.intr_enable = (NI_TOP_INT_STAT_PSPID_0_ENABLE |
			NI_TOP_INT_STAT_PSPID_1_ENABLE |
			NI_TOP_INT_STAT_PSPID_2_ENABLE |
			NI_TOP_INT_STAT_PSPID_3_ENABLE |
			NI_TOP_INT_STAT_PSPID_4_ENABLE |
			NI_TOP_INT_STAT_PSPID_5_ENABLE |
			NI_TOP_INT_STAT_PSPID_6_ENABLE |
			NI_TOP_INT_STAT_PSPID_7_ENABLE |
			NI_TOP_INT_STAT_RXFIFO_ENABLE |
			NI_TOP_INT_STAT_TXEM_ENABLE |
			NI_TOP_INT_STAT_PC0_ENABLE |
			NI_TOP_INT_STAT_PC1_ENABLE |
			NI_TOP_INT_STAT_PC2_ENABLE |
			NI_TOP_CPUXRAM_STAT_CNTR_ENABLE |
			NI_TOP_CPUXRAM_STAT_ERR_ENABLE),

	.intr_e_reg  = NI_TOP_NI_INTENABLE_0,

	.intr_handle = cs_ne_nitop_intr_handle,

	.sub_module[0] = &cs_ne_ni_port_info,
	.sub_module[1] = &cs_ne_ni_port_info,
	.sub_module[2] = &cs_ne_ni_port_info,
	.sub_module[3] = &cs_ne_ni_port_info,
	.sub_module[4] = &cs_ne_ni_port_info,
	.sub_module[5] = &cs_ne_ni_port_info,
	.sub_module[6] = &cs_ne_ni_port_info,
	.sub_module[7] = &cs_ne_ni_port_info,
	.sub_module[8] = &cs_ne_ni_rxfifo_info,
	.sub_module[9] = &cs_ne_ni_txem_info,
	.sub_module[10] = &cs_ne_ni_pc_info,
	.sub_module[11] = &cs_ne_ni_pc_info,
	.sub_module[12] = &cs_ne_ni_pc_info,
	.sub_module[13] = &cs_ne_ni_xram_cnt_info,
	.sub_module[14] = &cs_ne_ni_xram_err_info,

	.sub_intr_reg[0] = NI_TOP_NI_PORT_0_INTERRUPT_0,
	.sub_intr_reg[1] = NI_TOP_NI_PORT_0_INTERRUPT_0 + 8,
	.sub_intr_reg[2] = NI_TOP_NI_PORT_0_INTERRUPT_0 + 16,
	.sub_intr_reg[3] = NI_TOP_NI_PORT_0_INTERRUPT_0 + 24,
	.sub_intr_reg[4] = NI_TOP_NI_PORT_0_INTERRUPT_0 + 32,
	.sub_intr_reg[5] = NI_TOP_NI_PORT_0_INTERRUPT_0 + 40,
	.sub_intr_reg[6] = NI_TOP_NI_PORT_0_INTERRUPT_0 + 48,
	.sub_intr_reg[7] = NI_TOP_NI_PORT_0_INTERRUPT_0 + 56,
	.sub_intr_reg[8] = NI_TOP_NI_RXFIFO_INTERRUPT_0,
	.sub_intr_reg[9] = NI_TOP_NI_TXEM_INTERRUPT_0,
	.sub_intr_reg[10] = NI_TOP_NI_PC0_INTERRUPT_0,
	.sub_intr_reg[11] = NI_TOP_NI_PC0_INTERRUPT_0 + 8,
	.sub_intr_reg[12] = NI_TOP_NI_PC0_INTERRUPT_0 + 16,
	.sub_intr_reg[13] = NI_TOP_NI_CPUXRAM_CNTR_INTERRUPT_0,
	.sub_intr_reg[14] = NI_TOP_NI_CPUXRAM_ERR_INTERRUPT_0,
};

struct cs_ne_irq_info cs_ne_global_irq_info = {
	.module_name = "NE_GLOBAL",
	.intr_status = (
#ifndef CONFIG_INTR_COALESCING
			       GLOBAL_NE_INT_NI_XRAM_RX_STATUS |
#endif
			       GLOBAL_NE_INT_NI_XRAM_TX_STATUS |
			       GLOBAL_NE_INT_NI_STATUS |
			       GLOBAL_NE_INT_FE_STATUS |
			       GLOBAL_NE_INT_TM_STATUS |
			       GLOBAL_NE_INT_QM_STATUS |
			       GLOBAL_NE_INT_SCH_STATUS),

	.intr_enable = (
#ifndef CONFIG_INTR_COALESCING
			       GLOBAL_NE_INT_NI_XRAM_RX_ENABLE |
#endif
			       GLOBAL_NE_INT_NI_XRAM_TX_ENABLE |
			       GLOBAL_NE_INT_NI_ENABLE |
			       GLOBAL_NE_INT_FE_ENABLE |
			       GLOBAL_NE_INT_TM_ENABLE |
			       GLOBAL_NE_INT_QM_ENABLE |
			       GLOBAL_NE_INT_SCH_ENABLE),

	.intr_e_reg  = GLOBAL_NETWORK_ENGINE_INTENABLE_0,

	.intr_handle = cs_ne_global_intr_handle,

#ifndef CONFIG_INTR_COALESCING
	.sub_module[0] = &cs_ne_ni_xram_rx_irq_info,
#endif
	.sub_module[1] = &cs_ne_ni_xram_tx_irq_info,
	.sub_module[2] = &cs_ne_ni_irq_info,
	.sub_module[3] = &cs_ne_fetop_irq_info,
	.sub_module[4] = &cs_ne_tmtop_irq_info,
	.sub_module[5] = &cs_ne_qmtop_irq_info,
	.sub_module[6] = &cs_ne_schtop_irq_info,
#ifndef CONFIG_INTR_COALESCING
	.sub_intr_reg[0] = NI_TOP_NI_CPUXRAM_RXPKT_INTERRUPT_0,
#endif
	.sub_intr_reg[1] = NI_TOP_NI_CPUXRAM_TXPKT_INTERRUPT_0,
	.sub_intr_reg[2] = NI_TOP_NI_INTERRUPT_0,
	.sub_intr_reg[3] = FETOP_FE_INTERRUPT_0,
	.sub_intr_reg[4] = TM_TC_INTERRUPT_0,
	.sub_intr_reg[5] = QM_INTERRUPT_0,
	.sub_intr_reg[6] = SCH_INTERRUPT_0,
};

#if 0
int irqreturn_t ne_interrupt(int irq, void *dev_instance)
{

	return 0;
};
#endif

/*
 * Traverse generic interrupt hierary tree, set enable register values
 */
int cs_ne_irq_init(const struct cs_ne_irq_info *irq_module)
{
	int i = 0;
	writel(irq_module->intr_enable, irq_module->intr_e_reg);
	while (i<32) {
		if (irq_module->sub_module[i])
			cs_ne_irq_init(irq_module->sub_module[i]);
		i++;
	}
	return 0;
};

int cs_ne_fedbg_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
};

int cs_ne_fehash_intr_handle(u32 dev_id,
			     const struct cs_ne_irq_info *irq_module,
			     u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
};

int cs_ne_felpm_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
};

int cs_ne_fepe_intr_handle(u32 dev_id,
			   const struct cs_ne_irq_info *irq_module,
			   u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
};

int cs_ne_fefwd_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
};

int cs_ne_fetop_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
};

int cs_ne_tmpol_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_tmbm_intr_handle(u32 dev_id,
			   const struct cs_ne_irq_info *irq_module,
			   u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_tmpm_intr_handle(u32 dev_id,
			   const struct cs_ne_irq_info *irq_module,
			   u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_tmtop_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_qmtop_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_schcnfg_intr_handle(u32 dev_id,
			      const struct cs_ne_irq_info *irq_module,
			      u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_schtop_intr_handle(u32 dev_id,
			     const struct cs_ne_irq_info *irq_module,
			     u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_ni_xram_rx_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	//u32 flag = 1;
	//int i=0;
	//u32 status;
	DBG(printk(KERN_INFO "%s::status %x\n", __func__, intr_status));
#if 0
	while (i <= 8) {
		if ((intr_status & flag) && irq_module->sub_module[i]) {

		};
		i++;
		flag <<= 1;
	}
#endif
	return 0;
}

int cs_ne_ni_xram_tx_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_ni_port_handle(u32 dev_id,
			 const struct cs_ne_irq_info *irq_module,
			 u32 intr_status)
{
	u32 err_status;
	
	//DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
	//	   __func__, intr_status));
	err_status = (intr_status & NI_PORT_TXFIFO_UNDERRUN ||
			intr_status & NI_PORT_TXFIFO_OVERRUN ||	 
			intr_status & NI_PORT_RXCNTRL_OVERRUN ||	 
			intr_status & NI_PORT_RXCNTRL_USAGE_EXCEED ||	 
			intr_status & NI_PORT_TXEM_CRCERR_CNTMSB_SET);  
	if (err_status) {
		DBG(printk("************* NI_PORT ***************\n"));
		DBG(printk("%s:: Need HW support, intr_status 0x%08x\n", 
			__func__, intr_status));
	}	   	   
	return 0;
}

int cs_ne_ni_rxfifo_intr_handle(u32 dev_id,
				const struct cs_ne_irq_info *irq_module,
				u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	
	if (intr_status) {
		DBG(printk("************** RXFIFO **************\n"));
		DBG(printk("%s:: Need HW support, intr_status 0x%08x\n", 
			__func__, intr_status));
	}	   
	return 0;
}

int cs_ne_ni_txem_intr_handle(u32 dev_id,
			      const struct cs_ne_irq_info *irq_module,
			      u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_ni_pc_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_ni_xram_cntr_intr_handle(u32 dev_id,
				   const struct cs_ne_irq_info *irq_module,
				   u32 intr_status)
{
	//DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
	//	   __func__, intr_status));
	return 0;
}

int cs_ne_ni_xram_err_intr_handle(u32 dev_id,
				  const struct cs_ne_irq_info *irq_module,
				  u32 intr_status)
{
	DBG(printk(KERN_INFO "%s::intr status 0x%08x\n",
		   __func__, intr_status));
	return 0;
}

int cs_ne_nitop_intr_handle(u32 dev_id,
			    const struct cs_ne_irq_info *irq_module,
			    u32 intr_status)
{
	//DBG(printk(KERN_INFO "%s::intr status 0x%08x, irq module %s\n",
	//	   __func__, intr_status, irq_module->module_name));
	return 0;
}

/*
 * This becomes generic IRQ tree traversal handler.
 * RX service function is separate, since they come from different interrupt
 * lines now
 */
int cs_ne_global_intr_handle(u32 dev_id,
			     const struct cs_ne_irq_info *irq_module,
			     u32 intr_status)
{
	u32 flag = 1;
	int i = 0;
	u32 status;
	if (irq_module == NULL) {
		printk(KERN_INFO "%s::irq module is empty!\n", __func__);
		return 0;
	}
	/* We may have different scheme of handling interrupts per module priority, etc. */
	while (i < 32) {
		if ((intr_status & flag & irq_module->intr_enable)
		    && irq_module->sub_module[i]) {
			status = readl(irq_module->sub_intr_reg[i]);
			writel(status, irq_module->sub_intr_reg[i]);
//                      DBG(printk(KERN_INFO "%s::sub module %x, irq %x, status %x, intr_status %x, sub mod name %s\n",
//                              __func__, i, irq_module->sub_intr_reg[i], status, intr_status,
//                              irq_module->sub_module[i]->module_name));
			if (status) {
				//irq_module->intr_handle(dev_id, irq_module->sub_module[i], status);
				irq_module->sub_module[i]->intr_handle(dev_id,
								       irq_module->
								       sub_module
								       [i],
								       status);
				cs_ne_global_intr_handle(dev_id,
							 irq_module->
							 sub_module[i], status);
			}
		}
		i++;
		flag <<= 1;
	}
	return 0;
};
