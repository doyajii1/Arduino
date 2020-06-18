/**
  This example is for Series 1 XBee (802.15.4) as msg sender:

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
// SoftwareSerial setup with ssRX/TX pin
SoftwareSerial softSerial(ssRX, ssTX);


XBee xbee = XBee();

// Two bytes to hold a 10-bit data used for msg.
uint32_t payload[6];

// With Series 1 you can use either 16-bit or 64-bit addressing
// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0x1874, (uint8_t*)payload, sizeof(payload));

// 64-bit addressing: This is the SH + SL address of remote XBee
//XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00001874);
// unless you have MY on the receiving radio set to FFFF, this will be received as a RX16 packet
//Tx64Request tx = Tx64Request(addr64, payload, sizeof(payload));

TxStatusResponse txStatus = TxStatusResponse();

char incomingByte;
String inData;

/*
 * Setup env. for serial comm.
 */
void setup() {
  // start software serial for xbee.
  softSerial.begin(9600);
  xbee.setSerial(softSerial);

  // start serial for bluetooth
  Serial.begin(115200);
  Serial.println("started");
}

/*
 * continuously prepare payload, send data, check response
 */
void loop() {
  //use this method to sample data from sensor
  setPayload();

  // sends data through Xbee
  Serial.println("Sending data");
  xbee.send(tx);


  if (xbee.readPacket(500)) {
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {

      // get the delivery status, the fifth byte
      xbee.getResponse().getZBTxStatusResponse(txStatus);
      if (txStatus.getStatus() == SUCCESS) {
        Serial.println("Got Response");
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.println("Error1: error on TX status response");
  }

  //delay is not needed but placed for easier view
  delay(1000);
}

/*
 * payload setter
 */
int i = 0;
void setPayload() {
  if(i >= 200){
    i = 0;
  }
  payload[0] = i;
  i++;
  payload[1] = 1;
  payload[2] = 2;
  payload[3] = 3;
  payload[4] = 4;
  payload[5] = 5;
}
