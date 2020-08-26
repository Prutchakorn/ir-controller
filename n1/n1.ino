#include <BossWiFi.h>
#include <MicroGear.h>
#include <ArduinoJson.h>
#include <IRremote.h> // send pin 3 or 9
#include <IOXhop_FirebaseESP32.h> //this library must use ArduinoJson 5.11.0 library

//BossWiFi wifi = BossWiFi("true_home2G_8xu1", "0824520932");
//BossWiFi wifi = BossWiFi("MyDickIsBig", "0824520932Boss");
BossWiFi wifi = BossWiFi("AndroidAP", "12345678");

#define APPID   "IRHomeController"
#define KEY     "jOXh9JkDCQ0HM9D"
#define SECRET  "UOxlaPdawNjghaQ8Ui5gRKfeQ"
#define ALIAS   "N1Alias"

#define FIREBASE_HOST "irhomecontroller.firebaseio.com"
#define FIREBASE_AUTH "bqotYN8yenZB7HVK5ts9x648l2e9LGLSgUDgmWXH"

WiFiClient client;
int t = 0;
MicroGear microgear(client);

IRsend irsend;
const int RECV_PIN = 4;
IRrecv irrecv(RECV_PIN);
decode_results results;
const int cloneLED = 2;

char state = 0;
bool stateOutdated = 0;
char buff[16];

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

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  Serial.print("Incoming message --> ");
  msg[msglen] = '\0';
  Serial.println((char *)msg);
  String input = (char *)msg;
  if (input == "power") {
    clone("on");
    clone("off");
  } else if (input == "mute") {
    clone("mute");
  } else if (input == "chminus") {
    clone("chminus");
  } else if (input == "chplus") {
    clone("chplus");
  } else if (input == "volminus") {
    clone("volminus");
  } else if (input == "volplus") {
    clone("volplus");
  } else {
    DynamicJsonBuffer jsonBuffer;
    JsonObject& root = jsonBuffer.parseObject(input);
    String type = root[String("type")];
    unsigned long decCode = root[String("decCode")];
    int bits = root[String("bits")];
    Serial.println(type);
    Serial.println(decCode);
    Serial.println(bits);

    if (type == "SAMSUNG") {
      irsend.sendSAMSUNG(decCode, bits);
    } else if (type == "NEC") {
      irsend.sendNEC(decCode, bits);
    }
  }
}
void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}
void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++)
    Serial.print((char)msg[i]);
  Serial.println();
}
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  microgear.setAlias(ALIAS);

}

void setup() {
  microgear.on(MESSAGE, onMsghandler);
  microgear.on(PRESENT, onFoundgear);
  microgear.on(ABSENT, onLostgear);
  microgear.on(CONNECTED, onConnected);

  Serial.begin(115200);

  pinMode(LED_BUILTIN, OUTPUT);
  if (wifi.connect()) {
    microgear.init(KEY, SECRET, ALIAS);
    microgear.connect(APPID);
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }
}

void loop() {
  if (microgear.connected()) {
    microgear.loop();
    if (t >= 1000) {

      t = 0;
    }
    else t += 100;
  } else {
    Serial.println("connection lost, reconnect...");
    if (t >= 5000) {
      microgear.connect(APPID);
      t = 0;
    }
    else t += 100;
  }
  delay(100);
}

bool isCloned = false;

void clone(String title) {
  irrecv.enableIRIn();
  irrecv.blink13(true);
  while (!isCloned) {
    digitalWrite(cloneLED, HIGH);
    Serial.println("Cloning...");
    Serial.println(irrecv.decode(&results));
    if (irrecv.decode(&results)) {
      receiveSignal();
    }
    delay(500);
    digitalWrite(cloneLED, LOW);
    delay(500);
  }
  digitalWrite(cloneLED, LOW);
  isCloned = false;
  microgear.chat("android", "cloned" + title);

  //TODO clear
  while (!microgear.connected()) {
    Serial.println("connection lost, reconnect...");
    if (t >= 5000) {
      microgear.connect(APPID);
      microgear.chat("android", "cloned" + title);
      t = 0;
    }
    else t += 100;
  }
  microgear.chat("android", "cloned" + title);
}

void receiveSignal()
{
  Serial.println("receive: ");
  int typeId = (results.decode_type == -1 || results.bits == 0) ? 0 : results.decode_type;
  if (typeId != 0) {
    if (pushFirebase(typeId)) {
      Serial.println("Cloned");
      isCloned = true;
    } else {
      Serial.println("Try again!!!");
    }
  } else {
    Serial.println("Try again!!!");
  }
  irrecv.resume(); // Receive the next value
}

bool pushFirebase(int typeId) {
  JsonObject &info = StaticJsonBuffer<200>().createObject();
  info["typeId"] = typeId;
  info["type"] = signalType[typeId];
  info["hexCode"] = String(results.value, HEX);
  info["decCode"] = results.value;
  info["bits"] = results.bits;
  info.prettyPrintTo(Serial);
  Serial.println();
  Firebase.push("signals", info);
  if (Firebase.failed())
  {
    Serial.println("Firebase failed:");
    Serial.println(Firebase.error());
    return false;
  } else {
    return true;
  }
  delay(5000);
}



