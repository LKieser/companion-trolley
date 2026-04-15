#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "motor_driver.h"


Adafruit_BNO055 bno = Adafruit_BNO055(55);

float velX = 0;
float velY = 0;
float velZ = 0;

unsigned long lastTime = 0;

void setup_imu_driver() {
  if (!bno.begin()) {
    Serial.println("BNO055 not detected!");
    while (1);
  }

  delay(1000);

  bno.setExtCrystalUse(true);
  lastTime = millis();
  Serial.println("BNO055 Ready");
}



void read_imu() {
  unsigned long currentTime = millis();
  float dt = (currentTime - lastTime) / 1000.0;
  lastTime = currentTime;

  // ORIENTATION (Euler)

  imu::Vector<3> euler = bno.getVector(Adafruit_BNO055::VECTOR_EULER);

  float heading = euler.x();   // Compass Heading
  float roll    = euler.z();
  float pitch   = euler.y();

  // LINEAR ACCELERATION (gravity removed)
  imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

  float ax = accel.x();
  float ay = accel.y();
  float az = accel.z();

  // Integrate acceleration -> velocity
  velX += ax * dt;
  velY += ay * dt;
  velZ += az * dt;

  // Print Data
  Serial.print("Heading: ");
  Serial.print(heading);

  Serial.print("  Pitch: ");
  Serial.print(pitch);

  Serial.print("  Roll: ");
  Serial.print(roll);

  Serial.print("  | Vel X: ");
  Serial.print(velX);

  Serial.print("  Vel Y: ");
  Serial.print(velY);

  Serial.print("  Vel Z: ");
  Serial.println(velZ);

  delay(200);
}