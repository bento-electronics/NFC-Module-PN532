/**
 * This example demonstrates pushing a NDEF message from Arduino + NFC Shield to Android 4.0+ device
 *
 * This demo does not support UNO, because UNO board has only one HardwareSerial.
 * Do not try to use SoftwareSerial to control PN532, it won't work. 
 * SotfwareSerial is not fast and stable enough.
 * 
 * This demo only supports the Arduino board which has at least 2 Serial, 
 * Like Leonard(1 USB serial and 1 Hardware serial), Mega ect.
 *
 * Make sure your PN532 board is in HSU(High Speed Uart) mode.
 *
 * This demo is tested with Leonard.
 */


#include <PN5321.h>
#include <NFCLinkLayer.h>
#include <SNEP.h>

#include <NdefMessage.h>

/** Use hardware serial to control PN532 */
// PN532                    Arduino
// VCC            -->          5V
// GND            -->         GND
// RXD            -->      Serial1-TX
// TXD            -->      Serail1-RX
/** Serial1 can be  */
PN532 nfc(Serial1);
NFCLinkLayer linkLayer(&nfc);
SNEP snep(&linkLayer);


// NDEF messages
#define MAX_PKT_HEADER_SIZE  50
#define MAX_PKT_PAYLOAD_SIZE 100
uint8_t txNDEFMessage[MAX_PKT_HEADER_SIZE + MAX_PKT_PAYLOAD_SIZE];
uint8_t *txNDEFMessagePtr; 
uint8_t txLen;

void setup(void) {
  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("----------------- nfc ndef push url --------------------"));


  txNDEFMessagePtr = &txNDEFMessage[MAX_PKT_HEADER_SIZE];
  NdefMessage message = NdefMessage();
  message.addUriRecord("http://hackersatberkeley.com");
  txLen = message.getEncodedSize();
  if (txLen <= MAX_PKT_PAYLOAD_SIZE) {
    message.encode(txNDEFMessagePtr);
  } 
  else {
    Serial.println("Tx Buffer is too small.");
    while (1) {
    }
  }

  nfc.initializeReader();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); 
  Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); 
  Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); 
  Serial.println((versiondata>>8) & 0xFF, DEC);
  Serial.print("Supports "); 
  Serial.println(versiondata & 0xFF, HEX);

  nfc.SAMConfig();
}

void loop(void) 
{
  Serial.println();
  Serial.println(F("---------------- LOOP ----------------------"));
  Serial.println();

  uint32_t txResult = GEN_ERROR;

  if (IS_ERROR(nfc.configurePeerAsTarget(SNEP_SERVER))) {
    extern uint8_t pn532_packetbuffer[];

    Serial.println(F("\nSNEP Sever:Blocking wait response."));
    nfc.readspicommand(PN532_TGINITASTARGET, (PN532_CMD_RESPONSE *)pn532_packetbuffer, 0);
  }

  txResult = snep.pushPayload(txNDEFMessagePtr, txLen);
  Serial.print(F("Result: 0x"));
  Serial.println(txResult, HEX);     
  if(txResult == 0x00000001){
    delay(3000); 
  }
}




