/*
 * HY32D.h
 *
 * Created: 09/05/2022 17:28:10
 *  Author: willi
 */ 


#ifndef HY32D_H_
#define HY32D_H_

#define CS 6 // PB6
#define BL_PWM 5 // PB5
#define WR 4 // PB4
#define RD 6 // PE6
#define DC 7 // PE7
#define RESET 0 // PB0

void initHY32D(void);
void wrSignal(void);
void rdSignal(void);
void setIOtoOutput(void);
void setIOtoInput(void);
void writeIndex(unsigned short index);
void writeData(unsigned char lData, unsigned char hData);
void writeToRegister(unsigned short index, unsigned char lData, unsigned char hData);

#endif /* HY32D_H_ */