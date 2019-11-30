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
#include "AlarmKeyPad.h"

#include <errno.h>
#include <applibs/log.h>
#include <applibs/adc.h>

#include <unistd.h>
#include <time.h>

int gpioLineFd = -1;
int gpioTxFd = -1;
int uartFd = -1;
int rxb_head = 0;
int rxb_tail = 0;
char readBuffer[RX_BUFFER_SIZE];

#define bit4800Delay 192000


void alarmKeyPad_Init()
{
	// Create a UART_Config object, open the UART and set up UART event handler
	UART_Config uartConfig;
	UART_InitConfig(&uartConfig);
	uartConfig.baudRate = 4800;
	uartConfig.dataBits = UART_DataBits_Eight;
	uartConfig.stopBits = UART_StopBits_One;
	uartConfig.parity = UART_Parity_Even;
	uartConfig.flowControl = UART_FlowControl_None;
	uartFd = UART_Open(MT3620_ISU0_UART, &uartConfig);

	/*
	    Since a GPIO used for uart cannot be accessed directly, 
		another input is required in order to be able to query 
		the state of the RX line. This is to be used in the 
		pulseIn function.
	*/
	gpioLineFd = GPIO_OpenAsInput(GPIO_LINE);

	/* 
	    Avoid using uart for the wire out. THe line high while in idle will
		trigger a safe thread in the Alarm board.
	*/
	gpioTxFd = GPIO_OpenAsOutput(GPIO_TX, GPIO_OutputMode_PushPull, GPIO_Value_Low);

}

int alarmKeyPad_GetUartFd()
{
	if (gpioLineFd < 0) return -1;
	return uartFd;
}

char alarmKeyPad_Read()
{   
	if (rxb_head == rxb_tail) return -1;
	char c = readBuffer[rxb_tail];
	rxb_tail = (rxb_tail + 1) % RX_BUFFER_SIZE;
	return c;
}

int alarmKeyPad_Available()
{
	if (rxb_head == rxb_tail)
	{
		const size_t receiveBufferSize = 256;
		uint8_t receiveBuffer[receiveBufferSize + 1]; // allow extra byte for string termination
		ssize_t bytesRead;

		// Read incoming UART data. It is expected behavior that messages may be received in multiple
		// partial chunks.
		bytesRead = read(uartFd, receiveBuffer, receiveBufferSize);
		if (bytesRead < 0) {
			Log_Debug("ERROR: Could not read UART: %s (%d).\n", strerror(errno), errno);
			return -1;
		}
		// if(bytesRead > 0)
		//	Log_Debug("Bytes Read %d\n", bytesRead);
		for (short i = 0; i < bytesRead; i++)
		{
			readBuffer[rxb_head] = receiveBuffer[i];
			rxb_head = (rxb_head + 1) % RX_BUFFER_SIZE;
		}
	}
	return ((unsigned int)(RX_BUFFER_SIZE + rxb_head - rxb_tail)) % RX_BUFFER_SIZE;
}

void alarmKeyPad_Flush()
{
}

unsigned long get_nanos(void) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return (unsigned long)(ts.tv_sec * 1000000000L + ts.tv_nsec);
}

unsigned long get_micros(void) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return (unsigned long)(ts.tv_sec * 1000000L + ts.tv_nsec / 1000);
}

unsigned long get_mills(void) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return (unsigned long)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000);
}

int alarmKeyPad_pulseIn(GPIO_Value_Type state, long time_out)
{
	GPIO_Value_Type buttonState;
	unsigned long start = get_micros(), pulse_start;
	GPIO_GetValue(gpioLineFd, &buttonState);

	if (buttonState == state)
		return 0;

	while (buttonState != state)
	{
		unsigned long tt1 = get_micros();
		if (tt1 - start > time_out)
			return 0;
		GPIO_GetValue(gpioLineFd, &buttonState);
	}
	pulse_start = get_micros();

	while (buttonState == (state))
	{
		if (get_micros() - start > time_out)
			return 0;
		GPIO_GetValue(gpioLineFd, &buttonState);
	}
	int res = (get_micros() - pulse_start);
	//Log_Debug("Pulse Duration = %d \n", res);
	return res;
}

void delayNanoseconds(int delay)
{
	unsigned long tt = get_nanos();
	while (get_nanos() - tt < delay);
}

void delayMicroseconds(int delay)
{
	unsigned long tt = get_micros();
	while (get_micros() - tt < delay);
}

void AlarmKeyPad_writeChar(uint8_t chr)
{
	uint8_t bits[13];
	bits[0] = GPIO_Value_Low;
	chr = ~chr;
	int parity = 0;
	int cBit = 0;
	for (uint8_t i = 1; i < 9; i++)
	{
		cBit = chr & 1;
		parity += cBit;
		bits[i] = cBit;
		chr >>= 1;
	}
	cBit = parity & 1;
	bits[9] = cBit;
	bits[10] = GPIO_Value_High;

	//unsigned long tt = get_micros();
	for (uint8_t i = 0; i < 11; i++)
	{
		GPIO_SetValue(gpioTxFd, bits[i]);
		delayNanoseconds(bit4800Delay);
	}
	//tt = get_micros() - tt;
	//float fact = 2291.6670 / tt;
	//bit4800Delay = bit4800Delay * fact;
	//Log_Debug("Time for Byte: %d\n", tt);
}

uint8_t bbit[12] = { 1,0,0,0,0,0,0,0,0,0,0,0 };
void AlarmKeyPad_SendBit(uint8_t bit)
{
	bit = bit % 8 + 2;
	bbit[bit] = 1;
	for (uint8_t i = 0; i < 10; i++)
	{
		GPIO_SetValue(gpioTxFd, bbit[i]);
		delayNanoseconds(bit4800Delay);
	}

	bbit[bit] = 0;
}

void AlarmKeyPad_txLow()
{
	GPIO_SetValue(gpioTxFd, GPIO_Value_Low);
	delayNanoseconds(bit4800Delay);
}

void AlarmKeyPad_txHigh()
{
	GPIO_SetValue(gpioTxFd, GPIO_Value_High);
	delayNanoseconds(bit4800Delay);
}

int AlarmKeyPad_waitChange()
{
	GPIO_Value_Type lineInit, lineState;
	GPIO_GetValue(gpioLineFd, &lineInit);
	GPIO_GetValue(gpioLineFd, &lineState);
	unsigned long tt = get_micros();
	while (lineState == lineInit)
	{
		GPIO_GetValue(gpioLineFd, &lineState);
	}
	return (int)get_micros() - tt;
}