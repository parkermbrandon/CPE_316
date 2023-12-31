#include "main.h"

void TIM2_IRQHandler(void);

volatile uint8_t flag = 0;

int main(void)
{
    // Enable clock for GPIOA and TIM2
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // Configure PA0 and PA1 as output
    GPIOA->MODER &= ~(GPIO_MODER_MODE0 | GPIO_MODER_MODE1);
    GPIOA->MODER |= GPIO_MODER_MODE0_0 | GPIO_MODER_MODE1_0;

    // Set up TIM2
    TIM2->PSC = 0;  // No prescaling
    TIM2->ARR = 0xFFFFFFFF;  // Run continuously
    TIM2->CCR1 = 1;  // 50% duty cycle
    TIM2->DIER |= TIM_DIER_CC1IE;  // Enable Capture/Compare 1 interrupt

    // Enable TIM2
    TIM2->CR1 |= TIM_CR1_CEN;

    // Enable TIM2 interrupt
    NVIC_EnableIRQ(TIM2_IRQn);

    // Enable MCO, select MSI (4 MHz source)
    RCC->CFGR = ((RCC->CFGR & ~(RCC_CFGR_MCOSEL)) | (RCC_CFGR_MCOSEL_0));

    // Configure MCO output on PA8
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER &= ~GPIO_MODER_MODE8;
    GPIOA->MODER |= (2 << GPIO_MODER_MODE8_Pos);
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT8;
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD8;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEED8;
    GPIOA->AFR[1] &= ~GPIO_AFRH_AFSEL8;

    // Enable global interrupts
    __enable_irq();

    while (1)
    {
        // Do nothing, let ISR handle everything
    }
}

void TIM2_IRQHandler(void)
{
    // Set PA1 high at ISR entry
    GPIOA->BSRR = GPIO_BSRR_BS1;

    // Check if the Capture/Compare 1 interrupt flag is set
    if (TIM2->SR & TIM_SR_CC1IF)
    {
        // Clear the Capture/Compare 1 interrupt flag
        TIM2->SR &= ~TIM_SR_CC1IF;

        // Toggle PA0
        GPIOA->ODR ^= GPIO_ODR_OD0;

        // Reset the counter
        TIM2->CNT = 0;
    }

    // Set PA1 low at ISR exit
    GPIOA->BSRR = GPIO_BSRR_BR1;
}
