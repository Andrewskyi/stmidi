/**
  ******************************************************************************
  * @file    UsbMidiClass.c
  * @author  MCD Application Team
  * @brief   This file provides the high layer firmware functions to manage the
  *          following functionalities of the USB MIDI Class:
  *           - Initialization and Configuration of high and low layer
  *           - Enumeration as MIDI Device (and enumeration for each implemented memory interface)
  *           - OUT/IN data transfer
  *           - Command IN transfer (class requests management)
  *           - Error management
  *
  *  @verbatim
  *
  *          ===================================================================
  *                                MIDI Class Driver Description
  *          ===================================================================
  *           This driver manages the "Universal Serial Bus Class Definitions for Communications Devices
  *           Revision 1.2 November 16, 2007" and the sub-protocol specification of "Universal Serial Bus
  *           Communications Class Subclass Specification for PSTN Devices Revision 1.2 February 9, 2007"
  *           This driver implements the following aspects of the specification:
  *             - Device descriptor management
  *             - Configuration descriptor management
  *             - Enumeration as MIDI device with 2 data endpoints (IN and OUT) and 1 command endpoint (IN)
  *             - Requests management (as described in section 6.2 in specification)
  *             - Abstract Control Model compliant
  *             - Union Functional collection (using 1 IN endpoint for control)
  *             - Data interface class
  *
  *           These aspects may be enriched or modified for a specific user application.
  *
  *            This driver doesn't implement the following aspects of the specification
  *            (but it is possible to manage these features with some modifications on this driver):
  *             - Any class-specific aspect relative to communication classes should be managed by user application.
  *             - All communication classes other than PSTN are not managed
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include <usb/UsbMidiClass.h>
#include "usbd_ctlreq.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup UsbMidiClass
  * @brief usbd core module
  * @{
  */

/** @defgroup UsbMidiClass_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup UsbMidiClass_Private_Defines
  * @{
  */
/**
  * @}
  */


/** @defgroup UsbMidiClass_Private_Macros
  * @{
  */

/**
  * @}
  */


/** @defgroup UsbMidiClass_Private_FunctionPrototypes
  * @{
  */


static uint8_t  UsbMidiClass_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

static uint8_t  UsbMidiClass_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

static uint8_t  UsbMidiClass_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req);

static uint8_t  UsbMidiClass_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum);

static uint8_t  UsbMidiClass_DataOut(USBD_HandleTypeDef *pdev,
                                 uint8_t epnum);

static uint8_t  UsbMidiClass_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t  *UsbMidiClass_GetFSCfgDesc(uint16_t *length);

static uint8_t  *UsbMidiClass_GetHSCfgDesc(uint16_t *length);

static uint8_t  *UsbMidiClass_GetOtherSpeedCfgDesc(uint16_t *length);

static uint8_t  *UsbMidiClass_GetOtherSpeedCfgDesc(uint16_t *length);

uint8_t  *UsbMidiClass_GetDeviceQualifierDescriptor(uint16_t *length);

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t UsbMidiClass_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

/**
  * @}
  */

/** @defgroup UsbMidiClass_Private_Variables
  * @{
  */


/* MIDI interface class callbacks structure */
USBD_ClassTypeDef  UsbMidiClass =
{
  UsbMidiClass_Init,
  UsbMidiClass_DeInit,
  UsbMidiClass_Setup,
  NULL,                 /* EP0_TxSent, */
  UsbMidiClass_EP0_RxReady,
  UsbMidiClass_DataIn,
  UsbMidiClass_DataOut,
  NULL,
  NULL,
  NULL,
  UsbMidiClass_GetHSCfgDesc,
  UsbMidiClass_GetFSCfgDesc,
  UsbMidiClass_GetOtherSpeedCfgDesc,
  UsbMidiClass_GetDeviceQualifierDescriptor,
};

/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN uint8_t UsbMidiClass_CfgHSDesc[USB_MIDI_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /*Configuration Descriptor*/
  // _highspd_dscr
  0x09,   /* bLength: Configuration Descriptor size (highspd_dscr_realend-_highspd_dscr) */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  LOBYTE(USB_MIDI_CONFIG_DESC_SIZ), /* wTotalLength:no of returned bytes */
  HIBYTE(USB_MIDI_CONFIG_DESC_SIZ),
  0x02,   /* bNumInterfaces: 2 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xC0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */

  /*---------------------------------------------------------------------------*/
  /* Dummy audio control class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x00,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x00,   /* bNumEndpoints: 0 endpoints used */
  0x01,   /* bInterfaceClass */
  0x01,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x00,   /* iInterface: */

  /* Class-specific AC Interface Descriptor (HEADER) */
  0x09, /* heder length */
  0x24, /* CS_INTERFACE */
  0x01, /* HEADER subtype */
  0x00, /* bcdADC = 1.0 */
  0x01,
  0x09, /* wTotalLength LSB (here only header) */
  0x00, /* wTotalLength MSB */
  0x01, /* bInCollection (number of streaming interfaces) */
  0x01, /* baInterfaceNr(1) = 1 (MIDI Streaming) */

  // =========================================================
  // MIDI Streaming Interface (standard header, class-specific header)
  // =========================================================
  0x09, /* interface desc length */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x01, /* bInterfaceNumber = 1 */
  0x00, /* bAlternateSetting */
  0x02, /* bNumEndpoints = 2 (OUT + IN) */
  0x01, /* bInterfaceClass = AUDIO */
  0x03, /* bInterfaceSubClass = MIDI_STREAMING */
  0x00, /* bInterfaceProtocol */
  0x00, /* iInterface */

  // _MS_Header:
  0x07, // length (_MS_HeaderEnd - _MS_Header)
  0x24, // CS_INTERFACE
  0x01, // MS_HEADER
  0x00,
  0x01, // bcdMSC = 1.0
  LOBYTE(37/*(_MS_ClassEnd - _MS_Header)*/), //wTotalLength of class-specific
  HIBYTE(37/*(_MS_ClassEnd - _MS_Header)*/), //MS descriptors (from _MS_Header to _MS_ClassEnd)
  //_MS_HeaderEnd:

  // =========================
  // MIDI class-specific descriptors (jacks)
  // =========================
  //_MIDI_InJackEmb:
  0x06, // (_MIDI_InJackEmbEnd - _MIDI_InJackEmb)
  0x24, // CS_INTERFACE
  0x02, // MIDI_IN_JACK
  0x01, // JackType = EMBEDDED
  0x01, // bJackID = 1
  0x04, // iJack
  //_MIDI_InJackEmbEnd:

  //_MIDI_InJackExt:
  0x06, // (_MIDI_InJackExtEnd - _MIDI_InJackExt)
  0x24, // CS_INTERFACE
  0x02, // MIDI_IN_JACK
  0x02, // JackType = EXTERNAL
  0x02, // bJackID = 2
  0x04, // iJack
  //_MIDI_InJackExtEnd:

  //_MIDI_OutJackEmb:
  0x09, // (_MIDI_OutJackEmbEnd - _MIDI_OutJackEmb)
  0x24, // CS_INTERFACE
  0x03, // MIDI_OUT_JACK
  0x01, // JackType = EMBEDDED
  0x03, // bJackID = 3
  0x01, // bNrInputPins
  0x02, // BaSourceID(1) = 2  (from external IN jack)
  0x01, // BaSourcePin(1) = 1
  0x00, // iJack
  //_MIDI_OutJackEmbEnd:

  // _MIDI_OutJackExt:
  0x09, // (_MIDI_OutJackExtEnd - _MIDI_OutJackExt)
  0x24, // CS_INTERFACE
  0x03, // MIDI_OUT_JACK
  0x02, // JackType = EXTERNAL
  0x04, // bJackID = 4
  0x01, // bNrInputPins
  0x01, // BaSourceID(1) = 1 (from emb IN Jack)
  0x01, // BaSourcePin(1) = 1
  0x00, // iJack
  //_MIDI_OutJackExtEnd:

  // mark end of MS class-specific block (used for MS header wTotalLength)
  // _MS_ClassEnd:

  // =========================================================
  // Endpoint: EP1 OUT (Host -> Device)
  // =========================================================
  //_EP1_OUT:
  0x09, // (_EP1_OUTEnd - _EP1_OUT)
  USB_DESC_TYPE_ENDPOINT, // ENDPOINT
  MIDI_OUT_EP, // bEndpointAddress
  0x02, // bmAttributes = Bulk
  LOBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),
  0x00, // bInterval (ignored for bulk)
  0x00, // bRefresh
  0x00, // bSynchAddress
  //_EP1_OUTEnd:

  //_EP1_OUT_Class:
  0x05, // (_EP1_OUT_ClassEnd - _EP1_OUT_Class)
  0x25, // CS_ENDPOINT
  0x01, // MS_GENERAL
  0x01, // bNumEmbMIDIJack
  0x01, // BaAssocJackID(1) = 1  (embedded IN jack)
  //_EP1_OUT_ClassEnd:

  // =========================================================
  // Endpoint: EP1 IN (Device -> Host)
  // =========================================================
  //_EP1_IN:
  0x09, // (_EP1_INEnd - _EP1_IN)
  USB_DESC_TYPE_ENDPOINT, // ENDPOINT
  MIDI_IN_EP, // bEndpointAddress
  0x02, // bmAttributes = Bulk
  LOBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),
  0x00, // bInterval: ignore for Bulk transfer
  0x00, // bRefresh
  0x00, // bSynchAddress
  //_EP1_INEnd:

  //_EP1_IN_Class:
  0x05, // (_EP1_IN_ClassEnd - _EP1_IN_Class)
  0x25, // CS_ENDPOINT
  0x01, // MS_GENERAL
  0x01, // bNumEmbMIDIJack
  0x03 // BaAssocJackID(1) = 3  (embedded OUT jack)
  //_EP1_IN_ClassEnd:
  // highspd_dscr_realend
} ;


/* USB MIDI device Configuration Descriptor */
__ALIGN_BEGIN uint8_t UsbMidiClass_CfgFSDesc[USB_MIDI_CONFIG_DESC_SIZ] __ALIGN_END =
{
	/*Configuration Descriptor*/
	// _highspd_dscr
	0x09,   /* bLength: Configuration Descriptor size (highspd_dscr_realend-_highspd_dscr) */
	USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
	LOBYTE(USB_MIDI_CONFIG_DESC_SIZ), /* wTotalLength:no of returned bytes */
	HIBYTE(USB_MIDI_CONFIG_DESC_SIZ),
	0x02,   /* bNumInterfaces: 2 interface */
	0x01,   /* bConfigurationValue: Configuration value */
	0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
	0xC0,   /* bmAttributes: self powered */
	0x32,   /* MaxPower 0 mA */

	/*---------------------------------------------------------------------------*/
	/* Dummy audio control class interface descriptor*/
	0x09,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
	0x00,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x00,   /* bNumEndpoints: 0 endpoints used */
	0x01,   /* bInterfaceClass */
	0x01,   /* bInterfaceSubClass: */
	0x00,   /* bInterfaceProtocol: */
	0x00,   /* iInterface: */

	/* Class-specific AC Interface Descriptor (HEADER) */
	0x09, /* heder length */
	0x24, /* CS_INTERFACE */
	0x01, /* HEADER subtype */
	0x00, /* bcdADC = 1.0 */
	0x01,
	0x09, /* wTotalLength LSB (here only header) */
	0x00, /* wTotalLength MSB */
	0x01, /* bInCollection (number of streaming interfaces) */
	0x01, /* baInterfaceNr(1) = 1 (MIDI Streaming) */

	// =========================================================
	// MIDI Streaming Interface (standard header, class-specific header)
	// =========================================================
	0x09, /* interface desc length */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
	0x01, /* bInterfaceNumber = 1 */
	0x00, /* bAlternateSetting */
	0x02, /* bNumEndpoints = 2 (OUT + IN) */
	0x01, /* bInterfaceClass = AUDIO */
	0x03, /* bInterfaceSubClass = MIDI_STREAMING */
	0x00, /* bInterfaceProtocol */
	0x00, /* iInterface */

	// _MS_Header:
	0x07, // length (_MS_HeaderEnd - _MS_Header)
	0x24, // CS_INTERFACE
	0x01, // MS_HEADER
	0x00,
	0x01, // bcdMSC = 1.0
	LOBYTE(37/*(_MS_ClassEnd - _MS_Header)*/), //wTotalLength of class-specific
	HIBYTE(37/*(_MS_ClassEnd - _MS_Header)*/), //MS descriptors (from _MS_Header to _MS_ClassEnd)
	//_MS_HeaderEnd:

	// =========================
	// MIDI class-specific descriptors (jacks)
	// =========================
	//_MIDI_InJackEmb:
	0x06, // (_MIDI_InJackEmbEnd - _MIDI_InJackEmb)
	0x24, // CS_INTERFACE
	0x02, // MIDI_IN_JACK
	0x01, // JackType = EMBEDDED
	0x01, // bJackID = 1
	0x04, // iJack
	//_MIDI_InJackEmbEnd:

	//_MIDI_InJackExt:
	0x06, // (_MIDI_InJackExtEnd - _MIDI_InJackExt)
	0x24, // CS_INTERFACE
	0x02, // MIDI_IN_JACK
	0x02, // JackType = EXTERNAL
	0x02, // bJackID = 2
	0x04, // iJack
	//_MIDI_InJackExtEnd:

	//_MIDI_OutJackEmb:
	0x09, // (_MIDI_OutJackEmbEnd - _MIDI_OutJackEmb)
	0x24, // CS_INTERFACE
	0x03, // MIDI_OUT_JACK
	0x01, // JackType = EMBEDDED
	0x03, // bJackID = 3
	0x01, // bNrInputPins
	0x02, // BaSourceID(1) = 2  (from external IN jack)
	0x01, // BaSourcePin(1) = 1
	0x00, // iJack
	//_MIDI_OutJackEmbEnd:

	// _MIDI_OutJackExt:
	0x09, // (_MIDI_OutJackExtEnd - _MIDI_OutJackExt)
	0x24, // CS_INTERFACE
	0x03, // MIDI_OUT_JACK
	0x02, // JackType = EXTERNAL
	0x04, // bJackID = 4
	0x01, // bNrInputPins
	0x01, // BaSourceID(1) = 1 (from emb IN Jack)
	0x01, // BaSourcePin(1) = 1
	0x00, // iJack
	//_MIDI_OutJackExtEnd:

	// mark end of MS class-specific block (used for MS header wTotalLength)
	// _MS_ClassEnd:

	// =========================================================
	// Endpoint: EP1 OUT (Host -> Device)
	// =========================================================
	//_EP1_OUT:
	0x09, // (_EP1_OUTEnd - _EP1_OUT)
	USB_DESC_TYPE_ENDPOINT, // ENDPOINT
	MIDI_OUT_EP, // bEndpointAddress
	0x02, // bmAttributes = Bulk
	LOBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE),
	0x00, // bInterval (ignored for bulk)
	0x00, // bRefresh
	0x00, // bSynchAddress
	//_EP1_OUTEnd:

	//_EP1_OUT_Class:
	0x05, // (_EP1_OUT_ClassEnd - _EP1_OUT_Class)
	0x25, // CS_ENDPOINT
	0x01, // MS_GENERAL
	0x01, // bNumEmbMIDIJack
	0x01, // BaAssocJackID(1) = 1  (embedded IN jack)
	//_EP1_OUT_ClassEnd:

	// =========================================================
	// Endpoint: EP1 IN (Device -> Host)
	// =========================================================
	//_EP1_IN:
	0x09, // (_EP1_INEnd - _EP1_IN)
	USB_DESC_TYPE_ENDPOINT, // ENDPOINT
	MIDI_IN_EP, // bEndpointAddress
	0x02, // bmAttributes = Bulk
	LOBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE),
	0x00, // bInterval: ignore for Bulk transfer
	0x00, // bRefresh
	0x00, // bSynchAddress
	//_EP1_INEnd:

	//_EP1_IN_Class:
	0x05, // (_EP1_IN_ClassEnd - _EP1_IN_Class)
	0x25, // CS_ENDPOINT
	0x01, // MS_GENERAL
	0x01, // bNumEmbMIDIJack
	0x03 // BaAssocJackID(1) = 3  (embedded OUT jack)
	//_EP1_IN_ClassEnd:
	// highspd_dscr_realend
} ;

__ALIGN_BEGIN uint8_t UsbMidiClass_OtherSpeedCfgDesc[USB_MIDI_CONFIG_DESC_SIZ] __ALIGN_END =
{
	/*Configuration Descriptor*/
	// _highspd_dscr
	0x09,   /* bLength: Configuration Descriptor size (highspd_dscr_realend-_highspd_dscr) */
	USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
	LOBYTE(USB_MIDI_CONFIG_DESC_SIZ), /* wTotalLength:no of returned bytes */
	HIBYTE(USB_MIDI_CONFIG_DESC_SIZ),
	0x02,   /* bNumInterfaces: 2 interface */
	0x01,   /* bConfigurationValue: Configuration value */
	0x00,   /* iConfiguration: Index of string descriptor describing the configuration */
	0xC0,   /* bmAttributes: self powered */
	0x32,   /* MaxPower 0 mA */

	/*---------------------------------------------------------------------------*/
	/* Dummy audio control class interface descriptor*/
	0x09,   /* bLength: Endpoint Descriptor size */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
	0x00,   /* bInterfaceNumber: Number of Interface */
	0x00,   /* bAlternateSetting: Alternate setting */
	0x00,   /* bNumEndpoints: 0 endpoints used */
	0x01,   /* bInterfaceClass */
	0x01,   /* bInterfaceSubClass: */
	0x00,   /* bInterfaceProtocol: */
	0x00,   /* iInterface: */

	/* Class-specific AC Interface Descriptor (HEADER) */
	0x09, /* heder length */
	0x24, /* CS_INTERFACE */
	0x01, /* HEADER subtype */
	0x00, /* bcdADC = 1.0 */
	0x01,
	0x09, /* wTotalLength LSB (here only header) */
	0x00, /* wTotalLength MSB */
	0x01, /* bInCollection (number of streaming interfaces) */
	0x01, /* baInterfaceNr(1) = 1 (MIDI Streaming) */

	// =========================================================
	// MIDI Streaming Interface (standard header, class-specific header)
	// =========================================================
	0x09, /* interface desc length */
	USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
	0x01, /* bInterfaceNumber = 1 */
	0x00, /* bAlternateSetting */
	0x02, /* bNumEndpoints = 2 (OUT + IN) */
	0x01, /* bInterfaceClass = AUDIO */
	0x03, /* bInterfaceSubClass = MIDI_STREAMING */
	0x00, /* bInterfaceProtocol */
	0x00, /* iInterface */

	// _MS_Header:
	0x07, // length (_MS_HeaderEnd - _MS_Header)
	0x24, // CS_INTERFACE
	0x01, // MS_HEADER
	0x00,
	0x01, // bcdMSC = 1.0
	LOBYTE(37/*(_MS_ClassEnd - _MS_Header)*/), //wTotalLength of class-specific
	HIBYTE(37/*(_MS_ClassEnd - _MS_Header)*/), //MS descriptors (from _MS_Header to _MS_ClassEnd)
	//_MS_HeaderEnd:

	// =========================
	// MIDI class-specific descriptors (jacks)
	// =========================
	//_MIDI_InJackEmb:
	0x06, // (_MIDI_InJackEmbEnd - _MIDI_InJackEmb)
	0x24, // CS_INTERFACE
	0x02, // MIDI_IN_JACK
	0x01, // JackType = EMBEDDED
	0x01, // bJackID = 1
	0x04, // iJack
	//_MIDI_InJackEmbEnd:

	//_MIDI_InJackExt:
	0x06, // (_MIDI_InJackExtEnd - _MIDI_InJackExt)
	0x24, // CS_INTERFACE
	0x02, // MIDI_IN_JACK
	0x02, // JackType = EXTERNAL
	0x02, // bJackID = 2
	0x04, // iJack
	//_MIDI_InJackExtEnd:

	//_MIDI_OutJackEmb:
	0x09, // (_MIDI_OutJackEmbEnd - _MIDI_OutJackEmb)
	0x24, // CS_INTERFACE
	0x03, // MIDI_OUT_JACK
	0x01, // JackType = EMBEDDED
	0x03, // bJackID = 3
	0x01, // bNrInputPins
	0x02, // BaSourceID(1) = 2  (from external IN jack)
	0x01, // BaSourcePin(1) = 1
	0x00, // iJack
	//_MIDI_OutJackEmbEnd:

	// _MIDI_OutJackExt:
	0x09, // (_MIDI_OutJackExtEnd - _MIDI_OutJackExt)
	0x24, // CS_INTERFACE
	0x03, // MIDI_OUT_JACK
	0x02, // JackType = EXTERNAL
	0x04, // bJackID = 4
	0x01, // bNrInputPins
	0x01, // BaSourceID(1) = 1 (from emb IN Jack)
	0x01, // BaSourcePin(1) = 1
	0x00, // iJack
	//_MIDI_OutJackExtEnd:

	// mark end of MS class-specific block (used for MS header wTotalLength)
	// _MS_ClassEnd:

	// =========================================================
	// Endpoint: EP1 OUT (Host -> Device)
	// =========================================================
	//_EP1_OUT:
	0x09, // (_EP1_OUTEnd - _EP1_OUT)
	USB_DESC_TYPE_ENDPOINT, // ENDPOINT
	MIDI_OUT_EP, // bEndpointAddress
	0x02, // bmAttributes = Bulk
	LOBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),
	0x00, // bInterval (ignored for bulk)
	0x00, // bRefresh
	0x00, // bSynchAddress
	//_EP1_OUTEnd:

	//_EP1_OUT_Class:
	0x05, // (_EP1_OUT_ClassEnd - _EP1_OUT_Class)
	0x25, // CS_ENDPOINT
	0x01, // MS_GENERAL
	0x01, // bNumEmbMIDIJack
	0x01, // BaAssocJackID(1) = 1  (embedded IN jack)
	//_EP1_OUT_ClassEnd:

	// =========================================================
	// Endpoint: EP1 IN (Device -> Host)
	// =========================================================
	//_EP1_IN:
	0x09, // (_EP1_INEnd - _EP1_IN)
	USB_DESC_TYPE_ENDPOINT, // ENDPOINT
	MIDI_IN_EP, // bEndpointAddress
	0x02, // bmAttributes = Bulk
	LOBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
	HIBYTE(MIDI_DATA_HS_MAX_PACKET_SIZE),
	0x00, // bInterval: ignore for Bulk transfer
	0x00, // bRefresh
	0x00, // bSynchAddress
	//_EP1_INEnd:

	//_EP1_IN_Class:
	0x05, // (_EP1_IN_ClassEnd - _EP1_IN_Class)
	0x25, // CS_ENDPOINT
	0x01, // MS_GENERAL
	0x01, // bNumEmbMIDIJack
	0x03 // BaAssocJackID(1) = 3  (embedded OUT jack)
	//_EP1_IN_ClassEnd:
	// highspd_dscr_realend
};

/**
  * @}
  */

/** @defgroup UsbMidiClass_Private_Functions
  * @{
  */

/**
  * @brief  UsbMidiClass_Init
  *         Initialize the MIDI interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  UsbMidiClass_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0U;
  UsbMidiClass_HandleTypeDef   *hmidi;

  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Open EP IN */
    USBD_LL_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_HS_IN_PACKET_SIZE);

    pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 1U;

    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_HS_OUT_PACKET_SIZE);

    pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 1U;

  }
  else
  {
    /* Open EP IN */
    USBD_LL_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_FS_IN_PACKET_SIZE);

    pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 1U;

    /* Open EP OUT */
    USBD_LL_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK,
                   MIDI_DATA_FS_OUT_PACKET_SIZE);

    pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 1U;
  }
  /* Open Command IN EP */
  USBD_LL_OpenEP(pdev, MIDI_CMD_EP, USBD_EP_TYPE_INTR, MIDI_CMD_PACKET_SIZE);
  pdev->ep_in[MIDI_CMD_EP & 0xFU].is_used = 1U;

  pdev->pClassData = USBD_malloc(sizeof(UsbMidiClass_HandleTypeDef));

  if (pdev->pClassData == NULL)
  {
    ret = 1U;
  }
  else
  {
    hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;

    /* Init  physical Interface components */
    ((UsbMidiClass_ItfTypeDef *)pdev->pUserData)->Init();

    /* Init Xfer states */
    hmidi->TxState = 0U;
    hmidi->RxState = 0U;

    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP, hmidi->RxBuffer,
                             MIDI_DATA_HS_OUT_PACKET_SIZE);
    }
    else
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP, hmidi->RxBuffer,
                             MIDI_DATA_FS_OUT_PACKET_SIZE);
    }
  }
  return ret;
}

/**
  * @brief  UsbMidiClass_Init
  *         DeInitialize the MIDI layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
static uint8_t  UsbMidiClass_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  uint8_t ret = 0U;

  /* Close EP IN */
  USBD_LL_CloseEP(pdev, MIDI_IN_EP);
  pdev->ep_in[MIDI_IN_EP & 0xFU].is_used = 0U;

  /* Close EP OUT */
  USBD_LL_CloseEP(pdev, MIDI_OUT_EP);
  pdev->ep_out[MIDI_OUT_EP & 0xFU].is_used = 0U;

  /* Close Command IN EP */
  USBD_LL_CloseEP(pdev, MIDI_CMD_EP);
  pdev->ep_in[MIDI_CMD_EP & 0xFU].is_used = 0U;

  /* DeInit  physical Interface components */
  if (pdev->pClassData != NULL)
  {
    ((UsbMidiClass_ItfTypeDef *)pdev->pUserData)->DeInit();
    USBD_free(pdev->pClassData);
    pdev->pClassData = NULL;
  }

  return ret;
}

/**
  * @brief  UsbMidiClass_Setup
  *         Handle the MIDI specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
static uint8_t  UsbMidiClass_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req)
{
  UsbMidiClass_HandleTypeDef   *hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;
  uint8_t ifalt = 0U;
  uint16_t status_info = 0U;
  uint8_t ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS :
      if (req->wLength)
      {
        if (req->bmRequest & 0x80U)
        {
          ((UsbMidiClass_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
                                                            (uint8_t *)(void *)hmidi->data,
                                                            req->wLength);

          USBD_CtlSendData(pdev, (uint8_t *)(void *)hmidi->data, req->wLength);
        }
        else
        {
          hmidi->CmdOpCode = req->bRequest;
          hmidi->CmdLength = (uint8_t)req->wLength;

          USBD_CtlPrepareRx(pdev, (uint8_t *)(void *)hmidi->data, req->wLength);
        }
      }
      else
      {
        ((UsbMidiClass_ItfTypeDef *)pdev->pUserData)->Control(req->bRequest,
                                                          (uint8_t *)(void *)req, 0U);
      }
      break;

    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_INTERFACE:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, &ifalt, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE:
          if (pdev->dev_state != USBD_STATE_CONFIGURED)
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return ret;
}

/**
  * @brief  UsbMidiClass_DataIn
  *         Data sent on non-control IN endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  UsbMidiClass_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UsbMidiClass_HandleTypeDef *hmidi = (UsbMidiClass_HandleTypeDef *)pdev->pClassData;
  PCD_HandleTypeDef *hpcd = pdev->pData;

  if (pdev->pClassData != NULL)
  {
    if ((pdev->ep_in[epnum].total_length > 0U) && ((pdev->ep_in[epnum].total_length % hpcd->IN_ep[epnum].maxpacket) == 0U))
    {
      /* Update the packet total length */
      pdev->ep_in[epnum].total_length = 0U;

      /* Send ZLP */
      USBD_LL_Transmit(pdev, epnum, NULL, 0U);
    }
    else
    {
      hmidi->TxState = 0U;
    }
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

/**
  * @brief  UsbMidiClass_DataOut
  *         Data received on non-control Out endpoint
  * @param  pdev: device instance
  * @param  epnum: endpoint number
  * @retval status
  */
static uint8_t  UsbMidiClass_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
  UsbMidiClass_HandleTypeDef   *hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;

  /* Get the received data length */
  hmidi->RxLength = USBD_LL_GetRxDataSize(pdev, epnum);

  /* USB data will be immediately processed, this allow next USB traffic being
  NAKed till the end of the application Xfer */
  if (pdev->pClassData != NULL)
  {
    ((UsbMidiClass_ItfTypeDef *)pdev->pUserData)->Receive(hmidi->RxBuffer, &hmidi->RxLength);

    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}

/**
  * @brief  UsbMidiClass_EP0_RxReady
  *         Handle EP0 Rx Ready event
  * @param  pdev: device instance
  * @retval status
  */
static uint8_t  UsbMidiClass_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
  UsbMidiClass_HandleTypeDef   *hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;

  if ((pdev->pUserData != NULL) && (hmidi->CmdOpCode != 0xFFU))
  {
    ((UsbMidiClass_ItfTypeDef *)pdev->pUserData)->Control(hmidi->CmdOpCode,
                                                      (uint8_t *)(void *)hmidi->data,
                                                      (uint16_t)hmidi->CmdLength);
    hmidi->CmdOpCode = 0xFFU;

  }
  return USBD_OK;
}

/**
  * @brief  UsbMidiClass_GetFSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *UsbMidiClass_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(UsbMidiClass_CfgFSDesc);
  return UsbMidiClass_CfgFSDesc;
}

/**
  * @brief  UsbMidiClass_GetHSCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *UsbMidiClass_GetHSCfgDesc(uint16_t *length)
{
  *length = sizeof(UsbMidiClass_CfgHSDesc);
  return UsbMidiClass_CfgHSDesc;
}

/**
  * @brief  UsbMidiClass_GetCfgDesc
  *         Return configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
static uint8_t  *UsbMidiClass_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = sizeof(UsbMidiClass_OtherSpeedCfgDesc);
  return UsbMidiClass_OtherSpeedCfgDesc;
}

/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
uint8_t  *UsbMidiClass_GetDeviceQualifierDescriptor(uint16_t *length)
{
  *length = sizeof(UsbMidiClass_DeviceQualifierDesc);
  return UsbMidiClass_DeviceQualifierDesc;
}

/**
* @brief  UsbMidiClass_RegisterInterface
  * @param  pdev: device instance
  * @param  fops: CD  Interface callback
  * @retval status
  */
uint8_t  UsbMidiClass_registerInterface(USBD_HandleTypeDef   *pdev,
                                    UsbMidiClass_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;

  if (fops != NULL)
  {
    pdev->pUserData = fops;
    ret = USBD_OK;
  }

  return ret;
}

/**
  * @brief  UsbMidiClass_SetTxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Tx Buffer
  * @retval status
  */
uint8_t  UsbMidiClass_SetTxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff,
                              uint16_t length)
{
  UsbMidiClass_HandleTypeDef   *hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;

  hmidi->TxBuffer = pbuff;
  hmidi->TxLength = length;

  return USBD_OK;
}


/**
  * @brief  UsbMidiClass_SetRxBuffer
  * @param  pdev: device instance
  * @param  pbuff: Rx Buffer
  * @retval status
  */
uint8_t  UsbMidiClass_SetRxBuffer(USBD_HandleTypeDef   *pdev,
                              uint8_t  *pbuff)
{
  UsbMidiClass_HandleTypeDef   *hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;

  hmidi->RxBuffer = pbuff;

  return USBD_OK;
}

/**
  * @brief  UsbMidiClass_TransmitPacket
  *         Transmit packet on IN endpoint
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  UsbMidiClass_TransmitPacket(USBD_HandleTypeDef *pdev)
{
  UsbMidiClass_HandleTypeDef   *hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;

  if (pdev->pClassData != NULL)
  {
    if (hmidi->TxState == 0U)
    {
      /* Tx Transfer in progress */
      hmidi->TxState = 1U;

      /* Update the packet total length */
      pdev->ep_in[MIDI_IN_EP & 0xFU].total_length = hmidi->TxLength;

      /* Transmit next packet */
      USBD_LL_Transmit(pdev, MIDI_IN_EP, hmidi->TxBuffer,
                       (uint16_t)hmidi->TxLength);

      return USBD_OK;
    }
    else
    {
      return USBD_BUSY;
    }
  }
  else
  {
    return USBD_FAIL;
  }
}


/**
  * @brief  UsbMidiClass_ReceivePacket
  *         prepare OUT Endpoint for reception
  * @param  pdev: device instance
  * @retval status
  */
uint8_t  UsbMidiClass_ReceivePacket(USBD_HandleTypeDef *pdev)
{
  UsbMidiClass_HandleTypeDef   *hmidi = (UsbMidiClass_HandleTypeDef *) pdev->pClassData;

  /* Suspend or Resume USB Out process */
  if (pdev->pClassData != NULL)
  {
    if (pdev->dev_speed == USBD_SPEED_HIGH)
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             MIDI_OUT_EP,
                             hmidi->RxBuffer,
                             MIDI_DATA_HS_OUT_PACKET_SIZE);
    }
    else
    {
      /* Prepare Out endpoint to receive next packet */
      USBD_LL_PrepareReceive(pdev,
                             MIDI_OUT_EP,
                             hmidi->RxBuffer,
                             MIDI_DATA_FS_OUT_PACKET_SIZE);
    }
    return USBD_OK;
  }
  else
  {
    return USBD_FAIL;
  }
}
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
