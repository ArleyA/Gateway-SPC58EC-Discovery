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

#include <can_lld_cfg.h>

#include <device.h>
#include <candefs.h>
#include <gateway.h>


#define TO_UINT32(_buf_) \
        (((uint32_t)(((uint8_t *)(_buf_))[3]) <<   0) | \
         ((uint32_t)(((uint8_t *)(_buf_))[2]) <<   8) | \
         ((uint32_t)(((uint8_t *)(_buf_))[1]) <<  16) | \
         ((uint32_t)(((uint8_t *)(_buf_))[0]) <<  24))


/*
 * This callback is called by CAN interrupt on receiving a message.
 */
void SUBSYS_0_CAN1_rx_callback(uint32_t msg_buf, CANRxFrame can_msg) {
  (void)msg_buf;

  GatewayMsg_t gw_msg;

  /*
   * Silently failing if message length is
   * greater than gateway message data length.
   */
  if (can_msg.DLC <= GATEWAY_MSG_DATA_LEN) {
    gw_msg.type = GATEWAY_MSG_CAN;
    gw_msg.id   = can_msg.ID;

    memcpy(gw_msg.data, can_msg.data32, can_msg.DLC);
    gw_msg.data_len = can_msg.DLC;

    send_to_gateway_from_ISR(&gw_msg);
  }
}

void send_to_can(void *device, uint32_t msg_id, uint8_t *data, uint16_t len) {
  CANTxFrame can_msg;
  uint32_t data0, data1;

  data0 = TO_UINT32(&data[0]);
  data1 = TO_UINT32(&data[4]);

  can_msg.TYPE = CAN_ID_STD;
  can_msg.ID = msg_id;
  can_msg.DLC = len;
  can_msg.data32[0] = data0;
  can_msg.data32[1] = data1;

  can_lld_transmit((CANDriver *)device, CAN_ANY_TXBUFFER, &can_msg);
}

void can_start(void) {
  /* Start CAN */
  can_lld_start(&CAN_DEVICE, &can_config_SUBSYS_0_CAN1);
}
