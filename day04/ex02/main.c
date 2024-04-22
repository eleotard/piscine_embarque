#include <avr/io.h>
#include <util/twi.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h> 


#define UART_BAUDRATE (uint32_t)115200
#define FREQ_SCL 100000 //100kHz <=> 100000 cycles/sec
#define PRESCALER 1
#define SLA	0x38
#define BUFF_SIZE 11
#define DEBUG 0


void	int_to_hex_str(uint8_t data, char *data_str)
{
	const char hex_digits[] = "0123456789ABCDEF";

	data_str[0] = hex_digits[(data >> 4) & 0x0F];
	data_str[1] = hex_digits[data & 0x0F];
	data_str[2] = '\0';
}

void	ft_bzero(char *buff, uint8_t size)
{
	for(uint8_t i = 0; i < size; i++)
		buff[i] = '\0';
}

void	reset_srh_and_st(uint32_t *s_rh, uint32_t *s_t)
{
	*s_rh = 0;
	*s_t = 0;
}

void	uart_init()
{
	uint32_t ubrr;

	ubrr = (F_CPU / (16 * UART_BAUDRATE));
	UBRR0L = (uint8_t)(ubrr & 0xFF);
	UBRR0H = (uint8_t)(ubrr >> 8);
    UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	UCSR0B |= (1 << RXCIE0);
}

void	uart_tx(unsigned char c)
{
	while(!(UCSR0A & (1 << UDRE0)))
	{}
	UDR0 = c;
}

void uart_printstr(const char* s)
{
	while (*s)
	{
		if (*s == '\n')
			uart_tx('\r');
		uart_tx(*s);
		s++;
	}
}

void	uart_debug(const char* s)
{
	if (DEBUG)
	{
		while (*s)
		{
			if (*s == '\n')
				uart_tx('\r');
			uart_tx(*s);
			s++;
		}
	}
}

void	i2c_init()
{
	TWBR = ((F_CPU / FREQ_SCL) - 16) / (2 * PRESCALER);
}

//check calibre et busy 
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

void	print_hex_value(uint8_t *data)
{
	char	data_str[7][3];

	for (uint8_t i = 0; i < 7; i++)
	{
		int_to_hex_str(data[i], data_str[i]);
		uart_printstr(data_str[i]);
		uart_tx(' ');
	}
	uart_tx('\b');
}

void	create_srh_and_st(uint32_t *s_rh, uint32_t *s_t, uint32_t data, uint8_t i)
{
	if (i == 1)
		*s_rh |= data;
	else if (i == 2)
	{
		*s_rh = *s_rh << 8;
		*s_rh |=  data; 
	}
	else if (i == 3)
	{
		*s_rh = *s_rh << 4;
		*s_rh = *s_rh | (data >> 4);
		*s_t = (data & 0x0F);
	}
	else if (i == 4 || i == 5)
	{
		*s_t = *s_t << 8;
		*s_t |= data;
	}
}

// void create_srh_and_st(uint32_t *s_rh, uint32_t *s_t, uint8_t *data)
// {
// 	*s_rh = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] & 0xF0);
// 	*s_t = (((uint32_t)data[2] & 0x0F) << 16) | ((uint32_t)data[3] << 8) | (uint32_t)data[4];
// }

float	calculate_humidity(uint32_t s_rh)
{
	float humidity = 0;

	humidity = ((float)s_rh / 1048576.0) * 100.0;
	return humidity;
}

float	calculate_temperature(uint32_t s_t)
{
	float temperature = 0;

	temperature = ((float)s_t / 1048576.0) * 200.0 - 50.0;
	return temperature;
}

void	print_temp_and_humidity(uint32_t s_rh, uint32_t s_t)
{
	char	temperature[BUFF_SIZE];
	char	humidity[BUFF_SIZE];

	dtostrf(calculate_temperature(s_t), 4, 1, temperature);
	dtostrf(calculate_humidity(s_rh), 3, 0, humidity);
	uart_printstr("Temperature: "); //0.0 et humidity cest 0
	uart_printstr(temperature);
	uart_printstr("°C, Humidity: ");
	uart_printstr(humidity);
	uart_printstr("%\n");
}

int main()
{
	uint8_t		data[7];
	uint32_t	s_rh = 0;
	uint32_t	s_t = 0;

	uart_init();
	_delay_ms(100);
	i2c_init();
	while(1)
	{
		i2c_start();
		i2c_write(0x38 << 1);
		_delay_ms(10);
		i2c_write(0xAC);
		i2c_write(0x33);
		i2c_write(0x00);
		_delay_ms(80);
		//i2c_stop();
		i2c_start();
		i2c_write((0x38 << 1) | 1);
		for (uint8_t i = 0; i < 7; i++)
		{
			i2c_read();
			data[i] = TWDR;
			create_srh_and_st(&s_rh, &s_t, (uint32_t)data[i], i);
		}
		// create_srh_and_st(&s_rh, &s_t, &data[1]);
		i2c_stop();
		// print_hex_value(data);
		print_temp_and_humidity(s_rh, s_t);
		reset_srh_and_st(&s_rh, &s_t);
		_delay_ms(2000);
	}
	return 1;
}