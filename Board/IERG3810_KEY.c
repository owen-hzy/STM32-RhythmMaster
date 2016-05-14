#include "IERG3810_KEY.h"

void IERG3810_KEY_Init(void) 
{
	// Init KEY_UP
	GPIOA->CRL &= 0xFFFFFFF0;
	GPIOA->CRL |= 0x00000008;
	GPIOA->ODR &= 0xFFFFFFFE;
	
	// Init KEY0
	GPIOE->CRL &= 0xFFF0FFFF;
	GPIOE->CRL |= 0x00080000;
	GPIOE->ODR |= 1 << 4;
	
	// Init KEY1
	GPIOE->CRL &= 0xFFFF0FFF;
	GPIOE->CRL |= 0x00008000;
	GPIOE->ODR |= 1 << 3;
	
	// Init KEY2
	GPIOE->CRL &= 0xFFFFF0FF;
	GPIOE->CRL |= 0x00000800;
	GPIOE->ODR |= 1 << 2;
}
