#ifndef __BSP_PIT_H__
#define __BSP_PIT_H__

#include "syscfg.h"

#define USEC			1000		//1000uS = 1ms

void pit_init(unsigned long uSec);
void delay_us(u32 us);
void delay_ms(u32 ms);

#endif

