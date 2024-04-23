#include "rush00.h"

void	i2c_init(void)
{
	TWBR = ((F_CPU / FREQ_SCL) - 16) / (2 * PRESCALER);
}

void	i2c_read(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
	i2c_wait();
	if (TWSR == 0x50)
		uart_printstr("Data byte has been recieved; ACK has been received\n");
	else
		uart_printstr("ERROR: Data byte has not been recieved\n");
}

uint8_t	i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait();
	if (TWSR == 0x28) //data has been transmitted,ACK returned
		return (uart_debug("Data byte has been transmitted; ACK has been received\n"),
			TWSR);
	else if (TWSR == 0x18)
		return (uart_debug("SLA+W has been transmitted; ACK has been received\n"),
			TWSR);
	else if (TWSR == 0x40)
		return (uart_debug("SLA+R has been transmitted; ACK has been received\n"),
			TWSR);
	else if (TWSR == 0x58)
		return (uart_debug("data byte has been transmitted; NACK has been received\n"),
			TWSR);
	else if (TWSR == 0x60)
		return (uart_debug("SLA+W has been received; ACK has been returned\n"),
			TWSR);
	else if (TWSR == 0xA8)
		return (uart_debug("SLA+R has been received; ACK has been returned\n"),
			TWSR);
	else if (TWSR == 0x30)
		return (uart_debug("data transmitted, NACK received\n"),
			TWSR);
	char buff[3];
	int_to_hex_str(TWSR, buff);
	uart_debug("value of TWSR:");
	uart_debug(buff);
	uart_debug("!\n");
	return (uart_debug("ERROR - (data or +R +W)\n"), TWSR);
}

void i2c_wait()
{
	while (!(TWCR & (1 << TWINT)))
		;
}

uint8_t	i2c_start(uint8_t read, uint8_t address)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	i2c_wait();
	// uart_printstr("Start...\r\n");
	if (TW_STATUS != TW_START) //check status code == start
	{
		uart_debug("ERROR START");
		return (TW_STATUS);
	}
	// read address of slave into register
	TWDR = (address << 1) + read;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait();
	if ((TW_STATUS != TW_MT_SLA_ACK && !read) || (TW_STATUS != TW_MR_SLA_ACK
			&& read))
	{
		uart_debug("ERROR SLAVE ACK");
		return (TW_STATUS);
	}
	return (0);
}

void	i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}
