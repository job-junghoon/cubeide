/*
 * nmea2000.c
 *
 *  Created on: 2020. 9. 7.
 *      Author: hhwang
 */
#include "main.h"
#include "nmea2000.h"


NMEA2000 Nmea2000;


void CheckNmea2000Connection(){
	if(Nmea2000.ConnectCnt-- <= 0){
		Nmea2000.IsConnect=0;
		Nmea2000.ConnectCnt=0;
	}
}

void ResetNmea2000Connection(){
	Nmea2000.ConnectCnt = 20;
	Nmea2000.IsConnect = 1;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	if(hcan->Instance == CAN1){

		CAN_RxHeaderTypeDef pRxHeader;
		uint16_t temp=0;
		uint8_t rxData[8];

		HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &pRxHeader, &rxData[0]);

		if(0x9f11260==pRxHeader.ExtId){ /* Heading */
		//if(0xdf11960==pRxHeader.ExtId){ /* ? */
			temp=rxData[2]<<8|rxData[1];
			//heading=temp*0.0001*180/3.14;

			Nmea2000.Heading=temp*0.0001*180/3.14;
			ResetNmea2000Connection();
		}
	}
}
