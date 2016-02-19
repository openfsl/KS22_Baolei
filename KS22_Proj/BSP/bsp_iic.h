#ifndef __BSP_IIC_H__
#define __BSP_IIC_H__

#include "syscfg.h"

#define EEPROM_ADDRESS			0xA0

#define IIC_DisableAck()		LPI2C1_C1 |= LPI2C_C1_TXAK_MASK

#define IIC_ReStart()			LPI2C1_C1 |= LPI2C_C1_RSTA_MASK;

#define IIC_Start()				LPI2C1_C1 |= LPI2C_C1_TX_MASK;\
								LPI2C1_C1 |= LPI2C_C1_MST_MASK
								
#define IIC_Stop()				LPI2C1_C1 &= ~LPI2C_C1_MST_MASK;\
								LPI2C1_C1 &= ~LPI2C_C1_TX_MASK

#define IIC_EnterRxMode()		LPI2C1_C1 &= ~LPI2C_C1_TX_MASK;\
								LPI2C1_C1 &= ~LPI2C_C1_TXAK_MASK

#define IIC_Wait()				while((LPI2C1_S & LPI2C_S_IICIF_MASK)==0) {} \
								LPI2C1_S |= LPI2C_S_IICIF_MASK;

#define IIC_write_byte(data)	LPI2C1_D = data;


#endif