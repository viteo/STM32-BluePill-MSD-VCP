/*
 * virtualComPort.h
 *
 *  Created on: 11 oct 2021
 *      Author: v.simonenko
 */

#ifndef USB_VIRTUALCOMPORT_VIRTUALCOMPORT_H_
#define USB_VIRTUALCOMPORT_VIRTUALCOMPORT_H_

#include <stdint.h>

void VCP_AddToTxBuff(uint8_t received_byte);
void VCP_ProcessRxBuff(uint8_t *data_buffer, uint8_t Nb_bytes);
void VCP_SendTxBufPacketToUsb(void);
void Handle_USBAsynchXfer(void);

#endif /* USB_VIRTUALCOMPORT_VIRTUALCOMPORT_H_ */
