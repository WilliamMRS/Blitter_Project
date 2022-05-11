/*
 * Blitter_Controller.c
 *
 * Created: 05/04/2022 17:48:09
 * Author : William M. R. Schmidt
 *
 *	ATmega169PA
 *  ISP - JTAGICE3
 *
 */ 

#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "ASCII.h"
#include "UART.h"
#include "HY32D.h"
#include "UARTCommands.h"
#include "misc.h"

// Global variables
uint8_t remoteEcho = 0;

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

unsigned short statusRead(void){ // reads SR register
	unsigned short data;
	CS_LOW;
	DC_LOW;
	data = readDataLines();// read data coming through IO lines
	char ldata = data & 0xFF;
	char udata = (data >> 8) & 0xFF;
	ldata = intToAscii(ldata);
	transmitUART(ldata);
	udata = intToAscii(udata);
	transmitUART(udata);
	transmitUART(CR);
	CS_HIGH;
	return 0;
}

unsigned short readLCDData(void){
	unsigned short data;
	DC_HIGH;
	WR_HIGH;
	data = readDataLines();
	transmitUART((char)data);
	return data;
}

int main(void)
{	
	// Startup sequence
    initUART(); // initialize the UART
	initHY32D(); // initialize HY32D screen
	sei(); //Enable global interrupt
	startupMessage();
    while (1) 
    {
		//_delay_ms(1000);
		//statusRead();
		/*TODO:
		Snakk med skjermen
		Tegn noe til skjermen 
		Lag et funksjonsbibliotek for skjermen
			
		Brukt USART til å kjøre kommandoer
			
		Få kontakt med flashminnet
		Tegn fra flashminnet til skjermen*/
    }
}

ISR(USART0_RX_vect){
	// Read received data to variable. This also clears the interrupt flag. If data isn't read a new interrupt will immediately happen. See 19.7.3 datasheet.
	// When the receive complete interrupt enable (RXCIEn) in UCSRnB is set, the USART receive complete interrupt will be executed as long as the RXCn flag is set.
	unsigned char receivedByte = UARTBuffer; // local temporary variable for received byte
	if (receivedByte){
		if (receivedByte == 45) { // that's the '-' sign.
			remoteEcho = ~remoteEcho;
		}else if(receivedByte == S){
			systemCheck();
		}
	// If echo is on, and the ASCII character is higher than 31, or Bell, Carriage Return, Line Feed or backspace, then echo the character. Other are filtered as to not get strange behavior from Putty.
	}
	if (remoteEcho && ((receivedByte > 31) || (receivedByte == Bell) || (receivedByte == CR) || (receivedByte == LF) || (receivedByte == backspace))){
		if(UCSR0A & (1 << UDRE0)){ // check if byte is 1
			UARTBuffer = receivedByte;
		}
	}
}