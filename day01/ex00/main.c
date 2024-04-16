#include <avr/io.h>


#define CYCLE_PER_ITERATION 3 
#define CPU_FREQUENCE 8000000
#define SLEEP	0.5
#define ITERATIONS (uint32_t)((SLEEP * CPU_FREQUENCE) / CYCLE_PER_ITERATION)

int main()
{
	DDRB |= (1 << PB1); //definit PB1 comme output
	//int n = (0.5 * F_CPU) / 1000;
	while (1)
	{
		uint32_t i = 0;
		while (i < ITERATIONS)
			i++;
		PORTB ^= (1 << PB1); //allume ou eteint la led en definissant le bit concerne a 0(eteint) s'il est a 1(allume) et inversement
	}
	return 0;
}