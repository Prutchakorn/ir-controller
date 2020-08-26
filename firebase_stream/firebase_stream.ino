#include<BossWiFi.h>
#include <IOXhop_FirebaseESP32.h> //this library must use ArduinoJson 5.11.0 library

#define WIFI_STA_NAME "true_home2G_8xu1"
#define WIFI_STA_PASS "0824520932"

BossWiFi wifi = BossWiFi(WIFI_STA_NAME, WIFI_STA_PASS);

#define FIREBASE_HOST "irhomecontroller.firebaseio.com"
#define FIREBASE_AUTH "bqotYN8yenZB7HVK5ts9x648l2e9LGLSgUDgmWXH"

void setup() {
  Serial.begin(115200);
  if (wifi.connect()) {
      Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
      stream();
  }
}

void loop() {
  if (wifi.isConnected()) {
    Serial.println("hi");
    delay(2000);
  }
}

void stream() {
  Firebase.stream("test", [](FirebaseStream stream) {
    String eventType = stream.getEvent();
    eventType.toLowerCase();

    Serial.print("event: ");
    Serial.println(eventType);
    if (eventType == "put") {
      Serial.print("data: ");
      Serial.println(stream.getDataString());
      String path = stream.getPath();
      String data = stream.getDataString();
      JsonObject &objects = stream.getData();
      String type = objects["type"];
      Serial.println(type);
    }
  });
}


