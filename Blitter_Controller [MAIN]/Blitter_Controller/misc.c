/*
 * misc.c
 *
 * Created: 11.05.2022 01:13:55
 *  Author: William M. R. Schmidt
 */ 

#include "stdint.h"
#include "UART.h"
#include "misc.h"

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

// experimental to say the least
void transmitInt(uint32_t value){
		uint8_t hByte;
		uint8_t mByte;
		uint8_t lByte;
		hByte = ((value >> 16) & 0x0F);
		mByte = ((value >> 8) & 0xFF);
		lByte = value & 0xFF;
		transmitUART(CR);
		transmitUART('0');
		transmitUART('x');
		if(hByte > 15){
			uint8_t uNumber;
			uint8_t lNumber;
			uNumber = (hByte & 0xF0);
			lNumber = (hByte & 0x0F);
			transmitUART(toHex(uNumber));
			transmitUART(toHex(lNumber));
			}else{
			transmitUART(toHex(hByte));
		}
		if(mByte > 15){
			uint8_t uNumber;
			uint8_t lNumber;
			uNumber = (mByte & 0xF0);
			lNumber = (mByte & 0x0F);
			transmitUART(toHex(uNumber));
			transmitUART(toHex(lNumber));
			}else{
			transmitUART(toHex(mByte));
		}
		if(lByte > 15){
			uint8_t uNumber;
			uint8_t lNumber;
			uNumber = (lByte & 0xF0);
			lNumber = (lByte & 0x0F);
			transmitUART(toHex(uNumber));
			transmitUART(toHex(lNumber));
			}else{
			transmitUART(toHex(lByte));
		}
		transmitUART(CR);
}