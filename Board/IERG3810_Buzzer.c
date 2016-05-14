#include "IERG3810_Buzzer.h"

void IERG3810_Buzzer_Init(void)
{
	// Init Buzzer
	GPIOB->CRH &= 0xFFFFFFF0;
	GPIOB->CRH |= 0x00000003;
}
