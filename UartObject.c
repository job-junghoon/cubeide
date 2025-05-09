#include "UartObject.h"
#include "main.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

UartObject m_uartPcu;
UartObject m_uartMnc;
UartObject m_uartReserve;
UartObject m_uartOcu;

UartObject m_uartNdmcIns;
UartObject m_uartNdmcGps;

/**
 * UART 수신 인터럽트 콜백함수
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART8) // PCU
	{
		m_uartPcu.buff[m_uartPcu.head] = m_uartPcu.rxByte;
		m_uartPcu.head = ((m_uartPcu.head + 1U) % RX_BUFFSIZE);

		HAL_UART_Receive_IT(huart, &m_uartPcu.rxByte, 1);

	}
	else if(huart->Instance == USART3) // MNC
	{
		m_uartMnc.buff[m_uartMnc.head] = m_uartMnc.rxByte;
		m_uartMnc.head = ((m_uartMnc.head + 1U) % RX_BUFFSIZE);

		HAL_UART_Receive_IT(huart, &m_uartMnc.rxByte, 1);
	}

	else if(huart->Instance == USART6) // Reserve
	{
		m_uartReserve.buff[m_uartReserve.head] = m_uartReserve.rxByte;
		m_uartReserve.head = ((m_uartReserve.head + 1U) % RX_BUFFSIZE);

		HAL_UART_Receive_IT(huart, &m_uartReserve.rxByte, 1);
	}

	else if(huart->Instance == USART1) // OCU
	{
		m_uartOcu.buff[m_uartOcu.head] = m_uartOcu.rxByte;
		m_uartOcu.head = ((m_uartOcu.head + 1U) % RX_BUFFSIZE);

		HAL_UART_Receive_IT(huart, &m_uartOcu.rxByte, 1);
	}

	else if(huart->Instance == UART4) // NDMC INS
	{
		m_uartNdmcIns.buff[m_uartNdmcIns.head] = m_uartNdmcIns.rxByte;
		m_uartNdmcIns.head = ((m_uartNdmcIns.head + 1U) % RX_BUFFSIZE);

		HAL_UART_Receive_IT(huart, &m_uartNdmcIns.rxByte, 1);

	}

	else if(huart->Instance == UART5) // NDMC GPS
	{
		m_uartNdmcGps.buff[m_uartNdmcGps.head] = m_uartNdmcGps.rxByte;
		m_uartNdmcGps.head = ((m_uartNdmcGps.head + 1U) % RX_BUFFSIZE);

		HAL_UART_Receive_IT(huart, &m_uartNdmcGps.rxByte, 1);
	}

	else
	{
		printf("DEfensive Code");
	}

}


/**
 * UartObject 객체들 초기화
 */
void InitUartObject(void)
{
	(void)memset(&m_uartPcu, 0, sizeof(m_uartPcu));
	(void)memset(&m_uartMnc, 0, sizeof(m_uartMnc));
	(void)memset(&m_uartReserve, 0, sizeof(m_uartReserve));
	(void)memset(&m_uartOcu, 0, sizeof(m_uartOcu));

	(void)memset(&m_uartNdmcIns, 0, sizeof(m_uartNdmcIns));
	(void)memset(&m_uartNdmcGps, 0, sizeof(m_uartNdmcGps));

}

/**
 * data의 CRC를 계산
 */
uint16_t CalculateCRC(const uint8_t data[], const uint16_t length)
{
	uint16_t crc = 0U;

	for(uint16_t i=0U; i<length; i=i+1U)
	{
		crc = ((crc >> 8U) | (crc << 8U));//(uint8_t)없애면 될수도 있음
		crc = (crc ^ data[i]);
		crc = (crc ^ ((crc & 0xffU) >> 4U));//(uint8_t)없애면 될수도 있음
		crc = (crc ^ (crc << 12U));
		crc = (crc ^ ((crc & 0x00ffU) << 5U));
	}

	return crc;
}

/**
 * data의 CRC를 검사.(데이터 유효성 검사)
 */
#if 1
bool CheckChecksum(uint8_t* data, const uint16_t length)
{
	uint16_t crc1 = 0;
	uint16_t crc2 = 0;

	char strHex[5];


	if ((length <= 517U) && (length > 6U))
	{
		// 체크섬 값 추출 (문자열의 끝에서 6번째부터 4글자 추출)
		if(data != NULL)
		{
			(void)strncpy(strHex, (char *)&data[length - 6U], 4);
		}
		strHex[4] = '\0';

		// 16진수 문자열을 정수로 변환 (02EF -> 0x02EF)
		crc1 = (uint16_t)strtol(strHex, NULL, 16);

		crc2 = CalculateCRC(data, length - 7U);
	}

	return (crc1 == crc2);
}//플래그 써서 수정
#endif


/**
 * 인터럽트에서 수신한 데이터를 처리버퍼에 저장하고 하나의 패킷이 완성되었는지 체크
 * 루프문에서 호출
 */
#if 1
void CheckRecvData(UartObject* uartObj)
{
	if(((uartObj->tail) != (uartObj->head)) && ((uartObj->tail) < RX_BUFFSIZE))
	{
		uint8_t data = uartObj->buff[uartObj->tail];
		uartObj->tail = ((uartObj->tail + 1U) % RX_BUFFSIZE);

		bool overbuff = false;

		if((uartObj->index) >= (RX_DATA_BUFFSIZE - 1U))
		{
			uartObj->index = 0U;
			(void)memset(uartObj->rxData, 0, sizeof(uartObj->rxData));//return 뺏음
			overbuff = true;

		}

        if(overbuff == false)
        {
        	if((uartObj->index == 0U) && (data == '\0'))
			{
				printf("DEfensive Code");
			}  // 널값 반환  else문 사용해서 분기시켜서 테스트
			else
			{
				uartObj->rxData[uartObj->index] = data;

				uartObj->index = (uartObj->index + 1U);

				if(data == '\n')
				{
					uartObj->rxData[uartObj->index] = '\0';
					uartObj->isComplete = true;
				}
			}
        }

   }


}
#endif

/**
 * 인터럽트에서 수신한 NDMC INS 데이터를 처리버퍼에 저장하고 하나의 패킷이 완성되었는지 체크.
 */
void CheckRecvInsData(void)
{
	if (((m_uartNdmcIns.head) != (m_uartNdmcIns.tail)) && ((m_uartNdmcIns.tail < RX_BUFFSIZE) && (m_uartNdmcIns.index < RX_DATA_BUFFSIZE)))
	{
		m_uartNdmcIns.isDisconnected = false;
		m_uartNdmcIns.disconnectCount = 0U;

		// 데이터 읽기
		uint8_t data = m_uartNdmcIns.buff[m_uartNdmcIns.tail];
		m_uartNdmcIns.tail = ((m_uartNdmcIns.tail + 1U) % RX_BUFFSIZE);

		if(m_uartNdmcIns.index == 0U)
		{
			if(data == 0x02U) // stx
			{
				m_uartNdmcIns.rxData[m_uartNdmcIns.index] = data;
				m_uartNdmcIns.index = (m_uartNdmcIns.index + 1U);
			}
			else // STX가 아니면 이 패킷은 무시
			{
				m_uartNdmcIns.errorCount = (m_uartNdmcIns.errorCount + 1U);
			}
		}
		else
		{
			m_uartNdmcIns.rxData[m_uartNdmcIns.index] = data;
			m_uartNdmcIns.index = (m_uartNdmcIns.index + 1U);
		}
	}
	else
	{
		m_uartNdmcIns.isDisconnected = true;
	}

	if(m_uartNdmcIns.index == 16U)
	{
		if(m_uartNdmcIns.rxData[15] == 0x03U) // etx 체크
		{
			// 14번지까지의 데이터를 합산하여 체크섬 계산
			//uint16_t sum = 0U
			uint8_t sum = 0U;
			for (uint8_t i = 0U; i < 14U; i=i+1U)
			{
				sum = sum + m_uartNdmcIns.rxData[i];
			}
			uint8_t checksum = (uint8_t)(-sum); // 체크섬 계산 //이게 맞음 체크섬 구하는 공식

			if (checksum == m_uartNdmcIns.rxData[14])
			{
				m_uartNdmcIns.isComplete = true;
				m_uartNdmcIns.errorCount = 0U;
			}
			else
			{
				m_uartNdmcIns.isComplete = false;
				m_uartNdmcIns.errorCount = (m_uartNdmcIns.errorCount + 1U);
			}

		}
		else
		{
			m_uartNdmcIns.isComplete = false;
			m_uartNdmcIns.errorCount = (m_uartNdmcIns.errorCount + 1U);
		}

		(m_uartNdmcIns.index = 0U); // 다음 데이터를 위해 인덱스 초기화
	}

}



/**
 * 인터럽트에서 수신한 NDMC GPS 데이터를 처리버퍼에 저장하고 하나의 패킷이 완성되었는지 체크.
 */
void CheckRecvGpsData(void)
{
	if (((m_uartNdmcGps.head != m_uartNdmcGps.tail)) && ((m_uartNdmcGps.tail < RX_BUFFSIZE) && (m_uartNdmcGps.index < RX_DATA_BUFFSIZE)))
	{
		m_uartNdmcGps.isDisconnected = false;
		m_uartNdmcGps.disconnectCount = 0U;

		// 데이터 읽기
		uint8_t data = m_uartNdmcGps.buff[m_uartNdmcGps.tail];
		m_uartNdmcGps.tail = ((m_uartNdmcGps.tail + 1U) % RX_BUFFSIZE);

		if(m_uartNdmcGps.index == 0U)
		{
			if(data == 0x4EU) // stx
			{
				m_uartNdmcGps.rxData[m_uartNdmcGps.index] = data;
				m_uartNdmcGps.index = (m_uartNdmcGps.index + 1U);
			}
			else
			{
				m_uartNdmcGps.errorCount = (m_uartNdmcGps.errorCount + 1U);
			}
		}
		else if(m_uartNdmcGps.index == 1U)
		{
			if(data == 0x44U) // 두 번째 데이터: 0x44 확인
			{
				m_uartNdmcGps.rxData[m_uartNdmcGps.index] = data;
				m_uartNdmcGps.index = (m_uartNdmcGps.index + 1U);
			}
			else
			{
				m_uartNdmcGps.errorCount = (m_uartNdmcGps.errorCount + 1U);
			}
		}
		else // 세번째 데이터부터 수신된 데이터 저장
		{
			m_uartNdmcGps.rxData[m_uartNdmcGps.index] = data;
			m_uartNdmcGps.index = (m_uartNdmcGps.index + 1U);
		}
	}
	else
	{
		m_uartNdmcGps.isDisconnected = true;
	}

	// 데이터 길이가 60에 도달하면 체크섬 검증
	if(m_uartNdmcGps.index == 60U)
	{
		//uint16_t sum = 0U
		uint8_t sum = 0U;
		for (uint8_t i = 0U; i < 59U; i = i +1U)
		{
			sum = sum + m_uartNdmcGps.rxData[i];
		}
		uint8_t checksum = (uint8_t)sum;

		if (checksum == m_uartNdmcGps.rxData[59])
		{
			m_uartNdmcGps.isComplete = true;
			m_uartNdmcGps.errorCount = 0U;
		}
		else
		{
			m_uartNdmcGps.isComplete = false;
			m_uartNdmcGps.errorCount = (m_uartNdmcGps.errorCount + 1U);
		}

		(m_uartNdmcGps.index = 0U); // 다음 데이터를 위해 인덱스 초기화
	}

}

/**
 * NDMC 장치의 연결 검사. 100ms 마다 호출.
 * disconnectCount 검사 및 초기화는 ProcessData.c의 ProcessNdmcXXX 에서 함
 */
void CheckNdmcConnect(void)
{
	if(m_uartNdmcIns.isDisconnected)
	{
		m_uartNdmcIns.disconnectCount = (m_uartNdmcIns.disconnectCount + 1U);
	}

	if(m_uartNdmcGps.isDisconnected)
	{
		m_uartNdmcGps.disconnectCount = (m_uartNdmcGps.disconnectCount + 1U);

	}
}








