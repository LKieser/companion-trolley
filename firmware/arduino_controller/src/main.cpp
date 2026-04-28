#include <Arduino.h>
#include "sonar_driver.h"
#include "pi_bridge.h"
#include "imu_driver.h"
#include "motor_driver.h"
#include "joystick_control.h"
#include "led_driver.h"

#define JOYSTICK_MODE 0
#define AUTONMOUS_MODE 1

// timing
unsigned long lastImuRead = 0;
unsigned long lastMotorRun = 0;
unsigned long lastButtonCheck = 0;
unsigned long lastFlash = 0;

const unsigned long imuPeriod  = 20;  // ms delay
const unsigned long motorPeriod = 20;
const unsigned long buttonPeriod = 50;
const unsigned long flashPeriod = 1000;

// motor control
int left_drive = 0;
int right_drive = 0;

// joystick
bool button_pressed = false;

// state
int mode = AUTONMOUS_MODE;
int led_state = 0;

void setup() {
  Serial.begin(230400);
  delay(1000); // needed for IMU to fully initialize
  setup_pi_bridge();
  setup_imu_driver();
  setup_motor_driver();
  setup_joystick();
  setup_led_driver();
}

void loop() {
    unsigned long now = millis();
    // IMU
    if (now - lastImuRead >= imuPeriod) {
      lastImuRead = now;
      read_imu();
    }
    // Joystick Button
    // if (now - lastButtonCheck >= buttonPeriod) {
    //   lastButtonCheck = now;
    //   button_pressed = read_joystick_button();
    //   if (button_pressed) {
    //     mode = !mode;
    //     Serial.print("Button Pressed. Mode switched to: ");
    //     Serial.println(mode);
    //   }
    // }
    // Motor operation
    if (now - lastMotorRun >= motorPeriod) {
      lastMotorRun = now;
      switch (mode) {
        case JOYSTICK_MODE:
          // read_joystick_drive(&left_drive, &right_drive);
          Serial.println("Joystick mode");
          blue();
          break;
        case AUTONMOUS_MODE:
          read_message_from_pi(&left_drive, &right_drive);
          if (left_drive != 0 || right_drive != 0) {
            green();
          }
          else {
            red();
          }
          break;
        default:
          left_drive = 0;
          right_drive = 0;
          Serial.println("Mode broken. Default to 0 PWM");
      }

    // if (now - lastFlash >= flashPeriod) {
    //   switch (mode) {
    //     case 0:
    //       red();
    //       break;
    //     case 1:
    //       green();
    //       break;
    //     case 2:
    //       blue();
    //       break;
    //   }
    //   led_state += 1;
    //   if (led_state > 3) {
    //     led_state = 0;
    //   }
      // Serial.println(led_state);
    // }
      // pwm messages for pi
      // Serial.print("VERBOSE,");
      // Serial.print(left_drive);
      // Serial.print(",");
      // Serial.println(right_drive);
      run_motors(left_drive, right_drive);
    }


}