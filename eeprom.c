/*
 * eeprom.c
 *
 *  Created on: Jun 8, 2020
 *      Author: hhwang
 */



/*
 * eeprom.c
 *
 * Created: 2019-06-11 오후 3:18:43
 *  Author: hhwang
 */


#include <string.h>
#include <main.h>
#include "eeprom.h"

#define EE93C56A		0
#define EE93C56B		1
#define SPI_DELAY		32

#define EESEL_LOW()		HAL_GPIO_WritePin(GPIOH, EESEL_Pin, GPIO_PIN_RESET)
#define EESEL_HIGH()	HAL_GPIO_WritePin(GPIOH, EESEL_Pin, GPIO_PIN_SET)
#define EECLK_LOW()		HAL_GPIO_WritePin(GPIOI, EECLK_Pin, GPIO_PIN_RESET)
#define EECLK_HIGH()	HAL_GPIO_WritePin(GPIOI, EECLK_Pin, GPIO_PIN_SET)
#define EEDO_LOW()		HAL_GPIO_WritePin(GPIOH, EEDO_Pin, GPIO_PIN_RESET)
#define EEDO_HIGH()		HAL_GPIO_WritePin(GPIOH, EEDO_Pin, GPIO_PIN_SET)
#define EECS_A_LOW()	HAL_GPIO_WritePin(GPIOH, EECSA_Pin, GPIO_PIN_RESET)
#define EECS_A_HIGH()	HAL_GPIO_WritePin(GPIOH, EECSA_Pin, GPIO_PIN_SET)
#define EECS_B_LOW()	HAL_GPIO_WritePin(GPIOI, EECSB_Pin, GPIO_PIN_RESET)
#define EECS_B_HIGH()	HAL_GPIO_WritePin(GPIOI, EECSB_Pin, GPIO_PIN_SET)
#define EEDI_A()		HAL_GPIO_ReadPin(GPIOH, EEDIA_Pin)
#define EEDI_B()		HAL_GPIO_ReadPin(GPIOI, EEDIB_Pin)



void Write93C56(uint8_t ab);
void Read93C56Ex(uint8_t ab, uint8_t* data, uint8_t sp, uint8_t len);

void spi_bit_tx ( uint8_t bit );
uint8_t spi_bit_rx(uint8_t ab);
void ewen_set_bitbang (uint8_t ab);
uint8_t data_read_bitbang ( uint8_t addr, uint8_t ab );
void data_write_bitbang ( uint8_t addr, uint8_t data, uint8_t ab);


//--------------------------------------------------------------------------------------
static unsigned char rawData[256] = {
	//	    |        mac address          |
	0xA5,0x00,0x0C,0xFB,0x01,0x02,0x04,0x01,  //0x0000
	0x04,0x05,0x09,0x04,0x0A,0x1D,0x00,0x00,  //0x0008
	0x00,0x00,0x00,0x00,0x00,0x00,0x12,0x22,  //0x0010
	0x12,0x2B,0x12,0x34,0x12,0x3D,0x00,0x00,  //0x0018
	0x24,0x04,0x14,0x95,0x00,0x01,0x9B,0x18,  //0x0020
	0x00,0x02,0x00,0x00,0x01,0x00,0x01,0x00,  //0x0028
	0x32,0x00,0x00,0x00,0x00,0x00,0x21,0x43,  //0x0030
	0x05,0x01,0x0A,0x03,0x53,0x00,0x4D,0x00,  //0x0038
	0x53,0x00,0x43,0x00,0x12,0x01,0x00,0x02,  //0x0040
	0xFF,0x00,0x01,0x40,0x24,0x04,0x00,0xEC,  //0x0048
	0x00,0x01,0x01,0x00,0x00,0x01,0x09,0x02,  //0x0050
	0x27,0x00,0x01,0x01,0x00,0xE0,0x01,0x09,  //0x0058
	0x04,0x00,0x00,0x03,0xFF,0x00,0xFF,0x00,  //0x0060
	0x12,0x01,0x00,0x02,0xFF,0x00,0xFF,0x40,  //0x0068
	0x24,0x04,0x00,0xEC,0x00,0x01,0x01,0x00,  //0x0070
	0x00,0x01,0x09,0x02,0x27,0x00,0x01,0x01,  //0x0078
	0x00,0xE0,0x01,0x09,0x04,0x00,0x00,0x03,  //0x0080
	0xFF,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,  //0x0088
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x0090
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x0098
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00A0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00A8
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00B0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00B8
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00C0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00C8
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00D0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00D8
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00E0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00E8
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,   //0x00F0
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00   //0x00F8
};


void TestEeprom(){

	int ab = 1;

	/*
	EESEL_HIGH();
	ewen_set_bitbang(ab);
	data_write_bitbang ( 0, 0xAA, ab);
	EESEL_LOW();
	 */

	EESEL_HIGH();
	ewen_set_bitbang(ab);
	int rd = data_read_bitbang ( 0, ab );
	EESEL_LOW();

#if 0
	if(rd==0xA5)  HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_SET);
#endif
}

int Check_Eeprom(uint8_t ab)
{
	uint8_t a5;
	// 74HC157 MUX Set CPU
	EESEL_HIGH();
	a5 = data_read_bitbang ( 0x0, ab );
	EESEL_LOW();
	if(a5 == 0xA5) return 1;
	return 0;
}//EEPROM에 0xA5가 있는지 확인합니다.맞으면 1 아니면 0 반환

void Init_Eeprom(void)
{
	if(!Check_Eeprom(EE93C56A))
	{
		Write93C56(EE93C56A);
		Write93C56(EE93C56B);
	}
}//EEPROM을 초기화


#if 0

void SetMacAddr(unsigned char *mac)
{
	int i;
	for(i=0;i<6;i++)
	{
		rawData[i+1] = mac[i];

	}
}

void GetMacAddr(unsigned char *mac)
{
	memset(mac, 0, 6);

	int i;
	unsigned char temp;
	EESEL_HIGH();
	for(i=0;i<6;i++)
	{
		mac[i] = data_read_bitbang(i+1, EE93C56A);
	}
	EESEL_LOW();
}

void WriteMacAddr(uint8_t ab, uint8_t* data)
{
	uint8_t i, temp[6];
	//Write mac address
	Write93C56Ex(ab,data,1,6);
	Read93C56Ex(ab,temp,1,6);
}


#endif

void Write93C56(uint8_t ab)
{
	int i;

	EESEL_HIGH();//EEPROM칩 선택 상태

	ewen_set_bitbang (ab);//쓰기 가능상태 설정

	DelayUs(SPI_DELAY);
	for ( i = 0; i < 256; i ++ )
	{
		if(EE93C56A==0&&i==2)
			data_write_bitbang ( i,rawData[i]+1,ab);
		else
			data_write_bitbang ( i,rawData[i],ab);
	}



	EESEL_LOW();
}

void Write93C56Ex(uint8_t ab, uint8_t* data, uint8_t sp, uint8_t len)
{
	int i, idx=0;

	EESEL_HIGH();

	ewen_set_bitbang (ab);

	DelayUs(SPI_DELAY);
	for ( i = sp; i < sp+len; i ++ )
	{
		data_write_bitbang ( i,data[idx++],ab);
	}

	EESEL_LOW();
}//지정된 범위까지 EEPROM에 데이터를 쓴다

void Read93C56Ex(uint8_t ab, uint8_t* data, uint8_t sp, uint8_t len)
{
	int i, idx=0;

	EESEL_HIGH();

	ewen_set_bitbang (ab);

	DelayUs(SPI_DELAY);
	for ( i = sp; i < sp+len; i ++ )
	{
		data[idx++] = data_read_bitbang ( i,ab);
	}

	EESEL_LOW();
}//지정된 범위까지 EEPROM에 데이터를 읽는다


//--------------------------------------------------------------------------------------
void spi_bit_tx ( uint8_t bit )
{
	bit ? EEDO_HIGH() : EEDO_LOW() ;
	DelayUs(SPI_DELAY);

	EECLK_HIGH();//클럭 하이
	DelayUs(SPI_DELAY);

	EECLK_LOW();//클럭 LOW
	DelayUs(SPI_DELAY);

	EEDO_LOW() ;//데이터 출력을 LOW
	DelayUs(SPI_DELAY);
}//SPI를 통해 한비트를 전송한다.  주어진 비트를 SPI 출력핀 EEDO로 전송한다. 비트를 전송하고 클럭 신호를 생성하여 데이터가 전송되도록 한다

uint8_t spi_bit_rx(uint8_t ab)
{
	uint8_t d;

	EECLK_HIGH();
	DelayUs(SPI_DELAY);

	d = ab ? EEDI_B() : EEDI_A();//데이터 입력을 읽는다

	EECLK_LOW();//클럭을 LOW로 설정
	DelayUs(SPI_DELAY);

	return d ? 1 : 0;// 읽은 값을 반환
}//SPI데이터 입력핀에서 한비트씩 데이터를 읽고 클럭신호를 생성한다.

uint8_t data_read_bitbang ( uint8_t addr, uint8_t ab )
{
	int i;
	uint8_t rx = 0;

	ab ? EECS_B_HIGH() : EECS_A_HIGH();//EEPROM칩 선택

	spi_bit_tx ( 1 );       // Start

	spi_bit_tx ( 1 );       // Read Op code
	spi_bit_tx ( 0 );

	spi_bit_tx ( 0 );       // for extra clock
	for ( i = 7; i >= 0; i-- )
	spi_bit_tx ( (addr >> i ) & 1 );//주소 비트 전송

	for ( i = 0; i < 8; i++ )
	{
		rx <<= 1;
		rx |= spi_bit_rx (ab);
	}//데이터 비트 수신

	EECS_A_LOW();//칩 선택 해제
	EECS_B_LOW();
	return rx;
}//EEPROM을 선택하고 읽기 작업을 시작하는 비트를 전송합니다 그 다음 주소비트를 전송하고 데이터를 읽어옵니다

/* enable erase/write */
void ewen_set_bitbang (uint8_t ab)
{
	int i;

	ab ? EECS_B_HIGH() : EECS_A_HIGH();

	spi_bit_tx ( 1 );//START 명령 전송

	spi_bit_tx ( 0 );// EWEN 명령 전송
	spi_bit_tx ( 0 );

	for ( i = 0; i < 9; i++ )
	spi_bit_tx ( 1 );// 명령 비트 전송

	EECS_A_LOW(); // EEPROM 칩 선택 해제
	EECS_B_LOW();

	DelayUs(SPI_DELAY);
}//EEPROM의 쓰기 가능 상태를 설정하기 위해 EWEN(Erase/Write Enable) 명령을 전송합니다.

void data_write_bitbang ( uint8_t addr, uint8_t data, uint8_t ab)
{
	int i;

	ab ? EECS_B_HIGH() : EECS_A_HIGH();

	spi_bit_tx ( 1 );       // start

	spi_bit_tx ( 0 );       // opcode
	spi_bit_tx ( 1 );
	spi_bit_tx ( 0 );       // for dummy
	for ( i = 7; i >= 0; i-- )
	spi_bit_tx ( (addr >> i ) & 1 );

	for ( i = 7; i >= 0; i-- )
	spi_bit_tx ( (data >> i ) & 1 );

	EECS_A_LOW();
	EECS_B_LOW();

	/* one fake clock */
	EECLK_HIGH();
	DelayUs(SPI_DELAY);

	EECLK_LOW();
	DelayUs(SPI_DELAY);

	ab ? EECS_B_HIGH() : EECS_A_HIGH();

	/* clock 'til ready */
	EECLK_HIGH();
	DelayUs(SPI_DELAY);
	while ( !(ab ? EEDI_B() : EEDI_A()) )
	{
		EECLK_HIGH();
		DelayUs(SPI_DELAY);

		EECLK_LOW();
		DelayUs(SPI_DELAY);
	}

	EECS_A_LOW();
	EECS_B_LOW();
}
// EEPROM을 선택하고, 쓰기 작업을 시작하는 비트를 전송합니다. 그런 다음, 주소와 데이터를 전송하고, 데이터가 EEPROM에 기록될 때까지 기다립니다.

