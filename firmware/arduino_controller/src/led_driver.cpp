#include <Adafruit_NeoPixel.h>

#define LED_PIN 2      // data pin connected to DIN
#define NUM_LEDS 60

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup_led_driver() {
    strip.begin();
    strip.show(); // initialize all LEDs off
}

void red() {
    strip.fill(strip.Color(255, 0, 0));
    strip.show();
}

void green() {
    strip.fill(strip.Color(0, 255, 0));
    strip.show();
}

void blue() {
    strip.fill(strip.Color(0, 0, 255));
    strip.show();
}