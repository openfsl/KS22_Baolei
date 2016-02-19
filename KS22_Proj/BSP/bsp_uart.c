#include "bsp_uart.h"

void uart_gpio_init()
{
	/* Turn on all port clocks */
	SIM_SCGC5 |= 	SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTE_MASK;
	
	//UART0 - PTA1/PTA2
	PORTA_PCR1 = PORT_PCR_MUX(2);			//PTA1 -- UART0_RX
	PORTA_PCR2 = PORT_PCR_MUX(2);			//PTA2 -- UART0_TX
	
	PORTE_PCR0 = PORT_PCR_MUX(3);			//PTE0 -- UART1_TX
	PORTE_PCR1 = PORT_PCR_MUX(3);			//PTE1 -- UART1_RX
}

void uart_init(UART_MemMapPtr uartch,int baud)
{
	register uint16 sbr, brfa;
	uint8 temp;
	
	u32 sysclk = sys_bus_clk/1000;
		 
	/* Enable the clock to the selected UART */
	if (uartch == UART0){
		SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
		sysclk = sys_core_clk/1000;
	}
	else if (uartch == UART1){	
		SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
		sysclk = sys_core_clk/1000;
	}
		
	else if (uartch == UART2)	SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
		
	/*
	 * Make sure that the transmitter and receiver are disabled while we
	 * change settings.
	 */
	UART_C2_REG(uartch) &= ~(UART_C2_TE_MASK | UART_C2_RE_MASK);

	/*
	 * Configure the UART for 8-bit mode, no parity
	 * We need all default settings, so entire register is cleared
	 */
	UART_C1_REG(uartch) = 0;

	/* Calculate baud settings */
	sbr = (uint16) ((sysclk * 1000) / (baud * 16));

	/* Save off the current value of the UARTx_BDH except for the SBR field */
	temp = UART_BDH_REG(uartch) & ~(UART_BDH_SBR(0x1F));

	UART_BDH_REG(uartch) = temp | UART_BDH_SBR(((sbr & 0x1F00) >> 8));
	UART_BDL_REG(uartch) = (uint8) (sbr & UART_BDL_SBR_MASK);

	/*
	 * Determine if a fractional divider is needed to get
	 * closer to the baud rate
	 */
	brfa = (((sysclk * 32000) / (baud * 16)) - (sbr * 32));

	/*
	 * Save off the current value of the UARTx_C4 register except for the
	 * BRFA field
	 */
	temp = UART_C4_REG(uartch) & ~(UART_C4_BRFA(0x1F));

	UART_C4_REG(uartch) = temp | UART_C4_BRFA(brfa);

	/* Enable receiver and transmitter */
	UART_C2_REG(uartch) |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
}

/*
 * \brief	Wait for a character to be received on the specified UART
 * \author
 * \param	channel		UART channel to read from
 * \return	the received character
 * \todo
 * \warning
 */
char uart_getchar(UART_MemMapPtr channel)
{
	/* Wait until character has been received */
	while (!(UART_S1_REG(channel) & UART_S1_RDRF_MASK)) ;

	/* Return the 8-bit data from the receiver */
	return UART_D_REG(channel);
}

/*
 * \brief	Wait for space in the UART Tx FIFO and then send a character
 * \author
 * \param	channel	UART channel to send to
 * \param	ch		character to send
 * \return	none
 * \todo
 * \warning
 */
void uart_putchar(UART_MemMapPtr channel, char ch)
{
	/* Wait until space is available in the FIFO */
	while (!(UART_S1_REG(channel) & UART_S1_TDRE_MASK)) ;

	/* Send the character */
	UART_D_REG(channel) = (uint8) ch;
}

/*
 * \brief	Check to see if a character has been received
 * \author
 * \param	channel	UART channel to check for a character
 * \param	ch		character to send
 * \return	0		No character received
 * \return	1		Character has been received
 * \todo
 * \warning
 */
int uart_getchar_present(UART_MemMapPtr channel)
{
	return (UART_S1_REG(channel) & UART_S1_RDRF_MASK);
}

#if 0
void  UART0_IRQHandler (void)
{     
   UART0_MemMapPtr uartPtr = UART0_BASE_PTR;                            /* 发送中断处理程序             */
   #if UART0_SEND_IRQ                                                   /* 用户定义                     */ 
   #endif
    
   #if UART0_RECEIVE_IRQ
    
   while (UART0_S1_REG(uartPtr) & UART0_S1_RDRF_MASK) {                 /* 清除中断标志                 */
       uart0SendChar(UART0_D_REG(uartPtr));                             /* 返回接收数据                 */
//     while(!UART0_D_REG(uartPtr));                                    /* 清接收缓冲区                 */
   }   
   #endif
}
#endif

//---------------- for printf -------------------------
//不使用半主机模式
#if 1 //如果没有这段，则需要在target选项中选择使用USE microLIB
#pragma import(__use_no_semihosting)
struct __FILE  
{  
	int handle;  
};  
FILE __stdout;  

void _sys_exit(int x)  
{  
	x = x;  
}
#endif

int fputc(int ch, FILE *f)
{
	while (!(UART_S1_REG(DBG_PORT) & UART_S1_TDRE_MASK));
    UART_D_REG(DBG_PORT) = ch;  

	return ch;
}