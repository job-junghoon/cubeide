/*
 * main_worker.c
 *
 *  Created on: Jun 16, 2020
 *      Author: hhwang
 */
#include "main.h"
#include "project.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void RefleshSystemInfo();

int Tick10msCnt =0;
int ReadyToSystemStatus =0;

void main_worker(void)
{

	if(ProcRxRingBuf(pj->ComRasp)<0) 		HAL_UART_Receive_IT(&huart1, pj->ComRasp->Ring.buf + pj->ComRasp->Ring.in, 1);
	if(ProcRxRingBufEx(pj->ComAux, 0)<0) 	HAL_UART_Receive_IT(&huart4, pj->ComAux->Ring.buf + pj->ComAux->Ring.in, 1);
	if(ProcRxRingBuf(pj->ComPcu)<0) 		HAL_UART_Receive_IT(&huart6, pj->ComPcu->Ring.buf + pj->ComPcu->Ring.in, 1);

	if(pj->Tick1s){
		pj->ResetTick1s(), HAL_GPIO_TogglePin(LED_ACT_GPIO_Port, LED_ACT_Pin);
	}

	if(pj->Tick100ms){
		pj->ResetTick100ms();
		pj->DoBlink();
		/* Display */
		//Check System
		DisplayCheckSystem();
		//Side LED3
		CheckFrontLED3();


	}

	if(pj->Tick10ms){
		pj->ResetTick10ms();


		if(++Tick10msCnt>=5){
			Tick10msCnt = 0;
			ReadyToSystemStatus = 1;
		}

		if(pj->ComRasp->TxReady==1){

			char* dat = (char*)pj->ComRasp->TxBuf;
			if((strncmp(dat,"$KW",3)==0)||(strncmp(dat,"$KB",3)==0))
			{
				 ProcRaspRxData(dat);
			}else{
				strcpy((char*)pj->ComAux->ProcBuf, (char*)pj->ComRasp->TxBuf);
				HAL_UART_Transmit_DMA(&huart4, (uint8_t*)pj->ComAux->ProcBuf, strlen((char*)pj->ComAux->ProcBuf));
			}
			pj->ComRasp->TxReady=0;

		}


		if(pj->ComAux->TxReady==1){
			char* dat = (char*)pj->ComAux->TxBuf;
			HAL_UART_Transmit_DMA(&huart1, (uint8_t*)dat, strlen(dat));
			pj->ComAux->TxReady=0;
		}else if(ReadyToSystemStatus==1){
			ReadyToSystemStatus = 0;

			RefleshSystemInfo();
			char* txbuf = (char*)pj->ComRasp->ProcBuf;
			int len = sprintf(txbuf, "$KB,SYS,%c%c%c%c,%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c,%c,%c" \
					",%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%c%c%c%c%c,%d,%.2f",
					pj->RdIO->Sw[0]?'1':'0',
					pj->RdIO->Sw[1]?'2':'0',
					pj->RdIO->Sw[2]?'3':'0',
					pj->RdIO->Sw[3]?'4':'0',
					pj->RdIO->Key[0]?'1':'0',
					pj->RdIO->Key[1]?'2':'0',
					pj->RdIO->Key[2]?'3':'0',
					pj->RdIO->Key[3]?'4':'0',
					pj->RdIO->Key[4]?'5':'0',
					pj->RdIO->Key[5]?'P':'0',
					pj->RdIO->Key[6]?'6':'0',
					pj->RdIO->Key[7]?'7':'0',
					pj->RdIO->Key[8]?'8':'0',
					pj->RdIO->Key[9]?'9':'0',
					pj->RdIO->Key[10]?'Z':'0',
					pj->RdIO->Key[11]?'.':'0',
					pj->RdIO->Key[12]?'C':'0',
					pj->RdIO->Key[13]?'U':'0',
					pj->RdIO->Key[14]?'D':'0',
					pj->RdIO->Key[15]?'B':'0',
					pj->RdIO->Key[16]?'F':'0',
					pj->RdIO->Key[17]?'E':'0',
					pj->RdIO->DecRefSig?'D':'0',
					pj->RdIO->DecSelDash?'S':'0',
					(int)adcVal[0]&0x0000FFFF,
					(int)(adcVal[0]&0xFFFF0000)>>16,
					(int)adcVal[1]&0x0000FFFF,
					(int)(adcVal[1]&0xFFFF0000)>>16,
					(int)adcVal[2]&0x0000FFFF,
					(int)(adcVal[2]&0xFFFF0000)>>16,
					(int)adcVal[3]&0x0000FFFF,
					(int)(adcVal[3]&0xFFFF0000)>>16,
					pj->RdIO->DecRefSigFromBDMUX?'D':'0',
					pj->RdIO->DecEMCON[0]?'A':'0',
					pj->RdIO->DecEMCON[1]?'B':'0',
					pj->RdIO->DecEMCON[2]?'C':'0',
					pj->RdIO->DecEMCON[3]?'D':'0',
					pj->Nmea2000->IsConnect, //pj->Nmea2000->ConnectCnt,//
					pj->Nmea2000->Heading
			);

			txbuf[len]='\0';
			TransByArray((uint8_t*)txbuf,1);
		}

		if(pj->ComPcu->TxReady==1){
			char* dat = (char*)pj->ComPcu->TxBuf;
			ProcPcuRxData(dat);
			pj->ComPcu->TxReady=0;
		}

	}

#if 0
	if(pj->Tick1s){
		pj->ResetTick1s();
		//HAL_GPIO_TogglePin(LED_ACT_GPIO_Port, LED_ACT_Pin);

		//HAL_GPIO_TogglePin(GPIOH, LED_GPS_Pin);
		//HAL_GPIO_TogglePin(GPIOH, LED_TRK_Pin);
		//HAL_GPIO_TogglePin(GPIOH, LED_ERR_Pin);

	}



	HAL_UART_Receive_IT(&huart6, pj->ComPcu->Ring.buf + pj->ComPcu->Ring.in, 1);


	if(pj->Tick100ms){
		RefleshSystemInfo();
		pj->ResetTick100ms();
		pj->Nmea2000->CheckConnection();

		//ComRasp
		while(!IsEmptyMsgQueue(&pj->ComRasp->RxQueue))
		{
			DeMsgQueue(&pj->ComRasp->RxQueue, rxbuf);
			ProcRaspRxData(rxbuf);
		}

		if(!IsEmptyMsgQueue(&pj->ComRasp->TxQueue))
		{
			DeMsgQueue(&pj->ComRasp->TxQueue, txbuf);
			HAL_UART_Transmit_DMA(&huart1, (uint8_t*)txbuf, strlen(txbuf));
		}

		//ComPcu, RX only
		while(!IsEmptyMsgQueue(&pj->ComPcu->RxQueue))
		{
			DeMsgQueue(&pj->ComPcu->RxQueue, rxbuf);
			ProcPcuRxData(rxbuf);
		}


		/* Display */
		//Check System
		DisplayCheckSystem();
		//Side LED3
		CheckFrontLED3();

		//Blink
		blink_cnt++;
		if(blink_cnt==5){
			blink_cnt = 0;
			pj->Blink=!pj->Blink;
		}
	}

#endif
}


void RefleshSystemInfo()
{

	pj->RdIO->Sw[0] = HAL_GPIO_ReadPin(GPIOD, SW_0_Pin)>0?0:1;
	pj->RdIO->Sw[1] = HAL_GPIO_ReadPin(GPIOD, SW_1_Pin)>0?0:1;
	pj->RdIO->Sw[2] = HAL_GPIO_ReadPin(GPIOD, SW_2_Pin)>0?0:1;
	pj->RdIO->Sw[3] = HAL_GPIO_ReadPin(GPIOD, SW_3_Pin)>0?0:1;

	pj->RdIO->Key[0] = HAL_GPIO_ReadPin(GPIOE, KEY_1_Pin)>0?0:1;
	pj->RdIO->Key[1] = HAL_GPIO_ReadPin(GPIOE, KEY_2_Pin)>0?0:1;
	pj->RdIO->Key[2] = HAL_GPIO_ReadPin(GPIOE, KEY_3_Pin)>0?0:1;
	pj->RdIO->Key[3] = HAL_GPIO_ReadPin(GPIOE, KEY_4_Pin)>0?0:1;
	pj->RdIO->Key[4] = HAL_GPIO_ReadPin(GPIOE, KEY_5_Pin)>0?0:1;
	pj->RdIO->Key[5] = HAL_GPIO_ReadPin(GPIOE, KEY_6_Pin)>0?0:1;
	pj->RdIO->Key[6] = HAL_GPIO_ReadPin(GPIOB, KEY_7_Pin)>0?0:1;
	pj->RdIO->Key[7] = HAL_GPIO_ReadPin(GPIOB, KEY_8_Pin)>0?0:1;
	pj->RdIO->Key[8] = HAL_GPIO_ReadPin(GPIOH, KEY_9_Pin)>0?0:1;
	pj->RdIO->Key[9] = HAL_GPIO_ReadPin(GPIOH, KEY_10_Pin)>0?0:1;
	pj->RdIO->Key[10] = HAL_GPIO_ReadPin(GPIOH, KEY_11_Pin)>0?0:1;
	pj->RdIO->Key[11] = HAL_GPIO_ReadPin(GPIOH, KEY_12_Pin)>0?0:1;
	pj->RdIO->Key[12] = HAL_GPIO_ReadPin(GPIOH, KEY_13_Pin)>0?0:1;
	pj->RdIO->Key[13] = HAL_GPIO_ReadPin(GPIOH, KEY_14_Pin)>0?0:1;
	pj->RdIO->Key[14] = HAL_GPIO_ReadPin(GPIOH, KEY_15_Pin)>0?0:1;
	pj->RdIO->Key[15] = HAL_GPIO_ReadPin(GPIOB, KEY_16_Pin)>0?0:1;
	pj->RdIO->Key[16] = HAL_GPIO_ReadPin(GPIOB, KEY_17_Pin)>0?0:1;
	pj->RdIO->Key[17] = HAL_GPIO_ReadPin(GPIOB, KEY_18_Pin)>0?0:1;

	pj->RdIO->DecRefSig = HAL_GPIO_ReadPin(GPIOE, DEC_REFSIG_Pin)>0?0:1;
	pj->RdIO->DecSelDash = HAL_GPIO_ReadPin(GPIOE, CON_R_Pin)>0?0:1;

	pj->RdIO->DecRefSigFromBDMUX = HAL_GPIO_ReadPin(GPIOI, DecRefSigFromBDMUX_Pin)>0?0:1;
	pj->RdIO->DecEMCON[0] = HAL_GPIO_ReadPin(GPIOC, DecEmconA_Pin)>0?0:1;
	pj->RdIO->DecEMCON[1] = HAL_GPIO_ReadPin(GPIOI, DecEmconB_Pin)>0?0:1;
	pj->RdIO->DecEMCON[2] = HAL_GPIO_ReadPin(GPIOI, DecEmconC_Pin)>0?0:1;
	pj->RdIO->DecEMCON[3] = 'D';//HAL_GPIO_ReadPin(GPIOI, DecEmconD_Pin)>0?0:1;

#if 0

	int len = sprintf(txbuf, "$KB,SYS,%c%c%c%c,%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c,%c,%c" \
			",%04d,%04d,%04d,%04d,%04d,%04d,%04d,%04d,%c%c%c%c%c,%d,%.2f",
			pj->RdIO->Sw[0]?'1':'0',
			pj->RdIO->Sw[1]?'2':'0',
			pj->RdIO->Sw[2]?'3':'0',
			pj->RdIO->Sw[3]?'4':'0',
			pj->RdIO->Key[0]?'1':'0',
			pj->RdIO->Key[1]?'2':'0',
			pj->RdIO->Key[2]?'3':'0',
			pj->RdIO->Key[3]?'4':'0',
			pj->RdIO->Key[4]?'5':'0',
			pj->RdIO->Key[5]?'P':'0',
			pj->RdIO->Key[6]?'6':'0',
			pj->RdIO->Key[7]?'7':'0',
			pj->RdIO->Key[8]?'8':'0',
			pj->RdIO->Key[9]?'9':'0',
			pj->RdIO->Key[10]?'Z':'0',
			pj->RdIO->Key[11]?'.':'0',
			pj->RdIO->Key[12]?'C':'0',
			pj->RdIO->Key[13]?'U':'0',
			pj->RdIO->Key[14]?'D':'0',
			pj->RdIO->Key[15]?'B':'0',
			pj->RdIO->Key[16]?'F':'0',
			pj->RdIO->Key[17]?'E':'0',
			pj->RdIO->DecRefSig?'D':'0',
			pj->RdIO->DecSelDash?'S':'0',
			(int)adcVal[0]&0x0000FFFF,
			(int)(adcVal[0]&0xFFFF0000)>>16,
			(int)adcVal[1]&0x0000FFFF,
			(int)(adcVal[1]&0xFFFF0000)>>16,
			(int)adcVal[2]&0x0000FFFF,
			(int)(adcVal[2]&0xFFFF0000)>>16,
			(int)adcVal[3]&0x0000FFFF,
			(int)(adcVal[3]&0xFFFF0000)>>16,
			pj->RdIO->DecRefSigFromBDMUX?'D':'0',
			pj->RdIO->DecEMCON[0]?'A':'0',
			pj->RdIO->DecEMCON[1]?'B':'0',
			pj->RdIO->DecEMCON[2]?'C':'0',
			pj->RdIO->DecEMCON[3]?'D':'0',
			pj->Nmea2000->IsConnect, //pj->Nmea2000->ConnectCnt,//
			pj->Nmea2000->Heading
	);

	txbuf[len]='\0';

	TransByArray((uint8_t*)txbuf, 1);
#endif
}
