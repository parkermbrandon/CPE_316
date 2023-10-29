#include "main.h"

volatile uint16_t adc_value;
volatile uint8_t adc_flag = 0;
uint16_t adc_samples[20];
volatile uint8_t adc_index = 0;

void SystemClock_Config(void);

// Function to initialize LPUART1
void LPUART1_init() {
    // Set IOSV bit for PG[15:2]
    PWR->CR2 |= PWR_CR2_IOSV;

    // Enable LPUART1 clock
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN;

    // Enable GPIOG clock
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOGEN;

    // Configure PG7 as LPUART1_TX and PG8 as LPUART1_RX
    GPIOG->MODER &= ~(GPIO_MODER_MODE7 | GPIO_MODER_MODE8);
    GPIOG->MODER |= (GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1);

    // Set Alternate Function for PG7 (LPUART1_TX)
    GPIOG->AFR[0] |= (8U << GPIO_AFRL_AFSEL7_Pos);

    // Set Alternate Function for PG8 (LPUART1_RX)
    GPIOG->AFR[1] |= (8U << GPIO_AFRH_AFSEL8_Pos);

    // Configure for 8 data bits
	LPUART1->CR1 &= ~(USART_CR1_M);

    // Set Baud Rate Divisor
    LPUART1->BRR = 0x2B671;  // 90Mhz * 256 / 115200 (desired baud) = 0x2B671

    // Configure for 1 stop bit
	LPUART1->CR2 &= ~(USART_CR2_STOP);

    // Enable LPUART, Transmitter and Receiver
    LPUART1->CR1 |= (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE);
}

//void ADC_init() {
//    // Enable ADC1 clock
//    SET_BIT(RCC->AHB2ENR, RCC_AHB2ENR_ADCEN);
//
//    // Disable deep-power-down mode
//    CLEAR_BIT(ADC1->CR, ADC_CR_DEEPPWD);
//
//    // Enable voltage regulator
//    SET_BIT(ADC1->CR, ADC_CR_ADVREGEN);
//
//    // Configure ADC clock to be at least 24 MHz (90/4 = 22.5 MHz)
//    // Assuming that ADC clock is derived from system clock
//    MODIFY_REG(ADC1->CFGR2, ADC_CCR_CKMODE, 0x00); // Clock mode: 00 (Synchronous clock mode)
//
//    // Set sample time to 2.5 ADC clock cycles
//    MODIFY_REG(ADC1->SMPR1, ADC_SMPR1_SMP1, 0x00); // 000: 2.5 cycles
//
//    // Set resolution to 12-bit
//    MODIFY_REG(ADC1->CFGR, ADC_CFGR_RES, 0x00); // 00: 12-bit resolution
//
//    // Set reference voltage to 3.3V (default, no need to set in register)
//
//    // Enable ADC
//    SET_BIT(ADC1->CR, ADC_CR_ADEN);
//
//    // Wait for ADC to be ready
//    while (!(READ_BIT(ADC1->ISR, ADC_ISR_ADRDY))) {
//        // Wait
//    }
//
//    // Enable end-of-conversion interrupt
//    SET_BIT(ADC1->IER, ADC_IER_EOCIE);
//
//    // Enable ADC interrupt in NVIC
//    NVIC_EnableIRQ(ADC1_IRQn);
//}

void ADC_init() {
	// Enable ADC clock
	RCC->AHB2ENR |= RCC_AHB2ENR_ADCEN;

	// Enable GPIOC clock
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;

    // Configure PA0 as analog input
    GPIOC->MODER |= GPIO_MODER_MODE0;  // Set to 11 (Analog mode)

    // Reset the ADC1 control register
    ADC1->CR = 0x20000000;

    // Step 1: Exit Deep-power-down mode
    ADC1->CR &= ~ADC_CR_DEEPPWD;

    // Step 2: Enable voltage regulator
    ADC1->CR |= ADC_CR_ADVREGEN;

    // Wait for ADC voltage regulator startup time (10ms as per datasheet)
    HAL_Delay(10);

    // Step 3: Ensure ADEN=0
    ADC1->CR &= ~ADC_CR_ADEN;

    // Step 4: Perform calibration
    // For single-ended input
    //ADC1->CR &= ~ADC_CR_ADCALDIF;
    //ADC1->CR |= ADC_CR_ADCAL;
    //while (ADC1->CR & ADC_CR_ADCAL);

    // Step 5: Enable the ADC
    // Clear the ADRDY bit
    ADC1->ISR |= ADC_ISR_ADRDY;
    // Enable the ADC
    ADC1->CR |= ADC_CR_ADEN;
    // Wait for ADC to be ready
    //while (!(ADC1->ISR & ADC_ISR_ADRDY));

    // Other configurations (sample time, resolution, etc.)
    // Configure ADC clock to be at least 24 MHz (90/4 = 22.5 MHz)
    ADC1->CFGR2 &= ~ADC_CCR_CKMODE; // Clock mode: 00 (Synchronous clock mode)

    // Set sample time to 2.5 ADC clock cycles
    ADC1->SMPR1 &= ~ADC_SMPR1_SMP1; // 000: 2.5 cycles

    // Set resolution to 12-bit
    ADC1->CFGR &= ~ADC_CFGR_RES; // 00: 12-bit resolution

    // Configure PA0 as analog input
    GPIOC->MODER |= GPIO_MODER_MODE0;  // Set to 11 (Analog mode)

    // Select channel 0 for the first conversion in the sequence
    ADC1->SQR1 |= (0 << ADC_SQR1_SQ1_Pos);

    // Enable end-of-conversion interrupt
    ADC1->IER |= ADC_IER_EOCIE;

    // Enable ADC interrupt in NVIC
    NVIC_EnableIRQ(ADC1_IRQn);
}



// Function to print a string via LPUART1
void UART_print(const char *str) {
    while (*str) {
        // Wait for Transmit Data Register Empty
        while (!(LPUART1->ISR & USART_ISR_TXE));

        // Write data to TDR
        LPUART1->TDR = *str;

        // Wait for Transmission Complete
        while (!(LPUART1->ISR & USART_ISR_TC));

        str++;
    }
}

// Function to send escape codes via USART
void UART_ESC_Code(const char *str) {
    UART_print("\033[");  // ESC [
    UART_print(str);
}

// LPUART1 Interrupt Service Routine
void LPUART1_IRQHandler(void) {
    // Check if data is available to read
    if (LPUART1->ISR & USART_ISR_RXNE) {
        // Read received data
        char received_char = LPUART1->RDR;

        // Check for special characters for color change
        if (received_char == 'R') {
            UART_ESC_Code("31m");  // Set text color to red
        } else if (received_char == 'B') {
            UART_ESC_Code("34m");  // Set text color to blue
        } else if (received_char == 'G') {
            UART_ESC_Code("32m");  // Set text color to green
        } else if (received_char == 'W') {
            UART_ESC_Code("37m");  // Set text color to white
        } else {
            // Echo received data
            char str[2] = {received_char, '\0'};
            UART_print(str);
        }

    }
}

void ADC1_IRQHandler(void)
{
    // Clear the interrupt flag
    CLEAR_BIT(ADC1->ISR, ADC_ISR_EOC);

    // Read the ADC value
    adc_samples[adc_index] = ADC1->DR;

    // Increment the index
    adc_index++;

    // Check if we've collected 20 samples
    if(adc_index >= 20)
    {
        adc_flag = 1;  // Set a flag to process the data
        adc_index = 0; // Reset the index
    }
    else
    {
        // Start the next conversion
        SET_BIT(ADC1->CR, ADC_CR_ADSTART);
    }
}

void process_samples() {
    uint16_t min_value = 0xFFFF;
    uint16_t max_value = 0;
    uint32_t sum_value = 0;

    for (int i = 0; i < 20; i++) {
        if (adc_samples[i] < min_value) {
            min_value = adc_samples[i];
        }
        if (adc_samples[i] > max_value) {
            max_value = adc_samples[i];
        }
        sum_value += adc_samples[i];
    }

    uint32_t avg_value = sum_value / 20;

    // Print the min, max, and average values
    char buffer[50];
    //sprintf(buffer, "Min: %d, Max: %d, Avg: %d\r\n", min_value, max_value, avg_value);
    //UART_print(buffer);

    // Reset for next round
    min_value = 0xFFFF;
    max_value = 0;
    sum_value = 0;
}

void draw_init(void)
{
		// Clear the screen
		UART_ESC_Code("2J");

		// Move the cursor down 3 lines and to the right 5 spaces
		UART_ESC_Code("3B");  // Move down 3 lines
		UART_ESC_Code("5C");  // Move right 5 spaces

		// Print text
		UART_print("All good students read the");

		// Move the cursor down 1 line and to the left 21 spaces
		UART_ESC_Code("1B");  // Move down 1 line
		UART_ESC_Code("21D"); // Move left 21 spaces

		// Change the text to blinking mode
		UART_ESC_Code("5m");

		// Print text
		UART_print("Reference Manual");

		// Move cursor back to the top left position
		UART_ESC_Code("H");

		// Remove character attributes (disable blinking text)
		UART_ESC_Code("0m");

		// Print text
		UART_print("Input: ");
}

int main() {
    // Initialize LPUART1
	HAL_Init();
    SystemClock_Config();
	LPUART1_init();

	// Enable LPUART1 RX interrupt
	LPUART1->CR1 |= USART_CR1_RXNEIE;
	NVIC_EnableIRQ(LPUART1_IRQn);

	// Initialization code here
	    ADC_init();

	// Start the first conversion
	SET_BIT(ADC1->CR, ADC_CR_ADSTART);

	draw_init();

	uint8_t sample_count = 0;


    // Main loop
    while (1)
    {
    	// Check for the global flag set by the ADC ISR
		//if (adc_flag)
		//{
			// Save the converted value into an array
			adc_samples[sample_count] = adc_value;

			// Reset the flag
			adc_flag = 0;

			 // Start the next set of conversions
			SET_BIT(ADC1->CR, ADC_CR_ADSTART);

			// Increment sample count
			sample_count++;

			// After collecting 20 samples
			if (sample_count >= 20) {
				// Reset sample count
				sample_count = 0;

				// Process the samples
				process_samples();
			}
		//}

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
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable MSI Auto calibration
  */
  HAL_RCCEx_EnableMSIPLLMode();
}

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
