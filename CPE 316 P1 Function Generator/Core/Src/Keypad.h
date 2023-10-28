#ifndef SRC_KEYPAD_H_
#define SRC_KEYPAD_H_
#include <stdint.h>
#include "main.h"

class Keypad {
public:
	Keypad();
	void Keypad_init();
	char tick();
	void update_LEDs(char key);
	char findButtonPressed();

	char lastKey = '\0';

private:
	static const uint8_t ROWS = 4;  // Four rows
	static const uint8_t COLS = 3;  // Three columns

	char keys[ROWS][COLS] = {
	    {'1', '2', '3'},
	    {'4', '5', '6'},
	    {'7', '8', '9'},
	    {'*', '0', '#'}
	};

	uint8_t rowPins[ROWS] = {10, 3, 5, 4};  // PB10, PB3, PB5, PB4
	uint8_t colPins[COLS] = {0 , 12, 1};  // PA0, PA4, PA1
	volatile uint8_t keystates[ROWS * COLS] = {0};
};

#endif

