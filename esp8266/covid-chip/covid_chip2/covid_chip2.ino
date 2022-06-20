/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/* Create a WiFi access point and provide a web server on it. */

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <DNSServer.h> 

#ifndef APSSID
#define APSSID "Covid Chip 5G Control"
#define APPSK  ""
#endif

/* Set these to your desired credentials. */
const char *ssid = APSSID;
const char *password = APPSK;
//const byte DNS_PORT = 53;

//DNSServer dnsServer;
ESP8266WebServer server(80);

void setup_ip_and_server();

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
   connected to this access point to see it.
*/
void handleRoot() {
  server.send(200, "text/html", 
    "<h1>This device is property of Government</h1>"
    "<h3>If you are unauthorised person, please disconnect immediately!</h3>"
    "<h3>This security incident will be reported</h3>"
    "<p>Data:</p>"
    "<p>Vaccine: Vaxzevria Covid-19</p>"
    "<p>Date of vaccination: 2021 Jun 19</p>"
    "<p>Name of the person: R********** G*************</p>"
    "<p>Person number in Government database: RG-92301-AZ-5G-991</p>"
    "<p>Chemtrail index: 4</p>"
    "<p>Enter password to login into the chip:" 
    "<input type=password></input></p>");
  setup_ip_and_server();
}


void dhcps_lease_test(void){
   struct dhcps_lease dhcp_lease;
   IP4_ADDR(&dhcp_lease.start_ip, 192, 168, 4, 100);
   IP4_ADDR(&dhcp_lease.end_ip, 192, 168, 4, 120);
   wifi_softap_set_dhcps_lease(&dhcp_lease);
}


void setup_ip_and_server() {
   struct ip_info info;
   wifi_set_opmode( SOFTAP_MODE ); //Set softAP
   wifi_softap_dhcps_stop();
   IP4_ADDR(&info.ip, 192, 168, 4, 1);
   IP4_ADDR(&info.gw, 192, 168, 4, 1);
   IP4_ADDR(&info.netmask, 255, 255, 255, 0);
   wifi_set_ip_info(SOFTAP_IF, &info);
   dhcps_lease_test();
   wifi_softap_dhcps_start();

  IPAddress APIP(192, 168, 4, 1); 
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));

  //dnsServer.start(DNS_PORT, "*", APIP);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  
}


void setup() {  
  //delay(1000);
  Serial.begin(115200);
  Serial.println();
  
  
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  setup_ip_and_server();

}

void loop() {
  //dnsServer.processNextRequest();
  server.handleClient();
  
}
