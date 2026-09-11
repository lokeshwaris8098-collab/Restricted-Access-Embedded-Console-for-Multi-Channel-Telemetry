#include"decleration.h"
#include"delay.h"

#define scl_pin 0x00000010
#define sda_pin 0x00000040

#define clk 60000000
#define pclk (clk/4)
#define i2c_speed 100000
#define load_val ((pclk/i2c_speed)/2)

#define AA_BIT 2
#define SI_BIT 3
#define STOP_BIT 4
#define START_BIT 5
#define I2C_ENABLE_BIT 6


//keypad
#define c0 (IOPIN1&(1<<23))
#define c1 (IOPIN1&(1<<24))
#define c2 (IOPIN1&(1<<25))
#define c3 (IOPIN1&(1<<26))

#define r0 (1<<27)
#define r1 (1<<28)
#define r2 (1<<29)
#define r3 (1<<30)

// lcd decleration

#define lcd_d 0xff<<8
#define rs 1<<16
#define e 1<<17

//SPI define micro

unsigned int keypad[4][4]={{1,2,3,4},{5,6,7,8},{9,0,11,12},{13,14,15,16}};

unsigned char p[10] __attribute__((at(0x40000040)))="";

unsigned int keypad_scan(void)
{
  unsigned char row_val=0,col_val=0;
   IODIR1=r0|r1|r2|r3;
   while(1)
   {
     IOCLR1=r0|r1|r2|r3;
	 IOSET1=c0|c1|c2|c3;
	 while((c0&&c1&&c2&&c3)==1);

	 //test row-0
	 IOCLR1=r0;
	 IOSET1=r1|r2|r3;
	 if((c0&&c1&&c2&&c3)==0)
	 {
	   row_val=0;
	   break;

	 }

	 //test row-1
	 IOCLR1=r1;
	 IOSET1=r0|r2|r3;
	 if((c0&&c1&&c2&&c3)==0)
	 {
	   row_val=1;
	   break;
	 }

	 //test row-2
	 IOCLR1=r2;
	 IOSET1=r0|r1|r3;
	 if((c0&&c1&&c2&&c3)==0)
	 {
	   row_val=2;
	   break;
	 }

	  //test row-3
	 IOCLR1=r3;
	 IOSET1=r0|r1|r2;
	 if((c0&&c1&&c2&&c3)==0)
	 {
	   row_val=3;
	   break;
	 }

   }//inner while
   if(c0==0)
   col_val=0;
   else if (c1==0)
   col_val=1;
   else if (c2==0)
   col_val=2;
   else{
   col_val=3;
   }
   delay_ms(200);
   while((c0&&c1&&c2&&c3)==0);
   return keypad[row_val][col_val];
 }


void i2c_init(void)

{

  PINSEL0|=scl_pin | sda_pin;
  I2SCLL=load_val;

  I2SCLH=load_val;

  I2CONSET=1<<I2C_ENABLE_BIT ; 

}	
void i2c_page_write(unsigned char slave_add,unsigned char buff_add,unsigned char* s,unsigned char nbyte)
{
  int i=0;
  i2c_start();
  i2c_write(slave_add<<1);
  i2c_write(buff_add);
  for(;i<nbyte;i++)
  {
    i2c_write(s[i]);
  }
  i2c_stop();
  delay_ms(10);
}

void i2c_seq_read(unsigned char slave_add,unsigned char rbuff_add,unsigned char* s,unsigned char nbyte)
{
  int i;
  i2c_start();
  i2c_write(slave_add<<1);
  i2c_write(rbuff_add);
  i2c_restart();
  i2c_write(slave_add<<1|1);
  for(i=0;i<nbyte-1;i++)
  {
    s[i]=i2c_masack();
  }
  s[i]=i2c_ack();
  i2c_stop();
}

void i2c_start(void)
{
  I2CONSET=1<<START_BIT;
  
  while(((I2CONSET>>SI_BIT)&1)==0);
  
  I2CONCLR=1<<START_BIT;  
}

void i2c_restart(void)
{
  I2CONSET=1<<START_BIT;

  I2CONCLR=1<<SI_BIT;

  while(((I2CONSET>>SI_BIT)&1)==0);

  I2CONCLR=1<<START_BIT;
}

void i2c_write(unsigned char dat)
{
  I2DAT=dat;

  I2CONCLR=1<<SI_BIT;

  while(((I2CONSET>>SI_BIT)&1)==0);
}

void i2c_stop(void)
{
  I2CONSET=1<<STOP_BIT;

  I2CONCLR=1<<SI_BIT;
}

unsigned char i2c_ack(void)
{
  I2CONSET=0X00;

  I2CONCLR=1<<SI_BIT;

  while(((I2CONSET>>SI_BIT)&1)==0);

  return I2DAT;
}

unsigned char i2c_masack(void)
{
  I2CONSET=0X04;

  I2CONCLR=1<<SI_BIT;

  while(((I2CONSET>>SI_BIT)&1)==0);

  I2CONCLR=0X04;

  return I2DAT;
}




void lcd_init(void)
{
  IODIR0|=lcd_d|rs|e;
  lcd_command(0x01);
  lcd_command(0x02);
  lcd_command(0x0c);
  lcd_command(0x38);

}

void lcd_command(unsigned char cmd)
{
  IOCLR0=lcd_d;
  IOSET0=cmd<<8;
  IOCLR0=rs;
  IOSET0=e;
  delay_ms(2);
  IOCLR0=e;
}

void lcd_data(unsigned char dat)
{
  IOCLR0=lcd_d;
  IOSET0=dat<<8;
  IOSET0=rs;
  IOSET0=e;
  delay_ms(2);
  IOCLR0=e;
}
void lcd_str(unsigned char *p)
{
  while(*p)
  {
    lcd_data(*p++);
  }
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
void lcd_float(float f_val)
{
  unsigned int number;
  number = f_val;
  lcd_integer(number);
  lcd_data('.');
  number=(f_val-number)*100;
  lcd_integer(number);	
}




  
