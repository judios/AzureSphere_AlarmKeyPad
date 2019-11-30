/*
	Copyright 2019 Julian Diaz

	This file is part of AlarmKeyPad.

	AlarmKeyPad is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	AlarmKeyPad is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with AlarmKeyPad.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
    Some functionality in this file has been derived or inspired by the HardwareSerial
	functionality in the Arduino IDE. Therefore, the Arduino IDE license may be applicable. 
*/

#pragma once

#include <string.h>
#include <stdio.h>


#define HIGH 1
#define LOW 0
#define DEVICE_ADDRESS 20
#define GPIO_LINE 16
#define MT3620_ISU0_UART 4

#define RX_BUFFER_SIZE 256

#define DEFAULT_PULSE_TIMEOUT 1000000L

#define UART_STRUCTS_VERSION 1
#include <applibs/uart.h>
#include <applibs/gpio.h>

void alarmKeyPad_Init();

extern int uartFd;
char alarmKeyPad_Read();
int alarmKeyPad_Available();
size_t alarmKeyPad_Write(char val);
void alarmKeyPad_Flush();

// use time_out= DEFAULT_PULSE_TIMEOUT
int alarmKeyPad_pulseIn(GPIO_Value_Type state, long time_out);
unsigned long get_mills(void);


