#include "syscfg.h"
#include "bsp_pit.h"
#include "bsp_uart.h"
#include "app_usb.h"
#include "bsp_led.h"

static volatile u8 ms_flag = 0;

extern void sys_clk_init(void);

void SysTick_Handler(void) 
{
	ms_flag = 1;
}


void hw_init()
{	
	sys_clk_init();
	
	bsp_led_init();
	uart_gpio_init();
	uart_init(DBG_PORT,DBG_PORT_BAUD);				//³õÊ¼»¯Debug port
	usb_app_init();
	pit_init(USEC);									/* 100 us*/
	SysTick_Config(sys_core_clk/1000); 				/* Generate interrupt each 1 ms       */
	
	app_can_init();
}

void tmr_ms_chk()
{
	static u32 tcnt = 0;
	if(ms_flag)
	{
		ms_flag = 0;
		if(++tcnt >= 500)
		{
			tcnt = 0;
			led_togle(LED1);
		}
	}
}