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
#include <lindefs.h>
#include <gateway.h>
#include <routing.h>



void lin_slave_start(void) {
  /* Start LIN Slave */
  lin_lld_start(&LIN_SLAVE, &lin_config_lin_slave);
}


/*
 * This callback is called by LIN slave interrupt on a master receive
 * message request (transmit from slave to master).
 */
uint32_t lin_slave_tx_callback(LinDriver *ldp, uint8_t idMessage, uint8_t *buffer, uint16_t len) {
  (void)ldp;
  uint8_t i;
  uint32_t ret;

  /* Fill in data buffer */
  switch (idMessage) {
  case LIN_MSG_1:
    for (i = 0; i < len; i++) {
      buffer[i] = (uint8_t)'A' + i;
    }
    ret = len;
    break;
  default:
    ret = 0;
    break;
  }

  return ret;
}


/*
 * This callback is called by LIN slave interrupt on a master transmit
 * message request (receive from slave to master).
 */
uint32_t lin_slave_rx_callback(LinDriver *ldp, uint8_t idMessage, uint8_t *buffer, uint16_t len) {

  (void)ldp;

  GatewayMsg_t gw_msg;

  /*
   * Silently failing if message length is
   * greater than gateway message data length.
   */
  if (len <= GATEWAY_MSG_DATA_LEN) {
    gw_msg.type = GATEWAY_MSG_LIN;
    gw_msg.id   = idMessage;

    memcpy(gw_msg.data, buffer, len);
    gw_msg.data_len = len;

    send_to_gateway_from_ISR(&gw_msg);
  }

  return 0;
}
