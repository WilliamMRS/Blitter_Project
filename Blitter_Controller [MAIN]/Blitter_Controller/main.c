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
#include "SRAM.h"

// Global variables
uint8_t remoteEcho = 0;
//unsigned char lData; // not redeclaring these variables increases performance a lot
//unsigned char hData;

void initTimers(){
	// CS02,01,00 controls prescaling. Set CS00 to 1, so it's direct clocked (fastest possible).
	// Set COM0A1 so PB4 port is overridden and outputs the counter interrupts as clocks. (see page 104)
	disable_counter_PB4;
	TIMSK0 = 0x00 | (1 << OCIE0A); // Enable interrupts for timer output compare match.
	//TIMSK2 = 0x00 | (1 << OCIE2A);
}

int main(void)
{	
	// Init IO:
	// PORT B to output
	DDRB |= (1 << RESET);
	DDRB |= (1 << CS);
	DDRB |= (1 << BL_PWM);
	DDRB |= (1 << WR_BLT_CLK); // set to input, so it's disabled for now.
	DDRB |= (1 << BLT_EN); // Setting BLT_EN to output;
	// PORT E to output
	DDRE |= (1 << SRAM_OE); // Port 4 to output (/SRAM OE)	(D16)
	DDRE |= (1 << SRAM_WE); // Port 5 to output (/SRAM WE)	(D17)
	DDRE |= (1 << RD); // Port 6 to output (RD)			(D18)
	DDRE |= (1 << DC); // Port 7 to output (DC)			(D19)
	DDRE |= (1 << LOAD); // Setting LOAD to output;
	// Set outlines
	BLT_EN_LOW; // Setting BLT_EN to low.
	// All are set to High (disabled). DC doesn't matter.
	CS_HIGH;
	WR_BLT_CLK_HIGH;
	RD_HIGH;
	DC_HIGH;
	LOAD_HIGH; // Setting LOAD to high (disabled)
	//PORTB |= (1 << BL_PWM); // Not enabled rn as it turns off display. Handle this pin later (Pwm control)
	
	// Set IO (D0-D15) to output
	DDRA = 0xFF; // D0 - D7
	DDRC = 0xFF; // D8 - D15
	
    initUART(); // initialize the UART
	initHY32D(); // initialize HY32D screen
	initTimers(); // init counters for blitting

	sei(); //Enable global interrupt
	systemCheck();
	startupMessage();
	
	writeSRAM(Magenta, Yellow, Light_Blue, 0);
	writeSRAM(Black, White, Black, pixels);
	//writeSRAM(Cyan, Blue, Red, pixels*2);
	
    while (1) 
    {
		/*TODO:
		
		System control:
		sjekk skjerm (forcibly les av SR)
		sjekk SRAM   (skriv til SRAM, les av verdien, hvis det er samme som ble skrevet (på samme adresse) we good.
		sjekk UART	(gjøres allerede)
		sjekk tellere (sett de til en startverdi, sett IO til input og les av)
		
		Use timers (using software to increase max value) to blit via PB4 and PE4. (uhhhh)
			
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
				Write text coming via UART to specific coordinates on screen: (command, start x, start y, color, data)
				
			Brukt USART til å kjøre kommandoer
				
			// kan lage funksjon som gjør at skjermen ikke hopper addresse når den har tegnet.
		
		- MÅ:  Bruke en ATmega169A sammen med ekstern SRAM og 5 stk 74-logikk tellere for 
			   å aksellerere grafikk på en 320x240 RGB LCD-skjerm.
		- MÅ:  UART for kommunikasjon til omverden. Grafikk-kommandoer og data kommer over UART.
		- MÅ:  Definere kommandoer over UART for å tegne grafikk og tekst.
		- BØR: System-kontroll ved start.
		- BØR: Lagring av grafikk i FLASH.
		
		- KAN: Lage et scriptingspråk med makroer som kan lagres i ekstern FLASH for enerering av grafikk.
		- KAN: Presse ytelsen i systemet.
		- KAN: Krever kanskje utvidet bruk av pekere som funksjonspekere
		- KAN: Krever kanskje innslag av assembly i høynivåkode. Selvstudium.
		- INFO:Utviklingen vil skje på egen maskinvareplattform. */
    }
}

ISR(TIMER0_COMP_vect){
	//transmitUART('w');
	//wrSignal();
	//rdSignalSRAM();
	/*
	memCounter++;
	if(memCounter >= memTarget){
		TCCR0A = 0x00;
		TCCR2A = 0x00;
		transmit8BitAsHex((memCounter & 0xFF));
		transmit8BitAsHex((memCounter >> 8) & 0xFF);
		CS_HIGH; // deselect LCD and SRAM
		BLT_EN_LOW; // counters disabled
		SRAMOutputDisable(); // disable SRAM OE, WE
		setIOtoOutput();
	}*/
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
		}else if(receivedByte == 'w'){
			writeSRAM(Red, Blue, Green, pixels);
		}else if(receivedByte == '1'){
			readSRAM(0);
		}else if(receivedByte == '2'){
			readSRAM(pixels);
		}else if(receivedByte == '3'){
			readSRAM(pixels*2);
		}else if(receivedByte == 'T'){
			screenTest();
		}else if(receivedByte == 'o'){
			presetCounters(1000);
		}
	}
	if (remoteEcho && ((receivedByte > 31) || (receivedByte == Bell) || (receivedByte == CR) || (receivedByte == LF) || (receivedByte == backspace))){
		transmitUART(receivedByte);
	}
}