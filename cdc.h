#ifndef CDC_H
#define CDC_H

#include <libopencm3/usb/cdc.h>
#include <libopencm3/usb/usbd.h>
#include <stdlib.h>
#include <stdint.h>

/* Serial ACM interface */
#define CDCACM_PACKET_SIZE 	128
#define CDCACM_UART_ENDPOINT	0x03
#define CDCACM_INTR_ENDPOINT	0x84

extern const struct usb_interface_descriptor uart_comm_iface[];
extern const struct usb_interface_descriptor uart_data_iface[];
extern const struct usb_iface_assoc_descriptor uart_assoc;

#endif // CDC_h