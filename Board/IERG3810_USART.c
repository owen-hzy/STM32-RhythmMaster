#include "IERG3810_USART.h"

void IERG3810_USART2_init(u32 pclk1, u32 bound)
{
	// USART2 connects to APB1, at top side of project board
	float temp;
	u16 mantissa;
	u16 fraction;
	temp = (float) (pclk1 * 1000000) / (bound * 16); //pclk1 is MHZ, so we multiply by 1000000, check RM0008 page770
	mantissa = temp; // only get the integer part
	fraction = (temp - mantissa) * 16; // float part multiple by 16, to get the integer representation
	mantissa <<= 4;
	mantissa += fraction;
	RCC->APB2ENR |= 1 << 2; // enable GPIOA clock, since we need PA2 and PA3
	RCC->APB1ENR |= 1 << 17; // USART2EN = 1, enable USART2 clock
	GPIOA->CRL &= 0xFFFF00FF; // set PA2, PA3 alternate function
	GPIOA->CRL |= 0x00008B00;
	RCC->APB1RSTR |= 1 << 17; // USART2RST = 1, reset USART2
	RCC->APB1RSTR &= ~(1 << 17); // USART2RST = 0, after reset, clear the reset bit, otherwise, keeps resetting
	USART2->BRR = mantissa; // set baud rate
	USART2->CR1 |= 0x2008; // UE = 1, M = 0, TE = 1, enable USART, and set word length to be 8 data bits. Then enable transmitter
							// When TE is set there is a 1 bit-time delay before the transmission starts
}	

void IERG3810_USART1_init(u32 pclk2, u32 bound)
{
	float temp;
	u16 mantissa;
	u16 fraction;
	temp = (float) (pclk2 * 1000000) / (bound * 16);
	mantissa = temp;
	fraction = (temp - mantissa) * 16;
	mantissa <<= 4;
	mantissa += fraction;
	RCC->APB2ENR |= 1 << 2;
	RCC->APB2ENR |= 1 << 14; // USART1EN = 1, enable USART1 clock
	GPIOA->CRH &= 0xFFFFF00F; // set PA9, PA10 alternate function
	GPIOA->CRH |= 0x000008B0;
	RCC->APB2RSTR |= 1 << 14; // USART1RST = 1, reset USART1
	RCC->APB2RSTR &= ~(1 << 14); // USART1RST = 0, clear reset bit
	USART1->BRR = mantissa;
	USART1->CR1 |= 0x2008;
}

void USART_print(u8 USARTport, char *st)
{
	u8 i = 0;
	while (st[i] != 0x00)
	{
		if (USARTport == 0x01)
		{
			USART1->DR = st[i];
			while (! (USART1->SR >> 7));  // wait unitl data has been transfered to the shift register
		}
		if (USARTport == 0x02)
		{
			USART2->DR = st[i];
			while (! (USART2->SR >> 7));
		}
//		Delay(50000);
		if (i == 255) break;
		i++;
	}
}
