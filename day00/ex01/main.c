#include <avr/io.h>


//PORTX: envoie des donnes sur la carte
//PINX: recoit des donnees de la carte

int main()
{
	DDRB |= (1 << PB0); //definit la broche 0 du portB comme output
	PORTB |= 1; //change l'etat a 1 de cette broche
	
}

//la machine s eteint quand elle arrive a la fin du prog
//ensuite quand la machine s eteint elle se rallume
//refait le programme etc
