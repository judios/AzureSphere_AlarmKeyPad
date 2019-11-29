#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>
#include <applibs/i2c.h>

#include "AlarmKeyPad.h"
#include "bus_listener/Bus_Reactor.h"

#define BUFFER_LEN 100

char messageBuffer[BUFFER_LEN];
void publish_debug_message(char* mensaje) 
{
	Log_Debug(mensaje);
}

void upated_event() {
	statusHandler_to_string(messageBuffer);

	Log_Debug("\nStatus to string\n");
	publish_debug_message(messageBuffer);

	statusHandler_debug_to_string(messageBuffer);
	Log_Debug("\nStatus debug to string\n");
	publish_debug_message(messageBuffer);
}

void message_updated() {
	// 32 characters
	displayHandler_get_display_message(messageBuffer);

	Log_Debug("\nDisplay Message\n");
	Log_Debug(messageBuffer);

	displayHandler_debug_to_string(messageBuffer);
	Log_Debug("\nDisplay debug to string\n");
	publish_debug_message(messageBuffer);
}


int main(void)
{
	alarmKeyPad_Init();
	if (uartFd < 0)
	{
		Log_Debug("ERROR: Could not open UART: %s (%d).\n", strerror(errno), errno);
		return -1;
	}
	else
	{
		Log_Debug("KeyPad created on UART (%d)", uartFd);
	}
	   
	busReactor_Init(DEVICE_ADDRESS);
	busReactor_attach_display(message_updated);
	busReactor_attach_status(upated_event);
	busReactor_attach_debug(publish_debug_message);


    while (true) {
		busReactor_handleEvents();
    }
}