/*
 * DIYables OLED SSD1309 – Contrast & Dim
 *
 * Demonstrates contrast control and dim functionality.
 *
 * Product page: https://diyables.io/products/2.4-inch-oled-display-module-ssd1309-128x64
 * Created by DIYables.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <DIYables_OLED_SSD1309.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C

DIYables_OLED_SSD1309 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1309_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1309 allocation failed"));
    for (;;)
      ;
  }

  // Draw a test pattern
  display.clearDisplay();
  display.fillRect(0, 0, 64, 32, SSD1309_PIXEL_ON);
  display.fillRect(64, 32, 64, 32, SSD1309_PIXEL_ON);
  display.setTextSize(1);
  display.setTextColor(SSD1309_PIXEL_INVERSE);
  display.setCursor(16, 28);
  display.println(F("Contrast Demo"));
  display.display();
  delay(2000);
}

void loop() {
  // Gradually increase contrast from 0 to 255
  for (int c = 0; c <= 255; c += 5) {
    display.setContrast((uint8_t)c);
    delay(30);
  }
  delay(1000);

  // Gradually decrease contrast from 255 to 0
  for (int c = 255; c >= 0; c -= 5) {
    display.setContrast((uint8_t)c);
    delay(30);
  }
  delay(1000);

  // Dim on / off toggle
  display.dim(true);
  delay(2000);
  display.dim(false);
  delay(2000);
}
