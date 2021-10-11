#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "generic.h"
#include "device.h"

#include "hw_config.h"
#include "usb_core.h"
#include "usb_init.h"
#include "usb_pwr.h"

#include "terminal.h"

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
	USB_Interrupts_Config(ENABLE);

	term_srv_init(term_send_data, cmd_list, cmd_count);

	USB_Init();

	while (1)
	{
		GPIO_ToggleBits(GPIOC, PIN_LED);
		DWT_Delay_ms(500);
	}
}
