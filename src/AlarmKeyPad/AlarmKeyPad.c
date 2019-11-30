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
int uartFd = -1;
int rxb_head = 0;
int rxb_tail = 0;
char readBuffer[RX_BUFFER_SIZE];


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

size_t alarmKeyPad_Write(char val)
{
	ssize_t bytesSent = write(uartFd, &val, 1);
	while(bytesSent == 0)
		bytesSent = write(uartFd, &val, 1);

	return bytesSent;
}

void alarmKeyPad_Flush()
{
}

static unsigned long get_micros(void) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return (unsigned long)ts.tv_sec * 1000000L + ts.tv_nsec/1000;
}

unsigned long get_mills(void) {
	struct timespec ts;
	timespec_get(&ts, TIME_UTC);
	return (unsigned long)(ts.tv_sec * 1000L + ts.tv_nsec / 1000000);
}


int alarmKeyPad_pulseIn(GPIO_Value_Type state, long time_out )
{
	GPIO_Value_Type buttonState;
	unsigned long start = get_micros(), pulse_start;
	GPIO_GetValue(gpioLineFd, &buttonState);
	
	if (buttonState == state) 
		return 0;

	while (buttonState != state)
	{
		if (get_micros() - start > time_out) 
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
	int res = (get_micros() - pulse_start) ;
	Log_Debug("Pulse Duration = %d \n", res);
	return res;
}
