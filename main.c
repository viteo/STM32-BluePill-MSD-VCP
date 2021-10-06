#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "generic.h"
#include "device.h"

#include "hw_config.h"
#include "usb_core.h"
#include "usb_init.h"
#include "usb_pwr.h"

extern __IO uint8_t Receive_Buffer[64];
extern __IO uint32_t Receive_length;
extern __IO uint32_t length;
uint8_t Send_Buffer[64];
uint32_t packet_sent = 1;
uint32_t packet_receive = 1;

int main()
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = PIN_LED;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	SystemInit();
	Set_System();
	Set_USBClock();
	DWT_Delay_Init();
	DWT_Delay_ms(500);
	USB_Interrupts_Config(ENABLE);
	DWT_Delay_ms(500);


	USB_Init();
	DWT_Delay_ms(500);





	while (1)
	{
		GPIO_ToggleBits(GPIOC, PIN_LED);
		if (bDeviceState == CONFIGURED)
		{
			CDC_Receive_DATA();
			/*Check to see if we have data yet */
			if (Receive_length != 0)
			{
				if (packet_sent == 1)
					CDC_Send_DATA((unsigned char*) Receive_Buffer, Receive_length);
				Receive_length = 0;
			}
		}
	}
}
