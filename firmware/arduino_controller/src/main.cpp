#include <Arduino.h>
#include "sonar_driver.h"
#include "pi_bridge.h"
#include "imu_driver.h"

void setup() {
  Serial.begin(115200);
  delay(1000); // needed for IMU to fully initialize
  setup_pi_bridge();
  setup_imu_driver();
}

void loop() {
  read_message_from_pi();
  read_imu();
}