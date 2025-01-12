#include "SimpleQR.h"
#include <string.h>

SimpleQR::SimpleQR(Adafruit_ST7735* display) {
    tft = display;
    fgColor = ST77XX_BLACK;
    bgColor = ST77XX_WHITE;
    scale = 6;
}

void SimpleQR::init() {
    tft->fillScreen(bgColor);
}

void SimpleQR::setColors(uint16_t foreground, uint16_t background) {
    fgColor = foreground;
    bgColor = background;
}

void SimpleQR::setScale(uint8_t s) {
    scale = s;
}

void SimpleQR::setModule(int x, int y, bool value) {
    if(x >= 0 && x < QR_SIZE && y >= 0 && y < QR_SIZE) {
        matrix[y][x] = value;
    }
}

bool SimpleQR::getModule(int x, int y) {
    if(x >= 0 && x < QR_SIZE && y >= 0 && y < QR_SIZE) {
        return matrix[y][x];
    }
    return false;
}

void SimpleQR::addFinderPattern(int xOffset, int yOffset) {
    for(int y = 0; y < 7; y++) {
        for(int x = 0; x < 7; x++) {
            bool isBlack = (y == 0 || y == 6 || x == 0 || x == 6 || 
                           (x >= 2 && x <= 4 && y >= 2 && y <= 4));
            setModule(xOffset + x, yOffset + y, isBlack);
        }
    }
    
    for(int i = -1; i <= 7; i++) {
        setModule(xOffset - 1, yOffset + i, false);
        setModule(xOffset + 7, yOffset + i, false);
        setModule(xOffset + i, yOffset - 1, false);
        setModule(xOffset + i, yOffset + 7, false);
    }
}

void SimpleQR::addTimingPatterns() {
    for(int x = 8; x < QR_SIZE-8; x++) {
        setModule(x, 6, x % 2 == 0);
    }
    
    for(int y = 8; y < QR_SIZE-8; y++) {
        setModule(6, y, y % 2 == 0);
    }
}

void SimpleQR::addFormatInfo() {
    const uint16_t format = 0x77C4;
    
    for(int i = 0; i <= 5; i++)
        setModule(i, 8, (format >> i) & 1);
    setModule(7, 8, (format >> 6) & 1);
    setModule(8, 8, (format >> 7) & 1);
    setModule(8, 7, (format >> 8) & 1);
    
    for(int i = 0; i < 6; i++)
        setModule(8, 5-i, (format >> (14-i)) & 1);
}

void SimpleQR::encodeData(const char* text) {
    int len = strlen(text);
    if(len > MAX_LENGTH) len = MAX_LENGTH;
    
    uint8_t dataBuffer[32];
    memset(dataBuffer, 0, sizeof(dataBuffer));

    dataBuffer[0] = 0x40;  
    dataBuffer[1] = len;  
    memcpy(dataBuffer + 2, text, len);

    int bitIndex = 0;
    int x = QR_SIZE - 1;
    int y = QR_SIZE - 1;
    bool upward = true;

    while(bitIndex < (len + 2) * 8) {
        if(x >= 0 && !(x == 6 || y == 6 || (x < 9 && y < 9) || 
           (x > QR_SIZE-9 && y < 9) || (x < 9 && y > QR_SIZE-9))) {
            
            bool bit = (dataBuffer[bitIndex / 8] & (1 << (7 - (bitIndex % 8)))) != 0;
            setModule(x, y, bit);

            bitIndex++;
        }
        
        if(upward) {
            if(x > 0) { x--; y++; } else { x--; upward = false; }
        } else {
            if(x > 0) { x--; y--; } else { x--; upward = true; }
        }
        if(x < 0) { x = QR_SIZE - 1; y -= 2; if(y == 6) y--; }
    }
}

void SimpleQR::applyMask() {
    for(int y = 0; y < QR_SIZE; y++) {
        for(int x = 0; x < QR_SIZE; x++) {
            if((y + x) % 2 == 0) {
                if(!(x == 6 || y == 6 || (x < 9 && y < 9) || 
                   (x > QR_SIZE-9 && y < 9) || (x < 9 && y > QR_SIZE-9))) {
                    matrix[y][x] = !matrix[y][x];
                }
            }
        }
    }
}

void SimpleQR::render() {
    int offset_x = (tft->width() - (QR_SIZE * scale)) / 2;
    int offset_y = (tft->height() - (QR_SIZE * scale)) / 2;

    tft->fillScreen(bgColor);

    // Draw a border around the QR code
    int border = scale * 2;  // Optional: Adjust border size
    tft->fillRect(
        offset_x - border,
        offset_y - border,
        QR_SIZE * scale + border * 2,
        QR_SIZE * scale + border * 2,
        bgColor
    );

    for(int y = 0; y < QR_SIZE; y++) {
        for(int x = 0; x < QR_SIZE; x++) {
            if(matrix[y][x]) {
                tft->fillRect(
                    x * scale + offset_x,
                    y * scale + offset_y,
                    scale - 1,
                    scale - 1,
                    fgColor
                );
            }
        }
    }
}

bool SimpleQR::create(const char* text) {
    memset(matrix, 0, sizeof(matrix));

    addFinderPattern(0, 0);
    addFinderPattern(QR_SIZE-7, 0);
    addFinderPattern(0, QR_SIZE-7);
    
    addTimingPatterns();
    
    encodeData(text);
    
    applyMask();
    
    addFormatInfo();
    
    render();
    
    return true;
}
