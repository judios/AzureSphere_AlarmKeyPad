#pragma once

/*
  Variables set in the IotCentral App.

  Device Properties:
    "zone1", "zone2", "zone3", "zone4", 
	"zone5", "zone6", "zone7", "zone8", 
	"battery", "status_msg", "lastMessages"

  Telemetry State
    "state"

  Telemetry Event
    "triggered"

*/

#include "parson.h"
#include "build_options.h"


#define JSON_BUFFER_SIZE 204


static const char cstrDeviceTwinJsonInteger[] = "{\"%s\": %d}";
static const char cstrDeviceTwinJsonFloat[] = "{\"%s\": %.2f}";
static const char cstrDeviceTwinJsonBool[] = "{\"%s\": %s}";
static const char cstrDeviceTwinJsonString[] = "{\"%s\": \"%s\"}";
#ifdef IOT_CENTRAL_APPLICATION
static const char cstrDeviceTwinJsonFloatIOTC[] = "{\"%s\": {\"value\": %.2f, \"status\" : \"completed\" , \"desiredVersion\" : %d }}";
static const char cstrDeviceTwinJsonBoolIOTC[] = "{\"%s\": {\"value\": %s, \"status\" : \"completed\" , \"desiredVersion\" : %d }}";
static const char cstrDeviceTwinJsonIntegerIOTC[] = "{\"%s\": {\"value\": %d, \"status\" : \"completed\" , \"desiredVersion\" : %d }}";
static const char cstrDeviceTwinJsonStringIOTC[] = "{\"%s\": {\"value\": \"%s\", \"status\" : \"completed\" , \"desiredVersion\" : %d }}";
#endif


static int desiredVersion = 1;

typedef enum {
	TYPE_INT = 0,
	TYPE_FLOAT = 1,
	TYPE_BOOL = 2,
	TYPE_STRING = 3
} data_type_t;

typedef struct {
	char* twinKey;
	void* twinVar;
	data_type_t twinType;
	bool active_high;
} twin_t;

void checkAndUpdateDeviceTwin(char*, void*, data_type_t, bool);


void deviceTwinChangedHandler(JSON_Object* desiredProperties);

void deviceMessageReceived(const char* payload);
