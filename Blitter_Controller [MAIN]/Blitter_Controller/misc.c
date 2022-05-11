/*
 * misc.c
 *
 * Created: 11.05.2022 01:13:55
 *  Author: William M. R. Schmidt
 */ 

#include "stdint.h"

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
	}
}