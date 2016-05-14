#include "IERG3810_LED.h"

void IERG3810_LED_Init(void)
{
	// Init DS0(LED0)
	// Enable GPIOB clock
	RCC->APB2ENR |= 1 << 3;
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00300000;
	
	// Init DS1(LED1)
	// Enable GPIOE clock
	RCC->APB2ENR |= 1 << 6;
	GPIOE->CRL &= 0xFF0FFFFF;
	GPIOE->CRL |= 0x00300000;
}
