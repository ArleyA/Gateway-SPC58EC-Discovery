/****************************************************************************
*
* Copyright © 2017-2019 STMicroelectronics - All Rights Reserved
*
* This software is licensed under SLA0098 terms that can be found in the
* DM00779817_1_0.pdf file in the licenses directory of this software product.
* 
* THIS SOFTWARE IS DISTRIBUTED "AS IS," AND ALL WARRANTIES ARE DISCLAIMED, 
* INCLUDING MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
*
*****************************************************************************/

/* Inclusion of the main header files of all the imported components in the
   order specified in the application wizard. The file is generated
   automatically.*/
#include <components.h>
#include <gateway.h>
#include <led.h>
#include <remote.h>
#include "FreeRTOS.h"
#include "task.h"

/*
 * Hook for FreeRTOS idle task.
 */
void vApplicationIdleHook(void) {
	TOGGLE_LED_HB();
}



/*
 * Application entry point.
 */
int main(void) {


	uint32_t error;

  /* Initialization of all the imported components in the order specified in
     the application wizard. The function is generated automatically.*/
  componentsInit(); //-->component.c

  /* Enable Interrupts */
  irqIsrEnable(); //-->irq.c

  /* Start the gateway task */
  error = gateway_start(); //-->gateway.c

  if (!error) {
    /* Start the remote device emulation task */
    error = remote_start(); //-->remote.c
  }

  if (!error) {

    /* Start all tasks */
    vTaskStartScheduler();
  }


  /* If all is well, the scheduler will now be running, and the following
     line will never be reached.  If the following line does execute, then
     there was insufficient FreeRTOS heap memory available for the idle and/or
     timer tasks     to be created.  See the memory management section on the
     FreeRTOS web site for more details.  http://www.freertos.org/a00111.html */
  for( ;; ) {}

  return 0;
}
