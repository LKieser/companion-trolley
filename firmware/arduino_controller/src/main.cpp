#include <Arduino.h>
#include "sonar_driver.h"
#include "pi_bridge.h"
#include "imu_driver.h"
#include "motor_driver.h"
#include "joystick_control.h"

#define JOYSTICK_MODE 0
#define AUTONMOUS_MODE 1

// timing
unsigned long lastImuRead = 0;
unsigned long lastMotorRun = 0;
unsigned long lastButtonCheck = 0;

const unsigned long imuPeriod  = 20;  // ms delay
const unsigned long motorPeriod = 20; // ms delay
const unsigned long buttonPeriod = 50;

// motor control
int left_drive = 0;
int right_drive = 0;

// joystick
bool button_pressed = false;

// state
int mode = JOYSTICK_MODE;

void setup() {
  Serial.begin(115200);
  delay(1000); // needed for IMU to fully initialize
  setup_pi_bridge();
  setup_imu_driver();
  setup_motor_driver();
}

void loop() {
    unsigned long now = millis();
    // IMU
    if (now - lastImuRead >= imuPeriod) {
      lastImuRead = now;
      read_imu();
    }
    // Joystick Button
    if (now - lastButtonCheck >= buttonPeriod) {
      lastButtonCheck = now;
      button_pressed = read_joystick_button();
      if (button_pressed) {
        mode = !mode;
      }
      // Serial.println(mode);
    }

    if (now - lastMotorRun >= motorPeriod) {
      lastMotorRun = now;
      // read_joystick_drive(&left_drive, &right_drive);
      run_motors(left_drive, right_drive);
    }

    read_message_from_pi();
}