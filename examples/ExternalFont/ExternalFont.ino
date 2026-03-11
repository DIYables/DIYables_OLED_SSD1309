/*
 * DIYables OLED SSD1309 – External Font
 *
 * Demonstrates how to use custom fonts from the Adafruit GFX library
 * on the SSD1309 OLED display.
 *
 * Adafruit_GFX ships with a collection of FreeFont files located in
 * the Fonts/ folder of the library. Include the header for the desired
 * font, then activate it with display.setFont(&fontName).
 * Call display.setFont(NULL) to switch back to the built-in 5x7 font.
 *
 * Product page: https://diyables.io/products/2.4-inch-oled-display-module-ssd1309-128x64
 * Created by DIYables.
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <DIYables_OLED_SSD1309.h>

// Include the external fonts you want to use
// Full list: see Adafruit-GFX-Library/Fonts/
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT   64
#define OLED_RESET      -1
#define SCREEN_ADDRESS  0x3C

DIYables_OLED_SSD1309 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1309_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1309 allocation failed"));
    for (;;);
  }

  display.clearDisplay();
  display.display();
}

void loop() {
  showBuiltInFont();
  delay(3000);

  showSerifFont();
  delay(3000);

  showSansBoldFont();
  delay(3000);

  showMonoFont();
  delay(3000);
}

// ── Built-in 5×7 font (default) ─────────────────────────────────────
void showBuiltInFont() {
  display.clearDisplay();
  display.setFont(NULL);  // Switch back to the default built-in font
  display.setTextSize(1);
  display.setTextColor(SSD1309_PIXEL_ON);

  display.setCursor(0, 0);
  display.println(F("Built-in 5x7 font"));
  display.println();
  display.setTextSize(2);
  display.println(F("DIYables"));

  display.display();
}

// ── FreeSerif 9pt ────────────────────────────────────────────────────
void showSerifFont() {
  display.clearDisplay();
  display.setFont(&FreeSerif9pt7b);  // Activate the external font
  display.setTextSize(1);
  display.setTextColor(SSD1309_PIXEL_ON);

  // NOTE: With custom fonts the cursor Y is the text *baseline*,
  //       not the top-left corner as with the built-in font.
  display.setCursor(0, 14);
  display.println(F("FreeSerif 9pt"));
  display.setCursor(0, 38);
  display.println(F("DIYables OLED"));
  display.setCursor(0, 58);
  display.println(F("Hello World!"));

  display.display();
}

// ── FreeSansBold 12pt ────────────────────────────────────────────────
void showSansBoldFont() {
  display.clearDisplay();
  display.setFont(&FreeSansBold12pt7b);
  display.setTextSize(1);
  display.setTextColor(SSD1309_PIXEL_ON);

  display.setCursor(0, 20);
  display.println(F("SansBold"));
  display.setCursor(0, 52);
  display.println(F("DIYables"));

  display.display();
}

// ── FreeMono 9pt ─────────────────────────────────────────────────────
void showMonoFont() {
  display.clearDisplay();
  display.setFont(&FreeMono9pt7b);
  display.setTextSize(1);
  display.setTextColor(SSD1309_PIXEL_ON);

  display.setCursor(0, 14);
  display.println(F("FreeMono 9pt"));
  display.setCursor(0, 34);
  display.println(F("0123456789"));
  display.setCursor(0, 54);
  display.println(F("!@#$%^&*()"));

  display.display();
}
