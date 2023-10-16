#include "main.h"
#include "ExternalDAC.h"
#include "Keypad.h"
#include <stdio.h>

void SystemClock_Config(void);
void TIM2_init();
extern "C" void TIM2_IRQHandler(void);

volatile uint8_t flagA2outpu = 0;
uint8_t dataToTransmit =0;

#define ARRAY_SIZE 256

External_DAC dac;
Keypad keys;

int waveform = 3;  // 0 for square, 1 for sine, 2 for triangle, 3 for sawtooth
int frequency = 1000;  // Frequency in Hz
int i = 0;  // Index for waveform array
int dutyCycle = 50;  // Duty cycle for square wave

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}


// Initialize TIM2 for 16 µs interval
void TIM2_init() {
	RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
	TIM2->PSC = 0;
	TIM2->ARR = 400;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
}

//extern "C" void SPI_IRQHandler(void)
//{
//	//Load data for transmission
//	SPI1->DR ()
//}

/*extern "C" void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        keys.tick();
        char key = keys.findButtonPressed();

        uint16_t value_A, value_B;
        int modulo_value = 800;  // Default for 100Hz

        // Update waveform and frequency based on keypad input
        if (key >= '1' && key <= '5') {
            frequency = (key - '0') * 100;
            switch (frequency) {
                case 100: modulo_value = 800; break;
                case 200: modulo_value = 400; break;
                case 300: modulo_value = 267; break;
                case 400: modulo_value = 200; break;
                case 500: modulo_value = 160; break;
            }
        } else if (key >= '6' && key <= '9') {
            waveform = key - '6';
        } else if (key == '*') {
            dutyCycle = max(10, dutyCycle - 10);
        } else if (key == '#') {
            dutyCycle = min(90, dutyCycle + 10);
        } else if (key == '0') {
            dutyCycle = 50;
        }

        int wave_index = ((frequency / 100) - 1) * 4 + waveform;

        const uint16_t *waveform_arrays[] = {
		    sine_wave_100Hz, triangle_wave_100Hz, sawtooth_wave_100Hz, square_wave_100Hz,
			sine_wave_200Hz, triangle_wave_200Hz, sawtooth_wave_200Hz, square_wave_200Hz,
			sine_wave_300Hz, triangle_wave_300Hz, sawtooth_wave_300Hz, square_wave_300Hz,
			sine_wave_400Hz, triangle_wave_400Hz, sawtooth_wave_400Hz, square_wave_400Hz,
			sine_wave_500Hz, triangle_wave_500Hz, sawtooth_wave_500Hz, square_wave_500Hz,
	   };

        // Select the appropriate waveform array based on the frequency and waveform type
        const uint16_t *current_wave = waveform_arrays[wave_index];

        // Get values for both DACs
        value_A = current_wave[i];
        value_B = current_wave[i + 1];

        // Write to both DACs
        dac.DAC_write(value_A, value_B);

        // Update index based on frequency
        i = (i + 2) % modulo_value;
    }
}*/

extern "C" void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        keys.tick();
        char key = keys.findButtonPressed();

        uint16_t value_A, value_B;
        int modulo_value = 800;  // Default for 100Hz
        const uint16_t *current_wave = nullptr;

        // Update waveform and frequency based on keypad input
        if (key >= '1' && key <= '5') {
            frequency = (key - '0') * 100;
            switch (frequency) {
                case 100: modulo_value = 800; break;
                case 200: modulo_value = 400; break;
                case 300: modulo_value = 267; break;
                case 400: modulo_value = 200; break;
                case 500: modulo_value = 160; break;
            }
        }

        if (key >= '6' && key <= '9') {
            waveform = key - '6';
        }

        switch (frequency) {
            case 100:
                switch (waveform) {
                    case 0: current_wave = sine_wave_100Hz; break;
                    case 1: current_wave = triangle_wave_100Hz; break;
                    case 2: current_wave = sawtooth_wave_100Hz; break;
                    case 3: current_wave = square_wave_100Hz; break;
                }
                break;
            case 200:
            	switch (waveform) {
					case 0: current_wave = sine_wave_200Hz; break;
					case 1: current_wave = triangle_wave_200Hz; break;
					case 2: current_wave = sawtooth_wave_200Hz; break;
					case 3: current_wave = square_wave_200Hz; break;
				}
				break;
			case 300:
				switch (waveform) {
					case 0: current_wave = sine_wave_300Hz; break;
					case 1: current_wave = triangle_wave_300Hz; break;
					case 2: current_wave = sawtooth_wave_300Hz; break;
					case 3: current_wave = square_wave_300Hz; break;
				}
				break;
			case 400:
				switch (waveform) {
					case 0: current_wave = sine_wave_400Hz; break;
					case 1: current_wave = triangle_wave_400Hz; break;
					case 2: current_wave = sawtooth_wave_400Hz; break;
					case 3: current_wave = square_wave_400Hz; break;
				}
				break;
			case 500:
				switch (waveform) {
					case 0: current_wave = sine_wave_500Hz; break;
					case 1: current_wave = triangle_wave_500Hz; break;
					case 2: current_wave = sawtooth_wave_500Hz; break;
					case 3: current_wave = square_wave_500Hz; break;
				}
			break;
        }

        if (current_wave) {
            value_A = current_wave[i];
            value_B = current_wave[i + 1];
            dac.DAC_write(value_A, value_B);
        }

        // Update index based on frequency
        i = (i + 2) % modulo_value;
    }
}


/*
extern "C" void TIM2_IRQHandler(void)
{
    if (TIM2->SR & TIM_SR_UIF)
    {
        TIM2->SR &= ~TIM_SR_UIF;
        keys.tick();
        char key = keys.findButtonPressed();

        uint16_t value_A, value_B;
        int modulo_value = 800;  // Default for 100Hz

        // Update waveform and frequency based on keypad input
        if (key >= '1' && key <= '5') {
            frequency = (key - '0') * 100;
            switch (frequency) {
                case 100:
                    modulo_value = 800;
                    break;
                case 200:
                    modulo_value = 400;
                    break;
                case 300:
                    modulo_value = 267;
                    break;
                case 400:
                    modulo_value = 200;
                    break;
                case 500:
                    modulo_value = 160;
                    break;
            }
        } else if (key == '6') {
			waveform = 0;
		} else if (key == '7') {
			waveform = 1;
		} else if (key == '8') {
			waveform = 2;
		} else if (key == '9') {
			waveform = 3;
		} else if (key == '*') {
			dutyCycle = max(10, dutyCycle - 10);
		} else if (key == '#') {
			dutyCycle = min(90, dutyCycle + 10);
		} else if (key == '0') {
			dutyCycle = 50;
		}

        // Normalize frequency to start from 0
        //int normalized_frequency = (frequency / 100) - 1;

        // Calculate index into waveform_arrays
        //int wave = normalized_frequency * 4 + waveform;

        // Array of pointers to waveform arrays
        const uint16_t *waveform_arrays[] = {
            sine_wave_100Hz, triangle_wave_100Hz, sawtooth_wave_100Hz, square_wave_100Hz,
			sine_wave_200Hz, triangle_wave_200Hz, sawtooth_wave_200Hz, square_wave_200Hz,
			sine_wave_300Hz, triangle_wave_300Hz, sawtooth_wave_300Hz, square_wave_300Hz,
			sine_wave_400Hz, triangle_wave_400Hz, sawtooth_wave_400Hz, square_wave_400Hz,
			sine_wave_500Hz, triangle_wave_500Hz, sawtooth_wave_500Hz, square_wave_500Hz,
        };

        // Select the appropriate waveform array based on the frequency and waveform type
        const uint16_t *current_wave = waveform_arrays[waveform];

        // Get values for both DACs
        value_A = current_wave[i];
        value_B = current_wave[i + 1];

        // Write to both DACs
        dac.DAC_write(value_A, value_B);

        // Update index based on frequency
        i = (i + 2) % modulo_value;
    }
}
*/


//
//extern "C" void TIM2_IRQHandler(void)
//{
//	 if (TIM2->SR & TIM_SR_UIF)
//	 {
//	        TIM2->SR &= ~TIM_SR_UIF;
//	        keys.tick();
//	        char key = keys.findButtonPressed();
//
//	        // Update waveform and frequency based on keypad input
//	            if (key >= '1' && key <= '5') {
//	                frequency = (key - '0') * 100;
//	            } else if (key == '6') {
//	                waveform = 0;
//	            } else if (key == '7') {
//	                waveform = 1;
//	            } else if (key == '8') {
//	                waveform = 2;
//	            } else if (key == '9') {
//	                waveform = 3;
//	            } else if (key == '*') {
//	                dutyCycle = max(10, dutyCycle - 10);
//	            } else if (key == '#') {
//	                dutyCycle = min(90, dutyCycle + 10);
//	            } else if (key == '0') {
//	                dutyCycle = 50;
//	            }
//
//	            uint16_t value_A, value_B;
//
//	            switch (waveform) {
//					case 0:
//						value_A = sine_wave_100Hz[i];
//						value_B = sine_wave_100Hz[i + 1];
//						break;
//					case 1:
//						value_A = triangle_wave_100Hz[i];
//						value_B = triangle_wave_100Hz[i + 1];
//						break;
//					case 2:
//						value_A = sawtooth_wave_100Hz[i];
//						value_B = sawtooth_wave_100Hz[i + 1];
//						break;
//					case 3:
//						value_A = square_wave_100Hz[i];
//						value_B = square_wave_100Hz[i + 1];
//						break;
//				}
//
//	                    // Write to both DACs
//	                    dac.DAC_write(value_A, value_B);
//	        // Update index based on frequency
//	        i = (i + 2) % 800;//(1000 * 500 / frequency);  // Assuming waveform arrays have 256 points and max frequency is 500 Hz
//	 }
//}

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
		//testWaveforms(dac);
		//keys.tick();
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

