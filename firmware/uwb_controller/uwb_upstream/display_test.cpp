#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

// Configuration for the 1.3" OLED on the ESP32 UWB Pro
// Pins for this board are typically: SDA=4, SCL=5
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 5, /* data=*/ 4);

void setup() {
  u8g2.begin();
}

void loop() {
  u8g2.clearBuffer();					// clear the internal memory
//   u8g2.setFont(u8g2_font_ncenB08_tr);	// choose a suitable font
//   u8g2.drawStr(0,10,"ESP32 UWB Pro");	// write something to the internal memory
//   u8g2.drawStr(0,30,"Battery System OK");
//   u8g2.drawStr(0,50,"I wish I had been more careful with my battery");
  u8g2.sendBuffer();					// transfer internal memory to the display
  delay(1000);
}