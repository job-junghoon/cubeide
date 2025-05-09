/*
 * concept.c
 *
 *  Created on: 2021. 2. 23.
 *      Author: hhwang
 *
 *
 *
 *		STM32F756 MCU	|
 *  					|* INTERNAL *********************************************************************
 *   					|<--------> UART1 	<--------> Raspberry PI
 *   					|<--------> UART4 	<--------> AUX 0 Port
 *   					|<--------> UART6 	<--------> PCU Input
 *   					|<--------> SPI 	<--------> 93C56 (EEPROM)
 *  					|                           	- MAC address
 *  					|<--------> SPI		<--------> VFD
 *  					|<--------> GPIO	<--------> 1) KEY
 *  					|                              2) EMCON
 *  					|                              3) Reference Frequency
 *  					|                              4) Detector
 *  					|                              5) Switch
 *  					|
 *  					|* EXTERNAL *********************************************************************
 *   					|<--------> CAN 	<--------> NMEA2000
 *  					|
 *  					|
 *  					|
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */




