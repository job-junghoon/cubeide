/*
 * util.c
 *
 *  Created on: Jun 16, 2020
 *      Author: hhwang
 */

#include <string.h>
#include <stdlib.h>

unsigned short CalculateCRC(unsigned char data[], unsigned int length)
{
	unsigned int i;
	unsigned short crc = 0;
	for(i=0; i<length; i++)
	{
		crc = (unsigned char)(crc >> 8) | (crc << 8);
		crc ^= data[i];
		crc ^= (unsigned char)(crc & 0xff) >> 4;
		crc ^= crc << 12;
		crc ^= (crc & 0x00ff) << 5;

	}
	return crc;
}

int CheckChecksum(unsigned char *Array, int len)
{
	unsigned int CRC_Value,CRC_Value_1,strLen;
	char hex_char[5];
	strLen=len;
	if( strLen>1000 || strLen<10)return 0;
	hex_char[0]=Array[strLen-6];
	hex_char[1]=Array[strLen-5];
	hex_char[2]=Array[strLen-4];
	hex_char[3]=Array[strLen-3];
	hex_char[4]='\0';
	CRC_Value_1=strtol(hex_char,'\0',16);
	CRC_Value=CalculateCRC((unsigned char *)Array,strLen-7);

	if(CRC_Value==CRC_Value_1)	return 1;
	else 						return 0;
}
