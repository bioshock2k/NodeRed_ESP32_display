#include "wifi_setup.h"
#include <WiFi.h>

#define WIFISSID1 "ZON-C080" // Put your WifiSSID here
#define PASSWORD1 "alvaroeomaior" // Put your wifi password here
#define LED 2

  // Set your Static IP address
  IPAddress local_IP(192, 168, 1, 187);
  // Set your Gateway IP address
  IPAddress gateway(192, 168, 1, 1);
  IPAddress subnet(255, 255, 255, 0);
  IPAddress primaryDNS(8, 8, 8, 8);   //optional
  IPAddress secondaryDNS(8, 8, 4, 4); //optional
 
 
 void wf :: all(){
  pinMode(LED, OUTPUT);
  int wifi_ON=0;
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    Serial.println("STA Failed to configure");
  }

  WiFi.begin(WIFISSID1, PASSWORD1);
   
  Serial.println();
  Serial.print("Wait for WiFi...");
  int not_wifi=0;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
    not_wifi++;
    
    if(not_wifi>40){
      
      //esp_deep_sleep_start();
      ESP.restart();
    }
  }
   digitalWrite(LED,HIGH);
  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
 }
