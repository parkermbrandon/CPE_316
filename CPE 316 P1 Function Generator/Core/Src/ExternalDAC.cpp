#include "ExternalDAC.h"

extern const uint16_t sine_wave[256] = {
    1861, 1907, 1952, 1998, 2043, 2089, 2134, 2179, 2224, 2269, 2313, 2357, 2401, 2445, 2488, 2531,
    2573, 2615, 2657, 2698, 2738, 2778, 2818, 2857, 2895, 2933, 2970, 3006, 3042, 3077, 3111, 3144,
    3177, 3209, 3240, 3270, 3300, 3328, 3356, 3383, 3408, 3433, 3457, 3480, 3502, 3523, 3543, 3562,
    3580, 3597, 3613, 3628, 3642, 3655, 3666, 3677, 3686, 3695, 3702, 3708, 3713, 3717, 3720, 3721,
    3722, 3721, 3720, 3717, 3713, 3708, 3702, 3695, 3686, 3677, 3666, 3655, 3642, 3628, 3613, 3597,
    3580, 3562, 3543, 3523, 3502, 3480, 3457, 3433, 3408, 3383, 3356, 3328, 3300, 3270, 3240, 3209,
    3177, 3144, 3111, 3077, 3042, 3006, 2970, 2933, 2895, 2857, 2818, 2778, 2738, 2698, 2657, 2615,
    2573, 2531, 2488, 2445, 2401, 2357, 2313, 2269, 2224, 2179, 2134, 2089, 2043, 1998, 1952, 1907,
    1861, 1815, 1770, 1724, 1679, 1633, 1588, 1543, 1498, 1453, 1409, 1365, 1321, 1277, 1234, 1191,
    1149, 1107, 1065, 1024, 984, 944, 904, 865, 827, 789, 752, 716, 680, 645, 611, 578,
    545, 513, 482, 452, 422, 394, 366, 339, 314, 289, 265, 242, 220, 199, 179, 160,
    142, 125, 109, 94, 80, 67, 56, 45, 36, 27, 20, 14, 9, 5, 2, 1,
    0, 1, 2, 5, 9, 14, 20, 27, 36, 45, 56, 67, 80, 94, 109, 125,
    142, 160, 179, 199, 220, 242, 265, 289, 314, 339, 366, 394, 422, 452, 482, 513,
    545, 578, 611, 645, 680, 716, 752, 789, 827, 865, 904, 944, 984, 1024, 1065, 1107,
    1149, 1191, 1234, 1277, 1321, 1365, 1409, 1453, 1498, 1543, 1588, 1633, 1679, 1724, 1770, 1815,
};

extern const uint16_t square_wave[256] = {
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722, 3722,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
    1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861, 1861,
};

extern const uint16_t sawtooth_wave[256] = {
    1861, 1868, 1876, 1883, 1890, 1897, 1905, 1912, 1919, 1926, 1934, 1941, 1948, 1956, 1963, 1970,
    1977, 1985, 1992, 1999, 2006, 2014, 2021, 2028, 2035, 2043, 2050, 2057, 2065, 2072, 2079, 2086,
    2094, 2101, 2108, 2115, 2123, 2130, 2137, 2145, 2152, 2159, 2166, 2174, 2181, 2188, 2195, 2203,
    2210, 2217, 2224, 2232, 2239, 2246, 2254, 2261, 2268, 2275, 2283, 2290, 2297, 2304, 2312, 2319,
    2326, 2334, 2341, 2348, 2355, 2363, 2370, 2377, 2384, 2392, 2399, 2406, 2413, 2421, 2428, 2435,
    2443, 2450, 2457, 2464, 2472, 2479, 2486, 2493, 2501, 2508, 2515, 2523, 2530, 2537, 2544, 2552,
    2559, 2566, 2573, 2581, 2588, 2595, 2602, 2610, 2617, 2624, 2632, 2639, 2646, 2653, 2661, 2668,
    2675, 2682, 2690, 2697, 2704, 2712, 2719, 2726, 2733, 2741, 2748, 2755, 2762, 2770, 2777, 2784,
    2792, 2799, 2806, 2813, 2821, 2828, 2835, 2842, 2850, 2857, 2864, 2871, 2879, 2886, 2893, 2901,
    2908, 2915, 2922, 2930, 2937, 2944, 2951, 2959, 2966, 2973, 2981, 2988, 2995, 3002, 3010, 3017,
    3024, 3031, 3039, 3046, 3053, 3060, 3068, 3075, 3082, 3090, 3097, 3104, 3111, 3119, 3126, 3133,
    3140, 3148, 3155, 3162, 3170, 3177, 3184, 3191, 3199, 3206, 3213, 3220, 3228, 3235, 3242, 3249,
    3257, 3264, 3271, 3279, 3286, 3293, 3300, 3308, 3315, 3322, 3329, 3337, 3344, 3351, 3359, 3366,
    3373, 3380, 3388, 3395, 3402, 3409, 3417, 3424, 3431, 3438, 3446, 3453, 3460, 3468, 3475, 3482,
    3489, 3497, 3504, 3511, 3518, 3526, 3533, 3540, 3548, 3555, 3562, 3569, 3577, 3584, 3591, 3598,
    3606, 3613, 3620, 3627, 3635, 3642, 3649, 3657, 3664, 3671, 3678, 3686, 3693, 3700, 3707, 3715,
};

extern const uint16_t triangle_wave[256] = {
    1861, 1876, 1890, 1905, 1919, 1934, 1948, 1963, 1977, 1992, 2006, 2021, 2035, 2050, 2065, 2079,
    2094, 2108, 2123, 2137, 2152, 2166, 2181, 2195, 2210, 2224, 2239, 2254, 2268, 2283, 2297, 2312,
    2326, 2341, 2355, 2370, 2384, 2399, 2413, 2428, 2443, 2457, 2472, 2486, 2501, 2515, 2530, 2544,
    2559, 2573, 2588, 2602, 2617, 2632, 2646, 2661, 2675, 2690, 2704, 2719, 2733, 2748, 2762, 2777,
    2792, 2806, 2821, 2835, 2850, 2864, 2879, 2893, 2908, 2922, 2937, 2951, 2966, 2981, 2995, 3010,
    3024, 3039, 3053, 3068, 3082, 3097, 3111, 3126, 3140, 3155, 3170, 3184, 3199, 3213, 3228, 3242,
    3257, 3271, 3286, 3300, 3315, 3329, 3344, 3359, 3373, 3388, 3402, 3417, 3431, 3446, 3460, 3475,
    3489, 3504, 3518, 3533, 3548, 3562, 3577, 3591, 3606, 3620, 3635, 3649, 3664, 3678, 3693, 3707,
    3722, 3707, 3693, 3678, 3664, 3649, 3635, 3620, 3606, 3591, 3577, 3562, 3548, 3533, 3518, 3504,
    3489, 3475, 3460, 3446, 3431, 3417, 3402, 3388, 3373, 3359, 3344, 3329, 3315, 3300, 3286, 3271,
    3257, 3242, 3228, 3213, 3199, 3184, 3170, 3155, 3140, 3126, 3111, 3097, 3082, 3068, 3053, 3039,
    3024, 3010, 2995, 2981, 2966, 2951, 2937, 2922, 2908, 2893, 2879, 2864, 2850, 2835, 2821, 2806,
    2792, 2777, 2762, 2748, 2733, 2719, 2704, 2690, 2675, 2661, 2646, 2632, 2617, 2602, 2588, 2573,
    2559, 2544, 2530, 2515, 2501, 2486, 2472, 2457, 2443, 2428, 2413, 2399, 2384, 2370, 2355, 2341,
    2326, 2312, 2297, 2283, 2268, 2254, 2239, 2224, 2210, 2195, 2181, 2166, 2152, 2137, 2123, 2108,
    2094, 2079, 2065, 2050, 2035, 2021, 2006, 1992, 1977, 1963, 1948, 1934, 1919, 1905, 1890, 1876,
};

External_DAC::External_DAC() {

}

External_DAC::~External_DAC() {
}

void External_DAC::DAC_init(void)
{
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 | GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
    GPIOA->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
    GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5 | GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
    GPIOA->AFR[0] |= ((5 << GPIO_AFRL_AFSEL4_Pos) | (5 << GPIO_AFRL_AFSEL5_Pos) | (5 << GPIO_AFRL_AFSEL6_Pos) | (5 << GPIO_AFRL_AFSEL7_Pos));
    GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 | GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
    GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 | GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
    SPI1->CR1 = SPI_CR1_MSTR;
    SPI1->CR2 = SPI_CR2_SSOE | SPI_CR2_NSSP | (0xF << SPI_CR2_DS_Pos);
    SPI1->CR1 |= SPI_CR1_SPE;
//	  RCC->AHB2ENR |= (RCC_AHB2ENR_GPIOAEN);
//
//	  GPIOA->MODER &= ~(GPIO_MODER_MODE4 | GPIO_MODER_MODE5 |		// mask function
//						GPIO_MODER_MODE6 | GPIO_MODER_MODE7);
//	  GPIOA->MODER |= (GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 |	// enable alternate function
//					   GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1);
//
//	  GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL4 | GPIO_AFRL_AFSEL5 |		// mask AF selection
//						  GPIO_AFRL_AFSEL6 | GPIO_AFRL_AFSEL7);
//	  GPIOA->AFR[0] |= ((5 << GPIO_AFRL_AFSEL4_Pos) |				// select SPI_1 (AF5)
//						(5 << GPIO_AFRL_AFSEL5_Pos) |
//						(5 << GPIO_AFRL_AFSEL6_Pos) |
//						(5 << GPIO_AFRL_AFSEL7_Pos));
//
//	  GPIOA->OTYPER &= ~(GPIO_OTYPER_OT4 | GPIO_OTYPER_OT5 |		// push-pull output
//						  GPIO_OTYPER_OT6 | GPIO_OTYPER_OT7);
//
//	  GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD4 | GPIO_PUPDR_PUPD5 |		// no pull ups or pull downs
//						GPIO_PUPDR_PUPD6 | GPIO_PUPDR_PUPD7);
//
//	  GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED4 | 					// low speed
//						  GPIO_OSPEEDR_OSPEED5 |
//						  GPIO_OSPEEDR_OSPEED6 |
//						  GPIO_OSPEEDR_OSPEED7);
//
//	  // configure SPI 1
//	  RCC->APB2ENR |= (RCC_APB2ENR_SPI1EN);		// enable SPI1 clock
//	  SPI1->CR1 = (SPI_CR1_MSTR);				// enable master mode, fck/2, hardware CS, MSB first, full duplex
//	  SPI1->CR2 = (SPI_CR2_SSOE |				// enable CS output
//				   SPI_CR2_NSSP |				// create CS pulse
//				   //SPI_CR2_TXEIE |				// Tx buffer empty interrupt enable
//				   (0xF << SPI_CR2_DS_Pos));	// 16-bit data frames
//	  SPI1->CR1 |= (SPI_CR1_SPE);				// enable SPI
}

// Write a 12-bit value to the DAC
void External_DAC::DAC_write(uint16_t value)
{
    uint16_t spi_data = (value & 0x0FFF) | 0x3000;  // 12-bit value, OR 0x3000 sets the Gain and buffer in the DAC see page 18 of datasheet
    while(!(SPI1->SR & SPI_SR_TXE));  // Wait for TXE (Transmit buffer empty)
    SPI1->DR = spi_data;  // Send the data
}

// Convert a voltage value into a 12-bit value to control the DAC
uint16_t External_DAC::DAC_mvolt_convert(float mvoltage)
{
	return (mvoltage * 4095) / 3300;
}

/* Matlab script for generating lookup tables
% Parameters
fs = 256;  % Sampling frequency
t = 0:1/fs:1-1/fs;  % Time vector
Vref = 3.3;  % Reference voltage
bit_depth = 4095;  % 12-bit DAC
dc_offset = 1.5;  % DC offset in volts
Vpp = 3.0;  % Peak-to-peak voltage in volts

% Scaling factors
scale_factor = bit_depth / Vref;
dc_offset_scaled = round(dc_offset * scale_factor);
amplitude_scaled = round((Vpp / 2) * scale_factor);

% Generate Sine Wave
sine_wave = round(dc_offset_scaled + amplitude_scaled * sin(2 * pi * t));

% Generate Square Wave
square_wave = round(dc_offset_scaled + amplitude_scaled * (square(2 * pi * t) + 1) / 2);

% Generate Triangle Wave
triangle_wave = round(dc_offset_scaled + amplitude_scaled * (sawtooth(2 * pi * t, 0.5) + 1) / 2);

% Generate Sawtooth Wave
sawtooth_wave = round(dc_offset_scaled + amplitude_scaled * (sawtooth(2 * pi * t) + 1) / 2);

% Print Lookup Tables
print_lookup_table('sine_wave', sine_wave);
print_lookup_table('square_wave', square_wave);
print_lookup_table('sawtooth_wave', sawtooth_wave);
print_lookup_table('triangle_wave', triangle_wave);

% Function to print lookup table in 16x16 format
function print_lookup_table(name, data)
    fprintf('const uint16_t %s[%d] = {\n', name, length(data));
    for i = 1:16:length(data)
        fprintf('    ');
        fprintf('%d, ', data(i:min(i+15, end)));
        fprintf('\n');
    end
    fprintf('};\n');
end
 */



