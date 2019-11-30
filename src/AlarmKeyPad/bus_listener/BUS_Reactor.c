/*
    Copyright 2013 Jose Castellanos Molina
    
	Modified in 2019 for use with AlarmKePad project by Julian Diaz

    This file is part of homesecurity.

    homesecurity is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    homesecurity is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with homesecurity.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "Bus_Reactor.h"
#include <applibs/log.h>

// Callbacks references
panelStatusCallback callbackStatus;
panelDisplayCallback callbackDisplay;
panelDebugProtocolCallback debugCallback;
panelF9Callback callbackF9;

// Flag to track send request
bool wantToSend;

// Internal Device Address
int device_address;
int sequence;
int8_t header;
char keys_to_send[KEY_MESSAGE_LEN];

/*
 * Called after and F6 even to give us the chance to
 * write to the BUS, if it is our address
 *//*
 * Called after and F6 even to give us the chance to
 * write to the BUS, if it is our address
 */
int count = 0;
void busReactor_on_acknowledge() {

	int devAdrr = acknowledgeHandler_get_ack_address();
	Log_Debug("Device Address %d\n", devAdrr);
	delayMicroseconds(210);
	if (devAdrr == device_address) {

		uint8_t charsSend = strlen(keys_to_send);

		if (charsSend > 0 && charsSend < KEY_MESSAGE_LEN) {
			delayMicroseconds(600); 
			AlarmKeyPad_txHigh();
			sequence = acknowledgeHandler_get_seq_number();
			int bufferIndex = 0;

			// 2 MSB represent the Seq
			// 6 LSB represent address
			// 1100 0000 0xc0
			// 0011 1111 0x3f
			header = (((++sequence << 6) & 0xc0) ^ 0xc0) | (device_address & 0x3f);
			AlarmKeyPad_writeChar(header);
			AlarmKeyPad_writeChar(charsSend + 1); // message length

			int chksum = charsSend + 1;
			for (int i = 0; i < charsSend; i++) {
				// [0-9]
				if (keys_to_send[i] >= 0x30 && keys_to_send[i] <= 0x39) {
					AlarmKeyPad_writeChar((keys_to_send[i] - 0x30));
					chksum += (keys_to_send[i] - 0x30);
					// [#]
				}
				else if (keys_to_send[i] == 0x23) {
					AlarmKeyPad_writeChar(0x0B);
					chksum += 0x0B;
					// [*]
				}
				else if (keys_to_send[i] == 0x2A) {
					AlarmKeyPad_writeChar(0x0A);
					chksum += 0x0A;
					// [A B C D]
				}
				else if (keys_to_send[i] >= 0x41 && keys_to_send[i] <= 0x44) {
					AlarmKeyPad_writeChar((keys_to_send[i] - 0x25));
					chksum += (keys_to_send[i] - 0x25);
				}
			}

			AlarmKeyPad_writeChar((uint8_t)((0x100 - header - chksum) & 0xFF)); // checksum
			AlarmKeyPad_txLow();
			//Log_Debug("Change Line %d\n", pulse);
		}
	}
}

/********************************************
 *  PUBLIC Methods
 *
 ********************************************/


void busReactor_Init( int device_address_parm) {
    wantToSend = false;
    sequence = 1;
    header = 0x00;
    device_address = device_address_parm;
    callbackDisplay = NULL;
    callbackStatus = NULL; 
}

/*
 * handle Serial events must be called inside the loop method
 */
void busReactor_handleEvents() {

	if (wantToSend == false) {
		// handle Reception
		if (alarmKeyPad_Available() > 0) {
			char cr = alarmKeyPad_Read();

			if (cr == F7_DISPLAY_EVENT) {

				if (displayHandler_handle_event(F7_DISPLAY_EVENT) > 0) {
					busReactor_acknowledgeAddress();

					// IF there is no callback exit without processing
					if (callbackDisplay != NULL) {
						(*callbackDisplay)();
					}
				}
			}
			else if (cr == F6_ACK_EVENT) {

				if (acknowledgeHandler_handle_event(F6_ACK_EVENT) > 0) {
					busReactor_on_acknowledge();
				}
			}
			else if (cr == F2_STATUS_EVENT) {

				if (statusHandler_handle_event(F2_STATUS_EVENT) > 0) {
					// IF there is no callback exit without processing
					if (callbackStatus == NULL) {
						return;
					}
					(*callbackStatus)();
				}

			}
			else if (cr == F9E_UNK_EVENT) {

				if (msg9eHandler_handle_event(F9E_UNK_EVENT) > 0) {

					if (callbackF9 == NULL) {
						return;
					}
					(*callbackF9)();
				}

			}
			else if (header != 0x00 && cr == header) {
				header = 0x00;
				if (debugCallback == NULL) {
					return;
				}

				char auxBuffer[12];
				sprintf(auxBuffer, "!MACK:[%02x]", cr);
				(*debugCallback)(auxBuffer);

			}
			else {
				// Unknown messages or cero
				if (debugCallback == NULL && cr != 0x00) {
					return;
				}

				/*
				  char auxBuffer[4];
				  sprintf(auxBuffer,"%02x,", cr );
				  (*debugCallback)( auxBuffer );  
				*/
			}

			// No incomming data ? check if transmition is needed
		}
	}
	else if (wantToSend == true) 
	{
		busReactor_acknowledgeAddress();
	}
}

void busReactor_acknowledgeAddress() {
	// Waits on pin1 for a HIGH value is at least 12 millisec   

	if (alarmKeyPad_pulseIn(GPIO_Value_High, DEFAULT_PULSE_TIMEOUT) >= 12000) {

		delayMicroseconds(2300);
		alarmKeyPad_pulseIn(GPIO_Value_High, DEFAULT_PULSE_TIMEOUT);

		delayMicroseconds(2300);
		alarmKeyPad_pulseIn(GPIO_Value_High, DEFAULT_PULSE_TIMEOUT);

		AlarmKeyPad_SendBit(3);
		wantToSend = false;
		Log_Debug("Address Sent\n");
	}
}

void busReactor_request_to_send(const char *messageToKeyPad) {
    memset(keys_to_send, 0x00, sizeof(keys_to_send));
    strncpy(keys_to_send, messageToKeyPad, KEY_MESSAGE_LEN);
    wantToSend = true;
}

void busReactor_attach_display(panelDisplayCallback displayCallback)
{
	callbackDisplay = displayCallback;
}

void busReactor_attach_status(panelStatusCallback statusCallback)
{
	callbackStatus = statusCallback;
}

void busReactor_attach_f9(panelF9Callback _callbackF9)
{
	callbackF9 = _callbackF9;
}

void busReactor_attach_debug(panelDebugProtocolCallback _debugCallback)
{
	debugCallback = _debugCallback;
}

void busReactor_deattach_debug()
{
}

int busReactor_RunCommand(runCmd cmd)
{
	switch (cmd)
	{
	case clearFaults:
		busReactor_request_to_send("41121");
		break;

	case armAway:
		busReactor_request_to_send("41122");
		break;

	case armStay:
		busReactor_request_to_send("41123");
		break;

	case disarm:
		busReactor_request_to_send("41121");
		break;

	default:
		break;

	}

}

