#include "bsp_can.h"

str_can_rx can0_rx,can1_rx;

void app_can_init()
{
	can0_rx.mb = 1;
	
	str_mb_mbopt opt1 =
	{
		.format = CAN_ID_FORMAT_EXT,
		.type = CAN_ID_TYPE_DATA,
		.id = 0,
		.mask = 0
	};
	
	str_mb_mbopt opt2 =
	{
		.format = CAN_ID_FORMAT_STD,
		.type = CAN_ID_TYPE_DATA,
		.id = 0,
		.mask = 0
	};
	
	can_gpio_init();
	
	bsp_can_init(CAN0,BAUD500K);
	//can_mb_cfg(CAN0,can0_rx.mb,&opt);
	can_mb_cfg(CAN0,0,&opt1);		//邮箱0和1分别接收标准帧和扩展帧
	can_mb_cfg(CAN0,1,&opt2);
	//can_it_cfg(CAN0,can0_rx.mb);
	can_it_cfg(CAN0,0);
	can_it_cfg(CAN0,1);
	bsp_can_enable(CAN0);

	bsp_can_init(CAN1,BAUD500K);
	//can_mb_cfg(CAN0,can0_rx.mb,&opt);
	can_mb_cfg(CAN1,0,&opt1);		//邮箱0和1分别接收标准帧和扩展帧
	can_mb_cfg(CAN1,1,&opt2);
	//can_it_cfg(CAN0,can0_rx.mb);
	can_it_cfg(CAN1,0);
	can_it_cfg(CAN1,1);
	bsp_can_enable(CAN1);
	
	str_can_msg txmsg = 
	{
		.ID = 0x123,
		.IDE = CAN_ID_FORMAT_EXT,
		.RTR = CAN_ID_TYPE_DATA,
		.DLC = 8,
		.Data[0] = 1,
		.Data[1] = 2,
		.Data[2] = 3,
		.Data[3] = 4,
		.Data[4] = 5,
		.Data[5] = 6,
		.Data[6] = 7,
		.Data[7] = 8,
	};
	can_send_msg(CAN0,3,&txmsg);
}

void can_msg_proc(str_can_msg * msg)
{
	u8 i;
	printf("RXID is %x , DLC is %d ",msg->ID, msg->DLC);
	printf("IDE is : ");
	if(msg->IDE) 	printf("EXT, Data is :\r\n");
	else 			printf("STD, Data is :\r\n");
	
	for(i=0;i<msg->DLC;i++)
	{
		printf("0x%x, ",msg->Data[i]);
	}
	printf("\r\n---------------------\r\n");
}

extern void can_rx_to_usb(int num,str_can_msg* msg);
void can_rx_chk()
{
	if(can0_rx.rx_flag)
	{
		can0_rx.rx_flag = 0;
		can_rx_to_usb(0,&can0_rx.rx_msg);
		//can_msg_proc(&can0_rx.rx_msg);
	}
	
	if(can1_rx.rx_flag)
	{
		can1_rx.rx_flag = 0;
		can_rx_to_usb(1,&can1_rx.rx_msg);
		//can_msg_proc(&can0_rx.rx_msg);
	}
}