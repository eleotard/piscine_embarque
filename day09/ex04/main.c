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
void	uart_printhexa(uint8_t data);
void	uart_printbinary(uint8_t value);

const uint8_t digits[10] = {
    // abcdefg
    0b00111111, // 0
    0b00000110, // 1
    0b01011011, // 2
    0b01001111, // 3
    0b01100110, // 4
    0b01101101, // 5
    0b01111101, // 6
    0b00000111, // 7
    0b01111111, // 8
    0b01101111  // 9
};

void	i2c_init()
{
	TWBR = ((F_CPU / FREQ_SCL) - 16) / (2 * PRESCALER);
}

uint8_t	i2c_read(void)
{
	//(1 << TWEA) sert a dire que le maitre doit envoyer un 
	//signal ACK apres chaque reception de data
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
    while (!(TWCR & (1 << TWINT)));
    if (TW_STATUS == TW_MR_DATA_ACK)
        uart_debug("Data byte has been recieved; ACK has been received\n");
    else
        uart_debug("ERROR: Data byte has not been recieved\n");
	return TWDR;
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

uint8_t 	i2c_read_reg(uint8_t reg_addr)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(reg_addr); 
	i2c_stop();

	i2c_start();
	i2c_write((0x20 << 1) + 1);
	i2c_read();
	i2c_stop();
	uart_printbinary(TWDR);
	return (TWDR);
}

void	set_output_port_0(uint8_t conf)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x02);
	i2c_write(conf);
	i2c_stop();
}

void	set_conf_port_0(uint8_t conf)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x06);
	i2c_write(conf);
	i2c_stop();
}

void	set_output_port_1(uint8_t conf)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x03);
	i2c_write(conf);
	i2c_stop();
}

void	set_conf_port_1(uint8_t conf)
{
	i2c_start();
	i2c_write(0x20 << 1);
	i2c_write(0x07);
	i2c_write(conf);
	i2c_stop();
}

int main()
{
	//uint8_t count = 0;
	i2c_init();
	uart_init();

	set_conf_port_1(0b00000000);
	set_conf_port_0(0b00111111);
	//ca va tellement vite le clignotement qu'on dirait que les chiffres persistent
	//multiplexing
	//persistance de la vision
	while (1)
	{
		set_output_port_1(digits[4]);
		set_output_port_0(0b10111111); //taffiches 4
		set_output_port_0(0b11111111); //tu l'eteint
		//_delay_ms(500);
		set_output_port_1(digits[2]);
		set_output_port_0(0b01111111); //taffiches 2
		set_output_port_0(0b11111111); //tu l'eteint
		//_delay_ms(500);
	}
	return 1;
}