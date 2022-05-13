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

void loadDataToOutputLines(unsigned short data){
	lData = (data & 0xFF);
	hData = ((data >> 8) & 0xFF);
	D0_D7 = lData; // Write data to GPIO lines (lines should by default be output)
	D8_D15 = hData;
}

// It looks like a write signal to the CLK ruins something ..
void readSRAM(uint32_t memStart){
	SRAMOutputDisable(); // disable SRAM OE, WE
	presetCounters(memStart); // set counters to 0
	
	//BLT_RST (PB0), BLT_LD, BLT_EN should all be HIGH
	//Then do BLT_CLK for it to count
	BLT_EN_HIGH; // counters enabled
	RESET_HIGH; // for counter BLT_RST
	LOAD_HIGH; // for counter BLT_LD
	CS_LOW; // Select screen and SRAM
	
	writeIndex(0x22); // ensure writing to screenbuffer
	DC_HIGH; // So it can write to screen!
	setIOtoInput(); // Set D0-D15 to input so it doesn't interfere with SRAM to Screen lines
	
	for(unsigned long int i = 0; i < (pixels); i++){
		rdSignalSRAM(); // SRAM OE goes LOW-HIGH reading the SRAM values on the address specified by the counters to the screen.
		// PE4 flip fast as f
		wrSignal(); // counters increment by one and screen updates. BLT_EN and WR
		// PB4 flip fast as f
	}
	
	CS_HIGH; // deselect LCD and SRAM
	BLT_EN_LOW; // counters disabled
	SRAMOutputDisable(); // disable SRAM OE, WE
	setIOtoOutput();
}

void writeSRAM(uint16_t color1, uint16_t color2, uint16_t color3, uint32_t memStart){
	SRAMOutputDisable(); // Disable SRAM
	presetCounters(memStart); // Set counters to your desired value (up to 2^20, or about 1 million). Also sets IO to output.
	//setIOtoOutput();
	BLT_EN_HIGH; // counters enabled
	LOAD_HIGH; // blt LOAD
	RESET_HIGH; // for counters
	
	CS_LOW; // Select LCD and SRAM
	SRAM_OE_HIGH; // Set output enable to disabled.
	writeIndex(0x22); // Set display to write to video ram to avoid it writing to any other register.
	DC_HIGH;
	// Loops with data to transfer. This is hardcoded for now

	for(unsigned long int i = 0; i < (pixels/3); i++){
		loadDataToOutputLines(color1);
		wrSignalSRAM(); // WriteEnable to SRAM
		wrSignal(); // CLK sends write signal to display, and increment counters by 1.
	}
	for(unsigned long int i = 0; i < (pixels/3); i++){
		loadDataToOutputLines(color2);
		wrSignalSRAM(); // WriteEnable to SRAM
		wrSignal(); // counters increment by one.
	}
	for(unsigned long int i = 0; i < (pixels/3); i++){
		loadDataToOutputLines(color3);
		wrSignalSRAM(); // WriteEnable to SRAM
		wrSignal(); // counters increment by one.
	}
	
	CS_HIGH; // deselect LCD and SRAM
	BLT_EN_LOW; // counters disabled
	SRAMOutputDisable(); // disable SRAM
}

int main(void)
{	
	// Init IO:
	// PORT B to output
	DDRB |= (1 << RESET);
	DDRB |= (1 << CS);
	DDRB |= (1 << BL_PWM);
	DDRB |= (1 << WR_BLT_CLK);
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
	
	sei(); //Enable global interrupt
	systemCheck();
	startupMessage();
	
	writeSRAM(Magenta, Yellow, Light_Blue, 0);
	writeSRAM(Black, White, Black, pixels);
	writeSRAM(Cyan, Blue, Red, pixels*2);
    while (1) 
    {
		/*TODO:
		R02h to fix display flicker. Do that after initialization.
		Blitting:
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
		- BØR: System-kontroll ved start.
		- BØR: Lagring av grafikk i FLASH.
		
		- KAN: Lage et scriptingspråk med makroer som kan lagres i ekstern FLASH for enerering av grafikk.
		- KAN: Presse ytelsen i systemet.
		- KAN: Krever kanskje utvidet bruk av pekere som funksjonspekere
		- KAN: Krever kanskje innslag av assembly i høynivåkode. Selvstudium.
		- INFO:Utviklingen vil skje på egen maskinvareplattform. */
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