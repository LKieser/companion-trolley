#include <Arduino.h>
#include "motor_driver.h"

#define WHEEL_BASE 0.43
// max with 18V is 2.69 m/s. After load assuming only 75% efficiency: 2.69 * 0.75 = 2.02 m/s
// 0.56 equates with 50 PWM for testing
#define MAX_SPEED 0.56

void convert_to_pwm(float linear_x, float angular_z, int* left_drive, int* right_drive);

void setup_pi_bridge() {
     Serial.setTimeout(20); // this is for the readStringUntil() function for the serial to stop it from blocking the full 1000ms when waiting for a message
}

void read_message_from_pi(int* left_drive, int* right_drive) {
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        // Parse the string
        int index_one = input.indexOf(',');
        if (index_one == -1) {
            Serial.print("DEBUG, Bad message on arduino serial");
        } // protect against no comma
        String header = input.substring(0, index_one); // grabs the substring from position 1 to 1 before the comma position

        if (header == "CMD_VEL") {
            int index_two = input.indexOf(',', index_one + 1); // starts at one spot ahead of the last index and looks for next comma

            // Extract the two floats
            float linear_x = input.substring(index_one + 1, index_two).toFloat();
            float angular_z = input.substring(index_two + 1).toFloat();
            convert_to_pwm(linear_x, angular_z, left_drive, right_drive);
        }
    }
}

void convert_to_pwm(float linear_x, float angular_z, int* left_drive, int* right_drive) {
    // formula: left velocity (m/s) = linear_velocity (m/s) - (angular_velocity (rad/s) * wheel_base/2 (m))
    // formula: right velocity (m/s) = linear_velocity (m/s) + (angular_velocity (rad/s) * wheel_base/2 (m))
    float left_velocity = linear_x - (angular_z * (WHEEL_BASE/2));
    float right_velocity = linear_x + (angular_z * (WHEEL_BASE/2));

    // ensure neither value is higher than it should be
    float max_velocity = max(abs(left_velocity), abs(right_velocity));
    if (max_velocity > MAX_SPEED) {
        float scale = MAX_SPEED / max_velocity;
        left_velocity *= scale;
        right_velocity *= scale;
    }

    // map the velocity to the correct PWM value
    *left_drive  = (int)((left_velocity / MAX_SPEED) * MAX_PWM);
    *right_drive = (int)((right_velocity / MAX_SPEED) * MAX_PWM);

    // motor reads these commands backwards from what is sent
    *left_drive *= -1;
    *right_drive *= -1;
}