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
#ifndef _LED_H_
#define _LED_H_

#include <components.h>

#define TOGGLE_LED_TX()   pal_lld_togglepad(PORT_LED1, LED1)
#define TOGGLE_LED_RX()   pal_lld_togglepad(PORT_LED2, LED2)
#define TOGGLE_LED_HB()   pal_lld_togglepad(PORT_LED3, LED3)


#endif /* _LED_H_ */
