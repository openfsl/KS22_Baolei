#ifndef __BSP_CAN_H__
#define __BSP_CAN_H__

#include "syscfg.h"

#if !defined(ABS)
    #define ABS(a)         (((a) < 0) ? (-(a)) : (a))
#endif

#define CAN_MB_MAX			16

#define CAN_RX_MB			2		//CAN接收邮箱使用2个 0和1

#define CAN_GET_MB_CODE(cs)         (((cs) & CAN_CS_CODE_MASK)>>CAN_CS_CODE_SHIFT)
#define CAN_GET_FRAME_LEN(cs)       (((cs) & CAN_CS_DLC_MASK)>>CAN_CS_DLC_SHIFT)
#define CAN_GET_IDE(cs)				(((cs) & CAN_CS_IDE_MASK)>>CAN_CS_IDE_SHIFT)
#define CAN_GET_RTR(cs)				(((cs) & CAN_CS_RTR_MASK)>>CAN_CS_RTR_SHIFT)

#define CAN_GET_STD_ID(id)			(((id) & CAN_ID_STD_MASK)>>CAN_ID_STD_SHIFT)
#define CAN_GET_EXT_ID(id)			(((id) & CAN_ID_EXT_MASK)>>CAN_ID_EXT_SHIFT)

typedef enum
{
	CAN_ID_FORMAT_STD = 0,
	CAN_ID_FORMAT_EXT = 1,
}CAN_ID_FORMAT;

typedef enum
{
	CAN_ID_TYPE_DATA   = 0,
	CAN_ID_TYPE_REMOTE = 1,
}CAN_ID_TYPE;

typedef enum
{
	BAUD1M   = 1000000,
	BAUD500K =  500000,
	BAUD125K =  125000,
	BAUD100K =  100000,
}CANBaud;

typedef struct 
{
	CAN_ID_FORMAT 	format;		//CAN ID format
	CAN_ID_TYPE 	type;		//CAN ID Type
	u32 id;
	u32 mask;
}str_mb_mbopt;

//#define CAN_ID_STD                  0x00000000           
//#define CAN_ID_EXT                  0x00000004

#define CAN_RTR_DATA                0x00000000         
#define CAN_RTR_REMOTE              0x00000002

typedef struct
{
  u32 			 ID;  				//0 to 0x7FF or 0 to 0x1FFFFFFF
  CAN_ID_FORMAT	 IDE; 
  u8			 RTR; 
  u8			 DLC;     			//len
  u8			 Data[8];  			//数据
	
  u16			 time_stamp;		//时间戳  
                            
}str_can_msg;

typedef struct
{
	u8 mb;					//rx 使用的邮箱
	volatile u8 rx_flag;	//rx 收到数据标志
	str_can_msg rx_msg;
}str_can_rx;

extern str_can_rx can0_rx,can1_rx;

/* CAN MB Type */
typedef enum
{
    kFlexCanTX_Inactive  = 0x08, 	/*!< MB is not active.*/
    kFlexCanTX_Abort     = 0x09, 	/*!< MB is aborted.*/
    kFlexCanTX_Data      = 0x0C, 	/*!< MB is a TX Data Frame(MB RTR must be 0).*/
    kFlexCanTX_Remote    = 0x1C, 	/*!< MB is a TX Remote Request Frame (MB RTR must be 1).*/
    kFlexCanTX_Tanswer   = 0x0E, 	/*!< MB is a TX Response Request Frame from.*/
																	/*!  an incoming Remote Request Frame.*/
    kFlexCanTX_NotUsed   = 0xF,  	/*!< Not used*/
    kFlexCanRX_Inactive  = 0x0, 	/*!< MB is not active.*/
    kFlexCanRX_Full      = 0x2, 	/*!< MB is full.*/
    kFlexCanRX_Empty     = 0x4, 	/*!< MB is active and empty.*/
    kFlexCanRX_Overrun   = 0x6, 	/*!< MB is overwritten into a full buffer.*/
    //kFlexCanRX_Busy      = 0x8, /*!< FlexCAN is updating the contents of the MB.*/
																	/*!  The CPU must not access the MB.*/
    kFlexCanRX_Ranswer   = 0xA, 	/*!< A frame was configured to recognize a Remote Request Frame*/
																	/*!  and transmit a Response Frame in return.*/
    kFlexCanRX_NotUsed   = 0xF, 	/*!< Not used*/
}CAN_MBCode_Type;

void can_gpio_init();
void bsp_can_init(CAN_Type * CANx,CANBaud baud);		//初始化CAN，配置波特率
bool can_mb_cfg(CAN_Type * CANx,u8 mb,str_mb_mbopt * mbopt);	//邮箱配置
void bsp_can_enable(CAN_Type * CANx);	//使能CAN模块
void can_it_cfg(CAN_Type *CANx,u8 mb);	//cAN接收邮箱中断配置

bool can_send_msg(CAN_Type *CANx, uint32_t mb,str_can_msg * msg);

#endif



