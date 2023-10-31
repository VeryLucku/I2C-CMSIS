#include <stdint.h>
#include "ssd1306_font.h"

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 32
#define SSD1306_BUFFER_SIZE SSD1306_WIDTH*SSD1306_HEIGHT/8

#define WRITE_COMMAND_MEM_ADDR 0x00
#define WRITE_DATA_MEM_ADDR 0x40

#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0xFF  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

typedef struct {
    uint16_t CurrentX;
    uint16_t CurrentY;
    uint8_t Initialized;
    uint8_t DisplayOn;
} SSD1306_t;

char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color);
char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color);
void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color);
void ssd1306_UpdateScreen();
void ssd1306_Fill(SSD1306_COLOR color);

void ssd1306_init();