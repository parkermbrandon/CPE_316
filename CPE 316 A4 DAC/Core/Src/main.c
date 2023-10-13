#include "main.h"

 void DAC_init(void)
 {
	  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);

	  GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |		// mask function
						GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
	  GPIOA->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 |	// enable alternate function
					   GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);

	  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5 |		// mask AF selection
						  GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
	  GPIOA->AFR[0] |= ((5 << GPIO_AFRL_AFSEL4_Pos) |				// select SPI_1 (AF5)
						(5 << GPIO_AFRL_AFSEL5_Pos) |
						(5 << GPIO_AFRL_AFSEL6_Pos) |
						(5 << GPIO_AFRL_AFSEL7_Pos));

	  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |		// push-pull output
						  GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);

	  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |		// no pull ups or pull downs
						GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);

	  GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED4 | 					// low speed
						  GPIO_OSPEEDR_OSPEED5 |
						  GPIO_OSPEEDR_OSPEED6 |
						  GPIO_OSPEEDR_OSPEED7);

	  // configure SPI 1
	  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);		// enable SPI1 clock
	  SPI1->CR1 = (SPI_CR1_MSTR);				// enable master mode, fck/2, hardware CS, MSB first, full duplex
	  SPI1->CR2 = (SPI_CR2_SSOE |				// enable CS output
				   SPI_CR2_NSSP |				// create CS pulse
				   (0xF << SPI_CR2_DS_Pos));	// 16-bit data frames
	  SPI1->CR1 |= (SPI_CR1_SPE);				// enable SPI
 }

 // Write a 12-bit value to the DAC
 void DAC_write(uint16_t value) {
     uint16_t spi_data = (value & 0x0FFF) | 0x3000;  // 12-bit value, OR 0x3000 sets the Gain and buffer in the DAC see page 18 of datasheet
     while(!(SPI1->SR & SPI_SR_TXE));  // Wait for TXE (Transmit buffer empty)
     SPI1->DR = spi_data;  // Send the data
 }

// Convert a voltage value into a 12-bit value to control the DAC
uint16_t DAC_mvolt_conv(float mvoltage) {
	return (mvoltage * 4095) / 3300;
}

int main(void)
{

  HAL_Init();
  DAC_init();

  uint16_t low_val = DAC_mvolt_conv(1000);
  uint16_t high_val = DAC_mvolt_conv(2000);

  //uint16_t dac_value = 0x6A7;

while (1)
  {
	 for (int i = 0; i < 100; ++i)
	 {
		if (i < 25)
		{
			DAC_write(high_val);
		} else
		{
			DAC_write(low_val);
		}
		HAL_Delay(1);
	 }
	 //DAC_write(dac_value);
  }
}
