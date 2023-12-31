#include "main.h"
#include <stdint.h>

#ifndef SRC_EXTERNALDAC_H_
#define SRC_EXTERNALDAC_H_

extern const uint16_t sine_wave[720];
extern const uint16_t square_wave[720];
extern const uint16_t sawtooth_wave[720];
extern const uint16_t triangle_wave[720];

class External_DAC {
public:
	External_DAC();
	virtual ~External_DAC();
	void DAC_init();
	void DAC_write(uint16_t value_A, uint16_t value_B);
	uint16_t DAC_mvolt_convert(float mvoltage);
};

#endif
