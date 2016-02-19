#include "bsp_led.h"

void bsp_led_init()
{
	SIM_SCGC5 |=  SIM_SCGC5_PORTA_MASK;
	PORTA_PCR18 = PORT_PCR_MUX(1);				//PTA18 -- LED0
	PORTA_PCR19 = PORT_PCR_MUX(1);				//PTA19 -- LED1
	GPIOA_PDDR = ((1 << 18) | (1 << 19));		// Set as an output
}

void led_on(LEDNum num)
{
	GPIOA_PCOR = num;
}

void led_off(LEDNum num)
{
	GPIOA_PSOR = num;
}

void led_togle(LEDNum num)
{
	GPIOA_PTOR = num;
}

void led_run_chk()
{
	static u32 tcnt = 0;
	if(++tcnt >= 0xfffff)
	{
		tcnt = 0;
		led_togle(LED0);
	}
}