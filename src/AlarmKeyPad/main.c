#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include <applibs/log.h>
#include <applibs/gpio.h>

#include "i2c_drv.h"
#include "AlarmKeyPad.h"
#include "bus_listener/Bus_Reactor.h"

#include "oled.h"

#define BUFFER_LEN 100

char messageBuffer[BUFFER_LEN];


void display_message(char* message)
{
	update_oled_message(message);
}

/* Callback function for bus_listener */
void publish_debug_message(char* mensaje) 
{
	Log_Debug(mensaje);
}

/* Callback function for bus_listener */
void upated_event() {
	statusHandler_to_string(messageBuffer);

	Log_Debug("\nStatus to string\n");
	publish_debug_message(messageBuffer);

	statusHandler_debug_to_string(messageBuffer);
	Log_Debug("\nStatus debug to string\n");
	publish_debug_message(messageBuffer);
}

/* Callback function for bus_listener */
void message_updated() {
	// 32 characters
	displayHandler_get_display_message(messageBuffer);

	display_message(messageBuffer);
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

	// Attach Callbacks to bus_listener
	busReactor_Init(DEVICE_ADDRESS);
	busReactor_attach_display(message_updated);
	busReactor_attach_status(upated_event);
	busReactor_attach_debug(publish_debug_message);

	// Start I2C
	initI2c();

	// Start OLED
	if (oled_init())
	{
		Log_Debug("OLED not found!\n");
	}
	else
	{
		Log_Debug("OLED found!\n");
	}

	// Draw AVNET logo
	oled_draw_logo();

    while (true) {
		busReactor_handleEvents();
    }

	closeI2c();
}