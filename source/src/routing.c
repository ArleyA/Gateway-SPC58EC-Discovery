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
#include <string.h>
#include <components.h>

#include <device.h>
#include <candefs.h>
#include <lindefs.h>
#include <ethdefs.h>
#include <gateway.h>
#include <routing.h>

/* Just for access to ETHD1 */
#include <eth.h>


typedef struct RoutingTables_s {
  uint32_t        msg_type;  /* Message type (LIN, CAN, ETH) */
  RoutingTable_t *table;     /* Routing table */
} RoutingTables_t;


/*
 * LIN routing table
 */
RoutingTable_t lin_routing_table[] = {
  { LIN_MSG_2,     LIN_TO_CAN, &CAN_DEVICE, CAN_MSG_2},
  { LIN_MSG_3,     LIN_TO_LIN, &LIN_MASTER, LIN_MSG_3},
  { LIN_MSG_4,     LIN_TO_ETH, &ETH_DEVICE, ETH_MSG_3},
  {         0, INVALID_ACTION,        NULL,         0}
};

/*
 * CAN routing table
 */
RoutingTable_t can_routing_table[] = {
  { CAN_MSG_1,     CAN_TO_CAN, &CAN_DEVICE, CAN_MSG_3},
  { CAN_MSG_4,     CAN_TO_LIN, &LIN_MASTER, LIN_MSG_3},
  { CAN_MSG_5,     CAN_TO_ETH, &ETH_DEVICE, ETH_MSG_1},
  {         0, INVALID_ACTION,        NULL,         0}
};

/*
 * ETH routing table
 */
RoutingTable_t eth_routing_table[] = {
  { ETH_MSG_1,     ETH_TO_CAN, &CAN_DEVICE, CAN_MSG_3},
  { ETH_MSG_2,     ETH_TO_LIN, &LIN_MASTER, LIN_MSG_3},
  { ETH_MSG_3,     ETH_TO_ETH, &ETH_DEVICE, ETH_MSG_4},
  {         0, INVALID_ACTION,        NULL,         0}
};

/*
 * Routing tables
 */
static RoutingTables_t routing_tables[] = {
  { GATEWAY_MSG_LIN, lin_routing_table },
  { GATEWAY_MSG_CAN, can_routing_table },
  { GATEWAY_MSG_ETH, eth_routing_table }
};


/*
 * Returns routing table size
 */
#define TABLE_SIZE(_table_) (sizeof(_table_)/sizeof(_table_[0]))


RoutingTable_t *get_routing_table(uint32_t msg_type, uint32_t msg_id) {
  uint32_t i;
  RoutingTable_t *table;

  table = NULL;

  for (i = 0; i < TABLE_SIZE(routing_tables); i++) {
    if (routing_tables[i].msg_type == msg_type) {
      table = routing_tables[i].table;
      break;
    }
  }

  if (table != NULL) {
    while (table->action != INVALID_ACTION) {
      if (table->src_msg_id == msg_id) {
        break;
      }
      table++;
    }

    if (table->action == INVALID_ACTION) {
      table = NULL;
    }
  }

  return table;
}


uint32_t get_action(RoutingTable_t *table) {

  uint32_t action;

  action = INVALID_ACTION;

  if (table != NULL) {
        action = table->action;
  }

  return action;
}

extern void *get_destination_device(RoutingTable_t *table) {

  void *dev;

  dev = NULL;

  if (table != NULL) {
     dev = table->dst_device;
  }
  return dev;
}


uint32_t get_destination_message_id(RoutingTable_t *table) {

  uint32_t dst;

  dst = INVALID_DESTINATION;

  if (table != NULL) {
        dst = table->dst_msg_id;
  }

  return dst;
}
