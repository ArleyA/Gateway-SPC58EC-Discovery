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
#ifndef _ETH_H_
#define _ETH_H_

#include <platform.h>

extern uint32_t ETHD1;

#ifdef __cplusplus
extern "C" {
#endif

extern void eth_start(void);

extern void send_to_eth(void *device, uint8_t msg_id, uint8_t *buffer, uint16_t len);

#ifdef __cplusplus
}
#endif


#endif /* _ETH_H_ */
