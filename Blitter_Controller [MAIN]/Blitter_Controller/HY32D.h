/*
 * HY32D.h
 *
 * Created: 09/05/2022 17:28:10
 *  Author: willi
 */ 


#ifndef HY32D_H_
#define HY32D_H_

#define height 240
#define width 320
#define pixels 76800

void initHY32D(void);
void wrSignal(void);
void rdSignal(void);
void setIOtoOutput(void);
void setIOtoInput(void);
void writeIndex(unsigned short index);
void writeData(unsigned short data);
void writeToRegister(unsigned short index, unsigned short data);
void lcdStatusRead(void);
unsigned short readDataLines(void);
unsigned short readLCDData(void);

void fillScreen(unsigned short color);
void screenTest(void);
void colorTest(void);
void drawImage(unsigned short image[]);

unsigned char lData;
unsigned char hData;

#endif /* HY32D_H_ */