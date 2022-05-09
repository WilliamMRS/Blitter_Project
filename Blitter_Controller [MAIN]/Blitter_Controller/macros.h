/*
 * macros.h
 *
 * Created: 05/05/2022 20:21:22
 *  Author: William M. R. Schmidt
 */ 

#ifndef F_CPU
#define F_CPU 7372800UL
#endif

#ifndef MACROS_H_
#define MACROS_H_

#define uart_bps 9600L  // max 57600

#define UARTRxNotEmpty (UCSR0A & (1 << RXC0))    // Filter out the Rx buffer empty bit from the UART control register
#define UARTBuffer UDR0
#define txBufferFree ( UCSR0A & (1 << UDRE0))    // Filter out the Tx buffer free bit from the UART control register
#define Bell 0x07
#define backspace 0x08
#define CR 0x0D
#define LF 0x0A

#define BUFFER_SIZE 8 // 8 bytes buffer size

#define A 0x41
#define B 0x42
#define C 0x43
#define D 0x44
#define E 0x45
#define F 0x46
#define G 0x47
#define H 0x48
#define I 0x49
#define J 0x4A
#define K 0x4B
#define L 0x4C
#define M 0x4D
#define N 0x4E
#define O 0x4F
#define P 0x50
#define Q 0x51
#define R 0x52
#define S 0x53
#define T 0x54
#define U 0x55
#define W 0x56
#define X 0x57
#define Y 0x58

#define a 0x61
#define b 0x62
#define c 0x63
#define d 0x64
#define e 0x65
#define f 0x66
#define g 0x67
#define h 0x68
#define i 0x69
#define j 0x6A
#define k 0x6B
#define l 0x6C
#define m 0x6D
#define n 0x6E
#define o 0x6F
#define p 0x70
#define q 0x71
#define r 0x72
#define s 0x73
#define t 0x74
#define u 0x75
#define v 0x76
#define w 0x77
#define x 0x78
#define y 0x79

#define _space 0x20
#define _colon 0x3A

#endif /* MACROS_H_ */