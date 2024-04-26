#include <avr/io.h>

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

void	ft_ADC()
{
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
}