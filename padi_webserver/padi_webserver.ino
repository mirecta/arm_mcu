/*
 This sketch shows how to open/close file and perform read/write to it.
 */
#include "SdFatFs.h"
#include "util.h"
#include <WiFi.h>

#include <PadiWebServer.h>

extern "C" {
void UserPreInit(void)
{
   Init_CPU_CLK_UART(1,38400); // 83.3 MHz
   // 0 - 166666666 Hz, 1 - 83333333 Hz, 2 - 41666666 Hz, 3 - 20833333 Hz, 4 - 10416666 Hz, 5 - 4000000 Hz
   // 6 - 200000000 Hz, 7 - 10000000 Hz, 8 - 50000000 Hz, 9 - 25000000 Hz, 10 - 12500000 Hz, 11 - 4000000 Hz
}  
} // extern "C"

SdFatFs fs;
Config_t cfg;
int status = WL_IDLE_STATUS;     // the Wifi radio's status

PadiWebServer server(80);

void handleCmd(){
rtl_printf("i am in \n");
String str = server.arg("plain");
for(int i = 0; i < str.size(); ++i){
  rtl_printf(" %d ",str.data()[i]);
 }
 rtl_printf("\n");
server.send(200);
}

void handleNotFound(){

  if(server.serveStatic(fs,"/www/") != -1 )
        return;
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  
}

void setup() {
  sys_info();
  fs.begin();
 
   WiFi.status(); //this magic init wifi
   
 readConfig(fs,cfg);
 if (cfg.ap){
   //if(1){
   // attempt to start AP:
  while (status != WL_CONNECTED) {
    printf("Attempting to start AP with SSID:%s\n ", cfg.ssid);
    status = WiFi.apbegin(cfg.ssid, cfg.passwd, cfg.channel);
    delay(10000);
  }

 }else{
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    printf("Attempting to connect to WPA SSID: %s\n",cfg.ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(cfg.ssid, cfg.passwd);
    // wait 10 seconds for connection:
    delay(20000);
  }  
 }
 
  IPAddress ip = WiFi.localIP();
  printf("IP is %s\n ", ip.get_address());


  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  server.on("/cmd",handleCmd);
  server.onNotFound(handleNotFound);

  server.begin();
  //fs.end();
}

void loop() {
  server.handleClient();
  //delay(100);

}
