#ifndef _CANTATA_H_
#define _CANTATA_H_

#include "stdint.h"
#define static
#define volatile
#define __interrupt
#define near

int num_of_iter, index_instr_for;
long getForLoopNumOfIteration(void);
void TimerInterruptEmulator(void);
void TachoInterrupEmulator(void);
void alter_tacho_flags(void);

extern int dummymem[0x10000];

#endif /*_CANTATA_H_*/
