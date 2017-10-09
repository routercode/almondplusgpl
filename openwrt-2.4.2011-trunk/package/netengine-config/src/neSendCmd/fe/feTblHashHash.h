#ifndef _FE_TABLE_HASH_HASH_
#define _FE_TABLE_HASH_HASH_

#ifndef bool
#define bool int
#endif				

#include <cs75xx_fe_core_table.h>

#define FE_HASH_HASH_STRUCTURE_SIZE     116
/* 
 * Hash Match Table Entry
 * Total 24k entries.
 * Hash result, Result table index, mask ptr, etc.
 */
typedef struct fe_hash_hash {
	union {
		unsigned char HHBuffer[FE_HASH_HASH_STRUCTURE_SIZE];
		struct {
			fe_sw_hash_t swhash;
		}; /* struct */
	}; /* union */
} fe_hash_hash_s;

#endif /* _FE_TABLE_HASH_HASH_ */
