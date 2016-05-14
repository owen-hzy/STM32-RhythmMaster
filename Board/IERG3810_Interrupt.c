#include "IERG3810_Interrupt.h"

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup) 
{
	// Set PRIGROUP AIRCR[10:8]
	u32 temp, temp1;
	temp1 = prigroup & 0x00000007; // only concern 3 bits
	temp1 <<= 8; // move to the right bits  AIRCR[10:8]
	temp = SCB->AIRCR;
	temp &= 0x0000F8FF;	// clear bit 8 to bit 10
	temp |= 0x05FA0000; // 0x05FA is the default for the reserved bits
	temp |= temp1;
	SCB->AIRCR = temp;
}

void IERG3810_key2_ExtiInit(void)
{
	// KEY2 at PE2, EXTI-2, IRQ #8
	RCC->APB2ENR |= 1 << 6; //Enable I/O port E clock
	GPIOE->CRL &= 0xFFFFF0FF;
	GPIOE->CRL |= 0x00000800;
	GPIOE->ODR |= 1 << 2; // Set PE2 to input pull-up
	RCC->APB2ENR |= 0x01; // Alternate Function I/O clock enabled
	AFIO->EXTICR[0] &= 0xFFFFF0FF;
	AFIO->EXTICR[0] |= 0x00000400; //Set PE2 as EXTI2 input
	EXTI->IMR |= 1 << 2; // Interrupt request from line 2 is not masked
	EXTI->FTSR |= 1 << 2; // Falling trigger enabled
	NVIC->IP[8] = 0x65; // set priority for IRQ #8
	NVIC->ISER[0] |= 1 << 8; // enabled IRQ #8
}

void IERG3810_keyUP_ExtiInit(void)
{
	// KEY UP at PA0, EXTI-0, IRQ #6
	RCC->APB2ENR |= 1 << 2; // Enable I/O port A clock
	GPIOA->CRL &= 0xFFFFFFF0;
	GPIOA->CRL |= 0x00000008;
	GPIOA->ODR &= 0xFFFFFFFE; // Set PA0 to input pull-down
	RCC->APB2ENR |= 0x01; // Alternate Function I/O clock enabled
	AFIO->EXTICR[0] &= 0xFFFFFFF0; // Set PA0 as EXTI0 input
	EXTI->IMR |= 1 << 0; // Interrupt request from line0 is not masked
	EXTI->RTSR |= 1 << 0; // Rising trigger enabled for line 0
	NVIC->IP[6] = 0x75;
	NVIC->ISER[0] |= 1 << 6; // enabled IRQ #6
}

void IERG3810_PS2key_ExtiInit(void)
{
	// PS/2 CLK at PC11, EXTI-11, IRQ #40
	RCC->APB2ENR |= 1 << 4; // Enable I/O port C clock
	GPIOC->CRH &= 0xFFFF00FF;
	GPIOC->CRH |= 0x00008800; // Set PC10, PC11 as input pull-up
	GPIOC->ODR |= 0x00000C00;
	RCC->APB2ENR |= 0x01; // Alternate Function I/O clock enabled
	AFIO->EXTICR[2] &= 0xFFFF0FFF;
	AFIO->EXTICR[2] |= 0x00002000; // Set PC11 as EXTI11 input
	EXTI->IMR |= 1 << 11; // Interrupt request from line11 is not masked
	EXTI->FTSR |= 1 << 11; // Falling trigger enabled for input line 11
	NVIC->IP[40] = 0x10;
	NVIC->ISER[1] |= 1 << 8; // enabled IRQ #40
}
