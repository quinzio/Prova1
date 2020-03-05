/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  TIM1_API.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file TIM1_RTI.c
    \version see \ref FMW_VERSION 
    \brief Autonomous periodical timing.
    \details Routines for time base operation and variables related periodical timing (for windowed watchdog).\n
    \note none
    \warning none
*/

#ifdef _CANTATA_
#include "cantata.h"
#endif /*_CANTATA_*/
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register      /* Needed for macro definitions */
#include "TIM1_API.h"
#include "main.h"           /* Per strDebug */
#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif /* Nop() */

/*--------------- Public Variables declaration section ---------------------*/

/* ---------------------- Private variables ----------------------------------*/


/*-----------------------------------------------------------------------------*/
/** 
    \fn API_InitAPI
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio  \n <em>Reviewer</em>

    \date 07/01/2013
    \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initialize Autonomous Periodic Interrupt(API)\n  
    \details When WINDOWED_WATCHDOG is defined API is enabled and connected to ACLK.\n
      The autonomous clock (ACLK) is not factory trimmed and its frequency is 10kHz.\n
    \return none 
    \note none
    \warning none  
   
*/
void API_InitAPI(void)
{
  /* Initialize API Counter and interrupt connected to ACLK */
  /* Initialize API to manage Windowed watchdog */
  #if defined(WINDOWED_WATCHDOG)

   API_ENABLE_API;

  #else

   API_DISABLE_API;

  #endif /*WINDOWED_WATCHDOG*/
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
