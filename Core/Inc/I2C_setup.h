#include "stm32f4xx.h"

#define SSD1306_I2C_ADDR        (0x3C << 1)
#define SSD1306_WIDTH 128
#define SSD1306_Height 32
#define SSD1306_BUFFER_SIZE SSD1306_WIDTH*SSD1306_Height/8

#define WRITE_COMMAND_MEM_ADDR 0x00
#define WRITE_DATA_MEM_ADDR 0x40

#define SSD1306_X_OFFSET_LOWER 0
#define SSD1306_X_OFFSET_UPPER 0

// Enumeration for screen colors
typedef enum {
    Black = 0x00, // Black color, no pixel
    White = 0x01  // Pixel is set. Color depends on OLED
} SSD1306_COLOR;

void I2C_init();

void I2C_send(uint8_t *data, uint8_t data_size, uint16_t mem_addr);