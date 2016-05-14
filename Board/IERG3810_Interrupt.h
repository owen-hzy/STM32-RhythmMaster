#ifndef __IERG3810_Interrupt_H
#define __IERG3810_Interrupt_H
#include "stm32f10x.h"

void IERG3810_NVIC_SetPriorityGroup(u8 prigroup);
void IERG3810_key2_ExtiInit(void);
void IERG3810_keyUP_ExtiInit(void);
void IERG3810_PS2key_ExtiInit(void);

#endif
