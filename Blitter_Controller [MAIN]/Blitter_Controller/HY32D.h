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

void wrSignal(void);
void rdSignal(void);
void setIOtoOutput(void);
void setIOtoInput(void);

#endif /* HY32D_H_ */