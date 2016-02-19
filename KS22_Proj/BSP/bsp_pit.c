#include "bsp_pit.h"

void delay_us(u32 us)							//使用PIT0做死等延时 精确延时
{
	;
}

void delay_ms(u32 ms)
{
	;
}

void pit_init(unsigned long uSec)				//us
{
	uint32_t bus_clk, temp;
	float bus_clk_ns;
/*
	bus_clk = (CLK0_FREQ_HZ / ((MCG_C5 & 0x1F) + 1));
	bus_clk = bus_clk * ((MCG_C6 & 0x1F) + 24);
	bus_clk >>= 1;
*/
	bus_clk = sys_bus_clk;
	bus_clk_ns = ((float)1000000000 / (float)bus_clk);
	uSec *= 1000;		/* make it nS */
	bus_clk_ns = (float)uSec / bus_clk_ns;
	temp = (uint32_t)bus_clk_ns;

	SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;	/* enable PIT module */

	PIT_TCTRL1 = 0x00;					/* disable PIT0 */
	PIT_LDVAL1 = temp;
	PIT_TCTRL1 = PIT_TCTRL_TIE_MASK;	/* enable PIT0 and interrupt */
	PIT_TFLG1 = 0x01;					/* clear flag */
	PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK;

	enable_irq(PIT1_IRQn);
	PIT_MCR = 0x00;
}

void PIT1_IRQHandler(void)	
{  
	PIT_TFLG1 = 0x01;			/* clear flag */
	PIT_TCTRL1;					/* dummy read to ensure the interrupt, */
}
