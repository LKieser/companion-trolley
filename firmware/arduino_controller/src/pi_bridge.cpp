#include <Arduino.h>

void setup_pi_bridge() {
     Serial.setTimeout(20); // this is for the readStringUntil() function for the serial to stop it from blocking the full 1000ms when waiting for a message
}

void read_message_from_pi() {
    if (Serial.available() > 0) {
    String msg = Serial.readStringUntil('\n');
    msg.trim();
    // add in setting the message that comes in as the return of this function or a pointer passed to this function.
    // use the pointer method to get debug as one message and the cmd_vel as another message.
    }
}