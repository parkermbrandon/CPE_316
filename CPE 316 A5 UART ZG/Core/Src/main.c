#include "main.h"

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

	draw_init();



    // Main loop
    while (1) {

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
