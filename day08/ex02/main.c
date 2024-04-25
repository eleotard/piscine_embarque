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

// typedef struct s_led {
// 	uint8_t r;
// 	uint8_t g;
// 	uint8_t b;
// } t_led;

// typedef struct rgb_s {
// 	t_led d6;
// 	t_led d7;
// 	t_led d8; 
// } t_rgb;

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


int main()
{
	SPI_init();

	while (1)
	{
		_delay_ms(83);
		send_colordata_to_D6(127, 0, 255);
		_delay_ms(250);
		send_colordata_to_D6(0, 0, 0);
		_delay_ms(83);
		send_colordata_to_D7(127, 0, 255);
		_delay_ms(250);
		send_colordata_to_D7(0, 0, 0);
		_delay_ms(84);
		send_colordata_to_D8(127, 0, 255);
		_delay_ms(250);
		send_colordata_to_D8(0, 0, 0);
	}
}


// void	send_colordata(t_rgb *leds)
// {
// 	for (uint8_t i = 0; i < 4; i++)
// 		SPI_masterTransmitByte(0);
// 	SPI_masterTransmitByte(255);
// 	SPI_masterTransmitByte(leds->d6.b);
// 	SPI_masterTransmitByte(leds->d6.g);
// 	SPI_masterTransmitByte(leds->d6.r);
// 	SPI_masterTransmitByte(255);
// 	SPI_masterTransmitByte(leds->d7.b);
// 	SPI_masterTransmitByte(leds->d7.g);
// 	SPI_masterTransmitByte(leds->d7.r);
// 	SPI_masterTransmitByte(255);
// 	SPI_masterTransmitByte(leds->d8.b);
// 	SPI_masterTransmitByte(leds->d8.g);
// 	SPI_masterTransmitByte(leds->d8.r);
// 	for (uint8_t j = 0; j < 4; j++)
// 		SPI_masterTransmitByte(255);
// }

// void	setLeds(uint8_t leds_nb, t_rgb *leds, uint8_t r, uint8_t g, uint8_t b)
// {
// 	switch (led_nb) {
// 		case D6:
// 			leds->d6.r = r;
// 			leds->d6.g = g;
// 			leds->d6.b = b;
// 			break;
// 		case D7:
// 			leds->d7.r = r;
// 			leds->d7.g = g;
// 			leds->d7.b = b;
// 			break;
// 		case D8:
// 			leds->d8.r = r;
// 			leds->d8.g = g;
// 			leds->d8.b = b;
// 			break;
// 	}
// }