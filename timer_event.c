/*
 * timer_event.c
 *
 *  Created on: 2020. 6. 14.
 *      Author: hhwang
 */

#include "main.h"
#include "project.h"
#include <stdio.h>
#include <string.h>




/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){

	if(htim->Instance == TIM6) {
		pj->SetTick10ms();
	}
	else if(htim->Instance == TIM7) {
		pj->SetTick100ms();
	}
	else if(htim->Instance == TIM10) {
		pj->SetTick1s();
	}
}

