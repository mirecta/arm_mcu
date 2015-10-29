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


/*help


keycodes http://www.mindrunway.ru/IgorPlHex/USBKeyScan.pdf
out endpoint  https://github.com/openstenoproject/stenosaurus/blob/master/bootloader/usb.c

  */



#include <stdio.h>
#include <stdlib.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>

#include "lcdi2c.h"

extern void  initialise_monitor_handles(void);



static usbd_device *usbd_dev;

LcdI2c lcd(0x20,4,5,6,7);

#define MAX_PACKET_SIZE 64


struct keyboard_report_t
{
    uint8_t modifier;
    uint8_t reserved;
    uint8_t keycode[6];
}__attribute__ ((packed));



struct keyboard_report_t kr;

const struct usb_device_descriptor dev_descr = {
	.bLength = USB_DT_DEVICE_SIZE,
	.bDescriptorType = USB_DT_DEVICE,
	.bcdUSB = 0x0200,
	.bDeviceClass = 0,
	.bDeviceSubClass = 0,
	.bDeviceProtocol = 0,
	.bMaxPacketSize0 = MAX_PACKET_SIZE,
	.idVendor = 0x0483,
	.idProduct = 0x5710,
	.bcdDevice = 0x0200,
	.iManufacturer = 1,
	.iProduct = 2,
	.iSerialNumber = 3,
	.bNumConfigurations = 1,
};



static const uint8_t hid_report_descriptor[] = {
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0x95, MAX_PACKET_SIZE,         //   REPORT_COUNT (MAX_PACKET_SIZE)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x06, 0x00, 0xFF,              //   USAGE_PAGE (Vendor Defined Page 1)
    0x19, 0x01,                    //   USAGE_MINIMUM (1)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0xff,                    //   LOGICAL_MAXIMUM (255)
    0x29, 0x40,                    //   USAGE_MAXIMUM (64)
    0x91, 0x00,                    //   OUTPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION
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

const struct usb_endpoint_descriptor hid_endpoints[] = {
    {USB_DT_ENDPOINT_SIZE,USB_DT_ENDPOINT,0x81,USB_ENDPOINT_ATTR_INTERRUPT,sizeof(struct keyboard_report_t),0x20,0,0 },
    {USB_DT_ENDPOINT_SIZE,USB_DT_ENDPOINT,0x01,USB_ENDPOINT_ATTR_INTERRUPT,MAX_PACKET_SIZE,0x20,0,0 },};
/*
    hid_endpoints[0].bLength = USB_DT_ENDPOINT_SIZE;
    hid_endpoints[0].bDescriptorType = USB_DT_ENDPOINT;
    hid_endpoints[0].bEndpointAddress = 0x81;
    hid_endpoints[0].bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT;
    hid_endpoints[0].wMaxPacketSize = sizeof(struct keyboard_report_t);
    hid_endpoints[0].bInterval = 0x20;
    
    hid_endpoints[1].bLength = USB_DT_ENDPOINT_SIZE;
    hid_endpoints[1].bDescriptorType = USB_DT_ENDPOINT;
    hid_endpoints[1].bEndpointAddress = 0x01;
    hid_endpoints[1].bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT;
    hid_endpoints[1].wMaxPacketSize = 64;
    hid_endpoints[1].bInterval = 0x20;

*/
    

const struct usb_interface_descriptor hid_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints =2,
	.bInterfaceClass = USB_CLASS_HID,
	.bInterfaceSubClass = 0, /* boot */
	.bInterfaceProtocol = 1, /* keyboard */
	.iInterface = 0,

	.endpoint = hid_endpoints,

	.extra = &hid_function,
	.extralen = sizeof(hid_function),
};





const struct usb_config_descriptor::usb_interface  ifaces[] = {
    {
        0,1,0,&hid_iface
	//.num_altsetting = 1,
	//.altsetting = &hid_iface,
    }
};

const struct usb_config_descriptor config = {
	.bLength = USB_DT_CONFIGURATION_SIZE,
	.bDescriptorType = USB_DT_CONFIGURATION,
	.wTotalLength = 0,
	.bNumInterfaces = 1,
	.bConfigurationValue = 1,
	.iConfiguration = 0,
	.bmAttributes = 0xC0,
	.bMaxPower = 0x32,

	.interface =  ifaces,
};

static const char *usb_strings[] = {
	"mIRECta tech.",
	"HID Keyboard",
	"Keyboard",
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

static int hid_control_request(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			int (**complete)(usbd_device *, struct usb_setup_data *))
{
	(void)complete;
	(void)dev;

	if((req->bmRequestType != 0x81) ||
	   (req->bRequest != USB_REQ_GET_DESCRIPTOR) ||
	   (req->wValue != 0x2200))
		return 0;

	/* Handle the HID report descriptor. */
	*buf = (uint8_t *)hid_report_descriptor;
	*len = sizeof(hid_report_descriptor);

	return 1;
}


static void endpoint_callback(usbd_device *_usbd_dev, uint8_t ep) {
    uint8_t hid_buffer[MAX_PACKET_SIZE + 1];

    uint16_t bytes_read = usbd_ep_read_packet(_usbd_dev,ep,hid_buffer,sizeof(hid_buffer));
    //process packet
    /*commands: 0x00 - NOP
                0x01 - clear
                0x02 - off backlight
                0x03 - on backlight
                0x04 0xXX 0xYY - gotoxy to position X and Y
                0x05 0xSS ...... - print string with S size 
    */
    uint8_t i = 0;
    while (i < MAX_PACKET_SIZE){
        switch (hid_buffer[i]){
         
            case 0x00: // NOP
                ++i;
                break;

            case 0x01: // clear display
                lcd.clear();
                ++i;
                break;

            case 0x02: // backlight off
                lcd.setBacklight(0);
                ++i;
                break;

            case 0x03: // backlight on 
                lcd.setBacklight(1);
                ++i;
                break;

           case 0x04: // gotoxy
                if ( i + 2 < MAX_PACKET_SIZE){
                 lcd.gotoxy(hid_buffer[i+1],hid_buffer[i+2]);
                }
                i+=3;
                break;

           case 0x05: //print
                if (i + 1 < MAX_PACKET_SIZE){
                    if (i + 1 + hid_buffer[i+1] <  MAX_PACKET_SIZE){
                        lcd.print( (char*) &hid_buffer[i+2], hid_buffer[i+1]);
                        i+= hid_buffer[i+1] + 2;
                    
                    }else{
                        i= MAX_PACKET_SIZE;
                    }
                }else{
                 i = MAX_PACKET_SIZE;
                }
               break;
            default:
               i = MAX_PACKET_SIZE;
               break;

        }

    }
    
  

}

static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
	(void)wValue;
	(void)dev;

	usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, sizeof(keyboard_report_t), NULL);
     // Set up endpoint 1 for data coming OUT from the host.
    usbd_ep_setup(dev, 0x01, USB_ENDPOINT_ATTR_INTERRUPT, MAX_PACKET_SIZE, endpoint_callback);


	usbd_register_control_callback(
				dev,
				USB_REQ_TYPE_STANDARD | USB_REQ_TYPE_INTERFACE,
				USB_REQ_TYPE_TYPE | USB_REQ_TYPE_RECIPIENT,
				hid_control_request);

	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	/* SysTick interrupt every N clock pulses: set reload to N-1 */
	systick_set_reload(99999);
	systick_interrupt_enable();
	systick_counter_enable();
}


void i2c_setup(){
   
    /* Enable clocks for I2C2 and AFIO. */

    /* Set alternate functions for the SCL and SDA pins of I2C2. */
    gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
            GPIO_I2C1_SCL | GPIO_I2C1_SDA);

    /* Disable the I2C before changing any configuration. */
    i2c_peripheral_disable(I2C1);

    /* APB1 is running at 36MHz. */
    i2c_set_clock_frequency(I2C1, I2C_CR2_FREQ_36MHZ);

    /* 400KHz - I2C Fast Mode */
    i2c_set_fast_mode(I2C1);
  
    /*
     * fclock for I2C is 36MHz APB2 -> cycle time 28ns, low time at 400kHz
     * incl trise -> Thigh = 1600ns; CCR = tlow/tcycle = 0x1C,9;
     * Datasheet suggests 0x1e.
     */
    i2c_set_ccr(I2C1, 0x1e);

    /*
     * fclock for I2C is 36MHz -> cycle time 28ns, rise time for
     * 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
     * Incremented by 1 -> 11.
     */
  //    i2c_set_trise(I2C1, 0x0b);

//      i2c_set_own_7bit_slave_address(I2C1, 0x32);  
      i2c_set_dutycycle(I2C1, I2C_CCR_DUTY_DIV2);
      i2c_disable_ack(I2C1);

    //addressing mode
    i2c_peripheral_enable(I2C1);
}


void clock_setup(){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_I2C1);
    rcc_periph_clock_enable(RCC_AFIO);
    rcc_periph_clock_enable(RCC_GPIOE);
    rcc_periph_clock_enable(RCC_GPIOC);
}

void gpio_setup(){

    gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT_50_MHZ,
            GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO1);
    gpio_set(GPIOC,GPIO1);

}

void read_buttons(){
//keys 
uint16_t portc = ~gpio_port_read(GPIOC);

   kr.modifier = 0x00;
   kr.reserved = 0x00;
   kr.keycode[0] = 0x00;
   kr.keycode[1] = 0x00;
   kr.keycode[2] = 0x00;
   kr.keycode[3] = 0x00;
   kr.keycode[4] = 0x00;
   kr.keycode[5] = 0x00;

   
if (portc & 0x0002){

    kr.keycode[0] = 0x04;

}
else{
    kr.keycode[0] = 0;
}



}


    

int main(void)
{
    clock_setup();
    gpio_setup();
    i2c_setup();

    lcd.init();

#if defined(ENABLE_SEMIHOSTING) && (ENABLE_SEMIHOSTING) 
    initialise_monitor_handles();
#endif
    

	usbd_dev = usbd_init(&stm32f103_usb_driver, &dev_descr, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usbd_dev, hid_set_config);


	while (1)
		usbd_poll(usbd_dev);
}

void sys_tick_handler(void)
{
    //gpio_toggle(GPIOE,GPIO5);
    //read buttons
    read_buttons();
    usbd_ep_write_packet(usbd_dev, 0x81, (const void *) (&kr), sizeof(struct keyboard_report_t));
}
    

