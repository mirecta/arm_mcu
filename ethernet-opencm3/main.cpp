

#include <libopencm3/stm32/spi.h>
#include <libopencm3/stm32/timer.h>

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#include "disp.h"



//ethernet magic
#include "net/enc28j60.h"
#include "net/net.h"
#include "net/ip_arp_udp_tcp.h"
#include "net/spi.h"







void delay( const uint32_t loop){
    for (uint32_t i = 0; i < loop; i++)    /* Wait a bit. */
        __asm__("nop");

}


void clock_setup(void){
    rcc_clock_setup_in_hse_8mhz_out_72mhz();
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOE);
   
    rcc_periph_clock_enable(RCC_GPIOA);
    
}

void gpio_setup(void){
    gpio_set_mode(GPIOC, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO0 | GPIO1);
    //i want pullup to A8, A9, A10, A07
    gpio_set(GPIOC,GPIO0 | GPIO1);

    gpio_set_mode(GPIOE, GPIO_MODE_OUTPUT_50_MHZ,GPIO_CNF_OUTPUT_PUSHPULL, GPIO5|GPIO6);



    

}

static unsigned char mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24};
static unsigned char myip[4] = {192,168,1,101};
#define BUFFER_SIZE 1500//400
static unsigned char buf[BUFFER_SIZE+1];
char resp[] = "OK";

int main(void){

    clock_setup();
    gpio_setup();
    gpio_set(GPIOE, GPIO5);
    dispInit();
    uint32_t portc;
    uint8_t enable=0;
    int a = 0;
    unsigned int plen = 0;
    unsigned char payloadlen=0;

    
    ENC28J60_SPI_Init();

    enc28j60Init(mymac);
    init_ip_arp_udp_tcp(mymac,myip,80);

    enc28j60PhyWrite(PHLCON,0x7a4);
    enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz


    while(1){
        plen = enc28j60PacketReceive(BUFFER_SIZE, buf);


        do{
            /*plen will ne unequal to zero if there is a valid packet (without crc error) */
            if(plen==0)
            {
                continue;
            }
            // arp is broadcast if unknown but a host may also
            // verify the mac address by sending it to 
            // a unicast address.
            if(eth_type_is_arp_and_my_ip(buf,plen))
            {
                make_arp_answer_from_request(buf);
                continue;
            }

            // check if ip packets are for us:
            if(eth_type_is_ip_and_my_ip(buf,plen)==0)
            {
                continue;
            }


            if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
            {
                // a ping packet, let's send pong	
                make_echo_reply_from_request(buf, plen);
                ++a;
                continue;
            }

            if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==4&&buf[UDP_DST_PORT_L_P]==0xb0)
            {
                payloadlen=buf[UDP_LEN_L_P]-UDP_HEADER_LEN;
                // you must sent a string starting with v
                // e.g udpcom version 10.0.0.24
                //buf[UDP_DATA_P];
                if(payloadlen == 4){
                    a = (*(int32_t*)(&buf[UDP_DATA_P]));
                    make_udp_reply_from_request(buf,resp,2,1200);
                }
                continue;
            }


        }while(0);

        dispPrintNumber(a);
        delay(200000);
    }
/*
    while (1) {
        portc = (~gpio_port_read(GPIOC));
        if(portc &0x02)
            enable = ~enable;

        if (a > 99999999)
            a = 0;

        dispPrintNumber(a);
        ++a;

        delay(200000);
        if(enable)
            gpio_toggle(GPIOE,GPIO5|GPIO6);
    }
*/
}
