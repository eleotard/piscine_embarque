#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/twi.h>

#define FREQ_SCL 100000 //100kHz <=> 100000 cycles/sec
#define PRESCALER 1

void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();
void	uart_debug(const char *s);


void	i2c_init()
{
	TWBR = ((F_CPU / FREQ_SCL) - 16) / (2 * PRESCALER);
}

void	i2c_read(void)
{
	//(1 << TWEA) sert a dire que le maitre doit envoyer un 
	//signal ACK apres chaque reception de data
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    if (TW_STATUS == TW_MR_DATA_ACK)
        uart_debug("Data byte has been recieved; ACK has been received\n");
    else
        uart_debug("ERROR: Data byte has not been recieved\n");
}

void	i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TW_STATUS == TW_MT_DATA_ACK) //0x28
		uart_debug("Data byte has been transmitted; ACK has been received\n");
	else if (TW_STATUS == TW_MT_SLA_ACK) //0x18
		uart_debug("SLA+W has been transmitted; ACK has been received\n");
	else if(TW_STATUS == TW_MR_SLA_ACK) //0x40
		uart_debug("SLA+R has been transmitted; ACK has been received\n");
	else
		uart_debug("ERROR - (data or +R +W)\n");
}

void	i2c_start()
{
	TWCR = 0;
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));
	if (TWSR == 0x08)
		uart_debug("A START condition has been transmitted\n");
	else if (TWSR == 0x10)
		uart_debug("A repeated START condition has been transmitted\n");
	else
		uart_debug("ERROR -(START condition)\n");
}

void	i2c_stop()
{
	TWCR = (1<<TWINT) | (1<<TWEN) | (1<<TWSTO);
}

int main()
{
	i2c_init();
	uart_init();

	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x06); 
	i2c_write(0xF7); //configure 03 comme output
	i2c_stop();
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x02);
	i2c_write(0xF7);
	i2c_stop();



	while (1);
	return 1;
}