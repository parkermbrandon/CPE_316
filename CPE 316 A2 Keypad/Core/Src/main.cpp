#include "main.h"
#include <stdio.h>

void SystemClock_Config(void);

#define _KEYPAD_SETTLING_DELAY 5

const uint8_t ROWS = 4;  // Four rows
const uint8_t COLS = 3;  // Three columns

char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};

uint8_t rowPins[ROWS] = {10, 3, 5, 4};  // PB10, PB3, PB5, PB4
uint8_t colPins[COLS] = {0 , 12, 1};  // PA0, PA4, PA1

volatile uint8_t keystates[ROWS * COLS] = {0};

void Init_GPIO(void)
{
    // Enable GPIOA, GPIOB, and GPIOC clocks
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN | RCC_AHB2ENR_GPIOAEN;

	// Clear the LED GPIO registers
	GPIOB->MODER &= ~GPIO_MODER_MODER6;
	GPIOC->MODER &= ~GPIO_MODER_MODER7;
	GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER8);

	// Set the LED GPIO pins as output
	GPIOB->MODER |= GPIO_MODER_MODER6_0;
	GPIOC->MODER |= GPIO_MODER_MODER7_0;
	GPIOA->MODER |= GPIO_MODER_MODER9_0 | GPIO_MODER_MODER8_0;

    // Configure ROWs as input with pull-up
    GPIOB->MODER &= ~(GPIO_MODER_MODER10 | GPIO_MODER_MODER3 | GPIO_MODER_MODER5 | GPIO_MODER_MODER4);
    GPIOB->PUPDR |= (GPIO_PUPDR_PUPDR10_0 | GPIO_PUPDR_PUPDR3_0 | GPIO_PUPDR_PUPDR5_0 | GPIO_PUPDR_PUPDR4_0);

    // Configure COLs as output
    GPIOA->MODER &= ~(GPIO_MODER_MODER12 | GPIO_MODER_MODER0 | GPIO_MODER_MODER1);
    GPIOA->MODER |= (GPIO_MODER_MODER12_0 | GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0);

    // Initialize columns to high
    for (int c = 0; c < COLS; c++) {
        GPIOA->BSRR = (1U << colPins[c]);
    }
}

void tick() {
  for (int c = 0; c < COLS; c++) {
    GPIOA->BSRR = (1U << (colPins[c] + 16));  // sets the current column pin to low
    // This is done because the rows are pulled-up. When a button is pressed, this column will be shorted to one of the rows, pulling it to a low state.

    for (int r = 0; r < ROWS; r++) {
      int i = r * COLS + c;
      bool pressed = !(GPIOB->IDR & (1U << rowPins[r])); // This checks if the current row pin is low (i.e., a button on this row has been pressed).

      if (pressed) {
        keystates[i] = 1;
      } else {
        keystates[i] = 0;
      }
    }
    GPIOA->BSRR = (1U << colPins[c]); // After checking all rows for a given column, the column is set back to high
  }
}

void update_LEDs(char key) {
  // Clear all LEDs
  GPIOB->BSRR = GPIO_BSRR_BR_6;
  GPIOC->BSRR = GPIO_BSRR_BR_7;
  GPIOA->BSRR = GPIO_BSRR_BR_9 | GPIO_BSRR_BR_8;

  int binaryRepresentation = -1;

  if (key >= '0' && key <= '9') {
    binaryRepresentation = key - '0';  // Convert char to int
  } else if (key == '*') {
    binaryRepresentation = 14;  // 1110 in binary
  } else if (key == '#') {
    binaryRepresentation = 15;  // 1111 in binary
  }

  if (binaryRepresentation != -1) {
    if (binaryRepresentation & 0b0001) GPIOB->BSRR = GPIO_BSRR_BS_6;
    if (binaryRepresentation & 0b0010) GPIOC->BSRR = GPIO_BSRR_BS_7;
    if (binaryRepresentation & 0b0100) GPIOA->BSRR = GPIO_BSRR_BS_9;
    if (binaryRepresentation & 0b1000) GPIOA->BSRR = GPIO_BSRR_BS_8;
  }
}

int main(void)
{
  HAL_Init();
  //SystemClock_Config();
  Init_GPIO();

  while (1)
  {
    tick();  // Update keypad status

    for (int r = 0; r < ROWS; r++) {
      for (int c = 0; c < COLS; c++) {
        int i = r * COLS + c;
        if (keystates[i]) {
          char key = keys[r][c];
          update_LEDs(key);  // Update LEDs based on pressed key
          //break;
        }
      }
    }
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

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
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }

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
