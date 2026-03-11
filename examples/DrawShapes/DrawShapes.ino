/*
 * DIYables OLED SSD1309 – Draw Shapes
 *
 * Demonstrates drawing primitives on a 128x64 I2C OLED display
 * driven by the SSD1309 controller.
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
}

void loop() {
  demoPrimitives();
  demoFilledShapes();
  demoRoundRects();
  demoTriangles();
}

// ------------------------------------------------------------------
void demoPrimitives() {
  display.clearDisplay();

  // Draw individual pixels
  for (int16_t i = 0; i < display.width(); i += 4) {
    display.drawPixel(i, i * display.height() / display.width(), SSD1309_PIXEL_ON);
  }
  display.display();
  delay(1500);

  // Draw lines
  display.clearDisplay();
  for (int16_t i = 0; i < display.width(); i += 8) {
    display.drawLine(0, 0, i, display.height() - 1, SSD1309_PIXEL_ON);
  }
  for (int16_t i = 0; i < display.height(); i += 8) {
    display.drawLine(0, 0, display.width() - 1, i, SSD1309_PIXEL_ON);
  }
  display.display();
  delay(1500);

  // Draw rectangles
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 4) {
    display.drawRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                     SSD1309_PIXEL_ON);
  }
  display.display();
  delay(1500);
}

// ------------------------------------------------------------------
void demoFilledShapes() {
  // Filled rectangles
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2; i += 6) {
    display.fillRect(i, i, display.width() - 2 * i, display.height() - 2 * i,
                     SSD1309_PIXEL_INVERSE);
  }
  display.display();
  delay(1500);

  // Circles
  display.clearDisplay();
  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 5) {
    display.drawCircle(display.width() / 2, display.height() / 2, i,
                       SSD1309_PIXEL_ON);
  }
  display.display();
  delay(1500);

  // Filled circles
  display.clearDisplay();
  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 6) {
    display.fillCircle(display.width() / 2, display.height() / 2, i,
                       SSD1309_PIXEL_INVERSE);
  }
  display.display();
  delay(1500);
}

// ------------------------------------------------------------------
void demoRoundRects() {
  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 4) {
    display.drawRoundRect(i, i, display.width() - 2 * i,
                          display.height() - 2 * i, display.height() / 4,
                          SSD1309_PIXEL_ON);
  }
  display.display();
  delay(1500);

  display.clearDisplay();
  for (int16_t i = 0; i < display.height() / 2 - 2; i += 4) {
    display.fillRoundRect(i, i, display.width() - 2 * i,
                          display.height() - 2 * i, display.height() / 4,
                          SSD1309_PIXEL_INVERSE);
  }
  display.display();
  delay(1500);
}

// ------------------------------------------------------------------
void demoTriangles() {
  display.clearDisplay();
  for (int16_t i = 0; i < max(display.width(), display.height()) / 2; i += 5) {
    display.drawTriangle(
        display.width() / 2, display.height() / 2 - i,
        display.width() / 2 - i, display.height() / 2 + i,
        display.width() / 2 + i, display.height() / 2 + i, SSD1309_PIXEL_ON);
  }
  display.display();
  delay(1500);

  display.clearDisplay();
  for (int16_t i = max(display.width(), display.height()) / 2; i > 0; i -= 6) {
    display.fillTriangle(
        display.width() / 2, display.height() / 2 - i,
        display.width() / 2 - i, display.height() / 2 + i,
        display.width() / 2 + i, display.height() / 2 + i, SSD1309_PIXEL_INVERSE);
  }
  display.display();
  delay(1500);
}
