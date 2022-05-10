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

// Prototypes
void initUART (void);
int transmitUART(char data);


// Global variables
char receivedByte;
uint8_t remoteEcho = 0;

#define CS 6 // PB6
#define BL_PWM 5 // PB5
#define WR 4 // PB4
#define RD 6 // PE6
#define DC 7 // PE7

#define RESET 0 // PB0
/*
		PORTC.DIR |= ( 1 << LED3); // Sets led to output
		PORTD.OUT &= ~(1 << LED1); // Sets led OUT to 0, turning LED ON.
*/
void LCDSetup(){
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
	DDRA = 0xFF;
	DDRC = 0xFF; 
}

// Use PORTA, PORTC to set bits.

void setIOtoOutput(void){
	DDRA = 0xFF;
	DDRC = 0xFF;
}
void setIOtoInput(void){
	DDRA = 0x00;
	DDRC = 0x00;
}

int main(void)
{	
	/*
	Set up interface mapping. PS3, PS2, PS1, PS0
	
	Configure for 8 bit operation. 8-bit 6800 parallel interface
	PS3: 1, PS2: 0, PS1: 1, PS0: 0
		CS is active LOW when using screen.
	Read command:				RW: 1, E: high-> low, DC: 0
	Read parameters or status:	RW: 1, E: high-> low, DC: 1
	Write command:				RW: 0, E: high-> low, DC 0
	Write command:				RW: 0, E: high-> low, DC 1
	*/	
	/*
	Configure for 16 bit operation
	*/
	
	// The index register, which only is accessed by having DC low and RW low decides which register is written data to.
	

	
	void writeIndex(unsigned short index){
		// DC LOW
		// RD High
		// IO Write
		// WRLOW
		// WRHIGH
	}
	
	unsigned short readStatus(void){
		// DC LOW
		// WR HIGH
		return 0;
	}
	
	void writeSignal(void){
		// WR LOW
		// WR HIGH
	}
	void readSignal(void){
		// RD LOW
		// RD HIGH
	}
	
    initUART(); // initialize the UART
	sei(); //Enable global interrupt
	
    while (1) 
    {
    /* 
		TODO:
			Snakk med skjermen
			Tegn noe til skjermen
			Lag et funksjonsbibliotek for skjermen
			
			Brukt USART til å kjøre kommandoer
			
			Få kontakt med flashminnet
			Tegn fra flashminnet til skjermen
	 */
    }
}

// Type S for system check:
void systemCheck(){
	transmitUART(CR);
	transmitUART(S);
	transmitUART(y);
	transmitUART(s);
	transmitUART(t);
	transmitUART(e);
	transmitUART(m);
	transmitUART(_space);
	transmitUART(c);
	transmitUART(h);
	transmitUART(e);
	transmitUART(c);
	transmitUART(k);
	transmitUART(_colon);
	transmitUART(CR);
	// Check and print results of various components on the blitter board:
}

ISR(USART0_RX_vect){
	// Read received data to variable. This also clears the interrupt flag. If data isn't read a new interrupt will immediately happen. See 19.7.3 datasheet.
	// When the receive complete interrupt enable (RXCIEn) in UCSRnB is set, the USART receive complete interrupt will be executed as long as the RXCn flag is set.
	receivedByte = UARTBuffer; // local temporary variable for received byte
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