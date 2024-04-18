#include <avr/io.h>
#include <avr/interrupt.h>


#define LED_FREQ 50
#define PRESCALE 256
#define true 1
#define false 0

typedef int bool;
volatile bool up = true;

volatile int count = 0;
//volatile uint8_t count_back = 0;

//sert a gerer la frequence de la led
void set_timer_1()
{
	TCCR1A = 0;
	TCCR1B = 0;
	//mode 14-ICR1
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << WGM13);

	// prediv 1024
	TCCR1B |= (1 << CS10);
	TCCR1B |= (1 << CS12);

	//oc1a
	TCCR1A |= (1 << COM1A1);

	ICR1 = 100;
	//OCR1A = 0;
}

void set_timer_0()
{
	//mode 7-OCRA
	TCCR0A = 0;
	TCCR0B = 0;
	TCCR0A |= (1 << WGM00);
	TCCR0A |= (1 << WGM01);
	TCCR0B |= (1 << WGM02);

	// prediv 1024
	TCCR0B |= (1 << CS00);
	TCCR0B |= (1 << CS02);

	OCR0A = 15625 / 200; //=78
}

ISR (TIMER0_COMPA_vect)
{
	if (up == true)
		count++;
	else 
		count--;
	if (count == 100)
		up = false;
	if (count == 0)
		up = true;
	OCR1A = count;
}

int main()
{
	DDRB |=  (1 << PB0);
	DDRB |= (1 << PB1);

	set_timer_1();
	set_timer_0();
	TIMSK0 |= (1 << OCIE0A); //active interruption pour timer0
	sei();
	while(1){}

}