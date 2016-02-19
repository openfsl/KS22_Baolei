#include "app_usb.h"
#include "bsp_can.h"

void ack_version(void);
void can_rx_to_usb(int num,str_can_msg* msg);
void can_tx_from_usb(u8 * buf,u8 len);

void usb_recv_proc(uint8_t * buf,uint16_t len)	//USB接收数据处理函数
{
	//usb_send_buf(buf, len);	//将收到的数据 发送回去
	
	if(buf[0] != 0xFA)	return;		//成帧判断

	if(buf[3] == 0x00)
	{
		if(buf[4] == 0x02)					//读取版本号
		{
			ack_version();
		}
	}
	
	else if(buf[3] == 0x01)
	{
		if(buf[4] == 0x03)					//CAN 发送
		{
			can_tx_from_usb(&buf[5],len-5);
		}
		else if(buf[4] == 0x05)
		{
			//can_cfg_from_usb(&Buf[5],Len-5);
		}
		else if(buf[4] == 0x06)				//读取配置参数
		{
			//can_read_cfg_to_usb(Buf[5]);
		}
	}
}

void usb_app_init()
{
	usbd_init();
    usbd_connect(__TRUE);
  
    //while (!usbd_configured ());          /* Wait for device to configure */
	//printf("usbd configure complete\r\n");
}

void ack_version()
{
	u8 buf[25];
	u8 i = 0;
	buf[i++] = 0xFA;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0;		//DEVICE_CLASS_CDC
	buf[i++] = 0x02;
	buf[i++] = 0x01;	//V1.0
	buf[i++] = 0x00;
	buf[i++] = 0xFD;
	
	buf[2] = i;
	usb_send_buf(buf,i);
}

void can_rx_to_usb(int num,str_can_msg* msg)	//将CAN收到的数据发送到USB
{
	u32 id;
	u8 buf[35];
	
	u8 j,i = 0;
	buf[i++] = 0xFA;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0x01;		//DEVICE_CLASS_CDC
	buf[i++] = 0x03;
	
	buf[i++] = num;			//通道
	buf[i++] = msg->IDE;
	buf[i++] = msg->RTR;
	
	id = msg->ID;
	buf[i++] = id>>24;
	buf[i++] = id>>16;
	buf[i++] = id>>8;
	buf[i++] = id;
	buf[i++] = msg->DLC;
	for(j=0;j<msg->DLC;j++)
	{
		buf[i++] = msg->Data[j];
	}
	buf[i++] = 0xFD;
	
	buf[2] = i;
	usb_send_buf(buf,i);
}

void can_tx_from_usb(u8 * buf,u8 len)	//将从USB收到的数据通过CAN发送出去
{
	int i,ch;
	str_can_msg msg;
	if(len < 9)	return;
	
	ch = buf[0];
	msg.IDE = buf[1];
	msg.RTR = buf[2];
	msg.ID = (buf[3]<<24)|(buf[4]<<16)|(buf[5]<<8)|(buf[6]);
	msg.DLC = buf[7];
	for(i=0;i<msg.DLC;i++)
	{
		msg.Data[i] = buf[8+i];
	}
	
	if(ch == 0)	can_send_msg(CAN0,3,&msg);
	else		can_send_msg(CAN0,3,&msg);
}