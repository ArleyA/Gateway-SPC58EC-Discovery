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
#ifndef _LINDEFS_H_
#define _LINDEFS_H_

#include <platform.h>

/*
 * Used by master to read 4 bytes from slave.
 *
 * Filter is set in slave node, so for this message
 * direction is Transmit (from a slave point of view).
 */
#define LIN_MSG_1      0x25
#define LIN_MSG_1_SIZE    4

/*
 * Used by master to send 4 bytes to slave.
 *
 * Filter is set in slave node, so for this message
 * direction is Receive (from a slave point of view).
 */
#define LIN_MSG_2      0x26
#define LIN_MSG_2_SIZE    4

/*
 * Used by master to send 8 bytes to slave.
 *
 * Filter is set in slave node, so for this message
 * direction is Receive (from a slave point of view).
 */
#define LIN_MSG_3      0x27
#define LIN_MSG_3_SIZE    8

/*
 * Used by master to send 8 bytes to slave.
 *
 * Filter is set in slave node, so for this message
 * direction is Receive (from a slave point of view).
 */
#define LIN_MSG_4      0x28
#define LIN_MSG_4_SIZE    8


#endif /* _LINDEFS_H_ */
