#ifndef _CAL_HW_HASH_H_
#define _CAL_HW_HASH_H_

#ifndef bool
#define bool int
#endif

#include <cs_types.h>
#include <cs75xx_fe_core_table.h>
typedef enum { 
	CRC16_CCITT, CRC16_14_1, CRC16_14_2, CRC16_14_3,
} CRC16_POLYNOMIAL_DEF;
cs_status fe_tbl_HashHash_cal_crc(fe_sw_hash_t * swhash, cs_uint32 * pCrc32,
				  cs_uint16 * pCrc16,
				  cs_uint8 crc16_polynomial);

#endif /*_CAL_HW_HASH_H_ */
