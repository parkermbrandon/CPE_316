#include "Keypad.h"

const uint8_t ROWS = 4;  // Four rows
const uint8_t COLS = 3;  // Three columns
//uint32_t systemTime = 0;

Keypad::Keypad() {
	for (int i = 0; i < ROWS * COLS; ++i)
	{
		keystates[i] = 0;
	}
}

void Keypad::Keypad_init(void)
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
    for (int c = 0; c < COLS; c++)
    {
        GPIOA->BSRR = (1U << colPins[c]);
    }
}

bool Keypad::hasButtonBeenPressed() {
    for (int i = 0; i < ROWS * COLS; ++i) {
        if (keystates[i]) {
            return true;
        }
    }
    return false;
}

char Keypad::tick() {
  for (int c = 0; c < COLS; c++)
  {
    GPIOA->BSRR = (1U << (colPins[c] + 16));  // sets the current column pin to low
    for (int r = 0; r < ROWS; r++)
    {
      int i = r * COLS + c;
      bool pressed = !(GPIOB->IDR & (1U << rowPins[r])); // This checks if the current row pin is low (i.e., a button on this row has been pressed).

      if (pressed)
      {
        keystates[i] = 1;

      } else {
    	 keystates[i] = 0;
      }
    }

    GPIOA->BSRR = (1U << colPins[c]); // After checking all rows for a given column, the column is set back to high
  }
  findButtonPressed();
}

void Keypad::update_LEDs(char key) {

  // Clear all LEDs
  GPIOB->BSRR = GPIO_BSRR_BR_6;
  GPIOC->BSRR = GPIO_BSRR_BR_7;
  GPIOA->BSRR = GPIO_BSRR_BR_9 | GPIO_BSRR_BR_8;

  int binaryRepresentation = -1;

  if (key >= '0' && key <= '9')
  {
	binaryRepresentation = key - '0';  // Convert char to int
  } else if (key == '*') {
	binaryRepresentation = 14;  // 1110 in binary
  } else if (key == '#') {
	binaryRepresentation = 15;  // 1111 in binary
  }

  if (binaryRepresentation != -1)
  {
	if (binaryRepresentation & 0b0001) GPIOB->BSRR = GPIO_BSRR_BS_6;
	if (binaryRepresentation & 0b0010) GPIOC->BSRR = GPIO_BSRR_BS_7;
	if (binaryRepresentation & 0b0100) GPIOA->BSRR = GPIO_BSRR_BS_9;
	if (binaryRepresentation & 0b1000) GPIOA->BSRR = GPIO_BSRR_BS_8;
  }
}

char Keypad::findButtonPressed()
{
    static char lastKey = '\0';  // Keep track of the last key pressed
    static uint32_t lastDebounceTime = 0;  // the last time the output pin was toggled
    const uint32_t debounceDelay = 500;  // the debounce time in milliseconds

    char currentKey = '\0';  // Initialize to no key pressed

    for (int r = 0; r < ROWS; r++)
    {
        for (int c = 0; c < COLS; c++)
        {
            int i = r * COLS + c;
            if (keystates[i])
            {
                currentKey = keys[r][c];
                break;  // Exit the loop once a key is found
            }
        }
        if (currentKey != '\0')
        {
            break;  // Exit the loop once a key is found
        }
    }

    if (currentKey != '\0' && (systemTime - lastDebounceTime) > debounceDelay)
        {
            lastKey = currentKey;
            update_LEDs(currentKey);
            lastDebounceTime = systemTime;
        }
        else if (currentKey == '\0')
        {
            lastKey = '\0';
        }

    return lastKey;  // Return the current key if pressed, otherwise the last key
}
