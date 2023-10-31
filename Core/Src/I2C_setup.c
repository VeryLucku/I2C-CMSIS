#include "I2C_setup.h"

void pre_send_data_ops(uint16_t mem_addr);
void post_send_data_ops();

void I2C_init()
{
    SystemCoreClockUpdate();

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);

    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);

    //enable alternate function mode
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE6, 0b10 << GPIO_MODER_MODE6_Pos);
    MODIFY_REG(GPIOB->MODER, GPIO_MODER_MODE7, 0b10 << GPIO_MODER_MODE7_Pos);

    //enable open-drain mode
    SET_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT6);
    SET_BIT(GPIOB->OTYPER, GPIO_OTYPER_OT7);

    // change to I2C mode
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL6, 0b0100 << GPIO_AFRL_AFSEL6_Pos);
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL7, 0b0100 << GPIO_AFRL_AFSEL7_Pos);

    //set I2C freq = APB1 freq
    uint32_t APB1_clock = SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos];
    MODIFY_REG(I2C1->CR2, I2C_CR2_FREQ_Msk, APB1_clock/1000000);

    //enable fast mode
    SET_BIT(I2C1->CCR, I2C_CCR_FS);

    //set duty=2:1
    CLEAR_BIT(I2C1->CCR, I2C_CCR_DUTY);
    
    //set I2C freq to 400k Hz
    uint32_t I2C_clock = 400000;
    uint16_t ccr = APB1_clock/I2C_clock/3+1;
    MODIFY_REG(I2C1->CCR, I2C_CCR_CCR, ccr);

    //set max Trise value
    MODIFY_REG(I2C1->TRISE, I2C_TRISE_TRISE, (((APB1_clock/1000000) * 300U) / 1000U) + 1U);

    //enable peripheral
    SET_BIT(I2C1->CR1, I2C_CR1_PE);
}

void I2C_send_batch(uint8_t *data, uint8_t data_size, uint16_t mem_addr)
{
    pre_send_data_ops(mem_addr);

    //send data
    for (uint8_t pos = 0; pos < data_size; ++pos) {
        I2C1->DR = data[pos];
        //wait until data register empty flag
        while (!READ_BIT(I2C1->SR1, I2C_SR1_TXE));
    }

    post_send_data_ops();
}

void I2C_send_byte(uint8_t data, uint16_t mem_addr)
{
    pre_send_data_ops(mem_addr);

    //send data
    I2C1->DR = data;

    post_send_data_ops();
}

void pre_send_data_ops(uint16_t mem_addr) {
    //start bit
    SET_BIT(I2C1->CR1, I2C_CR1_START);

    //wait until SB event
    while(!READ_BIT(I2C1->SR1, I2C_SR1_SB));
    //clear SB flag
    (void) I2C1->SR1;

    //send ssd1306 driver address to accept with controller
    I2C1->DR = SSD1306_I2C_ADDR;

    //wait until address was sent
    while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR));
    //clear ADDR register
    (void) I2C1->SR1;
    (void) I2C1->SR2;

    //send adress in reciever memory
    I2C1->DR = mem_addr & 0x00FF;
    //wait until data register empty flag
    while (!READ_BIT(I2C1->SR1, I2C_SR1_TXE));
}

void post_send_data_ops() {
    //wait until byte transfer finished
    while(!READ_BIT(I2C1->SR1, I2C_SR1_BTF));

    //stop bit
    SET_BIT(I2C1->CR1, I2C_CR1_STOP);
}



