/**
on xbee shieldv1.1:
DOUT8
DIN9
TX/RX
ID 3332
DL 1874
MY 1874
BD 9600
AP 2
*/
#include "XBee.h"
#include <SoftwareSerial.h>

// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
SoftwareSerial nss(ssRX, ssTX);

/*
This example is for Series 1 XBee (802.15.4)
Receives either a RX16 or RX64 packet and sets a PWM value based on packet data.
Error led is flashed if an unexpected packet is received
*/

XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle 
Rx16Response rx16 = Rx16Response();
// unused at this communication
// Rx64Response rx64 = Rx64Response();


uint8_t option = 0;

uint32_t dataR[6];
uint8_t  *data = (uint8_t*)dataR;

void setup() {
  // start serial for xbee
  nss.begin(9600);
  xbee.setSerial(nss);
  
  // start serial for bluetooth
  Serial.begin(115200);
  Serial.println("started");
}

static unsigned long lastTimeItHappened = 0;
// continuously reads packets, looking for RX16 or RX64
void loop() {

  // Notify board is alive every 5 secs
  if (millis() - lastTimeItHappened >= 5000) {
    lastTimeItHappened = millis();
    Serial.println("listening...");
  }
  
  xbee.readPacket();  
  if (xbee.getResponse().isAvailable()) {

    // got something
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {
      
      // got a rx packet
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
        xbee.getResponse().getRx16Response(rx16);
        option = rx16.getOption();
        for (int i = 0; i < sizeof(dataR); i++){
          data[i] = rx16.getData(i);
        }
      } else {
        Serial.println("Error 16bit Response");
      }

      Serial.println(dataR[0]);
      Serial.println(dataR[1]);
      Serial.println(dataR[2]);
      Serial.println(dataR[3]);
      Serial.println(dataR[4]);                                
      Serial.println(dataR[5]);      
    }
    else {
      Serial.println("Error Received 64 bit instead 16 bit response or undefined response.");
    }
    
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error reading packet.  Error code: ");  
    Serial.println(xbee.getResponse().getErrorCode());
  } 
}
