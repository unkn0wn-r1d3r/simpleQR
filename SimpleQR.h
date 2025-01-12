#ifndef SIMPLE_QR_H
#define SIMPLE_QR_H

#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class SimpleQR {
private:
    static const uint8_t QR_SIZE = 21;  // Version 1 QR code is 21x21
    static const uint8_t MAX_LENGTH = 15;
    
    Adafruit_ST7735* tft;
    bool matrix[QR_SIZE][QR_SIZE];
    uint16_t fgColor;
    uint16_t bgColor;
    uint8_t scale;
    uint8_t dataBuffer[64];
    
    void addFinderPattern(int x, int y);
    void addTimingPatterns();
    void addFormatInfo();
    void setModule(int x, int y, bool value);
    bool getModule(int x, int y);
    void encodeData(const char* text);
    void applyMask();
    
public:
    SimpleQR(Adafruit_ST7735* display);
    void init();
    void setColors(uint16_t foreground, uint16_t background);
    void setScale(uint8_t scale);
    bool create(const char* text);
    void render();
};

#endif
