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


unsigned char sBuffer[IO_SIZE];
unsigned char *sPut;
unsigned char *sGet;
int buffer_length;

void SIOInit(void)
{
    sPut = sBuffer;
    sGet = sBuffer;
    buffer_length = 0;
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

void Rotate_By_Angle(enum Rotate angle)
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

void doAction(char* action) 
{
  if(strcmp(action, RightComand) == 0) {
    Rotate_By_Angle(right);
  }
  if(strcmp(action, LeftComand) == 0) {
    Rotate_By_Angle(left);
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

ISR(USART_RXC_vect)
{
  unsigned char NUM = UDR;
  unsigned char byte_receive = 1;

  if(NUM == '\r') {
    uart_send(NUM);
    char* tmp = getStringFromBuffer();
    doAction(tmp);
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

  str_uart_send("Inited\n");
  while (1)
  {
    
    _delay_ms(1000);
  //  str_uart_send("AT");
    // Rotate_By_Angle(right);
    // _delay_ms(1000);
    // Rotate_By_Angle(left);
    // _delay_ms(1000);
  }
}