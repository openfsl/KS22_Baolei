#ifndef _COMMON_H_
#define _COMMON_H_

#include "MKS22F25612.h"
#include "stdio.h"
#include <stdint.h>

#define enable_irq 			NVIC_EnableIRQ
#define disable_irq 		NVIC_DisableIRQ
#define EnableInterrupts 	__enable_irq()
#define DisableInterrupts 	__disable_irq()

/********************************************************************/
typedef unsigned char		u8;     /* 无符号8位整型变量            */
typedef unsigned short int	u16;    /* 无符号16位整型变量           */
typedef unsigned int		u32;    /* 无符号32位整型变量           */

/* The basic data types */
typedef unsigned char		uint8;		/*  8 bits */
typedef unsigned short int	uint16;		/* 16 bits */
typedef unsigned long int	uint32;		/* 32 bits */

typedef char				int8;		/*  8 bits */
typedef short int			int16;		/* 16 bits */
typedef int					int32;		/* 32 bits */

typedef volatile int8		vint8;		/*  8 bits */
typedef volatile int16		vint16;		/* 16 bits */
typedef volatile int32		vint32;		/* 32 bits */

typedef volatile uint8		vuint8;		/*  8 bits */
typedef volatile uint16		vuint16;	/* 16 bits */
typedef volatile uint32		vuint32;	/* 32 bits */

/********************************************************************/

extern u32 	sys_core_clk;	//系统时钟
extern u32 	sys_bus_clk;	//总线时钟
#define get_sys_clk()		sys_core_clk
#define get_bus_clk()		sys_bus_clk

typedef enum
{
	FALSE = 0,
	TRUE = 1,
}bool;

#ifdef	FALSE
#undef	FALSE
#endif
#define FALSE	(0)

#ifdef	TRUE
#undef	TRUE
#endif
#define	TRUE	(1)

#ifdef	NULL
#undef	NULL
#endif
#define NULL	(0)

#ifdef  ON
#undef  ON
#endif
#define ON      (1)

#ifdef  OFF
#undef  OFF
#endif
#define OFF     (0)

#define SCB_SCR_SLEEPDEEP_MASK  (1<<2)


#endif /* _COMMON_H_ */
