/*
 This sketch shows how to open/close file and perform read/write to it.
 */

#include "SdFatFs.h"
#include "util.h"

extern "C" {
void UserPreInit(void)
{
   Init_CPU_CLK_UART(7,38400); // 83.3 MHz
   // 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
   // 6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000 Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000 Hz
}  
} // extern "C"

char filename[] = "test.txt";
char write_content[] = "hello world!";

SdFatFs fs;

void setup() {
  char buf[128];
  char *parts[2];
  int rd;

  fs.begin();

  
  SdFatFile file = fs.open("/conf/device.conf");

  memset(buf, 0, sizeof(buf));

  do{
    rd = file.readline(buf,sizeof(buf));
    //int c = getlen(buf);
    int c = split(buf,parts,'=');
    if (c != 2) continue;
    
    if(streq(strip(parts[0]),"mode")){
      if(streq(strip(parts[1]),"ap")){
        printf("vyrobim ap \n");
      }
    }
    if(streq(strip(parts[0]),"ssid")){
      printf ("ESSID bude:'%s'\n",strip(parts[1]));
    }
    if(streq(strip(parts[0]),"passwd")){
      printf ("heslo bude:'%s'\n",strip(parts[1]));
    }
    /*if (c == 2){
      printf(">>>%s<<< >>>%s<<<<\n", strip(parts[0]),strip(parts[1]));
    }*/
  }while (rd != -1);
  
  //file.read(buf, sizeof(buf));

  file.close();


  fs.end();
}

void loop() {

}
