#include <IRremote.h>

const int RECV_PIN = 3;
IRrecv irrecv(RECV_PIN);
decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn(); // Start the receiver
  irrecv.blink13(true);
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
  "SAIJODENKI"
};

void loop() {
  if (irrecv.decode(&results)) {
    receiveSignal();
  }
}

void receiveSignal() {
  Serial.println("receive: ");
  signalInfo();
  irrecv.resume(); // Receive the next value
}

void signalInfo() {
  int index = results.decode_type;
  if (index == -1 || results.bits == 0) {
    index = 0;
  }
  Serial.println(index);
  Serial.print(signalType[index]);
  Serial.print(":");
  Serial.println(results.value, HEX);
  Serial.println(results.value);
  Serial.print("bits: ");
  Serial.println(results.bits);
  for(int i = 0; i < results.rawlen; i++) {
    Serial.print(results.rawbuf[i]);
    Serial.print(" ");
  }
  Serial.println();
  delay(2000);
}

