#ifndef __CS_NE_IOCTL_H__
#define __CS_NE_IOCTL_H__


#define SIOCDNEPRIVATE	SIOCDEVPRIVATE + 0x6	/* 0x89F0 + 0x6 */
#define	REGREAD			0
#define REGWRITE		1
#define NE_NI_IOCTL		2
#define	NE_FE_IOCTL		3
#define	NE_QM_IOCTL		4
#define	NE_SCH_IOCTL	5
#define	GMIIREG			6
#define	SMIIREG			7


/* Table id */
typedef enum {
	CS_IOTCL_TBL_CLASSIFIER,    /* 0 */
	CS_IOCTL_TBL_SDB,
	CS_IOCTL_TBL_HASH_MASK,
	CS_IOCTL_TBL_LPM,
	CS_IOCTL_TBL_HASH_MATCH,

	CS_IOCTL_TBL_FWDRSLT,       /* 5 */
	CS_IOCTL_TBL_QOSRSLT,
	CS_IOCTL_TBL_L3_IP,
	CS_IOCTL_TBL_L2_MAC,
	CS_IOCTL_TBL_VOQ_POLICER,

	CS_IOCTL_TBL_LPB,           /* 10 */
	CS_IOCTL_TBL_AN_BNG_MAC,
	CS_IOCTL_TBL_PORT_RANGE,
	CS_IOCTL_TBL_VLAN,
	CS_IOCTL_TBL_ACL_RULE,

	CS_IOCTL_TBL_ACL_ACTION,    /* 15 */
	CS_IOCTL_TBL_PE_VOQ_DROP,
	CS_IOCTL_TBL_ETYPE,
	CS_IOCTL_TBL_LLC_HDR,
	CS_IOCTL_TBL_FVLAN,

	CS_IOCTL_TBL_HASH_HASH,     /* 20 */
	CS_IOCTL_TBL_MAX,
} cs_ioctl_table_id_e;


/* Table Command  */
typedef enum
{
	CMD_ADD,
	CMD_DELETE,
	CMD_FLUSH,
	CMD_GET,
	CMD_REPLACE,
	CMD_INIT,

	CMD_MAX,
}COMMAND_DEF;


typedef struct {
	unsigned short		cmd;	/* command ID */
	unsigned short		len;	/* data length, excluding this header */
} NECMD_HDR_T;


/* REGREAD */
typedef struct {
	unsigned int		location;
	unsigned int		length;
	unsigned int		size;
} REGREAD_T;

/* REGWRITE */
typedef struct {
	unsigned int		location;
	unsigned int		data;
	unsigned int		size;
} REGWRITE_T;

/* GMIIREG */
typedef	struct{
	unsigned short		phy_addr;
	unsigned short		phy_reg;
	unsigned short		phy_len;
} GMIIREG_T;

/* SMIIREG */
typedef	struct{
	unsigned short		phy_addr;
	unsigned short		phy_reg;
	unsigned int		phy_data;
} SMIIREG_T;

typedef union
{
	REGREAD_T reg_read;
	REGWRITE_T	reg_write;
	GMIIREG_T get_mii_reg;
	SMIIREG_T set_mii_reg;
} NE_REQ_E;

typedef struct {
    unsigned char       Module;         // reference MODULE_DEF
    unsigned char       table_id;       // reference xx_TABLE_DEF
    unsigned char       cmd;            // reference COMMAND_DEF
    int                 Bypass;         // 0: Disable, 1: Enable
    unsigned short		idx_start;      // defined for command GET
    unsigned short		idx_end;        // defined for command GET
} NEFE_CMD_HDR_T;

typedef enum
{
	MODULE_NI,
	MODULE_FE,
	MODULE_TM,
	MODULE_SCH,
	MODULE_QM,

	MODULE_NULL,
}MODULE_DEF;



/* You can put all IOCTL structure here 
   for Ex. REGREAD, REGWRITE, GMIIREG... */

#endif//__CS_NE_IOCTL_H__
