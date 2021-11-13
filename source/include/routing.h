/****************************************************************************
*
* Copyright © 2018-2019 STMicroelectronics - All Rights Reserved
*
* This software is licensed under SLA0098 terms that can be found in the
* DM00779817_1_0.pdf file in the licenses directory of this software product.
*
* THIS SOFTWARE IS DISTRIBUTED "AS IS," AND ALL WARRANTIES ARE DISCLAIMED,
* INCLUDING MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*
*****************************************************************************/
#ifndef _ROUTiNG_H_
#define _ROUTiNG_H_

#include <platform.h>
#include <gateway.h>

/*
 * Routing table
 */
typedef struct RoutingTable_s {
  uint32_t  src_msg_id;    /* Source message */
  uint32_t  action;        /* How to route the message */
  void     *dst_device;    /* Destination device */
  uint32_t  dst_msg_id;    /* Destination message id */
} RoutingTable_t;


#ifdef __cplusplus
extern "C" {
#endif

extern RoutingTable_t *get_routing_table(uint32_t msg_type, uint32_t msg_id);
extern uint32_t get_action(RoutingTable_t *table);
extern void *get_destination_device(RoutingTable_t *table);
extern uint32_t get_destination_message_id(RoutingTable_t *table);

#ifdef __cplusplus
}
#endif


#endif /* _ROUTiNG_H_ */
