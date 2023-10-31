#include "stm32f4xx.h"

#define SSD1306_I2C_ADDR        (0x3C << 1)

#define WRITE_COMMAND_MEM_ADDR 0x00
#define WRITE_DATA_MEM_ADDR 0x40

void I2C_init();

void I2C_send_batch(uint8_t *data, uint8_t data_size, uint16_t mem_addr);
void I2C_send_byte(uint8_t data, uint16_t mem_addr);