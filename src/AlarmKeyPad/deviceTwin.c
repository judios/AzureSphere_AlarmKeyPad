
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "deviceTwin.h"
#include "build_options.h"
#include "azure_iot_utilities.h"
#include "bus_listener/Bus_Reactor.h"

#include <applibs/log.h>


void checkAndUpdateDeviceTwin(char* property, void* value, data_type_t type, bool ioTCentralFormat)
{
	int nJsonLength = -1;

	char* pjsonBuffer = (char*)malloc(JSON_BUFFER_SIZE);
	if (pjsonBuffer == NULL) {
		Log_Debug("ERROR: not enough memory to report device twin changes.");
	}

	if (property != NULL) {

		// report current device twin data as reported properties to IoTHub

		switch (type) {
		case TYPE_BOOL:
#ifdef IOT_CENTRAL_APPLICATION
			if (ioTCentralFormat) {
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonBoolIOTC, property, *(bool*)value ? "true" : "false", desiredVersion);
			}
			else
#endif 
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonBool, property, *(bool*)value ? "true" : "false", desiredVersion);

			break;
		case TYPE_FLOAT:
#ifdef IOT_CENTRAL_APPLICATION			
			if (ioTCentralFormat) {
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonFloatIOTC, property, *(float*)value, desiredVersion);
			}
			else
#endif 
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonFloat, property, *(float*)value, desiredVersion);
			break;
		case TYPE_INT:
#ifdef IOT_CENTRAL_APPLICATION		
			if (ioTCentralFormat) {
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonIntegerIOTC, property, *(int*)value, desiredVersion);
			}
			else
#endif
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonInteger, property, *(int*)value, desiredVersion);
			break;
		case TYPE_STRING:
#ifdef IOT_CENTRAL_APPLICATION			
			if (ioTCentralFormat) {
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonStringIOTC, property, (char*)value, desiredVersion);
			}
			else
#endif 
				nJsonLength = snprintf(pjsonBuffer, JSON_BUFFER_SIZE, cstrDeviceTwinJsonString, property, (char*)value, desiredVersion);
			break;
		}

		if (nJsonLength > 0) {
			if(LOG_IOT)
				Log_Debug("[MCU] Updating device twin: %s\n", pjsonBuffer);
			AzureIoT_TwinReportStateJson(pjsonBuffer, (size_t)nJsonLength);
		}
		free(pjsonBuffer);
	}
}


void deviceTwinChangedHandler(JSON_Object* desiredProperties)
{
	int result = 0;

	// Pull the twin version out of the message.  We use this value when we echo the new setting back to IoT Connect.
	if (json_object_has_value(desiredProperties, "$version") != 0)
	{
		desiredVersion = (int)json_object_get_number(desiredProperties, "$version");
	}

}


void deviceMessageReceived(const char* payload)
{

}

int deviceDirectMethod(const char* directMethodName, const char* payload,
	size_t payloadSize, char** responsePayload,
	size_t* responsePayloadSize)
{
	if (strcmp(directMethodName, "clearFaults") == 0)
	{

		static const char clearFaultExecuted[] = "\"Fault Zones Cleared\"";
		*responsePayloadSize = strlen(clearFaultExecuted);
		*responsePayload = (unsigned char*)malloc(*responsePayloadSize);
		if (*responsePayload != NULL) {
			strncpy((char*)(*responsePayload), clearFaultExecuted, *responsePayloadSize);
		}
		else {
			Log_Debug("ERROR: Cannot create response message for method call.\n");
			abort();
		}
		busReactor_RunCommand(clearFaults);
		Log_Debug("Clear Faults Called\n");
		return 1;
	}

	if (strcmp(directMethodName, "armAwayCmd") == 0)
	{

		static const char clearFaultExecuted[] = "\"Armed Away\"";
		*responsePayloadSize = strlen(clearFaultExecuted);
		*responsePayload = (unsigned char*)malloc(*responsePayloadSize);
		if (*responsePayload != NULL) {
			strncpy((char*)(*responsePayload), clearFaultExecuted, *responsePayloadSize);
		}
		else {
			Log_Debug("ERROR: Cannot create response message for method call.\n");
			abort();
		}
		busReactor_RunCommand(armAway);
		Log_Debug("Arm Away Called\n");
		return 1;
	}

	if (strcmp(directMethodName, "armStayCmd") == 0)
	{

		static const char clearFaultExecuted[] = "\"Armed Stay\"";
		*responsePayloadSize = strlen(clearFaultExecuted);
		*responsePayload = (unsigned char*)malloc(*responsePayloadSize);
		if (*responsePayload != NULL) {
			strncpy((char*)(*responsePayload), clearFaultExecuted, *responsePayloadSize);
		}
		else {
			Log_Debug("ERROR: Cannot create response message for method call.\n");
			abort();
		}
		busReactor_RunCommand(armStay);
		Log_Debug("Arm Stay Called\n");
		return 1;
	}

	if (strcmp(directMethodName, "disarmCmd") == 0)
	{

		static const char clearFaultExecuted[] = "\"Disarmed\"";
		*responsePayloadSize = strlen(clearFaultExecuted);
		*responsePayload = (unsigned char*)malloc(*responsePayloadSize);
		if (*responsePayload != NULL) {
			strncpy((char*)(*responsePayload), clearFaultExecuted, *responsePayloadSize);
		}
		else {
			Log_Debug("ERROR: Cannot create response message for method call.\n");
			abort();
		}
		busReactor_RunCommand(disarm);
		Log_Debug("Disarm Called\n");
		return 1;
	}
}
