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
#include <stdio.h>
#include <limits.h>
#include <FreeRTOS.h>
#include <list.h>
#include <queue.h>
#include <FreeRTOS_IP.h>
#include <FreeRTOS_sockets.h>

#include <can.h>
#include <lindefs.h>
#include <lin_master.h>
#include <lin_slave.h>
#include <eth.h>
#include <gateway.h>
#include <routing.h>

#define ENABLE_GATEWAY_LOG  1
#define LOG_BUFFER_LEN      64

#if (ENABLE_GATEWAY_LOG == 1)
#define LOG_MESSAGE(_action_, _id_, _dst_, _len_, _data_) \
        do { \
            uint8_t __i; \
            static char __buf[LOG_BUFFER_LEN]; \
            uint8_t __n; \
            char *__ptr = __buf; \
            int32_t __s = sizeof(__buf); \
            for (__i = 0; __i < (_len_); __i++) { \
                __n = snprintf(__ptr, __s, " 0x%02x ", (_data_)[__i]); \
                __ptr += __n; \
                __s -= __n; \
                if (__s <= 0) { \
                	break; \
                } \
            } \
            FreeRTOS_printf(("%s - src = 0x%04x, dst = 0x%04x, len = %d, data = %s\n", (_action_), (_id_), (_dst_), (_len_), __buf)); \
        } while (0)

#else
#define LOG_MESSAGE(_action_, _id_, _dst_, _len_, _data_) do {} while(0)
#endif

#define GATEWAY_TASK_PRIORITY     2
#define GATEWAY_TASK_STACK_SIZE   1024


#define	GATEWAY_QUEUE_SIZE        32
#define	GATEWAY_QUEUE_ITEM_SIZE   sizeof(GatewayMsg_t)


static QueueHandle_t gatewayQueue;
static TaskHandle_t gatewayTaskHandle;


void send_to_gateway_from_ISR(GatewayMsg_t *gw_msg) {

  /* Add message to gateway data queue. */
  xQueueSendToBackFromISR(gatewayQueue, gw_msg, 0);

  /* Notify gateway task of a new message. */
  xTaskNotifyFromISR(gatewayTaskHandle, 0, eSetBits, NULL);
}

void send_to_gateway_from_task(GatewayMsg_t *gw_msg) {

  taskENTER_CRITICAL();
  /* Add message to gateway data queue. */
  xQueueSendToBack(gatewayQueue, gw_msg, 0);

  /* Notify gateway task of a new message. */
  xTaskNotify(gatewayTaskHandle, 0, eSetBits);
  taskEXIT_CRITICAL();
}


portTASK_FUNCTION(gatewayTask, pvParam) {

  (void)pvParam;
  uint32_t ret;

  GatewayMsg_t gw_msg;
  RoutingTable_t *table;
  uint32_t action;
  uint32_t dst;
  void *device;

  /* Start LIN slave */
  lin_slave_start();

  /* Start LIN master */
  lin_master_start();

  /* Start CAN */
  can_start();

  /* Start ETH server */
  eth_start();

  for ( ; ; ) {

    /* Wait for a new message to route */
    ret = xTaskNotifyWait(0x0, ULONG_MAX, NULL, portMAX_DELAY);

    if (ret == pdTRUE) {

      /* Get the gateway message to process */
      ret = xQueueReceive(gatewayQueue, &gw_msg, 10);
      if (ret == pdTRUE) {

        /* Get routing table for this message type (LIN, CAN, ETH) and message id */
        table = get_routing_table(gw_msg.type, gw_msg.id);

        if (table != NULL) {

          /* Get action to apply to this message */
          action = get_action(table);

          /* Get device destination handler */
          device = get_destination_device(table);

          /* Get destination id */
          dst = get_destination_message_id(table);

          if ((device != NULL) && (dst != INVALID_DESTINATION)) {

            /* ready to route data */
            switch (action) {

            case LIN_TO_CAN:
              LOG_MESSAGE("LIN TO CAN", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_can(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case LIN_TO_ETH:
              LOG_MESSAGE("LIN TO ETH", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_eth(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case LIN_TO_LIN:
              LOG_MESSAGE("LIN TO LIN", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_lin(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case CAN_TO_LIN:
              LOG_MESSAGE("CAN TO LIN", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_lin(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case CAN_TO_ETH:
              LOG_MESSAGE("CAN TO ETH", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_eth(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case CAN_TO_CAN:
              LOG_MESSAGE("CAN TO CAN", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_can(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case ETH_TO_CAN:
              LOG_MESSAGE("ETH TO CAN", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_can(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case ETH_TO_LIN:
              LOG_MESSAGE("ETH TO LIN", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_lin(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            case ETH_TO_ETH:
              LOG_MESSAGE("ETH TO ETH", gw_msg.id, dst, gw_msg.data_len, gw_msg.data);
              send_to_eth(device, dst, gw_msg.data, gw_msg.data_len);
              break;

            default:
              break;
            }
          }
        }
      }
    }
  }
}


uint32_t gateway_start(void) {

  uint32_t ret;

  gatewayQueue = xQueueCreate(GATEWAY_QUEUE_SIZE, GATEWAY_QUEUE_ITEM_SIZE);

  /* create gateway task */
  ret = xTaskCreate(gatewayTask, "gatewayTask", GATEWAY_TASK_STACK_SIZE, NULL, GATEWAY_TASK_PRIORITY, &gatewayTaskHandle);

  return (ret == pdPASS) ? 0 : 1;
}

