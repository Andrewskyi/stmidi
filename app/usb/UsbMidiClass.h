/**
  ******************************************************************************
  * @file    UsbMidiClass.h
  * @author  apaluch
  * @brief   header file for the UsbMidiClass.c file.
  ******************************************************************************
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */
#ifndef __USB_MIDI_CLASS_H__
#define __USB_MIDI_CLASS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include  "usbd_ioreq.h"

#define MIDI_IN_EP                                   0x81U  /* EP1 for data IN */
#define MIDI_OUT_EP                                  0x01U  /* EP1 for data OUT */

#ifndef MIDI_HS_BINTERVAL
#define MIDI_HS_BINTERVAL                          0x10U
#endif /* MIDI_HS_BINTERVAL */

#ifndef MIDI_FS_BINTERVAL
#define MIDI_FS_BINTERVAL                          0x10U
#endif /* MIDI_FS_BINTERVAL */

/* MIDI Endpoints parameters: you can fine tune these values depending on the needed baudrates and performance. */
#define MIDI_DATA_HS_MAX_PACKET_SIZE                 512U  /* Endpoint IN & OUT Packet size */
#define MIDI_DATA_FS_MAX_PACKET_SIZE                 64U  /* Endpoint IN & OUT Packet size */

#define USB_MIDI_CONFIG_DESC_SIZ                     101U
#define MIDI_DATA_HS_IN_PACKET_SIZE                  MIDI_DATA_HS_MAX_PACKET_SIZE
#define MIDI_DATA_HS_OUT_PACKET_SIZE                 MIDI_DATA_HS_MAX_PACKET_SIZE

#define MIDI_DATA_FS_IN_PACKET_SIZE                  MIDI_DATA_FS_MAX_PACKET_SIZE
#define MIDI_DATA_FS_OUT_PACKET_SIZE                 MIDI_DATA_FS_MAX_PACKET_SIZE


typedef struct
{
  uint32_t bitrate;
  uint8_t  format;
  uint8_t  paritytype;
  uint8_t  datatype;
} UsbMidiClass_LineCodingTypeDef;

typedef struct _UsbMidiClass_Itf
{
  int8_t (* Init)(void);
  int8_t (* DeInit)(void);
  int8_t (* Control)(uint8_t cmd, uint8_t *pbuf, uint16_t length);
  int8_t (* Receive)(uint8_t *Buf, uint32_t *Len);

} UsbMidiClass_ItfTypeDef;


typedef struct
{
  uint32_t data[MIDI_DATA_HS_MAX_PACKET_SIZE / 4U];      /* Force 32bits alignment */
  uint8_t  CmdOpCode;
  uint8_t  CmdLength;
  uint8_t  *RxBuffer;
  uint8_t  *TxBuffer;
  uint32_t RxLength;
  uint32_t TxLength;

  __IO uint32_t TxState;
  __IO uint32_t RxState;
}
UsbMidiClass_HandleTypeDef;


extern USBD_ClassTypeDef  UsbMidiClass;
#define UsbMidiClass_CLASS    &UsbMidiClass

uint8_t  UsbMidiClass_registerInterface(USBD_HandleTypeDef   *pdev,
                                    UsbMidiClass_ItfTypeDef *fops);

uint8_t  UsbMidiClass_SetTxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff,
                              uint16_t length);

uint8_t  UsbMidiClass_SetRxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff);

uint8_t  UsbMidiClass_ReceivePacket(USBD_HandleTypeDef *pdev);

uint8_t  UsbMidiClass_TransmitPacket(USBD_HandleTypeDef *pdev);

#ifdef __cplusplus
}
#endif

#endif  /* __USB_MIDI_CLASS_H__ */
