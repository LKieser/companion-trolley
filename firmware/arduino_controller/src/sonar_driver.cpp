#include <Arduino.h>
#include "sonar_driver.h"

const int trigPin[20] = {52,50,48,46,44,42,40,38};
const int echoPin[20] = {53,51,49,47,45,43,41,39};

float duration, distance;

void initSonar() {
    for (int i = 0; i < 8; i++) {
        pinMode(trigPin[i], OUTPUT);
        pinMode(echoPin[i], INPUT);
    }
}

void runOneSonar(int sonarIndex) {
  digitalWrite(trigPin[sonarIndex], LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin[sonarIndex], HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin[sonarIndex], LOW);

  duration = pulseIn(echoPin[sonarIndex], HIGH);
  distance = (duration*.0343)/2;
  Serial.print("Distance for sonar ");
  Serial.print(sonarIndex);
  Serial.print(": ");
  Serial.println(distance);
}

void readSonar() {
    for (int i = 0; i < 8; i++) {
        runOneSonar(i);
        delay(10);
    }
}