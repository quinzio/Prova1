/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INI0_InitS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INI0_InitS12.h
    \version see \ref FMW_VERSION 
    \brief Functions providing micro peripherals initialization.
    \details These routines are called as first steps when entering into main
    and provide basic hw initialization for micro peripherals.\n
    \note none
    \warning none
*/

#ifndef INIT_H
#define INIT_H  

/* Public Constants -------------------------------------------------------- */
/* Clock */
 #define MTC_CLOCK	(u32)20000000 	/* Resolution: 1Hz */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void Init_InitMCU(void);
/*void Init_InitGPIO(void);*/
void Init_InitTimer(void);
void Init_InitPWMGenerator(void);
void Init_InitADC(void);
void Init_InitTacho(void);
#endif /*INIT_H */
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
