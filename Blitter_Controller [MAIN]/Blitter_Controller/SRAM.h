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
void SRAMOutputDisable(void);
void presetCounters(uint32_t value);
void setCountersToZero(void);

#endif /* SRAM_H_ */