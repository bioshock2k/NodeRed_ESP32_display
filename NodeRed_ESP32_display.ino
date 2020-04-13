////////////////////////////////////////////////////////////
// 		    Code writen by: bioshock2k  4/13/2020         //
//	Github repository:  bioshock2k /ESP32_House_monitoring//
////////////////////////////////////////////////////////////
#include <OLEDDisplay.h>
#include <OLEDDisplayFonts.h>
#include <OLEDDisplayUi.h>
#include <SH1106.h>
#include <SH1106Brzo.h>
#include <SH1106Spi.h>
#include <SH1106Wire.h>
#include <SSD1306.h>
#include <SSD1306Brzo.h>
#include <SSD1306I2C.h>
#include <SSD1306Spi.h>
#include <SSD1306Wire.h>

#include <ETH.h>
#include <WiFi.h>


#include <PubSubClient.h>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "esp_err.h"

#include "wifi_setup.h"

RTC_DATA_ATTR int bootCount = 0;
String p1 = "00";
String p2 = "00";
String p3 = "00";
int c1,c2,c3=0;
//RTC Configuration
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  30        /* Time ESP32 will go to sleep (in seconds) */


 //IN
const char* mqtt_server = "192.168.1.72";
float temperature = 0;

float value; // To store incoming value.
uint8_t variable; // To keep track of the state machine and be able to use the switch case.
const int ERROR_VALUE = 65535; // Set here an error value

long lastMsg = 0;
char msg[20];
//objeto controlador do display de led
/*
  0x3c  :   é um identificador único para comunicação do display
  pino 5 e 4 são os de comunicação (SDA, SDC)   
*/
SSD1306  screen(0x3c, 5, 4);

wf wf;        //WIFI config library

//pino que ligamos o potenciometr

#define C1_TOPIC    "garage/ocurrent1"
#define C2_TOPIC    "garage/ocurrent2"
#define C3_TOPIC    "garage/ocurrent3"
#define C4_TOPIC    "garage/current4"
//utilizado para fazer o contador de porcentagem
int contador;

// *************************************************************** //
WiFiClient espClient;
PubSubClient client(espClient);

int fan=0;
void receivedCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic[15]);
  
  char a;
  Serial.print("Payload :");
  for (int i = 0; i < length; i++) {
      Serial.print((char)payload[i]);
      
    }
    Serial.println();

  if(topic[15]=='1'){
      
        if((char)payload[1]=='.'){
          p1[0]='0';
          p1[1]=(char)payload[0];
          c1=payload[2];
        }
        else{
         p1[0]=(char)payload[0];
         p1[1]=(char)payload[1];
         c1=payload[3];
        }
        
    Serial.print("Payload 1: ");
    Serial.println(p1);
  }
   if(topic[15]=='2'){  
       

        if((char)payload[1]=='.'){
          p2[0]='0';
          p2[1]=(char)payload[0];
          c2=payload[2];
        }
        else{
          p2[0]=(char)payload[0];
          p2[1]=(char)payload[1];
          c2=payload[3];
        }
    Serial.print("Payload 2: ");
    Serial.println(p2);
  }
   if(topic[15]=='3'){
      

        if((char)payload[1]=='.'){
          p3[0]='0';
          p3[1]=(char)payload[0];
          c3=payload[2];
        }
        else{
          p3[0]=(char)payload[0];
          p3[1]=(char)payload[1];
          c3=payload[3];
        }
    Serial.print("Payload 3: ");
    Serial.println(p3);
  }
}

void mqttconnect() {
  /* Loop until reconnected */
   int trys=0;
  while (!client.connected()) {
    Serial.print("MQTT connecting ...");
    /* client ID */
    String clientId = "ESP32Client2";
    /* connect now */
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      trys=0;
      /* subscribe topic with default QoS 0*/
      client.subscribe(C1_TOPIC);
      client.subscribe(C2_TOPIC);
      client.subscribe(C3_TOPIC);
    } 
    else if( trys>20){
    ESP.restart();
    trys=0;
    }
    else {
      trys++;
      Serial.print("failed, status code =");
      Serial.print(client.state());
      Serial.println("try again in 3 seconds");
      Serial.print("trys :");
      Serial.print(trys);
      /* Wait 5 seconds before retrying */
      delay(3000);
    }
  }
}

void setup() {
  btStop();       //Disable bluetooth
  Serial.begin(115200);
  Serial.println();

  ++bootCount;
  Serial.println("Boot number: " + String(bootCount));
  
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  
  // Inicializa o objeto que controlará o que será exibido na tela
  screen.init();
 
  //gira o display 180º (deixa de ponta cabeça)
 // display.flipScreenVertically();
 
  //configura a fonte de escrita "ArialMT_Plain_10"
  screen.setFont(ArialMT_Plain_24);
 
  //---------------------------------   //WIFI BEGIN
  
  wf.all();                             // WIFI configuration
   /* configure the MQTT server with IPaddress and port */
  client.setServer(mqtt_server, 1883);
  /* this receivedCallback function will be invoked 
  when client received subscribed topic */
  client.setCallback(receivedCallback);
}

void loop() 
{
  if (!client.connected()) {  //Runs once!
    mqttconnect();
   }
 
  //limpa todo o display, apaga o contúdo da tela
  screen.clear();

  screen.drawString(0, 15, String(p1));
  screen.drawProgressBar(0, 40, 24, 3, (c1*100/9));
  screen.drawString(48, 15, String(p2));
  screen.drawProgressBar(48, 40, 24, 3, (c2*100/9));
  screen.drawString(98, 15, String(p3));
  screen.drawProgressBar(98, 40, 24, 3, (c3*100/9));

  
  //desenha a progress bar
 // drawProgressBar();
 
  //exibe na tela o que foi configurado até então.
  screen.display();
  client.loop();
  
long now = millis();
  if (now - lastMsg > 2000) {
    lastMsg = now;
  }
}

//função para desenhar a progress bar no display
void drawProgressBar() {
   
  Serial.print(">> ");
  Serial.println(contador);
   
  // desenha a progress bar
  /*
   * drawProgressBar(x, y, width, height, value);
    parametros (p):
      p1: x       --> coordenada X no plano cartesiano
      p2: y       --> coordenada Y no plano cartesiano
      p3: width   --> comprimento da barra de progresso
      p4: height  --> altura da barra de progresso
      p5: value   --> valor que a barra de progresso deve assumir
     
  */
  screen.drawProgressBar(10, 32, 100, 10, contador);
  // configura o alinhamento do texto que será escrito
  //nesse caso alinharemos o texto ao centro
  screen.setTextAlignment(TEXT_ALIGN_CENTER);
   
  //escreve o texto de porcentagem
  /*
   * drawString(x,y,text);
    parametros (p):
     p1: x      --> coordenada X no plano cartesiano
     p2: y      --> coordenada Y no plano cartesiano
     p3: string --> texto que será exibido
  */
  screen.drawString(64, 15, String(contador) + "%");
 
  //se o contador está em zero, escreve a string "valor mínimo"
  if(contador == 0){
    screen.drawString(64, 45, "Valor mínimo");
  }
  //se o contador está em 100, escreve a string "valor máximo"
  else if(contador == 100){
    screen.drawString(64, 45, "Valor máximo");
  }
}
