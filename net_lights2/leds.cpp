


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include <stdlib.h>
#include <stdio.h>



#include "ws2811.h"




#define COLOR_COUNT 9
#define MODE_MAX 4
#define SPEED_MAX 8

uint8_t mode = 2;
uint8_t maxL = 30;
uint8_t speed = 0;
const uint16_t setSpeed[SPEED_MAX+1] = {10,15,30,50,100,200,500,1000,2000};
uint8_t updown[LED_COUNT];
uint8_t colorIndex = 8;
int recvfakecount = 0;
volatile static uint8_t recvcmd[3];
volatile static uint8_t recvindex = 0;


struct SetColor_t{
    uint8_t hue;
    uint8_t saturation;
};

SetColor_t setColor[COLOR_COUNT];



void resetColors(){
    
    setColor[0].hue = 0;
    setColor[0].saturation = 255;

    setColor[1].hue = 32;
    setColor[1].saturation = 255;

    setColor[2].hue = 64;
    setColor[2].saturation = 255;

    setColor[3].hue = 96;
    setColor[3].saturation = 255;
    
    setColor[4].hue = 128;
    setColor[4].saturation = 255;
    
    setColor[5].hue = 160;
    setColor[5].saturation = 255;
    
    setColor[6].hue = 192;
    setColor[6].saturation = 255;

    setColor[7].hue = 224;
    setColor[7].saturation = 255;

    setColor[8].hue = 0;
    setColor[8].saturation = 0;

}


extern "C"  void  initialise_monitor_handles(void);

void delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}

void handle_serial(void);

void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOE);
    //buttons
    rcc_periph_clock_enable(RCC_GPIOC);

	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_AFIO);
	rcc_periph_clock_enable(RCC_USART1);

}

void gpio_setup(void){


    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_50_MHZ,
                          GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);




    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO1);
    //i want pullup to A8, A9, A10, A07
    gpio_set(GPIOC,GPIO0 | GPIO1);



}

static void usart_setup(void){
	/* Enable the USART1 interrupt. */
	nvic_enable_irq(NVIC_USART1_IRQ);

	/* Setup GPIO pin GPIO_USART1_RE_TX on GPIO port B for transmit. */
	//gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
	//	      GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);

	/* Setup GPIO pin GPIO_USART1_RE_RX on GPIO port B for receive. */
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT,
		      GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);

	/* Setup UART parameters. */
	usart_set_baudrate(USART1, 115200);
	usart_set_databits(USART1, 8);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_set_mode(USART1, USART_MODE_RX);

	/* Enable USART1 Receive interrupt. */
	USART_CR1(USART1) |= USART_CR1_RXNEIE;

	/* Finally enable the USART. */
	usart_enable(USART1);
}


void usart1_isr(void){

    static uint8_t rx;

	if (((USART_CR1(USART1) & USART_CR1_RXNEIE) != 0) &&
			((USART_SR(USART1) & USART_SR_RXNE) != 0)) {
		gpio_toggle(GPIOC, GPIO13);

        if (recvindex < 3){
            rx = usart_recv(USART1);
            recvcmd[recvindex] = rx;
            recvindex++;
        }
	}
}


int main(void){

#if defined(ENABLE_SEMIHOSTING) && (ENABLE_SEMIHOSTING)
        initialise_monitor_handles();
#endif
    delay(300000);
    resetColors();
    clock_setup();
    gpio_setup();
    gpio_set(GPIOC, GPIO13);

    usart_setup();
    // init led strip in HSV mode
    ws2811Init(1);





      for(int i = 0; i < LED_COUNT; ++i){
                    line[i].a = rand()%255;
                    line[i].b = 0;
                    line[i].c = rand() % maxL;

                    updown[i] = rand()%2;
      }



    int changed = 0;

   ws2811Sync();
    while (1) {
//        delay(3000000);
        

   if(mode == 1 || mode == 2){
      for(int i = 0; i < LED_COUNT; ++i)
        {
            if(updown[i] == 0){
                if(line[i].c == 0){
                    line[i].a = setColor[colorIndex].hue;
                    line[i].b = setColor[colorIndex].saturation;
                    changed++;
                    ++line[i].c;
                    updown[i] = 1;
                }else{
                    --line[i].c;
                }
            }else{
                if(line[i].c >=  maxL){
                    --line[i].c;
                    updown[i] = 0;
                }else{
                    ++line[i].c;
                }

            }
        }
        if (changed >= 2*LED_COUNT){
            if(mode == 2){
               ++colorIndex;
               if (colorIndex >= COLOR_COUNT)
                   colorIndex = 0;
            }
            changed = 0;
        }
   }else if( mode == 3){
      for(int i = 0; i < LED_COUNT; ++i)
        {
            line[i].a = setColor[colorIndex].hue;
            line[i].b = setColor[colorIndex].saturation;
            line[i].c = maxL;
        }   
   }else if(mode == 4){
      for(int i = 0; i < LED_COUNT; ++i)
        {
            line[i].a = 0;
            line[i].b = 0;
            line[i].c = 0;
        }   
            while(!ws2811IsReady());
            ws2811Sync();
            mode = 0;        

   }


        delay(3000000/setSpeed[speed]);
        

        if(mode){
            while(!ws2811IsReady());
            ws2811Sync();
        }
        //handleEthernet();
        //handle serial buffer
        handle_serial();
    }


}

// 0x00 0x00 0x00 - reset
// 0x01 0xMM 0x00 - set mode MM (1 - 4)
// 0x02 0xSS 0x00 - set speed  (0 - 8)
// 0x03 0xHH 0xSS - set color hue HH sat SS
// 0x04 0xLL 0x00 - set max lightness


void handle_serial(void){
    if (recvindex == 3){

            switch(recvcmd[0]){

                case 0x00: // reset to defaults
                    resetColors();
                    colorIndex = 8;
                    mode = 2;
                    maxL = 30;
                    speed = 0;
                    //reset L
                    for(int i = 0; i < LED_COUNT; ++i)
                        line[i].c = rand() % maxL;
                    
                    break;

                case 0x01: // set mode
                    if (recvcmd[1] <= MODE_MAX)
                        mode = recvcmd[1];
                    if (mode == 2 || mode == 1){
                        for(int i = 0; i < LED_COUNT; ++i)
                            line[i].c = rand() % maxL;

                    }
                            
                    break;

                case 0x02: //set speed
                    if (recvcmd[1] <= SPEED_MAX)
                        speed = recvcmd[1];
                    break;

                case 0x03: //set color
                    ++colorIndex;

                    if (colorIndex >= COLOR_COUNT)
                        colorIndex = 0;

                    setColor[colorIndex].hue = recvcmd[1];
                    setColor[colorIndex].saturation = recvcmd[2];
                    break;

                case 0x04: //set maxL
                    maxL = recvcmd[1];
                   
                    if (mode == 2 || mode == 1){
                        for(int i = 0; i < LED_COUNT; ++i)
                            line[i].c = rand() % maxL;

                    }
                    
                    
                    break;

                default:
                    //prd
                    break;


            }
     //ready for new data
     recvindex = 0;
    }else if (recvindex != 0){
        recvfakecount++;
        if (recvfakecount == 100){
            recvindex = 0;
            recvfakecount = 0;
        }
    }
}
/*void handleEthernet(void){

    unsigned int plen = 0;
    unsigned char payloadlen=0;

    plen = enc28j60PacketReceive(BUFFER_SIZE, buf);


    if(plen==0)
    {
        return;
    }
    // arp is broadcast if unknown but a host may also
    // verify the mac address by sending it to 
    // a unicast address.
    if(eth_type_is_arp_and_my_ip(buf,plen))
    {
        make_arp_answer_from_request(buf);
        return;
    }

    // check if ip packets are for us:
    if(eth_type_is_ip_and_my_ip(buf,plen)==0)
    {
        return;
    }


    if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
    {
        // a ping packet, let's send pong	
        make_echo_reply_from_request(buf, plen);
        return;
    }
    //UDP port 1200
    if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==4&&buf[UDP_DST_PORT_L_P]==0xb0)
    {
        payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
        // you must sent a string starting with v
        // e.g udpcom version 10.0.0.24
        //buf[UDP_DATA_P];
        if(payloadlen == 3){

            switch(buf[UDP_DATA_P]){

                case 0x00: // reset to defaults
                    resetColors();
                    colorIndex = 8;
                    mode = 2;
                    maxL = 30;
                    speed = 0;
                    //reset L
                    for(int i = 0; i < LED_COUNT; ++i)
                        line[i].c = rand() % maxL;
                    
                    break;

                case 0x01: // set mode
                    if (buf[UDP_DATA_P + 1] <= MODE_MAX)
                        mode = buf[UDP_DATA_P + 1];
                    if (mode == 2 || mode == 1){
                        for(int i = 0; i < LED_COUNT; ++i)
                            line[i].c = rand() % maxL;

                    }
                            
                    break;

                case 0x02: //set speed
                    if (buf[UDP_DATA_P + 1] <= SPEED_MAX)
                        speed = buf[UDP_DATA_P + 1];
                    break;

                case 0x03: //set color
                    ++colorIndex;

                    if (colorIndex >= COLOR_COUNT)
                        colorIndex = 0;

                    setColor[colorIndex].hue = buf[UDP_DATA_P + 1];
                    setColor[colorIndex].saturation = buf[UDP_DATA_P + 2];
                    break;

                case 0x04: //set maxL
                    maxL = buf[UDP_DATA_P + 1];
                   
                    if (mode == 2 || mode == 1){
                        for(int i = 0; i < LED_COUNT; ++i)
                            line[i].c = rand() % maxL;

                    }
                    
                    
                    break;

                default:
                    //prd
                    break;


            }

        }

        //a = (*(int32_t*)(&buf[UDP_DATA_P]));
        //make_udp_reply_from_request(buf,resp,2,1200);

        return;
    }
}
*/





