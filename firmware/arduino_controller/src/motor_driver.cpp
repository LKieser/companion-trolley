#include <Adafruit_Sensor.h>

#include "motor_driver.h"

// ================= MOTOR =================
// Left motor
#define L_RPWM 5
#define L_LPWM 4
#define L_REN  7
#define L_LEN  6

// Right motor
#define R_RPWM 10
#define R_LPWM 11
#define R_REN  12
#define R_LEN  13

// PWM of 0=0V, 255=24V
// PWM of 191 = 2.69 m/s
// Calculate with RPM * 0.56(wheel sircumferance) / 60 sec

const int MAX_PWM = 50; // PWM of 191 = 18v = 2.69 m/s || PWM of 50 ~ 0.75 m/s
const int MIN_PWM = 10; // Lowest PWM before stall current
const int RAMP_STEP = 2;

// Soft start tracking
int currentLeftPWM = 0;
int currentRightPWM = 0;

// Soft ramp
int rampPWM(int current, int target) {
  if (current < target) {
    current += RAMP_STEP;
    if (current > target) current = target;
  } else if (current > target) {
    current -= RAMP_STEP;
    if (current < target) current = target;
  }
  return current;
}

// Motor control
void setMotor(int rpwm, int lpwm, int speedVal) {
  // add limits to PWM
  speedVal = constrain(speedVal, -MAX_PWM, MAX_PWM);
  // avoid stall currrent deadzone between 10 and -10
  if (speedVal > 0 && speedVal < MIN_PWM) {
    speedVal = MIN_PWM;
  }
  if (speedVal < 0 && speedVal > -MIN_PWM) {
    speedVal = -MIN_PWM;
  }

  if (speedVal > 0) {
    analogWrite(rpwm, speedVal);
    analogWrite(lpwm, 0);
  } else if (speedVal < 0) {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, -speedVal);
  } else {
    analogWrite(rpwm, 0);
    analogWrite(lpwm, 0);
  }
}

void setup_motor_driver() {
    // Set pins
    pinMode(L_REN, OUTPUT);
    pinMode(L_LEN, OUTPUT);
    pinMode(R_REN, OUTPUT);
    pinMode(R_LEN, OUTPUT);

    // enable motors
    digitalWrite(L_REN, HIGH);
    digitalWrite(L_LEN, HIGH);
    digitalWrite(R_REN, HIGH);
    digitalWrite(R_LEN, HIGH);

    Serial.println("System Ready");
}

void run_motors(int left_drive, int right_drive) {
  int targetLeft  = left_drive;
  int targetRight = right_drive;

  targetLeft  = constrain(targetLeft, -MAX_PWM, MAX_PWM);
  targetRight = constrain(targetRight, -MAX_PWM, MAX_PWM);

  currentLeftPWM  = rampPWM(currentLeftPWM, targetLeft);
  currentRightPWM = rampPWM(currentRightPWM, targetRight);

  setMotor(L_RPWM, L_LPWM, currentLeftPWM);
  setMotor(R_RPWM, R_LPWM, currentRightPWM);
}