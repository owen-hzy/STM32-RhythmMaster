#ifndef __IERG3810_TFTLCD_H
#define __IERG3810_TFTLCD_H
#include "stm32f10x.h"

typedef struct
{
	u16 LCD_REG;
	u16 LCD_RAM;
} LCD_TypeDef;

#define LCD_BASE	((u32) (0x6C000000 | 0x000007FE))
#define LCD     	((LCD_TypeDef *) LCD_BASE)

void IERG3810_TFTLCD_init(void);
void IERG3810_TFTLCD_SetParameter(void);
void IERG3810_TFTLCD_WrReg(u16 regval);
void IERG3810_TFTLCD_WrData(u16 data);
void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y);
void IERG3810_TFTLCD_SevenSegment(u16 color, u16 start_x, u16 start_y, u8 digit);
void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bgcolor);
void IERG3810_TFTLCD_ShowChar_Transparent(u16 x, u16 y, u8 ascii, u16 color);
void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 font_index, u16 color, u16 bgcolor);
void IERG3810_TFTLCD_Clear(u16 color);
void Count_Down(void);
void IERG3810_TFTLCD_Print(char* str, u16 x, u16 y, u16 color);
void Print_Name(u8 person_id, u16 x, u16 y);
void LCD_LIGHT_ON(void);

#endif
