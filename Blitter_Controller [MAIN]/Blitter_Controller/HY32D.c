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

unsigned char lData; // not redeclaring these variables increases performance a lot
unsigned char hData;

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
	//PORTB |= (1 << BL_PWM);
	PORTB |= (1 << WR);
	PORTE |= (1 << RD);
	PORTE |= (1 << DC);
	// Set IO to output
	DDRA = 0xFF; // D0 - D7
	DDRC = 0xFF; // D8 - D15
	_delay_ms(100);
	RST_HIGH;
	_delay_ms(5);
	RST_LOW;
	_delay_ms(15);
	RST_HIGH;
	_delay_ms(15);
	
	// Power supply setting (See page 71 of SSD1289 datasheet):
	// Set R07h at 0021h
	writeToRegister(0x07, 0x0021);
	// Set R00h at 0001h (turn on oscillator)
	writeToRegister(0x00, 0x0001);
	// Set R07h at 0023h
	writeToRegister(0x07, 0x0023);
	// Set R10h at 0000h Exit sleep mode
	writeToRegister(0x10, 0x0000);
	// wait 30ms
	_delay_ms(30);
	// Set R07h at 0033h. Which means grayscale level output. See page 36.
	writeToRegister(0x07, 0x0033);
	// Entry mode setting
	writeToRegister(0x11, 0x6830);
	// LCD driver AC setting
	writeToRegister(0x02, 0x0000);
	// Ram data write
	colorTest();
	// Display on
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
void writeData(unsigned short data){
	DC_HIGH;
	lData = (data & 0xFF);
	hData = ((data >> 8) & 0xFF);
	D0_D7 = lData; // Write data to GPIO lines (lines should by default be output)
	D8_D15 = hData;
	wrSignal();
}

void writeToRegister(unsigned short index, unsigned short data){
	CS_LOW;
	writeIndex(index);
	writeData(data);
	CS_HIGH;
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
	_delay_ms(100);
}

void fillScreen(unsigned short color){
	CS_LOW;
	writeIndex(0x22);
	for(unsigned long int it = 0; it < pixels; it++){
		writeData(color);
	}
	CS_HIGH;
}

void screenTest(void){
	CS_LOW;
	fillScreen(White);
	fillScreen(Black);
	fillScreen(Grey);
	fillScreen(Red);
	fillScreen(Yellow);
	fillScreen(Blue);
	fillScreen(Magenta);
	fillScreen(Green);
	fillScreen(Cyan);
	fillScreen(Light_Yellow);
	fillScreen(Light_Blue);
	colorTest();
	CS_HIGH;
}

void colorTest(void){
	CS_LOW;
	writeIndex(0x22);
	for(int it = 0; it < 7680; it++){
		writeData(White);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Black);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Grey);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Blue);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Light_Blue);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Red);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Magenta);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Green);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Cyan);
	}
	for(int it = 0; it < 7680; it++){
		writeData(Yellow);
	}
	CS_HIGH;
}

/*
	Usage: Reads and returns data on D0-D15.
	Prereq: Set CS_LOW first.
*/
unsigned short readDataLines(void) {
	unsigned short data;
	// Set IO to input
	setIOtoInput();
	RD_LOW;
	data = DATA_IN;
	data = DATA_IN; // Reading twice to make sure data is good (not sure if it's needed)
	RD_HIGH;
	// Set IO to output
	setIOtoOutput();
	return data;
}

unsigned short readLCDData(void){
	unsigned short data;
	DC_HIGH;
	WR_HIGH;
	data = readDataLines();
	transmitUART((char)data);
	return data;
}

void statusRead(void){ // reads SR register
	unsigned short data;
	CS_LOW;
	DC_LOW;
	data = readDataLines();// read data coming through IO lines
	uint8_t udata = ((data >> 8) & 0xFF);
	uint8_t ldata = (data & 0xFF);
	transmitUART(CR);
	transmitUART(D);
	transmitUART(A);
	transmitUART(T);
	transmitUART(A);
	transmitUART(_colon);
	transmitUART(_space);
	transmitUART('0');
	transmitUART(x);
	uint8_t udatauhex = ((udata >> 4) & 0x0F);
	uint8_t udatalhex = (udata & 0x0F); // xxxx xxxx & 0000 1111 => xxxx 1001
	transmitUART(toHex(udatauhex));
	transmitUART(toHex(udatalhex));
	uint8_t ldatauhex = ((ldata >> 4) & 0x0F);
	uint8_t ldatalhex = (ldata & 0x0F); // xxxx xxxx & 0000 1111 => xxxx 1001
	transmitUART(toHex(ldatauhex));
	transmitUART(toHex(ldatalhex));
	transmitUART(CR);
	CS_HIGH;
}