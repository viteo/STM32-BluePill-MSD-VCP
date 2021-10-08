/**
  ******************************************************************************
  * @file    usb_conf.h
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Custom HID demo configuration file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

// Interface numbers
#define MSC_INTERFACE_IDX 0x0			// Index of MSC interface
#define CDC_INTERFACE_IDX 0x1			// Index of CDC interface

#define IN_EP_DIR						0x80 // Adds a direction bit

// endpoints numbers
#define MSC_EP_IDX                      0x01
#define CDC_CMD_EP_IDX                  0x02
#define CDC_EP_IDX                      0x03

#define MSC_OUT_EP                      MSC_EP_IDX                     /* EP1 for BULK OUT */
#define MSC_IN_EP                       (MSC_EP_IDX | IN_EP_DIR)       /* EP1 for BULK IN */
#define CDC_CMD_EP                      (CDC_CMD_EP_IDX | IN_EP_DIR)   /* EP2 for CDC commands */
#define CDC_OUT_EP                      CDC_EP_IDX                     /* EP3 for data OUT */
#define CDC_IN_EP                       (CDC_EP_IDX | IN_EP_DIR)       /* EP3 for data IN */

/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/
#define EP_NUM     (4) //counting EP0..EP3

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
#define BTABLE_ADDRESS      (0x00)

/* CONTROL EP0 */
/* rx/tx buffer base address */
#define ENDP0_RXADDR        (0x20)
#define ENDP0_TXADDR        (0x60)

/* MSC EP1 */
/* rx/tx buffer base address */
#define ENDP1_RXADDR        (0xA0)
#define ENDP1_TXADDR        (0xE0)

/* CDC EP2 EP3 */
/* cmd/rx/tx buffer base address */
#define ENDP2_CMDADDR       (0x100)
#define ENDP3_RXADDR        (0x140)
#define ENDP3_TXADDR        (0x180)

/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
#define IMR_MSK (CNTR_CTRM  | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM  | CNTR_SOFM \
                 | CNTR_ESOFM | CNTR_RESETM )

/*#define CTR_CALLBACK*/
/*#define DOVR_CALLBACK*/
/*#define ERR_CALLBACK*/
/*#define WKUP_CALLBACK*/
/*#define SUSP_CALLBACK*/
/*#define RESET_CALLBACK*/
#define SOF_CALLBACK
/*#define ESOF_CALLBACK*/

/* CTR service routines */
/* associated to defined endpoints */
/* #define  EP1_IN_Callback   NOP_Process */
//#define  EP2_IN_Callback   NOP_Process
//#define  EP3_IN_Callback   NOP_Process
#define  EP4_IN_Callback   NOP_Process
#define  EP5_IN_Callback   NOP_Process
#define  EP6_IN_Callback   NOP_Process
#define  EP7_IN_Callback   NOP_Process

//#define  EP1_OUT_Callback   NOP_Process
#define  EP2_OUT_Callback   NOP_Process
//#define  EP3_OUT_Callback   NOP_Process
#define  EP4_OUT_Callback   NOP_Process
#define  EP5_OUT_Callback   NOP_Process
#define  EP6_OUT_Callback   NOP_Process
#define  EP7_OUT_Callback   NOP_Process

#endif /*__USB_CONF_H*/

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

