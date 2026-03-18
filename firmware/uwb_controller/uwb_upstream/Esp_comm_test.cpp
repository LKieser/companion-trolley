#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 CONNECTED");

  Serial.printf("PSRAM detected: %s\n", psramFound() ? "YES" : "NO");
  Serial.printf("Free heap: %u\n", ESP.getFreeHeap());
  Serial.printf("Free PSRAM: %u\n", ESP.getFreePsram());
}

void loop() {}