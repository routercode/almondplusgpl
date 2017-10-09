#include <cs_fe_mc.h>

struct cs_mcg_resource_info cs_mcg_tbl;

cs_status cs_mcg_init() {
	int i;
	
	for (i = 0; i < MCIDX_TABLE_SIZE; i++)
		cs_mcg_tbl.mcidx_mirror[i] = 0;
	for (i = 0; i < MAX_VTABLE; i++)
		cs_mcg_tbl.mcgid_vtable_usage[i] = CS_VTABLE_INVALID;
	for (i = 0; i < MAX_MCGID; i++)
		cs_mcg_tbl.mcgid_std_usage[i] = CS_MCGID_INVALID;
	spin_lock_init(&cs_mcg_tbl.lock);
}

/*
 * This is an internal function. 
 * Assume we are already obtain the lock. 
 */

/*
 * see if there is any vtable valid. If not, it means we should clear out the
 * mcgid usage too since it cannot exists without vtable
 */

#define NO_VALID_VT_FOUND -1
int __ck_vtable() {
	int i;
	for (i = 0; i < MAX_VTABLE; i++) {
		if (cs_mcg_tbl.mcgid_vtable_usage[i] == CS_VTABLE_VALID)
			return i;
	}
	for (i = 0; i < MAX_MCGID; i++)
		cs_mcg_tbl.mcgid_std_usage[i] = CS_MCGID_INVALID;
	for (i = 0; i < MCIDX_TABLE_SIZE; i++)
		cs_mcg_tbl.mcidx_mirror[i] = 0;

	return NO_VALID_VT_FOUND;
}

cs_status __clear_mcg_id( int id) {
	int i;

	for (i = 0; i < MAX_VTABLE; i++) {
		/*
		 * Fix the table to clear all entries with the new mcgid
		 * with vtable.
		 */
		if (cs_mcg_tbl.mcgid_vtable_usage[i] == CS_VTABLE_VALID) 
			cs_mcg_tbl.mcidx_mirror[i * MCG_INCREMENT + id] = 0;
	}
}


cs_status cs_allocate_mcg_vtable_id( int *id, int forced)
{
	int i, dup_table;

	spin_lock(&(cs_mcg_tbl.lock));
	// Look for a valid vtable to duplicate from. if no valid vtable, that
	// means the mcgid are invalid
	dup_table = __ck_vtable();

	if (forced) {
		if(cs_mcg_tbl.mcgid_vtable_usage[*id] != CS_VTABLE_INVALID) { 
			spin_unlock(&(cs_mcg_tbl.lock));
			return CS_MCG_RESOURCE_EXHAUSTED;
		}
	} else {
		for (i = 0; i < MAX_VTABLE; i++)
			if (cs_mcg_tbl.mcgid_vtable_usage[i] ==
					CS_VTABLE_INVALID)
				break;
		if (i == MAX_VTABLE) {
			spin_unlock(&cs_mcg_tbl.lock);
			return CS_MCG_RESOURCE_EXHAUSTED;
		}
		*id = i;
	}
	cs_mcg_tbl.mcgid_vtable_usage[*id] = CS_VTABLE_VALID;
	if (dup_table != NO_VALID_VT_FOUND) {
		for (i = 0; i < MAX_MCGID; i++) {
			/* copy mcgid from other table */
			cs_mcg_tbl.mcidx_mirror[(*id) * MCG_INCREMENT + i] =
				cs_mcg_tbl.mcidx_mirror[dup_table * MCG_INCREMENT + i];
		}
	}
	/* FIXME: Write back to HW */
	spin_unlock(&cs_mcg_tbl.lock);
	return CS_OK;
}

cs_status cs_deallocate_mcg_vtable_id(int id)
{
	int i;

	spin_lock(&cs_mcg_tbl.lock);
	if (cs_mcg_tbl.mcgid_vtable_usage[id] != CS_VTABLE_VALID) {
		spin_unlock(&cs_mcg_tbl.lock);
		return CS_ERROR;
	}
	cs_mcg_tbl.mcgid_vtable_usage[id] = CS_VTABLE_INVALID;
	spin_unlock(&cs_mcg_tbl.lock);

	for (i = 0; i < MAX_MCGID; i++) {
		cs_mcg_tbl.mcidx_mirror[id * MCG_INCREMENT + i] = 0;
	}
	__ck_vtable();

	// FIXME: write to hardware
	spin_unlock(&cs_mcg_tbl.lock);
	return CS_OK;
}

cs_status cs_allocate_mcg_id(int *id)
{
	int i;

	spin_lock(&(cs_mcg_tbl.lock));
	for (i = 0; i < MAX_MCGID; i++) 
		if (cs_mcg_tbl.mcgid_std_usage[i] == CS_MCGID_INVALID)
			break;
	if (i == MAX_MCGID) {
		spin_unlock(&cs_mcg_tbl.lock);
		return CS_MCG_RESOURCE_EXHAUSTED;
	}
	*id = i;
	cs_mcg_tbl.mcgid_std_usage[*id] = CS_MCGID_VALID;
	__clear_mcg_id(*id);
	// write back to hardware

	spin_unlock(&cs_mcg_tbl.lock);
	return CS_OK;
}

cs_status cs_deallocate_mcg_id(int id)
{
	spin_lock(&cs_mcg_tbl.lock);
	cs_mcg_tbl.mcgid_std_usage[id] = CS_MCGID_INVALID;

	__clear_mcg_id(id);
	// write back to hardware

	spin_unlock(&cs_mcg_tbl.lock);
	return CS_OK;
}

cs_status cs_allocate_std_mcidx(int mcgid, int *idx)
{
	int i,j;

	spin_lock(&cs_mcg_tbl.lock);
	for (i = 0; i < MCIDX_MAX_REP; i++) {
		if (!(cs_mcg_tbl.mcidx_mirror[mcgid] & (1 << i)))
			break;
	}
	if (i == MCIDX_MAX_REP) {
		spin_unlock(&cs_mcg_tbl.lock);
		return CS_MCG_RESOURCE_EXHAUSTED;
	}
	// need to do it MAX_VTABLE times since we are overloading
	for (j = 0; j < MAX_VTABLE; j++) {
		cs_mcg_tbl.mcidx_mirror[j * MCG_INCREMENT + mcgid] |= (1 << i);
	}
	// FIXME: write to hardware
	*idx = i;
	spin_unlock(&cs_mcg_tbl.lock);
	return CS_OK;
}

cs_status cs_deallocate_std_mcidx(int mcgid, int idx)
{
	int j;

	spin_lock(&cs_mcg_tbl.lock);
	if (cs_mcg_tbl.mcgid_vtable_usage[mcgid] != CS_VTABLE_VALID) {
		spin_unlock(&cs_mcg_tbl.lock);
		return CS_ERROR;
	}
	for (j = 0; j < MAX_VTABLE; j++)
		cs_mcg_tbl.mcidx_mirror[j * MCG_INCREMENT + mcgid] &=
			~(1 << idx);

	// FIXME: write to hardware
	spin_unlock(&cs_mcg_tbl.lock);
	return CS_OK;
}

