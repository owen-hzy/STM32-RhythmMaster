#include "IERG3810_TFTLCD.h"
#include "IERG3810_Clock.h"
#include "FONT.h"

void IERG3810_TFTLCD_init(void)
{
	RCC->AHBENR |= 1 << 8;	//FSMCEN = 1; FSMC clock enabled
	RCC->APB2ENR |= 1 << 3;	//IOPBEN = 1; IO port B clock enabled
	RCC->APB2ENR |= 1 << 5; //IOPDEN = 1;
	RCC->APB2ENR |= 1 << 6; //IOPEEN = 1;
	RCC->APB2ENR |= 1 << 8;	//IOPGEN = 1;
	
	// configure PB0 as general purpose output push-pull 
	GPIOB->CRL &= 0xFFFFFFF0;
	GPIOB->CRL |= 0x00000003;
	
	// PortD 8,9,10,14,15,0,1,4,5 as alternate function output push-pull B=1011
	GPIOD->CRH &= 0x00FFF000;
	GPIOD->CRH |= 0xBB000BBB;
	GPIOD->CRL &= 0xFF00FF00;
	GPIOD->CRL |= 0x00BB00BB;
	
	// PortE
	GPIOE->CRH &= 0x00000000;
	GPIOE->CRH |= 0xBBBBBBBB;
	GPIOE->CRL &= 0x0FFFFFFF;
	GPIOE->CRL |= 0xB0000000;
	
	// PortG 12,0
	GPIOG->CRH &= 0xFFF0FFFF;
	GPIOG->CRH |= 0x000B0000;
	GPIOG->CRL &= 0xFFFFFFF0; //PG0 -> RS
	GPIOG->CRL |= 0x0000000B;
	
	// LCD uses FSMC Bank 4 memory bank
	// Use Mode A access
	FSMC_Bank1->BTCR[6] = 0x00000000;	//FSMC_BCR4 (reset)
	FSMC_Bank1->BTCR[7] = 0x00000000;	//FSMC_BTR4	(reset)
	FSMC_Bank1E->BWTR[6] = 0x00000000;	//FSMC_BWTR4	(reset)
	FSMC_Bank1->BTCR[6] |= 1 << 12;	//WREN=1; Write operations are enabled for the bank 
	FSMC_Bank1->BTCR[6] |= 1 << 14;	//EXTMOD=1; Values inside FSMC_BWTR register are taken into account, so it allows different timings for read and write
	FSMC_Bank1->BTCR[6] |= 1 << 4;	//MWID=01; Defines the external memory device width as 16 bits
	FSMC_Bank1->BTCR[7] |= 0 << 28;	//ACCMOD=00; Access Mode A, this register bit specifies the asynchronous access modes
	FSMC_Bank1->BTCR[7] |= 1 << 0;	//ADDSET=0001; 0001: ADDSET phase duration = 2 * HCLK clock cycle, Duration of the address setup phase
	FSMC_Bank1->BTCR[7] |= 0xF << 8;	//DATAST=00001111; DATAST phase duration = 16 * HCLK clock cycle, duration of the data phase
	FSMC_Bank1E->BWTR[6] |= 0 << 28;	//ACCMOD=00; Access Mode A, for write
	FSMC_Bank1E->BWTR[6] |= 0 << 0;		//ADDSET=0000; ADDSET phase duration = 1 * HCLK clock cycle
	FSMC_Bank1E->BWTR[6] |= 3 << 8;		//DATAST=00000011; DATAST phase duration = 4 * HCLK clock cycle
	FSMC_Bank1->BTCR[6] |= 1 << 0;		//MBKEN=1; Enables the memory bank, then others are disabled
	IERG3810_TFTLCD_SetParameter();
	LCD_LIGHT_ON();
}

void IERG3810_TFTLCD_SetParameter(void)
{
	IERG3810_TFTLCD_WrReg(0x01);	//Resets the commands and parameters to their S/W reset default values
	IERG3810_TFTLCD_WrReg(0x11);	//Exit sleep mode, internal oscillator is started, and panel scanning is started
	
	IERG3810_TFTLCD_WrReg(0x3A);	//Set pixel format
	IERG3810_TFTLCD_WrData(0x55);	//MCU 16 bit and RGB interface also 16 bit
	
	IERG3810_TFTLCD_WrReg(0x29);	// Display ON, output from the frame memory is enabled
	
	IERG3810_TFTLCD_WrReg(0x36);
	IERG3810_TFTLCD_WrData(0xCA);	//Row/Column Exchange = 0, Row access order = 1, 
									//column access order = 1, vertical refresh order = 0, horizaontal refresh order = 0, BGR = 1
									//The virtual (0,0) is at bottom-right corner
}

void IERG3810_TFTLCD_WrReg(u16 regval)
{
	LCD->LCD_REG = regval;
}

void IERG3810_TFTLCD_WrData(u16 data)
{
	LCD->LCD_RAM = data;
}

void IERG3810_TFTLCD_FillRectangle(u16 color, u16 start_x, u16 length_x, u16 start_y, u16 length_y)
{
	u32 index = 0;
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(start_x >> 8);
	IERG3810_TFTLCD_WrData(start_x & 0xFF);
	IERG3810_TFTLCD_WrData((start_x + length_x - 1) >> 8);
	IERG3810_TFTLCD_WrData((start_x + length_x - 1) & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(start_y >> 8);
	IERG3810_TFTLCD_WrData(start_y & 0xFF);
	IERG3810_TFTLCD_WrData((start_y + length_y - 1) >> 8);
	IERG3810_TFTLCD_WrData((start_y + length_y - 1) & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2C);
	for (index = 0; index < length_x * length_y; index++)
	{
		IERG3810_TFTLCD_WrData(color);
	}
}

void a(u16 color, u16 start_x, u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0082, 0x000A);
}

void b(u16 color, u16 start_x, u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x004B, 0x0037);
}

void c(u16 color, u16 start_x, u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x + 0x0041, 0x000A, start_y + 0x000A, 0x0037);
}

void d(u16 color, u16 start_x, u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y, 0x000A);
}

void e(u16 color, u16 start_x, u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x, 0x000A, start_y + 0x000A, 0x0037);
}

void f(u16 color, u16 start_x, u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x, 0x000A, start_y + 0x004B, 0x0037);
}

void g(u16 color, u16 start_x, u16 start_y)
{
	IERG3810_TFTLCD_FillRectangle(color, start_x + 0x000A, 0x0037, start_y + 0x0041, 0x000A);
}

void IERG3810_TFTLCD_SevenSegment(u16 color, u16 start_x, u16 start_y, u8 digit)
{
	switch (digit)
	{
		case 0x00:
			a(color, start_x, start_y);
			b(color, start_x, start_y);
			c(color, start_x, start_y);
			d(color, start_x, start_y);
			e(color, start_x, start_y);
			f(color, start_x, start_y);
			break;
		case 0x01:
			b(color, start_x, start_y);
			c(color, start_x, start_y);
			break;
		case 0x02:
			a(color, start_x, start_y);
			b(color, start_x, start_y);
			e(color, start_x, start_y);
			d(color, start_x, start_y);
			g(color, start_x, start_y);
			break;
		case 0x03:
			a(color, start_x, start_y);
			b(color, start_x, start_y);
			c(color, start_x, start_y);
			d(color, start_x, start_y);
			g(color, start_x, start_y);
			break;
		case 0x04:
			b(color, start_x, start_y);
			c(color, start_x, start_y);
			g(color, start_x, start_y);
			f(color, start_x, start_y);
			break;
		case 0x05:
			a(color, start_x, start_y);
			c(color, start_x, start_y);
			d(color, start_x, start_y);
			f(color, start_x, start_y);
			g(color, start_x, start_y);
			break;
		case 0x06:
			a(color, start_x, start_y);
			c(color, start_x, start_y);
			d(color, start_x, start_y);
			e(color, start_x, start_y);
			f(color, start_x, start_y);
			g(color, start_x, start_y);
			break;
		case 0x07:
			a(color, start_x, start_y);
			b(color, start_x, start_y);
			c(color, start_x, start_y);
			break;
		case 0x08:
			a(color, start_x, start_y);
			b(color, start_x, start_y);
			c(color, start_x, start_y);
			d(color, start_x, start_y);
			e(color, start_x, start_y);
			f(color, start_x, start_y);
			g(color, start_x, start_y);
			break;
		case 0x09:
			a(color, start_x, start_y);
			b(color, start_x, start_y);
			c(color, start_x, start_y);
			d(color, start_x, start_y);
			f(color, start_x, start_y);
			g(color, start_x, start_y);
			break;
		default:
			break;
	}
}

void IERG3810_TFTLCD_ShowChar(u16 x, u16 y, u8 ascii, u16 color, u16 bgcolor)
{
	u8 i, j;
	u8 index;
	u8 height = 16, length = 8;
	if (ascii < 32 || ascii > 126) return;
	ascii -= 32;
	
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(x >> 8);
	IERG3810_TFTLCD_WrData(x & 0xFF);
	IERG3810_TFTLCD_WrData((x + length - 1) >> 8);
	IERG3810_TFTLCD_WrData((x + length - 1) & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(y >> 8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData((y + height - 1) >> 8);
	IERG3810_TFTLCD_WrData((y + height - 1) & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2C);
	for (j = 0; j < height / 8; j++)
	{
		for (i = 0; i < height / 2; i++)
		{
			for (index = 0; index < length; index++)
			{
				if ((asc2_1608[ascii][index * 2 + 1 - j] >> i) & 0x01)	// Render from bottom
					IERG3810_TFTLCD_WrData(color);
				else
					IERG3810_TFTLCD_WrData(bgcolor);
			}
		}
	}
}

void IERG3810_TFTLCD_ShowChar_Transparent(u16 x, u16 y, u8 ascii, u16 color)
{
	u8 i, j;
	u8 index;
	u8 height = 16, length = 8;
	if (ascii < 32 || ascii > 126) return;
	ascii -= 32;
	
	for (j = 0; j < height / 8; j++)
	{
		for (i = 0; i < height / 2; i++)
		{
			for (index = 0; index < length; index++)
			{
				if ((asc2_1608[ascii][index * 2 + 1 - j] >> i) & 0x01)
					IERG3810_TFTLCD_FillRectangle(color, (u16) x + index, 0x0001, (u16) y + (height / 2) * j + i, 0x0001);
			}
		}
	}
}

void IERG3810_TFTLCD_ShowChinChar(u16 x, u16 y, u8 font_index, u16 color, u16 bgcolor)
{
	u8 i, j;
	u8 index;
	u8 height = 16, length = 16;
	
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(x >> 8);
	IERG3810_TFTLCD_WrData(x & 0xFF);
	IERG3810_TFTLCD_WrData((x + length - 1) >> 8);
	IERG3810_TFTLCD_WrData((x + length - 1) & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(y >> 8);
	IERG3810_TFTLCD_WrData(y & 0xFF);
	IERG3810_TFTLCD_WrData((y + height - 1) >> 8);
	IERG3810_TFTLCD_WrData((y + height - 1) & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2C);
	
	for (j = 0; j < height / 8; j++)
	{
		for (i = 0; i < height / 2; i++)
		{
			for (index = 0; index < length; index++)
			{
				if ((chi_1616[font_index][index * 2 + 1 - j] >> i) & 0x01)
					IERG3810_TFTLCD_FillRectangle(color, (u16) x + index, 0x0001, (u16) y + (height / 2) * j + i, 0x0001);
			}
		}
	}
}

// Clear the LCD
// color: the color to fill the LCD
void IERG3810_TFTLCD_Clear(u16 color)
{
	u32 index = 0;
	IERG3810_TFTLCD_WrReg(0x2A);
	IERG3810_TFTLCD_WrData(0x0000 >> 8);
	IERG3810_TFTLCD_WrData(0x0000 & 0xFF);
	IERG3810_TFTLCD_WrData(0x013F >> 8);
	IERG3810_TFTLCD_WrData(0x013F & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2B);
	IERG3810_TFTLCD_WrData(0x0000 >> 8);
	IERG3810_TFTLCD_WrData(0x0000 & 0xFF);
	IERG3810_TFTLCD_WrData(0x0140 >> 8);
	IERG3810_TFTLCD_WrData(0x0140 & 0xFF);
	IERG3810_TFTLCD_WrReg(0x2C);
	for (index = 0; index < 240 * 320; index++)
	{
		IERG3810_TFTLCD_WrData(color);
	}
}

void Count_Down(void)
{
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x09);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x08);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x07);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x06);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x05);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x04);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x03);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x02);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x01);
	Delay(1000000);
	IERG3810_TFTLCD_FillRectangle(0xFFFF, 0x0000, 0x013F, 0x0000, 0x013F);
	IERG3810_TFTLCD_SevenSegment(0x07FF, 0x0050, 0x0050, 0x00);
	Delay(1000000);
}

void IERG3810_TFTLCD_Print(char* str, u16 x, u16 y, u16 color)
{
	u8 i = 0;
	while(str[i] != 0x00)
	{
		if (str[i] != 0x00)
			IERG3810_TFTLCD_ShowChar_Transparent(x + i * 0x08, y, str[i], color);
		i++;
	}
}

void Print_Name(u8 person_id, u16 x, u16 y)
{
	IERG3810_TFTLCD_ShowChinChar(x, y, person_id * 0x03 + 0x00, 0x07FF, 0xF7EE);
	IERG3810_TFTLCD_ShowChinChar(x + 0x10, y, person_id * 0x03 + 0x01, 0x07FF, 0xF7EE);
	IERG3810_TFTLCD_ShowChinChar(x + 0x20, y, person_id * 0x03 + 0x02, 0x07FF, 0xF7EE);
}

void LCD_LIGHT_ON(void)
{
	GPIOB->BSRR = 1 << 0;
}
