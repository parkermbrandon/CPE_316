#include "main.h"

void TIM2_IRQHandler(void);

volatile uint8_t flagA2outpu = 0;

int mainA2output(void)
{
    // Enable clock for GPIOA and TIM2
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;

    // Configure PA0 as output
    GPIOA->MODER &= ~GPIO_MODER_MODE0;
    GPIOA->MODER |= GPIO_MODER_MODE0_0;

    // Set up TIM2
    TIM2->PSC = 0;  // No prescaling
    TIM2->ARR = 799;  // Auto-reload value for 5 kHz
    TIM2->CCR1 = 199;  // 25% duty cycle
    TIM2->DIER |= TIM_DIER_CC1IE;  // Enable Capture/Compare 1 interrupt

    // Enable TIM2
    TIM2->CR1 |= TIM_CR1_CEN;

    // Enable TIM2 interrupt
    NVIC_EnableIRQ(TIM2_IRQn);

    // Enable global interrupts
    __enable_irq();

    while (1)
    {
        // Do nothing, let ISR handle everything
    }
}

void TIM2_IRQHandlerA2output(void)
{
    // Check if the Capture/Compare 1 interrupt flag is set
    if (TIM2->SR & TIM_SR_CC1IF)
    {
        // Clear the Capture/Compare 1 interrupt flag
        TIM2->SR &= ~TIM_SR_CC1IF;

        if (flagA2outpu == 0)
        {
            // Set PA0 high
            GPIOA->BSRR = GPIO_BSRR_BS0;
            TIM2->CCR1 = 199;  // 25% of the period
            flagA2outpu = 1;
        }
        else
        {
            // Set PA0 low
            GPIOA->BSRR = GPIO_BSRR_BR0;
            TIM2->CCR1 = 799;  // Reset to 100% of the period
            flagA2outpu = 0;
        }
    }
}
