#include <BossWiFi.h>
#include <IOXhop_FirebaseESP32.h> //this library must use ArduinoJson 5.11.0 library
#include <IRremote.h>

//BossWiFi wifi = BossWiFi("MyDickIsBig", "0824520932Boss");
BossWiFi wifi = BossWiFi("true_home2G_8xu1", "0824520932");
#define FIREBASE_HOST "irhomecontroller.firebaseio.com"
#define FIREBASE_AUTH "bqotYN8yenZB7HVK5ts9x648l2e9LGLSgUDgmWXH"

const int RECV_PIN = 23;
IRrecv irrecv(RECV_PIN);
decode_results results;

const int button = 15;
const int sound = 2;
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

void setup()
{
  Serial.begin(115200);
  if (wifi.connect())
  {
    pinMode(sound, OUTPUT);
    digitalWrite(sound, LOW);
    pinMode(button, INPUT);
    digitalWrite(button, HIGH);
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  }
}

bool isCloning = false;

void loop()
{
  if (wifi.isConnected())
  {
    if (digitalRead(button) == LOW)
    {
      irrecv.enableIRIn();
      irrecv.blink13(true);
      isCloning = true;
    }
    if (isCloning)
    {
      Serial.println("please, transmit your signal!!!");
      tone(500);
      if (irrecv.decode(&results))
      {
        Serial.println("Cloning...");
        receiveSignal();
      }
    }
    delay(1000);
  }
}

void receiveSignal()
{
  Serial.println("receive: ");
  int typeId = (results.decode_type == -1 || results.bits == 0) ? 0: results.decode_type;
  if (typeId != 0) {
    if (pushFirebase(typeId)) {
      Serial.println("Cloned");
      tone(1000);
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
    isCloning = false;
    return true;
  }
}

void tone(int t) {
  digitalWrite(sound, HIGH);
  delay(t);
  digitalWrite(sound, LOW);
}



