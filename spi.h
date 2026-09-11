//#include<LPC21xx.h>
//#include"delay.h" 
#include"decleration.h"                               
																/* defines.h */
#ifndef __DEFINES_H__
#define __DEFINES_H__

#define SETBIT(WORD,BITPOS)         (WORD|=1<<BITPOS)
#define CLRBIT(WORD,BITPOS)         (WORD&=~(1<<BITPOS))
#define CPLBIT(WORD,BITPOS)         (WORD^=(1<<BITPOS))
#define WRITEBIT(WORD,BITPOS,BIT)   (BIT ? SETBIT(WORD,BITPOS): CLRBIT(WORD,BITPOS))
#define READBIT(WORD,BITPOS)        ((WORD>>BITPOS)&1)
/*
typedef unsigned int u32;
typedef signed int s32;
typedef unsigned short int u16;
typedef signed short int s16;
typedef unsigned char u8;
typedef signed char s8;
typedef float f32;
typedef double f64;*/

#endif

unsigned char SPI_0(unsigned char data);

/* spi_defines.h */
#ifndef __SPI_DEFINES_H__
#define __SPI_DEFINES_H__

#define CS 7

//defines for clock /baudrate

#define FOSC 12000000
#define CCLK (5*FOSC)
#define PCLK (CCLK/4)
#define SPCCR_VAL 60
#define SPI_RATE (unsigned char)(PCLK/SPCCR_VAL)

// SPCR Bits Setting 
#define Mode_0     0x00  // CPOL 0 CPHA 0
#define Mode_1     0x08  // CPOL 0 CPHA 1
#define Mode_2     0x10  // CPOL 1 CPHA 0
#define Mode_3     0x18  // CPOL 1 CPHA 1
#define MSTR_BIT   5     // SPI0 as Master 
#define LSBF_BIT   6     // default MSB first,if set LSB first
#define SPIE_BIT   7     //SPI Interrupt Enable Bit

// SPSR bits
#define SPIF_BIT   7    // Data Transfer Completion Flag

//SPINT bit
#define SPIINTF_BIT 0   //SPI Interrupt Flag Bit
#endif


#define lcd_d 0xff<<8
#define rs 1<<16
//#define rw 1<<18
#define e 1<<17

/*
void delay_milliseconds(u32 milliseconds)
{
	//T0CTCR=0X00;
	T0PR = 15000-1;
	T0TCR = 0X01;
	while(T0TC<milliseconds);
	T0TCR = 0x03;
	T0TCR = 0x00;
} */

 /*
void lcd_command(unsigned char d)
{
	IOPIN0 = d<<8;
	IOCLR0 = rs;
	//IOCLR0 = rw;
	IOSET0 =e;
	delay_ms(2);
	IOCLR0 =e;
}

void lcd_data(unsigned char d)
{
	IOPIN0 = d<<8;
	IOSET0 = rs;
	//IOCLR0 = rw;
	IOSET0 =e;
	delay_ms(2);
	IOCLR0 =e;
}

void lcd_init(void)
{
	IODIR0 |= lcd_d|rs|e;
	lcd_command(0x01);
	lcd_command(0x02);
	lcd_command(0x0c);
	lcd_command(0x38);
	lcd_command(0x80);
}

void lcd_str(unsigned char *s)
{
	while(*s)
		lcd_data(*s++);
}

void lcd_integer(int n)
{
	signed char arr[5],i=0;
	if(n==0)
		lcd_data('0');
	else
	{
		   if(n<0)
		   {
				 lcd_data('-');
				 n=-n;
       }
	      while(n>0)
      	{
		     arr[i++]=n%10;
		     n=n/10;
        }
        for(--i;i>=0;i--)
   	      lcd_data(arr[i]+48);
  }
}
void Float_display(float f_val)
{
  unsigned char number;
  number = f_val;
  lcd_integer(number);
  lcd_data('.');
  number=(f_val-number)*100;
  lcd_integer(number);	
} */


                        /*  file: mcp3204.c */
//#include "types.h"
//#include "spi.h"

float spi_read(unsigned char channelNo)
{
  unsigned char hByte,lByte;
  unsigned int adcVal=0;
   
  //select/activate chip 
  CLRBIT(IOPIN0,CS);
	//delay_ms(100);
	
  SPI_0(0x06);
  hByte = SPI_0(channelNo<<6);
  lByte = SPI_0(0x00);
	
	//de-select/de-activate chp
	SETBIT(IOPIN0,CS);
	//delay_ms(100);
  adcVal=((hByte&0x0f)<<8)|lByte;
  return ((adcVal*3.3)/4096)*10;
}
                     /* end of mcp3204.c */


                               /* spi.c */
//#include "types.h"
//#include "pin_function_defines.h"

// peripheral/hw spi
void Init_SPI0(void)
{
	
	PINSEL0 |=0X00001500;//P0.4 to P0.6 as SPI pins 
  S0SPCCR = SPI_RATE;             // cfg bit clock rate 
  S0SPCR  = (1<<MSTR_BIT|Mode_3); //spi module in master mode,
                                  //CPOL =1,CCPHA = 1. MSB first
	SETBIT(IOPIN0,CS);
	SETBIT(IODIR0,CS);
}

unsigned char SPI_0(unsigned char data)
{
   unsigned char stat;
   stat = S0SPSR;    //clear SPIF 
   S0SPDR = data;   // load spi tx reg
   while(READBIT(S0SPSR,SPIF_BIT)==0); // wait for transmission to complete
   return S0SPDR;    // read data from SPI data reg, place into buffer 
}
				/* end of spi.c */


                    /* main_hwspi_mcp3204_lcd_test.c */
//#include "types.h"
//#include "spi.h"
//#include "mcp3204.h"

/*
int main()
{
	f32 f1,f2;
	Init_SPI0(); //hw SPI initialisation
	lcd_init();	
	//str("MCP3204--ADC");
  while(1)
  {
    f1=Read_ADC_MCP3204(0);
		lcd_cmd(0x80);
		Float_display(f1);
		delay_ms(500);
	f2=Read_ADC_MCP3204(1);
		lcd_cmd(0xc0);
		Float_display(f2);
		delay_ms(500);

	}	
}
				*/



  