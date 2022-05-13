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
/*
#ifndef PORTE
// Microchip studio skal være vanskelig
#define PORTE _SFR_IO8(0x0E)
#endif
*/

#include "stdint.h"
#include <avr/io.h>
#include <util/delay.h>
#include "HY32D.h"
#include "UART.h"

// Blue, green, red
#define White 0xFFFF
#define Black 0x0000
#define Grey 0xF7DE
#define Red 0x001F
#define Yellow 0x051F
#define Blue 0xF800
#define Magenta 0xF81F
#define Green 0x07E0
#define Cyan 0xFFE0
#define Light_Blue 0xFCE7
#define Light_Yellow 0x7FFF

// special characters
#define Bell 0x07
#define backspace 0x08
#define CR 0x0D
#define LF 0x0A

// Control pins

// PORT E
#define LOAD 3
#define SRAM_OE 4
#define SRAM_WE 5
#define RD 6
#define DC 7

// PORT B
#define RESET 0 // Same as BLT_RST aka /SCLR
#define WR_BLT_CLK 4
#define BL_PWM 5
#define CS 6
#define BLT_EN 7

// Macros PORTB
#define WR_BLT_CLK_HIGH PORTB |= (1 << WR_BLT_CLK)
#define WR_BLT_CLK_LOW PORTB &= ~(1 << WR_BLT_CLK)
#define CS_HIGH PORTB |= (1 << CS)
#define CS_LOW PORTB &= ~(1 << CS)
#define RESET_HIGH PORTB |= (1 << RESET)
#define RESET_LOW PORTB &= ~(1 << RESET)
#define BLT_EN_HIGH PORTB |= (1 << BLT_EN)
#define BLT_EN_LOW PORTB &= ~(1 << BLT_EN)
// Macros PORTE
#define RD_HIGH PORTE |= (1 << RD)
#define RD_LOW PORTE &= ~(1 << RD)
#define DC_HIGH PORTE |= (1 << DC)
#define DC_LOW PORTE &= ~(1 << DC)
#define LOAD_HIGH PORTE |= (1 << LOAD)
#define LOAD_LOW PORTE &= ~(1 << LOAD)
#define SRAM_OE_HIGH PORTE |= (1 << SRAM_OE)
#define SRAM_OE_LOW PORTE &= ~(1 << SRAM_OE)
#define SRAM_WE_HIGH PORTE |= (1 << SRAM_WE)
#define SRAM_WE_LOW PORTE &= ~(1 << SRAM_WE)

#define enable_counter_PB4 TCCR0A = 0x00 | (0 << FOC0A) | (0 << WGM00) | (0 << COM0A1) | (1 << COM0A0) | (0 << WGM01) | (0 << CS02) | (0 << CS01) | (1 << CS00)
#define enable_counter_withoutpin_PB4 TCCR0A = 0x00 | (0 << FOC0A) | (0 << WGM00) | (0 << COM0A1) | (0 << COM0A0) | (0 << WGM01) | (1 << CS02) | (0 << CS01) | (1 << CS00)
#define disable_counter_PB4 TCCR0A = 0x00 | (0 << FOC0A) | (0 << WGM00) | (0 << COM0A1) | (0 << COM0A0) | (0 << WGM01) | (0 << CS02) | (0 << CS01) | (0 << CS00)

#define D0_D7 PORTA // used only for output
#define D8_D15 PORTC // used only for output
#define DATA_IN (PINC << 8) | (PINA & 0xFF) // macro for reading incoming data

#endif /* MISC_H_ */