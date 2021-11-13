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
#ifndef _LOG_H_
#define _LOG_H_

#include <platform.h>


#if (LOG_ENABLED == 1)
#define LOG_LIN_MSG(_mode_, _sender_, _msgid_, _buffer_, _size_) \
	do { \
		vTaskSuspendAll(); \
		write_lin_msg((_mode_), (uint8_t)(_sender_), (_msgid_), (_buffer_), (_size_)); \
		xTaskResumeAll(); \
	} while (0)

#define LOG_CAN_MSG(_mode_, _sender_, _msgid_, _buffer_, _size_) \
	do { \
		vTaskSuspendAll(); \
    	write_can_msg((_mode_), (uint8_t)(_sender_), (_msgid_), (uint8_t *)(_buffer_), (_size_)); \
		xTaskResumeAll(); \
	} while (0)

#define LOG_ETH_MSG(_mode_, _sender_, _msgid_, _buffer_, _size_) \
	do { \
		vTaskSuspendAll(); \
		write_eth_msg((_mode_), (uint8_t)(_sender_), (_msgid_), (_buffer_), (_size_)); \
		xTaskResumeAll(); \
	} while (0)

#else
#define LOG_LIN_MSG(_mode_, _sender_, _msgid_, _buffer_, _size_) do {} while(0)
#define LOG_CAN_MSG(_mode_, _sender_, _msgid_, _buffer_, _size_) do {} while(0)
#define LOG_ETH_MSG(_mode_, _sender_, _msgid_, _buffer_, _size_) do {} while(0)
#endif


#ifdef __cplusplus
extern "C" {
#endif

extern void log_start(void);

extern void write_lin_msg(uint8_t sender, uint8_t direction, uint32_t msg_id, uint8_t *msg, uint16_t len);
extern void write_can_msg(uint8_t sender, uint8_t direction, uint32_t msg_id, uint8_t *msg, uint16_t len);
extern void write_eth_msg(uint8_t sender, uint8_t direction, uint32_t msg_id, uint8_t *msg, uint16_t len);

#ifdef __cplusplus
}
#endif


#endif /* _LOG_H_ */
