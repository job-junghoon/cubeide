/*
 * disp.c
 *
 *  Created on: Jun 19, 2020
 *      Author: hhwang
 */

#include "main.h"
#include "project.h"
#include "sysdef.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "40t202da.h"

int DisplayVfdLogo();
int DisplayVfdCheckSystem();
int DisplayLEDLogo();
int DisplayLEDCheckSystem();

int DisplayLogo()
{
	int reval = 0;

	if(pj->RdIO->DecSelDash==1) reval = DisplayVfdLogo();
	else reval = DisplayLEDLogo();


	return reval;
}

int DisplayCheckSystem()
{
	int reval = 0;

	if(pj->RdIO->DecSelDash==1) reval = DisplayVfdCheckSystem();
	else reval = DisplayLEDCheckSystem();


	return reval;
}

void SetLED(uint8_t* d){
	HAL_GPIO_WritePin(GPIOF, VFD_RS_Pin, (GPIO_PinState)d[0]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_WR_Pin, (GPIO_PinState)d[1]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_RD_Pin, (GPIO_PinState)d[2]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_D0_Pin, (GPIO_PinState)d[3]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_D1_Pin, (GPIO_PinState)d[4]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_D2_Pin, (GPIO_PinState)d[5]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_D3_Pin, (GPIO_PinState)d[6]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_D4_Pin, (GPIO_PinState)d[7]>0?0:1);
	HAL_GPIO_WritePin(GPIOF, VFD_D5_Pin, (GPIO_PinState)d[8]>0?0:1);
}

void InitLED(){
	HAL_GPIO_WritePin(GPIOF, VFD_RS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_WR_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_RD_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_D0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_D1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_D2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_D3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_D4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOF, VFD_D5_Pin, GPIO_PIN_RESET);
}

int DisplayLEDLogo(){

	static uint8_t led_logo_idx=0;


	uint8_t all_h[10] = {1,1,1,1,1,1,1,1,1,1};
	uint8_t all_l[10] = {0,0,0,0,0,0,0,0,0,0};

	if(led_logo_idx==0)			InitLED();
	else if(led_logo_idx==3)	SetLED(all_l);
	else if(led_logo_idx==6)	SetLED(all_h);
	else if(led_logo_idx>10&&led_logo_idx<=19){
		int i=0;
		for(i=0;i<led_logo_idx-10;i++) all_l[i]=1;
		SetLED(all_l);
	}
	else if(led_logo_idx==20){
		SetLED(all_l);
		return -1;
	}

	HAL_Delay(200);

	led_logo_idx++;

	return 1;
}

int DisplayVfdLogo(){
	static int DashboardStep = 1;
	static int Random = 30;

	if(pj->ComRasp->rx_ready)  return -1;

	if(DashboardStep==1)
	{
		pj->Vfd->DisplayLine1("");
		pj->Vfd->DisplayLine2("");

		DashboardStep=2;
		Random=28;
	}
	else if(DashboardStep==2)
	{
		int ready = 0;
		int i = 0;
		for(i = 0;i<40;i++){
			//disp[0][i] = 0x08;
			if(Vfd.BufLine[0][i]!=0x06) {
				//if(disp[i]==' ')
				Vfd.BufLine[0][i] = (uint8_t)(rand()%7+1);
				ready++;
			}
			if(Vfd.BufLine[1][i]!=0x06)
			{
				Vfd.BufLine[1][i] = rand()%7+1;
				ready++;
			}
		}

		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		pj->Vfd->DisplayLine2(Vfd.BufLine[1]);


		if(ready <Random) DashboardStep=3;
		//30
		//43

		HAL_Delay(20);
	}
	else if(DashboardStep==3)
	{


		char tmp1[100];
		char tmp2[100];

		if(pj->RdIO->Sw[1])
		{
			char* line0 = "                 АНИВА                   ";
			char* line1 = "        БЛОК УПРАВЛЕНИЯ АНТЕННОЙ         ";
			ConvertStringToRuForSsVfd(0,line0, tmp1);
			while(strlen(tmp1)<40) strcat(tmp1," ");
			ConvertStringToRuForSsVfd(0,line1, tmp2);
			while(strlen(tmp2)<40) strcat(tmp2," ");
		}
		else{

			char* line0 = "               SUPERTRACK                ";
			char* line1 = "          ANTENNA CONTROL UNIT           ";
			strcpy(tmp1, line0);
			strcpy(tmp2, line1);
		}


		char* disp0 = tmp1;
		char* disp1 = tmp2;

		int ready = 0;
		int i = 0;
		//for(i = 0;i<40;i++) disp[0][i] = ' ', disp[1][i] = ' ';

		for(i = 0;i<40;i++){

			if(Vfd.BufLine[0][i]<=9){
				unsigned char tmp_val = rand()%8+1;
				if(tmp_val==1) Vfd.BufLine[0][i] = disp0[i];
				else Vfd.BufLine[0][i] = tmp_val;
				ready++;
			}

			if(Vfd.BufLine[1][i]<=9){
				unsigned char tmp_val = rand()%8+1;
				if(tmp_val==1) Vfd.BufLine[1][i] = disp1[i];
				else Vfd.BufLine[1][i] = tmp_val;
				ready++;
			}
		}


		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		pj->Vfd->DisplayLine2(Vfd.BufLine[1]);

		if(ready == 0) DashboardStep=4;


		HAL_Delay(50);
	}
	else if(DashboardStep==4)
	{
		return -1;
	}


	return 1;
}

int DisplayVfdLogo1(){
	static int DashboardStep = 1;

	if(pj->ComRasp->rx_ready)  return -1;

	if(DashboardStep==1)
	{
		pj->Vfd->DisplayLine1("");
		pj->Vfd->DisplayLine2("");

		DashboardStep=2;
	}
	else if(DashboardStep==2)
	{
		int ready = 0;
		int i = 0;
		for(i = 0;i<40;i++){
			if(Vfd.BufLine[0][i]!='\b') {
				Vfd.BufLine[0][i] = 0x08;
				ready++;
			}
			if(Vfd.BufLine[1][i]!='\b')
			{
				Vfd.BufLine[1][i] = 0x08;
			}
		}
		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		pj->Vfd->DisplayLine2(Vfd.BufLine[1]);

		if(ready <30) DashboardStep=3;

		HAL_Delay(10);
	}
	if(DashboardStep==3)
	{
		char tmp1[100];
		char tmp2[100];

		if(pj->RdIO->Sw[1])
		{
			char* line0 = "                 АНИВА                   ";
			char* line1 = "        БЛОК УПРАВЛЕНИЯ АНТЕННОЙ         ";
			ConvertStringToRuForSsVfd(0,line0, tmp1);
			while(strlen(tmp1)<40) strcat(tmp1," ");
			ConvertStringToRuForSsVfd(0,line1, tmp2);
			while(strlen(tmp2)<40) strcat(tmp2," ");
		}
		else{

			char* line0 = "               SUPERTRACK                ";
			char* line1 = "          ANTENNA CONTROL UNIT           ";
			strcpy(tmp1, line0);
			strcpy(tmp2, line1);
		}


		char* disp0 = tmp1;
		char* disp1 = tmp2;

		int ready = 0;
		int i = 0;
		//for(i = 0;i<40;i++) disp[0][i] = ' ', disp[1][i] = ' ';

		for(i = 0;i<40;i++){

			if(Vfd.BufLine[0][i]<=9){
				unsigned char tmp_val = rand()%8+1;
				if(tmp_val==1) Vfd.BufLine[0][i] = disp0[i];
				else Vfd.BufLine[0][i] = tmp_val;
				ready++;
			}

			if(Vfd.BufLine[1][i]<=9){
				unsigned char tmp_val = rand()%8+1;
				if(tmp_val==1) Vfd.BufLine[1][i] = disp1[i];
				else Vfd.BufLine[1][i] = tmp_val;
				ready++;
			}
		}


		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		pj->Vfd->DisplayLine2(Vfd.BufLine[1]);

		if(ready == 0) DashboardStep=4;

		HAL_Delay(100);
	}
	else if(DashboardStep==4)
	{
		return -1;
	}


	return 1;
}

static uint16_t con_error_swap = 0;
static uint16_t cs_cnt = 0;
static uint8_t rot_idx = 0;
static char rot[4] = {0x2d, 0x5c, 0x7c, 0x2f};
#define ADS_EXT_REF 3.26
#define PWR_R1 		13000
#define PWR_R2		680
#define FAN_R1 		33000
#define FAN_R2		6800
#define PWR_REF(x)	(x/4095.0*3.333333)

int DisplayVfdCheckSystem(){

	char tmp1[100];
	char tmp2[100];
	static int Offset = 2048;

	if(pj->RdIO->Sw[1])
	{
		char* line1 = "              %c ЗАГРУЗКА                ";
		ConvertStringToRuForSsVfd(0,line1, tmp2);
		while(strlen(tmp2)<40) strcat(tmp2," ");
	}
	else{
		char* line1 = "            %c CHECK SYSTEM              ";
		strcpy(tmp2, line1);
	}

	char* disp1 = tmp2;
	if(pj->ComRasp->rx_ready)  return -1;

	rot_idx = ++rot_idx>=4?0:rot_idx;
	sprintf(Vfd.BufLine[1], disp1, rot[rot_idx]);
	pj->Vfd->DisplayLine2(Vfd.BufLine[1]);

#if 0  /* Check the antenna state.  */

	if(cs_cnt>=60&&cs_cnt<130)
	{
		uint16_t adcVal1 = (adcVal[0]&0xFFFF0000)>>16;
		uint16_t adcVal4 = adcVal[2]&0x0000FFFF;
		uint16_t adcVal6 = adcVal[3]&0x0000FFFF;
		uint16_t adcVal7 = (adcVal[3]&0xFFFF0000)>>16;

		float TxCurrent=fabs((float)adcVal1-Offset)/65;
		float RxCurrent=fabs((float)adcVal4-Offset)/65;
		float TxVolt=(float)(PWR_REF(adcVal6)*(PWR_R1+PWR_R2))/PWR_R2;
		float RxVolt=(float)(PWR_REF(adcVal7)*(PWR_R1+PWR_R2))/PWR_R2;


		if(pj->RdIO->Sw[1])
		{
			sprintf(tmp1, " АНТ  %.1fВ %.2fA  /  BUC  %.1fВ %.2fA   ", RxVolt, RxCurrent, TxVolt, TxCurrent);
			ConvertStringToRuForSsVfd(0,tmp1, Vfd.BufLine[0]);
			while(strlen(Vfd.BufLine[0])<40) strcat(Vfd.BufLine[0]," ");
		}
		else{
			sprintf(Vfd.BufLine[0], " ANT  %.1fV %.2fA  /  BUC  %.1fV %.2fA   ", RxVolt, RxCurrent, TxVolt, TxCurrent);
		}
		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		//					  "       (!) ПРОВЕРКА СОЕДИНЕНИЯ          "
		//					  "  ANT  48.0V 0.12A  /  BUC  48.0V 0.12A "
		//					  "            %c CHECK SYSTEM "
	}
	else if(cs_cnt>130)
	{
		uint16_t adcVal2 = adcVal[1]&0x0000FFFF;
		uint16_t adcVal5 = (adcVal[2]&0xFFFF0000)>>16;
		float AcuDeg=((((4096.0-adcVal2)*215.0)/4095.0)-65.0)*(3.0/5.0);
		float FanVolt = (float)(PWR_REF(adcVal5)*(FAN_R1+FAN_R2))/FAN_R2;

		if(pj->RdIO->Sw[1])
		{
			sprintf(tmp1, "  БУА  TEP:%.1fГРД  /  КУЛЕР  %s", AcuDeg , FanVolt<11?"ВКЛ.":"ОШИБКА");
			ConvertStringToRuForSsVfd(0,tmp1, Vfd.BufLine[0]);
			while(strlen(Vfd.BufLine[0])<40) strcat(Vfd.BufLine[0]," ");
		}
		else{
			sprintf(Vfd.BufLine[0], "  ACU  TEP:%.1fDEG  /  FAN   %s", AcuDeg , FanVolt<11?"ON":"ERROR");
		}
		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		//					    "  ANT  48.0V 0.12A  /  BUC  48.0V 0.12A  "
		//					    "  ACU  48.0 DEG     /  FAN    ON-ERROR"
		//					    "            %c CHECK SYSTEM "
		cs_cnt=171;
	}
#else

	if(cs_cnt==30)
	{
		if(pj->ComPcu->rx_ready){
			if(pj->RdIO->Sw[1])
			{
				char* line0 = "           АНТЕННА ПОДКЛЮЧЕНА            ";
				ConvertStringToRuForSsVfd(0,line0, tmp1);
				while(strlen(tmp1)<40) strcat(tmp1," ");
			}
			else{
				char* line0 = "         ANTENNA CONNECTION OK           ";
				strcpy(tmp1, line0);
			}
			pj->Vfd->DisplayLine1(tmp1);
		}
		else{
			if(!con_error_swap){
				if(pj->RdIO->Sw[1])
				{
					char* line0 = "       (!) ПРОВЕРКА СОЕДИНЕНИЯ          ";
					ConvertStringToRuForSsVfd(0,line0, tmp1);
					while(strlen(tmp1)<40) strcat(tmp1," ");
				}
				else{
					char* line0 = "        (!)  CHECK CONNECTION           ";
					strcpy(tmp1, line0);
				}
				pj->Vfd->DisplayLine1(tmp1);
				con_error_swap=1;
			}
			else{
				if(pj->RdIO->Sw[1])
				{
					char* line0 = "       (!) ПРОВЕРКА СОЕДИНЕНИЯ          ";
					ConvertStringToRuForSsVfd(0,line0, tmp1);
					while(strlen(tmp1)<40) strcat(tmp1," ");
				}
				else{
					char* line0 = "      THE ANTENNA IS NOT CONNECTED     ";
					strcpy(tmp1, line0);
				}
				pj->Vfd->DisplayLine1(tmp1);
				con_error_swap=1;
				con_error_swap=0;
			}
			cs_cnt=10;
		}
	}
	else if(cs_cnt>=60&&cs_cnt<100)
	{
		uint16_t adcVal1 = (adcVal[0]&0xFFFF0000)>>16;
		uint16_t adcVal4 = adcVal[2]&0x0000FFFF;
		uint16_t adcVal6 = adcVal[3]&0x0000FFFF;
		uint16_t adcVal7 = (adcVal[3]&0xFFFF0000)>>16;

		float TxCurrent=fabs((float)adcVal1-Offset)/65;
		float RxCurrent=fabs((float)adcVal4-Offset)/65;
		float TxVolt=(float)(PWR_REF(adcVal6)*(PWR_R1+PWR_R2))/PWR_R2;
		float RxVolt=(float)(PWR_REF(adcVal7)*(PWR_R1+PWR_R2))/PWR_R2;


		if(pj->RdIO->Sw[1])
		{
			sprintf(tmp1, " АНТ  %.1fВ %.2fA  /  BUC  %.1fВ %.2fA   ", RxVolt, RxCurrent, TxVolt, TxCurrent);
			ConvertStringToRuForSsVfd(0,tmp1, Vfd.BufLine[0]);
			while(strlen(Vfd.BufLine[0])<40) strcat(Vfd.BufLine[0]," ");
		}
		else{
			sprintf(Vfd.BufLine[0], " ANT  %.1fV %.2fA  /  BUC  %.1fV %.2fA   ", RxVolt, RxCurrent, TxVolt, TxCurrent);
		}
		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		//					  "       (!) ПРОВЕРКА СОЕДИНЕНИЯ          "
		//					  "  ANT  48.0V 0.12A  /  BUC  48.0V 0.12A "
		//					  "            %c CHECK SYSTEM "
	}
	else if(cs_cnt>=100&&cs_cnt<140)
	{
		uint16_t adcVal2 = adcVal[1]&0x0000FFFF;
		uint16_t adcVal5 = (adcVal[2]&0xFFFF0000)>>16;
		float AcuDeg=((((4096.0-adcVal2)*215.0)/4095.0)-65.0)*(3.0/5.0);
		float FanVolt = (float)(PWR_REF(adcVal5)*(FAN_R1+FAN_R2))/FAN_R2;

		if(pj->RdIO->Sw[1])
		{
			sprintf(tmp1, "  БУА  TEP:%.1fГРД  /  КУЛЕР  %s", AcuDeg , FanVolt<11?"ВКЛ.":"ОШИБКА");
			ConvertStringToRuForSsVfd(0,tmp1, Vfd.BufLine[0]);
			while(strlen(Vfd.BufLine[0])<40) strcat(Vfd.BufLine[0]," ");
		}
		else{
			sprintf(Vfd.BufLine[0], "  ACU  TEP:%.1fDEG  /  FAN   %s", AcuDeg , FanVolt<11?"ON":"ERROR");
		}
		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		//					    "  ANT  48.0V 0.12A  /  BUC  48.0V 0.12A  "
		//					    "  ACU  48.0 DEG     /  FAN    ON-ERROR"
		//					    "            %c CHECK SYSTEM "
	}
	else if(cs_cnt>=140)
	{
		uint16_t adcVal2 = adcVal[1]&0x0000FFFF;
		uint16_t adcVal5 = (adcVal[2]&0xFFFF0000)>>16;
		float AcuDeg=((((4096.0-adcVal2)*215.0)/4095.0)-65.0)*(3.0/5.0);
		float FanVolt = (float)(PWR_REF(adcVal5)*(FAN_R1+FAN_R2))/FAN_R2;

		if(pj->RdIO->Sw[1])
		{
			sprintf(tmp1, "  БУА  TEP:%.1fГРД  /  КУЛЕР  %s", AcuDeg , FanVolt<11?"ВКЛ.":"ОШИБКА");
			ConvertStringToRuForSsVfd(0,tmp1, Vfd.BufLine[0]);
			while(strlen(Vfd.BufLine[0])<40) strcat(Vfd.BufLine[0]," ");
		}
		else{
			sprintf(Vfd.BufLine[0], "  ACU  TEP:%.1fDEG  /  FAN   %s", AcuDeg , FanVolt<11?"ON":"ERROR");
		}
		pj->Vfd->DisplayLine1(Vfd.BufLine[0]);
		//					    "  ANT  48.0V 0.12A  /  BUC  48.0V 0.12A  "
		//					    "  ACU  48.0 DEG     /  FAN    ON-ERROR"
		//					    "            %c CHECK SYSTEM "
		cs_cnt=200;
	}
#endif

	cs_cnt++;

	return -1;
}


int DisplayLED9CheckSystem()
{
	 if(pj->ComPcu->rx_ready<1){
		HAL_GPIO_TogglePin(GPIOF, VFD_D3_Pin);
	 }
	 else{
		 /* Front LED 9
		  * 1. IsRun
		  * 2. LedTrack
		  * 3. ModemRxLock
		  * 4. CarrierLock
		  * 5. LedGPS
		  * 6. ErrorCodel
		  * 7. ABSPortLANOpen
		  * */
		uint8_t LedTrack = AntStatus.Tracking==2?pj->Blink:AntStatus.Tracking==1?0:1;
		uint8_t LedGPS = AntStatus.GpsStatus==1?pj->Blink:AntStatus.GpsStatus==2?0:1;

		HAL_GPIO_WritePin(GPIOF, VFD_RS_Pin, (GPIO_PinState)AntStatus.IsRun>0?0:1);
		HAL_GPIO_WritePin(GPIOF, VFD_WR_Pin, (GPIO_PinState)LedTrack);
		HAL_GPIO_WritePin(GPIOF, VFD_RD_Pin, (GPIO_PinState)AntStatus.ModemRxLock>0?0:1);
		HAL_GPIO_WritePin(GPIOF, VFD_D0_Pin, (GPIO_PinState)AntStatus.CarrierLock>0?0:1);
		HAL_GPIO_WritePin(GPIOF, VFD_D1_Pin, (GPIO_PinState)(AntStatus.Tracking==1)&&AntStatus.TxOnOff>0?0:1);
		HAL_GPIO_WritePin(GPIOF, VFD_D2_Pin, (GPIO_PinState)LedGPS);
		HAL_GPIO_WritePin(GPIOF, VFD_D3_Pin, (GPIO_PinState)AntStatus.ErrorCode>0?0:1);
		HAL_GPIO_WritePin(GPIOF, VFD_D4_Pin, (GPIO_PinState)1);
		HAL_GPIO_WritePin(GPIOF, VFD_D5_Pin, (GPIO_PinState)1);
	 }

	return 1;
}

int DisplayLED5CheckSystem()
{
	 if(pj->ComPcu->rx_ready<1){
		HAL_GPIO_TogglePin(GPIOF, VFD_D3_Pin);
	 }
	 else{
		 /* Front LED 9
		  * 1. IsRun
		  * 2. LedTrack
		  * 3. DiSEqC
		  * 4. LedGPS
		  * 5. ErrorCodel
		  * */
		uint8_t LedTrack = AntStatus.Tracking==2?pj->Blink:AntStatus.Tracking==1?0:1;
		uint8_t LedGPS = AntStatus.GpsStatus==1?pj->Blink:AntStatus.GpsStatus==2?0:1;

		HAL_GPIO_WritePin(GPIOF, VFD_RD_Pin, (GPIO_PinState)AntStatus.IsRun>0?0:1);
		HAL_GPIO_WritePin(GPIOF, VFD_D0_Pin, (GPIO_PinState)LedTrack);
		HAL_GPIO_WritePin(GPIOF, VFD_D1_Pin, 1);
		HAL_GPIO_WritePin(GPIOF, VFD_D2_Pin, (GPIO_PinState)LedGPS);
		HAL_GPIO_WritePin(GPIOF, VFD_D3_Pin, (GPIO_PinState)AntStatus.ErrorCode>0?0:1);
	 }

	return 1;
}

int DisplayLEDCheckSystem()
{
	if(pj->ComRasp->rx_ready)  return -1;

	if(pj->RdIO->Sw[0]) DisplayLED5CheckSystem();
	else DisplayLED9CheckSystem();

	return 1;
}




int CheckFrontLED3(){

	static uint8_t idx = 0;

	if(pj->ComRasp->rx_ready)  return -1;

	if(idx==5){
		HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, GPIO_PIN_SET);
	}
	else if(idx==10){
		HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, GPIO_PIN_SET);
	}
	else if(idx==15){
		HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, GPIO_PIN_RESET);
	}
	else if(idx==20){
		HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, GPIO_PIN_RESET);
	}
	else if(idx==25){
		HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, GPIO_PIN_RESET);
	}
	else if(idx==30){
		HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, GPIO_PIN_RESET);
	}
	else if(idx==35){
		HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, GPIO_PIN_SET);
	}
	else if(idx>45 && pj->ComPcu->rx_ready<1){
		HAL_GPIO_TogglePin(GPIOH, LED_ERR_Pin);
	}

	idx++;
	idx=idx>50?50:idx;

	return 1;
}

