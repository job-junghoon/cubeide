/*
 * project.c
 *
 *  Created on: Jun 16, 2020
 *      Author: hhwang
 */

#include "project.h"
#include "eeprom.h"

RDIO RdIO;
RDIO RdIoCnt;
WRIO WrIO;
AntennaStatus AntStatus;

Project _pj;
Project* pj = &_pj;

void InitializeProject(){

	Init_Eeprom();
	InitializeVFD();
	InitializeCommunicator(&ComRasp);
	InitializeCommunicator(&ComPcu);
	InitializeCommunicator(&ComAux);

	pj->ComRasp = &ComRasp;
	pj->ComPcu = &ComPcu;
	pj->ComAux = &ComAux;
	pj->Vfd = &Vfd;
	pj->RdIO = &RdIO;
	pj->RdIoCnt = &RdIoCnt;
	pj->WrIO = &WrIO;
	pj->AntStatus = &AntStatus;

	pj->Nmea2000= &Nmea2000;
	pj->Nmea2000->CheckConnection = CheckNmea2000Connection;
	pj->Nmea2000->ResetConnection = ResetNmea2000Connection;

	pj->Tick100ms = 0;
	pj->Tick1s = 0;
	pj->Tick10ms = 0;
	pj->SetTick100ms = SetTick100ms;
	pj->ResetTick100ms = ResetTick100ms;
	pj->SetTick1s = SetTick1s;
	pj->ResetTick1s = ResetTick1s;
	pj->SetTick10ms = SetTick10ms;
	pj->ResetTick10ms = ResetTick10ms;
	pj->DoBlink = DoBlink;

}

void SetTick1s() { pj->Tick1s = 1; }
void ResetTick1s() { pj->Tick1s = 0; }
void SetTick100ms() { pj->Tick100ms = 1; }
void ResetTick100ms() { pj->Tick100ms = 0; }
void SetTick10ms() { pj->Tick10ms = 1; }
void ResetTick10ms() { pj->Tick10ms = 0; }

void DoBlink() {

	static int blink_cnt=0;
	if(++blink_cnt==5){
		blink_cnt = 0;
		pj->Blink=!pj->Blink;
	}
}



