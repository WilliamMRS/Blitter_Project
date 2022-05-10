/*
 * HY32D.c
 *
 * Created: 09/05/2022 17:27:45
 *  Author: William M. R. Schmidt
 */ 

#include <avr/io.h>
#include "ASCII.h"
#include "UART.h"
#include "HY32D.h"

void initHY32D(){
	// Setting CS, DC, RD, WR to output.
	DDRB |= (1 << CS);
	DDRB |= (1 << BL_PWM);
	DDRB |= (1 << WR);
	DDRE |= (1 << RD);
	DDRE |= (1 << DC);
	// All are set to High (disabled). DC doesn't matter.
	PORTB |= (1 << CS);
	PORTB |= (1 << BL_PWM);
	PORTB |= (1 << WR);
	PORTE |= (1 << RD);
	PORTE |= (1 << DC);
	// Set IO to output
	DDRA = 0xFF; // D0 - D7
	DDRC = 0xFF; // D8 - D15
}

void wrSignal(void){
	PORTB &= ~(1 << WR);// WR LOW
	PORTB |= (1 << WR);// WR HIGH
}

void rdSignal(void){
	PORTE &= ~(1 << RD); // RD LOW
	PORTE |= (1 << RD); // RD HIGH
}

void setIOtoOutput(void){
	DDRA = 0xFF;
	DDRC = 0xFF;
}
void setIOtoInput(void){
	DDRA = 0x00;
	DDRC = 0x00;
}