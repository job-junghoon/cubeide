/*
 * vfd40x2.c
 *
 *  Created on: Jun 4, 2020
 *      Author: hhwang
 */

#include "main.h"
#include <string.h>

VFD40x2 Vfd;

char LCDLine[2][41];
void Init_Lcd(void);
void clrscr (void);
void write_lcd(unsigned char x, unsigned char y, char *str);
void write_lcd_line1(char *str) { write_lcd(0,0,str);}
void write_lcd_line2(char *str) { write_lcd(0,1,str);}
void CG_RAM_Initialize(void);


void InitializeVFD(){

	memset(LCDLine, 0, sizeof(LCDLine));
	memset(Vfd.BufLine, ' ', sizeof(LCDLine));

	Init_Lcd();

	Vfd.Clear = clrscr;
	Vfd.DisplayLine1 = write_lcd_line1;
	Vfd.DisplayLine2 = write_lcd_line2;

	CG_RAM_Initialize();
}


void LCD_E(char x)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_10,(GPIO_PinState)x);
}

void LCD_RW(char x)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_9,(GPIO_PinState)x);
}

void LCD_RS(char x)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_8,(GPIO_PinState)x);
}

void LCD_DATA(char data)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_0,(GPIO_PinState)(0x01&(data>>0)));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_1,(GPIO_PinState)(0x01&(data>>1)));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_2,(GPIO_PinState)(0x01&(data>>2)));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_3,(GPIO_PinState)(0x01&(data>>3)));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_4,(GPIO_PinState)(0x01&(data>>4)));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_5,(GPIO_PinState)(0x01&(data>>5)));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6,(GPIO_PinState)(0x01&(data>>6)));
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_7,(GPIO_PinState)(0x01&(data>>7)));
}

void E_Pulse()
{
	LCD_E(1) ;
	DelayUs(50);
	LCD_E(0) ;
}

void display_ON_OFF(unsigned char d, unsigned char c, unsigned char b)
{
	unsigned char display;
	display = 0x08;

	LCD_RS(0) ;
	LCD_RW(0) ;

	if(d==VFD_ON) d = 0x04 ;
	else d = 0x00 ;

	if(c==VFD_ON) c = 0x02 ;
	else c = 0x00 ;

	if(b==VFD_ON) b = 0x01 ;
	else b = 0x00 ;

	LCD_DATA(display |d  |c |b);
	E_Pulse() ;
}

void FillSpace(char *Array, int FillNum)
{
	static int strl,i;
	strl=strlen(Array);

	if(strl>=FillNum)
	{
		Array[FillNum]='\0';
		return;
	}
	for(i=0;i<(FillNum-strl);i++)strcat(Array," ");
}

void Init_Lcd(void)
{
	LCD_E(0) ;
	LCD_RW(0) ;
	LCD_RS(0) ;

	DelayUs(3000);
	LCD_DATA(0x39) ;
	E_Pulse() ;

	DelayUs(6000);
	LCD_DATA(0x39) ;
	E_Pulse() ;
	DelayUs(500);

	LCD_DATA(0x39) ;
	E_Pulse() ;
	LCD_DATA(0x0f);
	E_Pulse() ;

	LCD_DATA(0x39) ;
	E_Pulse() ;

	LCD_DATA(0x06);
	E_Pulse() ;
	display_ON_OFF(1,0,0);
}


void write_char(char s)
{
	LCD_RS(1) ;
	LCD_RW(0) ;
	LCD_DATA (s);
	E_Pulse() ;
}

void clrscr (void)
{
	LCD_RS(0) ;
	LCD_RW(0) ;
	LCD_DATA(1);
	E_Pulse() ;

	DelayUs(100);
}

void lcd_disp ( char x, char y)
{
	LCD_RS(0) ;
	LCD_RW(0) ;
	if(y==0) LCD_DATA(x+0x80);
	else if(y==1) LCD_DATA(x + 0xc0) ;
	else if(y==2) LCD_DATA(x + 0x80 +20 );
	else if(y==3) LCD_DATA(x + 0xc0 +20 );

	E_Pulse() ;
}

void write_lcd(unsigned char x, unsigned char y, char *str)
{
	static int i;
	i=0;
	lcd_disp(x, y) ;

	while(*str)
	{
		if((x+i)>39)break;
		write_char(*str++);
		i++;
	}

	while(i++<40) write_char(' ');
}

void CG_RAM_ADDRESS(char address)
{


    LCD_RS(0) ;
    LCD_RW(0) ;
    LCD_DATA(0x40 | address);

    E_Pulse() ;

}
void CG_RAM_DATA(char s)
{

    LCD_RS(1) ;
    LCD_RW(0) ;
    LCD_DATA (s) ;
    E_Pulse() ;
 }

void CG_RAM_Initialize(void)
{

	CG_RAM_ADDRESS(0x00|0x00);CG_RAM_DATA(0x1F);  //1
	CG_RAM_ADDRESS(0x00|0x01);CG_RAM_DATA(0x1F);  //2
	CG_RAM_ADDRESS(0x00|0x02);CG_RAM_DATA(0x1F);  //3
	CG_RAM_ADDRESS(0x00|0x03);CG_RAM_DATA(0x1F);  //4
	CG_RAM_ADDRESS(0x00|0x04);CG_RAM_DATA(0x1F);  //5
	CG_RAM_ADDRESS(0x00|0x05);CG_RAM_DATA(0x1F);  //6
	CG_RAM_ADDRESS(0x00|0x06);CG_RAM_DATA(0x1F);  //7
	CG_RAM_ADDRESS(0x00|0x07);CG_RAM_DATA(0x1F);  //8

	CG_RAM_ADDRESS(0x08|0x00);CG_RAM_DATA(0x1F);  //1
	CG_RAM_ADDRESS(0x08|0x01);CG_RAM_DATA(0x00);  //2
	CG_RAM_ADDRESS(0x08|0x02);CG_RAM_DATA(0x00);  //3
	CG_RAM_ADDRESS(0x08|0x03);CG_RAM_DATA(0x00);  //4
	CG_RAM_ADDRESS(0x08|0x04);CG_RAM_DATA(0x00);  //5
	CG_RAM_ADDRESS(0x08|0x05);CG_RAM_DATA(0x00);  //6
	CG_RAM_ADDRESS(0x08|0x06);CG_RAM_DATA(0x00);  //7
	CG_RAM_ADDRESS(0x08|0x07);CG_RAM_DATA(0x00);  //8

	CG_RAM_ADDRESS(0x10|0x00);CG_RAM_DATA(0x00);  //1
	CG_RAM_ADDRESS(0x10|0x01);CG_RAM_DATA(0x00);  //2
	CG_RAM_ADDRESS(0x10|0x02);CG_RAM_DATA(0x00);  //3
	CG_RAM_ADDRESS(0x10|0x03);CG_RAM_DATA(0x1F);  //4
	CG_RAM_ADDRESS(0x10|0x04);CG_RAM_DATA(0x00);  //5
	CG_RAM_ADDRESS(0x10|0x05);CG_RAM_DATA(0x00);  //6
	CG_RAM_ADDRESS(0x10|0x06);CG_RAM_DATA(0x00);  //7
	CG_RAM_ADDRESS(0x10|0x07);CG_RAM_DATA(0x00);  //8

	CG_RAM_ADDRESS(0x18|0x00);CG_RAM_DATA(0x1F);  //1
	CG_RAM_ADDRESS(0x18|0x01);CG_RAM_DATA(0x00);  //2
	CG_RAM_ADDRESS(0x18|0x02);CG_RAM_DATA(0x00);  //3
	CG_RAM_ADDRESS(0x18|0x03);CG_RAM_DATA(0x1F);  //4
	CG_RAM_ADDRESS(0x18|0x04);CG_RAM_DATA(0x00);  //5
	CG_RAM_ADDRESS(0x18|0x05);CG_RAM_DATA(0x00);  //6
	CG_RAM_ADDRESS(0x18|0x06);CG_RAM_DATA(0x00);  //7
	CG_RAM_ADDRESS(0x18|0x07);CG_RAM_DATA(0x00);  //8

	CG_RAM_ADDRESS(0x20|0x00);CG_RAM_DATA(0x00);  //1
	CG_RAM_ADDRESS(0x20|0x01);CG_RAM_DATA(0x00);  //2
	CG_RAM_ADDRESS(0x20|0x02);CG_RAM_DATA(0x00);  //3
	CG_RAM_ADDRESS(0x20|0x03);CG_RAM_DATA(0x00);  //4
	CG_RAM_ADDRESS(0x20|0x04);CG_RAM_DATA(0x00);  //5
	CG_RAM_ADDRESS(0x20|0x05);CG_RAM_DATA(0x00);  //6
	CG_RAM_ADDRESS(0x20|0x06);CG_RAM_DATA(0x1F);  //7
	CG_RAM_ADDRESS(0x20|0x07);CG_RAM_DATA(0x00);  //8

	CG_RAM_ADDRESS(0x28|0x00);CG_RAM_DATA(0x1F);  //1
	CG_RAM_ADDRESS(0x28|0x01);CG_RAM_DATA(0x00);  //2
	CG_RAM_ADDRESS(0x28|0x02);CG_RAM_DATA(0x00);  //3
	CG_RAM_ADDRESS(0x28|0x03);CG_RAM_DATA(0x00);  //4
	CG_RAM_ADDRESS(0x28|0x04);CG_RAM_DATA(0x00);  //5
	CG_RAM_ADDRESS(0x28|0x05);CG_RAM_DATA(0x00);  //6
	CG_RAM_ADDRESS(0x28|0x06);CG_RAM_DATA(0x1F);  //7
	CG_RAM_ADDRESS(0x28|0x07);CG_RAM_DATA(0x00);  //8

	CG_RAM_ADDRESS(0x30|0x00);CG_RAM_DATA(0x00);  //1
	CG_RAM_ADDRESS(0x30|0x01);CG_RAM_DATA(0x00);  //2
	CG_RAM_ADDRESS(0x30|0x02);CG_RAM_DATA(0x00);  //3
	CG_RAM_ADDRESS(0x30|0x03);CG_RAM_DATA(0x1F);  //4
	CG_RAM_ADDRESS(0x30|0x04);CG_RAM_DATA(0x00);  //5
	CG_RAM_ADDRESS(0x30|0x05);CG_RAM_DATA(0x00);  //6
	CG_RAM_ADDRESS(0x30|0x06);CG_RAM_DATA(0x1F);  //7
	CG_RAM_ADDRESS(0x30|0x07);CG_RAM_DATA(0x00);  //8

	CG_RAM_ADDRESS(0x38|0x00);CG_RAM_DATA(0x1F);  //1
	CG_RAM_ADDRESS(0x38|0x01);CG_RAM_DATA(0x00);  //2
	CG_RAM_ADDRESS(0x38|0x02);CG_RAM_DATA(0x00);  //3
	CG_RAM_ADDRESS(0x38|0x03);CG_RAM_DATA(0x1F);  //4
	CG_RAM_ADDRESS(0x38|0x04);CG_RAM_DATA(0x00);  //5
	CG_RAM_ADDRESS(0x38|0x05);CG_RAM_DATA(0x00);  //6
	CG_RAM_ADDRESS(0x38|0x06);CG_RAM_DATA(0x1F);  //7
	CG_RAM_ADDRESS(0x38|0x07);CG_RAM_DATA(0x00);  //8

	CG_RAM_ADDRESS(0x40|0x00);CG_RAM_DATA(0x1F);  //1
	CG_RAM_ADDRESS(0x40|0x01);CG_RAM_DATA(0x1F);  //2
	CG_RAM_ADDRESS(0x40|0x02);CG_RAM_DATA(0x1F);  //3
	CG_RAM_ADDRESS(0x40|0x03);CG_RAM_DATA(0x1F);  //4
	CG_RAM_ADDRESS(0x40|0x04);CG_RAM_DATA(0x1F);  //5
	CG_RAM_ADDRESS(0x40|0x05);CG_RAM_DATA(0x1F);  //6
	CG_RAM_ADDRESS(0x40|0x06);CG_RAM_DATA(0x1F);  //7
	CG_RAM_ADDRESS(0x40|0x07);CG_RAM_DATA(0x1F);  //8

}

