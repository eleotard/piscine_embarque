#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>


void	uart_init(void);
void	uart_tx(unsigned char c);
void	uart_printstr(const char *s);
char	uart_rx(void);
void	uart_putnbr(int64_t n);
void	uart_newline();


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
	//Adjust for 8bits results
	ADMUX |= (1 << ADLAR);
	//Les bits MUX3 à MUX0 contrôlent la sélection des six lignes d'entrée à connecter au circuit de numérisation.
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


int main()
{
	char	data_str[3];
	
	uart_init();
	set_ADC();
	while (1)
	{
		ft_bzero(data_str, 3);
		//commence une conversion
		ADCSRA |= (1 << ADSC);
		//att la fin de la conversion
		while ((ADCSRA & (1 << ADSC)));
		int_to_hex_str(ADCH, data_str);
		uart_printstr(data_str);
		uart_newline();
		_delay_ms(20);
	}
	return 1;
}