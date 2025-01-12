#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <SimpleQR.h>

#define TFT_CS         15
#define TFT_RST        4
#define TFT_DC         2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);
SimpleQR qr(&tft);

void setup() {
    // Initialize the TFT display
    tft.initR(INITR_BLACKTAB);  
    tft.setRotation(1);  

    // Initialize the QR library
    qr.init();
    qr.setScale(6);  // Set scaling factor for QR code size
    qr.setColors(ST77XX_BLACK, ST77XX_WHITE);  // Set foreground and background colors

    // Generate the QR code with a test string
    qr.create("TEST123");
}

void loop() {
    // Nothing to do in the loop
}
