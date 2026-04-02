/*!
 * @file DIYables_OLED_SSD1309.cpp
 *
 * @mainpage Arduino library for monochrome OLEDs based on SSD1309 drivers.
 *
 * @section intro_sec Introduction
 *
 * This is the DIYables library for SSD1309-based monochrome OLED displays
 * using the I2C interface. It extends Adafruit_GFX for full graphics
 * support and follows the Adafruit_SSD1306 API for easy migration.
 *
 * @section dependencies Dependencies
 *
 * This library depends on
 * <a href="https://github.com/adafruit/Adafruit-GFX-Library">Adafruit_GFX</a>
 * being present on your system.
 *
 * @section author Author
 *
 * Written by DIYables.
 *
 * @section license License
 *
 * MIT license, all text above must be included in any redistribution.
 */

// ---- Platform-specific PROGMEM handling ----------------------------------
#if defined(__AVR__) || defined(ARDUINO_ARCH_RTTHREAD)
#include <avr/pgmspace.h>
#elif defined(ARDUINO_ARDUINO_NANO33BLE) || \
    defined(ARDUINO_ARCH_MBED_RP2040) || defined(ARDUINO_ARCH_RP2040)
#include <api/deprecated-avr-comp/avr/pgmspace.h>
#elif defined(ESP8266) || defined(ESP32)
#include <pgmspace.h>
#else
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#ifndef PROGMEM
#define PROGMEM
#endif
#endif

#include "DIYables_OLED_SSD1309.h"
#include <Adafruit_GFX.h>

// ---- I2C buffer size detection ------------------------------------------
// Different platforms expose different buffer-length macros.
// We pick the smallest safe value so that transmissions always fit.
#if defined(I2C_BUFFER_LENGTH)
#define SSD1309_WIRE_MAX min(256, I2C_BUFFER_LENGTH)
#elif defined(BUFFER_LENGTH)
#define SSD1309_WIRE_MAX min(256, BUFFER_LENGTH)
#elif defined(SERIAL_BUFFER_SIZE)
#define SSD1309_WIRE_MAX min(255, (SERIAL_BUFFER_SIZE - 1))
#else
#define SSD1309_WIRE_MAX 32 ///< Safe default for most Arduino cores
#endif

// ---- Wire write compatibility -------------------------------------------
#if ARDUINO >= 100
#define WIRE_WRITE wire->write
#else
#define WIRE_WRITE wire->send
#endif

// ---- I2C clock helpers --------------------------------------------------
#if (ARDUINO >= 157) && !defined(ARDUINO_STM32_FEATHER)
#define SETWIRECLOCK wire->setClock(wireClk)
#define RESWIRECLOCK wire->setClock(restoreClk)
#else
#define SETWIRECLOCK ///< Not available before Arduino 1.5.7
#define RESWIRECLOCK
#endif

// ---- Utility macro ------------------------------------------------------
#define ssd1309_swap(a, b) \
  (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

// =========================================================================
//  CONSTRUCTOR / DESTRUCTOR
// =========================================================================

/*!
 * @brief  Constructor for an I2C-interfaced SSD1309 display.
 * @param  w          Display width in pixels.
 * @param  h          Display height in pixels.
 * @param  twi        Pointer to a TwoWire instance (default &Wire).
 * @param  rst_pin    Reset pin, or -1 if not wired.
 * @param  clkDuring  I2C clock (Hz) used during transfers (default 400 kHz).
 * @param  clkAfter   I2C clock (Hz) restored after transfers (default 100 kHz).
 */
DIYables_OLED_SSD1309::DIYables_OLED_SSD1309(uint8_t w, uint8_t h,
                                               TwoWire *twi, int8_t rst_pin,
                                               uint32_t clkDuring,
                                               uint32_t clkAfter)
    : Adafruit_GFX(w, h), wire(twi ? twi : &Wire), buffer(NULL),
      i2caddr(0), vccstate(0), rstPin(rst_pin), _contrast(0x7F)
#if ARDUINO >= 157
      ,
      wireClk(clkDuring), restoreClk(clkAfter)
#endif
{
  (void)clkDuring; // Suppress unused-parameter warnings on old cores
  (void)clkAfter;
}

/*!
 * @brief  Destructor – frees the display buffer.
 */
DIYables_OLED_SSD1309::~DIYables_OLED_SSD1309(void) {
  if (buffer) {
    free(buffer);
    buffer = NULL;
  }
}

// =========================================================================
//  LOW-LEVEL I2C HELPERS
// =========================================================================

/*!
 * @brief  Send a single command byte over I2C.
 * @param  c  The command byte.
 */
void DIYables_OLED_SSD1309::ssd1309_command1(uint8_t c) {
  wire->beginTransmission(i2caddr);
  WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C# = 0 → command
  WIRE_WRITE(c);
  wire->endTransmission();
}

/*!
 * @brief  Send a list of command bytes stored in PROGMEM.
 * @param  c  Pointer to PROGMEM byte array.
 * @param  n  Number of bytes to send.
 */
void DIYables_OLED_SSD1309::ssd1309_commandList(const uint8_t *c, uint8_t n) {
  wire->beginTransmission(i2caddr);
  WIRE_WRITE((uint8_t)0x00); // Co = 0, D/C# = 0
  uint16_t bytesOut = 1;
  while (n--) {
    if (bytesOut >= SSD1309_WIRE_MAX) {
      wire->endTransmission();
      wire->beginTransmission(i2caddr);
      WIRE_WRITE((uint8_t)0x00);
      bytesOut = 1;
    }
    WIRE_WRITE(pgm_read_byte(c++));
    bytesOut++;
  }
  wire->endTransmission();
}

/*!
 * @brief  Public wrapper – issue a single command with clock management.
 * @param  c  The command byte.
 */
void DIYables_OLED_SSD1309::ssd1309_command(uint8_t c) {
  SETWIRECLOCK;
  ssd1309_command1(c);
  RESWIRECLOCK;
}

// =========================================================================
//  INITIALISATION
// =========================================================================

/*!
 * @brief  Allocate the frame buffer, reset the display (optional) and
 *         run the SSD1309 initialisation sequence.
 * @param  switchvcc   SSD1309_SWITCHCAPVCC or SSD1309_EXTERNALVCC.
 * @param  addr        7-bit I2C address (default 0x3C).
 * @param  reset       true to pulse the reset pin.
 * @param  periphBegin true to call Wire.begin() inside this function.
 * @return true on success.
 */
bool DIYables_OLED_SSD1309::begin(uint8_t switchvcc, uint8_t addr,
                                   bool reset, bool periphBegin) {
  // Allocate frame buffer: 1 bit per pixel, packed into bytes (8 rows/byte).
  if ((!buffer) &&
      !(buffer = (uint8_t *)malloc(WIDTH * ((HEIGHT + 7) / 8))))
    return false;

  clearDisplay();

  vccstate = switchvcc;
  i2caddr  = addr;

  // Start the I2C peripheral if requested
  if (periphBegin)
    wire->begin();

  // Hardware reset (if reset pin was provided)
  if (reset && (rstPin >= 0)) {
    pinMode(rstPin, OUTPUT);
    digitalWrite(rstPin, HIGH);
    delay(1);
    digitalWrite(rstPin, LOW);
    delay(10);
    digitalWrite(rstPin, HIGH);
    delay(10);
  }

  SETWIRECLOCK;

  // ---- SSD1309 Init Sequence --------------------------------------------
  // Step 1: Display OFF, clock, multiplex
  static const uint8_t PROGMEM init1[] = {
      SSD1309_DISPLAYOFF,          // 0xAE
      SSD1309_SETDISPLAYCLOCKDIV,  // 0xD5
      0x80,                        // Suggested ratio
      SSD1309_SETMULTIPLEX         // 0xA8
  };
  ssd1309_commandList(init1, sizeof(init1));
  ssd1309_command1(HEIGHT - 1);

  // Step 2: Offset, start line, charge pump (for compatibility)
  static const uint8_t PROGMEM init2[] = {
      SSD1309_SETDISPLAYOFFSET,    // 0xD3
      0x00,                        // No offset
      SSD1309_SETSTARTLINE | 0x00, // Start line #0
      SSD1309_CHARGEPUMP           // 0x8D – ignored by true SSD1309
  };
  ssd1309_commandList(init2, sizeof(init2));
  ssd1309_command1((vccstate == SSD1309_EXTERNALVCC) ? 0x10 : 0x14);

  // Step 3: Addressing mode, segment remap, COM scan direction
  static const uint8_t PROGMEM init3[] = {
      SSD1309_MEMORYMODE,          // 0x20
      0x00,                        // Horizontal addressing mode
      SSD1309_SEGREMAP | 0x01,     // Column 127 mapped to SEG0
      SSD1309_COMSCANDEC           // 0xC8 – scan from COM[N-1] to COM0
  };
  ssd1309_commandList(init3, sizeof(init3));

  // Step 4: COM pins & contrast – dependent on resolution
  uint8_t comPins  = 0x12;
  _contrast = 0xCF;

  if ((WIDTH == 128) && (HEIGHT == 64)) {
    comPins   = 0x12;
    _contrast = (vccstate == SSD1309_EXTERNALVCC) ? 0x9F : 0xCF;
  } else if ((WIDTH == 128) && (HEIGHT == 32)) {
    comPins   = 0x02;
    _contrast = 0x8F;
  } else if ((WIDTH == 96) && (HEIGHT == 16)) {
    comPins   = 0x02;
    _contrast = (vccstate == SSD1309_EXTERNALVCC) ? 0x10 : 0xAF;
  } else if ((WIDTH == 64) && (HEIGHT == 48)) {
    comPins   = 0x12;
    _contrast = (vccstate == SSD1309_EXTERNALVCC) ? 0x9F : 0xCF;
  } else if ((WIDTH == 64) && (HEIGHT == 32)) {
    comPins   = 0x12;
    _contrast = (vccstate == SSD1309_EXTERNALVCC) ? 0x10 : 0xCF;
  }

  ssd1309_command1(SSD1309_SETCOMPINS);
  ssd1309_command1(comPins);
  ssd1309_command1(SSD1309_SETCONTRAST);
  ssd1309_command1(_contrast);

  // Step 5: Pre-charge, VCOMH, final commands, display ON
  ssd1309_command1(SSD1309_SETPRECHARGE);
  ssd1309_command1((vccstate == SSD1309_EXTERNALVCC) ? 0x22 : 0xF1);

  static const uint8_t PROGMEM init5[] = {
      SSD1309_SETVCOMDETECT,       // 0xDB
      0x40,                        // VCOMH deselect level
      SSD1309_DISPLAYALLON_RESUME, // 0xA4 – output follows RAM
      SSD1309_NORMALDISPLAY,       // 0xA6
      SSD1309_DEACTIVATE_SCROLL,
      SSD1309_DISPLAYON            // 0xAF
  };
  ssd1309_commandList(init5, sizeof(init5));

  RESWIRECLOCK;

  return true;
}

// =========================================================================
//  DRAWING PRIMITIVES
// =========================================================================

/*!
 * @brief  Set, clear, or invert a single pixel in the buffer.
 */
void DIYables_OLED_SSD1309::drawPixel(int16_t x, int16_t y, uint16_t color) {
  if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
    switch (getRotation()) {
    case 1:
      ssd1309_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      break;
    case 3:
      ssd1309_swap(x, y);
      y = HEIGHT - y - 1;
      break;
    }
    switch (color) {
    case SSD1309_PIXEL_ON:
      buffer[x + (y / 8) * WIDTH] |= (1 << (y & 7));
      break;
    case SSD1309_PIXEL_OFF:
      buffer[x + (y / 8) * WIDTH] &= ~(1 << (y & 7));
      break;
    case SSD1309_PIXEL_INVERSE:
      buffer[x + (y / 8) * WIDTH] ^= (1 << (y & 7));
      break;
    }
  }
}

/*!
 * @brief  Clear the entire frame buffer (all pixels off).
 */
void DIYables_OLED_SSD1309::clearDisplay(void) {
  memset(buffer, 0, WIDTH * ((HEIGHT + 7) / 8));
}

// ---- Optimised filled rectangle (direct buffer access) ----

/*!
 * @brief  Fill a rectangle directly in the buffer.
 *
 * The default Adafruit_GFX fillRect calls drawFastHLine in a loop,
 * repeating rotation, clipping and pointer setup for every single row.
 * This override does all of that once, then writes whole byte-columns
 * (8 rows at a time) across the full width — significantly faster for
 * large filled areas, fillScreen, and clear-with-colour operations.
 */
void DIYables_OLED_SSD1309::fillRect(int16_t x, int16_t y, int16_t w,
                                      int16_t h, uint16_t color) {
  // ---- Handle rotation (map to raw coordinates) ----
  switch (getRotation()) {
  case 1: {
    int16_t t = x;
    x = WIDTH - y - h;
    y = t;
    int16_t tmp = w;
    w = h;
    h = tmp;
  } break;
  case 2:
    x = WIDTH  - x - w;
    y = HEIGHT - y - h;
    break;
  case 3: {
    int16_t t = x;
    x = y;
    y = HEIGHT - t - w;
    int16_t tmp = w;
    w = h;
    h = tmp;
  } break;
  }

  // ---- Clip to display bounds ----
  if (x < 0) { w += x; x = 0; }
  if (y < 0) { h += y; y = 0; }
  if ((x + w) > WIDTH)  w = WIDTH  - x;
  if ((y + h) > HEIGHT) h = HEIGHT - y;
  if (w <= 0 || h <= 0) return;

  // ---- Write directly into the page-organised buffer ----
  // Buffer layout: each byte = 8 vertical pixels in one column.
  // Byte at buffer[(page * WIDTH) + col], bit 0 = topmost pixel of page.

  int16_t yEnd = y + h;           // one past the last row
  int16_t startPage = y / 8;
  int16_t endPage   = (yEnd - 1) / 8;

  for (int16_t page = startPage; page <= endPage; page++) {
    // Build a bitmask for the rows within this page that are inside the rect
    uint8_t mask = 0xFF;
    if (page == startPage) {
      mask &= (0xFF << (y & 7));        // clear bits above the rect
    }
    if (page == endPage) {
      mask &= (0xFF >> (7 - ((yEnd - 1) & 7))); // clear bits below the rect
    }

    uint8_t *pBuf = &buffer[page * WIDTH + x];

    switch (color) {
    case SSD1309_PIXEL_ON:
      for (int16_t col = 0; col < w; col++) pBuf[col] |= mask;
      break;
    case SSD1309_PIXEL_OFF: {
      uint8_t invMask = ~mask;
      for (int16_t col = 0; col < w; col++) pBuf[col] &= invMask;
    } break;
    case SSD1309_PIXEL_INVERSE:
      for (int16_t col = 0; col < w; col++) pBuf[col] ^= mask;
      break;
    }
  }
}

// ---- Fast horizontal line (rotation-aware) ----

void DIYables_OLED_SSD1309::drawFastHLine(int16_t x, int16_t y, int16_t w,
                                           uint16_t color) {
  bool bSwap = false;
  switch (rotation) {
  case 1:
    bSwap = true;
    ssd1309_swap(x, y);
    x = WIDTH - x - 1;
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    x -= (w - 1);
    break;
  case 3:
    bSwap = true;
    ssd1309_swap(x, y);
    y = HEIGHT - y - 1;
    y -= (w - 1);
    break;
  }
  if (bSwap)
    drawFastVLineInternal(x, y, w, color);
  else
    drawFastHLineInternal(x, y, w, color);
}

void DIYables_OLED_SSD1309::drawFastHLineInternal(int16_t x, int16_t y,
                                                    int16_t w,
                                                    uint16_t color) {
  if ((y >= 0) && (y < HEIGHT)) {
    if (x < 0) {
      w += x;
      x = 0;
    }
    if ((x + w) > WIDTH) {
      w = (WIDTH - x);
    }
    if (w > 0) {
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x];
      uint8_t mask = 1 << (y & 7);
      switch (color) {
      case SSD1309_PIXEL_ON:
        while (w--) { *pBuf++ |= mask; }
        break;
      case SSD1309_PIXEL_OFF:
        mask = ~mask;
        while (w--) { *pBuf++ &= mask; }
        break;
      case SSD1309_PIXEL_INVERSE:
        while (w--) { *pBuf++ ^= mask; }
        break;
      }
    }
  }
}

// ---- Fast vertical line (rotation-aware) ----

void DIYables_OLED_SSD1309::drawFastVLine(int16_t x, int16_t y, int16_t h,
                                           uint16_t color) {
  bool bSwap = false;
  switch (rotation) {
  case 1:
    bSwap = true;
    ssd1309_swap(x, y);
    x = WIDTH - x - 1;
    x -= (h - 1);
    break;
  case 2:
    x = WIDTH - x - 1;
    y = HEIGHT - y - 1;
    y -= (h - 1);
    break;
  case 3:
    bSwap = true;
    ssd1309_swap(x, y);
    y = HEIGHT - y - 1;
    break;
  }
  if (bSwap)
    drawFastHLineInternal(x, y, h, color);
  else
    drawFastVLineInternal(x, y, h, color);
}

void DIYables_OLED_SSD1309::drawFastVLineInternal(int16_t x, int16_t __y,
                                                    int16_t __h,
                                                    uint16_t color) {
  if ((x >= 0) && (x < WIDTH)) {
    if (__y < 0) {
      __h += __y;
      __y = 0;
    }
    if ((__y + __h) > HEIGHT) {
      __h = (HEIGHT - __y);
    }
    if (__h > 0) {
      uint8_t y = __y, h = __h;
      uint8_t *pBuf = &buffer[(y / 8) * WIDTH + x];

      // First partial byte
      uint8_t mod = (y & 7);
      if (mod) {
        mod = 8 - mod;
        static const uint8_t PROGMEM premask[8] = {
            0x00, 0x80, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC, 0xFE};
        uint8_t mask = pgm_read_byte(&premask[mod]);
        if (h < mod)
          mask &= (0xFF >> (mod - h));
        switch (color) {
        case SSD1309_PIXEL_ON:
          *pBuf |= mask;
          break;
        case SSD1309_PIXEL_OFF:
          *pBuf &= ~mask;
          break;
        case SSD1309_PIXEL_INVERSE:
          *pBuf ^= mask;
          break;
        }
        pBuf += WIDTH;
      }

      if (h >= mod) {
        h -= mod;
        // Full bytes (8 rows at a time)
        if (h >= 8) {
          if (color == SSD1309_PIXEL_INVERSE) {
            do {
              *pBuf ^= 0xFF;
              pBuf += WIDTH;
              h -= 8;
            } while (h >= 8);
          } else {
            uint8_t val = (color != SSD1309_PIXEL_OFF) ? 0xFF : 0x00;
            do {
              *pBuf = val;
              pBuf += WIDTH;
              h -= 8;
            } while (h >= 8);
          }
        }

        // Final partial byte
        if (h) {
          mod = h & 7;
          static const uint8_t PROGMEM postmask[8] = {
              0x00, 0x01, 0x03, 0x07, 0x0F, 0x1F, 0x3F, 0x7F};
          uint8_t mask = pgm_read_byte(&postmask[mod]);
          switch (color) {
          case SSD1309_PIXEL_ON:
            *pBuf |= mask;
            break;
          case SSD1309_PIXEL_OFF:
            *pBuf &= ~mask;
            break;
          case SSD1309_PIXEL_INVERSE:
            *pBuf ^= mask;
            break;
          }
        }
      }
    }
  }
}

// ---- Pixel query helpers ----

/*!
 * @brief  Read back a single pixel from the buffer.
 */
bool DIYables_OLED_SSD1309::getPixel(int16_t x, int16_t y) {
  if ((x >= 0) && (x < width()) && (y >= 0) && (y < height())) {
    switch (getRotation()) {
    case 1:
      ssd1309_swap(x, y);
      x = WIDTH - x - 1;
      break;
    case 2:
      x = WIDTH - x - 1;
      y = HEIGHT - y - 1;
      break;
    case 3:
      ssd1309_swap(x, y);
      y = HEIGHT - y - 1;
      break;
    }
    return (buffer[x + (y / 8) * WIDTH] & (1 << (y & 7)));
  }
  return false;
}

/*!
 * @brief  Return a pointer to the display buffer.
 */
uint8_t *DIYables_OLED_SSD1309::getBuffer(void) {
  return buffer;
}

// =========================================================================
//  DISPLAY REFRESH
// =========================================================================

/*!
 * @brief  Transfer the entire frame buffer to the SSD1309 over I2C.
 */
void DIYables_OLED_SSD1309::display(void) {
  SETWIRECLOCK;

  // Set column and page address window to cover the full display
  static const uint8_t PROGMEM dlist1[] = {
      SSD1309_PAGEADDR,
      0,                 // Page start
      0xFF,              // Page end
      SSD1309_COLUMNADDR // Column start follows
  };
  ssd1309_commandList(dlist1, sizeof(dlist1));

  // Handle column offset for narrower displays centred on the SSD1309
  if (WIDTH == 64) {
    ssd1309_command1(0x20);             // Column start = 32
    ssd1309_command1(0x20 + WIDTH - 1); // Column end
  } else {
    ssd1309_command1(0);                // Column start = 0
    ssd1309_command1(WIDTH - 1);        // Column end
  }

#if defined(ESP8266)
  yield();
#endif

  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr   = buffer;

  wire->beginTransmission(i2caddr);
  WIRE_WRITE((uint8_t)0x40); // Co = 0, D/C# = 1 → data
  uint16_t bytesOut = 1;

  while (count--) {
    if (bytesOut >= SSD1309_WIRE_MAX) {
      wire->endTransmission();
      wire->beginTransmission(i2caddr);
      WIRE_WRITE((uint8_t)0x40);
      bytesOut = 1;
    }
    WIRE_WRITE(*ptr++);
    bytesOut++;
  }
  wire->endTransmission();

  RESWIRECLOCK;

#if defined(ESP8266)
  yield();
#endif
}

// =========================================================================
//  HARDWARE DISPLAY SETTINGS
// =========================================================================

/*!
 * @brief  Enable or disable display inversion (hardware).
 * @param  i  true = inverted, false = normal.
 */
void DIYables_OLED_SSD1309::invertDisplay(bool i) {
  SETWIRECLOCK;
  ssd1309_command1(i ? SSD1309_INVERTDISPLAY : SSD1309_NORMALDISPLAY);
  RESWIRECLOCK;
}

/*!
 * @brief  Dim the display by setting contrast to 0, or restore.
 * @param  dim  true = dim (contrast 0), false = normal contrast.
 */
void DIYables_OLED_SSD1309::dim(bool dim) {
  SETWIRECLOCK;
  ssd1309_command1(SSD1309_SETCONTRAST);
  ssd1309_command1(dim ? 0 : _contrast);
  RESWIRECLOCK;
}

/*!
 * @brief  Set the display contrast.
 * @param  contrast  Value 0-255.
 * @return true (always succeeds).
 */
bool DIYables_OLED_SSD1309::setContrast(uint8_t contrast) {
  SETWIRECLOCK;
  _contrast = contrast;
  ssd1309_command1(SSD1309_SETCONTRAST);
  ssd1309_command1(_contrast);
  RESWIRECLOCK;
  return true;
}
