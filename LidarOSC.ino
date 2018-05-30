#include <OSCMessage.h>

/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>

// WiFi network name and password:
const char * networkName = "dlink-4240";
const char * networkPswd = "ejjeb48364";

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
const char * udpAddress = "192.168.100.255";
const int udpPort = 3333;

//Are we currently connected?
boolean connected = false;

HardwareSerial Serial1(2);  // UART1/Serial1 pins 16,17

//The udp library class
WiFiUDP udp;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  Serial1.begin(115200);
  
  //Connect to the WiFi network
  connectToWiFi(networkName, networkPswd);
}

void loop(){
  //only send data when connected
  while(Serial1.available()>=9)
    {
        if((0x59 == Serial1.read()) && (0x59 == Serial1.read())) //Byte1 & Byte2
        {
            unsigned char t1 = Serial1.read(); //Byte3
            unsigned char t2 = Serial1.read(); //Byte4

            unsigned int distance = ( t2 << 8 ) + t1;
            
            t1 = Serial1.read(); //Byte5
            t2 = Serial1.read(); //Byte6

            unsigned int strength = ( t2 << 8 ) + t1;

            for(int i=0; i<3; i++) 
            { 
                Serial1.read(); ////Byte7,8,9
            }
            
            if(connected){
               //Send a packet
               udp.beginPacket(udpAddress,udpPort);
               OSCMessage msg_dst("/distance");
               msg_dst.add((int32_t)distance);
               msg_dst.add((int32_t)strength);
               msg_dst.send(udp);
               //udp.printf("distance %d\tStrength%d", distance, strength);
               udp.endPacket();
            }
        }
    }
}

void connectToWiFi(const char * ssid, const char * pwd){
  Serial.println("Connecting to WiFi network: " + String(ssid));

  // delete old config
  WiFi.disconnect(true);
  //register event handler
  WiFi.onEvent(WiFiEvent);
  
  //Initiate connection
  WiFi.begin(ssid, pwd);

  Serial.println("Waiting for WIFI connection...");
}

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());  
          //initializes the UDP state
          //This initializes the transfer buffer
          udp.begin(WiFi.localIP(),udpPort);
          connected = true;
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          connected = false;
          break;
    }
}
