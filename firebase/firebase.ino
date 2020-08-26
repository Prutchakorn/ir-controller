#include<WiFi.h>
#include <IOXhop_FirebaseESP32.h> //this library must use ArduinoJson 5.11.0 library

#define WIFI_STA_NAME "MyDickIsBig"
#define WIFI_STA_PASS "0824520932Boss"

#define FIREBASE_HOST "irhomecontroller.firebaseio.com"
#define FIREBASE_AUTH "bqotYN8yenZB7HVK5ts9x648l2e9LGLSgUDgmWXH "

bool isWiFiConnected = false;

void setup() {
  if (WiFiConnected()) {
    isWiFiConnected = true;
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  } else {
    isWiFiConnected = false;
  }
}

void loop() {
  if (isWiFiConnected) {
    firebaseConnection();
  }
}

int i = 0;
void firebaseConnection() {
  Firebase.setInt("IntNumber", i);
  if (Firebase.failed()) {

    Serial.println("set /IntNumber failed:");
    Serial.println(Firebase.error());
    return;
  }

  Serial.print("set /IntNumber to ");
  Serial.println(Firebase.getInt("IntNumber"));

  i++;

  delay(500);
}

bool WiFiConnected() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_STA_NAME);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

  byte timeout = 0;
  while (WiFi.status() != WL_CONNECTED && timeout < 30) {
    delay(500);
    Serial.print(".");
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    timeout++;
  }

  if (timeout < 30) {
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.print("WiFi connection is timeout!!!");
    return false;
  }

}


