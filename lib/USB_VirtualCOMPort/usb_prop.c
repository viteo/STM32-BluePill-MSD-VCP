/**
  ******************************************************************************
  * @file    usb_prop.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   All processing related to Custom Composite Device
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

#include "hw_config.h" 
#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_desc.h"
#include "usb_pwr.h"
#include "usb_bot.h"
#include "memory.h"
#include "mass_mal.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Request = 0;
LINE_CODING linecoding =
{
	115200, /* baud rate*/
	0x00,   /* stop bits-1*/
	0x00,   /* parity - none*/
	0x08    /* no. of bits 8*/
};

uint32_t Max_Lun = 0;
/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
  {
    EP_NUM,
    1
  };

DEVICE_PROP Device_Property =
  {
    Composite_Init,
    Composite_Reset,
    Composite_Status_In,
    Composite_Status_Out,
    Composite_Data_Setup,
    Composite_NoData_Setup,
    Composite_Get_Interface_Setting,
    Composite_GetDeviceDescriptor,
    Composite_GetConfigDescriptor,
    Composite_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
  };
USER_STANDARD_REQUESTS User_Standard_Requests =
  {
    Composite_GetConfiguration,
    Composite_SetConfiguration,
    Composite_GetInterface,
    Composite_SetInterface,
    Composite_GetStatus,
    Composite_ClearFeature,
    Composite_SetEndPointFeature,
    Composite_SetDeviceFeature,
    Composite_SetDeviceAddress
  };

ONE_DESCRIPTOR Device_Descriptor =
  {
    (uint8_t*)Composite_DeviceDescriptor,
    Composite_SIZ_DEVICE_DESC
  };

ONE_DESCRIPTOR Config_Descriptor =
{
	(uint8_t*)Composite_ConfigDescriptor,
	Composite_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[4] =
{
	{(uint8_t*)Composite_StringLangID, Composite_SIZ_STRING_LANGID},
	{(uint8_t*)Composite_StringVendor, Composite_SIZ_STRING_VENDOR},
	{(uint8_t*)Composite_StringProduct, Composite_SIZ_STRING_PRODUCT},
	{(uint8_t*)Composite_StringSerial, Composite_SIZ_STRING_SERIAL}
};

/* Extern variables ----------------------------------------------------------*/
extern unsigned char Bot_State;
extern Bulk_Only_CBW CBW;

/* Private function prototypes -----------------------------------------------*/
/* Extern function prototypes ------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
uint8_t *VCP_GetLineCoding(uint16_t Length);
uint8_t *VCP_SetLineCoding(uint16_t Length);
uint8_t *MSD_Get_Max_Lun(uint16_t Length);

/*******************************************************************************
 * Function Name  : Composite_Init
 * Description    : Composite USB device init routine
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Composite_Init(void)
{
	/* Update the serial number string descriptor with the data from the unique ID*/
	Get_SerialNum();

	pInformation->Current_Configuration = 0;

	/* Connect the device */
	PowerOn();

	/* Perform basic device initialization operations */
	USB_SIL_Init();

	bDeviceState = UNCONNECTED;
}

/*******************************************************************************
 * Function Name  : Composite_Reset
 * Description    : Composite USB device reset routine
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Composite_Reset(void)
{
	/* Set Composite_DEVICE as not configured */
	pInformation->Current_Configuration = 0;

	/* Current Feature initialization */
	pInformation->Current_Feature = Composite_ConfigDescriptor[7];

	/* Set Composite DEVICE with the default Interface*/
	pInformation->Current_Interface = 0;

	SetBTABLE(BTABLE_ADDRESS);

	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	Clear_Status_Out(ENDP0);
	SetEPRxValid(ENDP0);

	/* Initialize MSC Endpoints*/
	SetEPType(MSC_EP_IDX, EP_BULK);
	SetEPRxAddr(MSC_EP_IDX, ENDP1_RXADDR);
	SetEPRxStatus(MSC_EP_IDX, EP_RX_VALID);
	SetEPRxCount(MSC_EP_IDX, Device_Property.MaxPacketSize);
	SetEPTxAddr(MSC_EP_IDX, ENDP1_TXADDR);
	SetEPTxStatus(MSC_EP_IDX, EP_TX_NAK);

	/* Initialize CDC Endpoints */
	SetEPType(CDC_CMD_EP_IDX, EP_INTERRUPT);
	SetEPTxAddr(CDC_CMD_EP_IDX, ENDP2_CMDADDR);
	SetEPRxStatus(CDC_CMD_EP_IDX, EP_RX_DIS);
	SetEPTxStatus(CDC_CMD_EP_IDX, EP_TX_NAK);

	SetEPType(CDC_EP_IDX, EP_BULK);
	SetEPTxAddr(CDC_EP_IDX, ENDP3_TXADDR);
	SetEPTxStatus(CDC_EP_IDX, EP_TX_NAK);
	SetEPRxAddr(CDC_EP_IDX, ENDP3_RXADDR);
	SetEPRxStatus(CDC_EP_IDX, EP_RX_VALID);
	SetEPRxCount(CDC_EP_IDX, Device_Property.MaxPacketSize);

	/* Set this device to response on default address */
	SetDeviceAddress(0);
	bDeviceState = ATTACHED;

	CBW.dSignature = BOT_CBW_SIGNATURE;
	Bot_State = BOT_IDLE;
}

/*******************************************************************************
 * Function Name  : Composite_SetConfiguration
 * Description    : Handle the SetConfiguration request
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Composite_SetConfiguration(void)
{
	if (pInformation->Current_Configuration != 0)
	{
		/* Device configured */
		bDeviceState = CONFIGURED;

		ClearDTOG_TX(MSC_OUT_EP);
		ClearDTOG_RX(MSC_IN_EP);
		Bot_State = BOT_IDLE; /* set the Bot state machine to the IDLE state */
	}
}

/*******************************************************************************
 * Function Name  : Composite_SetDeviceAddress
 * Description    : Update the device state to addressed
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Composite_SetDeviceAddress(void)
{
	bDeviceState = ADDRESSED;
}

/*******************************************************************************
 * Function Name  : Composite_Status_In
 * Description    : Status IN routine
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Composite_Status_In(void)
{
	if (Request == SET_LINE_CODING)
	{
		Request = 0;
	}
}

/*******************************************************************************
 * Function Name  : Composite_Status_Out
 * Description    : Status OUT routine.
 * Input          : None.
 * Output         : None.
 * Return         : None.
 *******************************************************************************/
void Composite_Status_Out(void)
{
}

/*******************************************************************************
 * Function Name  : Composite_Data_Setup
 * Description    : Handle the data class specific requests
 * Input          : Request Nb
 * Output         : None.
 * Return         : USB_UNSUPPORT or USB_SUCCESS
 *******************************************************************************/
RESULT Composite_Data_Setup(uint8_t RequestNo)
{
	uint8_t* (*CopyRoutine)(uint16_t);
	CopyRoutine = NULL;

	if(((pInformation->USBbmRequestType & RECIPIENT) == INTERFACE_RECIPIENT && pInformation->USBwIndex == MSC_INTERFACE_IDX) ||
			((pInformation->USBbmRequestType & RECIPIENT) == ENDPOINT_RECIPIENT && ((pInformation->USBwIndex & 0x7F) == MSC_EP_IDX)))
	{ //handle MSC setup
		switch(RequestNo)
		{
		case GET_MAX_LUN:
		    CopyRoutine = MSD_Get_Max_Lun;
		    break;
		default:
			return USB_UNSUPPORT;
		}
	}
	else
	{ //handle CDC setup
		switch(RequestNo)
		{
		case GET_LINE_CODING:
			CopyRoutine = VCP_GetLineCoding;
			break;
		case SET_LINE_CODING:
			CopyRoutine = VCP_SetLineCoding;
			Request = SET_LINE_CODING;
			break;
		default:
			return USB_UNSUPPORT;
		}
	}

	if (CopyRoutine == NULL) //unlikely
	{
		return USB_UNSUPPORT;
	}

	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);
	return USB_SUCCESS;
}

/*******************************************************************************
 * Function Name  : Composite_NoData_Setup
 * Description    : handle the no data class specific requests
 * Input          : Request Nb
 * Output         : None
 * Return         : USB_UNSUPPORT or USB_SUCCESS
 *******************************************************************************/
RESULT Composite_NoData_Setup(uint8_t RequestNo)
{
	if ((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)))
	{
		switch (RequestNo)
		{
		case SET_COMM_FEATURE:
		case SET_CONTROL_LINE_STATE:
			return USB_SUCCESS;
		case MASS_STORAGE_RESET:
			/* Initialize MSC Endpoint*/
			ClearDTOG_TX(MSC_OUT_EP);
			ClearDTOG_RX(MSC_IN_EP);
			/*initialize the CBW signature to enable the clear feature*/
			CBW.dSignature = BOT_CBW_SIGNATURE;
			Bot_State = BOT_IDLE;
			return USB_SUCCESS;
		default:
			break;
		}
	}
	return USB_UNSUPPORT;
}

/*******************************************************************************
 * Function Name  : Composite_GetDeviceDescriptor
 * Description    : Gets the device descriptor
 * Input          : Length
 * Output         : None
 * Return         : The address of the device descriptor
 *******************************************************************************/
uint8_t* Composite_GetDeviceDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
 * Function Name  : Composite_GetConfigDescriptor
 * Description    : Gets the configuration descriptor
 * Input          : Length
 * Output         : None
 * Return         : The address of the configuration descriptor
 *******************************************************************************/
uint8_t* Composite_GetConfigDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
 * Function Name  : Composite_GetStringDescriptor
 * Description    : Gets the string descriptors according to the needed index
 * Input          : Length
 * Output         : None.
 * Return         : The address of the string descriptors.
 *******************************************************************************/
uint8_t* Composite_GetStringDescriptor(uint16_t Length)
{
	uint8_t wValue0 = pInformation->USBwValue0;
	if (wValue0 >= 4)
	{
		return NULL;
	}
	else
	{
		return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
	}
}

/*******************************************************************************
 * Function Name  : Composite_Get_Interface_Setting
 * Description    : tests the interface and the alternate setting according to the supported one.
 * Input          : - Interface : interface number
 *                  - AlternateSetting : Alternate Setting number
 * Output         : None.
 * Return         : USB_SUCCESS or USB_UNSUPPORT
 *******************************************************************************/
RESULT Composite_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
	if (AlternateSetting > 0)
	{
		return USB_UNSUPPORT; /* in this application we don't have AlternateSetting*/
	}
	else if (Interface > 1)
	{
		return USB_UNSUPPORT; /*in this application we have only 2 interfaces*/
	}
	return USB_SUCCESS;
}

/*******************************************************************************
 * Function Name  : Composite_ClearFeature
 * Description    : Handle the ClearFeature request
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void Composite_ClearFeature(void)
{
	/* when the host send a CBW with invalid signature or invalid length the two
	 Endpoints (IN & OUT) shall stall until receiving a Mass Storage Reset     */
	if (CBW.dSignature != BOT_CBW_SIGNATURE)
		Bot_Abort(BOTH_DIR);
}

/*******************************************************************************
* Function Name  : VCP_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *VCP_GetLineCoding(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
}

/*******************************************************************************
* Function Name  : VCP_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *VCP_SetLineCoding(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = sizeof(linecoding);
    return NULL;
  }
  return(uint8_t *)&linecoding;
}

/*******************************************************************************
* Function Name  : MSD_Get_Max_Lun
* Description    : Handle the Get Max Lun request.
* Input          : uint16_t Length.
* Output         : None.
* Return         : None.
*******************************************************************************/
uint8_t *MSD_Get_Max_Lun(uint16_t Length)
{
  if (Length == 0)
  {
    pInformation->Ctrl_Info.Usb_wLength = LUN_DATA_LENGTH;
    return 0;
  }
  else
  {
    return((uint8_t*)(&Max_Lun));
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
