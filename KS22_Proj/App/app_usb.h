#ifndef __USB_APP_H__
#define __USB_APP_H__
#include "syscfg.h"
#include "rl_usb.h"

//void usb_send_buf(uint8_t * buf,u16 len)
#define 	usb_send_buf	USBD_CDC_ACM_DataSend

void usb_recv_proc(uint8_t * buf,uint16_t len);	//USB接收数据处理函数
void usb_app_init(void);


#endif




