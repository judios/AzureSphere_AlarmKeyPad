#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "applibs_versions.h"
#include "azure_iot_utilities.h"
#include "connection_strings.h"
#include "build_options.h"

#include <applibs/log.h>
#include <applibs/gpio.h>
#include "i2c_drv.h"
#include <applibs/wificonfig.h>
#include <azureiot/iothub_device_client_ll.h>

#include "deviceTwin.h"

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

char lastStatusMsg[100] = "";
char lastDisplayMessage[100] = "";

// Variables used to keep track of the Zone States
unsigned long Zones[8] = { 0,0,0,0,0,0,0,0 };
char zonetag[6];
bool zoneStatT = true;
bool zoneStat = false;


/* Callback function for bus_listener */
void upated_event() {
	statusHandler_to_string(messageBuffer);

	Log_Debug("\nStatus to string\n");
	publish_debug_message(messageBuffer);
	if (strcmp(lastStatusMsg, messageBuffer) != 0)
	{
		checkAndUpdateDeviceTwin("status_msg", messageBuffer, TYPE_STRING, false);
		strcpy(lastStatusMsg, messageBuffer);
		if(statusHandler_get_fault() == F2_BYTE4_FAULT_ALARM)
			AzureIoT_SendMessage("{\"triggered\": \"1\"}");
	}

	statusHandler_debug_to_string(messageBuffer);
	Log_Debug("\nStatus debug to string\n");
	publish_debug_message(messageBuffer);
}

void message_updated() {
	// 32 characters
	displayHandler_get_display_message(messageBuffer);

	display_message(messageBuffer);
	Log_Debug("\nDisplay Message\n");
	Log_Debug(messageBuffer);
	
	// Update zone state if message related to Zone Fault
	if (!displayHandler_get_data2())
	{
		int zone = displayHandler_get_zone_field();

		Log_Debug("\n%03x",zone );
		if (zone > 0 && zone < 9)
		{
			zone--;
			if (Zones[zone] == 0)
			{
				sprintf(zonetag, "zone%1d", zone+1);
				checkAndUpdateDeviceTwin(zonetag, &zoneStatT, TYPE_BOOL, false);
				Log_Debug("%s at Fault", zonetag);
			}
			Zones[zone] = get_mills();
		}

	}

	if (strcmp(lastDisplayMessage, messageBuffer) != 0)
	{
		checkAndUpdateDeviceTwin("lastMessages", messageBuffer, TYPE_STRING, false);
		strcpy(lastDisplayMessage, messageBuffer);
	}

	displayHandler_debug_to_string(messageBuffer);
	Log_Debug("\nDisplay debug to string\n");
	publish_debug_message(messageBuffer);
}

// If not fault message received in 10 seconds, clear the zone fault
void clearZones()
{
	for (int i = 0; i < 8; i++)
	{
		if(Zones[i] > 0 && get_mills() - Zones[i] > 10000)
		{
			sprintf(zonetag, "zone%1d", i + 1);
			checkAndUpdateDeviceTwin(zonetag, &zoneStat, TYPE_BOOL, false);
			Log_Debug("%s Cleared", zonetag);
			Zones[i] = 0;
		}

	}

}


extern IOTHUB_DEVICE_CLIENT_LL_HANDLE iothubClientHandle;
uint32_t frequency;
char bssid[20];

int main(int argc, char* argv[])
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




	// Change this GPIO number and the number in app_manifest.json if required by your hardware.
	int fd = GPIO_OpenAsOutput(9, GPIO_OutputMode_PushPull, GPIO_Value_High);
	if (fd < 0) {
		Log_Debug(
			"Error opening GPIO: %s (%d). Check that app_manifest.json includes the GPIO used.\n",
			strerror(errno), errno);
		return -1;
	}

	const struct timespec sleepTime = { 2, 0 };
	display_message("Testing the message again");

	AzureIoT_SetDeviceTwinUpdateCallback(&deviceTwinChangedHandler);
	AzureIoT_SetMessageReceivedCallback(&deviceMessageReceived);

	if (!AzureIoT_SetupClient()) {
		Log_Debug("ERROR: Failed to set up IoT Hub client\n");
		return -1;
	} 

	WifiConfig_ConnectedNetwork network;
	int result = WifiConfig_GetCurrentNetwork(&network);

	if (result < 0)
	{
		Log_Debug("INFO: Not currently connected to a WiFi network.\n");
		return -1;
	}

	// Itinialize States Assuming everything is fine
	checkAndUpdateDeviceTwin("zone1", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("zone2", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("zone3", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("zone4", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("zone5", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("zone6", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("zone7", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("zone8", &zoneStat, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("battery", &zoneStatT, TYPE_BOOL, false);
	checkAndUpdateDeviceTwin("status_msg", "Azure Device Connected 20", TYPE_STRING, false);
	checkAndUpdateDeviceTwin("lastMessages", "Azure Device Connected 40", TYPE_STRING, false);

	AzureIoT_SendMessage("{\"state\": \"DisarmReady\"}");


	while (true) {

		result = WifiConfig_GetCurrentNetwork(&network);

		if (result < 0)
		{
			Log_Debug("INFO: Not currently connected to a WiFi network.\n");
		}
		else
		{
			busReactor_handleEvents();
			AzureIoT_DoPeriodicTasks();
			clearZones();
		}
	}
	

	closeI2c();
	
}