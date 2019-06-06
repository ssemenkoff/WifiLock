C code for ATMega32 connected with ESP8266 and servo for lock controlled via WiFi.

Finite automation for microcontroller:

States:
     State                             UART Send                       UART Recieve         New state
1.   ESPStart,                   -->   AT                        -->   OK             -->   ESPInited
2.   ESPInited,                  -->   AT+CWMODE=3               -->   OK             -->   ESPModeSet
3.   ESPModeSet,                 -->   AT+CWQAP                  -->   OK             -->   ESPDisconected
4.   ESPDisconected,             -->   AT+CWJAP="net","pas"      -->   OK             -->   ESPConnected
5.   ESPConnected,               -->   AT+CIPSTART="TCP","",80   -->   OK             -->   ESPConnectionStarted
6.   ESPConnectionStarted,       -->   AT+CIPSEND=21             -->   OK             -->   ESPDataStreamOpened
7.   ESPDataStreamOpened,        -->   GET /state HTTP/1.1       -->   SEND OK        -->   ESPDataSent
8.   ESPDataSent,                -->                             -->   { data }       -->   ESPDataRecieved
9.   ESPDataRecieved,            -->   AT+CIPCLOSE               -->   OK | ERROR     -->   ESPConnectionClosed
10.  ESPConnectionClosed         -->                             -->                  -->   ESPConnected