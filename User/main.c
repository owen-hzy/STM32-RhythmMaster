#include "stm32f10x.h"
#include "IERG3810_Clock.h"
#include "IERG3810_LED.h"
#include "IERG3810_KEY.h"
#include "IERG3810_USART.h"
#include "IERG3810_Interrupt.h"
#include "IERG3810_TIM.h"
#include "IERG3810_TFTLCD.h"
#include "bar.h"
#include "string.h"

#define BAR_NUMBER 30
#define low_1 136
#define low_2 121
#define low_3 108
#define low_4 102
#define low_5 90
#define low_6 80
#define low_7 71
#define mid_1 67
#define mid_2 61
#define mid_3 53
#define mid_4 50
#define mid_5 44
#define mid_6 39
#define mid_7 35
#define high_1 33
#define high_2 29
#define high_3 26
#define high_4 24
#define high_5 21
#define high_6 19
#define high_7 17
#define stop 36000

u8 task1HeartBeat;
u8 task2HeartBeat;
//u8 task3HeartBeat;

typedef struct bar
{
	int x;
	int y;
} bar; // Blue Bar 

struct long_bar
{
	int x;
	int y;
	u8 length;
}; // Green Long Bar

bar bar_array[BAR_NUMBER];

struct long_bar long_bar_array[1] = 
{
	{185, 330, 100}
};

char *song[4] = 
{
	"Song 1",
	"Song 2",
	"Song 3",
	"Song 4"
};

u8 level_speed[3] = 
{
	10, 
	5,
	2
};
char *level_name[3] =
{
	"Slow",
	"Middle",
	"Fast"
};

u8 level_index = 0;
u8 level_change = 0;

u16 music[] = {mid_1, mid_2, mid_3, mid_1, mid_1, mid_2, mid_3, mid_1, mid_3, mid_4, mid_5};
u16 music_index = 0;
u16 music_size = 11;
u8 play_music = 0;

// --- Global variables
u32 timeout = 10000; // for PS2 Keyboard timeout
u16 ps2key = 0;  // store PS2 Keyboard data
u32 ps2count = 0; // count PS2 keyboard received bit
u8 ps2dataReady = 0; // 1: PS2 keyboard received data correctly
u8 passed = 0;
u8 score = 0;
u8 missed = 0;
u8 start = 0;
u8 clear = 1;
u8 mode = 0;
u8 song_index = 0;
u8 song_change = 0;
u8 pointer_y = 50;
u8 pointer_change = 0;
u8 infrared = 0;
char text[4];
//u16 red1 = 0;
//u16 red2 = 0;
//u16 red3 = 0;
//u16 red4 = 0;

void read_key(void);
void Display_Gameover(void);
void Display_Pass(void);
void Display_Home(void);
void Display_Info(void);
void Init_Game(u8 choice);

void convert(u8 value, char *str)
{
	u8 i;
	for (i = 100; i > 0; i /= 10)
	{
		u8 d = value / i;
		*str++ = (char) (0x30 + d);
		value -= (d * i);
	}
	
	*str = 0;
}

int main(void)
{
	u8 i = 0;
	IERG3810_clock_tree_init();
//	IERG3810_USART1_init(72, 9600);
	IERG3810_LED_Init();
	IERG3810_NVIC_SetPriorityGroup(5);
	IERG3810_PS2key_ExtiInit();
	IERG3810_SYSTICK_Init10ms();
	IERG3810_TIM4_PwmInit(1999, 0);
	TIM4->CCER &= 0xFEFF;
	IERG3810_TFTLCD_init();
	
	GPIOB->BSRR |= 1 << 5; // DS0_OFF
	GPIOE->BSRR |= 1 << 5; // DS1_OFF
	
	while(1)
	{	
//		if (start == 3 && infrared == 1 && task3HeartBeat >= 1)
//		{
//			if (red4 >= 500 || red3 >= 500 || red2 >= 500 || red1 >= 500) // exceed 5s
//			{
//				red1 = 0;
//				red2 = 0;
//				red3 = 0;
//				red4 = 0;
//				clear = 1;
//				start = 1;
//				mode = 3;
//				play_music = 0;
//			}
//			
//			if ((GPIOA->IDR & GPIO_Pin_4) == 0) 
//			{
//				red4 += 1;
//			} 
//			else 
//			{
//				red4 = 0;
//			}
//			
//			if ((GPIOA->IDR & GPIO_Pin_3) == 0) 
//			{
//				red3 += 1;
//			}
//			else 
//			{
//				red3 = 0;
//			}
//			
//			if ((GPIOA->IDR & GPIO_Pin_2) == 0) 
//			{
//				red2 += 1;
//			}
//			else
//			{
//				red2 = 0;
//			}
//			
//			if ((GPIOA->IDR & GPIO_Pin_1) == 0) 
//			{
//				red1 += 1;
//			}
//			else
//			{
//				red1 = 0;
//			}
//			task3HeartBeat = 0;
//		}
		
		// music
		if (task2HeartBeat >= 50) //1s
		{
			if (play_music == 1)
			{
				TIM4->CCER |= 0x0100;
				TIM4->PSC = music[music_index];
				TIM4->CCR3 = 1000;
				music_index = (music_index + 1) % music_size;
			}
			else 
			{
				TIM4->CCER &= 0xFEFF;
			}
			task2HeartBeat = 0;
		}
		
		if (task1HeartBeat >= level_speed[level_index])
		{
			read_key();
			if (mode == 1 && infrared == 1)
			{
				// Init Sensors
				RCC->APB2ENR |= 1 << 2; // Enable Port A
				GPIOA->CRL &= 0xFFF0000F;
				GPIOA->CRL |= 0x00088880;
			}
			else  
			{
				RCC->APB2ENR &= 0xFFFFFFFB;
				GPIOA->CRL &= 0xFFF0000F;
			}
			
			if (start == 0) 
			{
				Display_Home();
			} 
			else if (start == 1)
			{
				Display_Gameover();
			}
			else if (start == 4)
			{
				Display_Pass();
			}
			else if (start == 6) // Info
			{
				Display_Info();
			}
			else {
				// renew
				if (start == 2) 
				{
					IERG3810_TFTLCD_Clear(0xFFFF);
					Init_Game(song_index);
					start = 3;
					play_music = 1;
					music_index = 0;
				}
				if (passed >= BAR_NUMBER)
				{
					clear = 1;
					start = 4;
					mode = 3;
					play_music = 0;
				}
				if (start == 8)
				{
					if (clear == 1)
					{
						IERG3810_TFTLCD_FillRectangle(0xFFFF, 0, 239, 18, 262);
						IERG3810_TFTLCD_FillRectangle(0xFFFF, 0, 239, 0, 3);
						IERG3810_TFTLCD_FillRectangle(0x0000, 37, 164, 98, 93);
						clear = 0;
					}
					
					IERG3810_TFTLCD_FillRectangle(0x2B12, 29, 180, 90, 8);
					IERG3810_TFTLCD_FillRectangle(0x2B12, 29, 180, 190, 8);
					IERG3810_TFTLCD_FillRectangle(0x2B12, 29, 8, 90, 101);
					IERG3810_TFTLCD_FillRectangle(0x2B12, 201, 8, 90, 101);
					
					IERG3810_TFTLCD_Print("Press 2 to continue", 45, 165, 0xFFFF);
					IERG3810_TFTLCD_Print("Press 1 to main", 45, 135, 0xFFFF);
					IERG3810_TFTLCD_Print("Press 0 to restart", 45, 105, 0xFFFF);
					
					mode = 8; // pause
				}
				else 
				{
					IERG3810_TFTLCD_FillRectangle(0xFFFF, 0, 239, 18, 262);
					IERG3810_TFTLCD_FillRectangle(0xFFFF, 0, 239, 0, 3);
					for (i = passed; i < BAR_NUMBER; i++) 
					{
						if (bar_array[i].y > -10 && bar_array[i].y < 279) 
						{
							IERG3810_TFTLCD_FillRectangle(0x000F, bar_array[i].x, BAR_LENGTH, bar_array[i].y, BAR_HEIGHT);
						}
						bar_array[i].y -= 10;
						if (bar_array[i].y < 0)
						{
							passed = i + 1;
							missed = passed - score;
							if (missed >= 10)
							{
								clear = 1;
								start = 1;
								mode = 3;
								play_music = 0;
							}
						}
					}
				}
				IERG3810_TFTLCD_FillRectangle(0XBC40, 0, 0x00EF, 280, 0x0003);
				// Draw tap
				IERG3810_TFTLCD_FillRectangle(0X841F, 3, 57, 3, 15);
				IERG3810_TFTLCD_FillRectangle(0X841F, 63, 57, 3, 15);
				IERG3810_TFTLCD_FillRectangle(0X841F, 123, 57, 3, 15);
				IERG3810_TFTLCD_FillRectangle(0X841F, 183, 54, 3, 15);
				
				IERG3810_TFTLCD_Print("Missed: ", 129, 289, 0x0000);
				convert(missed, text);
				IERG3810_TFTLCD_FillRectangle(0xFFFF, 199, 25, 289, 16);
				IERG3810_TFTLCD_Print(text, 199, 289, 0x0000);
				IERG3810_TFTLCD_Print("Score: ", 19, 289, 0x0000);
				convert(score, text);
				IERG3810_TFTLCD_FillRectangle(0xFFFF, 79, 25, 289, 16);
				IERG3810_TFTLCD_Print(text, 79, 289, 0x0000);
			}
			task1HeartBeat = 0;
		}
	}
}

void EXTI15_10_IRQHandler(void)
{
	Delay(10); // A short delay can eliminate the error that the processor is too fast
	ps2count++;
	if (ps2count >= 2 && ps2count <= 9)
	{
		if ((GPIOC->IDR & GPIO_Pin_10) != 0) 
		{
			ps2key |= 1 << (ps2count - 2);
		}
	}
	EXTI->PR |= 1 << 11;
}

void Init_Game(u8 choice)
{
	score = 0;
	missed = 0;
	passed = 0;
	if (choice == 0)
	{
		bar s1[BAR_NUMBER] = {
			{5, 640},
			{65, 700},
			{5, 840},
			{125, 960},
			{185, 1100},
			{5, 1300},
			{185, 1500},
			{5, 1700},
			{185, 1900},
			{125, 2000},
			{5, 2640},
			{65, 2700},
			{5, 2840},
			{125, 2960},
			{185, 3100},
			{5, 3300},
			{185, 3500},
			{5, 3700},
			{185, 3900},
			{125, 4000},
			{5, 4640},
			{65, 4700},
			{5, 4840},
			{125, 4960},
			{185, 4100},
			{5, 4300},
			{185, 4500},
			{5, 4700},
			{185, 4900},
			{125, 5000}
		};
		u8 i = 0;
		for (; i < BAR_NUMBER; i++)
		{
			bar_array[i].x = s1[i].x;
			bar_array[i].y = s1[i].y;
		}
	}
	else if (choice == 1)
	{
		bar s1[BAR_NUMBER] = {
			{ 5, 1300},
			{ 65, 1450},
			{ 185, 1600},
			{ 125, 1750},
			{ 125, 1900},
			{ 185, 2050},
			{ 5, 2200},
			{ 65, 2400},
			{ 185, 2600},
			{ 125, 2800},
			{ 65, 3000},
			{ 65, 3150},
			{ 185, 3300},
			{ 5, 3500},
			{ 125, 3700},
			{ 65, 3900},
			{ 185, 4100},
			{ 125, 4300},
			{ 5, 4500},
			{ 65, 4700},
			{ 5, 4900},
			{ 125, 5100},
			{ 125, 5300},
			{ 5, 5500},
			{ 65, 5700},
			{ 5, 5900},
			{ 125, 6100},
			{ 185, 6300},
			{ 5, 6500},
			{ 65, 6700}
		};
		u8 i = 0;
		for (; i < BAR_NUMBER; i++)
		{
			bar_array[i].x = s1[i].x;
			bar_array[i].y = s1[i].y;
		}
	}
	else if (choice == 2)
	{
		bar s1[BAR_NUMBER] = {
			{ 185, 1300},
			{ 125, 1450},
			{ 5, 1600},
			{ 185, 1750},
			{ 5, 1900},
			{ 65, 2050},
			{ 5, 2200},
			{ 65, 2350},
			{ 185, 2500},
			{ 125, 2650},
			{ 125, 2800},
			{ 185, 3050},
			{ 5, 3200},
			{ 65, 3350},
			{ 65, 3500},
			{ 185, 3650},
			{ 125, 3800},
			{ 5, 3950},
			{ 5, 4100},
			{ 185, 4250},
			{ 125, 4400},
			{ 65, 4500},
			{ 65, 4700},
			{ 185, 4850},
			{ 125, 5000},
			{ 5, 5150},
			{ 125, 5300},
			{ 185, 5450},
			{ 65, 5600},
			{ 125, 5750}
		};
		u8 i = 0;
		for (; i < BAR_NUMBER; i++)
		{
			bar_array[i].x = s1[i].x;
			bar_array[i].y = s1[i].y;
		}
	}
	else if (choice == 3)
	{
		bar s1[BAR_NUMBER] = {
			{ 5, 1000},
			{ 65, 1200},
			{ 5, 1400},
			{ 185, 1600},
			{ 125, 1800},
			{ 125, 2000},
			{ 5, 2200},
			{ 65, 2400},
			{ 65, 2600},
			{ 185, 2800},
			{ 5, 3000},
			{ 125, 3200},
			{ 5, 3400},
			{ 125, 3600},
			{ 185, 3800},
			{ 5, 4000},
			{ 125, 4200},
			{ 65, 4400},
			{ 185, 4600},
			{ 185, 4800},
			{ 125, 5000},
			{ 65, 5200},
			{ 125, 5400},
			{ 185, 5600},
			{ 65, 5800},
			{ 5, 6000},
			{ 65, 6200},
			{ 5, 6400},
			{ 125, 6600},
			{ 185, 6800}
		};
		u8 i = 0;
		for (; i < BAR_NUMBER; i++)
		{
			bar_array[i].x = s1[i].x;
			bar_array[i].y = s1[i].y;
		}
	}
}

void Display_Gameover(void)
{
	if (clear == 1)
	{
		IERG3810_TFTLCD_Clear(0x0000);
		clear = 0;
	}
	IERG3810_TFTLCD_FillRectangle(0x01CF, 0, 8, 0, 280);
	IERG3810_TFTLCD_FillRectangle(0x01CF, 232, 8, 0, 280);
	IERG3810_TFTLCD_FillRectangle(0x01CF, 0, 240, 0, 8);
	IERG3810_TFTLCD_FillRectangle(0x01CF, 0, 240, 272, 8);
	IERG3810_TFTLCD_Print("GAME OVER!", 79, 159, 0xFFFF);
	IERG3810_TFTLCD_Print("Press 0 To Restart", 39, 79, 0xFFFF);
	IERG3810_TFTLCD_Print("Press 1 To Main", 39, 39, 0xFFFF);
}

void Display_Pass(void)
{
	if (clear == 1)
	{
		IERG3810_TFTLCD_Clear(0x0000);
		clear = 0;
	}
	IERG3810_TFTLCD_FillRectangle(0x841F, 0, 8, 0, 280);
	IERG3810_TFTLCD_FillRectangle(0x841F, 232, 8, 0, 280);
	IERG3810_TFTLCD_FillRectangle(0x841F, 0, 240, 0, 8);
	IERG3810_TFTLCD_FillRectangle(0x841F, 0, 240, 272, 8);
	IERG3810_TFTLCD_Print("You Passed!", 69, 139, 0xFFFF);
	IERG3810_TFTLCD_Print("Your Score: ", 59, 99, 0xFFFF);
	IERG3810_TFTLCD_Print(text, 159, 99, 0xFFFF);
	IERG3810_TFTLCD_Print("Press 0 To Restart", 39, 59, 0xFFFF);
	IERG3810_TFTLCD_Print("Press 1 To Main", 39, 39, 0xFFFF);
}

void Display_Home(void) 
{
	if (clear == 1)
	{
		IERG3810_TFTLCD_Clear(0x0000);
		clear = 0;
	}
	IERG3810_TFTLCD_FillRectangle(0x7D7C, 29, 180, 249, 8);
	IERG3810_TFTLCD_FillRectangle(0x7D7C, 29, 180, 299, 8);
	IERG3810_TFTLCD_FillRectangle(0x7D7C, 29, 8, 249, 50);
	IERG3810_TFTLCD_FillRectangle(0x7D7C, 201, 8, 249, 50);
	IERG3810_TFTLCD_Print("Rhythm Master", 69, 269, 0xA651);
	
	Print_Name(0, 39, 229);
	IERG3810_TFTLCD_Print("1155014567", 119, 229, 0xFFFF);
	Print_Name(1, 39, 209);
	IERG3810_TFTLCD_Print("1155049987", 119, 209, 0xFFFF);
	
	// Draw song selection
	IERG3810_TFTLCD_FillRectangle(0x2B12, 29, 180, 90, 8);
	IERG3810_TFTLCD_FillRectangle(0x2B12, 29, 180, 190, 8);
	IERG3810_TFTLCD_FillRectangle(0x2B12, 29, 8, 90, 101);
	IERG3810_TFTLCD_FillRectangle(0x2B12, 201, 8, 90, 101);
	
	IERG3810_TFTLCD_Print("Speed", 90, 175, 0xF81F);
	IERG3810_TFTLCD_Print("<<", 49, 150, 0xFFFF);
	IERG3810_TFTLCD_Print(">>", 170, 150, 0xFFFF);
	if (level_change == 1)
	{
		IERG3810_TFTLCD_FillRectangle(0x0000, 90, 80, 150, 16);
		level_change = 0;
	}
	IERG3810_TFTLCD_Print(level_name[level_index], 90, 150, 0xFFFF);
	
	
	IERG3810_TFTLCD_Print("Music", 90, 120, 0xF81F);
	IERG3810_TFTLCD_Print("<<", 49, 100, 0xFFFF);
	IERG3810_TFTLCD_Print(">>", 170, 100, 0xFFFF);
	if (song_change == 1)
	{
		IERG3810_TFTLCD_FillRectangle(0x0000, 90, 80, 100, 16);
		song_change = 0;
	}
	IERG3810_TFTLCD_Print(song[song_index], 90, 100, 0xFFFF);
	
	if (pointer_change == 1)
	{
		IERG3810_TFTLCD_FillRectangle(0x0000, 50, 16, 10, 60);
		pointer_change = 0;
	}
	IERG3810_TFTLCD_Print(">", 50, pointer_y, 0xFFFF);
	IERG3810_TFTLCD_Print("Start(infrared)", 70, 50, 0xFFFF);
	IERG3810_TFTLCD_Print("Start(keyboard)", 70, 30, 0xFFFF);
	IERG3810_TFTLCD_Print("Info", 75, 10, 0xFFFF);
}

void Display_Info(void)
{
	if (clear == 1)
	{
		IERG3810_TFTLCD_Clear(0x0000);
		clear = 0;
	}
	
	IERG3810_TFTLCD_Print("Rhythm Master", 80, 299, 0x841F);
	IERG3810_TFTLCD_Print("1. Info", 3, 275, 0x01CF);
	IERG3810_TFTLCD_Print("Rhythm Master is a block", 3, 260, 0xFFFF);
	IERG3810_TFTLCD_Print("elimination game by tapping", 3, 245, 0xFFFF);
	IERG3810_TFTLCD_Print("on a specific bar while", 3, 230, 0xFFFF);
	IERG3810_TFTLCD_Print("block falling down", 3, 215, 0xFFFF);
	
	IERG3810_TFTLCD_Print("2. Instruction", 3, 195, 0x01CF);
	IERG3810_TFTLCD_Print("key 4: channel 1", 3, 180, 0xFFFF);
	IERG3810_TFTLCD_Print("key 5: channel 2", 3, 165, 0xFFFF);
	IERG3810_TFTLCD_Print("key 6: channel 3", 3, 150, 0xFFFF);
	IERG3810_TFTLCD_Print("key +: channel 4", 3, 135, 0xFFFF);
	IERG3810_TFTLCD_Print("key BackSpace: pause", 3, 120, 0xFFFF);
	IERG3810_TFTLCD_Print("key .: toggle music", 3, 105, 0xFFFF);
	
	IERG3810_TFTLCD_Print("3. Rules", 3, 85, 0x01CF);
	IERG3810_TFTLCD_Print("When user misses more than", 3, 70, 0xFFFF);
	IERG3810_TFTLCD_Print("10 bars. Then gameover", 3, 55, 0xFFFF);
	
	IERG3810_TFTLCD_Print("Press + To Back", 59, 19, 0x7FFF);
}

void read_key()
{
	u8 i = 0;
	
	if (infrared == 1 && mode == 1)
	{
		if ((GPIOA->IDR & GPIO_Pin_4) == 0) 
		{
			IERG3810_TFTLCD_FillRectangle(0xF800, 183, 54, 3, 15);
		}
		if ((GPIOA->IDR & GPIO_Pin_3) == 0) 
		{
			IERG3810_TFTLCD_FillRectangle(0xF800, 123, 57, 3, 15);
		}
		if ((GPIOA->IDR & GPIO_Pin_2) == 0) 
		{
			IERG3810_TFTLCD_FillRectangle(0xF800, 63, 57, 3, 15);
		}
		if ((GPIOA->IDR & GPIO_Pin_1) == 0) 
		{
			IERG3810_TFTLCD_FillRectangle(0xF800, 3, 57, 3, 15);
		}
		for (i = passed; i < BAR_NUMBER; i++) 
		{
			if (bar_array[i].y >= -3 && bar_array[i].y <= 15) 
			{
				if ((GPIOA->IDR & GPIO_Pin_1) == 0 && bar_array[i].x == 5) 
				{
					score += 1;
					passed += 1;
				}
				if ((GPIOA->IDR & GPIO_Pin_2) == 0 && bar_array[i].x == 65) 
				{
					score += 1;
					passed += 1;
				}
				if ((GPIOA->IDR & GPIO_Pin_3) == 0 && bar_array[i].x == 125)
				{
					score += 1;
					passed += 1;
				}
				if ((GPIOA->IDR & GPIO_Pin_4) == 0 && bar_array[i].x == 185)
				{
					score += 1;
					passed += 1;
				}
			}
		}
	}
	
	if (ps2count >= 11)
	{			
//		if (ps2key == 0x00F0) 
//		{
//			ps2key <<= 8;
//			ps2count = 0;
//			ps2dataReady = 0;
//		}
		
		if (mode == 0) // Home
		{
			ps2dataReady = 1;
			if (ps2key == 0x006B) //4
			{
				song_index -= 1;
				song_index = (song_index + 4) % 4;
				song_change = 1;
			}
			if (ps2key == 0x0074) // 6
			{
				song_index += 1;
				song_index %= 4;
				song_change = 1;
			}
			if (ps2key == 0x006C) //7
			{
				level_index = (level_index + 2) % 3;
				level_change = 1;
			}
			if (ps2key == 0x007D) // 9
			{
				level_index += 1;
				level_index %= 3;
				level_change = 1;
			}
			if (ps2key == 0x0072) // 2
			{
				if (pointer_y != 10)
				{
					pointer_y -= 20;
					pointer_change = 1;
				}
			}
			if (ps2key == 0x0075) //8
			{
				if (pointer_y != 50)
				{
					pointer_y += 20;
					pointer_change = 1;
				}
			}
			if (ps2key == 0x0079) //+
			{
				if (pointer_y == 50) //start
				{
					mode = 1; // play
					start = 2;
					infrared = 1;
				}
				if (pointer_y == 30) //start
				{
					mode = 1; // play
					start = 2;
					infrared = 0;
				}
				if (pointer_y == 10) //Info
				{
					clear = 1;
					mode = 2; //Info
					start = 6; // info
				}
			}
		}
		else if (mode == 1)
		{
			ps2dataReady = 1;
			
			if (infrared == 1)
			{
				switch (ps2key) 
				{
					case 0x0066:
						start = 8; //pause
						clear = 1;
						play_music = 0;
						break;
					case 0x0071: //. to toggle music
						if (play_music == 1)
						{
							play_music = 0;
						}
						else 
						{
							music_index = 0;
							play_music = 1;
						}
						break;
//					case 0x007C: // * to speed up
//						if (level_speed[level_index] > 5)
//						{							
//							level_speed[level_index] -= 5;
//						} else if (level_speed[level_index] >= 3)
//						{
//							level_speed[level_index] -= 2;
//						}
//						break;
//					case 0x007B: // - to slow down
//						if (level_speed[level_index] <= 50)
//						{
//							level_speed[level_index] += 5;
//						}
//						break;
					default:
						break;
				}
			}
			
			else
			{
			switch (ps2key) 
			{
				case 0x006B:
					IERG3810_TFTLCD_FillRectangle(0xF800, 3, 57, 3, 15);
					break;
				case 0x0073:
					IERG3810_TFTLCD_FillRectangle(0xF800, 63, 57, 3, 15);
					break;
				case 0x0074:
					IERG3810_TFTLCD_FillRectangle(0xF800, 123, 57, 3, 15);
					break;
				case 0x0079:
					IERG3810_TFTLCD_FillRectangle(0xF800, 183, 54, 3, 15);
					break;
				case 0x0066:
					start = 8; //pause
					clear = 1;
					play_music = 0;
					break;
				case 0x0071: //. to toggle music
					if (play_music == 1)
					{
						play_music = 0;
					}
					else 
					{
						music_index = 0;
						play_music = 1;
					}
					break;
//				case 0x007C: // * to speed up
//					if (level_speed[level_index] > 5)
//					{							
//						level_speed[level_index] -= 5;
//					} else if (level_speed[level_index] >= 3)
//					{
//						level_speed[level_index] -= 2;
//					}
//					break;
//				case 0x007B: // - to slow down
//					if (level_speed[level_index] <= 50)
//					{
//						level_speed[level_index] += 5;
//					}
//					break;
				default:
					break;
			}
			
			for (i = passed; i < BAR_NUMBER; i++) 
			{
				if (bar_array[i].y >= -3 && bar_array[i].y <= 15) 
				{
					switch (ps2key) 
					{
						case 0x006B:
							if (bar_array[i].x == 5) 
							{
								score += 1;
								passed += 1;
							}
							break;
						case 0x0073:
							if (bar_array[i].x == 65) 
							{
								score += 1;
								passed += 1;
							}
							break;
						case 0x0074:
							if (bar_array[i].x == 125)
							{
								score += 1;
								passed += 1;
							}
							break;
						case 0x0079:
							if (bar_array[i].x == 185)
							{
								score += 1;
								passed += 1;
							}
							break;
						default:
							break;
					}
				}
			}
			}
		}
		else if (mode == 2) // Info
		{
			ps2dataReady = 1;
			if (ps2key == 0x0079) //+
			{
				clear = 1;
				mode = 0;
				start = 0; //home
			}
		}
		else if (mode == 3) // gameover or win
		{
			ps2dataReady = 1;
			if (ps2key == 0x0070) //0 restart
			{
				clear = 1;
				mode = 1;
				start = 2;
			}
			if (ps2key == 0x0069) // 1 main
			{
				clear = 1;
				mode = 0;
				start = 0;
			}
		}
		else if (mode == 8) // pause
		{
			ps2dataReady = 1;
			if (ps2key == 0x0070) //0 restart
			{
				clear = 1;
				mode = 1;
				start = 2;
			}
			if (ps2key == 0x0069) // 1 main
			{
				clear = 1;
				mode = 0;
				start = 0;
			}
			if (ps2key == 0x0072) // 2 continue
			{
				mode = 1;
				start = 3; // initialized
				play_music = 1;
			}
		}
		if (ps2dataReady == 1)
		{
			timeout = 0;
		}
	}
	
	if (timeout == 0) // will clear PS2 keyboard data when timeout
	{
		timeout = 10000;
		ps2key = 0;
		ps2count = 0;
	}
	timeout--;
}
