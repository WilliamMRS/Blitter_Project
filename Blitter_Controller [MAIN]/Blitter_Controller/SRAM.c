/*
 * SRAM.c
 *
 * Created: 12/05/2022 14:45:23
 *  Author: William M. R. Schmidt
 */ 

#include "misc.h"
#include <util/delay.h>
#include "HY32D.h"
#include "UART.h"

// Sends write signal to SRAM, so SRAM takes the data on D0-D15 and 
// writes it to the address given by the counters.
void wrSignalSRAM(void){
	SRAM_WE_LOW;
	_delay_us(1); // TODO: needed?
	SRAM_WE_HIGH;
}

// Sends read signal to SRAM.
// SRAM sends data to D0-D15.
void rdSignalSRAM(void){
	SRAM_OE_LOW;
	//_delay_us(1); // needed?
	SRAM_OE_HIGH;
}

void SRAMOutputDisable(void){
	SRAM_OE_HIGH; // PORTE 4
	SRAM_WE_HIGH; // PORTE 5
}

/*
	BLT_RST (to reset counter values is also an option. See counter datasheet.
*/
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

// UNFINISHED
void readFromSRAM(int address){

}