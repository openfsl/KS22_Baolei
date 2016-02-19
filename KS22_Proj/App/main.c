#include "syscfg.h"
#include "bsp_pit.h"
#include "bsp_uart.h"
#include "app_usb.h"
#include "bsp_led.h"

void hw_init(void);
void tmr_ms_chk(void);

void HardFault_Handler()
{
	printf("system HardFault !\r\n");
	while(1);
}

int main()
{
	hw_init();
	printf("system init done!\r\n");
	
	while(1)
	{
		tmr_ms_chk();
		led_run_chk();
		can_rx_chk();
	}
}

