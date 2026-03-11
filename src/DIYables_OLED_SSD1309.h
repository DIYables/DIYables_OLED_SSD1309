/*!
 * @file DIYables_OLED_SSD1309.h
 *
 * Arduino library for monochrome OLED displays based on SSD1309 drivers,
 * using I2C interface.
 *
 * These displays use I2C to communicate, requiring 2 pins (SCL + SDA)
 * and optionally a RESET pin.
 *
 * This library extends Adafruit_GFX for full graphics support and
 * follows the Adafruit_SSD1306 API for easy migration.
 *
 * Written by DIYables.
 *
 * MIT license, all text above must be included in any redistribution.
 */

#ifndef DIYABLES_OLED_SSD1309_H
#define DIYABLES_OLED_SSD1309_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>

// --------------------------------------------------------------------------
// Pixel operation constants
// --------------------------------------------------------------------------
// Primary names – describe the operation, not a colour.
// Works regardless of panel colour (white, blue, yellow, etc.).
#define SSD1309_PIXEL_OFF     0 ///< Turn pixel off
#define SSD1309_PIXEL_ON      1 ///< Turn pixel on
#define SSD1309_PIXEL_INVERSE 2 ///< Toggle pixel state

// Adafruit_SSD1306-compatible aliases (for easy migration)
#define SSD1309_BLACK   SSD1309_PIXEL_OFF     ///< Alias: pixel off
#define SSD1309_WHITE   SSD1309_PIXEL_ON      ///< Alias: pixel on
#define SSD1309_INVERSE SSD1309_PIXEL_INVERSE ///< Alias: pixel toggle

// Unscoped short-hand aliases
#ifndef NO_DIYABLES_SSD1309_COLOR_COMPATIBILITY
#define BLACK   SSD1309_PIXEL_OFF
#define WHITE   SSD1309_PIXEL_ON
#define INVERSE SSD1309_PIXEL_INVERSE
#endif

// --------------------------------------------------------------------------
// VCC selection (passed to begin())
// --------------------------------------------------------------------------
#define SSD1309_EXTERNALVCC  0x01 ///< External display voltage source
#define SSD1309_SWITCHCAPVCC 0x02 ///< Generate display voltage from 3.3V

// --------------------------------------------------------------------------
// SSD1309 fundamental command defines
// --------------------------------------------------------------------------
#define SSD1309_MEMORYMODE          0x20 ///< Set memory addressing mode (0x20)
#define SSD1309_COLUMNADDR          0x21 ///< Set column start/end address
#define SSD1309_PAGEADDR            0x22 ///< Set page start/end address
#define SSD1309_SETCONTRAST         0x81 ///< Set contrast control
#define SSD1309_CHARGEPUMP          0x8D ///< Charge pump setting (compat)
#define SSD1309_SEGREMAP            0xA0 ///< Set segment re-map
#define SSD1309_DISPLAYALLON_RESUME 0xA4 ///< Resume display from RAM
#define SSD1309_DISPLAYALLON        0xA5 ///< Entire display ON (ignore RAM)
#define SSD1309_NORMALDISPLAY       0xA6 ///< Normal display
#define SSD1309_INVERTDISPLAY       0xA7 ///< Inverse display
#define SSD1309_SETMULTIPLEX        0xA8 ///< Set multiplex ratio
#define SSD1309_DISPLAYOFF          0xAE ///< Display OFF (sleep mode)
#define SSD1309_DISPLAYON           0xAF ///< Display ON (normal mode)
#define SSD1309_COMSCANINC          0xC0 ///< COM output scan direction: inc
#define SSD1309_COMSCANDEC          0xC8 ///< COM output scan direction: dec
#define SSD1309_SETDISPLAYOFFSET    0xD3 ///< Set display offset
#define SSD1309_SETDISPLAYCLOCKDIV  0xD5 ///< Set display clock divide ratio
#define SSD1309_SETPRECHARGE        0xD9 ///< Set pre-charge period
#define SSD1309_SETCOMPINS          0xDA ///< Set COM pins hardware config
#define SSD1309_SETVCOMDETECT       0xDB ///< Set VCOMH deselect level
#define SSD1309_SETSTARTLINE        0x40 ///< Set display start line (0x40-0x7F)

// --------------------------------------------------------------------------
// Scrolling command defines
// --------------------------------------------------------------------------
#define SSD1309_RIGHT_HORIZONTAL_SCROLL              0x26 ///< Right scroll
#define SSD1309_LEFT_HORIZONTAL_SCROLL               0x27 ///< Left scroll
#define SSD1309_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 0x29 ///< Diag right
#define SSD1309_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL  0x2A ///< Diag left
#define SSD1309_DEACTIVATE_SCROLL                    0x2E ///< Stop scroll
#define SSD1309_ACTIVATE_SCROLL                      0x2F ///< Start scroll
#define SSD1309_SET_VERTICAL_SCROLL_AREA             0xA3 ///< Scroll range

/*!
 * @brief Class for driving SSD1309-based OLED displays over I2C.
 *
 * Extends Adafruit_GFX so all GFX primitives (text, shapes, bitmaps, etc.)
 * are available.  The API deliberately mirrors Adafruit_SSD1306 to make
 * migrating existing sketches straightforward.
 */
class DIYables_OLED_SSD1309 : public Adafruit_GFX {
public:
  // --- Constructor / Destructor -------------------------------------------

  /*!
   * @brief Constructor for I2C-interfaced SSD1309 display.
   * @param w         Display width in pixels (e.g. 128).
   * @param h         Display height in pixels (e.g. 64 or 32).
   * @param twi       Pointer to TwoWire instance (default &Wire).
   * @param rst_pin   Reset pin (-1 if not used).
   * @param clkDuring I2C clock speed during library transfers (default 400 kHz).
   * @param clkAfter  I2C clock speed restored after transfers (default 100 kHz).
   */
  DIYables_OLED_SSD1309(uint8_t w, uint8_t h, TwoWire *twi = &Wire,
                         int8_t rst_pin = -1,
                         uint32_t clkDuring = 400000UL,
                         uint32_t clkAfter = 100000UL);

  ~DIYables_OLED_SSD1309(void);

  // --- Initialization -----------------------------------------------------

  /*!
   * @brief Allocate buffer, initialise I2C and configure the display.
   * @param switchvcc  SSD1309_SWITCHCAPVCC or SSD1309_EXTERNALVCC.
   * @param i2caddr    I2C address (default 0x3C).
   * @param reset      If true, perform a hardware reset (if rst_pin >= 0).
   * @param periphBegin If true, call Wire.begin() inside this function.
   * @return true on success, false if buffer allocation failed.
   */
  bool begin(uint8_t switchvcc = SSD1309_SWITCHCAPVCC,
             uint8_t i2caddr = 0x3C, bool reset = true,
             bool periphBegin = true);

  // --- Display control ----------------------------------------------------

  void display(void);          ///< Push RAM buffer to the display.
  void clearDisplay(void);     ///< Clear the RAM buffer (all pixels off).
  void invertDisplay(bool i);  ///< Invert display colours (hardware).
  void dim(bool dim);          ///< Dim display (low contrast) or restore.

  // --- Drawing (Adafruit_GFX overrides) -----------------------------------

  void drawPixel(int16_t x, int16_t y, uint16_t color) override;
  void drawFastHLine(int16_t x, int16_t y, int16_t w,
                     uint16_t color) override;
  void drawFastVLine(int16_t x, int16_t y, int16_t h,
                     uint16_t color) override;
  void fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
                uint16_t color) override;

  // --- Pixel query --------------------------------------------------------

  bool     getPixel(int16_t x, int16_t y);
  uint8_t *getBuffer(void);

  // --- Scrolling ----------------------------------------------------------

  void startscrollright(uint8_t start, uint8_t stop);
  void startscrollleft(uint8_t start, uint8_t stop);
  void startscrolldiagright(uint8_t start, uint8_t stop);
  void startscrolldiagleft(uint8_t start, uint8_t stop);
  void stopscroll(void);

  // --- Low-level command --------------------------------------------------

  void ssd1309_command(uint8_t c);
  bool setContrast(uint8_t contrast);

private:
  // Internal helpers
  void    ssd1309_command1(uint8_t c);
  void    ssd1309_commandList(const uint8_t *c, uint8_t n);
  void    drawFastHLineInternal(int16_t x, int16_t y, int16_t w,
                                uint16_t color);
  void    drawFastVLineInternal(int16_t x, int16_t y, int16_t h,
                                uint16_t color);

  TwoWire  *wire;         ///< Pointer to I2C bus instance.
  uint8_t  *buffer;       ///< Display RAM buffer.
  int8_t    i2caddr;      ///< 7-bit I2C address.
  int8_t    vccstate;     ///< SSD1309_SWITCHCAPVCC or SSD1309_EXTERNALVCC.
  int8_t    rstPin;       ///< Hardware reset pin (-1 = unused).
  uint8_t   _contrast;    ///< Current contrast value.
#if ARDUINO >= 157
  uint32_t  wireClk;      ///< I2C speed during transfers.
  uint32_t  restoreClk;   ///< I2C speed restored after transfers.
#endif
};

#endif // DIYABLES_OLED_SSD1309_H
