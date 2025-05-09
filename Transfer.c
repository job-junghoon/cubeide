#include "Transfer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "main.h"

static TxDmaQueue m_dmaQuePcu; //static붙였음
static TxDmaQueue m_dmaQueMnc;
static TxDmaQueue m_dmaQueOcu;
static TxDmaQueue m_dmaQueReserve;

static bool m_txbusy_ndmc_gps;


/**
 * UART DMA 전송 인터럽트 콜백함수
 */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == UART8) // PCU
	{
		m_dmaQuePcu.txBusy = false;
	}
    else if(huart->Instance == USART3) // MNC
    {
    	m_dmaQueMnc.txBusy = false;
    }
    else if(huart->Instance == USART1) // OCU
   	{
       	m_dmaQueOcu.txBusy = false;
   	}
    else if(huart->Instance == UART5) // NDMC GPS
   	{
    	m_txbusy_ndmc_gps = false;
   	}
    else if(huart->Instance == USART6) // 예약
    {
    	m_dmaQueReserve.txBusy = false;
    }
    else
    {
    	printf("DEfensive Code");
    }

}


/**
 * TxDmaQueue 초기화
 */
void InitTxDmaQueue(TxDmaQueue* dmaQue)
{
	dmaQue->txBusy = false;
	dmaQue->front = 0U;
	dmaQue->rear = 0U;

	// TxDMA 구조체 초기화
	for(uint8_t i = 0U; i < TX_DMA_QUEUESIZE; i=i+1U)
	{
		(void)memset(dmaQue->buff[i], 0, TX_DMA_BUFFSIZE);
		dmaQue->buffSize[i] = 0U;
	}
}

/**
 * TxDmaQueue 들 초기화
 */
void InitTransferData(void)
{
	InitTxDmaQueue(&m_dmaQuePcu);
	InitTxDmaQueue(&m_dmaQueMnc);
	InitTxDmaQueue(&m_dmaQueOcu);
	InitTxDmaQueue(&m_dmaQueReserve);

	m_txbusy_ndmc_gps = false;
}

/**
 * DMA로 전송할 데이터들 추가한다.
 * target: 전송할 UART.
 */
void AddTransferData(const uint8_t target, uint8_t* data, const uint16_t dataSize)
{
	TxDmaQueue* txDmaQue = NULL;

	switch(target)
	{
	case TRANS_PCU:
		txDmaQue = &m_dmaQuePcu;
		break;
	case TRANS_MNC:
		txDmaQue = &m_dmaQueMnc;
		break;
	case TRANS_OCU:
		txDmaQue = &m_dmaQueOcu;
		break;
	case TRANS_RESERVE:
		txDmaQue = &m_dmaQueReserve;
		break;
	default:
		printf("DEfensive Code");
		break;
	}

	if(txDmaQue != NULL)
	{
		uint8_t nextRear = ((txDmaQue->rear + 1U) % TX_DMA_QUEUESIZE);
		// 버퍼가 꽉 차 있어도 넣고 빼기 위한 처리
		if(nextRear == txDmaQue->front) // full
		{
			txDmaQue->front = ((txDmaQue->front + 1U) % TX_DMA_QUEUESIZE);
		}

        if((dataSize != 0U) && (data != NULL))//수정 했음
        {
        	(void)memcpy(txDmaQue->buff[txDmaQue->rear], data, dataSize);
        }

		txDmaQue->buffSize[txDmaQue->rear] = dataSize;//dataSize if문 사용해서 확인해야됨

		txDmaQue->rear = ((txDmaQue->rear + 1U) % TX_DMA_QUEUESIZE);
	}
}

/**
 * DMA로 데이터를 전송.
 */
void TransferUart(const uint8_t target)
{
	TxDmaQueue* txDmaQue = NULL;
	UART_HandleTypeDef* huartPtr = NULL;

	switch(target)
	{
	case TRANS_PCU:
		txDmaQue = &m_dmaQuePcu;
		huartPtr = &huart8;
		break;
	case TRANS_MNC:
		txDmaQue = &m_dmaQueMnc;
		huartPtr = &huart3;
		break;
	case TRANS_OCU:
		txDmaQue = &m_dmaQueOcu;
		huartPtr = &huart1;
		break;
	case TRANS_RESERVE:
		txDmaQue = &m_dmaQueReserve;
		huartPtr = &huart6;
		break;
	default:
		printf("DEfensive Code");
		break;
	}

    if(((txDmaQue != NULL) && (txDmaQue->front != txDmaQue->rear)) && (huartPtr != NULL))//huartPtr도 널 아니게 수정
	{
		if(txDmaQue->txBusy == false) // dma 가 사용중이 아니면 전송
		{
			txDmaQue->txBusy = true; // dma 사용 플래그 설정
			HAL_UART_Transmit_DMA(huartPtr, txDmaQue->buff[txDmaQue->front], txDmaQue->buffSize[txDmaQue->front]);

			txDmaQue->front = ((txDmaQue->front + 1U) % TX_DMA_QUEUESIZE);
		}
	}

}


/**
 * NDMC GPS 포트에 DMA로 데이터 전송. 100ms 간격으로 호출
 */
void TransferToNdmcGPS(const uint8_t bucTxStatus)
{
	static uint8_t gps_buff[9];
	(void)memset(gps_buff,0,sizeof(gps_buff));

	if(m_txbusy_ndmc_gps == false)
	{
		gps_buff[0] = 0x53U; // stx
		gps_buff[1] = 0x43U; // stx

		// BUC Tx Status(Mute)에 따른 값 설정
		if(bucTxStatus == 0U)
		{
			gps_buff[2] = 0x80U;
		}
		else
		{
			gps_buff[2] = 0x00U;
		}

		// 3 ~ 7은 사용안함
		for(uint8_t i = 3U; i < 8U; i = i +1U)
		{
			gps_buff[i] = 0x00U;
		}

		// 체크섬 계산
		uint8_t checksum = 0U;
		for(uint8_t i = 0U; i < 8U; i = i +1U)  // 마지막 바이트는 체크섬 저장을 위해 제외
		{
			checksum += gps_buff[i];
		}
		// 체크섬을 마지막 바이트에 저장
		gps_buff[8] = checksum;

		// DMA 전송
		m_txbusy_ndmc_gps = true;
		HAL_UART_Transmit_DMA(&huart5, gps_buff, 9);

	}

}







