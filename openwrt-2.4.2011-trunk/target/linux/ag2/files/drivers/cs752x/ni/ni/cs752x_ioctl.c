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

#include <linux/netdevice.h>
#include <mach/platform.h>
#include "cs752x_ioctl.h"
#include "cs752x_eth.h"

int cs_ne_ioctl(struct net_device *dev, struct ifreq *rq, int cmd)
{
	int i, err = 0;
	NECMD_HDR_T ne_hdr;
	NEFE_CMD_HDR_T fe_cmd_hdr;
	NE_REQ_E ctrl;
	u8 *req_datap;
	u8 tbl_entry[FE_MAX_ENTRY_SIZE];
	u32 phy_data_s = -1,location_r = -1,length_r = -1,location_w = -1,data_w = -1;
	u16 phy_addr = -1,phy_reg = -1,phy_len = -1, phy_addr_s = 0, phy_reg_s = 0;
	u32 size_r,size_w;

	if (copy_from_user((void *)&ne_hdr, rq->ifr_data, sizeof(ne_hdr)))
		return -EFAULT;

	req_datap = (u8 *)rq->ifr_data + sizeof(ne_hdr);

	switch (ne_hdr.cmd) {
	case REGREAD:	
		if (ne_hdr.len != sizeof(REGREAD_T))
			return -EPERM;
		if (copy_from_user((void *)&ctrl.reg_read, req_datap, sizeof(ctrl.reg_read)))
			return -EFAULT;
		location_r = ctrl.reg_read.location;
		length_r = ctrl.reg_read.length;
		size_r = ctrl.reg_read.size;
		
		//if (size_r == 1 && ((MIN_READ <= location_r) && (location_r <= MAX_READ)))
		if (size_r == 1)
			size_r = 4;
		if (size_r == 1)
			ni_dm_byte(location_r, length_r);
		if (size_r == 2)
			ni_dm_short(location_r, length_r);
		if (size_r == 4)
			ni_dm_long(location_r, length_r);
		break;	
	case REGWRITE:	
		if (ne_hdr.len != sizeof(REGWRITE_T))
			return -EPERM;
		if (copy_from_user((void *)&ctrl.reg_write, req_datap, sizeof(ctrl.reg_write)))
			return -EFAULT;
		location_w = ctrl.reg_write.location;
		data_w = ctrl.reg_write.data;
		size_w = ctrl.reg_write.size;
		if (size_w == 1) {
			if (data_w > 0xff)
				err = 1;
			} else {	
				writeb(data_w,location_w);
				printk("Write Data 0x%X to Location 0x%X\n",(u32)data_w, location_w);
			}
		if (size_w == 2) {
			if (data_w > 0xffff)
				err = 1;
			} else {
				writew(data_w, location_w);
				printk("Write Data 0x%X to Location 0x%X\n",(u32)data_w, location_w);
			}
		if (size_w == 4) {
			if (data_w > 0xffffffff)
				err = 1;
			} else {	
				writel(data_w, location_w);
				printk("Write Data 0x%X to Location 0x%X\n",(u32)data_w, location_w);
			}
		if (err == 1) {
			printk("Syntax:	ne write mem [-b <location>] [-d <data>] [-1|2|4]\n");
			printk("Options:\n");
			printk("\t-b  Register Address\n");
			printk("\t-d  Data Vaule\n");
			if (size_w == 1)
				printk("\t-1  Data 0x%X < 0xFF\n",data_w);
			if (size_w == 2)	
				printk("\t-2  Data 0x%X < 0xFFFF\n",data_w);
			if (size_w == 4)	
				printk("\t-4  Data 0x%X < 0xFFFFFFFF\n",data_w);
		}
		break;
	case GMIIREG:
		if (ne_hdr.len != sizeof(GMIIREG_T))
			return -EPERM;
		if (copy_from_user((void *)&ctrl.get_mii_reg, req_datap, sizeof(ctrl.get_mii_reg)))
			return -EFAULT; /* Invalid argument */
		phy_addr = ctrl.get_mii_reg.phy_addr;
		phy_reg = ctrl.get_mii_reg.phy_reg;
		phy_len = ctrl.get_mii_reg.phy_len;
		if ((phy_addr == 1) || (phy_addr == 2))	{
			for (i = 0; i < phy_len ; i++)	{
				unsigned int data;
				data = ni_mdio_read((int)phy_addr, (int)phy_reg); 
				printk("MII Phy %d Reg %d Data = 0x%x\n", phy_addr, phy_reg++, data);
			}
		} else { 
			err = 1;
		}
		
		if (err == 1) {
			printk("Syntax error!\n");
			printk("Syntax: MII read [-a phy addr] [-r phy reg] [-l length]\n");
			printk("Options:\n");
			printk("\t-a  Phy address\n");
			printk("\t-r  Phy registers\n");
			printk("\t-l  Display total registers\n");
			printk("MII Phy address -a %d error !! Phy address must be 1 or 2.\n", phy_addr);
		}
		break;
	case SMIIREG:
		if (ne_hdr.len != sizeof(SMIIREG_T))
			return -EPERM;
		if (copy_from_user((void *)&ctrl.set_mii_reg, req_datap, sizeof(ctrl.set_mii_reg)))
			return -EFAULT;
		
		phy_addr_s = ctrl.set_mii_reg.phy_addr;
		phy_reg_s = ctrl.set_mii_reg.phy_reg;
		phy_data_s = ctrl.set_mii_reg.phy_data;
		if ((phy_addr_s == 1) || (phy_addr_s == 2))	{
				ni_mdio_write((int)phy_addr_s, (int)phy_reg_s, (int)phy_data_s); 
				printk("Write MII Phy %d Reg %d Data = 0x%x\n", phy_addr_s, phy_reg_s, phy_data_s);
		} else { 
			err = 1;
		}
		if (err == 1) {
			printk("Syntax error!\n");
			printk("Syntax: MII write [-a phy addr] [-r phy reg] [-d data]\n");
			printk("Options:\n");
			printk("\t-a  Phy address\n");
			printk("\t-r  Phy registers\n");
			printk("\t-d  date\n");
			printk("MII Phy address -a %d error !! Phy address must be 1 or 2.\n", phy_addr);
		}
		break;		
	case NE_NI_IOCTL:
		printk("NE_NI_IOCTL\n");
		break;
	case NE_FE_IOCTL:
		if (copy_from_user((void *)&fe_cmd_hdr, req_datap, sizeof(NEFE_CMD_HDR_T)))
			return -EFAULT;
		ni_set_ne_enabled((fe_cmd_hdr.Bypass==1)?0:1);
		req_datap += sizeof(NEFE_CMD_HDR_T);
		switch(fe_cmd_hdr.table_id){
			case CS_IOTCL_TBL_CLASSIFIER:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_class_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_class(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_SDB:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_sdb_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_sdb(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_HASH_MASK:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_hash_mask_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_hashmask(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
#if 0	/* FIXME!! don't have it now */
			case CS_IOCTL_TBL_LPM:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_lpm_entry_t)))
					return -EFAULT;
				cs_ioctl_lpm(dev, &tbl_entry, fe_cmd_hdr.cmd);
				break;
#endif
			case CS_IOCTL_TBL_HASH_MATCH:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_hash_hash_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_hashhash(dev, (void*)&tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_FWDRSLT:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_fwd_result_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_fwdrslt(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_QOSRSLT:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_qos_result_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_qosrslt(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;			
			case CS_IOCTL_TBL_L2_MAC:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_l2_addr_pair_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_l2mac(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_L3_IP:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_l3_addr_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_l3ip(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_VOQ_POLICER:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_voq_pol_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_voqpol(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_LPB:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_lpb_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_lpb(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_AN_BNG_MAC:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_an_bng_mac_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_an_bng_mac(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_PORT_RANGE:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_port_range_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_l4portrngs(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_VLAN:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_vlan_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_vlan(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
#if 0
			case CS_IOCTL_TBL_ACL_RULE:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_acl_rule_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_aclrule(dev, &tbl_entry, fe_cmd_hdr.cmd);
				break;
#endif
			case CS_IOCTL_TBL_ACL_ACTION:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_acl_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_acl(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_PE_VOQ_DROP:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_pe_voq_drp_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_voqdrp(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_ETYPE:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_eth_type_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_etype(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_LLC_HDR:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_llc_hdr_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_llchdr(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			case CS_IOCTL_TBL_FVLAN:
				if (copy_from_user((void *)&tbl_entry, req_datap, 
							sizeof(fe_flow_vlan_entry_t)))
					return -EFAULT;
				cs_fe_ioctl_fvlan(dev, &tbl_entry, (void*)&fe_cmd_hdr);
				break;
			/* Some other tables that dont have ioctl definted in user: 
			 * hashcheck and hashoverflow. */
			default :
				break;
		}
		printk("NE_NI_IOCTL\n");
		break;
	case NE_QM_IOCTL:
		printk("NE_QM_IOCTL\n");
		break;
	case NE_SCH_IOCTL:
		printk("NE_SCH_IOCTL\n");
		break;
	default:
		return -EPERM;
	}

	return 0;
}
