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
#include "macros.h"
#include "UART.h"
#include "HY32D.h"

// Prototypes
void initUART (void);
int transmitUART(char data);


// Global variables
char receivedByte;
uint8_t remoteEcho = 0;

int main(void)
{	
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