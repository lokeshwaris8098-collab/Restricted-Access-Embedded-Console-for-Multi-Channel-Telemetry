#include<lpc21xx.h>
#include<string.h>
#include"keypad_i2c_define.h"
#include"spi.h"

// keypad_decleration

#define led1 1<<18
#define led2 1<<19
#define led3 1<<20
#define motor1 1<<21
#define motor2 1<<22



//unsigned char p[10]="";  // __attribute__((at(0x40000040)))="";
void uart0_config(void);
void uart0_tx(unsigned char);
void uart_integer(int);
void uart_float(float);
void uart_str(unsigned char* );
unsigned char uart_rx(void);

int main()
{
  int res,j,i,flag=0,ent=0;
  float f1,f2;
  unsigned char arr[7]="";

  i2c_init();
  lcd_init();
  Init_SPI0();
  uart0_config();
  
  lcd_command(0x01);
  lcd_command(0x80);
  lcd_str("Restricted Access");
  lcd_command(0xc0);
  lcd_str("Embedded Console");
  delay_ms(2000);

  lcd_command(0x01);
  lcd_command(0x80);
  lcd_str("For MultiChannel");
  lcd_command(0xc0);
  lcd_str("Telemetry");
  delay_ms(2000);



  i2c_page_write(0x50,0x00,"123456",6);
  i2c_seq_read(0x50,0x00,p,6);
  
  lcd_command(0x01);
  for(i=0;i<3;i++)
  {
    for(j=0;j<6;j++)
    {
	 // lcd_command(0x01);
	  lcd_command(0x80);
	  lcd_str("enter password");
      res=keypad_scan();
	  if(res>=0 && res<=9)
	  {
	  arr[j]=res+'0';
	  
	  lcd_command(0xc0+j);
	  //lcd_data(arr[j]);
	  lcd_data('*');
	  }
	  
    }
//	j=0;
	arr[6]='\0';
	

   
/*	lcd_command(0x01);  
	lcd_str(p);
	delay_ms(2000);*/
	/*
	lcd_command(0x01);
	lcd_command(0xc0)
	lcd_str(arr);
 	delay_ms(1000);*/

	if((strcmp((char *)p,(char *)arr))==0)
	{
	  lcd_command(0x01);
	  lcd_command(0x80);
	  lcd_str("Correct Password");
	  delay_ms(2000);
		break;
	}
	else
	{
	  

	  
	 lcd_command(0x01);
	 lcd_command(0x80);
	 lcd_str("Worng Password");
	 delay_ms(2000);

	 lcd_command(0x01);
	 lcd_command(0x80);
	 lcd_str("Try Again");
	 delay_ms(2000);
	
	 flag++;
   	 
	}
  }

  
  if(flag <2)
  {
  uart_str("at\r\n");
  //uart_str("\r\n");
  delay_ms(1000);

  uart_str("at+cmgf=1\r\n");
  //uart_str("\r\n");
  delay_ms(1000);

  uart_str("at+cmgs=\"+918610533189\"\r\n");
  //uart_str("\r\n");
  delay_ms(1000);

  uart_str("Restricted console Access granted");
  //uart_str("\r\n");
  delay_ms(1000);
  uart0_tx(26);
  delay_ms(4000);


  for(i=0;i<2;i++)
  {
  lcd_command(0x01);
  lcd_command(0x80);
  lcd_str("0:temp/1:volt");
  
  /*
  while(1)
  {
    f1=spi_read(0);
    lcd_command(0x01);
	lcd_command(0x80);
    lcd_float(f1);
	
	f2=spi_read(1);
	lcd_command(0xc0);
    lcd_float(f2);
	
  }*/
  
   ent=keypad_scan();
   lcd_command(0xc0);
   lcd_integer(ent);
   delay_ms(2000);
   if(ent == 0)
   {

   	 while(1)
	 {
     f1=spi_read(0);
	 lcd_command(0x01);
	 lcd_command(0x80);
	 lcd_str("temperature");
	 delay_ms(2000);
	 
	 //uart0_tx('\r');
	 //uart0_tx('\n');

	 //delay_ms(4000);


	 IODIR0|=led1|led2|led3|motor1|motor2;
	 IOSET0|=led1|led2|led3;
	 //IOCLR0=motor;
	 if(f1 < 10.0)
	 {
	   IOCLR0=led1;
	   delay_ms(1000);
	   lcd_command(0x01);
	   lcd_command(0x80);
	   lcd_str("temperature");
	   lcd_command(0xc0);
	   lcd_float(f1);
	   delay_ms(1000);
	   break;


	 }
	 else if(f1 < 20.0)
	 { 
	   IOCLR0=led1|led2;
	   delay_ms(1000);
	   lcd_command(0x01);
	   lcd_command(0x80);
	   lcd_str("temperature");
	   lcd_command(0xc0);
	   lcd_float(f1);
	   delay_ms(1000);
	   break;
	 }
	 else if(f1 < 30.0)
	 { 
	   IOCLR0=led1|led2|led3;
	   delay_ms(1000);
	   lcd_command(0x01);
	   lcd_command(0x80);
	   lcd_str("temperature");
	   lcd_command(0xc0);
	   lcd_float(f1);
	   delay_ms(1000);
	   break;
	 }
	 else
	 {
	   //lcd_command(0x01);
	   //lcd_command(0x80);
	   IOCLR0=led1|led2|led3;
	   IOSET0=motor1;
	   IOCLR0=motor2;
	   delay_ms(2000);
	   
	   
	   uart_str("at\r\n");
      //uart_str("\r\n");
       delay_ms(1000);

       uart_str("at+cmgf=1\r\n");
     //uart_str("\r\n");
       delay_ms(1000);

      uart_str("at+cmgs=\"+918610533189\"\r\n");
     //uart_str("\r\n");
       delay_ms(1000);
	   uart_str("temperature is max\r\n");
	   delay_ms(1000);
       uart0_tx(26);
       delay_ms(4000);
 	   //uart_str("\r\n");
	   
	   //uart_str("\n");
	 
	 


	  uart_str("at\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

     uart_str("at+cmgf=1\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

      uart_str("at+cmgs=\"+918610533189\"\r\n");
     //uart_str("\r\n");
      delay_ms(1000);
	  uart_str("temperature: ");
	  delay_ms(1000);
     uart0_tx(26);
     delay_ms(4000);
	 
	 //uart0_tx('\n');
	 //lcd_command(0xc0);
	 uart_str("at\r\n");
  //uart_str("\r\n");
  delay_ms(1000);

  uart_str("at+cmgf=1\r\n");
  //uart_str("\r\n");
  delay_ms(1000);

  uart_str("at+cmgs=\"+918610533189\"\r\n");
  //uart_str("\r\n");
  delay_ms(1000);
	 uart_float(f1);
	 delay_ms(1000);
     uart0_tx(26);
     delay_ms(4000);
	 break;
	 }
	 }
	 IOCLR0=motor1|motor2;
	  IOSET0=led1|led2|led3;
	 //uart_str("\r\n");
	 //delay_ms(4000);
  }
  else
  {
  f2=spi_read(1);
  //lcd_command(0x01);
  //lcd_command(0x80);
  uart_str("at\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

     uart_str("at+cmgf=1\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

      uart_str("at+cmgs=\"+918610533189\"\r\n");
     //uart_str("\r\n");
      delay_ms(1000);
  uart_str("operating volt:");
   delay_ms(1000);
     uart0_tx(26);
     delay_ms(4000);

  //uart0_tx('\n');
  //lcd_command(0xc0);
  uart_str("at\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

     uart_str("at+cmgf=1\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

      uart_str("at+cmgs=\"+918610533189\"\r\n");
     //uart_str("\r\n");
      delay_ms(1000);
  uart_float(f2/10);
   delay_ms(1000);
     uart0_tx(26);
     delay_ms(4000);
  uart_str("v\r\n");
  //uart_str("\r\n");
  //delay_ms(4000);
  /*
  if(f2 > 3.0)
  {
    //lcd_command(0x01);
	//lcd_command(0x80);
	uart_str("at\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

     uart_str("at+cmgf=1\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

      uart_str("at+cmgs=\"+918610533189\"\r\n");
     //uart_str("\r\n");
      delay_ms(1000);
    uart_str("voltage good\r\n");
	 delay_ms(1000);
     uart0_tx(26);
     delay_ms(4000);
	//uart_str("\r\n");
	//delay_ms(2000);
  }
  else
  {
    //lcd_command(0x01);
	//lcd_command(0x80);
		uart_str("at\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

     uart_str("at+cmgf=1\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

      uart_str("at+cmgs=\"+918610533189\"\r\n");
     //uart_str("\r\n");
      delay_ms(1000);
    uart_str("voltage low\r\n");
	 delay_ms(1000);
     uart0_tx(26);
     delay_ms(4000);
	//uart_str("\r\n");
//	uart0_tx('\n');
	//delay_ms(2000);
  }	*/
  
  //delay_ms(2000);
  }	
 }   
  lcd_command(0x01);
  lcd_str("successfull");
  delay_ms(4000);
  lcd_command(0x01);
  }
  else
  {
      lcd_command(0x01);
	  lcd_command(0x80);
	  lcd_str("access denied");
	  delay_ms(2000);

	    uart_str("at\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

     uart_str("at+cmgf=1\r\n");
     //uart_str("\r\n");
      delay_ms(1000);

      uart_str("at+cmgs=\"+918610533189\"\r\n");
     //uart_str("\r\n");
      delay_ms(1000);
  uart_str("unautherized person is trying to accessing");
   delay_ms(1000);
     uart0_tx(26);
     delay_ms(4000);
  }	
}

void uart0_config(void)
{
  PINSEL0|=0X05;
  U0LCR=0X83;
  U0DLL=97;
  U0DLM=0;
  U0LCR=0X03;
}

void uart0_tx(unsigned char txbytes)
{
  while(((U0LSR>>5)&1)==0);
  U0THR=txbytes;
}
void uart_integer(int n)
{
  signed char arr[5],i=0;
	if(n==0)
		uart0_tx('0');
	else
	{
		   if(n<0)
		   {
				 uart0_tx('-');
				 n=-n;
       }
	      while(n>0)
      	{
		     arr[i++]=n%10;
		     n=n/10;
        }
        for(--i;i>=0;i--)
   	      uart0_tx(arr[i]+48);
  }
}
void uart_float(float f)
{
  unsigned int number;
  number = f;
  uart_integer(number);
  uart0_tx('.');
  number=(f-number)*100;
  uart_integer(number);
}

void uart_str(unsigned char *p)
{
  while(*p)
  {
    uart0_tx(*p++);
  }
}

unsigned char uart_rx(void)
{ 
  while((U0LSR&1)==0);
  return U0RBR;
}




