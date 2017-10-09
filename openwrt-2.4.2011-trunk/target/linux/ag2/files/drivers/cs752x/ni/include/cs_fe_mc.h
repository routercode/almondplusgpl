#ifndef CS_MCG
#define CS_MSG 1

#include <mach/cs_types.h>
#include <linux/spinlock.h>

#define MCGID_BITS 8
#define MCGID_VTABLE_BITS 4 // Allow adjustment to number of bits for MCGID/VTABLE

#define MAX_VTABLE (1 << (MCGID_VTABLE_BITS-1))
#define MAX_MCGID (1 << (MCGID_BITS - MCGID_VTABLE_BITS - 1)) 
#define MCG_VTABLE_MASK (unsigned int)(0xFF << MCGID_VTABLE_BITS)
#define MCG_INCREMENT (1 << (MCGID_BITS - MCGID_VTABLE_BITS))
#define MCIDX_TABLE_SIZE 0xFF
#define MCIDX_MAX_REP 0x05
#define CS_VTABLE_INVALID 0
#define CS_MCGID_INVALID 0
#define CS_VTABLE_VALID 1
#define CS_MCGID_VALID 1

#define CS_MCG_SUCCESS  1
#define CS_MCG_RESOURCE_EXHAUSTED (-1)

struct cs_mcg_resource_info {
        unsigned int mcgid_vtable_usage[MAX_VTABLE];
	unsigned int mcgid_std_usage[MAX_MCGID];
        unsigned int mcidx_mirror[MCIDX_TABLE_SIZE];
        spinlock_t lock;
};

cs_status cs_allocate_mcg_vtable_id( int *id, int forced );
cs_status cs_allocate_std_mcidx ( int mcgid, int *idx );

cs_status cs_deallocate_mcg_vtable_id( int id );
cs_status cs_deallocate_std_mcidx ( int mcgid, int idx );

cs_status cs_mcg_init(void);

#endif
