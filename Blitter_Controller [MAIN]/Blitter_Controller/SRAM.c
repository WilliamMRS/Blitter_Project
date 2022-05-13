/*
 * SRAM.c
 *
 * Created: 12/05/2022 14:45:23
 *  Author: William M. R. Schmidt
 */ 

#include <avr/io.h>
#include "makroer.h"
#include <util/delay.h>
#include "HY32D.h"
#include "UART.h"

uint32_t memTarget = 0;
uint32_t memCounter = 0;
uint8_t continueReadFunction = 1;

// Sends write signal to SRAM, so SRAM takes the data on D0-D15 and 
// writes it to the address given by the counters.
void wrSignalSRAM(void){
	SRAM_WE_LOW;
	_delay_us(1); //needed?
	SRAM_WE_HIGH;
}

// Sends read signal to SRAM, thus SRAM sends data to D0-D15
void rdSignalSRAM(void){
	SRAM_OE_LOW;
	SRAM_OE_HIGH;
}

void SRAMOutputDisable(void){
	SRAM_OE_HIGH; // PORTE 4
	SRAM_WE_HIGH; // PORTE 5
}

void setCountersToZero(void){
	CS_HIGH; // Deselect LCD and SRAM
	SRAMOutputDisable(); // Disable SRAM
	setIOtoOutput(); // Set all lines to output
	RESET_LOW;
	_delay_us(10);
	RESET_HIGH;
	_delay_ms(10);
}

void presetCounters(uint32_t value){	
	CS_HIGH; // Deselect LCD and SRAM
	SRAMOutputDisable(); // Disable SRAM
	setIOtoOutput(); // Set all lines to output
	
	// Calculate the Bytes for the preset
	// TODO: Idk why but it prints not the expected hex numbers ...
	uint8_t uByte = ((value >> 16) & 0x0F); // only get the first 4 bits.
	uint8_t mByte = ((value >> 8) & 0xFF);
	uint8_t lByte = value & 0xFF;
	transmitUART(CR);
	transmit8BitAsHex(uByte);
	transmit8BitAsHex(mByte);
	transmit8BitAsHex(lByte);
	transmitUART(CR);

	D0_D7 = lByte;
	D8_D15 = mByte;
	// PORTE, set 4 - 7 to correct bits (these are the extra datalines going to counters)
	PORTE &= 0x0F; // clear upper bytes.
	PORTE |= (uByte << 4); // shifts the bytes up to the upper level and 'or' them
	
	_delay_ms(10);
	// Counter(s) signal lines (For Preset data: (CLR: H, LOAD: L))
	RESET_HIGH; // Set CLR (BLT_RST)(PB0)(RESET) to HIGH
	LOAD_LOW; // Set LOAD (PE3) to LOW, so the counters will accept the data.
	_delay_ms(10);
	wrSignal();// Send blitsignal (aka a clk)
	SRAMOutputDisable(); // SRAM Output disable (PE4 & PE5 are HIGH)
}

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
	
	// using this for loop if not using blitting with the integrated counters.
	for(unsigned long int i = 0; i < (pixels); i++){
		rdSignalSRAM(); // SRAM OE goes LOW-HIGH reading the SRAM values on the address specified by the counters to the screen.
		// PE4 flip fast as f
		wrSignal(); // counters increment by one and screen updates. BLT_EN and WR
		// PB4 flip fast as f
	}
	
	/*
		use modulus division to set counter target to a number that eventually hits memTarget with 0 carry.
	*/
	//memTarget = (memStart + pixels);
	//memCounter = memStart;
	//TCCR0A = 0x00 | (0 << FOC0A) | (0 << WGM00) | (0 << COM0A1) | (1 << COM0A0) | (0 << WGM01) | (0 << CS02) | (0 << CS01) | (1 << CS00);

	// Interrupt in main.c takes over
}

void loadDataToOutputLines(unsigned short data){
	lData = (data & 0xFF);
	hData = ((data >> 8) & 0xFF);
	D0_D7 = lData; // Write data to GPIO lines (lines should by default be output)
	D8_D15 = hData;
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