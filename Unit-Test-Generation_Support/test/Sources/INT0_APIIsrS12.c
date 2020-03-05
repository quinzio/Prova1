/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_APIIsrS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_APIIsrS12.c
    \version see \ref FMW_VERSION 
    \brief Autonomous periodical interrupt routines.
    \details These routines manage API interrupt ( for windowed watchdog).\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF       /* needed for const def */
#include "TIM1_API.h"
#include MAC0_Register      /* Needed for macro definitions */
#include "WTD1_Watchdog.h"
#include "INT0_APIIsrS12.h"
#include "main.h"           /* Per strDebug */

/*PRQA S 0380,0857 -- */

#if defined(WINDOWED_WATCHDOG)

  __interrupt void INT0_APIIsr(void)
  {
    if(BoolReadyResWatchdog==(BOOL)TRUE)
      {
        RESET_COP_WATCHDOG;
        BoolReadyResWatchdog=FALSE; 
      }
    else
      {
         /* Misra  */
      }
    API_CLEAR_PENDING_INTERRUPT;
  }
  
#endif /* WINDOWED_WATCHDOG  */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
