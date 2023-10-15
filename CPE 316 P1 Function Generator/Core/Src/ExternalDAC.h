#include "main.h"
#include <stdint.h>

#ifndef SRC_EXTERNALDAC_H_
#define SRC_EXTERNALDAC_H_

extern const uint16_t sine_wave[256];
extern const uint16_t square_wave[256];
extern const uint16_t sawtooth_wave[256];
extern const uint16_t triangle_wave[256];

class External_DAC {
public:
	External_DAC();
	virtual ~External_DAC();
	void DAC_init();
	void DAC_write(uint16_t value);
	uint16_t DAC_mvolt_convert(float mvoltage);
};

#endif