#include <Arduino.h>
#include "sonar_driver.h"
#include "pi_bridge.h"
#include "imu_driver.h"

unsigned long lastImuRead = 0;

const unsigned long imuPeriodMs  = 20;  // 50 Hz

void setup() {
  Serial.begin(115200);
  delay(1000); // needed for IMU to fully initialize
  setup_pi_bridge();
  setup_imu_driver();
}

void loop() {
  unsigned long now = millis();

  if (now - lastImuRead >= imuPeriodMs) {
    lastImuRead = now;
    read_imu();
  }

  read_message_from_pi();
}