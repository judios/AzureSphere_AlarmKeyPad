# VISTA ICM Replacement
This project is designed to allow you to connect an Arduino-like device to your Honeywell (Ademco) security panel and "listen in" for key events.  This project is an implemenation of reverse engineering the Ademco ECP keypad bus.


#Config HardwareSerial
```c
#define DEVICE_ADDRESS 19

HardwareSerial *myPanelSerial = &Serial1;
BUS_Reactor vista20p(&myPanelSerial, DEVICE_ADDRESS)
```

#Hardware Setup 
Using a MAX3232 Breakout [MAX3232](https://www.sparkfun.com/products/11189)
Connect the data-out wire (yellow) to a max3232 (R1IN) and the output (R1OUT) to pin 0(RX) on the Arduino. 
Connect the data-in wire (green) to a max3232 (T1OUT) and the output (T1IN) to pin 1(TX) on the Arduino. 

                 Read Only Interface                 |   Arduino    
                                                     |             
    +---------+               +------------+         |  +--------+ 
    |         | Yellow Wire   |   MAX3232  |TTL      |  |        | 
    | Ademco  |-------------->|R1IN   R1OUT|---------|->|0(RX)   | 
    |Vista 20P| 12V           |            | 5V      |  |        | 
    |  Panel  |               |            |         |  |        | 
    |         |<--------------|T1OUT   T1IN|<--------|--|1(TX)   | 
    +---------+ Green Wire    +------------+         |  +--------+ 
                                                     |             
                    

##Current address write status

Real keypad address 19:
 
![alt text](https://raw.githubusercontent.com/matlock08/homesecurity/master/docs/panelKeypad19OK.png "Keypad addr 19")

Arduino address 19: 
![alt text](https://raw.githubusercontent.com/matlock08/homesecurity/master/docs/arduinoKeypad19OK.png "Arduino addr 19")

#IOT
This sketch is publishing changes to a free [blynk server](http://www.blynk.cc/) you can download the native android aplication to see LCD on phone

Disarmed                   |  Fault                    |  System low Bat 
:-------------------------:|:-------------------------:|:-------------------------:
![alt text](https://raw.githubusercontent.com/matlock08/homesecurity/master/docs/disarmed.png "Disarmed")  |  ![alt text](https://raw.githubusercontent.com/matlock08/homesecurity/master/docs/fault.png "Fault")  |  ![alt text](https://raw.githubusercontent.com/matlock08/homesecurity/master/docs/system_lo_bat.png "System low BAT")  

 

#Protocol
Essentially, the data out wire uses 8-bit, even parity, 1 stop bit, inverted, NRZ +12 volt TTL signals.  But, the data out wire also acts somewhat like a clock wire sometimes.  

Regular messages are transmitted periodically and begin with either F7 or F2.  F7 messages are fixed length, F2 are dynamic length.  Both message types consist of a header and a body section.  The second by of F2 messages indicate the remaining bytes for said message.

When behaving as a clock wire, the panel will pull the line low for an extended period of time (&gt;10ms).  When this happens, any device that has data to send should pusle a response.  If the panel selects that device (in case there are multiple devices needing to send data) an F6 will be broadcast on the data out wire with the address of the device which should send data.  F6 messages behave like normal serial data.


##Pulsing
In order to avoid "open circuit" errors, every device periodically sends their device address to the panel.  This happens after F7 messages, and as a response to input querying from the panel.

Pulsing doesn't exactly match up with serial data.  Timing is handled by synchronizing to rising edges sent from the server.  This can be faked by sending a 0xFF byte because the start bit will raise the line for a short time, and all the 1s will bring the line low (because the data signals are inverted).  There doesn't seem to be any parity bits sent during this pusling phase.

When multiple devices ack, their pulses should coincide.  The result is that the last bit is ANDed together.

Address 16 - 11111110  (inverted on the wire looks like 00000001)
Address 21 - 11011111  (inverted on the wire looks like 00100000)
Together on the wire they look like a single byte of    00100001 0x21

In order to get perfect AND logic from multiple devices sending pulses at the same time, they must pulse a start bit and no data (0xFF) twice before sending their address bit mask.  When the line is pulled low for more than 12 ms, and a keypad has some information to transmit, this pulsing should synchronize with the rising edges from the panel.


    (high)---\___(10ms low)___/---\______/---\_______/---\_____  (Yellow data-out)
    
    __________________________/\_________/\__________/\_/\_____  (0xFF, 0xFF, 0xEF) (Green data-in)

```c
				LSB                         MSB
    				1248 1248  1248 1248  1248 1248
Address - 16    FF,FF,FE	1111 1111  1111 1111  0111 1111
Address - 17	FF,FF,FD	1111 1111  1111 1111  1011 1111
Address - 18	FF,FF,FB	1111 1111  1111 1111  1101 1111
Address - 19	FF,FF,F7	1111 1111  1111 1111  1110 1111 
Address - 20	FF,FF,EF	1111 1111  1111 1111  1111 0111
Address - 21	FF,FF,DF  	1111 1111  1111 1111  1111 1011
Address - 22	FF,FF,BF  	1111 1111  1111 1111  1111 1101
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
