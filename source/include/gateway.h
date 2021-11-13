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
#ifndef _GATEWAY_H_
#define _GATEWAY_H_

#include <platform.h>

/*
 * Gateway message type
 */
#define GATEWAY_MSG_LIN        0x01
#define GATEWAY_MSG_CAN        0x02
#define GATEWAY_MSG_ETH        0x03

/*
 * Gateway actions
 */
#define INVALID_ACTION         0xFFFFFFFF
#define LIN_TO_CAN             0x00000001
#define LIN_TO_ETH             0x00000002
#define LIN_TO_LIN             0x00000003
#define CAN_TO_LIN             0x00000004
#define CAN_TO_ETH             0x00000005
#define CAN_TO_CAN             0x00000006
#define ETH_TO_CAN             0x00000007
#define ETH_TO_LIN             0x00000008
#define ETH_TO_ETH             0x00000009

/*
 * Gatewar destinations
 */
#define INVALID_DESTINATION    0xFFFFFFFF


/*
 * Gatewy message data length
 */
#define GATEWAY_MSG_DATA_LEN   16
/*
 * Gateway message type.
 */
typedef struct GatewayMsg_s {
	uint32_t type;                         /* Message type: LIN, CAN, ETH, ... */
    uint32_t id;                           /* Message id: LIN id, CAN id, ETH address */
    uint32_t id2;                          /* Specific to message */
	uint32_t data_len;                     /* Data length */
	uint8_t  data[GATEWAY_MSG_DATA_LEN];   /* data buffer */
} GatewayMsg_t;


#ifdef __cplusplus
extern "C" {
#endif

extern void send_to_gateway_from_ISR(GatewayMsg_t *gw_msg);
extern void send_to_gateway_from_task(GatewayMsg_t *gw_msg);

extern uint32_t gateway_start(void);

#ifdef __cplusplus
}
#endif


#endif /* _GATEWAY_H_ */
