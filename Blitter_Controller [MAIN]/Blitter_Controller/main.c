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
#include "UART.h"
#include "HY32D.h"
#include "UARTCommands.h"
#include "misc.h"

// Global variables
uint8_t remoteEcho = 0;

	// CS SRAM low
	// CS LCD high
	// Reset all counters (so they become 0).
	// BLT_EN to enable counters. Send clk to increment counters.
	// Write to SRAM by loading data to output lines (same as when writing to screen) then sending WR signal

void writeSRAM(){
	// Reset counters
	// BLT_EN enable counters
	
	// Set tellere til rett addresse før man skriver til SRAM:
		// Sett SRAM og LCD CS_PINS til HIGH (altså ingen er valgt)
		// preset address
			// Bruk datalinjene fra mikrokontrolleren (sett alle til 0)
			// BLT_RST (to reset counter values is also an option
	
	// BLT_EN disabled
	// CS SRAM low (select sram)
	
	// Loop 
		// Load data to output lines
		// WriteEnable to SRAM (this increments internal address by 1)
}

void blitFromSRAM(void){
	// Preset counters to 0.
	// Sett BLT_EN enabled (enable counters)
	// Sett CS_LOW (enabling both SRAM and Screen)
	// Send blitSignal (this sends write signal to screen, and increments counters by 1)
}

void blitSignal(void){
	WR_BLT_CLK_HIGH;
	WR_BLT_CLK_LOW;
}

void setExtraBlitIOToOutput(void) {
	// This can probably be done at startup/initialization
	DDRE |= (1 << PE4); // Port 4 to output (/SRAM OE)	(D16)
	DDRE |= (1 << PE5); // Port 5 to output (/SRAM WE)	(D17)
	DDRE |= (1 << PE6); // Port 6 to output (RD)			(D18)
	DDRE |= (1 << PE7); // Port 7 to output (DC)			(D19)
	_delay_ms(100);
}

void presetCountersToZero(void){
	CS_HIGH; // Deselect LCD and SRAM
	setIOtoOutput(); // Set all lines to output
	setExtraBlitIOToOutput();
	// Give all outlines a value of 0
	D0_D7 = 0x00;
	D8_D15 = 0x00;
	PORTE &= ~(1 << PE4);
	PORTE &= ~(1 << PE5);
	PORTE &= ~(1 << PE6);
	PORTE &= ~(1 << PE7);

	RESET_HIGH; // Set CLR(BLT_RST)(PB0)(RESET) to HIGH
	PORTE &= ~(1 << PE3); // Set LOAD to LOW  (PE3)
	blitSignal();// Send blitsignal (aka a clk)
}

// Send BLT_CLK x antall ganger for å komme deg til den addressen du ønsker i SRAM'en.

int main(void)
{	
	// Startup sequence
	
	// Init counters:
	DDRE |= (1 << PE3); // Setting LOAD to output;
	PORTE |= (1 << PE3); // Setting LOAD to high (disabled)
	
	// Init IO and LCD:
	// Setting CS, DC, RD, WR to output.
	DDRB |= (1 << CS);
	DDRB |= (1 << BL_PWM);
	DDRB |= (1 << WR_BLT_CLK);
	DDRE |= (1 << RD);
	DDRE |= (1 << DC);
	// All are set to High (disabled). DC doesn't matter.
	PORTB |= (1 << CS);
	//PORTB |= (1 << BL_PWM);
	PORTB |= (1 << WR_BLT_CLK);
	PORTE |= (1 << RD);
	PORTE |= (1 << DC);
	// Set IO to output
	DDRA = 0xFF; // D0 - D7
	DDRC = 0xFF; // D8 - D15
	
    initUART(); // initialize the UART
	initHY32D(); // initialize HY32D screen
	sei(); //Enable global interrupt
	systemCheck();
	startupMessage();
    while (1) 
    {
		//_delay_ms(1000);
		//statusRead();
		/*TODO:
		
		Blitting:
			Function to preset counters to 0 or other desired value.
			Function to write to SRAM.
			Function to blit from SRAM using the counters.
		
		Today:
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
		Tonight:
			Flash:
			
			SRAM:
			
			System control:
				sjekk skjerm
				sjekk SRAM
				sjekk UART
				sjekk tellere
				
			Brukt USART til å kjøre kommandoer
			
			Få kontakt med flashminnet
				Tegn fra SRAM til skjermen v.h.a tellerne.
				
			// kan lage funksjon som gjør at skjermen ikke hopper addresse når den har tegnet.
		
		- MÅ:  Bruke en ATmega169A sammen med ekstern SRAM og 5 stk 74-logikk tellere for 
			   å aksellerere grafikk på en 320x240 RGB LCD-skjerm.
		- MÅ:  UART for kommunikasjon til omverden. Grafikk-kommandoer og data kommer over UART.
		- MÅ:  Definere kommandoer over UART for å tegne grafikk og tekst.
		- BØR: Lagring av grafikk i FLASH.
		- BØR: System-kontroll ved start.
		
		- KAN: Lage et scriptingspråk med makroer som kan lagres i ekstern FLASH for enerering av grafikk.
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
		}else if(receivedByte == 's'){
			systemCheck();
		}else if(receivedByte == 'l'){
			lcdStatusRead();
		}else if(receivedByte == 'b'){
			fillScreen(Blue);
		}else if(receivedByte == 'r'){
			//drawImage(image);
		}else if(receivedByte == 'T'){
			screenTest();
		}
	// If echo is on, and the ASCII character is higher than 31, or Bell, Carriage Return, Line Feed or backspace, then echo the character. Other are filtered as to not get strange behavior from Putty.
	}
	if (remoteEcho && ((receivedByte > 31) || (receivedByte == Bell) || (receivedByte == CR) || (receivedByte == LF) || (receivedByte == backspace))){
		transmitUART(receivedByte);
	}
}