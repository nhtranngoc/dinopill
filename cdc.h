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

void cdcacm_data_rx_cb(usbd_device *dev, uint8_t ep);
void cdcacm_set_config(usbd_device *dev, uint16_t wValue);
void send_chunked_blocking(char *buf, int len, usbd_device *dev, int endpoint, int max_packet_length);
#endif // CDC_h