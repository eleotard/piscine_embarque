#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();

void	set_ADC();
void	ft_ADC();

#define SPI_DDR DDRB
#define SS      PINB2 //Slave Select Line
#define MOSI    PINB3 //Master Out Slave In line
#define MISO    PINB4 //Master In Slave Out line
#define SCK     PINB5 //Shift Clock

#define RED 0
#define GREEN 1
#define BLUE 2

#define D6 0
#define D7 1
#define D8 2

void SPI_init()
{
    // set CS, MOSI and SCK to output, miso is an input
    SPI_DDR |= (1 << SS) | (1 << MOSI) | (1 << SCK);
    // enable SPI, set as master, and clock to fosc/128
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR1) | (1 << SPR0);
}

void SPI_masterTransmitByte(uint8_t data)
{
    // load data into data register
    SPDR = data;
    // Wait for transmission complete //polling
	//termine quand SPIF passe a 1
    while(!(SPSR & (1 << SPIF)));
}

void	send_colordata_to_LEDS(uint8_t leds_settings[3][3])
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(leds_settings[0][2]);
	SPI_masterTransmitByte(leds_settings[0][1]);
	SPI_masterTransmitByte(leds_settings[0][0]);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(leds_settings[1][2]);
	SPI_masterTransmitByte(leds_settings[1][1]);
	SPI_masterTransmitByte(leds_settings[1][0]);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(leds_settings[2][2]);
	SPI_masterTransmitByte(leds_settings[2][1]);
	SPI_masterTransmitByte(leds_settings[2][0]);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}

void	init_led_settings(uint8_t leds_settings[3][3])
{
	leds_settings[0][0] = 0;
	leds_settings[0][1] = 0;
	leds_settings[0][2] = 0;
	leds_settings[1][0] = 0;
	leds_settings[1][1] = 0;
	leds_settings[1][2] = 0;
	leds_settings[2][0] = 0;
	leds_settings[2][1] = 0;
	leds_settings[2][2] = 0;
}

int main()
{
	uart_init();
	SPI_init();
	set_ADC();
	DDRD &= ~(1 << PD2); //sw1
	DDRD &= ~(1 << PD4); //sw2
	uint8_t leds_settings[3][3];
	uint8_t cur_color = RED;
	uint8_t cur_led = D6;

	uint8_t sw1_is_pressed = 0;
	uint8_t sw2_is_pressed = 0;
	init_led_settings(leds_settings);
	send_colordata_to_LEDS(leds_settings);
	while (1)
	{
		ft_ADC();
		if (!(PIND & (1 << PD2)))
		{
			if (!sw1_is_pressed)
			{
				sw1_is_pressed = 1;
				cur_color++;
				if (cur_color == 3)
					cur_color = RED;
			}
		}
		else
			sw1_is_pressed = 0;
		if (!(PIND & (1 << PD4)))
		{
			if (!sw2_is_pressed)
			{
				sw2_is_pressed = 1;
				cur_led++;
				if (cur_led == 3)
					cur_led = D6;
			}
		}
		else
			sw2_is_pressed = 0;
		leds_settings[cur_led][cur_color] = ADCH;
		send_colordata_to_LEDS(leds_settings);
		_delay_ms(20);
	}
}