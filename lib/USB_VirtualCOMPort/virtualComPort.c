/**
  ******************************************************************************
  * @file    virtualComPort.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Virtual Com Port Configuration
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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_desc.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define VCP_TX_DATA_SIZE   512

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

uint8_t  VCP_Tx_Buffer[VCP_TX_DATA_SIZE]; 
uint32_t VCP_Tx_ptr_in = 0;
uint32_t VCP_Tx_ptr_out = 0;
uint32_t VCP_Tx_length  = 0;
uint8_t  USB_Tx_State = 0;

/* Extern variables ----------------------------------------------------------*/
extern LINE_CODING linecoding;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/*******************************************************************************
* Function Name  : VCP_AddToTxBuff.
* Description    : add char to VCP output Tx buffer
* Input          : None.
* Return         : none.
*******************************************************************************/
void VCP_AddToTxBuff(uint8_t received_byte)
{
	VCP_Tx_Buffer[VCP_Tx_ptr_in] = received_byte;

	VCP_Tx_ptr_in++;

	/* To avoid buffer overflow */
	if (VCP_Tx_ptr_in == VCP_TX_DATA_SIZE)
	{
		VCP_Tx_ptr_in = 0;
	}
}

/*******************************************************************************
* Function Name  : VCP_ProcessRxBuff
* Description    : process received data (data from host VCP Rx buffer)
* Input          : data_buffer: data address
                   Nb_bytes: number of bytes to process
* Return         : none
*******************************************************************************/
#include "term-srv.h"
void VCP_ProcessRxBuff(uint8_t* data_buffer, uint8_t Nb_bytes)
{

	while(Nb_bytes--)
	{
		term_srv_process(*data_buffer++);
	}
}

/*******************************************************************************
* Function Name  : VCP_SendTxBufPacketToUsb
* Description    : send data from VCP_Tx_Buffer to the USB. Manage the segmentation
*                  into USB FIFO buffer. Commit one packet to the USB at each call.
* Input          : globals:
*                  - USB_Tx_State: transmit state variable
*                  - VCP_Tx_Buffer: buffer of data to be sent
*                  - VCP_Tx_Length: amount of data (in bytes) ready to be sent
*                  - VCP_Tx_ptr_out: index in USART_Rx_Buffer of the first data to send
* Return         : none.
*******************************************************************************/
void VCP_SendTxBufPacketToUsb(void)
{
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;

	if (USB_Tx_State == 1)
	{
		if (VCP_Tx_length == 0)
		{
			USB_Tx_State = 0;
		}
		else
		{
			if (VCP_Tx_length > VIRTUAL_COM_PORT_DATA_SIZE)
			{
				USB_Tx_ptr = VCP_Tx_ptr_out;
				USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;

				VCP_Tx_ptr_out += VIRTUAL_COM_PORT_DATA_SIZE;
				VCP_Tx_length -= VIRTUAL_COM_PORT_DATA_SIZE;
			}
			else
			{
				USB_Tx_ptr = VCP_Tx_ptr_out;
				USB_Tx_length = VCP_Tx_length;

				VCP_Tx_ptr_out += VCP_Tx_length;
				VCP_Tx_length = 0;
			}

			UserToPMABufferCopy(&VCP_Tx_Buffer[USB_Tx_ptr], ENDP3_TXADDR, USB_Tx_length);
			SetEPTxCount(CDC_EP_IDX, USB_Tx_length);
			SetEPTxValid(CDC_EP_IDX);
		}
	}
}

/*******************************************************************************
* Function Name  : Handle_USBAsynchXfer.
* Description    : send data to USB.
* Input          : None.
* Return         : none.
*******************************************************************************/
void Handle_USBAsynchXfer (void)
{
	uint16_t USB_Tx_length;
	uint16_t USB_Tx_ptr;
	if (USB_Tx_State != 1)
	{
		if (VCP_Tx_ptr_out == VCP_TX_DATA_SIZE)
		{
			VCP_Tx_ptr_out = 0;
		}

		if (VCP_Tx_ptr_out == VCP_Tx_ptr_in)
		{
			USB_Tx_State = 0;
			return;
		}

		if (VCP_Tx_ptr_out > VCP_Tx_ptr_in) /* rollback */
		{
			VCP_Tx_length = VCP_TX_DATA_SIZE - VCP_Tx_ptr_out;
		}
		else
		{
			VCP_Tx_length = VCP_Tx_ptr_in - VCP_Tx_ptr_out;
		}

		if (VCP_Tx_length > VIRTUAL_COM_PORT_DATA_SIZE)
		{
			USB_Tx_ptr = VCP_Tx_ptr_out;
			USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;

			VCP_Tx_ptr_out += VIRTUAL_COM_PORT_DATA_SIZE;
			VCP_Tx_length -= VIRTUAL_COM_PORT_DATA_SIZE;
		}
		else
		{
			USB_Tx_ptr = VCP_Tx_ptr_out;
			USB_Tx_length = VCP_Tx_length;

			VCP_Tx_ptr_out += VCP_Tx_length;
			VCP_Tx_length = 0;
		}
		USB_Tx_State = 1;

		UserToPMABufferCopy(&VCP_Tx_Buffer[USB_Tx_ptr], ENDP3_TXADDR, USB_Tx_length);
		SetEPTxCount(CDC_EP_IDX, USB_Tx_length);
		SetEPTxValid(CDC_EP_IDX);
	}
}


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
