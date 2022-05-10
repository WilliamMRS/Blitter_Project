/*
 * UART.h
 *
 * Created: 09/05/2022 17:30:07
 *  Author: William M. R. Schmidt
 */ 


#ifndef UART_H_
#define UART_H_
#define UARTBuffer UDR0
#define uart_bps 9600L  // max 57600
#define UARTRxNotEmpty (UCSR0A & (1 << RXC0))    // Filter out the Rx buffer empty bit from the UART control register
#define txBufferFree ( UCSR0A & (1 << UDRE0))    // Filter out the Tx buffer free bit from the UART control register

void initUART(void);
int transmitUART (char data);

#endif /* UART_H_ */