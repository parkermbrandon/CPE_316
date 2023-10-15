#include "main.h"
#include "ExternalDAC.h"
#include "Keypad.h"

void SystemClock_Config(void);
void testWaveforms(External_DAC& dac);
void TIM2_init();
extern "C" void TIM2_IRQHandler(void);

volatile uint8_t flagA2outpu = 0;
uint8_t dataToTransmit =0;

#define ARRAY_SIZE 256

External_DAC dac;
Keypad keys;

// Global variables for waveform and frequency
int waveform = 0;  // 0 for square, 1 for sine, 2 for triangle, 3 for sawtooth
int frequency = 100;  // Frequency in Hz
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
	TIM2->ARR = 315;
	TIM2->DIER |= TIM_DIER_UIE;
	TIM2->CR1 |= TIM_CR1_CEN;
	NVIC_EnableIRQ(TIM2_IRQn);
}

//extern "C" void SPI_IRQHandler(void)
//{
//	//Load data for transmission
//	SPI1->DR ()
//}

extern "C" void TIM2_IRQHandler(void)
{
	 if (TIM2->SR & TIM_SR_UIF)
	 {
	        TIM2->SR &= ~TIM_SR_UIF;
	        keys.tick();
	        char key = keys.findButtonPressed();

	        // Update waveform and frequency based on keypad input
	            if (key >= '1' && key <= '5') {
	                frequency = (key - '0') * 100;
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

	        switch (waveform) {
	                case 0:  // Square wave
	                	dac.DAC_write(sine_wave[i]);
	                    break;
	                case 1:  // Sine wave
	                    dac.DAC_write(triangle_wave[i]);
	                    break;
	                case 2:  // Triangle wave
	                    dac.DAC_write(sawtooth_wave[i]);
	                    break;
	                case 3:  // Sawtooth wave
	                    dac.DAC_write(square_wave[i]);
	                    break;
	            }
	        // Update index based on frequency
	        i = (i + 1) % (256 * 500 / frequency);  // Assuming waveform arrays have 256 points and max frequency is 500 Hz
	 }
}

int main()
{

	__enable_irq();
	HAL_Init();
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

void testWaveforms(External_DAC& dac)
{
    int arraySize = 256;  // Replace with the actual size of your waveform arrays
    int delayTime = 1;  // Delay time in milliseconds, adjust as needed

    // Test Sine Wave
    for(int i = 0; i < 3; i++)
    {
		for (int i = 0; i < arraySize; ++i)
		{
			dac.DAC_write(sine_wave[i]);
			HAL_Delay(delayTime);
		}
    }

    // Test Square Wave
    for(int i = 0; i < 3; i++)
    {
		for (int i = 0; i < arraySize; ++i)
		{
			dac.DAC_write(square_wave[i]);
			HAL_Delay(delayTime);
		}
    }

    // Test Triangle Wave
    for(int i = 0; i < 3; i++)
    {
		for (int i = 0; i < arraySize; ++i)
		{
			dac.DAC_write(triangle_wave[i]);
			HAL_Delay(delayTime);
		}
    }

    // Test Sawtooth Wave
    for(int i = 0; i < 3; i++)
    {
		for (int i = 0; i < arraySize; ++i)
		{
			dac.DAC_write(sawtooth_wave[i]);
			HAL_Delay(delayTime);
		}
    }
}
