#include <Arduino.h>

void read_message_from_pi() {
    if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    Serial.println(msg);
    }
}