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



void send_to_lin(void *device, uint8_t msg_id, uint8_t *data, uint16_t len) {
  lin_lld_transmit((LinDriver *)device, msg_id, data, len);
}

void lin_master_start(void) {
  /* Start LIN Master */
  lin_lld_start(&LIN_MASTER, &lin_config_lin_master);
}
