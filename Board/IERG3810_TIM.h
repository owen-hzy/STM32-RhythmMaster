#ifndef __IERG3810_TIM_H
#define __IERG3810_TIM_H
#include "stm32f10x.h"

void IERG3810_TIM3_Init(u16 arr, u16 psc);
void IERG3810_TIM4_Init(u16 arr, u16 psc);
void IERG3810_SYSTICK_Init10ms(void);
void IERG3810_TIM3_PwmInit(u16 arr, u16 psc);
void IERG3810_TIM4_PwmInit(u16 arr, u16 psc);

#endif
