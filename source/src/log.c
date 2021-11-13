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
#include <string.h>
#include <components.h>
#include <log.h>


/* Serial device used for logging */
#define SERIAL_DEVICE SD5


static void write_msg (char *msg, uint16_t len) {
  while (sd_lld_write(&SERIAL_DEVICE, (uint8_t *)msg, len) == SERIAL_MSG_WAIT){}
}

static void write_new_line(void) {
  char nl[2];
  nl[0] = '\n';
  nl[1] = '\r';
  write_msg (nl, 2);
}


void log_start(void) {
  /* Start serial */
  sd_lld_start(&SERIAL_DEVICE, NULL);
}

void write_lin_msg(uint8_t sender, uint8_t direction, uint32_t msg_id, uint8_t *msg, uint16_t len) {
  static char buf[80];
  char *p;
  int32_t i, x, s;

  memset(buf, ' ', sizeof(buf));
  snprintf(buf, 64, "LIN:  %c  %c  0x%02x    (%d)",
              ((sender == (uint8_t)LIN_MODE_MASTER) ? 'M' : 'S'),
			  direction,
			  (unsigned char)msg_id,
			  (int)len);

  p = &buf[26];
  s = 80 - 26;
  for (x = i = 0; i < len; i++) {
    x = snprintf(p, s, "0x%02x ", msg[i]);
    p += x;
    s -= x;
    if (s <= 0) {
    	break;
    }
  }

  write_msg(buf, sizeof(buf));
  write_new_line();
}


void write_can_msg(uint8_t sender, uint8_t direction, uint32_t msg_id, uint8_t *msg, uint16_t len) {
  static char buf[80];
  char *p;
  int32_t i, x, s;

  (void)sender;

  memset(buf, ' ', sizeof(buf));
  snprintf(buf, 64, "CAN:     %c  0x%04x  (%d)",
			  direction,
			  (unsigned short)msg_id,
			  (int)len);

  p = &buf[26];
  s = 80 - 26;
  for (x = i = 0; i < len; i++) {
    x = snprintf(p, s, "0x%02x ", msg[i]);
    p += x;
    s -= x;
    if (s <= 0) {
    	break;
    }
  }

  write_msg(buf, sizeof(buf));
  write_new_line();
}

void write_eth_msg(uint8_t sender, uint8_t direction, uint32_t msg_id, uint8_t *msg, uint16_t len) {
  static char buf[80];
  char *p;
  int32_t i, x, s;

  (void)sender;

  memset(buf, ' ', sizeof(buf));
  snprintf(buf, 64, "ETH:     %c  0x%02x    (%d)",
			  direction,
			  (unsigned short)msg_id,
			  (int)len);

  p = &buf[26];
  s = 80 - 26;
  for (x = i = 0; i < len; i++) {
    x = snprintf(p, s, "0x%02x ", msg[i]);
    p += x;
    s -= x;
    if (s <= 0) {
    	break;
    }
  }

  write_msg(buf, sizeof(buf));
  write_new_line();
}
