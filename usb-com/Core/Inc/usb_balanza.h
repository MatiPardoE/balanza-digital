/*
 * usb_balanza.h
 *
 *  Created on: 24 oct. 2021
 *      Author: hernan
 */

#include "usbd_cdc_if.h"
#include "string.h"

#ifndef INC_USB_BALANZA_H_
#define INC_USB_BALANZA_H_

#define INIT_CMD    0
#define RX_CMD      1
#define RX_CEN      2
#define RX_DEC      3
#define RX_UNI      4
#define END_CMD     5

uint16_t(*hook_rx)(uint8_t *b, uint16_t len);
extern USBD_HandleTypeDef hUsbDeviceFS;

extern uint16_t weight_qt;
extern uint8_t flag_weight_qt;

void usb_balanza_init();
void handle_serial(uint8_t b);
uint16_t serial_rx(uint8_t *buf, uint16_t len);
void serial_tx_num(uint16_t num);
void serial_tx_cmd(const char* cmd);

#endif /* INC_USB_BALANZA_H_ */
