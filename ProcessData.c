#include "ProcessData.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "main.h"
#include "Transfer.h"
#include "UartObject.h"

extern uint16_t m_adc_val[3]; // main.c에 선언됨


PcuData m_pcuData;
ExternalData m_externalData;


void ProcessPacket_MNC(const char* packet);
void ProcessPacket_PCU(const char* packet, const uint16_t packetSize); // const 붙여서 수정했음
void ProcessSplitData1(char* data);
void ProcessSplitData2(char* data);
void ProcessSplitData3(char* data);
void ProcessSplitData4(char* data);
void ProcessSplitData5(char* data);

void StartInit(void)
{
    InitUartObject();
    InitTransferData();
    InitProcessData();

}

void StartIt(void)
{
	HAL_UART_Receive_IT(&huart8, &m_uartPcu.rxByte, 1); 
    HAL_UART_Receive_IT(&huart3, &m_uartMnc.rxByte, 1); 
    HAL_UART_Receive_IT(&huart6, &m_uartReserve.rxByte, 1); 
    HAL_UART_Receive_IT(&huart1, &m_uartOcu.rxByte, 1); 

    HAL_UART_Receive_IT(&huart4, &m_uartNdmcIns.rxByte, 1); 
    HAL_UART_Receive_IT(&huart5, &m_uartNdmcGps.rxByte, 1); 

}
void CalledFunction(void)
{
	  CheckRecvData(&m_uartPcu);
	  CheckRecvData(&m_uartMnc);
	  CheckRecvData(&m_uartReserve);
	  CheckRecvData(&m_uartOcu);

	  CheckRecvInsData();
	  CheckRecvGpsData();

	  
	  ProcessPCU();
	  ProcessMNC();
	  ProcessReserve();
	  ProcessOCU();

	  ProcessNdmcINS();
	  ProcessNdmcGPS();
}//수정 했음


void InitProcessData(void)
{
	(void)memset(&m_pcuData, 0, sizeof(m_pcuData));
	(void)memset(&m_externalData, 0, sizeof(m_externalData));
}
/**
 * ACU의 상태(온도, 전압) 구하기.
 */
void GetAcuStatus(void)
{
	// 전압 = adc * (refVolt / max_adc) * ((R1 + R2) / R1) 1.1은 offset
	m_pcuData.acuVolt = ((((float_t)m_adc_val[0] * (3.3f / 4095.0f)) * 14.0f) + 1.1f);
	// 온도
	m_pcuData.acuTemper = ((((float_t)m_adc_val[2] * (3.3f / 4095.0f)) - 0.5f) * 100.0f);
}

/**
 * BUC TX 상태(PcuData의 bucMute)를 얻는다.
 */
uint8_t GetBucTxStatus(void)
{
	return m_pcuData.bucMute;
}

/**
 * PCU에서 수신한 데이터 처리
 */
void ProcessPCU(void)
{
	if(m_uartPcu.isComplete == true)
	{
		uint16_t length = m_uartPcu.index;
		(m_uartPcu.index = 0U);

		if(CheckChecksum(m_uartPcu.rxData, length) == true)
		{
			// MNC TX DMA 버퍼에 추가
			AddTransferData(TRANS_MNC, m_uartPcu.rxData, length);
			// Reserve TX DMA 버퍼에 추가
			AddTransferData(TRANS_RESERVE, m_uartPcu.rxData, length);

			// 데이터 처리
			ProcessPacket_PCU((char*)m_uartPcu.rxData, length);

		}

		m_uartPcu.isComplete = false;
	}
}

/**
 * OCU에 전송할 패킷 생성. 내부 함수(ProcessPacket_PCU()에서 호출)
 */
uint16_t MakeOcuPacket(char* outBuff)
{

	static uint8_t ackNumber = 0U;
	uint16_t length;
	if(outBuff != NULL)
	{
		ackNumber = ((ackNumber + 1U) % 10U);

		length = (uint16_t)sprintf((char*)outBuff, "$KB,DAT_S,STS,"
		"Aa%u,Ad%u,Ag%u,Ah%u,Ai%u,Ba%u,Bc%u,Bd%u,Be%u,Bf%u,"
		"Bg%.4f,Bh%.4f,Bk%.2f,Cb%.2f,Cc%.2f,Eb%.2f,Ec%.2f,Ed%.2f,Va%u,Vb%u,"
		"Vc%u,Vf%.2f,Vi%.2f,Vj%.1f,Vk%.1f,Vh%.1f,Vn%.1f,Wc%u,Wd%u,We%u,"
		"Wf%u,Wh%.1f,Wi%.1f,Wk%.1f,Xa%.2f,Xc%.1f,Xf%u,Xh%u,Xo%u,Xp%u,"
		"Xv%u,Yk%.1f,Yl%.1f,Za0x%08lX,%u",
		m_pcuData.isRun, m_pcuData.trackStatus, m_pcuData.gpsStatus, m_pcuData.antMode, m_pcuData.runMode,
		m_pcuData.satID, m_pcuData.nimAGC, m_pcuData.nimSNR, m_pcuData.rssd, m_pcuData.rssdCN,
		m_pcuData.latitude, m_pcuData.longitude, m_pcuData.gmtTime, m_pcuData.targetElevation, m_pcuData.targetAzimuth,
		m_pcuData.elevation, m_pcuData.bank, m_pcuData.azimuth, m_pcuData.bucOnOff, m_pcuData.loopback,
		m_pcuData.moistureStatus, m_pcuData.canisterTemper, m_pcuData.canisterHum, m_pcuData.antCurrent, m_pcuData.antVolt,
		m_pcuData.coolantTemper, m_pcuData.coolantFlowRate, !m_pcuData.bucPllLock, m_pcuData.bucMute, m_pcuData.bucOverPower,
		m_pcuData.bucOverTemper, m_pcuData.bucOutPower, m_pcuData.bucTemper, m_pcuData.bucAtten, m_pcuData.antTemper,
		m_pcuData.heading, m_pcuData.emcon, m_pcuData.bbuTxStatus, m_pcuData.ndmcIns, m_pcuData.ndmcGps,
		m_pcuData.ndmcMast, m_pcuData.acuTemper, m_pcuData.acuVolt,m_pcuData.errorCode, ackNumber
				);

		// CRC 계산
        if(outBuff != NULL)
        {
			uint16_t crc = CalculateCRC((uint8_t*)outBuff, length);

			
			char strCrc[8];
			(void)sprintf(strCrc, "*%04X\r\n", crc);
			(void)strcat(outBuff, strCrc);

			length = length + (uint16_t)strlen(strCrc);
        }
	}

	else
	{
		length = 0;
	}

	return length;
}


/**
 * PCU의 패킷을 처리. 내부 함수
 */


#if 1
void ProcessPacket_PCU(const char* packet, const uint16_t packetSize) // 패킷 사이즈 체크 해서 수정
{
	bool splitting = true; // 데이터 분리 플래그
	uint16_t counting = 0U;
	uint16_t  number = 0U;
	uint16_t  length = 0U;
	char splittedData[100][80];  // 패킷을 데이터 단위로 분리해서 저장 //2차원 배열 초기화
	(void)memset(splittedData,0,sizeof(splittedData));
    //packetsize써서 구문 하나 추가

	while(true)
	{
		// ',' 또는 '*' 가 아니면 데이터
		if((packet[counting] !=',') && (packet[counting] != '*'))
		{
			splitting = false;
			splittedData[number][length] = packet[counting];
			length = length + 1U;
		}
		else
		{
			if(splitting == false) // 한 개의 데이터 분리
			{
				splittedData[number][length] = 0x00;
				length = 0U;
				number = number + 1U;
				splittedData[number][0] = 0x00;
				splitting = true;
			}
			else // 데이터가 없는 경우 (예: A0,,,,,*)
			{
				number = number + 1U;
				length = 0U;
			}
		}

		if((packet[counting] == '\n') || (packet[counting] == 0x00))
		{
			counting = 0U;
			break;
		}


		counting = counting + 1U;

	}

	if((strncmp(splittedData[0], "$KE", 3) == 0) || (strncmp(splittedData[0], "$KR", 3) == 0))
	{
		AddTransferData(TRANS_OCU, (uint8_t*)packet, packetSize);
	}


	
	else if(strncmp(splittedData[0], "$KB", 3) == 0)
	{
		bool isEndPacket = false;

		if(strncmp(splittedData[1], "DAT_S", 5) == 0)
		{
			if(strncmp(splittedData[2], "STS", 3)==0 )
			{
				for(uint16_t i=3U; i<number; i++)
				{

					ProcessSplitData1(splittedData[i]);
					ProcessSplitData2(splittedData[i]);
					ProcessSplitData3(splittedData[i]);
					ProcessSplitData4(splittedData[i]);
					ProcessSplitData5(splittedData[i]);

					if(strncmp(splittedData[i], "Za", 2) == 0)
					{
						m_pcuData.errorCode = (uint32_t)strtoul(&splittedData[i][2], NULL, 16);
						isEndPacket = true;
					}
					else
					{
                      printf("DEfensive Code");
					}

				}
			}
		}
		else
		{
			printf("DEfensive Code");
		}

		// 3번째 Burst message 까지 다 받은 경우에 OCU에 전송할 데이터 작성
		if(isEndPacket == true)
		{
			char ocuData[512];
			(void)memset(ocuData,0,sizeof(ocuData));
			// OCU에 전송할 데이터 생성
			uint16_t len = MakeOcuPacket(ocuData);
			// OCU TX DMA 버퍼에 추가
			if(m_pcuData.Burst==1U)
			{
			   AddTransferData(TRANS_OCU, (uint8_t*)ocuData, len);//enum사용하지 말고 구조체 사용해서 선언 (uint8_t)2로 해보길 바람
			   m_pcuData.Burst=0U;
			}
		}


	}
	else
	{
		printf("DEfensive Code");
	}

}
#endif

void ProcessSplitData1(char* data)
{
	if(strncmp(data, "Aa", 2) == 0) { m_pcuData.isRun = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Ad", 2) == 0) { m_pcuData.trackStatus = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Ag", 2) == 0) { m_pcuData.gpsStatus = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Ah", 2) == 0) { m_pcuData.antMode = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Ai", 2) == 0) { m_pcuData.runMode = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Ba", 2) == 0) { m_pcuData.satID = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Bc", 2) == 0) { m_pcuData.nimAGC = (uint16_t)atoi(&data[2]); }
	else if(strncmp(data, "Bd", 2) == 0) { m_pcuData.nimSNR =(uint16_t) atoi(&data[2]); }
	else { 	printf("DEfensive Code"); }
}

void ProcessSplitData2(char* data)
{
	if(strncmp(data, "Be", 2) == 0) { m_pcuData.rssd = (uint16_t)atoi(&data[2]); }
	else if(strncmp(data, "Bf", 2) == 0) { m_pcuData.rssdCN = (uint16_t)atoi(&data[2]); }
	else if(strncmp(data, "Bg", 2) == 0) { m_pcuData.latitude = atof(&data[2]); }
	else if(strncmp(data, "Bh", 2) == 0) { m_pcuData.longitude = atof(&data[2]); }
	else if(strncmp(data, "Bk", 2) == 0) { m_pcuData.gmtTime = atof(&data[2]); }
	else if(strncmp(data, "Cb", 2) == 0) { m_pcuData.targetElevation = atof(&data[2]); }
	else if(strncmp(data, "Cc", 2) == 0) { m_pcuData.targetAzimuth = atof(&data[2]); }
	else if(strncmp(data, "Eb", 2) == 0) { m_pcuData.elevation = atof(&data[2]); }
	else if(strncmp(data, "Ec", 2) == 0) { m_pcuData.bank = atof(&data[2]); }
	else { printf("DEfensive Code"); }
}

void ProcessSplitData3(char* data)
{
	if(strncmp(data, "Ed", 2) == 0) { m_pcuData.azimuth = atof(&data[2]); }
	else if(strncmp(data, "Va", 2) == 0) { m_pcuData.bucOnOff = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Vb", 2) == 0) { m_pcuData.loopback = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Vc", 2) == 0) { m_pcuData.moistureStatus = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Vf", 2) == 0) { m_pcuData.canisterTemper = atof(&data[2]); }
	else if(strncmp(data, "Vi", 2) == 0) { m_pcuData.canisterHum = atof(&data[2]); }
	else if(strncmp(data, "Vj", 2) == 0) { m_pcuData.antCurrent = atof(&data[2]); }
	else if(strncmp(data, "Vk", 2) == 0) { m_pcuData.antVolt = atof(&data[2]); }
	else if(strncmp(data, "Vh", 2) == 0) { m_pcuData.coolantTemper = atof(&data[2]); }
	else if(strncmp(data, "Vn", 2) == 0) { m_pcuData.coolantFlowRate = atof(&data[2]); }
	else{printf("DEfensive Code");}
}

void ProcessSplitData4(char* data)
{
	if(strncmp(data, "Wc", 2) == 0) { m_pcuData.bucPllLock = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Wd", 2) == 0) { m_pcuData.bucMute = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "We", 2) == 0) { m_pcuData.bucOverPower = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Wf", 2) == 0) { m_pcuData.bucOverTemper = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Wh", 2) == 0) { m_pcuData.bucOutPower = atof(&data[2]); }
	else if(strncmp(data, "Wi", 2) == 0) { m_pcuData.bucTemper = atof(&data[2]); }
	else if(strncmp(data, "Wk", 2) == 0) { m_pcuData.bucAtten = atof(&data[2]); }
	else if(strncmp(data, "Xa", 2) == 0) { m_pcuData.antTemper = atof(&data[2]); }
	else{printf("DEfensive Code");}
}
void ProcessSplitData5(char* data)
{
	if(strncmp(data, "Xc", 2) == 0) { m_pcuData.heading = atof(&data[2]); }
	else if(strncmp(data, "Xf", 2) == 0) { m_pcuData.emcon = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Xh", 2) == 0) { m_pcuData.bbuTxStatus = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Xo", 2) == 0) { m_pcuData.ndmcIns = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Xp", 2) == 0) { m_pcuData.ndmcGps = (uint8_t)atoi(&data[2]); }
	else if(strncmp(data, "Xv", 2) == 0) { m_pcuData.ndmcMast = (uint8_t)atoi(&data[2]); }
	else{printf("DEfensive Code");}
}


/**
 * MNC에서 수신한 데이터 처리
 */
void ProcessMNC(void)
{
	if(m_uartMnc.isComplete == true)
	{
		uint16_t length = m_uartMnc.index;
		m_uartMnc.index = 0U;

		if(CheckChecksum(m_uartMnc.rxData, length) == true)
		{
			
			AddTransferData(TRANS_PCU, m_uartMnc.rxData, length);

			
			ProcessPacket_MNC((char*)m_uartMnc.rxData);
		}

		m_uartMnc.isComplete = false;
	}
}


/**
 * MNC의 패킷을 처리. 내부 함수
 */
void ProcessPacket_MNC(const char* packet)
{
	bool splitting = true; // 데이터 분리 플래그
	uint8_t counting = 0U;
	uint8_t number = 0U;
	uint8_t length = 0U;

	char splittedData[100][80];
	(void)memset(splittedData,0,sizeof(splittedData));

	while(true)
	{
		// ',' 또는 '*' 가 아니면 데이터
		if((packet[counting] !=',') && (packet[counting] != '*'))
		{
			splitting = false;
			splittedData[number][length] = packet[counting];
			length = (length + 1U);
		}
		else
		{
			if(splitting == false) // 한 개의 데이터 분리
			{
				splittedData[number][length] = 0x00;
				length = 0U;
				number = number + 1U;
				splittedData[number][0] = 0x00;
				splitting = true;
			}
			else // 데이터가 없는 경우 (예: A0,,,,,*)
			{
				number = number + 1U;
				length = 0U;
			}
		}

		if((packet[counting] == '\n') || (packet[counting] == 0x00))
		{
			counting = 0U;
			break;
		}

		counting = (counting + 1U);
	}

	// "<KB" 메시지 처리
	if(strncmp(splittedData[0], "<KB", 3) == 0)
	{
		if(strncmp(splittedData[1], "DAT_S", 5) == 0)
		{
			if(strncmp(splittedData[2], "MNH", 3) == 0)
			{
				for(uint16_t i=3U; i<number; i = i +1U)
				{
					if(strncmp(splittedData[i], "Aa", 2) == 0) { m_externalData.headingStatus = (uint8_t)atoi(&splittedData[i][2]); }
					else if(strncmp(splittedData[i], "Ab", 2) == 0) { m_externalData.heading = (float_t)atof(&splittedData[i][2]); }
					else
					{
						printf("DEfensive Code\n");
					}
				}
			}
			else if(strncmp(splittedData[2], "BST", 3) == 0)
			{
				m_pcuData.Burst = 1U;
			}
			else
			{
				printf("DEfensive Code\n");
			}
		}

	}


}




/**
 * Reserve 포트에서 수신한 데이터 처리
 */
void ProcessReserve(void)
{
	if(m_uartReserve.isComplete == true)
	{
		uint16_t length = m_uartReserve.index;
		m_uartReserve.index = 0U;

		if(CheckChecksum(m_uartReserve.rxData, length) == true)
		{
			// PCU TX DMA 버퍼에 추가
			AddTransferData(TRANS_PCU, m_uartReserve.rxData, length);

			// 데이터 처리 (MNC와 동일)
			ProcessPacket_MNC((char*)m_uartReserve.rxData);
		}

		m_uartReserve.isComplete = false;
	}
}


/**
 * OCU에서 수신한 데이터 처리
 */
void ProcessOCU(void)
{
	if(m_uartOcu.isComplete == true)
	{
		uint16_t length = m_uartOcu.index;
		m_uartOcu.index = 0U;

		if(CheckChecksum(m_uartOcu.rxData, length) == true)
		{
			// PCU TX DMA 버퍼에 추가
			AddTransferData(TRANS_PCU, m_uartOcu.rxData, length);

			// 데이터 처리 (MNC와 동일)
			ProcessPacket_MNC((char*)m_uartOcu.rxData);
		}

		m_uartOcu.isComplete = false;
	}

}


/**
 * ACU에서 PCU로 전송할 데이터 생성 -> Trnas Queue 에 넣기
 * 데이터 생성전에 긴급송신차단 상태와 BBU 송신준비신호 상태 체크
 */
void MakeAcuDataToPcu(void)
{
    char sendBuff[256];
    (void)memset(sendBuff,0,sizeof(sendBuff));
	float_t heading =0;
	uint8_t headingStatus = 0U;

	// 시간상 문제 생기면 밖으로 빼서 따로 체크할 것
	m_externalData.emcon = (HAL_GPIO_ReadPin(GPIOI, GPIO_PIN_2) > 0U ? 0U : 1U);
	m_externalData.bbuStatus = (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_4) > 0U ? 0U : 1U);

	// MNC의 headingStatus가 정상이면 우선적으로 MNC 해딩값을 사용
	if(m_externalData.headingStatus == 1U)
	{
		heading = m_externalData.heading;//ok
		headingStatus = 1U;
	}
	else if(m_externalData.headingStatus == 0U)
	{
		if(m_externalData.ndmcInsStatus == 1U)
		{
			heading = m_externalData.ndmcHeading;
			headingStatus = 1U;
		}
		else
		{
			heading = 0.0f;
			headingStatus = 0U;
		}
	}
	else
	{
		printf("DEfensive Code");
	}

	// number 값 증가 및 0~9 순환
	m_externalData.number = (m_externalData.number + 1U) % 10U;

	// 메시지 작성 - 쉼표와 숫자 추가
	uint16_t length = (uint16_t)sprintf((char*)sendBuff, "$KB,DAT_S,STS,"
			"Aa%u,Ab%.2f,Ac%.2f,Ad%.2f,Ba%u,Bb%u,Bc%.2f,Bd%.2f,Hc%u,Hd%u,%u",
			headingStatus, round(heading), round(m_externalData.ndmcPitch), round(m_externalData.ndmcRoll),
			m_externalData.ndmcGpsStatus, m_externalData.satComMastStatus, round(m_externalData.ndmcLatitude), round(m_externalData.ndmcLongitude),
			m_externalData.emcon, m_externalData.bbuStatus, m_externalData.number);//(uint16_t)캐스팅 했음

	// CRC 계산 및 추가
	uint16_t crc = CalculateCRC((uint8_t*)sendBuff, length);
	// CRC를 문자열로 만들고 붙이기
	char strCrc[8];
	(void)sprintf(strCrc, "*%04X\r\n", crc);
	(void)strcat(sendBuff, strCrc);

	length = (length + (uint16_t)strlen(strCrc));//(uint16_t)붙였음

	// PCU TX DMA 버퍼에 추가
	AddTransferData(TRANS_PCU, (uint8_t*)sendBuff, length);

}



void ProcessINS(void)
{
	// 0 ~ 360 계산용 (INS heading). 0번 인덱스가 최상위 비트
	static const float_t ARR_DEGREE360[16] = {180.0F ,90.0F, 45.0F, 22.5F, 11.25F, 5.625F, 2.8125F, 1.40625F, 0.703125F, 0.351563F, 0.175781F, 0.087891F, 0.043945F, 0.021973F, 0.010986F, 0.005493F};
	// -90 ~ 90 계산용. 0번 인덱스가 최상위 비트
	static const float_t ARR_DEGREE90[16] = {-90.0F, 45.0F, 22.5F, 11.25F, 5.625F, 2.8125F, 1.40625F, 0.703125F, 0.351563F, 0.175781F, 0.087891F, 0.043945F, 0.021973F, 0.010986F, 0.005493F, 0.002746F};

	if(m_uartNdmcIns.isComplete == true)
	{
		// uint16_t(2 byte)로 변환
		uint16_t heading = (uint16_t)((m_uartNdmcIns.rxData[2] << 8U) | (m_uartNdmcIns.rxData[3]));
		uint16_t roll = (uint16_t)((m_uartNdmcIns.rxData[4] << 8U) | (m_uartNdmcIns.rxData[5]));
		uint16_t pitch = (uint16_t)((m_uartNdmcIns.rxData[6] << 8U) | (m_uartNdmcIns.rxData[7]));

		m_externalData.ndmcInsStatus = 1U;
		m_externalData.ndmcHeading = 0.0f;
		m_externalData.ndmcRoll = 0.0f;
		m_externalData.ndmcPitch = 0.0f;

		// 데이터 처리
		for (uint8_t i= 0U; i < 16U; i=i+1U)
		{
			if (((heading & (1U << (15U - i))) >> (15U - i)) == 1U)
			{
				m_externalData.ndmcHeading = ((float_t)m_externalData.ndmcHeading + ARR_DEGREE360[i]);
			}
			if (((roll & (1U << (15U - i))) >> (15U - i)) == 1U)
			{
				m_externalData.ndmcRoll = ((float_t)m_externalData.ndmcRoll + ARR_DEGREE90[i]);
			}
			if (((pitch & (1U << (15U - i))) >> (15U - i)) == 1U)
			{
				m_externalData.ndmcPitch = (m_externalData.ndmcPitch + ARR_DEGREE90[i]);
			}
		}

		m_uartNdmcIns.isComplete = false; // 데이터 처리가 끝났으면 false로 변경
	}
	else
	{
		// 패킷에러가 계속 발생했을 경우 ins 데이터들을 초기화
		if((m_uartNdmcIns.errorCount >= 5000U) || (m_uartNdmcIns.disconnectCount >= 100U))
		{
			m_uartNdmcIns.errorCount = 0U;
			m_uartNdmcIns.disconnectCount = 0U;

			m_externalData.ndmcInsStatus = 0U;
			m_externalData.ndmcHeading = 0.0f;
			m_externalData.ndmcRoll = 0.0f;
			m_externalData.ndmcPitch = 0.0f;
		}
	}

}


void ProcessGPS(void)
{

	static const float_t ARR_DEGREEGPS180Upper[16] = {-180.0F, 90.0F, 45.0F, 22.5F, 11.25F, 5.625F, 2.8125F, 1.40625F, 0.703125F, 0.351563F, 0.175782F, 0.087891F, 0.043946F, 0.021973F, 0.010987F, 0.005494F};

	static const float_t ARR_DEGREEGPS180Lower[16] = {0.002747F, 0.001374F, 0.000687F, 0.000344F, 0.000172F, 0.000086F, 0.000043F, 0.000022F, 0.000011F, 0.000006F, 0.000003F, 0.000002F, 0.000001F, 0.000001F, 0.000001F, 0.000001F};

	if(m_uartNdmcGps.isComplete == true)
	{
		// uint16_t(2 byte)로 변환
		uint16_t latitudeUpper = (uint16_t)((m_uartNdmcGps.rxData[4] << 8U) | (m_uartNdmcGps.rxData[5]));
		uint16_t latitudeLower = (uint16_t)((m_uartNdmcGps.rxData[6] << 8U) | (m_uartNdmcGps.rxData[7]));

		uint16_t longitudeUpper = (uint16_t)((m_uartNdmcGps.rxData[8] << 8U) | (m_uartNdmcGps.rxData[9]));
		uint16_t longitudeLower = (uint16_t)((m_uartNdmcGps.rxData[10] << 8U) | (m_uartNdmcGps.rxData[11]));

		
		uint8_t satcomMast = (uint8_t)(m_uartNdmcGps.rxData[38] & 0xC0U);
		// 2(1 0)=retracted, 1(0 1)=hoisted, 0(0 0)=intermediate position, 3(1 1)=not used
		m_externalData.satComMastStatus = (uint8_t)(satcomMast >> 6U);

		m_externalData.ndmcGpsStatus = 1U;
		m_externalData.ndmcLatitude = 0.0f;
		m_externalData.ndmcLongitude = 0.0f;

		// 데이터 처리
		for (uint8_t i = 0U; i < 16U; i=i+1U)
		{
			if (((latitudeUpper & (1U << (15U - i))) >> (15U - i)) == 1U)
			{
				m_externalData.ndmcLatitude = (m_externalData.ndmcLatitude + ARR_DEGREEGPS180Upper[i]);
			}
			if (((latitudeLower & (1U << (15U - i))) >> (15U - i)) == 1U)
			{
				m_externalData.ndmcLatitude = (m_externalData.ndmcLatitude + ARR_DEGREEGPS180Lower[i]);
			}
			if (((longitudeUpper & (1U << (15U - i))) >> (15U - i)) == 1U)
			{
				m_externalData.ndmcLongitude = (m_externalData.ndmcLongitude + ARR_DEGREEGPS180Upper[i]);
			}
			if (((longitudeLower & (1U << (15U - i))) >> (15U - i)) == 1U)
			{
				m_externalData.ndmcLongitude = (m_externalData.ndmcLongitude + ARR_DEGREEGPS180Lower[i]);
			}

		}

		m_uartNdmcGps.isComplete = false; // 데이터 처리가 끝났으면 false로 변경

	}
	else
	{
		// 패킷에러가 계속 발생했을 경우 gps 데이터들을 초기화 또는 10초 이상 수신없을 경우
		if((m_uartNdmcGps.errorCount >= 5000U) || (m_uartNdmcGps.disconnectCount >= 100U))
		{
			m_uartNdmcGps.errorCount = 0U;
			m_uartNdmcGps.disconnectCount = 0U;

			m_externalData.ndmcGpsStatus = 0U;
			m_externalData.ndmcLatitude = 0.0f;
			m_externalData.ndmcLongitude = 0.0f;
			m_externalData.satComMastStatus = 1U;

		}
	}

}

/**
 * 각 상태에 따른 전면 LED 제어.
 */
void ControlLed(void)
{
	/*
	 *
	 * 0 = ON, 1 = OFF
	 */
	// ACU 상태 표시. 전압 추가해야 함
	if(((m_pcuData.acuTemper > 65.0f) || (m_pcuData.acuTemper < -20.0f)) || ((m_pcuData.acuVolt < 22.0f) || (m_pcuData.acuVolt > 26.3f)))
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 1);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, 0);
	}

	// 안테나(pcu) 에러
	if(m_pcuData.errorCode != 0U)
	{
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 0);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 1);
	}
	else
	{
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_0, 1);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, 0);
	}


	switch (m_pcuData.antMode) //Ad, 위성 추적 상태
	{
	case 0U: // 스탠바이 모드 끄기
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, 1);
		break;
	case 1U: // 초기화 중 토글 핀
		HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_3);
		break;
	case 2U: // 탐색 중 토글핀
		HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_3);
		break;
	case 3U: // 추적 중 켜기
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, 0);
		break;
	case 4U: // 탐색 실패 끄기
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_3, 1);
		break;
	default:
		printf("DEfensive Code");
		break;
	}


	switch (m_pcuData.runMode) //Ai, 안테나 구동(동작) 모드
	{
	case 0U: // 대기, off
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_1, 1);
		break;
	case 1U: // 수동 구동모드, blink
	case 2U: // 수동 추적모드, blink
		HAL_GPIO_TogglePin(GPIOJ, GPIO_PIN_1);
		break;
	case 3U: // 자동 추적모드. on
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_1, 0);
		break;
	default:
		printf("DEfensive Code");
		break;
	}


	switch (m_pcuData.bucMute)//Wd
	{
	case 0U: //BUC TX ON
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_2, 0);
		break;
	case 1U: //BUC TX OFF
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_2, 1);
		break;
	default:
		printf("DEfensive Code");
		break;
	}


	switch (m_pcuData.ndmcIns)//Xo
	{
	case 0U:
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, 1);
		break;
	case 1U:
		HAL_GPIO_WritePin(GPIOF, GPIO_PIN_11, 0);
		break;
	default:
		printf("DEfensive Code");
		break;
	}

	// 미정
	switch (m_pcuData.ndmcGps) //Xp
	{
	case 0U:
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, 1);
		break;
	case 1U:
		HAL_GPIO_WritePin(GPIOJ, GPIO_PIN_4, 0);
		break;
	default:
		printf("DEfensive Code");
		break;
	}


	switch (m_pcuData.bbuTxStatus) //Xh
	{
		case 0U: // 송신 off 또는 연결안됨
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, 1);
			break;
		case 1U: // 송신 준비 상태. on
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_12, 0);
			break;
		default:
			printf("DEfensive Code");
			break;
	}


	switch (m_pcuData.emcon) //Xf
	{
		case 0U: // 차단
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 1);
			break;
		case 1U: // 미차단
			HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, 0);
			break;
		default:
			printf("DEfensive Code");
			break;
	}
}




