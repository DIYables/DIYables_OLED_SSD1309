# DIYables OLED SSD1309

Arduino library for **SSD1309-based monochrome OLED displays** over I2C.

[![DIYables 2.4 inch OLED Display Module SSD1309 128x64](https://diyables.io/images/products/2.4-inch-oled-display-module-ssd1309-128x64-1.jpg)](https://diyables.io/products/2.4-inch-oled-display-module-ssd1309-128x64)

**Product page:** [2.4-inch OLED Display Module SSD1309 128×64](https://diyables.io/products/2.4-inch-oled-display-module-ssd1309-128x64)

- Supports **128×64** and **128×32** displays (and other resolutions).
- Extends **Adafruit_GFX** — all graphics primitives are available.
- **API-compatible with Adafruit_SSD1306** for easy migration.
- Pure Arduino API — works on **all boards** (AVR, ESP32, ESP8266, STM32, RP2040, SAMD, nRF52, …).


# TUTORIALS
* [Arduino - SSD1309 OLED Display Module](https://arduinogetstarted.com/tutorials/arduino-ssd1309-oled-display)
* [Arduino Uno R4 - SSD1309 OLED Display Module](https://newbiely.com/tutorials/arduino-uno-r4/arduino-uno-r4-ssd1309-oled-display)
* [Arduino Nano - SSD1309 OLED Display Module](https://newbiely.com/tutorials/arduino-nano/arduino-nano-ssd1309-oled-display)
* [Arduino Mega - SSD1309 OLED Display Module](https://newbiely.com/tutorials/arduino-mega/arduino-mega-ssd1309-oled-display)
* [ESP32 - SSD1309 OLED Display Module](https://esp32io.com/tutorials/esp32-ssd1309-oled-display)
* [ESP8266 - SSD1309 OLED Display Module](https://newbiely.com/tutorials/esp8266/esp8266-ssd1309-oled-display)


# API Reference

The library mirrors the [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) API:

### Constructor

```cpp
DIYables_OLED_SSD1309(uint8_t width, uint8_t height,
                      TwoWire *twi = &Wire,
                      int8_t rst_pin = -1,
                      uint32_t clkDuring = 400000UL,
                      uint32_t clkAfter  = 100000UL);
```

### Initialisation

| Method | Description |
|--------|-------------|
| `bool begin(uint8_t switchvcc, uint8_t i2caddr, bool reset, bool periphBegin)` | Allocate buffer, init I2C, configure display. Returns `true` on success. |

### Display Control

| Method | Description |
|--------|-------------|
| `display()` | Push the RAM buffer to the OLED. |
| `clearDisplay()` | Clear the RAM buffer (all pixels off). |
| `invertDisplay(bool i)` | Invert display colours (hardware-level). |
| `dim(bool dim)` | Dim the display (`true`) or restore normal brightness. |
| `setContrast(uint8_t c)` | Set contrast (0–255). |

### Drawing

All [Adafruit_GFX](https://learn.adafruit.com/adafruit-gfx-graphics-library) primitives are inherited:

| Method | Description |
|--------|-------------|
| `drawPixel(x, y, color)` | Set/clear/invert a pixel. |
| `drawLine(x0, y0, x1, y1, color)` | Bresenham line. |
| `drawRect(x, y, w, h, color)` | Rectangle outline. |
| `fillRect(x, y, w, h, color)` | Filled rectangle. |
| `drawCircle(x, y, r, color)` | Circle outline. |
| `fillCircle(x, y, r, color)` | Filled circle. |
| `drawTriangle(…)` | Triangle outline. |
| `fillTriangle(…)` | Filled triangle. |
| `drawRoundRect(…)` / `fillRoundRect(…)` | Rounded rectangles. |
| `drawBitmap(x, y, bitmap, w, h, color)` | Monochrome bitmap. |
| `setCursor(x, y)` | Set text cursor position. |
| `setTextSize(s)` | Set text magnification. |
| `setTextColor(c)` | Set text foreground colour. |
| `print()` / `println()` | Print text (Arduino Print interface). |

### Optimised Fast Lines

| Method | Description |
|--------|-------------|
| `drawFastHLine(x, y, w, color)` | Fast horizontal line (direct buffer). |
| `drawFastVLine(x, y, h, color)` | Fast vertical line (direct buffer). |

### Buffer Access

| Method | Description |
|--------|-------------|
| `getPixel(x, y)` | Read pixel state from buffer. |
| `getBuffer()` | Get pointer to raw buffer. |

### Low-Level

| Method | Description |
|--------|-------------|
| `ssd1309_command(c)` | Send a raw command byte to the display. |

### Pixel Operation Constants

| Constant (preferred) | Alias | Value | Meaning |
|----------------------|-------|-------|---------|
| `SSD1309_PIXEL_OFF` | `SSD1309_BLACK` | 0 | Turn pixel off |
| `SSD1309_PIXEL_ON` | `SSD1309_WHITE` | 1 | Turn pixel on |
| `SSD1309_PIXEL_INVERSE` | `SSD1309_INVERSE` | 2 | Toggle pixel |

> **Note:** The old `SSD1309_BLACK`/`SSD1309_WHITE`/`SSD1309_INVERSE` names still work for Adafruit_SSD1306 compatibility.

## Examples

| Example | Description |
|---------|-------------|
| [HelloWorld](examples/HelloWorld/HelloWorld.ino) | Basic text output. |
| [DrawShapes](examples/DrawShapes/DrawShapes.ino) | Lines, rectangles, circles, triangles. |
| [Bitmap](examples/Bitmap/Bitmap.ino) | Drawing a monochrome bitmap. |
| [ContrastDim](examples/ContrastDim/ContrastDim.ino) | Contrast sweep and dim toggle. |

## Migrating from Adafruit_SSD1306

1. Replace `#include <Adafruit_SSD1306.h>` with `#include <DIYables_OLED_SSD1309.h>`.
2. Replace `Adafruit_SSD1306` with `DIYables_OLED_SSD1309`.
3. Replace `SSD1306_*` constants with `SSD1309_*` equivalents.
4. That's it — the API is identical.

## SSD1309 vs SSD1306

The SSD1309 is largely register-compatible with the SSD1306.  
Key differences:

- **No internal charge pump** – the SSD1309 relies on an external VCC supply. Most breakout boards include the necessary regulator, so this is transparent to the user.
- **Higher voltage range** – the SSD1309 supports up to 16 V VCC.
- The initialisation sequence in this library sends a charge-pump command for compatibility with modules that respond to it; true SSD1309 ICs will simply ignore it.

## Tested Hardware

| Board                   | Tested |
|-------------------------|:------:|
| Arduino Uno R3          |   ✅   |
| [DIYables STEM V3 *(works like Arduino Uno R3)*](https://diyables.io/products/diyables-stem-v3-fully-compatible-with-arduino-uno-r3) |   ✅   |
| Arduino Uno R4 WiFi     |   ✅   |
| [DIYables STEM V4 IoT *(works like Arduino Uno R4 WiFi)*](https://diyables.io/products/diyables-stem-v4-iot-fully-compatible-with-arduino-uno-r4-wifi) |   ✅   |
| [DIYables STEM V4B IoT *(works like Arduino Uno R4 WiFi)*](https://diyables.io/products/diyables-stem-v4b-iot-development-board-compatible-with-arduino-uno-r4-wifi-ra4m1-32-bit-arm-cortex-m4-with-esp32-s3-wifi-bluetooth-usb-c-for-learning-prototyping-education) |   ✅   |
| Arduino Uno R4 Minima   |   ✅   |
| [DIYables STEM V4 Edu *(works like Arduino Uno R4 Minima)*](https://diyables.io/stem-v4-edu) |   ✅   |
| Arduino Mega            |   ✅   |
| [DIYables MEGA2560 R3 *(works like Arduino Mega 2560 Rev3)*](https://diyables.io/atmega2560-board) |   ✅   |
| Arduino Due             |   ✅    |
| Arduino Giga            |   ✅    |
| [DIYables ESP32 Development Board](https://diyables.io/esp32-board) |   ✅   |
| [DIYables ESP32 S3, Uno-form factor](https://diyables.io/products/esp32-s3-development-board-with-esp32-s3-wroom-1-n16r8-wifi-bluetooth-uno-compatible-form-factor-works-with-arduino-ide) |   ✅   |
| Other boards            |   Not yet, expected to work    |

## About DIYables

[DIYables](https://diyables.io) provides high-quality, easy-to-use electronic components and modules for makers and educators. Visit us for tutorials, wiring guides, and more Arduino libraries.
