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

#include "stdint.h"

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
#define LOAD PE3
#define SRAM_OE PE4
#define SRAM_WE PE5
#define RD PE6
#define DC PE7

#define RESET PB0
#define WR_BLT_CLK PB4
#define BL_PWM PB5
#define CS PB6
#define BLT_EN PB7

// macros
#define RD_HIGH PORTE |= (1 << RD)
#define RD_LOW PORTE &= ~(1 << RD)
#define WR_BLT_CLK_HIGH PORTB |= (1 << WR_BLT_CLK)
#define WR_BLT_CLK_LOW PORTB &= ~(1 << WR_BLT_CLK)
#define DC_HIGH PORTE |= (1 << DC)
#define DC_LOW PORTE &= ~(1 << DC)
#define CS_HIGH PORTB |= (1 << CS)
#define CS_LOW PORTB &= ~(1 << CS)
#define RESET_HIGH PORTB |= (1 << RESET)
#define RESET_LOW PORTB &= ~(1 << RESET)
#define LOAD_HIGH PORTE |= (1 << LOAD)
#define LOAD_LOW PORTE &= ~(1 << LOAD)
#define BLT_EN_HIGH PORTB |= (1 << BLT_EN)
#define BLT_EN_LOW PORTB &= ~(1 << BLT_EN)
#define SRAM_OE_HIGH PORTE |= (1 << SRAM_OE)
#define SRAM_OE_LOW PORTE &= ~(1 << SRAM_OE)
#define SRAM_WE_HIGH PORTE |= (1 << SRAM_WE)
#define SRAM_WE_LOW PORTE &= ~(1 << SRAM_WE)

#define D0_D7 PORTA // used only for output
#define D8_D15 PORTC // used only for output
#define DATA_IN (PINC << 8) | (PINA & 0xFF) // macro for reading incoming data

uint8_t toHex(uint8_t number);

#endif /* MISC_H_ */