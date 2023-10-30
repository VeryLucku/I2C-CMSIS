#include "I2C_setup.h"


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
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL6, 0b0011 << GPIO_AFRL_AFSEL6_Pos);
    MODIFY_REG(GPIOB->AFR[0], GPIO_AFRL_AFSEL7, 0b0011 << GPIO_AFRL_AFSEL7_Pos);

    //set I2C freq = APB1 freq
    uint32_t APB1_clock = SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> RCC_CFGR_PPRE1_Pos];
    MODIFY_REG(I2C1->CR2, I2C_CR2_FREQ_Msk, APB1_clock);

    //enable fast mode
    SET_BIT(I2C1->CCR, I2C_CCR_FS);

    //set duty=2:1
    CLEAR_BIT(I2C1->CCR, I2C_CCR_DUTY);
    
    //set I2C freq to 400k Hz
    uint32_t I2C_clock = 400000;
    uint16_t ccr = APB1_clock/I2C_clock/3;
    MODIFY_REG(I2C1->CCR, I2C_CCR_CCR, ccr);

    //enable peripheral
    SET_BIT(I2C1->CR1, I2C_CR1_PE);

}
