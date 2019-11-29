#include "i2c_drv.h"

#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <applibs/log.h>
#include <applibs/i2c.h>

int i2cFd = -1;

int initI2c(void) {

	// Begin MT3620 I2C init 

	i2cFd = I2CMaster_Open(2);
	if (i2cFd < 0) {
		Log_Debug("ERROR: I2CMaster_Open: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	int result = I2CMaster_SetBusSpeed(i2cFd, I2C_BUS_SPEED_STANDARD);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetBusSpeed: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}

	result = I2CMaster_SetTimeout(i2cFd, 100);
	if (result != 0) {
		Log_Debug("ERROR: I2CMaster_SetTimeout: errno=%d (%s)\n", errno, strerror(errno));
		return -1;
	}
	return 0;
}

void closeI2c(void) {

	if (i2cFd != -1)
		close(i2cFd);
}
