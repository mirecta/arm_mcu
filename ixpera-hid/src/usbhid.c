/*
 * This file is part of the libopencm3 project.
 *
 * Copyright (C) 2010 Gareth McMullin <gareth@blacksphere.co.nz>
 * Copyright (C) 2011 Piotr Esden-Tempski <piotr@esden.net>
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
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/spi.h>
#include <libopencm3/usb/dwc/otg_fs.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>


/* Define this to include the DFU APP interface. */
//#define INCLUDE_DFU_INTERFACE

#ifdef INCLUDE_DFU_INTERFACE
#include <libopencm3/cm3/scb.h>
#include <libopencm3/usb/dfu.h>
#endif

static usbd_device *usbd_dev;
void init_io(void);
void read_buttons(void);

const struct usb_device_descriptor dev_descr = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = 64,
	.idVendor = 0x0483,
	.idProduct = 0x5710,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};

static const uint8_t hid_report_descriptor[] = {
		0x05, 0x01,                         // Usage Page (Generic Desktop)
		0x09, 0x06,                         // Usage (Keyboard)
		0xA1, 0x01,                         // Collection (Application)
		0x05, 0x07,                         //     Usage Page (Key Codes)
		0x19, 0xe0,                         //     Usage Minimum (224)
		0x29, 0xe7,                         //     Usage Maximum (231)
		0x15, 0x00,                         //     Logical Minimum (0)
		0x25, 0x01,                         //     Logical Maximum (1)
		0x75, 0x01,                         //     Report Size (1)
		0x95, 0x08,                         //     Report Count (8)
		0x81, 0x02,                         //     Input (Data, Variable, Absolute)

		0x95, 0x01,                         //     Report Count (1)
		0x75, 0x08,                         //     Report Size (8)
		0x81, 0x01,                         //     Input (Constant) reserved byte(1)
/*
		0x95, 0x05,                         //     Report Count (5)
		0x75, 0x01,                         //     Report Size (1)
		0x05, 0x08,                         //     Usage Page (Page# for LEDs)
		0x19, 0x01,                         //     Usage Minimum (1)
		0x29, 0x05,                         //     Usage Maximum (5)
		0x91, 0x02,                         //     Output (Data, Variable, Absolute), Led report
		0x95, 0x01,                         //     Report Count (1)
		0x75, 0x03,                         //     Report Size (3)
		0x91, 0x01,                         //     Output (Data, Variable, Absolute), Led report padding
*/
		0x95, 0x06,                         //     Report Count (6)
		0x75, 0x08,                         //     Report Size (8)
		0x15, 0x00,                         //     Logical Minimum (0)
		0x25, 0x65,                         //     Logical Maximum (101)
		0x05, 0x07,                         //     Usage Page (Key codes)
		0x19, 0x00,                         //     Usage Minimum (0)
		0x29, 0x65,                         //     Usage Maximum (101)
		0x81, 0x00,                         //     Input (Data, Array) Key array(6 bytes)


		0x09, 0x05,                         //     Usage (Vendor Defined)
		0x15, 0x00,                         //     Logical Minimum (0)
		0x26, 0xFF,0x00,                     //     Logical Maximum (255)
		0x95, 0x06,                         //     Report Count (6)
		0x75, 0x08,                         //     Report Size (8)
		0x81, 0x00,                         //     Input (Data, Array) Key array(6 bytes)

		0xC0                                // End Collection (Application)

	
};

static const struct {
	struct usb_hid_descriptor hid_descriptor;
	struct {
		uint8_t bReportDescriptorType;
		uint16_t wDescriptorLength;
	} __attribute__((packed)) hid_report;
} __attribute__((packed)) hid_function = {
	.hid_descriptor = {
		.bLength = sizeof(hid_function),
		.bDescriptorType = USB_DT_HID,
		.bcdHID = 0x0100,
		.bCountryCode = 0,
		.bNumDescriptors = 1,
	},
	.hid_report = {
		.bReportDescriptorType = USB_DT_REPORT,
		.wDescriptorLength = sizeof(hid_report_descriptor),
	}
};

const struct usb_endpoint_descriptor hid_endpoint = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x81,
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize = 14,
	.bInterval = 0x02,
};

const struct usb_interface_descriptor hid_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_HID,
	.bInterfaceSubClass = 1, /* boot */
	.bInterfaceProtocol = 1, /* keyboard */
	.iInterface = 0,

	.endpoint = &hid_endpoint,

	.extra = &hid_function,
	.extralen = sizeof(hid_function),
};

#ifdef INCLUDE_DFU_INTERFACE
const struct usb_dfu_descriptor dfu_function = {
	.bLength = sizeof(struct usb_dfu_descriptor),
	.bDescriptorType = DFU_FUNCTIONAL,
	.bmAttributes = USB_DFU_CAN_DOWNLOAD | USB_DFU_WILL_DETACH,
	.wDetachTimeout = 255,
	.wTransferSize = 1024,
	.bcdDFUVersion = 0x011A,
};

const struct usb_interface_descriptor dfu_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 1,
	.bAlternateSetting = 0,
	.bNumEndpoints = 0,
	.bInterfaceClass = 0xFE,
	.bInterfaceSubClass = 1,
	.bInterfaceProtocol = 1,
	.iInterface = 0,

	.extra = &dfu_function,
	.extralen = sizeof(dfu_function),
};
#endif

const struct usb_interface ifaces[] = {{
	.num_altsetting = 1,
	.altsetting = &hid_iface,
#ifdef INCLUDE_DFU_INTERFACE
}, {
	.num_altsetting = 1,
	.altsetting = &dfu_iface,
#endif
}};

const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
#ifdef INCLUDE_DFU_INTERFACE
	.bNumInterfaces = 2,
#else
	.bNumInterfaces = 1,
#endif
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0xC0,
	.bMaxPower = 0x32,

	.interface = ifaces,
};

static const char *usb_strings[] = {
	"IXPERTA",
	"HID Keyboard",
	"KEYBOARD 01",
};

/* Buffer used for control requests. */
uint8_t usbd_control_buffer[128];

static enum usbd_request_return_codes hid_control_request(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *dev, struct usb_setup_data *req))
{
	(void)complete;
	(void)dev;

	if((req->bmRequestType != 0x81) ||
	   (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
	   (req->wValue != 0x2200))
		return USBD_REQ_NOTSUPP;

	
	/* Handle the HID report descriptor. */
	*buf = (uint8_t *)hid_report_descriptor;
	*len = sizeof(hid_report_descriptor);
	
	return USBD_REQ_HANDLED;
}

#ifdef INCLUDE_DFU_INTERFACE
static void dfu_detach_complete(usbd_device *dev, struct usb_setup_data *req)
{
	(void)req;
	(void)dev;

	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, 0, GPIO15);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO10);
	gpio_set(GPIOA, GPIO10);
	scb_reset_core();
}

static enum usbd_request_return_codes dfu_control_request(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *dev, struct usb_setup_data *req))
{
	(void)buf;
	(void)len;
	(void)dev;

	if ((req->bmRequestType != 0x21) || (req->bRequest != DFU_DETACH))
		return USBD_REQ_NOTSUPP; /* Only accept class request. */

	*complete = dfu_detach_complete;

	return USBD_REQ_HANDLED;
}
#endif

static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
	(void)wValue;

	usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 14, NULL);

	usbd_register_control_callback(
				dev,
				USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				hid_control_request);
#ifdef INCLUDE_DFU_INTERFACE
	usbd_register_control_callback(
				dev,
				USB_REQ_TYPE_CLASS | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				dfu_control_request);
#endif
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8); 
    /* SysTick interrupt every N clock pulses: set reload to N-1 */ 
    systick_set_reload(99999); 
     
    systick_counter_enable();
    systick_interrupt_enable();

}

void init_io(void){
    rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);
	//rcc_periph_clock_enable(RCC_GPIOB);

	gpio_set_mode(GPIOC, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_PULL_UPDOWN, GPIO14|GPIO15);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
			GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0|GPIO1|GPIO2|GPIO3
			                           |GPIO4|GPIO5|GPIO6|GPIO7);
	gpio_set(GPIOC,GPIO14|GPIO15);
	gpio_set(GPIOA,GPIO0|GPIO1|GPIO2|GPIO3
		    |GPIO4|GPIO5|GPIO6|GPIO7);
}

uint8_t buf[14];

void read_buttons(void){
	//clear
	int i = 0;
	for (i = 0; i < 14; ++i){
		buf[i] = 0x00;
	}
	i = 0;
	if(!gpio_get(GPIOC,GPIO14)){
		buf[2+i] = 0x3a; //F1
		++i;
	}
	if(!gpio_get(GPIOC,GPIO15)){
		buf[2+i] = 0x3b; //F2
		++i;
	}
	if(!gpio_get(GPIOA,GPIO0)){
		buf[2+i] = 0x3c; //F3
		++i;
	}
	if(!gpio_get(GPIOA,GPIO1)){
		buf[2+i] = 0x3d; //F4
		++i;
	}
	if(!gpio_get(GPIOA,GPIO2)){
		buf[2+i] = 0x3e; //F5
		++i;
	}
	if(!gpio_get(GPIOA,GPIO3)){
		buf[2+i] = 0x3f; //F6
		++i;
	}
	if(!gpio_get(GPIOA,GPIO4)){
		buf[2+(i%6)] = 0x40; //F7
		++i;
	}
	if(!gpio_get(GPIOA,GPIO5)){
		buf[2+(i%6)] = 0x41; //F8
		++i;
	}
	if(!gpio_get(GPIOA,GPIO6)){
		buf[2+(i%6)] = 0x42; //F9
		++i;
	}
	if(!gpio_get(GPIOA,GPIO7)){
		buf[2+(i%6)] = 0x43; //F10
		++i;
	}
}

int main(void)
{
	int i;

	rcc_clock_setup_in_hse_8mhz_out_72mhz();
    init_io();
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
	for (i = 0; i < 0x800000; i++) {
		__asm__("nop");
	}

	gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
	gpio_set(GPIOC, GPIO13);

	usbd_dev = usbd_init(&st_usbfs_v1_usb_driver, &dev_descr, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usbd_dev, hid_set_config);

	/* Delay some seconds to show that pull-up switch works. */
	for (i = 0; i < 0x800000; i++)
		__asm__("nop");

    gpio_clear(GPIOC, GPIO13);

	while (1)
		usbd_poll(usbd_dev);
}



void sys_tick_handler(void)
{
	systick_interrupt_disable();
	read_buttons();
	gpio_set(GPIOC, GPIO13);

	usbd_ep_write_packet(usbd_dev, 0x81, buf, 14);
	gpio_clear(GPIOC, GPIO13);
	systick_interrupt_enable();

}
