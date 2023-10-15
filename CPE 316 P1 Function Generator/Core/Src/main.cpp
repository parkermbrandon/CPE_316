#include "main.h"
#include "ExternalDAC.h"
#include "Keypad.h"

void SystemClock_Config(void);
void testWaveforms(External_DAC& dac);

void testWaveforms(External_DAC& dac) {
    int arraySize = 256;  // Replace with the actual size of your waveform arrays
    int delayTime = 1;  // Delay time in milliseconds, adjust as needed

    // Test Sine Wave
    for(int i = 0; i < 3; i++){
    for (int i = 0; i < arraySize; ++i) {
        dac.DAC_write(sine_wave[i]);
        HAL_Delay(delayTime);
    }
    }

    // Test Square Wave
    for(int i = 0; i < 3; i++){
    for (int i = 0; i < arraySize; ++i) {
    	dac.DAC_write(square_wave[i]);
        HAL_Delay(delayTime);
    }
    }

    // Test Triangle Wave
    for(int i = 0; i < 3; i++){
    for (int i = 0; i < arraySize; ++i) {
    	dac.DAC_write(triangle_wave[i]);
        HAL_Delay(delayTime);
    }
    }

    // Test Sawtooth Wave
    for(int i = 0; i < 3; i++){
    for (int i = 0; i < arraySize; ++i) {
    	dac.DAC_write(sawtooth_wave[i]);
        HAL_Delay(delayTime);
    }
    }
}

int main()
{

	HAL_Init();

	Keypad keys;
	keys.Keypad_init();

	External_DAC dac;
	dac.DAC_init();

	while(1)
	{
		testWaveforms(dac);
		keys.tick();
	}

	return 0;
}
