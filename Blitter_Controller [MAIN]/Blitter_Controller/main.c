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

#include "macros.h"
#define F_CPU 7372800UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define uart_bps 9600L  // max 57600

#define UARTRxNotEmpty (UCSR0A & (1 << RXC0))    // Filter out the Rx buffer empty bit from the UART control register
#define UARTBuffer UDR0
#define txBufferFree ( UCSR0A & (1 << UDRE0))    // Filter out the Tx buffer free bit from the UART control register
#define Bell 0x07
#define backspace 0x08
#define CR 0x0D
#define LF 0x0A

#define BUFFER_SIZE 8 // 8 bytes buffer size

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
		transmitUART(89);
		transmitUART(69);
		transmitUART(83);
		transmitUART(CR);
		_delay_ms(1000);
    }
}

int transmitUART (char data){
	short timeoutLimit = 1000;
	short timeout = 0;
	while(1){
		if(UCSR0A & (1 << UDRE0)){
			UARTBuffer = data;
			return 1;
		}else{
			timeout++;
			if(timeout > timeoutLimit){
				return 0;
			}
		}
	}
}

ISR(USART0_RX_vect){
	// Read received data to variable. This also clears the interrupt flag. If data isn't read a new interrupt will immediately happen. See 19.7.3 datasheet.
	// When the receive complete interrupt enable (RXCIEn) in UCSRnB is set, the USART receive complete interrupt will be executed as long as the RXCn flag is set.
	receivedByte = UARTBuffer; // local temporary variable for received byte
	if (receivedByte){
		if (receivedByte == 45) { // that's the '-' sign.
			remoteEcho = ~remoteEcho;
		}
	// If echo is on, and the ASCII character is higher than 31, or Bell, Carriage Return, Line Feed or backspace, then echo the character. Other are filtered as to not get strange behavior from Putty.
	}
	if (remoteEcho && ((receivedByte > 31) || (receivedByte == Bell) || (receivedByte == CR) || (receivedByte == LF) || (receivedByte == backspace))){
		if(UCSR0A & (1 << UDRE0)){ // check if byte is 1
			UARTBuffer = receivedByte;
		}
	}
}

void initUART (void){
	// calculating the baud rate clock division registers at compile time and setting the baud rate registers properly at run time.
	const uint8_t baudRateReg_low = (uint8_t) ((( F_CPU / ( 16 * uart_bps )) - 1 ) & 0xFF);
	const uint8_t baudRateReg_high = (uint8_t) ((( F_CPU / ( 16 * uart_bps )) - 1 ) >> 8);
	UBRR0L = baudRateReg_low;
	UBRR0H = baudRateReg_high;
	
	// Alternative way of calculating the baud rate register, with casting
	//	const uint8_t UBRR_reg_val = (uint8_t) ((sysClk / 16.0 * (float) uart_bps) - 1.0);

	// Setting up the USART control registers. All bits are included for sake of overview. Compiler will optimize.
	UCSR0A = 0x00 | (0 << RXC0) | (0 << TXC0) | (0 << UDRE0) | (0 << FE0) | (0 << DOR0) | (0 << UPE0) | (0 << U2X0) | (0 << MPCM0);  // NOT running 2x mode
	UCSR0B = 0x00 | (1 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (1 << RXEN0) | (1 << TXEN0) | (0 << UCSZ02);  // Enable Rx and Tx setting the ports mux correctly
	UCSR0C = 0x00 | (0 << UMSEL0) | (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00); // Selecting 8-bit character for the USART
}