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

#define White 0xFFFF
#define Black 0x0000
#define Grey 0xF7DE
#define Blue 0x001F
#define Light_Blue 0x051F
#define Red 0xF800
#define Magenta 0xF81F
#define Green 0x07E0
#define Cyan 0x7FFF
#define Yellow 0xFFE0

// macros
#define RD_HIGH PORTE |= (1 << RD)
#define RD_LOW PORTE &= ~(1 << RD)
#define WR_HIGH PORTB |= (1 << WR)
#define WR_LOW PORTB &= ~(1 << WR)
#define DC_HIGH PORTE |= (1 << DC)
#define DC_LOW PORTE &= ~(1 << DC)
#define CS_HIGH PORTB |= (1 << CS)
#define CS_LOW PORTB &= ~(1 << CS)
#define RST_HIGH PORTB |= (1 << RESET)
#define RST_LOW PORTB &= ~(1 << RESET)
#define D0_D7 PORTA // used only for output
#define D8_D15 PORTC // used only for output
#define DATA_IN (PINC << 8) | (PINA & 0xFF) // macro for reading incoming data

char intToAscii(char number);

#endif /* MISC_H_ */