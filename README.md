# DIYables OLED SSD1309

Arduino library for **SSD1309-based monochrome OLED displays** over I2C.

[![DIYables 2.4 inch OLED Display Module SSD1309 128x64](https://diyables.io/images/products/2.4-inch-oled-display-module-ssd1309-128x64-1.jpg)](https://diyables.io/products/2.4-inch-oled-display-module-ssd1309-128x64)

**Product page:** [2.4-inch OLED Display Module SSD1309 128×64](https://diyables.io/products/2.4-inch-oled-display-module-ssd1309-128x64)

- Supports **128×64** and **128×32** displays (and other resolutions).
- Extends **Adafruit_GFX** — all graphics primitives are available.
- **API-compatible with Adafruit_SSD1306** for easy migration.
- Pure Arduino API — works on **all boards** (AVR, ESP32, ESP8266, STM32, RP2040, SAMD, nRF52, …).

## Installation

### Arduino IDE Library Manager

1. Open **Sketch → Include Library → Manage Libraries…**
2. Search for **DIYables_OLED_SSD1309**
3. Click **Install**

The library will automatically install its dependency (**Adafruit GFX Library**) if it is not already present.

### Manual Installation

1. Download or clone this repository.
2. Copy the `DIYables_OLED_SSD1309` folder into your Arduino `libraries` directory.
3. Install the [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) if you have not already.

## Wiring (I2C)

| OLED Pin | Arduino Uno/Nano | ESP32 | ESP8266 |
|----------|-----------------|-------|---------|
| SDA      | A4              | 21    | D2 (GPIO 4) |
| SCL      | A5              | 22    | D1 (GPIO 5) |
| VCC      | 3.3 V / 5 V    | 3.3 V | 3.3 V  |
| GND      | GND             | GND   | GND     |

> **Note:** Most SSD1309 modules have a default I2C address of **0x3C**.
> Some modules use **0x3D** — check your module's documentation.

## Quick Start

```cpp
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <DIYables_OLED_SSD1309.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT  64
#define OLED_RESET     -1       // -1 if no reset pin
#define SCREEN_ADDRESS 0x3C

DIYables_OLED_SSD1309 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  if (!display.begin(SSD1309_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1309 init failed"));
    for (;;);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1309_PIXEL_ON);
  display.setCursor(10, 24);
  display.println(F("DIYables"));
  display.display();
}

void loop() { }
```

## API Reference

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

### Scrolling (Hardware)

| Method | Description |
|--------|-------------|
| `startscrollright(start, stop)` | Scroll right (page range 0–7). |
| `startscrollleft(start, stop)` | Scroll left. |
| `startscrolldiagright(start, stop)` | Diagonal scroll right. |
| `startscrolldiagleft(start, stop)` | Diagonal scroll left. |
| `stopscroll()` | Stop scrolling. |

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
| [ScrollText](examples/ScrollText/ScrollText.ino) | Hardware scrolling demo. |
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

## License

MIT License — see [LICENSE](LICENSE).

## About DIYables

[DIYables](https://diyables.io) provides high-quality, easy-to-use electronic components and modules for makers and educators. Visit us for tutorials, wiring guides, and more Arduino libraries.
