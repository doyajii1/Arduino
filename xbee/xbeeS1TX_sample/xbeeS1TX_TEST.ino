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
This example is for Series 1 XBee
Sends a TX16 or TX64 request with the value of analogRead(pin5) and checks the status response for success
Note: In my testing it took about 15 seconds for the XBee to start reporting success, so I've added a startup delay
*/

XBee xbee = XBee();

// allocate two bytes for to hold a 10-bit analog reading
uint32_t payload[6];

// with Series 1 you can use either 16-bit or 64-bit addressing

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0x1874, (uint8_t*)payload, sizeof(payload));

// 64-bit addressing: This is the SH + SL address of remote XBee
//XBeeAddress64 addr64 = XBeeAddress64(0x00000000, 0x00001874);
// unless you have MY on the receiving radio set to FFFF, this will be received as a RX16 packet
//Tx64Request tx = Tx64Request(addr64, payload, sizeof(payload));

TxStatusResponse txStatus = TxStatusResponse();

char incomingByte;
String inData;

void setup() {
  // start serial for xbee.
  nss.begin(9600);
  xbee.setSerial(nss);
  
  // start soft serial for bluetooth
  Serial.begin(115200);
}


void loop() {
  delay(500);
  payload[0] = 0;
  payload[1] = 1;
  payload[2] = 2;
  payload[3] = 3;
  payload[4] = 4;
  payload[5] = 5;
  Serial.println("Sending data");
  xbee.send(tx);
  if (xbee.readPacket(500)) {
    // got a response!
    // should be a znet tx status
    if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
      xbee.getResponse().getZBTxStatusResponse(txStatus);

      // get the delivery status, the fifth byte
      if (txStatus.getStatus() == SUCCESS) {
        // success.  time to celebrate
        Serial.println("Got Response");
      }
    }
  } else if (xbee.getResponse().isError()) {
    Serial.println("Error TX");   
  }
}
