/*
 * oving3.c
 *
 * Created: 06.05.2022 18:03:41
 * Author : William
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#define uart_bps 9600L  // max 57600

#define UARTRxNotEmpty (UCSR0A & (1 << RXC0))    // Filter out the Rx buffer empty bit from the UART control register
#define UARTBuffer UDR0
#define keyPress (~PINB & 0b00001111)         // Invert the inputs because of active low and filter out the keys only.
#define txBufferFree ( UCSR0A & (1 << UDRE0))    // Filter out the Tx buffer free bit from the UART control register
#define key0 0b00000001
#define key1 0b00000010
#define key2 0b00000100
#define key3 0b00001000
#define Bell 0x07
#define backspace 0x08
#define CR 0x0D
#define LF 0x0A

#define BUFFER_SIZE 8 // 8 bytes buffer size

// Prototypes
void initUART (void);
void initLED (void);
void initKey (void);
void outputByteToLED(uint8_t outputData);
void writeBuffer(char data);
void emptyBuffer(void);

// Global variables
char buffer[BUFFER_SIZE];
int bufferIndex;
uint8_t remoteEcho = 0;
char receivedByte;

int main(void)
{
    initUART(); // initialize the UART
	initLED();
	initKey();
	bufferIndex = 0;
	sei(); //Enable global interrupt
	
    while (1) 
    {
		char keysPressed = keyPress;
		if (keysPressed){
			switch(keysPressed) {
				case(key0): 
					UARTBuffer = '1';
					break;
				case(key1):
					UARTBuffer = '2';
					break;
				case(key2):
					UARTBuffer = '3';
					break;
				case(key3):
					UARTBuffer = '4';
					break;
				default:
					break;
			}
			_delay_ms(100); // delay so not to spam the terminal
		}
    }
}

ISR(USART_RX_vect){
	// Read received data to variable. This also clears the interrupt flag. If data isn't read a new interrupt will immediately happen. See 19.7.3 datasheet.
	// When the receive complete interrupt enable (RXCIEn) in UCSRnB is set, the USART receive complete interrupt will be executed as long as the RXCn flag is set.
	receivedByte = UARTBuffer; // local temporary variable for received byte
	if (receivedByte){
		outputByteToLED(receivedByte); // Copy the received byte to the LEDs.
		// If the key received through UART is control character 0 ( <ctrl> <space> in PuTTY ) then toggle the state of serial echo.
		if (receivedByte == 45) { // that's the '-' sign. CTRL+SPACE doesnt work for me right now.
			remoteEcho = ~remoteEcho;
		}
		if(bufferIndex < BUFFER_SIZE+1){ // If bufferindex is less than buffersize write buffer.
			writeBuffer(receivedByte);
		}
		if(bufferIndex > BUFFER_SIZE){ // buffer is full (after position 7 is taken, bufferIndex is at 8)
			int i = BUFFER_SIZE+1;
			while(i >= 0){ // i = 9(CR), 8(0), 7(1) ... 1(7), 
				if(UCSR0A & (1 << UDRE0)){ // check if TX is free
					if(i == 0){
						UARTBuffer = CR;
					}
					else if(i <= BUFFER_SIZE){ // if i has gone through the CR, LF postings, it loops here.
						UARTBuffer = buffer[i]; // should be 8-8, 8-7, 8-6 ...
					}else if(i == BUFFER_SIZE+1){
						UARTBuffer = CR;
						//UARTBuffer = LF;
					}
					i -= 1;
				}	
			}
			resetBuffer();
		}
	// If echo is on, and the ASCII character is higher than 31, or Bell, Carriage Return, Line Feed or backspace, then echo the character. Other are filtered as to not get strange behavior from Putty.
	}
	if (remoteEcho && ((receivedByte > 31) || (receivedByte == Bell) || (receivedByte == CR) || (receivedByte == LF) || (receivedByte == backspace))){
		if(UCSR0A & (1 << UDRE0)){ // check if byte is 1
			UARTBuffer = receivedByte;
		}
	}
}

void writeBuffer(char data){
	buffer[bufferIndex] = data;
	bufferIndex += 1;
}

void resetBuffer(){ // resets index and sets all buffer variables to 0.
	bufferIndex = 0;
	for(short i = 0; i < 8; i++){
		buffer[i] = 0x00;
	}
}

void outputByteToLED(uint8_t outputData){
	PIND = ((~PORTD) ^ outputData) & 0b11111100; // Toggle the PORTD bits that differ from the requested value, by writing 1 to PIND for those bits.
	PINB = ((~PORTB) ^ (outputData << 4)) & 0b00110000; // Toggle the PORTB bits that differ from the requested value, by writing 1 to PINB for those bits.
}

void initLED (void){
	DDRD |= 0b11111100; // set the last 6 pins of PORTD as output for the LEDs
	PORTD |= 0b11111100; // switch off the LEDs
	DDRB |= 0b00110000; // set bit 5 and 6 of PORTB as outputs for the LEDs
	PORTB |=0b00110000; // switch off the LEDs
}

void initKey (void){
	DDRB &= 0b11110000; // makes sure the direction of switches are input. (can be omitted as default DDR is 0)
	PORTB |=0b00001111; // setting pull-up for switches
}


void initUART (void){
	// calculating the baud rate clock division registers at compile time and setting the baud rate registers properly at run time.
	const uint8_t baudRateReg_low = (uint8_t) ((( F_CPU / ( 16 * uart_bps )) - 1 ) & 0xFF);
	const uint8_t baudRateReg_high = (uint8_t) ((( F_CPU / ( 16 * uart_bps )) - 1 ) >> 8);
	UBRR0L = baudRateReg_low;
	UBRR0H = baudRateReg_high;
	
	// Alternative way of calculating the baud rate register, with casting
	//	const uint8_t UBRR_reg_val = (uint8_t) ((sysClk / 16.0 * (float) uart_bps) - 1.0);

	// Setting up the USART control registers. All bits are included for sake of overview. Compiler will optimize.
	UCSR0A = 0x00 | (0 << RXC0) | (0 << TXC0) | (0 << UDRE0) | (0 << FE0) | (0 << DOR0) | (0 << UPE0) | (0 << U2X0) | (0 << MPCM0);  // NOT running 2x mode
	UCSR0B = 0x00 | (1 << RXCIE0) | (0 << TXCIE0) | (0 << UDRIE0) | (1 << RXEN0) | (1 << TXEN0) | (0 << UCSZ02);  // Enable Rx and Tx setting the ports mux correctly
	UCSR0C = 0x00 | (0 << UMSEL01) | (0 << UMSEL00) | (0 << UPM01) | (0 << UPM00) | (0 << USBS0) | (1 << UCSZ01) | (1 << UCSZ00); // Selecting 8-bit character for the USART
	
	/* Control register descriptions (added here for convenience)
	[Initial value]
	
	
	UCSRnA – USART Control and Status Register n A
	-----------------------------------------------
	Bit 7 – [0] RXCn: USART Receive Complete
	Bit 6 – [0] TXCn: USART Transmit Complete
	Bit 5 – [1] UDREn: USART Data Register Empty
	Bit 4 – [0] FEn: Frame Error
	Bit 3 – [0] DORn: Data OverRun
	Bit 2 – [0] UPEn: USART Parity Error
	Bit 1 – [0] U2Xn: Double the USART Transmission Speed
	Bit 0 – [0] MPCMn: Multi-processor Communication Mode
	
	
	UCSRnB – USART Control and Status Register n B
	-----------------------------------------------
	Bit 7 – [0] RXCIEn: RX Complete Interrupt Enable n
	Bit 6 – [0] TXCIEn: TX Complete Interrupt Enable n
	Bit 5 – [0] UDRIEn: USART Data Register Empty Interrupt Enable n
	Bit 4 – [0] RXENn: Receiver Enable n
	Bit 3 – [0] TXENn: Transmitter Enable n
	Bit 2 – [0] UCSZn2: Character Size n
	Bit 1 – [0] RXB8n: Receive Data Bit 8 n
	Bit 0 – [0] TXB8n: Transmit Data Bit 8 n
	
	
	UCSRnC – USART Control and Status Register n C
	-----------------------------------------------
	Bits 7:6 – [00] UMSELn1:0 USART Mode Select
	Bits 5:4 – [00] UPMn1:0: Parity Mode
	Bit  3   – [0]  USBSn: Stop Bit Select
	Bits 2:1 – [11] UCSZn1:0: Character Size
	Bit  0   – [0]  UCPOLn: Clock Polarity


	UMSELn - USART Mode Select - Bits Settings
	------------------------------------------
	| UMSELn1 |  UMSELn0 | Mode
	|    0    |     0    |  Asynchronous UART
	|    0    |     1    |  Synchronous UART
	|    1    |     0    |  (reserved)
	|    1    |     1    |  Master SPI (MSPIM)
	
	
	UPMn1:0: Parity Mode
	--------------------
	| UPMn1 |  UPMn0 | Mode
	|   0   |    0   |  Disabled
	|   0   |    1   |  Reserved
	|   1   |    0   |  Enabled, even parity
	|   1   |    1   |  Enabled, odd parity
	
	
	USBS - Stop Bit Select - Bit Settings
	-------------------------------------
	| USBS | Mode
	|   0  |  1-bit
	|   1  |  2-bit
	
	UCSZn - Character Size - Bits Settings
    --------------------------------------
	| UCSZn2 | UCSZn1 | UCSZn0  | Mode
	|   0    |   0    |    0    |  5-bit
	|   0    |   0    |    1    |  6-bit
	|   0    |   1    |    0    |  7-bit
	|   0    |   1    |    1    |  8-bit
	|   1    |   0    |    0    |  Reserved
	|   1    |   0    |    1    |  Reserved
	|   1    |   1    |    0    |  Reserved
	|   1    |   1    |    1    |  9-bit
	
	
	UCPOLn - Clock Polarity - Bit Settings
    --------------------------------------
	| UCPOLn | Mode
	|    0   |  Falling XCKn edge
	|    1   |  Rising XCKn edge
	
	*/
	
	
	
	
}