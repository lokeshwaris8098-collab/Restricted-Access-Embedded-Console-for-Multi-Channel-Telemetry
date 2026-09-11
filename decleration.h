
// keypad_decleration
unsigned int keypad_scan(void);

void lcd_init(void);
void lcd_command(unsigned char);
void lcd_data(unsigned char);
void lcd_str(unsigned char*);
void lcd_integer(int);
void Float_display(float );

void i2c_init(void);
void i2c_page_write(unsigned char,unsigned char,unsigned char*,unsigned char);
void i2c_seq_read(unsigned char,unsigned char,unsigned char *,unsigned char);
void i2c_write(unsigned char);
void i2c_start(void);
void i2c_restart(void);
void i2c_stop(void);
unsigned char i2c_ack(void);
unsigned char i2c_masack(void);
/*
float spi_read(unsigned char);
void Init_SPI0(void);
unsigned char SPI0(unsigned char);
  */
