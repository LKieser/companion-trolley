#include <Wire.h>

#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <utility/imumaths.h>
#include "motor_driver.h"


Adafruit_BNO055 bno = Adafruit_BNO055(55);

bool imu_ok = true;

void setup_imu_driver() {
  if (!bno.begin()) {
    imu_ok = false;
  }

  bno.setExtCrystalUse(true);
}

void read_imu() {
    if (!imu_ok) {
        // Serial.println("DEBUG, BNO055 IMU not detected");
        return;
    }

    imu::Quaternion quat = bno.getQuat();
    imu::Vector<3> gyro = bno.getVector(Adafruit_BNO055::VECTOR_GYROSCOPE);
    imu::Vector<3> accel = bno.getVector(Adafruit_BNO055::VECTOR_LINEARACCEL);

    Serial.print("IMU,");

    // Heading
    Serial.print(quat.w(), 6); Serial.print(",");
    Serial.print(quat.x(), 6); Serial.print(",");
    Serial.print(quat.y(), 6); Serial.print(",");
    Serial.print(quat.z(), 6); Serial.print(",");

    // Angular velocity
    Serial.print(gyro.x(), 6); Serial.print(",");
    Serial.print(gyro.y(), 6); Serial.print(",");
    Serial.print(gyro.z(), 6); Serial.print(",");

    // Linear acceleration
    Serial.print(accel.x(), 6); Serial.print(",");
    Serial.print(accel.y(), 6); Serial.print(",");
    Serial.print(accel.z(), 6);

    Serial.println();  // end of line
}