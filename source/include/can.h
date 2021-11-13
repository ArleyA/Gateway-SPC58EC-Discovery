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
#ifndef _CAN_H_
#define _CAN_H_

#include <platform.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void can_start(void);

void send_to_can(void *device, uint32_t msg_id, uint8_t *data, uint16_t len);

#ifdef __cplusplus
}
#endif


#endif /* _CAN_H_ */
