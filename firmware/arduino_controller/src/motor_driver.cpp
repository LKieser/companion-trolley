#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>

#include "imu_driver.h"

// ================= IMU =================
// Adafruit_BNO055 bno = Adafruit_BNO055(55);

// ================= MOTOR PINS =================
// Left motor
#define L_RPWM 5
#define L_LPWM 4
#define L_REN  7
#define L_LEN  6

// Right motor (FIXED TO PWM PINS)
#define R_RPWM 10
#define R_LPWM 11
#define R_REN  8
#define R_LEN  9

// ================= JOYSTICK =================
#define JOY_X A0
#define JOY_Y A1

// ================= PID =================
float Kp = 2.0;
float Ki = 0.0;
float Kd = 0.5;

float targetHeading = 0;
float currentHeading = 0;

float error = 0;
float lastError = 0;
float integral = 0;

// ================= SETTINGS =================
int deadzone = 15;   // smaller = more responsive
const int MAX_PWM = 191;
const int RAMP_STEP = 5;

// Soft start tracking
int currentLeftPWM = 0;
int currentRightPWM = 0;

// Debug timing
unsigned long lastPrint = 0;
const int PRINT_INTERVAL = 100;

// ================= FUNCTIONS =================

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
  speedVal = constrain(speedVal, -MAX_PWM, MAX_PWM);

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

// ================= SETUP =================
void setup_motor_driver() {
  Serial.begin(115200);

  pinMode(L_REN, OUTPUT);
  pinMode(L_LEN, OUTPUT);
  pinMode(R_REN, OUTPUT);
  pinMode(R_LEN, OUTPUT);

  // ALWAYS ENABLE MOTORS (CRITICAL FIX)
  digitalWrite(L_REN, HIGH);
  digitalWrite(L_LEN, HIGH);
  digitalWrite(R_REN, HIGH);
  digitalWrite(R_LEN, HIGH);

  if (!bno.begin()) {
    Serial.println("BNO055 not detected!");
    while (1);
  }

  delay(1000);
  bno.setExtCrystalUse(true);

  targetHeading = getHeading();

  Serial.println("System Ready");
}

// ================= LOOP =================
void run_motors() {
  int rawX = analogRead(JOY_X);
  int rawY = analogRead(JOY_Y);

  int turnInput = joystickToSigned(rawX);
  int forwardInput = joystickToSigned(rawY);

  // ===== HEADING CONTROL =====
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

  int targetLeft  = forwardInput + correction;
  int targetRight = forwardInput - correction;

  targetLeft  = constrain(targetLeft, -MAX_PWM, MAX_PWM);
  targetRight = constrain(targetRight, -MAX_PWM, MAX_PWM);

  currentLeftPWM  = rampPWM(currentLeftPWM, targetLeft);
  currentRightPWM = rampPWM(currentRightPWM, targetRight);

  setMotor(L_RPWM, L_LPWM, currentLeftPWM);
  setMotor(R_RPWM, R_LPWM, currentRightPWM);

  // ===== DEBUG =====
  if (millis() - lastPrint > PRINT_INTERVAL) {
    lastPrint = millis();

    Serial.print("FWD:");
    Serial.print(forwardInput);

    Serial.print(" | PWM L:");
    Serial.print(currentLeftPWM);
    Serial.print(" R:");
    Serial.print(currentRightPWM);

    Serial.print(" | HEAD:");
    Serial.print(currentHeading);

    Serial.print(" | ERR:");
    Serial.println(error);
  }

  delay(20);
}