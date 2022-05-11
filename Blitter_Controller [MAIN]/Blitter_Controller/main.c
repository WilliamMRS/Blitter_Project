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
#include "stdint.h"
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
		
		LCD:
			Set coordinate before drawing (set memory addresses) (to 0,0 at start)
			Function for drawing lines
			Function for displaying text
		UART:
			Circular FIFO buffer that can take commands like fillScreen -red.
			Be able to send commands with information like x, y coordinates, color etc.
		UART Control:	
			Draw line via UART command line: (command, color, start x, start y, end x, end y)
			Draw rectangles: 
			Write text coming via UART to specific coordinates on screen: (command, start x, start y, color, data)
		System control:
			sjekk skjerm
			sjekk SRAM
			sjekk UART
			sjekk tellere
			
		Brukt USART til å kjøre kommandoer
			
		Få kontakt med flashminnet
		Tegn fra flashminnet til skjermen v.h.a tellerne.
		
		- MÅ: Bruke en ATmega169A sammen med eksterm SRAM og 5 stk 74-logikk tellere for å
		aksellerere grafikk på en 320x240 RGB LCD-skjerm.
		- MÅ: UART for kommunikasjon til omverden. Grafikk-kommandoer og data kommer over UART.
		- MÅ: Definere kommandoer over UART for å tegne grafikk og tekst.
		- BØR: Lagring av grafikk i FLASH.
		- BØR: System-kontroll ved start.
		- KAN: Lage et scriptingspråk med makroer som kan lagres i ekstern FLASH for enerering av
		grafikk.
		- KAN: Presse ytelsen i systemet.
		- KAN: Krever kanskje utvidet bruk av pekere som funksjonspekere
		- KAN: Krever kanskje innslag av assembly i høynivåkode. Selvstudium.
		- INFO:Utviklingen vil skje på egen maskinvareplattform.
		*/
    }
}

ISR(USART0_RX_vect){
	// Read received data to variable. This also clears the interrupt flag. If data isn't read a new interrupt will immediately happen. See 19.7.3 datasheet.
	// When the receive complete interrupt enable (RXCIEn) in UCSRnB is set, the USART receive complete interrupt will be executed as long as the RXCn flag is set.
	uint8_t receivedByte = UARTBuffer; // local temporary variable for received byte
	if (receivedByte){
		if (receivedByte == 45) { // that's the '-' sign.
			remoteEcho = ~remoteEcho;
		}else if(receivedByte == s){
			systemCheck();
		}else if(receivedByte == l){
			statusRead();
		}else if(receivedByte == b){
			fillScreen(Blue);
		}else if(receivedByte == r){
			fillScreen(Red);
		}else if(receivedByte == T){
			screenTest();
		}
	// If echo is on, and the ASCII character is higher than 31, or Bell, Carriage Return, Line Feed or backspace, then echo the character. Other are filtered as to not get strange behavior from Putty.
	}
	if (remoteEcho && ((receivedByte > 31) || (receivedByte == Bell) || (receivedByte == CR) || (receivedByte == LF) || (receivedByte == backspace))){
		transmitUART(receivedByte);
	}
}