#include "bsp_can.h"

void can_gpio_init()
{
	SIM_SCGC5 |=  SIM_SCGC5_PORTA_MASK|SIM_SCGC5_PORTB_MASK;				//ʹ��GPIOʱ��
	SIM_SCGC6 |=  (SIM_SCGC6_FLEXCAN0_MASK|SIM_SCGC6_FLEXCAN1_MASK);		//ʹ��CANʱ��
	
	PORTA_PCR12 = PORT_PCR_MUX(2);				//PTA12 -- CAN0_TX
	PORTA_PCR13 = PORT_PCR_MUX(2);				//PTA13 -- CAN0_RX

	PORTB_PCR2 = PORT_PCR_MUX(7);				//PTB2 -- CAN1_RX
	PORTB_PCR3 = PORT_PCR_MUX(7);				//PTB3 -- CAN1_TX	
}

//CAN ������ ����
#define FLEXCAN_TIME_QUANTA_NUM (10)
static u32 can_baud_cfg(CAN_Type *CANx, CANBaud baudrate)
{
	uint32_t priDiv = baudrate * FLEXCAN_TIME_QUANTA_NUM;
	if (0 == priDiv)	priDiv = 1;
    priDiv = (get_bus_clk() / priDiv) - 1;
	/* Desired baud rate is too low. */
    if (priDiv > 0xFF)  priDiv = 0xFF;
	
	 /* FlexCAN timing setting formula:
     * FLEXCAN_TIME_QUANTA_NUM = 1 + (PSEG1 + 1) + (PSEG2 + 1) + (PROPSEG + 1);
     */
    u32 preDivider = priDiv;
    u32 phaseSeg1 = 3;
    u32 phaseSeg2 = 2;
    u32 propSeg = 1;
    u32 rJumpwidth = 1;
	
	    /* Cleaning previous Timing Setting. */
    CANx->CTRL1 &= ~(CAN_CTRL1_PRESDIV_MASK | CAN_CTRL1_RJW_MASK | CAN_CTRL1_PSEG1_MASK | CAN_CTRL1_PSEG2_MASK |
                     CAN_CTRL1_PROPSEG_MASK);

    /* Updating Timing Setting according to configuration structure. */
    CANx->CTRL1 |=	(CAN_CTRL1_PRESDIV(preDivider) | CAN_CTRL1_RJW(rJumpwidth) |
					CAN_CTRL1_PSEG1(phaseSeg1) | CAN_CTRL1_PSEG2(phaseSeg2) | CAN_CTRL1_PROPSEG(propSeg));
}

//--------------------------------------------------------------//
//һ�� MB������ֻ������Ϊ���ձ�׼֡������չ֡������ͬʱ���ձ�׼֡����չ֡��
bool can_mb_cfg(CAN_Type * CANx,u8 mb,str_mb_mbopt * mbopt)
{
	u32 cs_temp = 0;
	if(mb >= CAN_MB_MAX)	return FALSE;
	
	if(mbopt->type == CAN_ID_TYPE_REMOTE)	cs_temp |= CAN_CS_RTR_MASK;		//Զ��֡
	
	if(mbopt->format == CAN_ID_FORMAT_EXT)					//��չ֡
	{
		cs_temp |= CAN_CS_IDE_MASK;
		CANx->MB[mb].CS = CAN_ID_EXT(mbopt->id);
	}
	else
	{
		CANx->MB[mb].CS = CAN_ID_STD(mbopt->id);
	}
	
	cs_temp |= CAN_CS_CODE(kFlexCanRX_Empty);		//�����ս��ջ�����
	CANx->MB[mb].CS = cs_temp;
	
	CANx->RXIMR[mb] = CAN_RXIMR_MI(mbopt->mask); 
	return TRUE;
}

void can_it_cfg(CAN_Type *CANx,u8 mb)		//ʹ��CAN����Ľ����ж�
{
	CANx->IMASK1 |= (1 << mb);
	
	if(CANx == CAN0)	enable_irq(CAN0_ORed_Message_buffer_IRQn);
	else				enable_irq(CAN1_ORed_Message_buffer_IRQn);
}

void bsp_can_init(CAN_Type * CANx,CANBaud baud)
{
	u8 i;
	
	CANx->CTRL1 |= CAN_CTRL1_CLKSRC_MASK;					//ѡ��ʱ��Դ��Busclock
    CANx->MCR &= ~CAN_MCR_MDIS_MASK;						/* enable module */
    
	CANx->MCR |= CAN_MCR_SOFTRST_MASK;						/* software reset */
	while(CAN_MCR_SOFTRST_MASK & (CANx->MCR)) {}; 
        
    CANx->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);	/* halt mode */
	while(!(CAN_MCR_FRZACK_MASK & (CANx->MCR))) {}; 
 
	//CAN0->MB	    
    for(i = 0; i < CAN_MB_MAX; i++)							/* init all mb */	//CAN_MB_MAX #define CAN_MB_MAX      (ARRAY_SIZE(CAN0->MB))
	{
		CANx->MB[i].CS = 0x00000000;
		CANx->MB[i].ID = 0x00000000;
		CANx->MB[i].WORD0 = 0x00000000;
		CANx->MB[i].WORD1 = 0x00000000;
        CANx->RXIMR[i] = 0x00000000;						/* received all frame */
	}
	CANx->IMASK1 = 0x00000000;
    CANx->IFLAG1 = 0xFFFFFFFF;
	
	/* set all masks */
	//CANx->RXMGMASK = CAN_ID_EXT(CAN_RXMGMASK_MG_MASK); 
    // CANx->RX14MASK = CAN_ID_EXT(CAN_RX14MASK_RX14M_MASK); 
    // CANx->RX15MASK = CAN_ID_EXT(CAN_RX15MASK_RX15M_MASK);
    CANx->MCR |= CAN_MCR_IRMQ_MASK;		//ʹ��˽�н������룬
    CANx->MCR &= ~CAN_MCR_IDAM_MASK;	//��ʽA��ÿ��ID���˱���һ��ȫID����׼����չ��
     /* bypass the frame sended by itself */
    CANx->MCR |= CAN_MCR_SRXDIS_MASK; 						// ��ֹ�Խ���λ��Ĭ��Ϊ0��ʹ���Խ��ܣ����з���ʱ�����������ڽ���MB��洢һ�¡�   
	CANx->MCR |= CAN_MCR_AEN_MASK;		//������ֹ����
    /* setting baudrate */
	can_baud_cfg(CANx,baud);
}

void bsp_can_enable(CAN_Type * CANx)
{
    /* enable module */
    CANx->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	
	while((CAN_MCR_FRZACK_MASK & (CANx->MCR)));
	while(((CANx->MCR)&CAN_MCR_NOTRDY_MASK));
	
	CANx->MCR &= ~CAN_MCR_MDIS_MASK;
	/* Wait FlexCAN exit from low-power mode. */
	while (CANx->MCR & CAN_MCR_LPMACK_MASK) {}
}

void can_set_rx_fifo(CAN_Type *CANx)			//����CAN��FIFO���չ���
{
    /* halt mode */
    CANx->MCR |= (CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while(!(CAN_MCR_FRZACK_MASK & (CANx->MCR))) {}; 
        
    CANx->MCR |= CAN_MCR_RFEN_MASK;
    CANx->CTRL2 |= CAN_CTRL2_RFFN(0);
    
    /* enable module */
    CANx->MCR &= ~(CAN_MCR_FRZ_MASK | CAN_MCR_HALT_MASK);
	while((CAN_MCR_FRZACK_MASK & (CANx->MCR)));
	while(((CANx->MCR)&CAN_MCR_NOTRDY_MASK));
}

bool is_mb_idle(CAN_Type *CANx, uint32_t mb)		//CANͨ��ͨ��״̬���
{
    uint32_t code;
    code = CAN_GET_MB_CODE(CANx->MB[mb].CS);
    if((code == kFlexCanTX_Inactive) || (code == kFlexCanRX_Inactive) || (code == kFlexCanRX_Empty))
    {
        return TRUE;
    }
    return FALSE;
}

/**
 * @brief  ��ȡCAN������յ�������
 * @param[in]  instance CANģ���
 *         			@arg HW_CAN0 оƬ��CANģ��0��
 *         			@arg HW_CAN1 оƬ��CANģ��1��
 * @retval 0 ����
 * \retval 1 ������ 
 * \retval 2 ���ڽ���
 */
uint32_t CAN_ReadData(CAN_Type *CANx, uint32_t mb,str_can_msg * msg)
{
	u32 code;

	//printf("cs is 0x%x\r\n",CANx->MB[mb].CS);
	
    code = CAN_GET_MB_CODE(CANx->MB[mb].CS);
    if((code & 0x01))
    { 
        return FALSE; /* MB is busy and controlled by hardware */
    }
	/* read content */
	msg->DLC = CAN_GET_FRAME_LEN(CANx->MB[mb].CS);
	msg->IDE = CAN_GET_IDE(CANx->MB[mb].CS);
	msg->RTR = CAN_GET_RTR(CANx->MB[mb].CS);
	
	if(msg->IDE)	msg->ID = CAN_GET_EXT_ID(CANx->MB[mb].ID);
	else			msg->ID = CAN_GET_STD_ID(CANx->MB[mb].ID);
	
	msg->time_stamp = CANx->MB[mb].CS & CAN_CS_TIME_STAMP_MASK;		//ʱ���
	
	msg->Data[0] = (CANx->MB[mb].WORD0>>24)&0xff;
	msg->Data[1] = (CANx->MB[mb].WORD0>>16)&0xff;
	msg->Data[2] = (CANx->MB[mb].WORD0>>8)&0xff;
	msg->Data[3] = (CANx->MB[mb].WORD0)&0xff;

	msg->Data[4] = (CANx->MB[mb].WORD1>>24)&0xff;
	msg->Data[5] = (CANx->MB[mb].WORD1>>16)&0xff;
	msg->Data[6] = (CANx->MB[mb].WORD1>>8)&0xff;
	msg->Data[7] = (CANx->MB[mb].WORD1)&0xff;

	code = CANx->TIMER;		/* unlock MB */
	
	return TRUE;
}

bool can_send_msg(CAN_Type *CANx, uint32_t mb,str_can_msg * msg)
{
	//if(!is_mb_idle(CANx,mb))	return FALSE;
		/* Clear interrupt if it is active                                          */
	if (CANx->IFLAG1 & (1 << mb)) CANx->IFLAG1 = (1 << mb);

	CANx->MB[mb].WORD0 = (msg->Data[0]<<24)|(msg->Data[1]<<16)|(msg->Data[2]<<8)|(msg->Data[3]);
    CANx->MB[mb].WORD1 = (msg->Data[4]<<24)|(msg->Data[5]<<16)|(msg->Data[6]<<8)|(msg->Data[7]);
	/* DLC field */
    CANx->MB[mb].CS &= ~CAN_CS_DLC_MASK;
    CANx->MB[mb].CS |= CAN_CS_DLC(msg->DLC);

    CANx->MB[mb].CS &= ~CAN_CS_RTR_MASK;		/* clear RTR */
	if(msg->IDE)	//��չ֡
	{
		CANx->MB[mb].ID = (msg->ID & (CAN_ID_STD_MASK | CAN_ID_EXT_MASK));  /* ID [28-0]*/
		CANx->MB[mb].CS |= (CAN_CS_SRR_MASK | CAN_CS_IDE_MASK);;
	}
	else
	{
        CANx->MB[mb].ID = CAN_ID_STD(msg->ID);  			/* ID[28-18] */
        CANx->MB[mb].CS &= ~(CAN_CS_IDE_MASK | CAN_CS_SRR_MASK); 		
	}
	
	CANx->MB[mb].CS &= ~CAN_CS_CODE_MASK;
    CANx->MB[mb].CS |= CAN_CS_CODE(kFlexCanTX_Data);
	return TRUE;
}

void CAN0_ORed_Message_buffer_IRQHandler()
{
	u32    iflag1, mb;
	iflag1 = CAN0->IFLAG1;
	
	for (mb = 0; mb < CAN_RX_MB; mb ++) 
	{
		if(iflag1 & (1<<mb))
		{
			CAN_ReadData(CAN0,mb,&can0_rx.rx_msg);
		}
	}
	
	CAN0->IFLAG1 = iflag1;		/* clear IT pending bit */
	can0_rx.rx_flag = 1;
}

void CAN1_ORed_Message_buffer_IRQHandler()
{
	u32    iflag1, mb;
	iflag1 = CAN1->IFLAG1;
	
	for (mb = 0; mb < CAN_RX_MB; mb ++) 
	{
		if(iflag1 & (1<<mb))
		{
			CAN_ReadData(CAN1,mb,&can1_rx.rx_msg);
		}
	}
	
	CAN1->IFLAG1 = iflag1;		/* clear IT pending bit */
	can1_rx.rx_flag = 1;
}
