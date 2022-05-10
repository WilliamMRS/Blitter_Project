/*
 * UARTCommands.c
 *
 * Created: 10.05.2022 20:03:16
 *  Author: William M. R. Schmidt
 */ 

#include "ASCII.h"
#include "UART.h"

// Type S for system check:
void systemCheck(void){
	transmitUART(CR);
	transmitUART(S);
	transmitUART(y);
	transmitUART(s);
	transmitUART(t);
	transmitUART(e);
	transmitUART(m);
	transmitUART(_space);
	transmitUART(c);
	transmitUART(h);
	transmitUART(e);
	transmitUART(c);
	transmitUART(k);
	transmitUART(_colon);
	transmitUART(CR);
	// Check and print results of various components on the blitter board:
}

void startupMessage(void){
	transmitUART(CR);
	transmitUART(B);
	transmitUART(l);
	transmitUART(i);
	transmitUART(t);
	transmitUART(t);
	transmitUART(e);
	transmitUART(r);
	transmitUART(_space);
	transmitUART(r);
	transmitUART(e);
	transmitUART(a);
	transmitUART(d);
	transmitUART(y);
	transmitUART(CR);
	// Check and print results of various components on the blitter board:
}