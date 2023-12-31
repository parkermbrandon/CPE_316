#include "main.h"

volatile uint16_t adc_value = 0;
volatile uint8_t adc_flag = 0;
uint16_t adc_samples[20];

void ADC_init() {
    // Enable ADC1 clock
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

    // Configure ADC clock to 24 MHz
    // Assuming system clock is 80 MHz
    RCC->CCIPR |= (0x1 << 28);

    // 12-bit resolution
    ADC1->CFGR &= ~ADC_CFGR_RES;

    // Single conversion, not repeated
    ADC1->CFGR &= ~ADC_CFGR_CONT;

    // Sample time of 2.5 ADC clock cycles
    ADC1->SMPR1 |= (0x2 << 3);

    // Configure to use channel 1 (ADC123_IN1)
    ADC1->SQR1 &= ~ADC_SQR1_SQ1;
    ADC1->SQR1 |= (1 << 6);  // 1 shifted by 6 positions

    // Use 3.3V reference
    ADC1->CR &= ~ADC_CR_DEEPPWD;
    ADC1->CR |= ADC_CR_ADVREGEN;

    ADC1->IER |= ADC_IER_EOCIE;

    if (ADC1->ISR & ADC_ISR_ADRDY) {
        ADC1->ISR |= ADC_ISR_ADRDY;  // Clear the flag by writing 1
    }

    // Enable ADC
    ADC1->CR |= ADC_CR_ADEN;
    while (!(ADC1->ISR & ADC_ISR_ADRDY));

    // Enable ADC interrupt
    NVIC_EnableIRQ(ADC1_2_IRQn);


}

void GPIO_Config() {
    // Enable GPIOC clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // Set PC0 as analog mode
    GPIOC->MODER |= (0x3 << (0 * 2));
}

void ADC1_2_IRQHandler() {
    if (ADC1->ISR & ADC_ISR_EOC) {
        adc_value = ADC1->DR;
        adc_flag = 1;
        ADC1->ISR |= ADC_ISR_EOC;  // Clear the flag
    }
}

int main() {
    uint16_t min, max, sum;
    uint32_t avg;
    int i = 0;

    GPIO_Config();
    ADC_init();

    // Start firsty conversion
    ADC1->CR |= ADC_CR_ADSTART;

    while (1) {
        if (adc_flag) {
            adc_samples[i++] = adc_value;
            adc_flag = 0;

            // Start another conversion
            ADC1->CR |= ADC_CR_ADSTART;

            if (i >= 20) {
                // Reset counter
                i = 0;

                // Calculate min, max, and avg
                min = max = adc_samples[0];
                sum = 0;

                for (int j = 0; j < 20; j++) {
                    if (adc_samples[j] < min) min = adc_samples[j];
                    if (adc_samples[j] > max) max = adc_samples[j];
                    sum += adc_samples[j];
                }

                avg = sum / 20;
            }
        }
    }
}
