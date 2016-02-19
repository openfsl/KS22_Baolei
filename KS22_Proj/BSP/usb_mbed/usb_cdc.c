/*
 * File:		usbcdckeil.c
 * Purpose:		Main process
 *
 */
/*
 * File:		KL25_USBCDC.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#ifdef CMSIS
#include "start.h"
#endif
#include "rl_usb.h"

typedef enum {
    USB_DISCONNECTED,
    USB_CONNECTING,
    USB_CONNECTED,
    USB_CHECK_CONNECTED,
    USB_CONFIGURED,
    USB_DISCONNECTING,
    USB_DISCONNECT_CONNECT
} USB_CONNECT;

// Global state of usb
USB_CONNECT usb_state;
/********************************************************************/
int main (void)
{
    uint8_t ch[64]={0};
    uint32_t cnt=0;
    
	 #ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
    start();
   #endif
        
  	printf("\n\r\n\r*** Running the usbcdckeil project ***\n\r");
		
    usbd_init();

    usbd_connect(__TRUE);

    usb_state = USB_CONNECTING;
    
    while (!usbd_configured ());          /* Wait for device to configure */

		printf("usbd configure complete\r\n");
		
    while(1) 
    {
        cnt = USBD_CDC_ACM_DataRead(ch, 64);
        if(cnt!=0)
          USBD_CDC_ACM_DataSend(ch, cnt);
    }
		
}
/********************************************************************/