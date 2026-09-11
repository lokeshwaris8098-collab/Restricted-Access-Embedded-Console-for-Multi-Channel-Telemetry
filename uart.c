#include<lpc21xx.h>

void uart_init(void);
void uart_tx(unsigned char);
//void uart_integer(int);
//void uart_float(float);
unsigned char uart_rx(void);

int main()
{
  unsigned char rxbyte;
  uart_init();
  while(1)
  {
  rxbyte=uart_rx();
  uart_tx(rxbyte);
  }
}

void uart_init(void)
{
  PINSEL0|=0X05;
  U0LCR=0X83;
  U0DLL=97;
  U0DLM=0;
  U0LCR=0x03;
}

void uart_tx(unsigned char txbyte)
{
   while(((U0LSR>>5)&1)==0);
   U0THR=txbyte; 
} 
unsigned char uart_rx(void)
{ 
  while((U0LSR&1)==0);
  return U0RBR;
}

