/*
 * serial.c
 *
 *  Created on: 2020. 6. 15.
 *      Author: hhwang
 */

#include "main.h"
#include "project.h"
#include "util.h"
#include "sysdef.h"
#include <string.h>
#include <stdlib.h>

Communicator ComRasp;
Communicator ComPcu;
Communicator ComAux;
int ProcRxRingBufEx(Communicator* com, unsigned char opt);

void InitializeCommunicator(Communicator* com)
{
	com->Idx = 0;
	com->Len = 0;
	memset(com->RcvBuf, 0, 2);
	memset(com->Buf, 0, MAXUARTBUF);

	memset(com->Ring.buf, 0, RING_BUF_SIZE);
	com->Ring.in = 0;
	com->Ring.out = 0;

	com->rx_ready = 0;

	com->TxReady = 0;
	memset(com->TxBuf, 0, MAXUARTBUF);
	com->ProcReady = 0;
	memset(com->ProcBuf, 0, MAXUARTBUF);
}


uint8_t IsReceivedRingBuf(RING_BUF* ring)
{
  return (ring->in!=ring->out)?1:0;
}

char GetRingBuf(RING_BUF* ring)
{
	char ch = '\0';

	if(IsReceivedRingBuf(ring))
	{
		ch = ring->buf[ring->out++];
		ring->out = ring->out > RING_BUF_SIZE - 1 ? 0 : ring->out;
	}

	return ch;
}

int ProcRxRingBuf(Communicator* com)
{
#if 1
	return  ProcRxRingBufEx(com, 1);
#else
	if(!IsReceivedRingBuf(&com->Ring)){
		//Retry rx interrupt.
		com->rx_check_cnt++;
		com->rx_check_cnt = com->rx_check_cnt > 10000  ? 0 : com->rx_check_cnt;

		if(com->rx_check_cnt==10000) return -1;
		else return 0;
	}

	//char ch = 'a';
	char ch = GetRingBuf(&com->Ring);
	com->Buf[com->Idx++] = ch;

	//check buffer length.
	if(com->Idx>=240){
		com->Idx = 0;
		com->Buf[com->Idx]= '\0';
		com->Len = 0;
		return 0;
	}

	if(ch=='\n'){
		com->Buf[com->Idx]= '\0';
		com->Len = com->Idx ;
		com->Idx = 0;

		if(!CheckChecksum(com->Buf, com->Len)) return 0;

		if(com->TxReady == 0){
			strcpy((char*)com->TxBuf, (char*)com->Buf);
			com->TxReady = 1;
		}
	}

	com->rx_check_cnt = 0;
	return 0;
#endif
}

int ProcRxRingBufEx(Communicator* com, unsigned char opt)
{
#if 1
	if(!IsReceivedRingBuf(&com->Ring)){
		//Retry rx interrupt.
		com->rx_check_cnt++;
		com->rx_check_cnt = com->rx_check_cnt > 10000  ? 0 : com->rx_check_cnt;

		if(com->rx_check_cnt==10000) return -1;
		else return 0;
	}
#endif

	//char ch = 'a';
	char ch = GetRingBuf(&com->Ring);
	com->Buf[com->Idx++] = ch;

	//check buffer length.
	if(com->Idx>=240){
		com->Idx = 0;
		com->Buf[com->Idx]= '\0';
		com->Len = 0;
		return 0;
	}

	if(ch=='\n'){
		com->Buf[com->Idx]= '\0';
		com->Len = com->Idx ;
		com->Idx = 0;

		if(opt==1) if(!CheckChecksum(com->Buf, com->Len)) return 0;

		if(com->TxReady == 0){
			strcpy((char*)com->TxBuf, (char*)com->Buf);
			com->TxReady = 1;
		}
	}

	com->rx_check_cnt = 0;
	return 0;
}


int ProcRaspRxData(char* data)
{
	int i, counting=0,smallCounting=0,dataNumber=0,continueOK=1,ealier=1;
	char UnProccessedData[100][80];
	char* AcqData = data;
	static char Disp[2][41];

	//0.Initialize buffer
	for(i=0;i<100;i++)	UnProccessedData[i][0]=0x00;
	//1.Split string from ',' or '*'
	while(continueOK==1)
	{
		if(AcqData[counting]!=',' && AcqData[counting]!='*')
		{
			ealier=0;
			UnProccessedData[dataNumber][smallCounting++]=AcqData[counting];
		}
		else
		{

			if(ealier==0)
			{
				UnProccessedData[dataNumber][smallCounting]=0x00;
				smallCounting=0;
				dataNumber++;
				UnProccessedData[dataNumber][0]=0x00;
				ealier=1;
			}
			else
			{
				dataNumber++;
				smallCounting=0;
			}
		}
		if(AcqData[counting]=='\n' || AcqData[counting]==0x00)
		{
			continueOK=0,counting=0;
			break;
		}
		counting++;
	}

	if(strncmp(UnProccessedData[0],"$KB",3)==0)
	{
		SetComReady(pj->ComRasp);

		if(strncmp(UnProccessedData[1],"DISP1",5)==0)
		{
			strncpy(Disp[0],data+10, 40);
			pj->Vfd->DisplayLine1(Disp[0]);
		}
		else if(strncmp(UnProccessedData[1],"DISP2",5)==0)
		{
			strncpy(Disp[1],data+10, 40);
			pj->Vfd->DisplayLine2(Disp[1]);
		}
		else if(strncmp(UnProccessedData[1],"DISP",4)==0)
		{
			strncpy(Disp[0],data+9, 40);
			//strcpy(Disp[0], UnProccessedData[2]);
			pj->Vfd->DisplayLine1(Disp[0]);
			strncpy(Disp[1],data+50, 40);
			//strcpy(Disp[1], UnProccessedData[3]);
			pj->Vfd->DisplayLine2(Disp[1]);
		}
		else if(strncmp(UnProccessedData[1],"SYS",3)==0)
		{
			pj->WrIO->LedGPS = (UnProccessedData[3][0]!='0'?0:1);
			pj->WrIO->LedErr = (UnProccessedData[3][1]!='0'?0:1);
			pj->WrIO->LedTrk = (UnProccessedData[3][2]!='0'?0:1);
			pj->WrIO->PwrBUC = (UnProccessedData[3][3]!='0'?1:0);
			pj->WrIO->PwrANT = (UnProccessedData[3][4]!='0'?1:0);
			pj->WrIO->Gyro = (UnProccessedData[3][5]!='0'?1:0);
			pj->WrIO->AuxSel1 = (UnProccessedData[3][6]!='0'?1:0);
			pj->WrIO->AuxSel2 = (UnProccessedData[3][7]!='0'?1:0);
			pj->WrIO->TxMute = (UnProccessedData[3][8]!='0'?1:0);
			pj->WrIO->SwRefSigOfBdmux = (UnProccessedData[3][9]!='0'?0:1); /* Low : Ext, High : Int */

			HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, (GPIO_PinState)pj->WrIO->LedGPS);
			HAL_GPIO_WritePin(GPIOH, LED_TRK_Pin, (GPIO_PinState)pj->WrIO->LedTrk);
			HAL_GPIO_WritePin(GPIOH, LED_ERR_Pin, (GPIO_PinState)pj->WrIO->LedErr);
			HAL_GPIO_WritePin(GPIOC, PWR_BUC_Pin, (GPIO_PinState)pj->WrIO->PwrBUC);
			HAL_GPIO_WritePin(GPIOC, PWR_ANT_Pin, (GPIO_PinState)pj->WrIO->PwrANT);
			HAL_GPIO_WritePin(GPIOI, GYRO_SEL_Pin, (GPIO_PinState)pj->WrIO->Gyro);
			HAL_GPIO_WritePin(GPIOI, AUX_SEL1_Pin, (GPIO_PinState)pj->WrIO->AuxSel1);
			HAL_GPIO_WritePin(GPIOI, AUX_SEL2_Pin, (GPIO_PinState)pj->WrIO->AuxSel2);
			HAL_GPIO_WritePin(GPIOI, TX_MUTE_Pin, (GPIO_PinState)pj->WrIO->TxMute);
			HAL_GPIO_WritePin(GPIOB, SW_10MHz_Pin, (GPIO_PinState)pj->WrIO->SwRefSigOfBdmux);

			/*Test*/ //HAL_GPIO_WritePin(GPIOH, LED_GPS_Pin, (GPIO_PinState)pj->WrIO->TxMute);
		}
		else if(strncmp(UnProccessedData[1],"LED",3)==0)
		{
			HAL_GPIO_WritePin(GPIOF, VFD_RS_Pin, (GPIO_PinState)UnProccessedData[2][0]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_WR_Pin, (GPIO_PinState)UnProccessedData[2][1]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_RD_Pin, (GPIO_PinState)UnProccessedData[2][2]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_D0_Pin, (GPIO_PinState)UnProccessedData[2][3]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_D1_Pin, (GPIO_PinState)UnProccessedData[2][4]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_D2_Pin, (GPIO_PinState)UnProccessedData[2][5]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_D3_Pin, (GPIO_PinState)UnProccessedData[2][6]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_D4_Pin, (GPIO_PinState)UnProccessedData[2][7]!='0'?0:1);
			HAL_GPIO_WritePin(GPIOF, VFD_D5_Pin, (GPIO_PinState)UnProccessedData[2][8]!='0'?0:1);
		}
		else if(strncmp(UnProccessedData[1],"MAC0",4)==0)
		{
			uint8_t MAC[6];
			MAC[0] = (uint8_t)atoi(UnProccessedData[2]);
			MAC[1] = (uint8_t)atoi(UnProccessedData[3]);
			MAC[2] = (uint8_t)atoi(UnProccessedData[4]);
			MAC[3] = (uint8_t)atoi(UnProccessedData[5]);
			MAC[4] = (uint8_t)atoi(UnProccessedData[6]);
			MAC[5] = (uint8_t)atoi(UnProccessedData[7]);
			Write93C56Ex(EE93C56A, MAC, 1, 6);
		}
		else if(strncmp(UnProccessedData[1],"MAC1",4)==0)
		{
			uint8_t MAC[6];
			MAC[0] = (uint8_t)atoi(UnProccessedData[2]);
			MAC[1] = (uint8_t)atoi(UnProccessedData[3]);
			MAC[2] = (uint8_t)atoi(UnProccessedData[4]);
			MAC[3] = (uint8_t)atoi(UnProccessedData[5]);
			MAC[4] = (uint8_t)atoi(UnProccessedData[6]);
			MAC[5] = (uint8_t)atoi(UnProccessedData[7]);
			Write93C56Ex(EE93C56B, MAC, 1, 6);
		}
		else if(strncmp(UnProccessedData[1],"MAC",3)==0)
		{
			uint8_t MAC[6];
			MAC[0] = (uint8_t)atoi(UnProccessedData[2]);
			MAC[1] = (uint8_t)atoi(UnProccessedData[3]);
			MAC[2] = (uint8_t)atoi(UnProccessedData[4]);
			MAC[3] = (uint8_t)atoi(UnProccessedData[5]);
			MAC[4] = (uint8_t)atoi(UnProccessedData[6]);
			MAC[5] = (uint8_t)atoi(UnProccessedData[7]);
			Write93C56Ex(EE93C56B, MAC, 1, 6);
		}
		else if(strncmp(UnProccessedData[1],"REBOOT",3)==0)
		{
			HAL_NVIC_SystemReset();
		}

	}

	return 0;
}

int ProcPcuRxData(char* data)
{


	int i, counting=0,smallCounting=0,dataNumber=0,continueOK=1,ealier=1;
	char UnProccessedData[100][40];
	char* AcqData = data;

	//0.Initialize buffer
	for(i=0;i<100;i++)	UnProccessedData[i][0]=0x00;
	//1.Split string from ',' or '*'
	while(continueOK==1)
	{
		if(AcqData[counting]!=',' && AcqData[counting]!='*')
		{
			ealier=0;
			UnProccessedData[dataNumber][smallCounting++]=AcqData[counting];
		}
		else
		{

			if(ealier==0)
			{
				UnProccessedData[dataNumber][smallCounting]=0x00;
				smallCounting=0;
				dataNumber++;
				UnProccessedData[dataNumber][0]=0x00;
				ealier=1;
			}
			else
			{
				dataNumber++;
				smallCounting=0;
			}
		}
		if(AcqData[counting]=='\n' || AcqData[counting]==0x00)
		{
			continueOK=0,counting=0;
			break;
		}
		counting++;
	}

	if(strncmp(UnProccessedData[0],"$KB",3)==0)
	{
		if(strncmp(UnProccessedData[1],"DAT_S",5)==0)
		{
			if(strncmp(UnProccessedData[2],"GPS",3)==0 )
			{
				/* NOP */
			}
			else
			{
				for(i=0;i<dataNumber;i++)
				{
					long templ=atol(&UnProccessedData[i][2]);
					float tempf=atof(&UnProccessedData[i][2]);

					if(strncmp(UnProccessedData[i],"Aa",2)==0)		AntStatus.IsRun=(int)templ;
					else if(strncmp(UnProccessedData[i],"Ac",2)==0)	AntStatus.CarrierLock=(int)templ;
					else if(strncmp(UnProccessedData[i],"Ad",2)==0)	AntStatus.Tracking=(int)templ;
					else if(strncmp(UnProccessedData[i],"Ag",2)==0)	AntStatus.GpsStatus=templ;
					else if(strncmp(UnProccessedData[i],"Ah",2)==0) AntStatus.Mode = (int)templ;
					else if(strncmp(UnProccessedData[i],"As",2)==0)	AntStatus.TxOnOff=(int)templ;
					else if(strncmp(UnProccessedData[i],"Ed",2)==0)	AntStatus.Relative = (float)tempf;
					else if(strncmp(UnProccessedData[i],"Xa",2)==0)	AntStatus.Temperature = (float)tempf;
					else if(strncmp(UnProccessedData[i],"Xd",2)==0)	AntStatus.ModemRxLock=(int)templ;
					else if(strncmp(UnProccessedData[i],"Za",2)==0) AntStatus.ErrorCode=(int)tempf;
				}
			}
		}
	}

#if 0
#endif

	return 0;
}
