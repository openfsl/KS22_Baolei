#include "bsp_iic.h"

/*
 * I2C Initialization
 * Set Baud Rate and turn on I2C0
 */
void init_I2C(void)
{
	SIM_SCGC5 |=  SIM_SCGC5_PORTC_MASK;
	SIM_SCGC4 |=  SIM_SCGC4_LPI2C1_MASK;			//Turn on clock to I2C` module

	/* configure GPIO for I2C0 function */
	PORTC_PCR10 = PORT_PCR_MUX(2);
	PORTC_PCR11 = PORT_PCR_MUX(2);

//	LPI2C1_F = 0x14;								/* set MULT and ICR */
//	LPI2C1_C1 = LPI2C_C1_IICEN_MASK;				/* enable IIC */
}