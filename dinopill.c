/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdlib.h>
#include <stdint.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <stdio.h>
#include "cdc.h"
#include "hid.h"
#include "adc.h"

#define OBJECT_BASELINE 420

static usbd_device *usbd_dev;
uint16_t sensor;

const struct usb_device_descriptor dev_descr = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x05ac,
	.idProduct = 0x0227,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = &hid_iface,
}, {
	.num_altsetting = 1,
	.iface_assoc = &uart_assoc,
	.altsetting = uart_comm_iface,
}, {
	.num_altsetting = 1,
	.altsetting = uart_data_iface,
}};

const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = sizeof(ifaces)/sizeof(ifaces[0]),
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0xC0,
	.bMaxPower = 0x32,

	.interface = ifaces,
};

static const char *usb_strings[] = {
	"nhtranngoc,. Inc",
	"STMDino",
	"ABC",
	"STMDino Serial Port"
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

static void usb_set_config(usbd_device *dev, uint16_t wValue) {
	hid_set_config(dev, wValue);
	cdcacm_set_config(dev, wValue);
}

int main(void) {
	rcc_clock_setup_in_hsi_out_48mhz();

	rcc_periph_clock_enable(RCC_GPIOA);
	/*
	 * This is a somewhat common cheap hack to trigger device re-enumeration
	 * on startup.  Assuming a fixed external pullup on D+, (For USB-FS)
	 * setting the pin to output, and driving it explicitly low effectively
	 * "removes" the pullup.  The subsequent USB init will "take over" the
	 * pin, and it will appear as a proper pullup to the host.
	 * The magic delay is somewhat arbitrary, no guarantees on USBIF
	 * compliance here, but "it works" in most places.
	 */
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
		GPIO_CNF_OUTPUT_PUSHPULL, GPIO12);
	gpio_clear(GPIOA, GPIO12);
	for (unsigned i = 0; i < 800000; i++) {
		__asm__("nop");
	}

	adc_setup();

	usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev_descr, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usbd_dev, usb_set_config);
	sensor = read_adc(0);

	while (1) {
		usbd_poll(usbd_dev);
	}
}

void sys_tick_handler(void) {
	// uint8_t buf[8] = { 0 };
	// buf[0] = 0; // shift
	// buf[1] = 0;
	// buf[2] = 0x04; // 'A'

	// usbd_ep_write_packet(usbd_dev, 0x81, buf, sizeof(buf));

	// // Release
	// buf[2] = 0;
	// usbd_ep_write_packet(usbd_dev, 0x81, buf, sizeof(buf));

	sensor = read_adc(0);
	char string[256];
	uint8_t len;

	if(sensor > OBJECT_BASELINE) {
		len = sprintf(string, "Obstacle.\n");
	} else {
		len = sprintf(string, "Normal...\n");
	}

	// uint8_t len = sprintf(string, "Light sensor: %d.\n", sensor);
	send_chunked_blocking(string, len, usbd_dev, CDCACM_UART_ENDPOINT, CDCACM_PACKET_SIZE);
}
