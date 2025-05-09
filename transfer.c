/*
 * transfer.c
 *
 *  Created on: Jun 17, 2020
 *      Author: hhwang
 */

#include "main.h"
#include "project.h"
#include "communicator.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void TransByArray(uint8_t* data, uint8_t type)
{
	TransByArrayEx(data, type, 0, 0, 1);
}

void TransByArrayEx(uint8_t* data, uint8_t type, uint8_t target, uint8_t targetflag, uint8_t iscrc)
{
	Communicator* com;
	char temp_crc[8];
	//char temp[MAXUARTBUF];


	if(strlen((char*)data) < 1) return;

	switch(type)
	{
	case 1:  com = pj->ComRasp; break;
	case 2:  com = pj->ComAux; break;
	case 3:  com = pj->ComPcu; break;
	default: return;
	}

//	strcpy(temp,(char*)data);
	char* temp = (char*)data;
	if(iscrc==1)
	{
		uint16_t len = 0;
		for(int i = 0; i<strlen((char*)temp);i++){
			if(temp[i]=='\r'||temp[i]=='\n'){
				len = i-1;
				break;
			}else{
				len = i;
			}
		}

		temp[len]=',';
		if(!targetflag) temp[len+1]=com->tx_idx+0x30;//아스키코드 변환
		else temp[len+1]=(char)target+0x30;
		temp[len+2]=0;
		com->tx_idx++;
		com->tx_idx=com->tx_idx%5;

		int crc = CalculateCRC((uint8_t*)temp,strlen(temp));
		sprintf(temp_crc,"*%04X\r\n",crc);
		strcat(temp, temp_crc);

		if(type==1) HAL_UART_Transmit_DMA(&huart1, (uint8_t*)temp, strlen((char*)temp));
		else if((type==2)) HAL_UART_Transmit_DMA(&huart4, (uint8_t*)temp, strlen((char*)temp));
		//else if((type==3)) HAL_UART_Transmit_DMA(&huart6, (uint8_t*)temp, strlen((char*)temp));
	}
	else{
		uint16_t len = 0;
		for(int i = 0; i<strlen((char*)data);i++){
			if(data[i]=='\n'){
				len = i + 1;
				break;
			}
		}

		if(type==1) HAL_UART_Transmit_DMA(&huart1, (uint8_t*)data,len);
		else if((type==2)) HAL_UART_Transmit_DMA(&huart4, (uint8_t*)data, len);
		//else if((type==3)) HAL_UART_Transmit_DMA(&huart6, (uint8_t*)data, len);
	}
}
