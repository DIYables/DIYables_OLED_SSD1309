# DIYables OLED SSD1309 – Library Reference

> **Library version:** 1.0.0  
> **Last updated:** March 2026  
> **License:** MIT

---

## Table of Contents

- [Overview](#overview)
- [Supported Hardware](#supported-hardware)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Wiring (I2C)](#wiring-i2c)
- [Class: DIYables_OLED_SSD1309](#class-diyables_oled_ssd1309)
  - [Constructor](#constructor)
  - [begin()](#begin)
  - [Display Control](#display-control)
    - [display()](#display)
    - [clearDisplay()](#cleardisplay)
    - [invertDisplay()](#invertdisplay)
    - [dim()](#dim)
    - [setContrast()](#setcontrast)
  - [Drawing Primitives (Overrides)](#drawing-primitives-overrides)
    - [drawPixel()](#drawpixel)
    - [drawFastHLine()](#drawfasthline)
    - [drawFastVLine()](#drawfastvline)
    - [fillRect()](#fillrect)
  - [Scrolling (Hardware)](#scrolling-hardware)
    - [startscrollright()](#startscrollright)
    - [startscrollleft()](#startscrollleft)
    - [startscrolldiagright()](#startscrolldiagright)
    - [startscrolldiagleft()](#startscrolldiagleft)
    - [stopscroll()](#stopscroll)
  - [Buffer Access](#buffer-access)
    - [getPixel()](#getpixel)
    - [getBuffer()](#getbuffer)
  - [Low-Level Commands](#low-level-commands)
    - [ssd1309_command()](#ssd1309_command)
- [Constants](#constants)
  - [Pixel Operation Constants](#pixel-operation-constants)
  - [VCC Selection Constants](#vcc-selection-constants)
  - [SSD1309 Command Defines](#ssd1309-command-defines)
- [Inherited Adafruit_GFX Methods](#inherited-adafruit_gfx-methods)
  - [Lines](#lines)
  - [Rectangles](#rectangles)
  - [Circles](#circles)
  - [Triangles](#triangles)
  - [Rounded Rectangles](#rounded-rectangles)
  - [Bitmaps](#bitmaps)
  - [Text](#text)
  - [External Fonts](#external-fonts)
  - [Display Rotation](#display-rotation)
- [Memory Usage](#memory-usage)
- [Performance Notes](#performance-notes)
- [Migrating from Adafruit_SSD1306](#migrating-from-adafruit_ssd1306)
- [SSD1309 vs SSD1306](#ssd1309-vs-ssd1306)
- [Troubleshooting](#troubleshooting)
- [Examples](#examples)

---

## Overview

`DIYables_OLED_SSD1309` is an Arduino library for driving **SSD1309-based monochrome OLED displays** over the **I2C** bus. It extends [Adafruit_GFX](https://github.com/adafruit/Adafruit-GFX-Library), giving you the full suite of graphics primitives (text, shapes, bitmaps, fonts), and deliberately mirrors the [Adafruit_SSD1306](https://github.com/adafruit/Adafruit_SSD1306) API so existing sketches can be migrated with minimal changes.

The library uses only the standard Arduino API (`Wire.h`, `digitalWrite`, `pinMode`, etc.), so it runs on **all boards** — AVR, ESP32, ESP8266, STM32, RP2040, SAMD, nRF52, and more.

---

## Supported Hardware

### Display Resolutions

| Width × Height | COM Pins Config | Notes |
|----------------|-----------------|-------|
| 128 × 64       | 0x12 (alt)      | Most common size |
| 128 × 32       | 0x02 (seq)      | Half-height panels |
| 96 × 16        | 0x02 (seq)      | Small panels |
| 64 × 48        | 0x12 (alt)      | Micro OLED |
| 64 × 32        | 0x12 (alt)      | Compact |

### Boards

Any board with an Arduino-compatible core and a hardware I2C peripheral:

- **AVR** — Arduino Uno, Nano, Mega, Leonardo, Pro Mini, etc.
- **ESP32** — ESP32, ESP32-S2, ESP32-S3, ESP32-C3
- **ESP8266** — NodeMCU, Wemos D1 Mini, etc.
- **RP2040** — Raspberry Pi Pico, Pico W
- **SAMD** — Arduino Zero, MKR series, Adafruit M0/M4
- **STM32** — Blue Pill, Nucleo, etc.
- **nRF52** — Adafruit Feather nRF52840, etc.
- **Teensy** — 3.x, 4.x
- Any other core that provides `Wire.h`

---

## Dependencies

| Library | Purpose |
|---------|---------|
| [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) | Base graphics class (auto-installed by Library Manager) |
| `Wire.h` | I2C communication (built-in with all Arduino cores) |

---

## Installation

### Via Arduino IDE Library Manager

1. **Sketch → Include Library → Manage Libraries…**
2. Search for **DIYables_OLED_SSD1309**
3. Click **Install**

The Adafruit GFX dependency is installed automatically.

### Manual

1. Download or clone the repository.
2. Copy the `DIYables_OLED_SSD1309` folder into your Arduino `libraries/` directory.
3. Install [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library) if not already present.

---

## Wiring (I2C)

| OLED Pin | Arduino Uno / Nano | Arduino Mega | ESP32 | ESP8266 | RP2040 (Pico) |
|----------|-------------------|--------------|-------|---------|---------------|
| SDA      | A4                | 20           | 21    | D2 (GPIO 4)  | GP4 |
| SCL      | A5                | 21           | 22    | D1 (GPIO 5)  | GP5 |
| VCC      | 3.3 V or 5 V     | 3.3 V or 5 V| 3.3 V | 3.3 V  | 3.3 V |
| GND      | GND               | GND          | GND   | GND     | GND |

> Most SSD1309 modules default to I2C address **0x3C**. Some use **0x3D** — check your module's documentation or run an I2C scanner sketch.

---

## Class: DIYables_OLED_SSD1309

```cpp
class DIYables_OLED_SSD1309 : public Adafruit_GFX
```

Inherits from `Adafruit_GFX`. All GFX methods are available in addition to the SSD1309-specific methods documented below.

---

### Constructor

```cpp
DIYables_OLED_SSD1309(uint8_t w, uint8_t h,
                      TwoWire *twi = &Wire,
                      int8_t rst_pin = -1,
                      uint32_t clkDuring = 400000UL,
                      uint32_t clkAfter  = 100000UL);
```

Creates a display object. **Does not** allocate memory or talk to the display — call `begin()` for that.

#### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `w` | `uint8_t` | — | Display width in pixels (e.g. `128`). |
| `h` | `uint8_t` | — | Display height in pixels (e.g. `64` or `32`). |
| `twi` | `TwoWire *` | `&Wire` | Pointer to the I2C bus instance. Use `&Wire1` for the second I2C bus on boards that support it. |
| `rst_pin` | `int8_t` | `-1` | Hardware reset pin. Pass `-1` if not connected. |
| `clkDuring` | `uint32_t` | `400000` | I2C clock speed (Hz) used during display transfers. |
| `clkAfter` | `uint32_t` | `100000` | I2C clock speed (Hz) restored after transfers, so other I2C devices are not affected. |

#### Example

```cpp
// 128x64 display on default I2C bus, no reset pin
DIYables_OLED_SSD1309 display(128, 64, &Wire, -1);

// 128x32 display on second I2C bus, reset on pin 4, 1 MHz I2C
DIYables_OLED_SSD1309 display(128, 32, &Wire1, 4, 1000000UL, 100000UL);
```

> **Note on I2C clock management:** On Arduino ≥ 1.5.7, the library automatically calls `Wire.setClock(clkDuring)` before each transfer and `Wire.setClock(clkAfter)` afterwards. On older cores the clock parameters are ignored.

---

### begin()

```cpp
bool begin(uint8_t switchvcc   = SSD1309_SWITCHCAPVCC,
           uint8_t i2caddr     = 0x3C,
           bool    reset       = true,
           bool    periphBegin = true);
```

Allocates the frame buffer, optionally calls `Wire.begin()`, optionally performs a hardware reset, and runs the full SSD1309 initialisation command sequence. **Must be called once in `setup()` before any drawing.**

#### Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `switchvcc` | `uint8_t` | `SSD1309_SWITCHCAPVCC` | Voltage source. Use `SSD1309_SWITCHCAPVCC` (generate from 3.3 V) or `SSD1309_EXTERNALVCC` (external supply). Most modules use `SSD1309_SWITCHCAPVCC`. |
| `i2caddr` | `uint8_t` | `0x3C` | 7-bit I2C address of the display. |
| `reset` | `bool` | `true` | If `true` and a reset pin was provided in the constructor, pulse the reset pin during initialisation. |
| `periphBegin` | `bool` | `true` | If `true`, call `Wire.begin()` inside this function. Set to `false` if you have already called `Wire.begin()` elsewhere (e.g. with custom pins on ESP32). |

#### Returns

`true` on success, `false` if the frame buffer allocation failed (out of memory).

#### Example

```cpp
void setup() {
  Serial.begin(9600);

  if (!display.begin(SSD1309_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1309 init failed"));
    for (;;);  // halt
  }

  display.clearDisplay();
  display.display();
}
```

#### Example with custom I2C pins (ESP32)

```cpp
void setup() {
  Wire.begin(16, 17);  // SDA=16, SCL=17

  if (!display.begin(SSD1309_SWITCHCAPVCC, 0x3C, true, false)) {
    // periphBegin = false because we already called Wire.begin()
    Serial.println(F("SSD1309 init failed"));
    for (;;);
  }
}
```

---

### Display Control

---

#### display()

```cpp
void display(void);
```

Transfers the entire frame buffer from RAM to the SSD1309 display over I2C. Call this after you have finished drawing to make changes visible on screen.

The transfer is optimised with batched I2C writes (filling each I2C transaction up to the platform's buffer limit) and uses the fast I2C clock configured in the constructor.

```cpp
display.clearDisplay();
display.drawPixel(10, 20, SSD1309_PIXEL_ON);
display.display();  // now the pixel is visible
```

---

#### clearDisplay()

```cpp
void clearDisplay(void);
```

Sets every byte in the frame buffer to `0x00` (all pixels off). **Does not** update the physical display — call `display()` afterwards.

```cpp
display.clearDisplay();
display.display();  // screen goes blank
```

---

#### invertDisplay()

```cpp
void invertDisplay(bool i);
```

Inverts the display at the **hardware** level. When inverted, lit pixels become dark and vice versa. This does not modify the frame buffer — it is a real-time hardware effect.

| Parameter | Type | Description |
|-----------|------|-------------|
| `i` | `bool` | `true` = inverted, `false` = normal. |

```cpp
display.invertDisplay(true);   // inverted
delay(1000);
display.invertDisplay(false);  // back to normal
```

---

#### dim()

```cpp
void dim(bool dim);
```

Dims the display by setting the contrast register to `0`, or restores the previous contrast value.

| Parameter | Type | Description |
|-----------|------|-------------|
| `dim` | `bool` | `true` = dim (contrast 0), `false` = restore to the last contrast value set by `setContrast()` or the default from `begin()`. |

```cpp
display.dim(true);   // very dim
delay(2000);
display.dim(false);  // normal brightness
```

---

#### setContrast()

```cpp
bool setContrast(uint8_t contrast);
```

Sets the display contrast. Higher values = brighter.

| Parameter | Type | Range | Description |
|-----------|------|-------|-------------|
| `contrast` | `uint8_t` | 0–255 | Contrast level. |

Returns `true` (always succeeds).

The set value is remembered internally so that `dim(false)` restores to the correct level.

```cpp
display.setContrast(0);    // minimum brightness
delay(1000);
display.setContrast(255);  // maximum brightness
```

---

### Drawing Primitives (Overrides)

These methods override the default `Adafruit_GFX` implementations with optimised versions that write directly to the page-oriented frame buffer.

---

#### drawPixel()

```cpp
void drawPixel(int16_t x, int16_t y, uint16_t color);
```

Sets, clears, or inverts a single pixel in the frame buffer.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int16_t` | Horizontal position (0 = left). |
| `y` | `int16_t` | Vertical position (0 = top). |
| `color` | `uint16_t` | Pixel operation: `SSD1309_PIXEL_ON`, `SSD1309_PIXEL_OFF`, or `SSD1309_PIXEL_INVERSE`. |

Rotation is handled automatically.

```cpp
display.drawPixel(64, 32, SSD1309_PIXEL_ON);       // turn on
display.drawPixel(64, 32, SSD1309_PIXEL_OFF);      // turn off
display.drawPixel(64, 32, SSD1309_PIXEL_INVERSE);  // toggle
```

---

#### drawFastHLine()

```cpp
void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
```

Draws a horizontal line directly in the frame buffer. Significantly faster than the GFX default, which calls `drawPixel()` in a loop.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int16_t` | Starting X coordinate. |
| `y` | `int16_t` | Y coordinate. |
| `w` | `int16_t` | Width (length) in pixels. |
| `color` | `uint16_t` | `SSD1309_PIXEL_ON`, `SSD1309_PIXEL_OFF`, or `SSD1309_PIXEL_INVERSE`. |

```cpp
display.drawFastHLine(0, 32, 128, SSD1309_PIXEL_ON);  // full-width line
```

---

#### drawFastVLine()

```cpp
void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
```

Draws a vertical line directly in the frame buffer. Processes whole bytes (8 pixels at a time) for aligned sections, making it very efficient for tall lines.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int16_t` | X coordinate. |
| `y` | `int16_t` | Starting Y coordinate. |
| `h` | `int16_t` | Height (length) in pixels. |
| `color` | `uint16_t` | `SSD1309_PIXEL_ON`, `SSD1309_PIXEL_OFF`, or `SSD1309_PIXEL_INVERSE`. |

```cpp
display.drawFastVLine(64, 0, 64, SSD1309_PIXEL_ON);  // full-height line
```

---

#### fillRect()

```cpp
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
```

Fills a rectangle directly in the frame buffer. This override is significantly faster than the default GFX implementation (which calls `drawFastHLine` row-by-row). It performs rotation and clipping once, then writes whole page-bytes across the full width.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int16_t` | Top-left X coordinate. |
| `y` | `int16_t` | Top-left Y coordinate. |
| `w` | `int16_t` | Width in pixels. |
| `h` | `int16_t` | Height in pixels. |
| `color` | `uint16_t` | `SSD1309_PIXEL_ON`, `SSD1309_PIXEL_OFF`, or `SSD1309_PIXEL_INVERSE`. |

```cpp
display.fillRect(10, 10, 50, 30, SSD1309_PIXEL_ON);
```

> **Tip:** `clearDisplay()` is essentially `fillRect(0, 0, WIDTH, HEIGHT, SSD1309_PIXEL_OFF)` but uses `memset` for maximum speed.

---

### Scrolling (Hardware)

Hardware scrolling shifts the display content in real time without CPU involvement. The content wraps around the edges. Pages refer to 8-pixel-high horizontal strips (page 0 = rows 0–7, page 1 = rows 8–15, etc.).

> **Important:** You must call `display()` before starting a scroll to ensure the display RAM contains the content you want to scroll. Do not draw new content while scrolling is active — call `stopscroll()` first.

---

#### startscrollright()

```cpp
void startscrollright(uint8_t start, uint8_t stop);
```

Starts continuous rightward horizontal scrolling.

| Parameter | Type | Description |
|-----------|------|-------------|
| `start` | `uint8_t` | First page to scroll (0–7). |
| `stop` | `uint8_t` | Last page to scroll (0–7). |

```cpp
display.startscrollright(0x00, 0x07);  // scroll entire display right
```

---

#### startscrollleft()

```cpp
void startscrollleft(uint8_t start, uint8_t stop);
```

Starts continuous leftward horizontal scrolling.

| Parameter | Type | Description |
|-----------|------|-------------|
| `start` | `uint8_t` | First page to scroll (0–7). |
| `stop` | `uint8_t` | Last page to scroll (0–7). |

```cpp
display.startscrollleft(0x00, 0x07);
```

---

#### startscrolldiagright()

```cpp
void startscrolldiagright(uint8_t start, uint8_t stop);
```

Starts continuous diagonal scrolling (right + vertical).

| Parameter | Type | Description |
|-----------|------|-------------|
| `start` | `uint8_t` | First page (0–7). |
| `stop` | `uint8_t` | Last page (0–7). |

```cpp
display.startscrolldiagright(0x00, 0x07);
```

---

#### startscrolldiagleft()

```cpp
void startscrolldiagleft(uint8_t start, uint8_t stop);
```

Starts continuous diagonal scrolling (left + vertical).

| Parameter | Type | Description |
|-----------|------|-------------|
| `start` | `uint8_t` | First page (0–7). |
| `stop` | `uint8_t` | Last page (0–7). |

```cpp
display.startscrolldiagleft(0x00, 0x07);
```

---

#### stopscroll()

```cpp
void stopscroll(void);
```

Stops any active hardware scrolling. Always call this before drawing new content after a scroll.

```cpp
display.stopscroll();
```

---

### Buffer Access

---

#### getPixel()

```cpp
bool getPixel(int16_t x, int16_t y);
```

Reads the state of a single pixel from the frame buffer. Rotation is handled automatically.

| Parameter | Type | Description |
|-----------|------|-------------|
| `x` | `int16_t` | Horizontal position. |
| `y` | `int16_t` | Vertical position. |

**Returns:** `true` if the pixel is on, `false` if off or out of bounds.

```cpp
if (display.getPixel(10, 20)) {
  Serial.println("Pixel is ON");
}
```

---

#### getBuffer()

```cpp
uint8_t *getBuffer(void);
```

Returns a pointer to the raw frame buffer. The buffer is organised in **pages** (horizontal strips of 8 pixels high). Each byte represents 8 vertical pixels in one column, with bit 0 being the topmost pixel of that page.

**Returns:** `uint8_t *` — pointer to the buffer, or `NULL` if `begin()` has not been called.

**Buffer size:** `WIDTH * ((HEIGHT + 7) / 8)` bytes.

- 128 × 64 → 1024 bytes
- 128 × 32 → 512 bytes

**Buffer layout:**

```
Byte index = (page * WIDTH) + column
Bit 0 = topmost pixel of the page, Bit 7 = bottommost pixel of the page.
```

```cpp
uint8_t *buf = display.getBuffer();
buf[0] = 0xFF;  // first column, top 8 pixels all ON
display.display();
```

---

### Low-Level Commands

---

#### ssd1309_command()

```cpp
void ssd1309_command(uint8_t c);
```

Sends a single raw command byte to the SSD1309 over I2C. The I2C clock is boosted for the transfer and restored afterwards.

| Parameter | Type | Description |
|-----------|------|-------------|
| `c` | `uint8_t` | The SSD1309 command byte. |

Use this for advanced scenarios like custom init sequences or register tweaks. See [SSD1309 Command Defines](#ssd1309-command-defines) for available command constants.

```cpp
display.ssd1309_command(SSD1309_DISPLAYOFF);  // put display to sleep
delay(5000);
display.ssd1309_command(SSD1309_DISPLAYON);   // wake up
```

---

## Constants

### Pixel Operation Constants

These are passed as the `color` parameter to all drawing functions. They describe the **operation** performed on pixels, not a colour.

| Constant (preferred) | Alias (legacy) | Value | Description |
|----------------------|----------------|-------|-------------|
| `SSD1309_PIXEL_OFF` | `SSD1309_BLACK` | `0` | Turn the pixel **off** (clear). |
| `SSD1309_PIXEL_ON` | `SSD1309_WHITE` | `1` | Turn the pixel **on** (set). |
| `SSD1309_PIXEL_INVERSE` | `SSD1309_INVERSE` | `2` | **Toggle** the pixel (XOR). |

**Unscoped aliases:** `BLACK`, `WHITE`, `INVERSE` are also defined for convenience. Define `NO_DIYABLES_SSD1309_COLOR_COMPATIBILITY` before including the header to disable them (e.g. if they clash with another library).

```cpp
#define NO_DIYABLES_SSD1309_COLOR_COMPATIBILITY  // disable BLACK/WHITE/INVERSE
#include <DIYables_OLED_SSD1309.h>
```

> **Why not "black" and "white"?** The physical pixel colour depends on the panel hardware (white, blue, yellow, green, etc.) and is not controlled by software. `PIXEL_ON` / `PIXEL_OFF` / `PIXEL_INVERSE` describe the actual operation unambiguously.

---

### VCC Selection Constants

Passed to `begin()` as the `switchvcc` parameter.

| Constant | Value | Description |
|----------|-------|-------------|
| `SSD1309_EXTERNALVCC` | `0x01` | Display powered by external VCC supply. |
| `SSD1309_SWITCHCAPVCC` | `0x02` | Generate display voltage internally from 3.3 V (most modules). |

---

### SSD1309 Command Defines

These are the raw SSD1309 register/command bytes. They can be sent via `ssd1309_command()` for advanced use.

| Constant | Value | Description |
|----------|-------|-------------|
| `SSD1309_MEMORYMODE` | `0x20` | Set memory addressing mode. |
| `SSD1309_COLUMNADDR` | `0x21` | Set column start/end address. |
| `SSD1309_PAGEADDR` | `0x22` | Set page start/end address. |
| `SSD1309_SETCONTRAST` | `0x81` | Set contrast control register. |
| `SSD1309_CHARGEPUMP` | `0x8D` | Charge pump setting (SSD1306 compat). |
| `SSD1309_SEGREMAP` | `0xA0` | Set segment re-map. |
| `SSD1309_DISPLAYALLON_RESUME` | `0xA4` | Resume output from RAM content. |
| `SSD1309_DISPLAYALLON` | `0xA5` | Force entire display ON (ignore RAM). |
| `SSD1309_NORMALDISPLAY` | `0xA6` | Normal (non-inverted) display mode. |
| `SSD1309_INVERTDISPLAY` | `0xA7` | Inverted display mode. |
| `SSD1309_SETMULTIPLEX` | `0xA8` | Set multiplex ratio. |
| `SSD1309_DISPLAYOFF` | `0xAE` | Display OFF (sleep mode). |
| `SSD1309_DISPLAYON` | `0xAF` | Display ON (normal operation). |
| `SSD1309_COMSCANINC` | `0xC0` | COM output scan direction: increment. |
| `SSD1309_COMSCANDEC` | `0xC8` | COM output scan direction: decrement. |
| `SSD1309_SETDISPLAYOFFSET` | `0xD3` | Set display offset. |
| `SSD1309_SETDISPLAYCLOCKDIV` | `0xD5` | Set display clock divide ratio. |
| `SSD1309_SETPRECHARGE` | `0xD9` | Set pre-charge period. |
| `SSD1309_SETCOMPINS` | `0xDA` | Set COM pins hardware configuration. |
| `SSD1309_SETVCOMDETECT` | `0xDB` | Set VCOMH deselect level. |
| `SSD1309_SETSTARTLINE` | `0x40` | Set display start line (0x40–0x7F). |

**Scrolling commands:**

| Constant | Value | Description |
|----------|-------|-------------|
| `SSD1309_RIGHT_HORIZONTAL_SCROLL` | `0x26` | Right horizontal scroll setup. |
| `SSD1309_LEFT_HORIZONTAL_SCROLL` | `0x27` | Left horizontal scroll setup. |
| `SSD1309_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL` | `0x29` | Diagonal right scroll setup. |
| `SSD1309_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL` | `0x2A` | Diagonal left scroll setup. |
| `SSD1309_DEACTIVATE_SCROLL` | `0x2E` | Deactivate scrolling. |
| `SSD1309_ACTIVATE_SCROLL` | `0x2F` | Activate scrolling. |
| `SSD1309_SET_VERTICAL_SCROLL_AREA` | `0xA3` | Set vertical scroll area. |

---

## Inherited Adafruit_GFX Methods

`DIYables_OLED_SSD1309` inherits from `Adafruit_GFX`, so all standard GFX drawing methods are available. Below is a summary — see the [Adafruit GFX documentation](https://learn.adafruit.com/adafruit-gfx-graphics-library) for full details.

For all drawing methods, the `color` parameter accepts `SSD1309_PIXEL_ON`, `SSD1309_PIXEL_OFF`, or `SSD1309_PIXEL_INVERSE`.

---

### Lines

```cpp
void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
```

Draws a line between two points using Bresenham's algorithm.

```cpp
display.drawLine(0, 0, 127, 63, SSD1309_PIXEL_ON);
```

---

### Rectangles

```cpp
void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
```

Draw outline or filled rectangles. `fillRect` is overridden in this library with an optimised page-based implementation.

```cpp
display.drawRect(10, 10, 40, 20, SSD1309_PIXEL_ON);   // outline
display.fillRect(60, 10, 40, 20, SSD1309_PIXEL_ON);   // filled
```

---

### Circles

```cpp
void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
```

```cpp
display.drawCircle(64, 32, 20, SSD1309_PIXEL_ON);
display.fillCircle(64, 32, 15, SSD1309_PIXEL_INVERSE);
```

---

### Triangles

```cpp
void drawTriangle(int16_t x0, int16_t y0,
                  int16_t x1, int16_t y1,
                  int16_t x2, int16_t y2, uint16_t color);
void fillTriangle(int16_t x0, int16_t y0,
                  int16_t x1, int16_t y1,
                  int16_t x2, int16_t y2, uint16_t color);
```

```cpp
display.drawTriangle(64, 10, 30, 50, 98, 50, SSD1309_PIXEL_ON);
```

---

### Rounded Rectangles

```cpp
void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                   int16_t r, uint16_t color);
void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h,
                   int16_t r, uint16_t color);
```

```cpp
display.drawRoundRect(5, 5, 118, 54, 10, SSD1309_PIXEL_ON);
```

---

### Bitmaps

```cpp
void drawBitmap(int16_t x, int16_t y,
                const uint8_t bitmap[], int16_t w, int16_t h,
                uint16_t color);
```

Draws a 1-bit-per-pixel bitmap stored in PROGMEM. Bits set to `1` in the bitmap are drawn using the specified `color`; bits set to `0` are left unchanged (transparent).

```cpp
static const uint8_t PROGMEM myBitmap[] = {
  0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18  // 8x8 heart
};
display.drawBitmap(60, 28, myBitmap, 8, 8, SSD1309_PIXEL_ON);
```

> **Tip:** Use tools like [image2cpp](https://javl.github.io/image2cpp/) to convert images to byte arrays.

---

### Text

The library inherits the Arduino `Print` class, so `print()` and `println()` work directly.

```cpp
// Set up text properties
display.setTextSize(1);                   // 1 = default 6x8, 2 = 12x16, etc.
display.setTextColor(SSD1309_PIXEL_ON);   // foreground only (transparent bg)
display.setCursor(0, 0);                  // top-left
display.println(F("Hello, World!"));

// Text with background (avoids clearing before redraw)
display.setTextColor(SSD1309_PIXEL_ON, SSD1309_PIXEL_OFF);  // fg, bg

// Wrap text at right edge
display.setTextWrap(true);   // default is true
```

**Text sizing:** `setTextSize(n)` multiplies the base font by `n`. The built-in font is 5×7 pixels plus 1-pixel spacing = 6×8 effective size.

| Size | Char Width | Char Height | Chars per line (128px) |
|------|-----------|-------------|----------------------|
| 1    | 6 px      | 8 px        | 21                   |
| 2    | 12 px     | 16 px       | 10                   |
| 3    | 18 px     | 24 px       | 7                    |
| 4    | 24 px     | 32 px       | 5                    |

---

### External Fonts

Adafruit GFX ships with a large collection of scalable FreeFont files. Include the header for the font you want, then activate it with `setFont()`.

```cpp
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

// Use an external font
display.setFont(&FreeSerif9pt7b);
display.setTextSize(1);
display.setTextColor(SSD1309_PIXEL_ON);
display.setCursor(0, 20);  // NOTE: y is the baseline for external fonts
display.println("Serif");

// Switch back to the built-in font
display.setFont(NULL);
```

> **Important:** For external fonts, `setCursor()` sets the **baseline** position, not the top-left corner. The text renders **upward** from the cursor Y. For the built-in font, the cursor is at the top-left.

**Available font families** (in `Fonts/` folder of Adafruit GFX):

| Family | Sizes | Variants |
|--------|-------|----------|
| `FreeMono` | 9pt, 12pt, 18pt, 24pt | Regular, Bold, Oblique, BoldOblique |
| `FreeSans` | 9pt, 12pt, 18pt, 24pt | Regular, Bold, Oblique, BoldOblique |
| `FreeSerif` | 9pt, 12pt, 18pt, 24pt | Regular, Bold, Italic, BoldItalic |

> **Memory warning:** External fonts are stored in PROGMEM but can use significant flash space. On AVR boards (32 KB flash), use them sparingly.

---

### Display Rotation

```cpp
void setRotation(uint8_t r);
```

Rotates all subsequent drawing operations.

| Value | Rotation |
|-------|----------|
| `0` | No rotation (default, landscape). |
| `1` | 90° clockwise. |
| `2` | 180° (upside down). |
| `3` | 270° clockwise (90° counter-clockwise). |

All drawing methods — including `drawPixel`, `drawFastHLine`, `drawFastVLine`, `fillRect`, `getPixel`, text, bitmaps, etc. — respect the current rotation setting.

```cpp
display.setRotation(2);  // upside-down
display.setCursor(0, 0);
display.println("Upside down!");
display.display();
```

---

## Memory Usage

The frame buffer is allocated dynamically on the **heap** by `begin()`.

| Resolution | Buffer Size | Typical total RAM (incl. object) |
|------------|-------------|----------------------------------|
| 128 × 64   | 1024 bytes  | ~1040 bytes |
| 128 × 32   | 512 bytes   | ~530 bytes |
| 96 × 16    | 192 bytes   | ~210 bytes |
| 64 × 48    | 384 bytes   | ~400 bytes |
| 64 × 32    | 256 bytes   | ~270 bytes |

> **AVR note:** The ATmega328P (Arduino Uno/Nano) has only 2 KB of RAM. A 128×64 display uses half of it for the buffer alone. Keep your sketch's RAM usage minimal, use `F()` for string literals, and avoid large local variables.

---

## Performance Notes

### Optimised Drawing

The library overrides four `Adafruit_GFX` methods with direct buffer-access implementations:

| Method | Optimisation |
|--------|-------------|
| `drawPixel()` | Direct byte/bit manipulation (no function-call overhead per pixel). |
| `drawFastHLine()` | Single bitmask applied across a byte run. |
| `drawFastVLine()` | Processes 8 rows at a time using full-byte writes for aligned sections. |
| `fillRect()` | Page-by-page bitmask writes — rotation and clipping done once, not per-row. |

### I2C Clock Boosting

On Arduino ≥ 1.5.7, the I2C clock is temporarily raised to `clkDuring` (default 400 kHz) for display transfers, then restored to `clkAfter` (default 100 kHz). This halves the transfer time for `display()` compared to the default 100 kHz clock.

### Batched I2C Writes

The `display()` method fills each I2C transaction up to the platform's maximum buffer size (auto-detected per platform), minimising the overhead of start/stop conditions.

### PROGMEM Tables

Look-up tables for partial-byte masks in `drawFastVLine` are stored in PROGMEM to save RAM on AVR.

---

## Migrating from Adafruit_SSD1306

1. Replace the include:
   ```cpp
   // Before
   #include <Adafruit_SSD1306.h>

   // After
   #include <DIYables_OLED_SSD1309.h>
   ```

2. Replace the class name:
   ```cpp
   // Before
   Adafruit_SSD1306 display(128, 64, &Wire, -1);

   // After
   DIYables_OLED_SSD1309 display(128, 64, &Wire, -1);
   ```

3. Replace constant prefixes (optional — the old names still work):
   ```cpp
   // Before                      // After (recommended)
   SSD1306_SWITCHCAPVCC      →   SSD1309_SWITCHCAPVCC
   SSD1306_WHITE             →   SSD1309_PIXEL_ON
   SSD1306_BLACK             →   SSD1309_PIXEL_OFF
   SSD1306_INVERSE           →   SSD1309_PIXEL_INVERSE
   ```

4. Done — the API is identical.

---

## SSD1309 vs SSD1306

| Feature | SSD1306 | SSD1309 |
|---------|---------|---------|
| Internal charge pump | Yes | No (requires external VCC) |
| Max VCC | 4.2 V | 16 V |
| Display size support | Up to 128×64 | Up to 128×64 |
| Register compatibility | — | Largely compatible with SSD1306 |
| Charge pump command | Required | Ignored (sent for compat) |

Most SSD1309 breakout modules include an onboard voltage regulator, so the difference is transparent to the user. This library sends the charge pump command for maximum hardware compatibility.

---

## Troubleshooting

### Display shows nothing after `begin()`

- **Forgot `display.display()`** — drawing to the buffer does not update the screen. Call `display()` after drawing.
- **Wrong I2C address** — try `0x3D` instead of `0x3C`, or run an I2C scanner sketch.
- **Wiring** — double-check SDA/SCL connections. Some modules need pull-up resistors.
- **Power** — ensure VCC is within the module's rated range (typically 3.3–5 V).

### `begin()` returns `false`

- **Out of memory** — the frame buffer allocation failed. On AVR, a 128×64 display needs 1 KB of RAM. Reduce RAM usage elsewhere or use a smaller display.

### Display content is garbled or shifted

- **Wrong resolution** — make sure the width/height in the constructor matches your physical display.
- **Wrong COM pins** — if you have an unusual panel, you may need to override the COM pins configuration via `ssd1309_command()`:
  ```cpp
  display.ssd1309_command(SSD1309_SETCOMPINS);
  display.ssd1309_command(0x12);  // or 0x02, depending on panel
  ```

### Scrolling corrupts the display

- Always call `display()` **before** starting a scroll.
- Call `stopscroll()` before drawing new content.
- Drawing while scrolling produces undefined results (this is a hardware limitation).

### I2C errors or hangs

- **Clock too fast** — some modules or long wires don't support 400 kHz. Lower `clkDuring` in the constructor:
  ```cpp
  DIYables_OLED_SSD1309 display(128, 64, &Wire, -1, 100000UL, 100000UL);
  ```
- **Bus contention** — if sharing the I2C bus with other devices, ensure addresses don't conflict and pull-ups are adequate.
- **ESP32 custom pins** — call `Wire.begin(SDA, SCL)` before `display.begin()` and pass `periphBegin = false`.

### Flickering

- Minimise the number of `display()` calls. Buffer all changes, then call `display()` once per frame.
- Avoid calling `clearDisplay()` + `display()` before re-drawing — it causes a visible blank frame. Instead, clear and re-draw into the buffer, then call `display()` once.

---

## Examples

| Example | Description | Key Concepts |
|---------|-------------|--------------|
| [HelloWorld](../examples/HelloWorld/HelloWorld.ino) | Basic text output on the OLED. | `begin()`, `setTextSize()`, `setTextColor()`, `setCursor()`, `print()`, `display()` |
| [DrawShapes](../examples/DrawShapes/DrawShapes.ino) | Animated demo of all shape primitives. | `drawPixel()`, `drawLine()`, `drawRect()`, `fillRect()`, `drawCircle()`, `fillCircle()`, `drawTriangle()`, `fillTriangle()`, `drawRoundRect()`, `fillRoundRect()` |
| [ScrollText](../examples/ScrollText/ScrollText.ino) | Hardware scrolling in all four directions. | `startscrollright()`, `startscrollleft()`, `startscrolldiagright()`, `startscrolldiagleft()`, `stopscroll()` |
| [Bitmap](../examples/Bitmap/Bitmap.ino) | Drawing PROGMEM bitmaps with alternating images. | `drawBitmap()`, PROGMEM arrays |
| [ContrastDim](../examples/ContrastDim/ContrastDim.ino) | Contrast sweep and dim/restore toggle. | `setContrast()`, `dim()` |
| [ExternalFont](../examples/ExternalFont/ExternalFont.ino) | Using FreeFont external fonts. | `setFont()`, baseline cursor positioning |

---

*DIYables — [diyables.io](https://diyables.io)*
