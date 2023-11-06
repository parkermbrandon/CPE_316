#include "main.h"

void SystemClock_Config(void);
void ADC_init(void);
void UART_init();
void UART_print(const char * message);
uint32_t ADC_Read(void);
void process_samples(uint16_t samples[], uint16_t *min, uint16_t *max, uint32_t *avg);
uint32_t calibrate_adc_value(uint16_t adc_value);
void USART_print_number(uint32_t number);

volatile uint16_t adc_val;
volatile uint8_t adc_flag = 0;
uint16_t adc_samples[20];

#define F_CLK 24000000 // 80 Mhz

void USART_init() {
    // Enable USART2 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN;

    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Configure PA2 as USART2_TX and PA3 as USART2_RX
    GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3);
    GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1);

    // Set pins to alternate function for USART2 (AF7 for STM32L476)
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL2 | GPIO_AFRL_AFSEL3);		// mask AF selection
    GPIOA->AFR[0] |= ((7 << GPIO_AFRL_AFSEL2_Pos ) | (7 << GPIO_AFRL_AFSEL3_Pos));

    // Configure for 8 data bits
    USART2->CR1 &= ~(USART_CR1_M);

    // Set the baudrate to 115200
    USART2->BRR = F_CLK / 115200;

    // Configure for 1 stop bit
    USART2->CR2 &= ~(USART_CR2_STOP);

    // Enable USART, Transmitter and Receiver
    USART2->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
}

void ADC_init(void) {


    // Enable the ADC interface clock
    RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

    // Configure the ADC clock source
    RCC->CCIPR &= ~RCC_CCIPR_ADCSEL;   // Clear the ADC clock selection bits
    RCC->CCIPR |= RCC_CCIPR_ADCSEL_1;  // Select HSI16 (16 MHz RC oscillator) as ADC clock

    // Clock divider setup
    ADC123_COMMON->CCR &= ~ADC_CCR_PRESC;  // Clear prescaler bits
    //ADC123_COMMON->CCR |= ADC_CLOCK_ASYNC_DIV1; // Set prescaler to DIV1

    // ADC configuration
    ADC1->CFGR &= ~ADC_CFGR_RES; // 12-bit resolution
    ADC1->CFGR &= ~ADC_CFGR_ALIGN; // Right alignment
    ADC1->CFGR &= ~ADC_CFGR_CONT; // Disable continuous conversion
    //ADC1->CFGR |= ADC_CFGR_CONT; // Enabled continuous conversion

    // ADC Sampling Time Configuration for Channel 9
    ADC1->SMPR1 &= ~ADC_SMPR1_SMP9; // Clear previous settings
    ADC1->SMPR1 |= ADC_SMPR1_SMP9_0; // Sampling time: 2.5 ADC clock cycles

    // ADC Regular Channel Configuration
    ADC1->SQR1 &= ~ADC_SQR1_SQ1; // Reset channel
    ADC1->SQR1 |= ADC_SQR1_SQ1_0 | ADC_SQR1_SQ1_3; // Set channel 9 to rank 1

    // Disable ADC deep power-down mode
    ADC1->CR &= ~ADC_CR_DEEPPWD;

    // Enable ADC internal voltage regulator
    ADC1->CR |= ADC_CR_ADVREGEN;

    // Delay for ADC voltage regulator stabilization (time is dependent on the system clock)
    HAL_Delay(1000);

    // Start ADC self-calibration
    //ADC1->CR |= ADC_CR_ADCAL;

    // Wait for calibration to complete
    //while (ADC1->CR & ADC_CR_ADCAL);

    // Enable ADC1
    ADC1->CR |= ADC_CR_ADEN;
    while(!(ADC1->ISR & ADC_ISR_ADRDY)); // Wait for ADC ready

    // Enable GPIOA clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Set PA4 as analog mode
    GPIOA->MODER |= GPIO_MODER_MODE0;
}

// Function to print a string via USART2
void USART_print(const char *str) {
    while (*str) {
        // Wait for Transmit Data Register Empty
        while (!(USART2->ISR & USART_ISR_TXE));

        // Write data to TDR
        USART2->TDR = *str;

        // Wait for Transmission Complete
        while (!(USART2->ISR & USART_ISR_TC));

        str++;
    }
}

void ADC1_2_IRQHandler(void) {
    // Check if ADC conversion is complete
    if (ADC1->ISR & ADC_ISR_EOC) {
        adc_val = ADC1->DR;  // Save conversion result
        adc_flag = 1;  // Set flag
    }
}

uint32_t ADC_Read(void) {
    ADC1->CR |= ADC_CR_ADSTART;
    while(adc_flag == 0);  // Wait for ADC conversion to complete
    adc_flag = 0;  // Clear flag
    return adc_val;
}

void process_samples(uint16_t samples[], uint16_t *min, uint16_t *max, uint32_t *avg) {
    *min = UINT16_MAX;
    *max = 0;
    *avg = 0;

    for (int i = 0; i < 20; i++) {
        if (samples[i] < *min) *min = samples[i];
        if (samples[i] > *max) *max = samples[i];
        *avg += samples[i];
    }
    *avg /= 20;
}

uint32_t calibrate_adc_value(uint16_t adc_value) {
    // 3.3V reference and 12-bit ADC
    // The value returned is in millivolts to avoid floating point.
    return (3300 * adc_value) / 4095;
}

void USART_print_number(uint32_t number) {
    char buffer[10];
    int index = 0;

    if (number == 0) {
        USART_print("0.00");
        return;
    }

    // Convert integer part
    uint32_t int_part = number / 100;
    uint32_t dec_part = number % 100;

    while (int_part > 0) {
        buffer[index++] = '0' + (int_part % 10);
        int_part /= 10;
    }

    for (int i = index - 1; i >= 0; i--) {
        char str[2] = {buffer[i], '\0'};
        USART_print(str);
    }

    USART_print(".");

    USART_print((dec_part < 10) ? "0" : "");
    if (dec_part < 10) {
        USART_print("0");  // Print leading zero for numbers below 10
    }
    USART_print_number(dec_part);  // Print the two-digit decimal part
}

int main(void){

	HAL_Init();
	SystemClock_Config();

	uint32_t sysClockFreq = HAL_RCC_GetSysClockFreq();
	USART_print_number(sysClockFreq);


	USART_init();
	USART_print("The ADC is initializing\n");
	ADC_init();
	USART_print("The ADC has initialized\n");

	uint16_t min, max;
	uint32_t avg;



    while(1) {
        for (int i = 0; i < 20; i++) {
            adc_samples[i] = ADC_Read();
        }

        process_samples(adc_samples, &min, &max, &avg);

        min = calibrate_adc_value(min);
        max = calibrate_adc_value(max);
        avg = calibrate_adc_value(avg);

        USART_print("Min: ");
        USART_print_number(min);
        USART_print(" mV\r\n");

        USART_print("Max: ");
        USART_print_number(max);
        USART_print(" mV\r\n");

        USART_print("Avg: ");
        USART_print_number(avg);
        USART_print(" mV\r\n");

        USART_print("\r\n");  // Newline for clarity

        HAL_Delay(1000);  // Delay for a second before the next batch of readings
    }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
