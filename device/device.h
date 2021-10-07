/*
 * device.h
 *
 * Device/project description
 *
 */

#ifndef DEVICE_DEVICE_H_
#define DEVICE_DEVICE_H_
#include "stm32f10x_gpio.h"

#define FLASH_SIZE		*(uint16_t*)0x1ffff7e0 // RM0008 30.1.1 - Flash size register in kB
#if defined(STM32F10X_HD) || defined(STM32F10X_HD_VL) || defined(STM32F10X_CL) || defined(STM32F10X_XL)
#define FLASH_PAGE_SIZE 0x800 //2048 bytes
#else
#define FLASH_PAGE_SIZE 0x400 //1024 bytes
#endif
#define FLASH_DISK_START_ADDRESS	FLASH_BASE + 40 * FLASH_PAGE_SIZE
#define FLASH_DISK_SIZE				(FLASH_SIZE - 40) * FLASH_PAGE_SIZE

//GPIO Defines
#define PIN_LED GPIO_Pin_13 //PC13	BluePill Green LED

#endif /* DEVICE_DEVICE_H_ */
