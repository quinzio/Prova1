/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  WTD1_Watchdog.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file WTD1_Watchdog.h
    \version see \ref FMW_VERSION 
    \brief Function to manage watchdog.
    \details \n
    \note none
    \warning none
*/

#ifndef WATCHDOG_H
#define WATCHDOG_H

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

#ifdef WINDOWED_WATCHDOG
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE
    extern BOOL BoolReadyResWatchdog;
  #pragma DATA_SEG DEFAULT
#endif

/* Public Functions prototypes --------------------------------------------- */
void WTD_InitWatchdog(void);
void WTD_ResetWatchdog(void);

#endif  /* WATCHDOG_H */

/*** (c) 2012 SPAL Automotive ****************** END OF FILE **************/
