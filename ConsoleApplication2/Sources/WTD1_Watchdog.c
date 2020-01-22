/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  WTD1_Watchdog.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register      /* Needed for Watchdog */
#include "TIM1_API.h"
#include "main.h"           /* Per strDebug */
#include "WTD1_Watchdog.h"

/*PRQA S 0380,0857 -- */

/* ---------------------------- Private Constants  ------------------------ */

/* ---------------------------- Public Variables   ------------------------ */
#ifdef WINDOWED_WATCHDOG
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE
    BOOL BoolReadyResWatchdog;
  #pragma DATA_SEG DEFAULT
#endif
/* ---------------------------- Private Variables  ------------------------ */

/* ---------------------------- Public prototipes  -------------------------*/

/* ---------------------------- Private prototipes -------------------------*/

  void WTD_InitWatchdog(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
  {
    #if defined(WINDOWED_WATCHDOG)    /* .. and wrong zero page addressing */
     WATCHDOG_INIT_WINDOWED_WTD;
     BoolReadyResWatchdog=FALSE;
    #else
     WATCHDOG_INIT_NORMAL_WTD;
    #endif
  }

  void WTD_ResetWatchdog(void)
  {
    #if defined(WINDOWED_WATCHDOG)
     BoolReadyResWatchdog=TRUE;
    #else
     RESET_COP_WATCHDOG;
    #endif
  }

/*** (c) 2012 SPAL Automotive ****************** END OF FILE **************/
