#include "main.h"
#include <stdint.h>

#ifndef SRC_EXTERNALDAC_H_
#define SRC_EXTERNALDAC_H_

extern const uint16_t sine_wave_100Hz[800];
extern const uint16_t square_wave_100Hz[800];
extern const uint16_t sawtooth_wave_100Hz[800];
extern const uint16_t triangle_wave_100Hz[800];

extern const uint16_t sine_wave_200Hz[400];
extern const uint16_t square_wave_200Hz[400];
extern const uint16_t sawtooth_wave_200Hz[400];
extern const uint16_t triangle_wave_200Hz[400];

extern const uint16_t sine_wave_300Hz[267];
extern const uint16_t square_wave_300Hz[267];
extern const uint16_t sawtooth_wave_300Hz[267];
extern const uint16_t triangle_wave_300Hz[267];

extern const uint16_t sine_wave_400Hz[200];
extern const uint16_t square_wave_400Hz[200];
extern const uint16_t sawtooth_wave_400Hz[200];
extern const uint16_t triangle_wave_400Hz[200];

extern const uint16_t sine_wave_500Hz[160];
extern const uint16_t square_wave_500Hz[160];
extern const uint16_t sawtooth_wave_500Hz[160];
extern const uint16_t triangle_wave_500Hz[160];

class External_DAC {
public:
	External_DAC();
	virtual ~External_DAC();
	void DAC_init();
	void DAC_write(uint16_t value_A, uint16_t value_B);
	uint16_t DAC_mvolt_convert(float mvoltage);
};

#endif
