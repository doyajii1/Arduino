/**
  This example is for Series 1 XBee (802.15.4) as msg receiver:

  Used pins on xbee shield(move jumper caps to match followings):
  DOUT8
  DIN9

  XCTU XBee S1 Config(TX/RX XBee):
  ID 3332
  DL 1874
  MY 1874
  BD 9600
  AP 2
*/

//SoftwareSerial is used to simulate serial comm. to XBee S1
#include <SoftwareSerial.h>
//got this from somewhere long ago, don't remember where I got them from 
#include "XBee.h"


// Connect Arduino pin 8 to TX of usb-serial device
uint8_t ssRX = 8;
// Connect Arduino pin 9 to RX of usb-serial device
uint8_t ssTX = 9;
//SoftwareSerial setup with ssRX/TX pin
SoftwareSerial softSerial(ssRX, ssTX);


XBee xbee = XBee();
XBeeResponse response = XBeeResponse();
// create reusable response objects for responses we expect to handle
Rx16Response rx16 = Rx16Response();

// received data is stored on dataR
uint32_t dataR[6];
uint8_t  *data = (uint8_t*)dataR;

// temp time holder for heart beat msg.
static unsigned long timeKeeper = 0;
// print interval (ex. 10 sec = 10000)
static unsigned long printAliveEvery = 10000;


/*
 * Setup env. for serial comm.
 */
void setup() {
  // start serial for xbee
  softSerial.begin(9600);
  xbee.setSerial(softSerial);

  // start serial for serial monitor or bluetooth(later?)
  Serial.begin(115200);
  Serial.println("started");
}


/*
 * continuously reads packets, looking for RX16 or RX64
 */
void loop() {

  // Notify monitor board is alive every specified seconds
  if (millis() - timeKeeper >= printAliveEvery) {
    timeKeeper = millis();
    Serial.println("alive...");
  }

  xbee.readPacket();
  if (xbee.getResponse().isAvailable()) {

    // got something
    if (xbee.getResponse().getApiId() == RX_16_RESPONSE || xbee.getResponse().getApiId() == RX_64_RESPONSE) {

      // got a rx packet
      if (xbee.getResponse().getApiId() == RX_16_RESPONSE) {
        xbee.getResponse().getRx16Response(rx16);

        for (int i = 0; i < sizeof(dataR); i++) {
          data[i] = rx16.getData(i);
        }

        //data is ready:
        Serial.print(dataR[0]);
        for (int i = 1; i <= 5; i++) {
          Serial.print(",");
          Serial.print(dataR[i]);
        }
        Serial.println();

      } else {
        Serial.println("Error 1: response handling for 64bit not implemented");
      }
    }
    else {
      Serial.println("Error 2: error on getting 16/64 API ID from response.");
    }
  } else if (xbee.getResponse().isError()) {
    Serial.print("Error 3: Error reading packet. Error code: ");
    Serial.println(xbee.getResponse().getErrorCode());
  }

}
