/*
 * UARTCommands.c
 *
 * Created: 10.05.2022 20:03:16
 *  Author: William M. R. Schmidt
 */ 

#include "misc.h"
#include "UART.h"
#include "HY32D.h"

// Type S for system check:
void systemCheck(void){
	transmitUART(CR);
	transmitUART('-');
	transmitUART(CR);
	transmitUART('S');	transmitUART('y'); 	transmitUART('s'); 	transmitUART('t'); 	transmitUART('e');
	transmitUART('m');	transmitUART(' '); 	transmitUART('c'); 	transmitUART('h'); 	transmitUART('e');
	transmitUART('c');	transmitUART('k'); 	transmitUART(':');
	lcdStatusRead();
	// Check and print results of various components on the blitter board:
}

void startupMessage(void){
	transmitUART('B');	transmitUART('l'); 	transmitUART('i'); 	transmitUART('t'); 	transmitUART('t');
	transmitUART('e');	transmitUART('r'); 	transmitUART(' '); 	transmitUART('r'); 	transmitUART('e');
	transmitUART('a');	transmitUART('d'); 	transmitUART('y');
	transmitUART(CR);
	// Check and print results of various components on the blitter board:
}