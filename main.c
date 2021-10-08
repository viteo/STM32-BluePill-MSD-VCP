#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "generic.h"
#include "device.h"

#include "hw_config.h"
#include "usb_core.h"
#include "usb_init.h"
#include "usb_pwr.h"

#include "term-srv.h"
#include "string.h"

void send_data(const char *data, int16_t size)
{
	while (size--)
	{
		VCP_AddToTxBuff(*data++);
	}
}

void test_cmd1(const char* data) {
    send_data(data, strlen(data));
}
void test_cmd2(const char* data) {
    send_data("command executed", strlen("command executed"));
}
void toggle_cmd(const char* data)
{
	send_data("LED switched", strlen("LED switched"));
	GPIO_ToggleBits(GPIOC, PIN_LED);
}

term_srv_cmd_t cmd_list[] = {
    { .cmd = "test-arg", .len = 8, .handler = test_cmd1 },
    { .cmd = "command", .len = 7, .handler = test_cmd2 },
    { .cmd = "toggle", .len = 6, .handler = toggle_cmd },
};

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

	term_srv_init(send_data, cmd_list, 3);

	USB_Init();

	while (1)
	{
		GPIO_ToggleBits(GPIOC, PIN_LED);
		DWT_Delay_ms(500);
	}
}
