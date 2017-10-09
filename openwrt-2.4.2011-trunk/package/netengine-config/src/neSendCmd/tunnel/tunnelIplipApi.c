/******************************************************************************
	 Copyright (c) 2012, Cortina Systems, Inc.  All rights reserved.

 ******************************************************************************
   Module      : tunnelIplipApi.c
   Date        : 2012-08-09
   Description : Cortina GoldenGate NetEngine configuration utility process
	             TUNNEL IPLIP API Table
   Author      : Wen Hsu <whsu@cortina-systems.com>
   Remarks     :

 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <ne_defs.h>
#include "../neSendCmd.h"
#include "../misc.h"


TUNNEL_IPLIP_SUB_CMD_T tunnel_iplip_subcmd_def[] = {
	{"cs_pppoe_port_add",		CS_IPLIP_PPPOE_PORT_ADD},
	{"cs_pppoe_port_delete",	CS_IPLIP_PPPOE_PORT_DELETE},
	{"cs_pppoe_port_encap_set",     CS_IPLIP_PPPOE_PORT_ENCAP_SET},
	{"cs_pppoe_port_encap_get",     CS_IPLIP_PPPOE_PORT_ENCAP_GET},
	{"cs_pppoe_port_config_set",	CS_IPLIP_PPPOE_PORT_SET},
	{"cs_pppoe_port_config_get",    CS_IPLIP_PPPOE_PORT_GET},
        {"cs_tunnel_add",		CS_IPLIP_TUNNEL_ADD},
        {"cs_tunnel_delete",		CS_IPLIP_TUNNEL_DELETE},
	{"cs_tunnel_delete_by_idx",	CS_IPLIP_TUNNEL_DELETE_BY_IDX},
        {"cs_tunnel_get",		CS_IPLIP_TUNNEL_GET},
	{"cs_l2tp_session_add",         CS_IPLIP_L2TP_SESSION_ADD},
        {"cs_l2tp_session_delete",      CS_IPLIP_L2TP_SESSION_DELETE},
        {"cs_l2tp_session_get",		CS_IPLIP_L2TP_SESSION_GET},
	{"cs_ipv6_over_l2tp_add",       CS_IPLIP_IPV6_OVER_L2TP_ADD},
        {"cs_ipv6_over_l2tp_delete",    CS_IPLIP_IPV6_OVER_L2TP_DELETE},
        {"cs_ipv6_over_l2tp_getnext",   CS_IPLIP_IPV6_OVER_L2TP_GETNEXT},
	{NULL,		0}
};

int tunnel_iplip_api_subcmd(char *pString,
		TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
	int i;
	printf("tunnel_iplip_api_subcmd");
	tunnel_iplip_api_tbl_info->api.sub_cmd = CS_IPLIP_MAX;
	for (i = 0; tunnel_iplip_subcmd_def[i].name != NULL; i++) {
		if (strcmp(pString, tunnel_iplip_subcmd_def[i].name) == 0) {
			tunnel_iplip_api_tbl_info->api.sub_cmd =
			    tunnel_iplip_subcmd_def[i].id;
			break;
		}
	}
	if (tunnel_iplip_api_tbl_info->api.sub_cmd >= CS_IPLIP_MAX)
		return STATUS_ERR_PARA_GETVALUE;
	return STATUS_SUCCESS;
} 

/* cs_pppoe_port_add (cs_dev_id_t device_id, cs_port_id_t port_id, cs_port_id_t pppoe_port_id) */
/* cs_pppoe_port_delete (cs_dev_id_t device_id, cs_port_id_t pppoe_port_id) */
int tunnel_iplip_api_pppoe_port_add_delete_param_device_id(char *pString,
		TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
	int retStatus;
	if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
		return retStatus;

	tunnel_iplip_api_tbl_info->api.pppoe_port_add_delete_param.device_id =
		(unsigned short)strtoul(pString, NULL, 0);
	return STATUS_SUCCESS;
} 

int tunnel_iplip_api_pppoe_port_add_delete_param_port_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.pppoe_port_add_delete_param.port_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_pppoe_port_add_delete_param_pppoe_port_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.pppoe_port_add_delete_param.pppoe_port_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}


/*cs_pppoe_port_encap_set (cs_dev_id_t device_id, cs_port_id_t pppoe_port_id, cs_pppoe_port_cfg_t *p_cfg)*/
/*cs_pppoe_port_encap_get (cs_dev_id_t device_id, cs_port_id_t pppoe_port_id, cs_pppoe_port_cfg_t *p_cfg)*/
int tunnel_iplip_api_pppoe_port_encap_param_device_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.pppoe_port_encap_param.device_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_pppoe_port_encap_param_pppoe_port_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.pppoe_port_encap_param.pppoe_port_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_pppoe_port_encap_param_src_mac(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        unsigned int tmpMac[6];
        int i, retStatus = STATUS_FAILURE;
        retStatus = ParseMAC(pString, tmpMac);
        if (retStatus == STATUS_SUCCESS) {
                for (i = 0; i < 6; i++) {
                        tunnel_iplip_api_tbl_info->api.pppoe_port_encap_param.pppoe_port_cfg.src_mac[i] =
                            (cs_uint8) tmpMac[i];
                }
        }
        return retStatus;
}

int tunnel_iplip_api_pppoe_port_encap_param_dest_mac(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        unsigned int tmpMac[6];
        int i, retStatus = STATUS_FAILURE;
        retStatus = ParseMAC(pString, tmpMac);
        if (retStatus == STATUS_SUCCESS) {
                for (i = 0; i < 6; i++) {
                        tunnel_iplip_api_tbl_info->api.pppoe_port_encap_param.pppoe_port_cfg.dest_mac[i]=
                            (cs_uint8) tmpMac[i];
                }
        }
        return retStatus;
}

int tunnel_iplip_api_pppoe_port_encap_param_pppoe_session_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.pppoe_port_encap_param.pppoe_port_cfg.pppoe_session_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_pppoe_port_encap_param_tx_phy_port_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.pppoe_port_encap_param.pppoe_port_cfg.tx_phy_port =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}


//cs_l2tp_session_add(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, uint16 session_id)
//cs_l2tp_session_delete(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, uint16 session_id)
//cs_l2tp_session_get (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, uint16 session_id, u8 *is_present)
int tunnel_iplip_api_l2tp_session_param_device_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.l2tp_session_param.device_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_l2tp_session_param_tunnel_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.l2tp_session_param.tunnel_id =
                (unsigned int)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_l2tp_session_param_session_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.l2tp_session_param.session_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

//cs_ipv6_over_l2tp_add(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_session_id_t session_id, cs_ip_address *ipv6_prefix)
//cs_ipv6_over_l2tp_delete (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_session_id_t session_id, cs_ip_address *ipv6_prefix)
//cs_ipv6_over_l2tp_getnext (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_session_id_t session_id, cs_ip_address *ipv6_prefix)
int tunnel_iplip_api_ipv6_over_l2tp_param_device_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.ipv6_over_l2tp_param.device_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_ipv6_over_l2tp_param_tunnel_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.ipv6_over_l2tp_param.tunnel_id =
                (unsigned int)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_ipv6_over_l2tp_param_session_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.ipv6_over_l2tp_param.session_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_ipv6_over_l2tp_param_ipv6_prefix(char *pString, TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        unsigned int ipVal[4];

        int i;

        if (pString == NULL) {

                return STATUS_FAILURE;
        }

        ParseIPv6(pString, ipVal);

        for (i = 0; i < 4; i++) {

                tunnel_iplip_api_tbl_info->api.ipv6_over_l2tp_param.ipv6_prefix.ipv6_addr[3-i] =  ipVal[i];

        }

	tunnel_iplip_api_tbl_info->api.ipv6_over_l2tp_param.ipv6_prefix.afi = 1; /* CS_IPV6 */

        return STATUS_SUCCESS;
}

int tunnel_iplip_api_ipv6_over_l2tp_param_ipv6_prefix_addr_len(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.ipv6_over_l2tp_param.ipv6_prefix.addr_len =
                (unsigned char)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

        /* cs_tunnel_add(cs_dev_id_t device_id, cs_tunnel_cfg_t *p_tunnel_cfg, cs_tunnel_id_t *p_tunnel_id)
           cs_tunnel_delete(cs_dev_id_t device_id, cs_tunnel_cfg_t *p_tunnel_cfg)
           cs_tunnel_delete_by_idx (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id)
           cs_tunnel_get(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_tunnel_cfg_t *p_tunnel_cfg) */
int tunnel_iplip_api_tunnel_param_device_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.device_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_tunnel_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_id =
                (unsigned int)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_dest_ipv4_addr(char *pString, TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        unsigned char ipVal[4];

	unsigned int ipv4_addr;

        if (pString == NULL) {

                return STATUS_FAILURE;
        }

        ParseIP(pString, ipVal);

	ipv4_addr = ntohl(*(unsigned int*)ipVal); /* host order  */
	
	tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.dest_addr.ipv4_addr = ipv4_addr;

	tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.dest_addr.afi = 0; /* CS_IPV4 */

        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_src_ipv4_addr(char *pString, TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        unsigned char ipVal[4];

	unsigned int ipv4_addr;

        if (pString == NULL) {
                return STATUS_FAILURE;
        }

        ParseIP(pString, ipVal);

	ipv4_addr = ntohl(*(unsigned int*)ipVal); /* host order  */
	
	tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.src_addr.ipv4_addr = ipv4_addr;

	tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.src_addr.afi = 0; /* CS_IPV4 */

        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_dest_ipv6_addr(char *pString, TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        unsigned int ipVal[4];

        int i;

        if (pString == NULL) {
                return STATUS_FAILURE;
        }

        ParseIPv6(pString, ipVal);

        for (i = 0; i < 4; i++) {

                tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.dest_addr.ipv6_addr[3-i] = ipVal[i];

        }

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.dest_addr.afi = 1; /* CS_IPV6 */

        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_src_ipv6_addr(char *pString, TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        unsigned int ipVal[4];

        int i;

        if (pString == NULL) {

                return STATUS_FAILURE;
        }

        ParseIPv6(pString, ipVal);

        for (i = 0; i < 4; i++) {

                tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.src_addr.ipv6_addr[3-i] = ipVal[i];

        }

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.src_addr.afi = 1; /* CS_IPV6 */

        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_dest_addr_len(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.dest_addr.addr_len =
                (unsigned char)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_src_addr_len(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.src_addr.addr_len =
                (unsigned char)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_tunnel_type(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.type =
                (unsigned char)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_tx_port(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.tx_port =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_l2tp_ver(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.l2tp.ver =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_l2tp_len(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.l2tp.len =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_l2tp_tid(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.l2tp.tid =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}


int tunnel_iplip_api_tunnel_param_l2tp_ipv4_id(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.l2tp.ipv4_id =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_l2tp_src_port(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.l2tp.src_port =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

int tunnel_iplip_api_tunnel_param_l2tp_dest_port(char *pString,
                TUNNEL_IPLIP_API_TABLE_INFO *tunnel_iplip_api_tbl_info)
{
        int retStatus;
        if ((retStatus = CheckParas(pString)) != STATUS_SUCCESS)
                return retStatus;

        tunnel_iplip_api_tbl_info->api.tunnel_param.tunnel_cfg.l2tp.dest_port =
                (unsigned short)strtoul(pString, NULL, 0);
        return STATUS_SUCCESS;
}

NE_FIELD_T tunnel_iplip_api[] = {
	{"api",				tunnel_iplip_api_subcmd},

	/* cs_pppoe_port_add (cs_dev_id_t device_id, cs_port_id_t port_id, cs_port_id_t pppoe_port_id) */
        /* cs_pppoe_port_delete (cs_dev_id_t device_id, cs_port_id_t pppoe_port_id) */
	{"pppoe_port_add_delete_param.device_id",		tunnel_iplip_api_pppoe_port_add_delete_param_device_id},
	{"pppoe_port_add_delete_param.port_id",			tunnel_iplip_api_pppoe_port_add_delete_param_port_id},
	{"pppoe_port_add_delete_param.pppoe_port_id",		tunnel_iplip_api_pppoe_port_add_delete_param_pppoe_port_id},
	
	/* cs_pppoe_port_encap_set (cs_dev_id_t device_id, cs_port_id_t pppoe_port_id, cs_pppoe_port_cfg_t *p_cfg)
           cs_pppoe_port_encap_get (cs_dev_id_t device_id, cs_port_id_t pppoe_port_id, cs_pppoe_port_cfg_t *p_cfg) */
	{"pppoe_port_encap_param.device_id",         		tunnel_iplip_api_pppoe_port_encap_param_device_id},
        {"pppoe_port_encap_param.pppoe_port_id",           	tunnel_iplip_api_pppoe_port_encap_param_pppoe_port_id},
	//cs_pppoe_port_cfg_t
	{"pppoe_port_encap_param.pppoe_session_id",  	   	tunnel_iplip_api_pppoe_port_encap_param_pppoe_session_id},
        {"pppoe_port_encap_param.tx_phy_port_id",          	tunnel_iplip_api_pppoe_port_encap_param_tx_phy_port_id},
        {"pppoe_port_encap_param.src_mac",                 	tunnel_iplip_api_pppoe_port_encap_param_src_mac},
        {"pppoe_port_encap_param.dest_mac",                	tunnel_iplip_api_pppoe_port_encap_param_dest_mac},


	/* cs_l2tp_session_add(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, uint16 session_id)
           cs_l2tp_session_delete(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, uint16 session_id)
           cs_l2tp_session_get (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, uint16 session_id, u8 *is_present) */
	{"l2tp_session_param.device_id",               		tunnel_iplip_api_l2tp_session_param_device_id},
	{"l2tp_session_param.tunnel_id",             		tunnel_iplip_api_l2tp_session_param_tunnel_id},
	{"l2tp_session_param.session_id",       		tunnel_iplip_api_l2tp_session_param_session_id},

	/* cs_ipv6_over_l2tp_add(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_session_id_t session_id, cs_ip_address *ipv6_prefix)
           cs_ipv6_over_l2tp_delete (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_session_id_t session_id, cs_ip_address *ipv6_prefix)
           cs_ipv6_over_l2tp_getnext (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_session_id_t session_id, cs_ip_address *ipv6_prefix) */
	{"ipv6_over_l2tp_param.device_id",                      tunnel_iplip_api_ipv6_over_l2tp_param_device_id},
        {"ipv6_over_l2tp_param.tunnel_id",                      tunnel_iplip_api_ipv6_over_l2tp_param_tunnel_id},
	{"ipv6_over_l2tp_param.session_id",                     tunnel_iplip_api_ipv6_over_l2tp_param_session_id},
	{"ipv6_over_l2tp_param.ipv6_prefix",                    tunnel_iplip_api_ipv6_over_l2tp_param_ipv6_prefix},
	{"ipv6_over_l2tp_param.ipv6_prefix.addr_len",           tunnel_iplip_api_ipv6_over_l2tp_param_ipv6_prefix_addr_len},

	/* cs_tunnel_add(cs_dev_id_t device_id, cs_tunnel_cfg_t *p_tunnel_cfg, cs_tunnel_id_t *p_tunnel_id)
           cs_tunnel_delete(cs_dev_id_t device_id, cs_tunnel_cfg_t *p_tunnel_cfg)
           cs_tunnel_delete_by_idx (cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id)
           cs_tunnel_get(cs_dev_id_t device_id, cs_tunnel_id_t tunnel_id, cs_tunnel_cfg_t *p_tunnel_cfg) */
	{"tunnel_param.device_id",  	                      	tunnel_iplip_api_tunnel_param_device_id},
        {"tunnel_param.tunnel_id",                        	tunnel_iplip_api_tunnel_param_tunnel_id},
	//{"tunnel_param.src_afi",               			tunnel_iplip_api_tunnel_param_src_afi},
	//{"tunnel_param.dest_afi",              			tunnel_iplip_api_tunnel_param_dest_afi},
	{"tunnel_param.src_ipv4_addr",         		    	tunnel_iplip_api_tunnel_param_src_ipv4_addr},
	{"tunnel_param.dest_ipv4_addr",             		tunnel_iplip_api_tunnel_param_dest_ipv4_addr},
	{"tunnel_param.src_ipv6_addr",             		tunnel_iplip_api_tunnel_param_src_ipv6_addr},
	{"tunnel_param.dest_ipv6_addr",             		tunnel_iplip_api_tunnel_param_dest_ipv6_addr},
	{"tunnel_param.src_addr_len",          			tunnel_iplip_api_tunnel_param_src_addr_len},
	{"tunnel_param.dest_addr_len",         			tunnel_iplip_api_tunnel_param_dest_addr_len},
	//cs_tunnel_cfg_t
	{"tunnel_param.tunnel_type",           			tunnel_iplip_api_tunnel_param_tunnel_type},
	{"tunnel_param.tx_port", 	       			tunnel_iplip_api_tunnel_param_tx_port},
	//cs_l2tp_tunnel_cfg_t
	{"tunnel_param.l2tp.ver",              			tunnel_iplip_api_tunnel_param_l2tp_ver},
	{"tunnel_param.l2tp.len", 	                	tunnel_iplip_api_tunnel_param_l2tp_len},
	{"tunnel_param.l2tp.tid",   	        		tunnel_iplip_api_tunnel_param_l2tp_tid},
	{"tunnel_param.l2tp.ipv4_id",  	        		tunnel_iplip_api_tunnel_param_l2tp_ipv4_id},
	{"tunnel_param.l2tp.src_port",              		tunnel_iplip_api_tunnel_param_l2tp_src_port},
	{"tunnel_param.l2tp.dest_port",             		tunnel_iplip_api_tunnel_param_l2tp_dest_port},
	{0,				0}
};

