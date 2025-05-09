/*
 * uart_event.c
 *
 *  Created on: 2020. 6. 14.
 *      Author: hhwang
 */
#include "main.h"
#include "project.h"
#include <stdlib.h>
#include <string.h>


#if 1
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		SetComReady(pj->ComRasp);

		if((ComRasp.Ring.buf + ComRasp.Ring.in) != NULL)
		{
			ComRasp.Ring.in++;
			ComRasp.Ring.in=ComRasp.Ring.in > RING_BUF_SIZE - 1 ? 0 : ComRasp.Ring.in;
		}


		HAL_UART_Receive_IT(huart, ComRasp.Ring.buf + ComRasp.Ring.in, 1);
	}
	else if(huart->Instance == USART6)
	{
		if(pj->ComRasp->rx_ready) return;

		SetComReady(pj->ComPcu);

		if((ComPcu.Ring.buf + ComPcu.Ring.in) != NULL)
		{
			ComPcu.Ring.in++;
			ComPcu.Ring.in=ComPcu.Ring.in > RING_BUF_SIZE - 1 ? 0 : ComPcu.Ring.in;

		}

		HAL_UART_Receive_IT(huart, ComPcu.Ring.buf + ComPcu.Ring.in, 1);
	}
	else if(huart->Instance == UART4)
	{

		SetComReady(pj->ComAux);

		if((ComAux.Ring.buf + ComAux.Ring.in) != NULL)
		{
			ComAux.Ring.in++;
			ComAux.Ring.in=ComAux.Ring.in > RING_BUF_SIZE - 1 ? 0 : ComAux.Ring.in;

		}

		HAL_UART_Receive_IT(huart, ComAux.Ring.buf + ComAux.Ring.in, 1);
	}

}
#endif


