#ifndef __CS_HW_ACCEL_BASE_H__
#define __CS_HW_ACCEL_BASE_H__

#if 0
void core_add_user(cs_uint16 hash_index, cs_uint8 vtable_id,
                cs_uint8 guid_cnt, cs_uint64 *p_guid);
struct cs_core_hash *core_find_user(cs_uint64 guid);
void core_delete_user(struct cs_core_hash *s);
void core_delete_all(void);
void core_print_users(void);
void core_get_users(cs_uint64 guid, struct cs_core_hash **p_index);
void cs_kernel_core_hash_timer_func(cs_uint32 data);
cs_uint8 __cs_kernel_to_hash_mask(CS_HASH_ENTRY_S *hash_entry, 
        fe_hash_mask_entry_t *mask_rule);
cs_uint32 __cs_kernel_core_add_hash_by_cb(CS_KERNEL_ACCEL_CB_T *cs_cb);
int __cs_kernel_to_forwarding_result(fe_hash_rslt_s *result);
cs_uint16 __cs_uu_flow_to_forwarding_result(fe_hash_rslt_s *result);
cs_status __cs_vtable_to_vlan_table(fe_hash_rslt_s *p_vlan);
cs_status __cs_kernel_to_checkmem(CS_HASH_ENTRY_S *p_hash, 
        fe_hash_check_entry_t *pchkmem);
cs_boolean __cs_kernel_check_qos_enbl(CS_HASH_ENTRY_S *p_hash);
cs_uint16 __cs_kernel_to_qos_result(fe_hash_rslt_s *result);
cs_uint8 __cs_kernel_to_hash_mask_qos_only(CS_HASH_ENTRY_S *hash_entry, 
		fe_hash_mask_entry_t *mask_rule);
cs_uint8 __cs_kernel_to_qos_hash_mask(CS_HASH_ENTRY_S *hash_entry, 
		fe_hash_mask_entry_t *mask_rule);
cs_status cs_kernel_update_hash_key_from_qos(fe_sw_hash_t *p_swhash);
cs_status cs_kernel_core_del_hash_by_guid(cs_uint64 guid);
cs_status cs_kernel_get_hash_key_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb,
                fe_sw_hash_t *p_key);
cs_status cs_kernel_get_hash_mask_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb,
                cs_uint64 *p_mask);
cs_status cs_kernel_get_hash_action_from_cs_cb(CS_KERNEL_ACCEL_CB_T *cs_cb,
                fe_hash_rslt_s *result);
cs_status cs_kernel_get_module_callback(cs_uint64 *p_guid_array,
                cs_uint8 guid_cnt,      cs_uint16 status);
cs_status cs_kernel_input_set_cb(struct sk_buff *skb);
NI_DEV_T *cs_kernel_core_find_gmacdev(struct net_device *device);
cs_uint32 __cs_kernel_core_add_hash_by_skb(struct sk_buff *skb);
cs_status cs_kernel_get_pktlen_rng_match_vector(cs_uint16 high, 
		cs_uint16 low, cs_uint8 *p_vector);
#endif
#endif
