#include <Arduino.h>
#include "sonar_driver.h"
#include "pi_bridge.h"

void setup() {
  // initSonar();
  Serial.begin(115200);
  Serial.setTimeout(20); // this is for the readStringUntil() function to stop it from blocking the full 1000ms when waiting for a message
}

void loop() {
  read_message_from_pi();
  // readSonar();
}