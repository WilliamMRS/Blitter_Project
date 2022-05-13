/*
 * UART.c
 *
 * Created: 09/05/2022 17:29:41
 *  Author: William M. R. Schmidt
 */

#include <avr/io.h>
#include "UART.h"
#include "HY32D.h"
#include "makroer.h"

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

uint8_t puttyCompatibleCheck(uint8_t byte){
	if((byte > 31) || (byte == Bell) || (byte == CR) || (byte == LF) || (byte == backspace)){
		return byte;
		}else{
		return (uint8_t)64; // this is a kinda derpy error message. Should change this to something like 'ERROR'
	}
}

int transmitUART (char data){
	short timeoutLimit = 1000; // after 1000 tries, just skip it and try next char. This is about 136 microseconds of time at 7.37 Mhz
	short timeout = 0;
	while(1){
		if(UCSR0A & (1 << UDRE0)){
			UARTBuffer = puttyCompatibleCheck(data);
			return 1;
			}else{
			timeout++;
			if(timeout > timeoutLimit){
				return 0;
			}
		}
	}
}

// four bit to Hex
uint8_t toHex(uint8_t number) {
	uint8_t num = '0'+number;
	if(num < 58){
		return num;
	}
	else if(num == 58){
		return 'A';
	}
	else if(num == 59){
		return 'B';
	}
	else if(num == 60){
		return 'C';
	}
	else if(num == 61){
		return 'D';
	}
	else if(num == 62){
		return 'E';
	}
	else if(num == 63){
		return 'F';
		}else{
		return 0;
	}
}

void transmit8BitAsHex(uint8_t data){
	transmitUART('0');
	transmitUART('x');
	uint8_t uhex = ((data >> 4) & 0x0F);
	uint8_t lhex = (data & 0x0F); // xxxx xxxx & 0000 1111 => xxxx 1001
	transmitUART(toHex(uhex));
	transmitUART(toHex(lhex));
	transmitUART(' ');
}