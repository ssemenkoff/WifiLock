C code for ATMega32 connected with ESP8266 and servo for lock controlled via WiFi.

Finite automation for microcontroller:

States:
| State                   | >   | UART Send                 | >   | UART Recieve   | >   | New state             |
| ----------------------- | --- | --------------------------| --- | -------------- | --- | --------------------- |
| ESPStart,               | >   | AT                        | >   | OK             | >   | ESPInited             |
| ESPInited,              | >   | AT+CWMODE=3               | >   | OK             | >   | ESPModeSet            |
| ESPModeSet,             | >   | AT+CWQAP                  | >   | OK             | >   | ESPDisconected        |
| ESPDisconected,         | >   | AT+CWJAP="net","pas"      | >   | OK             | >   | ESPConnected          |
| ESPConnected,           | >   | AT+CIPSTART="TCP","",80   | >   | OK             | >   | ESPConnectionStarted  |
| ESPConnectionStarted,   | >   | AT+CIPSEND=21             | >   | OK             | >   | ESPDataStreamOpened   |
| ESPDataStreamOpened,    | >   | GET /state HTTP/1.1       | >   | SEND OK        | >   | ESPDataSent           |
| ESPDataSent,            | >   |                           | >   | { data }       | >   | ESPDataRecieved       |
| ESPDataRecieved,        | >   | AT+CIPCLOSE               | >   | OK | ERROR     | >   | ESPConnectionClosed   | 
| ESPConnectionClosed     | >   |                           | >   |                | >   | ESPConnected          |