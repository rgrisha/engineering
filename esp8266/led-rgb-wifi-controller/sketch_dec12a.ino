
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
//#include <ArduinoJson.h>


#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = "kodiszki";
const char* password = "19Plintusas159753!!##";

IPAddress ip(192, 168, 1, 250); //Node static IP
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);


ESP8266WebServer server(80);

const int led = 2;

void handleRoot() {=
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}




uint8_t LED_R = D5; 
uint8_t LED_G = D7; 
uint8_t LED_B = D6; 

int ledRVal = 0;
int ledGVal = 0;
int ledBVal = 0;

//StaticJsonDocument<500> doc;

int ledStatusCounter = 0;

/*
int isArgName(const char* name, uint8_t argNum) {
  return !strcmp(name, server.argName(argNum), sizeof(name));
}
*/

int isArgName(const char* name, uint8_t argNum) {
  return server.argName(argNum) == name;
}

int asInt(const String& sval) {
  if(sval) {
    return sval.toInt(); 
  }
  return -1;
}

void handleSetLeds() {

  if (server.method() != HTTP_POST) {
    server.send(400, "text/plain", "Only POST accepted\n");
  }
  
  //deserializeJson(doc, server.arg("plain"));
  
  /*
  ledRVal = atoi(doc["led-r"]);
  ledGVal = atoi(doc["led-g"]);
  ledBVal = atoi(doc["led-b"]);
  */


  for (uint8_t i = 0; i < server.args(); i++) {
    /*
    if(isArgName("LED-R", i)) {
      ledRVal = atoi(server.arg(i));
    }
    else if(isArgName("LED-G", i)) {
      ledGVal = atoi(server.arg(i));
    }
    if(isArgName("LED-B", i)) {
      ledBVal = atoi(server.arg(i));
    }
    */
    if(isArgName("LED-R", i)) {
      ledRVal = asInt(server.arg(i));
    }
    else if(isArgName("LED-G", i)) {
      ledGVal = asInt(server.arg(i));
    }
    if(isArgName("LED-B", i)) {
      ledBVal = asInt(server.arg(i));
    }
    
  }

  
  Serial.print("r=");
  Serial.print(ledRVal);
  Serial.print(" g=");
  Serial.print(ledGVal);
  Serial.print(" b=");
  Serial.println(ledBVal);

  server.send(201, "text/plain", "Values accepted\n");
}

void connectIfNot() {

  if(WiFi.status() == WL_CONNECTED) {
    return;
  }
  
  //WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  if (!WiFi.config(ip, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
    ledStatusCounter++;
    digitalWrite(led, ledStatusCounter & 1);
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);

}

void setup() {

  

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B ,OUTPUT);

  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  
  Serial.begin(115200);

  connectIfNot();

  server.on("/", handleRoot);

  server.onNotFound(handleNotFound);

  server.on("/set-leds", handleSetLeds);

  server.begin();
  Serial.println("HTTP server started");
}


void loop() {

  connectIfNot();

  server.handleClient();
  
 /*
 digitalWrite(LED_R, HIGH); 
 delay(1000); 
 digitalWrite(LED_R, LOW); 
 digitalWrite(LED_G, HIGH); 
 delay(1000); 
 digitalWrite(LED_G, LOW); 
 digitalWrite(LED_B, HIGH);
 delay(1000);   
 digitalWrite(LED_B, LOW); 
 */

 analogWrite(LED_R, ledRVal);
 analogWrite(LED_G, ledGVal);
 analogWrite(LED_B, ledBVal);

 ledStatusCounter++;
 //Serial.println((ledStatusCounter >> 13) & 1);
 digitalWrite(led, (ledStatusCounter >> 13) & 1);
 
}   
