/**
  * This file is reworked version of usbd_desc.c
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __USBD_MIDI_DESC__C__
#define __USBD_MIDI_DESC__C__

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_def.h"

#define         DEVICE_ID1          (UID_BASE)
#define         DEVICE_ID2          (UID_BASE + 0x4)
#define         DEVICE_ID3          (UID_BASE + 0x8)

#define  USB_SIZ_STRING_SERIAL       0x1A

/** Descriptor for the Usb device. */
extern USBD_DescriptorsTypeDef FS_MidiDesc;

#ifdef __cplusplus
}
#endif

#endif /* __USBD_MIDI_DESC__C__ */

