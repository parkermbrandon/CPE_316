#include "main.h"
#include "ExternalDAC.h"
#include "Keypad.h"
#include <stdio.h>

void SystemClock_Config(void);
void TIM2_init();
extern "C" void TIM2_IRQHandler(void);

External_DAC dac;
Keypad keys;

static int lastFrequency = -1;
static int lastWaveform = -1;
static int i = 0;  // Index for waveform array
static int modulo_value = 720;  // Length of the lookup tables
static int frequency = 100;
static int waveform = 3;  // Default waveform
static int dutyCycle = 50;  // Default duty cycle
static bool keyWasPressed = false;
static bool anyKeyPressed = false;
uint32_t systemTime = 0;
int stride_length = 1;

// Initialize TIM2
void TIM2_init() {
    // Enable clock for TIM2
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    // Set the prescaler to 1
    TIM2->PSC = 1;
    // Set the Auto-Reload Register (ARR) value to 1114
    TIM2->ARR = 1114;
    // Enable Update Interrupt (UIE)
    TIM2->DIER |= TIM_DIER_UIE;
    // Enable the counter (CEN)
    TIM2->CR1 |= TIM_CR1_CEN;
    // Enable the TIM2 interrupt in the NVIC
    NVIC_EnableIRQ(TIM2_IRQn);
}


void updateFrequency(char key)
{
    if (key >= '1' && key <= '5')
    {
        frequency = (key - '0') * 100;
        switch (frequency)
        {
            case 100: stride_length = 1;  break;
            case 200: stride_length = 2;  break;
            case 300: stride_length = 3;  break;
            case 400: stride_length = 4;  break;
            case 500: stride_length = 5;  break;
        }
    }
}

void updateWaveform(char key)
{
    if (key >= '6' && key <= '9')
    {
        waveform = key - '6';
    }
}

void updateDutyCycle(char key)
{
	if (key == '*' || key == '#' || key == '0')
	{
	    anyKeyPressed = true;
	    if (!keyWasPressed)
	    {
	        if (key == '*')
	        {
	            dutyCycle = (dutyCycle <= 10) ? 10 : (dutyCycle - 10);
	        }
	        else if (key == '#')
	        {
	            dutyCycle = (dutyCycle >= 90) ? 90 : (dutyCycle + 10);
	        }
	        else if (key == '0')
	        {
	            dutyCycle = 50;
	        }
	        keyWasPressed = true;
	    }
	}
	else
	{
	    if (!anyKeyPressed)
	    {
	        keyWasPressed = false;
	    }
	    anyKeyPressed = false;
	}
}

bool shouldResetWaveform()
{
    if (lastWaveform != waveform || lastFrequency != frequency)
    {
        lastWaveform = waveform;
        lastFrequency = frequency;
        return true;
    }
    return false;
}

const uint16_t* getCurrentWave()
{
    switch (waveform)
    {
        case 0: return sine_wave; break;
        case 1: return triangle_wave; break;
        case 2: return sawtooth_wave; break;
        case 3: return square_wave; break;
    }
    return nullptr;
}

extern "C" void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        systemTime++;

        char key = keys.tick();

		updateFrequency(key);
		updateWaveform(key);
		updateDutyCycle(key);

		if (shouldResetWaveform())
		{
			i = 0;
		}

		uint16_t value_A, value_B;
		const uint16_t *current_wave = getCurrentWave();

		if (current_wave)
		{
			if (waveform == 3) // If waveform is square update duty cycle
			{
				int high_time = (dutyCycle * modulo_value) / 100;
				// Value of 3723 is 12 bit value that will output 3V from the DAC
				value_A = (i < high_time) ? 3723 : 0;
				value_B = value_A;
			} else {
				value_A = current_wave[i];
				value_B = current_wave[i + 1];
			}

			dac.DAC_write(value_A, value_B);
		}

	   i = (i + stride_length * 2) % modulo_value;
    }
}

int main()
{
	HAL_Init();
	SystemClock_Config();
	__enable_irq();
	TIM2_init();
	keys.Keypad_init();
	dac.DAC_init();

	while(1)
	{
	// Everything is handled by ISR
	}

	return 0;
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
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

