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
#include "makroer.h"
#include "SRAM.h"

// Global variables
uint8_t remoteEcho = 0;

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
		
    }
}

/*
	Interrupt kjører 1 gang hver 256. opptelling, som inkrementerer memCounter og sjekker om den er over memTarget
	og skal stoppe blittingen.
	Kan endre på target slik (memTarget %(modulus) TIMER0_COMP_Target) = 0. Da får vi ingen overshoot.
*/
ISR(TIMER0_COMP_vect){
	//transmitUART('w');
	//wrSignal();
	//rdSignalSRAM();
	/*
	memCounter += 256;
	// Ending of readSRAM function
	if(memCounter >= memTarget){
		disable_counter_PB4;
		CS_HIGH; // deselect LCD and SRAM
		BLT_EN_LOW; // counters disabled
		SRAMOutputDisable(); // disable SRAM OE, WE
		setIOtoOutput();
	}*/
}


ISR(USART0_RX_vect){
	// UART avbrudd inspirert av Ingulf sin eksempelkode fra Øving 3.
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