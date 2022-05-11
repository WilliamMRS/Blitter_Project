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
#include "misc.h"
#include <util/delay.h>

// This isn't ready, see datasheet on how to complete start up sequence.
void initHY32D(void){
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
	CS_LOW;
	// Power supply setting (See page 71 of SSD1289 datasheet):
	// Set R07h at 0021h
	writeIndex(0x07);
	writeData(0x21, 0x00);
	// Set R00h at 0001h
	writeIndex(0x00);
	writeData(0x01, 0x00);
	// Set R07h at 0023h
	writeIndex(0x07);
	writeData(0x23, 0x00);
	// Set R10h at 0000h Exit sleep mode
	writeIndex(0x10);
	writeData(0x00, 0x00);
	// wait 30ms
	_delay_ms(50);
	// Set R07h at 0033h
	writeIndex(0x07);
	writeData(0x33, 0x00);
	// Entry mode setting
	writeIndex(0x11);
	writeData(0x30, 0x68);
	// LCD driver AC setting
	writeIndex(0x02);
	writeData(0x00, 0x00);
	// Ram data write
	writeIndex(0x22);
	writeData(0x00, 0x00);
	// Display on
	CS_HIGH;
}

/*
	Usage: Writes to the index register. The index register points to where the next written data should go.
	Prereq: Set CS_LOW first.
*/
void writeIndex(unsigned short index){
	DC_LOW; // DC LOW
	RD_HIGH; // RD High (for good measure? can be omitted as RD should always be high)
	D0_D7 = index; // IO Write
	wrSignal();
}

/*
	Usage: Writes to the index register. The index register points to where the next written data should go.
	Prereq: Set CS_LOW first.
*/
void writeData(unsigned char lData, unsigned char hData){
	DC_HIGH;
	D0_D7 = lData; // Write data to GPIO lines (lines should by default be output)
	D8_D15 = hData;
	wrSignal();
}

void wrSignal(void){
	WR_LOW;
	WR_HIGH;
}

void rdSignal(void){
	RD_LOW;
	RD_HIGH;
}

void setIOtoOutput(void){
	DDRA = 0xFF;
	DDRC = 0xFF;
}
void setIOtoInput(void){
	DDRA = 0x00;
	DDRC = 0x00;
}