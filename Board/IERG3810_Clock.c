#include "IERG3810_Clock.h"

void IERG3810_clock_init(void)
{
	// Enable GPIOA clock
	RCC->APB2ENR |= 1 << 2;
	// Enable GPIOB clock
	RCC->APB2ENR |= 1 << 3;
	// Enable GPIOE clock
	RCC->APB2ENR |= 1 << 6;
	// Enable GPIOC clock
	RCC->APB2ENR |= 1 << 4;
}

void IERG3810_clock_tree_init(void)
{
	u8 PLL = 0x07;   // This will be 00000111, later will be used to set the PLL multiplication factor
	uint8_t temp = 0x00;
	RCC->CFGR &= 0xF8FF0000; // MCO[2:0] = 000, Clear Microcontroller clock output which will be not used in this lab, and reset ADC prescaler, APB1/2 high-spped-prescaler, AHB prescaler, SWS and SW register
	RCC->CR &= 0xFEF6FFFF; // PLLON = 0, CSSON = 0, HSEON = 0. Turn off clock before configuration
	RCC->CR |= 0x00010000; // HSEON = 1
	while (! (RCC->CR >> 17)); // Check HSERDY, wait until the external high-speed clock ready
	RCC->CFGR = 0x00000400; // PPRE1 = 100, HCLK divided by 2 to get APB1 clock, since HCLK is 72MHZ, so we get APB1 is 36MHZ
							// PPRE2 = 000, HCLK not divided to get APB2 clock which is 72MHZ
	RCC->CFGR |= PLL << 18; // PLLMUL[3:0] = 0111, PLL input clock * 9, since the HSE is 8MHZ, 8 * 9 = 72MHZ
	RCC->CFGR |= 1 << 16; // PLLSRC = 1, HSE oscillator clock selected as PLL input clock
	FLASH->ACR |= 0x32;  // set FLASH with 2 wait states, two wait states, if 48MHZ < SYSCLK <=72MHZ. 
						//Since the system clock is not default, so we need to tell the access time of flash memory, because the program(instruction and data) is read from flash
	RCC->CR |= 0x01000000; // PLLON = 1, after configuration, we can enable clock now
	while (! (RCC->CR >> 25)); // check PLLRDY, locked or not, wait until clock is stable
	RCC->CFGR |= 0x00000002; // SW = 10, Select PLL as system clock
	while (temp != 0x02) // check SWS, whether PLL has been used as system clock
	{
		temp = RCC->CFGR >> 2;
		temp &= 0x03; // clear the first 6 bits, only look at last 2 bits
	}
}

void Delay(u32 count)
{
	u32 i;
	for (i = 0; i < count; i++);
}
