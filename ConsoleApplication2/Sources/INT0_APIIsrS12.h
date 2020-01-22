/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_APIIsrS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_APIIsrS12.h
    \version see \ref FMW_VERSION 
    \brief Tacho interrupt routines prototypes.
    \details These routines manage API interrupts.\n
    \note none
    \warning none
*/

#ifndef INT0_APIIsrS12_H
#define INT0_APIIsrS12_H  


/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
#if defined(WINDOWED_WATCHDOG)
__interrupt void INT0_APIIsr(void);
#endif

#endif  // INT0_APIIsrS12_H

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/

