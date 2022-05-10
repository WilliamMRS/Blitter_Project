/*
 * misc.h
 *
 * Created: 11.05.2022 01:14:07
 *  Author: William M. R. Schmidt
 */ 


#ifndef MISC_H_
#define MISC_H_

#ifndef F_CPU
#define F_CPU 7372800UL
#endif

// macros
#define RD_HIGH PORTE |= (1 << RD)
#define RD_LOW PORTE &= ~(1 << RD)
#define WR_HIGH PORTB |= (1 << WR)
#define WR_LOW PORTB &= ~(1 << WR)
#define DC_HIGH PORTE |= (1 << DC)
#define DC_LOW PORTE &= ~(1 << DC)
#define CS_HIGH DDRB |= (1 << CS)
#define CS_LOW DDRB &= ~(1 << CS)
#define D0_D7 PORTA // used only for output
#define D8_D15 PORTC // used only for output
#define DATA_IN (PINC << 8) | (PINA & 0xFF) // macro for reading incoming data

char intToAscii(char number);

#endif /* MISC_H_ */