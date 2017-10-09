/*
 * Copyright (c) Cortina-Systems Limited 2010.  All rights reserved.
 *                Wen Hsu <wen.hsu@cortina-systems.com>
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
/*
 * cs_hw_accel_qos_data.h
 *
 * $Id: cs_hw_accel_qos_data.h,v 1.1.1.1 2011/08/10 01:19:26 peebles Exp $
 *
 * This header file defines the internal data structures and APIs for CS QoS.
 */

#ifndef __CS_HW_ACCEL_QOS_DATA_H__
#define __CS_HW_ACCEL_QOS_DATA_H__

#include <linux/list.h>
#include <mach/cs_types.h>
#include <mach/hw_accel.h>
#include <cs_hw_accel_util.h>

#define CS_QOS_VOQ_ID_FROM_PORT_QUEUE_ID(port_id, queue_id) \
	((port_id << 3) + (queue_id))
#define CS_QOS_VOQ_NUM				(112)
#define CS_QOS_VOQ_PER_PORT			(8)
#define CS_QOS_PORT_NUM				(14)
#define CS_QOS_VOQ_DEFAULT_QUANTA	(3044)
#define CS_QOS_VOQ_PRIORITY_MAX		(7)

#define CS_QOS_FLOW_POL_NUM			(128)

typedef enum {
	CS_QOS_VOQ_TYPE_DRR,
	CS_QOS_VOQ_TYPE_SP,
	CS_QOS_VOQ_TYPE_MAX,
} cs_qos_voq_type_e;

typedef struct {
	cs_qos_voq_type_e type;
	cs_uint8 priority;
	cs_uint32 weight;
	cs_uint32 rate;
} cs_qos_voq_param_t;

typedef struct cs_qos_guid_map_s {
	cs_uint8 pol_id;
	cs_uint64 guid;
	struct list_head pol_list;
	struct list_head guid_list;
} cs_qos_guid_map_t;

typedef struct cs_qos_pol_guid_s {
	cs_uint64 cnt;
	struct list_head guid_map_head;
} cs_qos_pol_guid_t;


#endif /* __CS_HW_ACCEL_QOS_DATA_H__ */
