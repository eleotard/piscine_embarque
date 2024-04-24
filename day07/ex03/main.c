#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();

void	set_ADC()
{
	//Select the high reference voltage: use of internal 1.1V
	ADMUX |= (1 << REFS0);
	ADMUX |= (1 << REFS1);
	//Adjust for 10bits results
	ADMUX &= ~(1 << ADLAR);
	//clock prescaler
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS0);
	//Selecting the ADC8 channel by writing the MUX3 
		//->enable the temperature sensor
	ADMUX &= ~(1 << MUX0);
	ADMUX &= ~(1 << MUX1);
	ADMUX &= ~(1 << MUX2);
	ADMUX |= (1 << MUX3);
	//activer les operations ADC
	ADCSRA |= (1 << ADEN);
}

void	ft_ADC()
{
	uint16_t voltage = 0;
	int16_t temperature = 0;

	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
	voltage = ADC;
	temperature = (voltage * 0.98) - 283;

	uart_printstr("Voltage: ");
	uart_putnbr(voltage);
	uart_newline();

	uart_printstr("Temp: ");
	uart_putnbr(temperature);
	uart_printstr("°C\n");
}

int main()
{
	uart_init();
	set_ADC();
	while (1)
	{
		_delay_ms(20);
		ft_ADC();
		uart_newline();
	}
	return 1;
}