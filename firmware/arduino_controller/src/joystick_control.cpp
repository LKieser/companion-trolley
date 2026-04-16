#include <Adafruit_Sensor.h>

#include "imu_driver.h"
#include "joystick_control.h"
#include "motor_driver.h"

// ================= JOYSTICK =================
#define JOY_X A0
#define JOY_Y A1
#define JOY_SW 8

int deadzone = 15;
int lastButtonState = 1;

// ================= PID =================
float Kp = 1.0;
float Ki = 1.0;
float Kd = 0.0;

float targetHeading = 0;
float currentHeading = 0;

float error = 0;
float lastError = 0;
float integral = 0;

float getHeading() {
  sensors_event_t event;
  bno.getEvent(&event);
  return event.orientation.x;
}

int applyDeadzone(int val, int center = 512) {
  if (abs(val - center) < deadzone) return center;
  return val;
}

int joystickToSigned(int val) {
  val = applyDeadzone(val);
  if (val == 512) return 0;
  return map(val, 0, 1023, -MAX_PWM, MAX_PWM);
}

void setup_joystick(){
    targetHeading = getHeading(); // gets heading from IMU
    pinMode(JOY_SW, INPUT_PULLUP);
}

bool read_joystick_button() {
    int buttonState = digitalRead(JOY_SW);
    // Serial.println(buttonState);
    if (lastButtonState == 1 && buttonState == 0) {
        if (buttonState == 0) {
            lastButtonState = buttonState;
            return true;
        }
    }

    lastButtonState = buttonState;
    return false;
}

void read_joystick_drive(int* left_drive, int* right_drive){
    // Read Joystick
    int rawX = analogRead(JOY_X);
    int rawY = analogRead(JOY_Y);

    int turnInput = joystickToSigned(rawX);
    int forwardInput = joystickToSigned(rawY);

    // Use PID
    targetHeading += (turnInput * 0.05);

    if (targetHeading > 360) targetHeading -= 360;
    if (targetHeading < 0) targetHeading += 360;

    currentHeading = getHeading();

    error = targetHeading - currentHeading;

    if (error > 180) error -= 360;
    if (error < -180) error += 360;

    integral += error;
    float derivative = error - lastError;

    float correction = Kp * error + Ki * integral + Kd * derivative;

    lastError = error;

    // Output commands
    *left_drive = forwardInput + correction;
    *right_drive = forwardInput - correction;

    // ===== DEBUG =====
    Serial.print("PWM L:");
    Serial.print(*left_drive);
    Serial.print(" R:");
    Serial.println(*right_drive);

    // Serial.print(" | HEAD:");
    // Serial.print(currentHeading);

    // Serial.print(" | ERR:");
    // Serial.println(error);
}

