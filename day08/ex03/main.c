#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

/*SPI == Serial Peripheral Interface,
data transfer protocol for microcontrollers*/

#define SPI_DDR DDRB
#define SS      PINB2 //Slave Select Line
#define MOSI    PINB3 //Master Out Slave In line
#define MISO    PINB4 //Master In Slave Out line
#define SCK     PINB5 //Shift Clock

typedef struct s_led {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} t_led;

typedef struct rgb_s {
	t_led d6;
	t_led d7;
	t_led d8; 
} t_rgb;

void	set_ADC()
{
	//Select the high reference voltage: use of AVCC as a reference
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	//Adjust for 8bits results
	ADMUX |= (1 << ADLAR);
	//on selectionne la ligne d entree du potentiometre rv1, la A0
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX &= ~(1 << MUX3);
	//clock prescaler
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS0); 
	//activer les operations ADC
	ADCSRA |= (1 << ADEN);
}

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

void	send_colordata_to_D6(uint8_t r, uint8_t g, uint8_t b)
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(b);
	SPI_masterTransmitByte(g);
	SPI_masterTransmitByte(r);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}

void	send_colordata_to_D7(uint8_t r, uint8_t g, uint8_t b)
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(b);
	SPI_masterTransmitByte(g);
	SPI_masterTransmitByte(r);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}

void	send_colordata_to_D8(uint8_t r, uint8_t g, uint8_t b)
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(b);
	SPI_masterTransmitByte(g);
	SPI_masterTransmitByte(r);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}


void	set_average(uint8_t nb)
{
	if (nb >= (33 * 255 / 100) && (nb < (66 * 255 / 100)))
	{
		send_colordata_to_D6(255, 255, 255);
	}
	else if ((nb > (66 * 255 / 100)) && (nb <= (66 * 255 / 100)))
		PORTB = (1 << PB0) | (1 << PB1);
	else if ((nb > (50 * 255 / 100)) && (nb <= (75 * 255 / 100)))
		PORTB = (1 << PB0) | (1 << PB1) | (1 << PB2);
	else
		PORTB = (1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4);
}

void	ft_ADC()
{
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
}

void	send_colordata(t_rgb *leds)
{
	for (uint8_t i = 0; i < 4; i++)
		SPI_masterTransmitByte(0);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(leds->d6.b);
	SPI_masterTransmitByte(leds->d6.g);
	SPI_masterTransmitByte(leds->d6.r);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(leds->d7.b);
	SPI_masterTransmitByte(leds->d7.g);
	SPI_masterTransmitByte(leds->d7.r);
	SPI_masterTransmitByte(255);
	SPI_masterTransmitByte(leds->d8.b);
	SPI_masterTransmitByte(leds->d8.g);
	SPI_masterTransmitByte(leds->d8.r);
	for (uint8_t j = 0; j < 4; j++)
		SPI_masterTransmitByte(255);
}

int main()
{
	SPI_init();
	t_rgb leds;

	leds = (t_rgb){0};
	while (1)
	{
		ft_ADC();
		set_average(ADCH);

		_delay_ms(83);
		send_colordata_to_D6(255, 255, 255);
		_delay_ms(250);
		send_colordata_to_D6(0, 0, 0);
		_delay_ms(83);
		send_colordata_to_D7(255, 255, 255);
		_delay_ms(250);
		send_colordata_to_D7(0, 0, 0);
		_delay_ms(84);
		send_colordata_to_D8(255, 255, 255);
		_delay_ms(250);
		send_colordata_to_D8(0, 0, 0);
	}
}