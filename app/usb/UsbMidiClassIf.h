#ifndef __USB_MIDI_IF_H__
#define __USB_MIDI_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <usb/UsbMidiClass.h>

/* Define size for the receive and transmit buffer over MIDI */
#define APP_RX_DATA_SIZE  1024
#define APP_TX_DATA_SIZE  1024

/** MIDI Interface callback. */
extern UsbMidiClass_ItfTypeDef UsbMidiClass_Interface_fops_FS;


uint8_t MIDI_Transmit_FS(uint8_t* Buf, uint16_t Len);

#ifdef __cplusplus
}
#endif

#endif /* __USB_MIDI_IF_H__ */

