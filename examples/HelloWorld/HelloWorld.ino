/*
 * DIYables OLED SSD1309 – Hello World
 *
 * Demonstrates basic text output on a 128x64 I2C OLED display
 * driven by the SSD1309 controller.
 *
 * Product page: https://diyables.io/products/2.4-inch-oled-display-module-ssd1309-128x64
 * Created by DIYables.
 *
 * TESTED HARDWARE
 * - Arduino Uno R3
 * - Arduino Uno R4 WiFi
 * - Arduino Uno R4 Minima
 * - Arduino Mega
 * - Arduino Due
 * - Arduino Giga
 * - DIYables STEM V3: https://diyables.io/stem-v3
 * - DIYables STEM V4 IoT: https://diyables.io/stem-v4-iot
 * - DIYables STEM V4B IoT: https://diyables.io/stem-v4b-iot
 * - DIYables STEM V4 Edu: https://diyables.io/stem-v4-edu
 * - DIYables MEGA2560 R3: https://diyables.io/atmega2560-board
 * - DIYables ESP32 Board: https://diyables.io/esp32-board
 * - DIYables ESP32 S3, Uno-form factor: https://diyables.io/esp32-s3-uno
 * - It is expected to work with other boards
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <DIYables_OLED_SSD1309.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1   // No reset pin (or set to actual pin#)
#define SCREEN_ADDRESS 0x3C // 0x3C for most modules

DIYables_OLED_SSD1309 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  // Initialise the OLED
  if (!display.begin(SSD1309_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1309 allocation failed"));
    for (;;)
      ; // halt
  }

  display.clearDisplay();

  // Draw "Hello, World!" in the default font
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1309_PIXEL_ON);  // Pixels on
  display.setCursor(0, 0);
  display.println(F("Hello, World!"));
  display.println();
  display.setTextSize(2);
  display.println(F("DIYables"));
  display.setTextSize(1);
  display.println();
  display.println(F("SSD1309 OLED 128x64"));
  display.display();                    // Push buffer to screen
}

void loop() {
  // Nothing to do here
}
