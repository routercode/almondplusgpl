#include <cs_vhash_table.h>
#include <linux/module.h>

struct cs_hash_vtable hash_vtable[MAX_VTABLE];

cs_status cs_vhtable_init() {
	int i;

	// mark all tables to be invalid
	for (i=0;i<MAX_VTABLE;i++) {
		hash_vtable[i].vid = CS_VTABLE_INVALID;
		// FIXME: init spinlock
	}
}

// should be inline
int cs_vhtable_get_mcgid ( int vtid) {
	return hash_vtable[vtid].mcg_vtable_id;
}

cs_status __cs_vhtable_allocate(int mcg_id, int type, int *vtid, char *name) 
{
	int i, tuple;
	//fe_sdb_vtable_s *vtable_sdb;
	//fe_class_vtable_s *vtable_class;
	
	/* get a fresh table */
	for (i=0; i<MAX_VTABLE; i++) { 
		if (hash_vtable[i].vid == CS_VTABLE_INVALID) 
			break;
	}
	if (i == MAX_VTABLE) {
		return CS_VTABLE_RESOURCE_EXHAUSTED;
	}

	memcpy( hash_vtable[i].table_name, name, MAX_VTABLE_NAME);
	hash_vtable[i].type = type;
	hash_vtable[i].mcg_vtable_id = mcg_id;
	hash_vtable[i].vid = i;
	*vtid = i;
	
	// FIXME: allocate classifier
	// cs_alloc_vtable( *vtid, 0, hash_vtable[i].vtable );

	//cs_vtable_allocate_class(i, type, &vtable_class);
	//cs_vtable_allocate_add_class(&vtable_class);	
	// FIXME: allocate SDB
	//cs_vtable_allocate_sdb(i, type, &vtable_sdb);
	//tuple = 1;
	//cs_vtable_allocate_add_sdb(&vtable_sdb,  tuple);
	//cs_vhtable_
	
	return CS_OK;
}	


/* 
 * Allocate a virtual hash table.  Bit forced is used to indicate a given MCGID
 * that the caller wants instead of letting the system allocate it dynamically.
 * This is more for the first/default vtable.
 */
cs_status cs_vhtable_allocate( int type, int mcgid, int *vtid, char *name, int forced ) {
	if ( cs_allocate_mcg_vtable_id (&mcgid, forced) != CS_MCG_SUCCESS ) 
		return CS_VTABLE_RESOURCE_EXHAUSTED;
	__cs_vhtable_allocate( mcgid, type, vtid, name);
}

cs_status cs_vhtable_get_id_from_name ( int *id, char *vtable_name) {
	int i;

	for (i=0;i<MAX_VTABLE;i++) {
		if ( hash_vtable[i].vid != CS_VTABLE_INVALID ) {
			if (!strcmp( hash_vtable[i].table_name, vtable_name) )
				*id = i;
				return CS_OK;
		}
	}
	return CS_ERROR;
}

cs_status cs_vhtable_deallocate( char *vtable_name) {
	int vtable_id;

	cs_vhtable_get_id_from_name ( &vtable_id, vtable_name);
	return cs_vhtable_deallocate_from_id( vtable_id);

}

cs_status cs_vhtable_deallocate_from_id( int id) {
	int h_idx;

	if (id == CS_VTABLE_INVALID) return CS_ERROR;

	for (h_idx = 0; h_idx < FE_VTABLE_SIZE; h_idx++) {
		// FIXME: hash entry release, which would release the result table entry
	}
	
	// FIXME: release classifier

	// FIXME: release sdb

	// FIXME: Deallocate mcgid
	cs_deallocate_mcg_vtable_id( hash_vtable[id].mcg_vtable_id );

	hash_vtable[id].vid = CS_VTABLE_INVALID;
	return CS_OK;
}

/*
cs_status cs_vhtable_insert_hash( int vtid, cs_uint16 crc16, cs_uint32 crc32,
        cs_uint8 mask_ptr, <result action>, cs_uint16 *h_idx,>);
{
	// FIXME: insert hash and its result
	// inserthash( sdb_id, ....)
}

cs_status cs_add_vtable_entry(cs_uint16 crc16, cs_uint32 crc32,
        cs_uint8 mask_ptr, cs_uint16 result_index, cs_uint16 *return_idx,
        fe_hash_vtable_s *table);

cs_status cs_vhtable_delete_hash( int vtid, cs_uint16 hash_idx) {
	// get result table/delete
	// get voqpol table/delete
	// get hash mask/delete
        // remove from sdb
}
cs_status cs_invalidate_vtable_entry(fe_hash_vtable_s *table,
        cs_uint16 index);

*/
