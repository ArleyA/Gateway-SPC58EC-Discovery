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
#include <FreeRTOS.h>
#include <list.h>
#include <FreeRTOS_IP.h>
#include <FreeRTOS_sockets.h>

#include <ethdefs.h>
#include <gateway.h>


#define ACCEPT_TASK_PRIORITY         2
#define ACCEPT_TASK_STACK_SIZE       1024

#define ETH_SERVER_TASK_PRIORITY     2
#define ETH_SERVER_TASK_STACK_SIZE   1024

static TaskHandle_t acceptTaskHandle;

uint32_t ETHD1 = 0;

portTASK_FUNCTION(eth_server, pvParam) {

  Socket_t eth_socket;
  GatewayMsg_t gw_msg;
  uint8_t buf[GATEWAY_MSG_DATA_LEN+1];
  int32_t n;

  /* Set the gateway socket */
  ETHD1 = (uint32_t)pvParam;
  eth_socket = (Socket_t)pvParam;


  for ( ; ; ) {
    n = FreeRTOS_recv(eth_socket, buf, 16, 0);

    if (n > 0) {
      /*
       * Silently failing if message length is
       * greater than gateway message data length.
       */
      if (n <= GATEWAY_MSG_DATA_LEN) {
        gw_msg.type = GATEWAY_MSG_ETH;
        gw_msg.id   = buf[0];
   	    memcpy(gw_msg.data, &buf[1], n - 1);
   	    gw_msg.data_len = n - 1;

   	    send_to_gateway_from_task(&gw_msg);
   	  }
    } else if (n == 0) {

    } else {
      FreeRTOS_shutdown(eth_socket, FREERTOS_SHUT_RDWR);
      break;
    }
  }

  /* Destroy this task */
  vTaskDelete(NULL);
}

portTASK_FUNCTION(acceptTask, pvParam) {

  Socket_t listening_socket = (Socket_t)pvParam;
  Socket_t connected_socket;
  struct freertos_sockaddr connected_client;
  socklen_t connected_client_size = sizeof(connected_client);

  for( ; ; ) {
    /* Wait for incoming connections. */
    connected_socket = FreeRTOS_accept(listening_socket, &connected_client, &connected_client_size);

    /* Create RTOS task to handle the connection. */
    xTaskCreate(eth_server,
                "serverTask",
                ETH_SERVER_TASK_STACK_SIZE,
	            (void *) connected_socket,
                ETH_SERVER_TASK_PRIORITY,
                NULL);
  }
}

void eth_start(void) {

  struct freertos_sockaddr bindAddress;
  Socket_t listening_socket;
  static const TickType_t listening_socket_accept_timeout = portMAX_DELAY;
  const BaseType_t childSockets = 5;
  uint32_t ret;

  listening_socket = FreeRTOS_socket(FREERTOS_AF_INET,
                                     FREERTOS_SOCK_STREAM,
                                     FREERTOS_IPPROTO_TCP);

  /* Check the socket was created successfully. */
  if (listening_socket != FREERTOS_INVALID_SOCKET) {

    /* Set a time out so accept() will just wait for a connection. */
    FreeRTOS_setsockopt(listening_socket,
                        0,
                        FREERTOS_SO_RCVTIMEO,
                        &listening_socket_accept_timeout,
                        sizeof(listening_socket_accept_timeout));

    /* Set the listening port to ETH_PORT. */
    bindAddress.sin_port = FreeRTOS_htons(ETH_PORT);

    if (FreeRTOS_bind(listening_socket, &bindAddress, sizeof(&bindAddress)) != 0) {
      FreeRTOS_closesocket(listening_socket);
    } else {
      FreeRTOS_listen(listening_socket, childSockets);

      /* create accept task */
      ret = xTaskCreate(acceptTask,
                        "acceptTask",
                        ACCEPT_TASK_STACK_SIZE,
						(void *) listening_socket,
                        ACCEPT_TASK_PRIORITY,
                        &acceptTaskHandle);
      if (ret != pdPASS) {
          FreeRTOS_closesocket(listening_socket);
      }
    }
  }
}


void send_to_eth(void *device, uint8_t msg_id, uint8_t *buffer, uint16_t len) {
  int32_t n;
  char data[20];
  Socket_t eth_socket = *((Socket_t *)device);


  if (eth_socket != FREERTOS_INVALID_SOCKET) {
    n = 0;
    data[0] = msg_id;
    memcpy(&data[1], buffer, len);
    len++;
    while (n < len) {
      n = FreeRTOS_send(eth_socket, &data[n], len - n, 0);
	  if (n < 0) {
	    /* Error: stop sending data */
	    break;
	  }
    }
  }
}
