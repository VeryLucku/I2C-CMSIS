#include "I2C_setup.h"
#include "ssd1306.h"

uint8_t SSD1306_buffer[SSD1306_BUFFER_SIZE];

static SSD1306_t SSD1306;

uint8_t init_data[] = {0xAE, //set display off
                    0x20,
                    0x00,//memory addressing mode (horizontal)
                    0xB0, //page start adress
                    0xC8, //scan direction
                    0x00, //set low column address
                    0x10, //set high column address
                    0x40, //set start line address
                    0x81,//set contrast
                    0xFF,
                    0xA1, //set segment re-map 0 to 127
                    0xA6, //normal color (0xA7 - inverse color)
                    0xA8, // set multiplex ratio (1 to 64)
                    0x1F, //set screen height
                    0xA4, //output follows RAM content (0xA5 - output ignores RAM content)
                    0xD3, //set display offset
                    0x00, //no offset
                    0xD5, //set display clock divide ratio/oscillator frequency
                    0xF0, // set divide ratio
                    0xD9, // set pre-charge period
                    0x22,
                    0xDA, // set com pins hardware configuration
                    0x02,
                    0xDB, // set vcomh
                    0x20, // 0.77Vcc
                    0x8D, // set DC-DC enable
                    0x14,
                    0xAF // on display
                    };
                    

void ssd1306_Fill(SSD1306_COLOR color) {
    for (int i = 0; i < SSD1306_BUFFER_SIZE; ++i) {
        SSD1306_buffer[i] = color;
    }
}

void ssd1306_UpdateScreen() {
    uint8_t data[] = {0xB0, 0x00+SSD1306_X_OFFSET_LOWER, 0x10+SSD1306_X_OFFSET_UPPER};
    for (uint8_t i = 0; i < SSD1306_HEIGHT/8; ++i) {
        I2C_send(data, 3, WRITE_COMMAND_MEM_ADDR);
        I2C_send(&SSD1306_buffer[SSD1306_WIDTH*i], SSD1306_WIDTH, WRITE_DATA_MEM_ADDR);
        ++data[0];
    }
}

void ssd1306_DrawPixel(uint8_t x, uint8_t y, SSD1306_COLOR color) {
    if(x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT) {
        // Don't write outside the buffer
        return;
    }
   
    // Draw in the right color
    if(color == White) {
        SSD1306_buffer[x + (y / 8) * SSD1306_WIDTH] |= 1 << (y % 8);
    } else { 
        SSD1306_buffer[x + (y / 8) * SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
}

char ssd1306_WriteChar(char ch, FontDef Font, SSD1306_COLOR color) {
    uint32_t i, b, j;
    
    // Check if character is valid
    if (ch < 32 || ch > 126)
        return 0;
    
    // Check remaining space on current line
    if (SSD1306_WIDTH < (SSD1306.CurrentX + Font.FontWidth) ||
        SSD1306_HEIGHT < (SSD1306.CurrentY + Font.FontHeight))
    {
        // Not enough space on current line
        return 0;
    }
    
    // Use the font to write
    for(i = 0; i < Font.FontHeight; i++) {
        b = Font.data[(ch - 32) * Font.FontHeight + i];
        for(j = 0; j < Font.FontWidth; j++) {
            if((b << j) & 0x8000)  {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR) color);
            } else {
                ssd1306_DrawPixel(SSD1306.CurrentX + j, (SSD1306.CurrentY + i), (SSD1306_COLOR)!color);
            }
        }
    }
    
    // The current space is now taken
    SSD1306.CurrentX += Font.FontWidth;
    
    // Return written char for validation
    return ch;
}

char ssd1306_WriteString(char* str, FontDef Font, SSD1306_COLOR color) {
    while (*str) {
        if (ssd1306_WriteChar(*str, Font, color) != *str) {
            // Char could not be written
            return *str;
        }
        str++;
    }
    
    // Everything ok
    return *str;
}

void ssd1306_init() {
    I2C_init();

    I2C_send(init_data, sizeof(init_data)/sizeof(init_data[0]), WRITE_COMMAND_MEM_ADDR);

    ssd1306_Fill(Black);

    ssd1306_UpdateScreen();

    // Set default values for screen object
    SSD1306.CurrentX = 0;
    SSD1306.CurrentY = 0;
}