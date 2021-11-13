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
#include <FreeRTOS.h>
#include <list.h>
#include <FreeRTOS_IP.h>
#include <FreeRTOS_sockets.h>
#include <components.h>
#include <can_lld_cfg.h>

#include <network.h>

#include <candefs.h>
#include <lindefs.h>
#include <ethdefs.h>


#define LOG_ENABLED 1
#include <log.h>


/*
 * The LinFlex device used as master.
 */
#define LIN_REMOTE                      LD13


/*
 * The CAN device.
 */
#define CAN_DEVICE                      CAND7


#define ETH_RECV_TASK_PRIORITY          2
#define ETH_RECV_TASK_STACK_SIZE        1024

#define REMOTE_DEVICE_TASK_PRIORITY     2
#define REMOTE_DEVICE_TASK_STACK_SIZE   1024

/* Ethernet socket used to send/receive data to/from gateway */
static Socket_t gateway_socket = FREERTOS_INVALID_SOCKET;

portTASK_FUNCTION(ethRecvTask, pvParam ) {

	  (void)pvParam;
	  uint8_t buf[16];
	  int32_t n;

	  for( ;; ) {
	    n = FreeRTOS_recv(gateway_socket, buf, 16, 0);

	    if (n > 0) {
	      LOG_ETH_MSG(0, (uint8_t)'R', buf[0], &buf[1], n - 1);
	    } else if (n == 0) {
	    } else {
	      FreeRTOS_shutdown(gateway_socket, FREERTOS_SHUT_RDWR);
	      break;
	    }
	  }

	  /* Destroy this task */
	  vTaskDelete(NULL);
	}


static void eth_send_message(uint8_t *data, uint16_t len) {
  int32_t n;
  int32_t p;

  p = n = 0;
  while (len != 0) {
    n = FreeRTOS_send(gateway_socket, &data[p], len, 0);
    p += n;
    len -= n;
	if (n < 0) {
	  /* Error: stop sending data */
	  break;
	}
  }
}


static int32_t eth_start_client(void) {

  int32_t ret, timeout;
  uint32_t address;
  struct freertos_sockaddr gateway_address;

  ret = 1;

  /* Create a socket. */
  gateway_socket = FreeRTOS_socket(FREERTOS_AF_INET,
                                   FREERTOS_SOCK_STREAM,
                                   FREERTOS_IPPROTO_TCP);

  /* Decode IP address */
#if (SPC5_USE_ETH0 == TRUE)
  address = FreeRTOS_inet_addr(SPC5_ETH0_IP_ADDR);
#else
  address = FreeRTOS_inet_addr(SPC5_ETH1_IP_ADDR);
#endif

  if (gateway_socket != FREERTOS_INVALID_SOCKET) {
    /*
     * Set the IP address and port of the gateway
     * socket to which this client socket will transmit.
     */
    gateway_address.sin_port = FreeRTOS_htons(ETH_PORT);
    gateway_address.sin_addr = address;
    gateway_address.sin_len = sizeof(gateway_address);

    	/* Connect to the gateway socket. */
    timeout = 1000000;
    do {

      /* Let the gateway task to start the Ethernet server */
      taskYIELD();

      ret = FreeRTOS_connect(gateway_socket, &gateway_address, sizeof(gateway_address));
    } while ((ret != 0) && (timeout-- != 0));

    if (ret == 0) {
      ret = xTaskCreate(ethRecvTask,
                        "ethRecvTask",
                        ETH_RECV_TASK_STACK_SIZE,
                        NULL,
                        ETH_RECV_TASK_PRIORITY,
                        NULL);

      ret = (ret == pdPASS) ? 0 : 1;
    }

    if (ret != 0) {
      /* Not able to connect to Ethernet gateway server */
      FreeRTOS_closesocket(gateway_socket);
    }
  }

  /* Return pdPASS on success, pdFAIL on error */
  return (ret == 0) ? pdPASS : pdFAIL;
}

/*
 * CAN
 */
void SUBSYS_1_CAN1_rx_callback(uint32_t msg_buf, CANRxFrame can_msg) {
  (void)msg_buf;
  (void)can_msg;
}

static void can_send_message(uint32_t msg_id, uint32_t data0, uint32_t data1) {
  CANTxFrame can_msg;

  can_msg.TYPE = CAN_ID_STD;
  can_msg.ID = msg_id;
  can_msg.DLC = 8U;
  can_msg.data32[0] = data0;
  can_msg.data32[1] = data1;

  can_lld_transmit(&CAN_DEVICE, CAN_ANY_TXBUFFER, &can_msg);
}

portTASK_FUNCTION(remoteDevTask, pvParam ) {
  (void)pvParam;

  uint8_t lin_tx_buf[LIN_DATA_LEN];
  uint8_t lin_rx_buf[LIN_DATA_LEN];

  uint8_t eth_tx_buf[ETH_HEADER_LEN + ETH_DATA_LEN];

  uint8_t counter;
  uint32_t i;
  uint32_t can_data[2];
  uint32_t eth_ok;

  /* Start logging */
  log_start();

  /* Start LIN Master */
  lin_lld_start(&LIN_REMOTE, &lin_config_lin_remote);

  /* Start CAN */
  can_lld_start(&CAN_DEVICE, &can_config_SUBSYS_1_CAN1);

  /* Start Ethernet connection */
  eth_ok = eth_start_client();

  for (i = 0; i < LIN_DATA_LEN; i++) {
    lin_tx_buf[i] = (uint8_t)'0' + i;
  }

  counter = 1;

  for ( ; ; ) {

    if ((counter & 0x01) != 0) {
      /* Send LIN_MSG_2 */
      lin_lld_transmit(&LIN_REMOTE, LIN_MSG_2, lin_tx_buf, LIN_MSG_2_SIZE);
      LOG_LIN_MSG(LIN_REMOTE.config->mode, 'T', LIN_MSG_2, lin_tx_buf, LIN_MSG_2_SIZE);
    }

    if ((counter & 0x01) != 0) {
      /* Send CAN_MSG_1 */
      can_data[0] = counter;
      can_data[1] = 0x11223344;
      can_send_message(CAN_MSG_1, can_data[0], can_data[1]);
      LOG_CAN_MSG(0, 'T', CAN_MSG_1, can_data, 8);
    }

    if ((counter & 0x01) != 0) {
      /* Send CAN_MSG_4 */
      can_data[0] = counter;
      can_data[1] = 0xAABBCCDD;
      can_send_message(CAN_MSG_4, can_data[0], can_data[1]);
      LOG_CAN_MSG(0, 'T', CAN_MSG_4, can_data, 8);
    }

    if ((counter & 0x08) != 0) {
      /* Send CAN_MSG_5 */
      can_data[0] = counter;
      can_data[1] = 0x55667788;
      can_send_message(CAN_MSG_5, can_data[0], can_data[1]);
      LOG_CAN_MSG(0, 'T', CAN_MSG_5, can_data, 8);
    }

    if ((counter & 0x10) != 0) {
        /* Send LIN_MSG_3 */
        lin_lld_transmit(&LIN_REMOTE, LIN_MSG_3, lin_tx_buf, LIN_MSG_3_SIZE);
        LOG_LIN_MSG(LIN_REMOTE.config->mode, (uint8_t)'T', LIN_MSG_3, lin_tx_buf, LIN_MSG_3_SIZE);
    }

    if ((counter & 0x08) != 0) {
        /* Send LIN_MSG_4 */
        lin_lld_transmit(&LIN_REMOTE, LIN_MSG_4, lin_tx_buf, LIN_MSG_4_SIZE);
        LOG_LIN_MSG(LIN_REMOTE.config->mode, (uint8_t)'T', LIN_MSG_4, lin_tx_buf, LIN_MSG_4_SIZE);
    }

    if ((counter & 0x04) != 0) {
    	/* Receive data from slave on LIN_MSG_1 */
        lin_lld_receive(&LIN_REMOTE, LIN_MSG_1, lin_rx_buf, LIN_MSG_1_SIZE);
        LOG_LIN_MSG(LIN_REMOTE.config->mode, (uint8_t)'R', LIN_MSG_1, lin_rx_buf, LIN_MSG_1_SIZE);
    }


    /* Ethernet */
    if (eth_ok == pdPASS) {

      if ((counter & 0x08) != 0) {
        /* Send ETH_MSG_1 */

        eth_tx_buf[0] = ETH_MSG_1;
        for (i = 1; i <= ETH_DATA_LEN; i++) {
          eth_tx_buf[i] = (uint8_t)'0' + i;
        }

        eth_send_message(eth_tx_buf, ETH_DATA_LEN + ETH_HEADER_LEN);
        LOG_ETH_MSG(0, (uint8_t)'T', ETH_MSG_1, eth_tx_buf, 8);
      }

      if ((counter & 0x10) != 0) {
    	/* Send ETH_MSG_2 */

        eth_tx_buf[0] = ETH_MSG_2;
        for (i = 1; i <= ETH_DATA_LEN; i++) {
          eth_tx_buf[i] = (uint8_t)'A' + i;
        }

        eth_send_message(eth_tx_buf, ETH_DATA_LEN + ETH_HEADER_LEN);
        LOG_ETH_MSG(0, (uint8_t)'T', ETH_MSG_2, eth_tx_buf, 8);
      }

      if ((counter & 0x01) != 0) {
    	/* Send ETH_MSG_3 */

        eth_tx_buf[0] = ETH_MSG_3;
        for (i = 1; i <= ETH_DATA_LEN; i++) {
          eth_tx_buf[i] = (uint8_t)'a' + i;
        }
        eth_send_message(eth_tx_buf, ETH_DATA_LEN + ETH_HEADER_LEN);
        LOG_ETH_MSG(0, (uint8_t)'T', ETH_MSG_3, eth_tx_buf, 8);
      }
    }

    counter++;
    vTaskDelay(500);
  }
}


uint32_t remote_start(void) {

  uint32_t ret;

  /* create remote device task */
  ret = xTaskCreate(remoteDevTask, "remoteDevTask", REMOTE_DEVICE_TASK_STACK_SIZE, NULL, REMOTE_DEVICE_TASK_PRIORITY, NULL);

  return (ret == pdPASS) ? 0 : 1;
}
