#include <linux/jiffies.h>
#include <cs_hw_accel_util.h>
#include <cs_hw_accel_core.h>
#include <linux/mutex.h>

/*
	GUID Global Data
	===========================================
*/

	//DEFINE_MUTEX(cs_global_guid_lock);
	cs_uint32 cs_global_guid_counter;
	cs_uint32 old_jiffies;

/*
	GUID Helper Functions
	===========================================
*/
#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)
#define ct_assert(e) enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }

ct_assert( sizeof(cs_uint64)==8 );
ct_assert( sizeof(cs_uint32)==4 );
ct_assert( sizeof(cs_uint16)==2 );

cs_uint64 cs_kernel_adapt_get_guid(cs_uint16 guid_type) {
	cs_uint64 tmpVal;
	
	//FIXME: mutex_lock instead of spin_lock
	//mutex_lock (&cs_global_guid_lock);
	tmpVal = cs_global_guid_counter;
	cs_global_guid_counter++;

	tmpVal &= ((cs_uint64) 0x0000FFFFFFFFFFFFLL) & ((cs_uint64) cs_global_guid_counter);
	//mutex_unlock( &cs_global_guid_lock);

	tmpVal |= (cs_uint64) (( (cs_uint64) guid_type << 48) & 0xFFFF000000000000LL );

	return tmpVal;
}

void cs_kernel_guid_init(void){
	//mutex_init(&cs_global_guid_lock);
	cs_global_guid_counter = 0x800000;
}


/*
	CB Global Data
	===========================================
*/
cs_kernel_cb *cs_global_cb[CS_KERNEL_MAX_ID_NUM];
cs_kernel_cb *cs_alloc_cb[CS_HASH_CB_ENTRYS];

/*
	CB Helper Functions
*/
int getIndex( cs_uint16 inVal) {
	int i=0;
	cs_uint32 val;

	val = (cs_uint32)(0xFFFF0000) | inVal;
	while ( !(val & 0x01) ) { val >>= 1; i++; }
	return i;
}

int cs_kernel_reg_hash_cb(cs_uint16 id_tag, void *cb_func) {
	int index;

	if (id_tag > CS_KERNEL_MAX_ID_NUM ) return CS_REG_FAILURE_TAG_OUT_OF_RANGE;
	index = getIndex( id_tag );
	if ( cs_global_cb [index] != NULL) return CS_REG_FAILURE_EXISTS;
	cs_global_cb[index] = cb_func;
	return CS_REG_SUCCESSFUL;
}
int cs_kernel_dereg_hash_cb(cs_uint16 id_tag) {
	int index;

	if (id_tag > CS_KERNEL_MAX_ID_NUM ) return CS_REG_FAILURE_TAG_OUT_OF_RANGE;
	index = getIndex( id_tag);
	cs_global_cb[index] = NULL;
	return CS_REG_SUCCESSFUL;
}
cs_kernel_cb *cs_kernel_get_cb_by_tag_id(cs_uint16 id_tag) {
	int index;

	if (id_tag > CS_KERNEL_MAX_ID_NUM) return NULL;
	index = getIndex(id_tag);
	return cs_global_cb[index];
}
void cs_kernel_hash_cb_init(){
	int i;

	for (i=0;i<CS_KERNEL_MAX_ID_NUM;i++)
		cs_global_cb[i] = NULL;

	cs_kernel_alloc_cb();
}
void cs_kernel_invoke_cb( cs_uint16 id_tag, cs_uint64 guid, int status) {
	int index;

	index = getIndex (id_tag);
	if (cs_global_cb[index]==NULL) return;
        (*cs_global_cb[index])(guid, status);
}

void cs_kernel_alloc_cb(){
	int i;

 	/* Only IPSec need build 2 hash entrys */
 	for (i = 0; i < CS_HASH_CB_ENTRYS; i ++) {
		cs_alloc_cb[i] = kmalloc(sizeof(CS_KERNEL_ACCEL_CB_T), GFP_KERNEL);
			if (cs_alloc_cb[i] == NULL) return;
				// return -ENOMEM;
	}
}

void cs_kernel_dalloc_cb(){
	int i;

 	for (i = 0; i < CS_HASH_CB_ENTRYS; i ++) {
		kfree(cs_alloc_cb[i]);
	}
}

/* set module GUID to skb->cb */
cs_status cs_kernel_add_guid(cs_uint64 guid, struct sk_buff *skb)
{
	CS_KERNEL_ACCEL_CB_T *cs_cb;
	cs_cb = CS_KERNEL_SKB_CB(skb);

	if (NULL == cs_cb) return CS_ERROR;
	if (MODULE_GUID_NUM == cs_cb->common.guid_cnt) return CS_ERROR;

	cs_cb->common.guid[cs_cb->common.guid_cnt] = guid;
	cs_cb->common.guid_cnt++;

	return CS_OK;
}
/*
	Utility initialization
	============================================
*/

cs_int32 cs_kernel_remove_hash_guid (cs_uint64 guid)
{
	return 0;
}

void cs_kernel_util_init() {
	cs_kernel_guid_init();
	cs_kernel_hash_cb_init();
}

