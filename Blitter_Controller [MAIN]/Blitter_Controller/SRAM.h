/*
 * SRAM.h
 *
 * Created: 12/05/2022 14:45:43
 *  Author: William M. R. Schmidt
 */ 


#ifndef SRAM_H_
#define SRAM_H_

void wrSignalSRAM(void);
void rdSignalSRAM(void);
void loadDataToOutputLines(unsigned short data);
void SRAMOutputDisable(void);
void presetCounters(uint32_t value);
void setCountersToZero(void);
void readSRAM(uint32_t memStart);
void writeSRAM(uint16_t color1, uint16_t color2, uint16_t color3, uint32_t memStart);

uint8_t continueReadFunction;
uint32_t memTarget;
uint32_t memCounter;

#endif /* SRAM_H_ */