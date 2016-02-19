#ifndef __BSP_LED_H__
#define __BSP_LED_H__

#include "syscfg.h"

typedef enum
{
	LED0 = 1<<18,
	LED1 = 1<<19,
}LEDNum;

void bsp_led_init();
void led_on(LEDNum num);
void led_off(LEDNum num);
void led_togle(LEDNum num);
void led_run_chk(void);












#endif

