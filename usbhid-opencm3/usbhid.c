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
#include <stdio.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/usb/usbd.h>
#include <libopencm3/usb/hid.h>


// analog A0,A1,A2,A3
// js buttons A8,A9
// left B4,B5,B6,B7
// right B12,B13,B14,B15
// start, select A7,A10

/* Define this to include the DFU APP interface. */
//#define INCLUDE_DFU_INTERFACE

#ifdef INCLUDE_DFU_INTERFACE
#include <libopencm3/cm3/scb.h>
#include <libopencm3/usb/dfu.h>
#endif

static usbd_device *usbd_dev;

extern void  initialise_monitor_handles(void);

volatile uint16_t result_array_raw[4]; //adc values

struct gamepad_report_t{
    uint16_t buttons;
    int8_t left_x;
    int8_t left_y;
    int8_t right_x;
    int8_t right_y;
} __attribute__ ((packed));


struct gamepad_report_t gr;

void wait(){
    int i;
    for (i = 0; i < 800000; i++)    /* Wait a bit. */
        __asm__("nop");
    
}

void nvic_setup(){
//    nvic_enable_irq(NVIC_HARD_FAULT_IRQ);
    nvic_enable_irq(NVIC_DMA1_CHANNEL1_IRQ);

}

void adc_setup(){
    uint8_t channel_array[16];

    /* make shure it didnt run during config */
    adc_off(ADC1);
    adc_enable_scan_mode(ADC1); /*scan mode means we scan all channels of the group to the end */
    adc_set_continuous_conversion_mode(ADC1); /* means we scan the group the whole day til someone disable continous mode */
    adc_disable_discontinuous_mode_regular(ADC1); /* discontinous mode means all channels in a group will be */
    adc_enable_external_trigger_regular(ADC1, ADC_CR2_EXTSEL_SWSTART);
    adc_set_right_aligned(ADC1);
    adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_7DOT5CYC);
   
    /* enable adc */
    adc_power_on(ADC1);

    /* wait for adc starting up*/
    wait();
    adc_reset_calibration(ADC1);
    adc_calibration(ADC1);

    /* Select the channel(s) (up  to 16) we want to convert. */
    channel_array[0] = 0; // 1
    channel_array[1] = 1;
    channel_array[2] = 2;
    channel_array[3] = 3; // 4

    adc_set_regular_sequence(ADC1, 4, channel_array);

    /* we want to use it with dma */
    adc_enable_dma(ADC1);
     wait();
    //start adc
    adc_start_conversion_regular(ADC1);

}

void dma_setup(){

    //ADC
    /* no reconfig for every ADC group conversion */
    dma_enable_circular_mode(DMA1, DMA_CHANNEL1);
    /* the memory pointer has to be increased, and the peripheral not */
    dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL1);
    /* ADC_DR is only 16bit wide in this mode */
    dma_set_peripheral_size(DMA1, DMA_CHANNEL1, DMA_CCR_PSIZE_16BIT);
    /*destination memory is also 16 bit wide */
    dma_set_memory_size(DMA1, DMA_CHANNEL1, DMA_CCR_MSIZE_16BIT);
    /* direction is from ADC to memory */
    dma_set_read_from_peripheral(DMA1, DMA_CHANNEL1);
    /* get the data from the ADC data register */
    dma_set_peripheral_address(DMA1, DMA_CHANNEL1,(uint32_t) &ADC_DR(ADC1));
    /* put everything in this array */
    dma_set_memory_address(DMA1, DMA_CHANNEL1, (uint32_t) &result_array_raw);
    /* we convert only 3 values in one adc-group */
    dma_set_number_of_data(DMA1, DMA_CHANNEL1, 4);
    /* we want an interrupt after the 3 halfwords. so we can compute something afterwards. */
    dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL1);

    /* dma ready to go. waiting til the peripheral gives the first data */
    dma_enable_channel(DMA1, DMA_CHANNEL1);
    //END ADC
}


void read_buttons(){
    uint16_t portb = ~gpio_port_read(GPIOB);
    uint16_t porta = ~gpio_port_read(GPIOA);
    gr.buttons = 0x00;

    gr.buttons |=((porta >> 7) & 0x0f) | ((portb & 0xf0)) | ((portb & 0xf000) >> 4) ;
}

void dma1_channel1_isr(){
   
    systick_interrupt_disable();

    gr.left_x = (result_array_raw[0] >> 4) - 127;
    gr.left_y = (result_array_raw[1] >> 4) - 127;
    gr.right_x = 127 - (result_array_raw[2] >> 4);
    gr.right_y = 127 - (result_array_raw[3] >> 4); 
    /* clear the interrupt flag */
    systick_interrupt_enable();
    DMA_IFCR(DMA1) |= DMA_IFCR_CGIF1;
}


void clock_setup(){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
 //   rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_GPIOB);
    rcc_periph_clock_enable(RCC_ADC1);
    rcc_periph_clock_enable(RCC_DMA1);
//  rcc_periph_clock_enable(RCC_AFIO);
}

void gpio_setup(){
 //   gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    //ADC
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0 | GPIO1 | GPIO2 | GPIO3 );
    gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO8 | GPIO9 | GPIO10 | GPIO7);
    //i want pullup to A8, A9, A10, A07
    gpio_set(GPIOA,GPIO8 | GPIO9 | GPIO10 | GPIO7);
    
    //external pullup version
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO4 | GPIO5 | GPIO6 | GPIO7 | GPIO12 | GPIO13 | GPIO14 | GPIO15);
    
    //internal pullup version
    //gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO4 | GPIO5 | GPIO6 | GPIO7 | GPIO12 | GPIO13 | GPIO14 | GPIO15);
    //gpio_set(GPIOB,GPIO4 | GPIO5 | GPIO6 | GPIO7 | GPIO12 | GPIO13 | GPIO14 | GPIO15);
    
}


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
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x05,                    // USAGE (Game Pad)
    0xa1, 0x01,                    // COLLECTION (Application)
    0xa1, 0x00,                    //   COLLECTION (Physical)
    0x05, 0x09,                    //     USAGE_PAGE (Button)
    0x19, 0x01,                    //     USAGE_MINIMUM (Button 1)
    0x29, 0x10,                    //     USAGE_MAXIMUM (Button 16)
    0x15, 0x00,                    //     LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //     LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //     REPORT_SIZE (1)
    0x95, 0x10,                    //     REPORT_COUNT (16)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0x05, 0x01,                    //     USAGE_PAGE (Generic Desktop)
    0x09, 0x30,                    //     USAGE (X)
    0x09, 0x31,                    //     USAGE (Y)
    0x09, 0x32,                    //     USAGE (Z)
    0x09, 0x33,                    //     USAGE (Rx)
    0x15, 0x81,                    //     LOGICAL_MINIMUM (-127)
    0x25, 0x7f,                    //     LOGICAL_MAXIMUM (127)
    0x75, 0x08,                    //     REPORT_SIZE (8)
    0x95, 0x04,                    //     REPORT_COUNT (4)
    0x81, 0x02,                    //     INPUT (Data,Var,Abs)
    0xc0,                          //   END_COLLECTION
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

const struct usb_endpoint_descriptor hid_endpoint = {
	.bLength = USB_DT_ENDPOINT_SIZE,
	.bDescriptorType = USB_DT_ENDPOINT,
	.bEndpointAddress = 0x81,
	.bmAttributes = USB_ENDPOINT_ATTR_INTERRUPT,
	.wMaxPacketSize = sizeof(struct gamepad_report_t),
	.bInterval = 0x20,
};

const struct usb_interface_descriptor hid_iface = {
	.bLength = USB_DT_INTERFACE_SIZE,
	.bDescriptorType = USB_DT_INTERFACE,
	.bInterfaceNumber = 0,
	.bAlternateSetting = 0,
	.bNumEndpoints = 1,
	.bInterfaceClass = USB_CLASS_HID,
	.bInterfaceSubClass = 1, /* boot */
	.bInterfaceProtocol = 2, /* mouse */
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
	"mIRECta tech.",
	"HID Gamepad",
	"Gamepad",
};

/* Buffer to be used for control requests. */
uint8_t usbd_control_buffer[128];

static int hid_control_request(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *, struct usb_setup_data *))
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

#ifdef INCLUDE_DFU_INTERFACE
static void dfu_detach_complete(usbd_device *dev, struct usb_setup_data *req)
{
	(void)req;
	(void)dev;

/*	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, 0, GPIO15);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,
		      GPIO_CNF_OUTPUT_PUSHPULL, GPIO10);
	gpio_set(GPIOA, GPIO10);*/
	scb_reset_core();
}

static int dfu_control_request(usbd_device *dev, struct usb_setup_data *req, uint8_t **buf, uint16_t *len,
			void (**complete)(usbd_device *, struct usb_setup_data *))
{
	(void)buf;
	(void)len;
	(void)dev;

	if ((req->bmRequestType != 0x21) || (req->bRequest != DFU_DETACH))
		return 0; /* Only accept class request. */

	*complete = dfu_detach_complete;

	return 1;
}
#endif

static void hid_set_config(usbd_device *dev, uint16_t wValue)
{
	(void)wValue;
	(void)dev;

	usbd_ep_setup(dev, 0x81, USB_ENDPOINT_ATTR_INTERRUPT, 4, NULL);

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
	systick_interrupt_enable();
	systick_counter_enable();
}

int main(void)
{
    clock_setup();
    gpio_setup();

    dma_setup(); // adc done interrupt
    adc_setup(); // 4 channels

    nvic_setup();
    


	usbd_dev = usbd_init(&stm32f103_usb_driver, &dev_descr, &config, usb_strings, 3, usbd_control_buffer, sizeof(usbd_control_buffer));
	usbd_register_set_config_callback(usbd_dev, hid_set_config);


	while (1)
		usbd_poll(usbd_dev);
}

void sys_tick_handler(void)
{

    //read buttons
    read_buttons();
    //other work made by DMA
	usbd_ep_write_packet(usbd_dev, 0x81, (const void *) (&gr), sizeof(struct gamepad_report_t));
}
