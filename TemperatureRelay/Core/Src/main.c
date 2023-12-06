/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <math.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// MAX31865 register addresses
#define MAX31865_CONFIG_REG           0x00
#define MAX31865_RTDMSB_REG           0x01
#define MAX31865_RTDLSB_REG           0x02
#define MAX31865_HFAULTMSB_REG        0x03
#define MAX31865_HFAULTLSB_REG        0x04
#define MAX31865_LFAULTMSB_REG        0x05
#define MAX31865_LFAULTLSB_REG        0x06
#define MAX31865_FAULTSTAT_REG        0x07
#define REF_RESISTANCE                430
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void MAX31865_WriteRegister(uint8_t reg, uint8_t value);
uint8_t MAX31865_ReadRegister(uint8_t reg);
void MAX31865_Init(void);
float MAX31865_ReadTemperature(void);
float MAX31865_TemperatureConversion(float resistance);
void PrintTemperature(float temperature);
void HandleMAX31865Faults(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void MAX31865_WriteRegister(uint8_t reg, uint8_t value) {
    uint8_t data[2];
    data[0] = reg | 0x80; // Write operation
    data[1] = value;
    HAL_SPI_Transmit(&hspi1, data, 2, 100);
}

uint8_t MAX31865_ReadRegister(uint8_t reg) {
    uint8_t value, address = reg & 0x7F; // Read operation
    HAL_SPI_Transmit(&hspi1, &address, 1, 100);
    HAL_SPI_Receive(&hspi1, &value, 1, 100);
    return value;
}
//
//void MAX31865_Init(void) {
//    // Start with BIAS = 1 (D7), Conversion Mode = 1 (D6)
//    uint8_t config = 0xC0;
//
//    // Set D4 for 3-wire RTD mode
//    config |= (1 << 4);
//
//    // Configure for automatic fault detection cycle: Set D3 and clear D2 (D3:D2 = 10)
//    config |= (1 << 3); // Set D3
//    config &= ~(1 << 2); // Clear D2
//
//    // Write the configuration to the MAX31865
//    MAX31865_WriteRegister(MAX31865_CONFIG_REG, config);
//}
void MAX31865_Init(void) {
    // Configure for BIAS (D7) = 1, Conversion Mode (D6) = 1, 3-wire RTD (D4) = 1, automatic fault detection (D3:D2 = 10)
    uint8_t config = 0xD0; // 0b11010000
    MAX31865_WriteRegister(MAX31865_CONFIG_REG, config);

    // Read back the configuration register to verify
    uint8_t readConfig = MAX31865_ReadRegister(MAX31865_CONFIG_REG);
    if (readConfig != config) {
        // Handle configuration mismatch error
        // Example: Print an error message or halt the program
    }
}

float MAX31865_ReadTemperature(void) {
    uint16_t rtd;
    rtd = MAX31865_ReadRegister(MAX31865_RTDMSB_REG) << 8;
    rtd |= MAX31865_ReadRegister(MAX31865_RTDLSB_REG);

    // Calculate the actual resistance of the RTD
    float resistance = ((float)rtd) / 32768.0 * REF_RESISTANCE;

    // Now use this resistance to calculate the temperature
    float temperature = MAX31865_TemperatureConversion(resistance);

    return temperature;
}

float MAX31865_TemperatureConversion(float resistance) {
    float temperature;

    // Constants for Callendar-Van Dusen equation (for PT100)
    const float A = 3.9083e-3;
    const float B = -5.775e-7;
    const float R0 = 100.0; // Resistance of PT100 at 0°C

    // Solving quadratic equation for temperature: 0 = R0 * (1 + A*T + B*T^2) - Resistance
    // This is simplified to a quadratic equation of the form: a*T^2 + b*T + c = 0
    float a = R0 * B;
    float b = R0 * A;
    float c = R0 - resistance;

    // Calculate the discriminant
    float discriminant = b * b - 4.0f * a * c;

    // Check if discriminant is positive
    if (discriminant >= 0.0f) {
        // Real solutions exist
        temperature = (-b + sqrt(discriminant)) / (2.0f * a);
    } else {
        // Handle error: discriminant is negative, no real solutions
        // This might indicate a measurement or hardware problem
        temperature = -999.99f; // Use an error value or handle this appropriately
    }

    return temperature;
}

uint8_t MAX31865_ReadFault(void) {
    return MAX31865_ReadRegister(MAX31865_FAULTSTAT_REG);
}

void PrintTemperature(float temperature) {
    char buffer[50];
    sprintf(buffer, "Temperature: %.2f C\r\n", temperature);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void HandleMAX31865Faults(void) {
    uint8_t fault = MAX31865_ReadFault();

    if (fault) {
        if (fault & (1 << 7)) {
            // Handle Overvoltage/Undervoltage fault
            // Example: Print error message or log fault
        }
        if (fault & (1 << 6)) {
            // Handle RTDIN- > 0.85 x VBIAS
            // Example: Check RTD connections
        }
        if (fault & (1 << 5)) {
            // Handle REFIN- > 0.85 x VBIAS
            // Example: Check reference voltage
        }
        if (fault & (1 << 4)) {
            // Handle RTD Low Threshold fault
            // Example: Check for RTD shorted
        }
        if (fault & (1 << 3)) {
            // Handle RTD High Threshold fault
            // Example: Check for RTD open circuit
        }

        // After handling, clear the faults
        MAX31865_WriteRegister(MAX31865_CONFIG_REG, 0x02); // Write to Fault Status Clear bit
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  MAX31865_Init();
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HandleMAX31865Faults();
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  float temperature = MAX31865_ReadTemperature();
	  PrintTemperature(temperature);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
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
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
  hspi1.Init.NSS = SPI_NSS_HARD_OUTPUT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
