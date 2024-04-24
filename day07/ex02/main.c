#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();

#define RV1 0
#define LDR 1 //Light Dependent Resistor
#define NTC 2 //Negative Temperature Coefficient

void	int_to_hex_str(uint8_t data, char *data_str)
{
	const char hex_digits[] = "0123456789ABCDEF";

	data_str[0] = hex_digits[(data >> 4) & 0x0F];
	data_str[1] = hex_digits[data & 0x0F];
	data_str[2] = '\0';
}

void	ft_bzero(char *data_str, uint8_t size)
{
	uint8_t i = 0;
	while (++i < size)
		data_str[i] = 0;
}

void	set_ADC()
{
	//Select the high reference voltage: use of AVCC as a reference
	ADMUX |= (1 << REFS0);
	ADMUX &= ~(1 << REFS1);
	//Adjust for 10bits results
	ADMUX &= ~(1 << ADLAR);
	//clock prescaler
	ADCSRA |= (1 << ADPS2);
	ADCSRA |= (1 << ADPS1);
	ADCSRA |= (1 << ADPS0); 
	//activer les operations ADC
	ADCSRA |= (1 << ADEN);
}

void	set_MUX(uint8_t input)
{
	/*Les bits MUX3 à MUX0 contrôlent la sélection des six lignes d'entrée
	à connecter au circuit de numérisation.*/

	if (input == RV1) //A0
	{
		ADMUX &= ~(1 << MUX0);
		ADMUX &= ~(1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
	}
	else if (input == LDR) //A1
	{
		ADMUX |= (1 << MUX0);
		ADMUX &= ~(1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
	}
	else if (input == NTC) //A2
	{
		ADMUX &= ~(1 << MUX0);
		ADMUX |= (1 << MUX1);
		ADMUX &= ~(1 << MUX2);
		ADMUX &= ~(1 << MUX3);
	}
}

void	ft_ADC(uint8_t input)
{
	uint16_t rec = 0;
	set_MUX(input);
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
	rec = ADC;
	uart_putnbr(rec);
	if (input != NTC)
		uart_printstr(", ");
}

int main()
{
	uart_init();
	set_ADC();
	while (1)
	{
		ft_ADC(RV1);
		ft_ADC(LDR);
		ft_ADC(NTC);
		uart_newline();
		_delay_ms(20);
	}
	return 1;
}