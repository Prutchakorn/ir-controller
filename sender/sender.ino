#include <IRremote.h> // send pin 3 or 9

IRsend irsend;
decode_results results;

void setup() {
  Serial.begin(115200);

}

char signalType[20][20] = {
  "UNKNOWN",
  "UNUSED"
  "RC5",
  "RC6",
  "NEC",
  "SONY",
  "PANASONIC",
  "JVC",
  "SAMSUNG",
  "WHYNTER",
  "AIWA_RC_T501",
  "LG",
  "SANYO",
  "MITSUBISHI",
  "DISH",
  "SHARP",
  "DENON",
  "PRONTO",
  "LEGO_PF"
};

void loop() {
  sendSignal();
}

unsigned int  rawData[73] = {9000,4500, 650,1650, 650,550, 650,550, 650,600, 600,600, 600,600, 650,550, 650,550, 650,1650, 650,550, 650,550, 650,1700, 600,600, 650,550, 650,550, 650,550, 650,550, 650,550, 650,600, 600,600, 650,550, 650,1650, 650,550, 650,550, 650,550, 650,550, 650,600, 600,600, 650,1650, 650,550, 650,1650, 650,550, 650,550, 650,1700, 600,600, 650};  // NEC 8090040A
unsigned int  data = 0x8090040A;

void sendSignal() {
  Serial.println("send: ");
  //  irsend.sendSAMSUNG(0xe0e040bf, 32);
//  irsend.sendNEC(0x8090040A, 32);
  irsend.sendRaw(rawData,73,38);    
  delay(5000);
}

