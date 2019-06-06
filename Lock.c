#ifndef F_CPU
#define F_CPU 8000000UL
#endif

#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#define IO_SIZE 64
#define RightComand "Right"
#define LeftComand "Left"

#define StartComand "AT\r\n"
#define SetModeComand "AT+CWMODE=3\r\n"
#define DisconnectWiFiComand "AT+CWQAP\r\n"
#define ConnectWiFiComand "AT+CWJAP=\"ssemenkoff-private\",\"semenkoff\"\r\n"
#define ConnectionStartCommand "AT+CIPSTART=\"TCP\",\"192.168.100.10\",80\r\n"
#define OpenDataStreamComand "AT+CIPSEND=21\r\n"
#define SendData "GET /state HTTP/1.1\r\n"
#define CloseConnectionComand "AT+CIPCLOSE\r\n"

#define ResponceOK "OK"
#define ResponceERROR "ERROR"

unsigned char sBuffer[IO_SIZE];
unsigned char *sPut;
unsigned char *sGet;
int buffer_length;

bool state_opened = true;
int current_state;

enum ESP8266_States {
  ESPStart,
  ESPInited,
  ESPModeSet,
  ESPDisconected,
  ESPConnected,
  ESPConnectionStarted,
  ESPDataStreamOpened,
  ESPDataSent,
  ESPDataRecieved,
  ESPConnectionClosed
};

void SIOInit(void)
{
    sPut = sBuffer;
    sGet = sBuffer;
    buffer_length = 0;
}

void Close(void) {
  state_opened = false;
  RotateTo(right);
}

void Open() {
  state_opened = true;
  RotateTo(left);
}

void SIOPut(unsigned char input)
{
    *sPut = input;
    sPut++;
    buffer_length++;

    if (sPut >= (sBuffer + IO_SIZE)) {
        sPut = sBuffer;
        buffer_length = 0;
    }
}

unsigned char SIOGet(void)
{
    unsigned char sreturn;
    sreturn = *sGet;
    sGet++;
    buffer_length--;

    if (sGet>= (sBuffer + IO_SIZE))
        sGet= sBuffer;

    return sreturn;
}  

void SIOClear(void) 
{
  sPut = sBuffer;
  sGet = sBuffer;
  buffer_length = 0;
}

enum Rotate
{
  left,
  right
};

void RotateTo(enum Rotate angle)
{
  switch (angle)
  {
  case left:
    OCR1A = 20000;
    _delay_ms(280);
    OCR1A = 24999;
    break;
  case right:
    OCR1A = 200;
    _delay_ms(280);
    OCR1A = 24999;
    break;
  default:
    break;
  }
}
void USART_Init(unsigned int baud)
{
  UBRRH = (unsigned char)(baud >> 8);
  UBRRL = (unsigned char)baud;
  UCSRB = (1 << RXCIE) | (1 << RXEN) | (1 << TXEN);
  UCSRC = (1 << URSEL) | (1 << USBS) | (1 << UCSZ0) | (1 << UCSZ1) | (1 << UPM1) | (1 << UPM0);
}

void uart_send(unsigned char data)
{
  while(!( UCSRA & (1 << UDRE)));
  UDR = data;
}

void str_uart_send(char *string)
{
  while (*string != '\0')
  {
    uart_send(*string);
    string++;
  }
}

char* getStringFromBuffer(void) {
  static char result[IO_SIZE];

  char *tmp = result;
  while(buffer_length) {
    char sym = SIOGet();
    *tmp = sym;
    tmp++;
  }
  *tmp = 0;

  return result;
}

void stateAutomatActionRecieveESP8266(char* responce) {
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPStart) {
    current_state = ESPInited;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPInited) {
    current_state = ESPModeSet;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPModeSet) {
    current_state = ESPDisconected;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPDisconected) {
    current_state = ESPConnected;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPConnected) {
    current_state = ESPConnectionStarted;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPConnectionStarted) {
    current_state = ESPDataStreamOpened;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPDataStreamOpened) {
    current_state = ESPDataSent;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPDataSent) {
    current_state = ESPDataRecieved;
    stateAutomatActionSendESP8266();
  }
  if(strcmp(responce, ResponceOK) == 0 && current_state == ESPDataRecieved) {
    current_state = ESPConnectionClosed;
    stateAutomatActionSendESP8266();
  }

  if(strcmp(responce, RightComand) == 0) {
    RotateTo(right);
  }
  if(strcmp(responce, LeftComand) == 0) {
    RotateTo(left);
  }
}

void stateAutomatActionSendESP8266() {
  if(current_state == ESPStart) {
    str_uart_send(StartComand);
  }
  if(current_state == ESPInited) {
    str_uart_send(SetModeComand);
  }
  if(current_state == ESPModeSet) {
    str_uart_send(DisconnectWiFiComand);
  }
  if(current_state == ESPDisconected) {
    str_uart_send(ConnectWiFiComand);
  }
  if(current_state == ESPConnected) {
    str_uart_send(ConnectionStartCommand);
  }
  if(current_state == ESPConnectionStarted) {
    str_uart_send(OpenDataStreamComand);
  }
  if(current_state == ESPDataStreamOpened) {
    str_uart_send(SendData)
  }
  if(current_state == ESPDataSent) {

  }
  if(current_state == ESPDataRecieved) {

  }
  if(current_state == ESPConnectionClosed) {

  }

  if(strcmp(action, RightComand) == 0) {
    RotateTo(right);
  }
  if(strcmp(action, LeftComand) == 0) {
    RotateTo(left);
  }
}

ISR(USART_RXC_vect)
{
  unsigned char NUM = UDR;
  unsigned char byte_receive = 1;

  if(NUM == '\r') {
    uart_send(NUM);
    char* tmp = getStringFromBuffer();
    stateAutomatActionRecieveESP8266(tmp);
    SIOClear();
    
    return;
  } 
  else if (NUM == 0xE1) {
    return;
  } 
  else {
    uart_send(NUM);
    SIOPut(NUM);
  }
}

int main(void)
{
  DDRD = 0b00100000;
  TCNT1 = 0;
  ICR1 = 24999;

  TCCR1A |= 1 << WGM11 | 1 << COM1A1;
  TCCR1B |= 1 << CS10 | 1 << CS10 | 1 << WGM12 | 1 << WGM13;

  OCR1A = 24999;
  _delay_ms(5000);
  USART_Init(51);
  SIOInit();
  sei();

  current_state = ESPStart;

  str_uart_send("Inited\n");
  while (1)
  {
    stateAutomatActionSendESP8266();
    _delay_ms(2000);
  }
}