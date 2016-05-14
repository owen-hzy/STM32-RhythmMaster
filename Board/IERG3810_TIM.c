#include "IERG3810_TIM.h"

void IERG3810_TIM3_Init(u16 arr, u16 psc)
{
	// TIM3, IRQ #29
	RCC->APB1ENR |= 1 << 1; // TIM3EN; TIM3 clock enabled
	TIM3->ARR = arr;
	TIM3->PSC = psc;
	TIM3->DIER |= 1 << 0;	// UIE; Update interrupt enabled
	TIM3->CR1 |= 0x01;	// CEN = 1; Counter enabled
	NVIC->IP[29] = 0x45;	// Set the priority for IRQ #29
	NVIC->ISER[0] |= (1 << 29);	// Enabled IRQ #29
}

void IERG3810_TIM4_Init(u16 arr, u16 psc)
{
	// TIM4, IRQ #30
	RCC->APB1ENR |= 1 << 2;	// TIM4EN; TIM4 clock enabled
	TIM4->ARR = arr;
	TIM4->PSC = psc;
	TIM4->DIER |= 1 << 0;
	TIM4->CR1 |= 0x01;
	NVIC->IP[30] = 0x45;
	NVIC->ISER[0] |= (1 << 30); // Enabled IRQ #30
}

void IERG3810_SYSTICK_Init10ms(void)
{
	SysTick->CTRL = 0;	// Clear Systick control and status register; e.g. reset the register
	SysTick->LOAD = 90000; // Since 10ms is 100 HZ, and select STCLK is 9MHZ
	SysTick->CTRL |= 0x03;	// TICKINT = 1 to enable the systick interrupt; enable counter operates in a multi-shot way
}

void IERG3810_TIM3_PwmInit(u16 arr, u16 psc)
{
	RCC->APB2ENR |= 1 << 3;		// IOPB EN = 1; IO port B clock enabled
	GPIOB->CRL &= 0xFF0FFFFF;
	GPIOB->CRL |= 0x00B00000;	// Alternate function output push-pull
	RCC->APB2ENR |= 1 << 0;		// AFIOEN; alternate function IO clock enabled
	AFIO->MAPR &= 0xFFFFF3FF;	// Clear TIM3_REMAP
	AFIO->MAPR |= 1 << 11;
	RCC->APB1ENR |= 1 << 1;		// TIM3EN = 1; TIM3 clock enabled
	TIM3->ARR = arr;
	TIM3->PSC = psc;
	TIM3->CCMR1 |= 7 << 12;		// CC2S = 00 means the CH2 is configured as output, and enable the output compare 2 mode
	TIM3->CCMR1 |= 1 << 11;		// OC2PE = 1; output compare 2 preload enabled	
	TIM3->CCER |= 1 << 4;		// Capture/Compare 2 output enabled
	TIM3->CR1 = 0x0080;		// TIMx_ARR register is buffered
	TIM3->CR1 |= 0x01;		// Counter enabled
	
	//***** Exp 5.7 *****
//	RCC->APB2ENR |= 1 << 4;
//	GPIOC->CRL &= 0x00FFFFFF;
//	GPIOC->CRL |= 0xBB000000;
//	GPIOC->CRH &= 0xFFFFFFF0;
//	GPIOC->CRH |= 0x0000000B;
//	RCC->APB2ENR |= 1 << 0;		// AFIOEN; alternate function IO clock enabled
//	AFIO->MAPR &= 0xFFFFF3FF;	// Clear TIM3_REMAP
//	AFIO->MAPR |= 3 << 10;		// Remap = 11
//	RCC->APB1ENR |= 1 << 1;		// TIM3EN = 1; TIM3 clock enabled
//	TIM3->ARR = arr;
//	TIM3->PSC = psc;
//	TIM3->CCMR1 |= 7 << 4;
//	TIM3->CCMR1 |= 7 << 12;		// CC2S = 00 means the CH2 is configured as output, and enable the output compare 2 mode
//	TIM3->CCMR2 |= 7 << 4;
//	TIM3->CCMR1 |= 1 << 3;
//	TIM3->CCMR1 |= 1 << 11;		// OC2PE = 1; output compare 2 preload enabled	
//	TIM3->CCMR2 |= 1 << 3;
//	TIM3->CCER |= 1 << 0;
//	TIM3->CCER |= 1 << 4;		// Capture/Compare 2 output enabled
//	TIM3->CCER |= 1 << 8;
//	TIM3->CR1 = 0x0080;		// TIMx_ARR register is buffered
//	TIM3->DIER |= 1 << 0;	// UIE; Update interrupt enabled
//	TIM3->CR1 |= 0x01;		// Counter enabled
//	NVIC->IP[29] = 0x45;	// Set the priority for IRQ #29
//	NVIC->ISER[0] |= (1 << 29);	// Enabled IRQ #29
	//***** End of 5.7 *****
}

void IERG3810_TIM4_PwmInit(u16 arr, u16 psc)
{
	RCC->APB2ENR |= 1 << 3;		// IOPB EN = 1; IO port B clock enabled
	GPIOB->CRH &= 0xFFFFFFF0;
	GPIOB->CRH |= 0x0000000B;	// Alternate function output push-pull
	RCC->APB2ENR |= 1 << 0;		// AFIOEN; alternate function IO clock enabled
	AFIO->MAPR &= 0xFFFFEFFF;	// Clear TIM4_REMAP, TIM4_CH3 / PB8
	RCC->APB1ENR |= 1 << 2;		// TIM4EN = 1; TIM4 clock enabled
	TIM4->ARR = arr;
	TIM4->PSC = psc;
	TIM4->CCMR2 |= 7 << 4;		// CC3S = 00 means the CH3 is configured as output, and enable the output compare 3 mode
	TIM4->CCMR2 |= 1 << 3;		// OC3PE = 1; output compare 3 preload enabled	
	TIM4->CCER |= 1 << 8;		// Capture/Compare 3 output enabled
	TIM4->CR1 = 0x0080;		// TIMx_ARR register is buffered
	TIM4->CR1 |= 0x01;		// Counter enabled
}
