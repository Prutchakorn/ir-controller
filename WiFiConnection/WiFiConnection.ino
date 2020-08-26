#include<BossWiFi.h>

#define WIFI_STA_NAME "true_home2G_8xu1"
#define WIFI_STA_PASS "0824520932"

BossWiFi wifi = BossWiFi(WIFI_STA_NAME, WIFI_STA_PASS);

void setup() {  
  Serial.begin(115200);
  if (wifi.connect()) {
    Serial.println("hello");
  }
}

void loop() {
  if (wifi.isConnected()) {
    Serial.println("hi");
    delay(2000);
  }
 }
