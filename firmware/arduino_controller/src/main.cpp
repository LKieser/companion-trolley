#include <Arduino.h>
#include "sonar_driver.h"

void setup() {
  initSonar();
  Serial.begin(9600);
}

void loop() {
  readSonar();
}