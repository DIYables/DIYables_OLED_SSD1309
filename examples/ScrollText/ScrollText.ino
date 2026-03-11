/*
 * DIYables OLED SSD1309 – Scroll Text
 *
 * Demonstrates the hardware scrolling features of the SSD1309 OLED.
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

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1309_PIXEL_ON);
  display.setCursor(10, 24);
  display.println(F("DIYables"));
  display.display();
  delay(2000);
}

void loop() {
  // Scroll right
  display.startscrollright(0x00, 0x07);
  delay(3000);
  display.stopscroll();
  delay(500);

  // Scroll left
  display.startscrollleft(0x00, 0x07);
  delay(3000);
  display.stopscroll();
  delay(500);

  // Diagonal scroll right
  display.startscrolldiagright(0x00, 0x07);
  delay(3000);
  display.stopscroll();
  delay(500);

  // Diagonal scroll left
  display.startscrolldiagleft(0x00, 0x07);
  delay(3000);
  display.stopscroll();
  delay(500);
}
