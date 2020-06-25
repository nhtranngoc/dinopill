#ifndef HID_H
#define HID_H

#include "stdint.h"
#include <stddef.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>

extern const uint8_t hid_report_descriptor[63];
extern const struct usb_interface_descriptor hid_iface;

void hid_set_config(usbd_device *dev, uint16_t wValue);

#endif // HID_H_