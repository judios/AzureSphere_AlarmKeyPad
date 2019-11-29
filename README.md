

# VISTA ICM Replacement
This project is designed to allow you to connect an Azure Sphere MT3620 based device to your Honeywell (Ademco) security panel and "listen in" for key events.  This project is an implemenation of the reverse engineering of the Ademco ECP keypad bus devloped by Mark Kimsal, and modified by Morpheus (See credits at the bottom of this page). 


#Hardware Setup 

This project is being developed using the following:
 - Honeywell Vista 20P Security Panel Version 10.23
 - Azure Sphere MT3620 Starter Kit by Avnet with OLED Display.
 - HiLetgo Mini RS232 to TTL (uses MAX3232).

Connect Vista 20 Terminal 7 (data-out wire, yellow) to a max3232 (R1IN) and the output (R1OUT) to pin GPIO28 (RX0, RX in Socket 1) on the Azure Sphere Kit.  Alternatively, one can use GPIO33(RX1, MISO in Socket 1)

                 Read Only Interface                 |   MT3620    
                                                     |             
    +---------+               +------------+         |  +--------+ 
    |         | Yellow Wire   |   MAX3232  |TTL      |  |        | 
    | Ademco  |-------------->|R1IN   R1OUT|---------|->|GPIO28  | 
    |Vista 20P| 12V           |            | 5V      |  |        | 
    |  Panel  |               |            |         |  |        | 
    |         |               |T1OUT   T1IN|         |  |        | 
    +---------+ Green Wire    +------------+         |  +--------+ 
   
                                                     |             
#Project Scope

The initial project scope is set to monitor the panel and output the messages to an OLED display connected to the MT3620 Starter Kit. The source code developed by Morpheus had to be translated to c in order for this to be used in a Azure Sphere high level App.                 

#Protocol
Essentially, the data out wire uses 8-bit, even parity, 1 stop bit, inverted, NRZ +12 volt TTL signals.  But, the data out wire also acts somewhat like a clock wire sometimes.  

Regular messages are transmitted periodically and begin with either F7 or F2.  F7 messages are fixed length, F2 are dynamic length.  Both message types consist of a header and a body section.  The second by of F2 messages indicate the remaining bytes for said message.

When behaving as a clock wire, the panel will pull the line low for an extended period of time (&gt;10ms).  When this happens, any device that has data to send should pusle a response.  If the panel selects that device (in case there are multiple devices needing to send data) an F6 will be broadcast on the data out wire with the address of the device which should send data.  F6 messages behave like normal serial data.


#Pulsing
In order to avoid "open circuit" errors, every device periodically sends their device address to the panel.  This happens after F7 messages, and as a response to input querying from the panel.

Pulsing doesn't exactly match up with serial data.  Timing is handled by synchronizing to rising edges sent from the server.  This can be faked by sending a 0xFF byte because the start bit will raise the line for a short time, and all the 1s will bring the line low (because the data signals are inverted).  There doesn't seem to be any parity bits sent during this pusling phase.

When multiple devices ack, their pulses should coincide.  The result is that the last bit is ANDed together.

Address 16 - 11111110  (inverted on the wire looks like 00000001)
Address 21 - 11011111  (inverted on the wire looks like 00100000)
Together on the wire they look like a single byte of    00100001 0x21


    (high)---\___(10ms low)___/---\______/---\_______/---\_____  (Yellow data-out)
    
    __________________________/\_________/\__________/\_/\_____  (0xFF, 0xFF, 0xEF) (Green data-in)

```c
                         LSB                         MSB
                         1248 1248  1248 1248  1248 1248
Address - 16    FF,FF,FE 1111 1111  1111 1111  0111 1111
Address - 17    FF,FF,FD 1111 1111  1111 1111  1011 1111
Address - 18    FF,FF,FB 1111 1111  1111 1111  1101 1111
Address - 19    FF,FF,F7 1111 1111  1111 1111  1110 1111
Address - 20    FF,FF,EF 1111 1111  1111 1111  1111 0111
Address - 21    FF,FF,DF 1111 1111  1111 1111  1111 1011
Address - 22    FF,FF,BF 1111 1111  1111 1111  1111 1101
```
#License
This project uses some parts of Arduino IDE - specifically the SoftwareSerial library.  So, whatever license that is under, this project is under (for the time being).

#Credits
I could not have cracked the nut of Vista ECP protocol without knowing that it was possible by reading Miguel Sanchez's article in Circuit Cellar.  Although he did not process signals sent from the panel, he figured out the baud rate, stop bit configuration, and the fact that the signal is inversed (0 is high, 1 is low).

Other projects that were invaluable to me are:

* Arduino SUMP analyzer firmware:
  * [http://github.com/gillham/logic_analyzer]
* Open Logic Sniffer (desktop):
  * [http://www.lxtreme.nl/ols/]
* Original Code Mark Kimsal (homesecurity):
  * [https://github.com/markkimsal/homesecurity]
* Modified  Code Morpheus (homesecurity):
  * [https://github.com/matlock08/homesecurity]