/*
 * Copyright (c) 2015, Majenko Technologies
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * * Neither the name of Majenko Technologies nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h> 
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <EEPROM.h>
#include "index.h"

int factorypin = 14;

const byte DNS_PORT = 53;

DNSServer dnsServer;

/* Set these to your desired credentials. */
const char ssid[] = "Stromcek";
const char def_password[] = "treetree";

ESP8266WebServer server(80);

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  server.sendHeader("Content-Encoding", "deflate");
	server.send_P(200, "text/html",index_page, sizeof(index_page) );
}

void writeEEPROM(const char *passwd){
   int addr = 0;
   while(passwd[addr] != '\0'){
    EEPROM.write(addr, passwd[addr]);
    ++addr;
   }
   EEPROM.write(addr, '\0');
   EEPROM.commit();    
}

void readEEPROM(char *passwd){
  
  int addr = 0;
  char val;
   do{
    val = EEPROM.read(addr);
    passwd[addr] = val;
    ++addr;
   }while(val != '\0');
      
}

void handleSetup(){
 String pwd = server.arg(0);
 //Serial.print("/setup ");
 //Serial.println(pwd);
 
 if(pwd.length() >= 8 && pwd.length() <= 20){
     writeEEPROM(pwd.c_str());
     ESP.restart();
 }
 server.send(200,"text/plain","");
}

void handleCmd(){
        // 0x00 0x00 0x00 - reset
        // 0x01 0xMM 0x00 - set mode MM (1 - 4)
        // 0x02 0xSS 0x00 - set speed  (0 - 8)
        // 0x03 0xHH 0xSS - set color hue HH sat SS
        // 0x04 0xLL 0x00 - set max lightness
char data[3] = {0x00,0x00,0x00};

String cmd = server.argName(0);
if (cmd == "mode"){
  data[0] = 0x01;
  data[1] =  server.arg(0).toInt();
}else if (cmd == "speed"){
  data[0] = 0x02;
  data[1] =  server.arg(0).toInt();
}else if (cmd == "light"){
  data[0] = 0x04;
  data[1] =  server.arg(0).toInt(); 
}else if (cmd == "hue"){
  data[0] = 0x03;
  data[1] =  server.arg(0).toInt();
  data[2] =  server.arg(1).toInt();
}
//Serial.print("/cmd ");
//Serial.println(server.argName(0));
Serial.write(data,3);
Serial.flush();
server.send(200,"text/plain","");
}

void handleNotFound(){
  server.send(404, "text/plain", "Url not found!!");
}

void setup() {

  char password[21];
  
  EEPROM.begin(21);
  pinMode(factorypin, INPUT);

  if (digitalRead(factorypin) == 0){
    writeEEPROM(def_password);
  }

  readEEPROM(password);
  
	delay(1000);
	Serial.begin(115200);
	//Serial.println();
	//Serial.print("Configuring access point...");
	/* You can remove the password parameter if you want the AP to be open. */
  
	WiFi.softAP(ssid, password);

	IPAddress myIP = WiFi.softAPIP();
	//setup dns
  // modify TTL associated  with the domain name (in seconds)
  // default is 60 seconds
  dnsServer.setTTL(1200);
  // set which return code will be used for all other domains (e.g. sending
  // ServerFailure instead of NonExistentDomain will reduce number of queries
  // sent by clients)
  // default is DNSReplyCode::NonExistentDomain
  dnsServer.setErrorReplyCode(DNSReplyCode::ServerFailure);
  // start DNS server for a specific domain name
  dnsServer.start(DNS_PORT, "stromcek.sk", myIP);


	
	//Serial.print("AP IP address: ");
	//Serial.println(myIP);
	server.on("/", handleRoot);
	server.on("/cmd",handleCmd);
  server.on("/setup",handleSetup);
  server.onNotFound(handleNotFound);
	server.begin();
	//Serial.println("HTTP server started");
}

void loop() {
  dnsServer.processNextRequest();
	server.handleClient();
}
