/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  ADM0_ADCS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file ADM0_ADCS12.c
    \version see \ref FMW_VERSION 
    \brief Basic ADC convertion.
    \details Routines for basic ADC operation and variables initialization.\n
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
#include SPAL_DEF
#include MAC0_Register  /* Needed for macro definitions */
#include ADM0_ADC
#include "main.h"       /* Per strDebug */

/*PRQA S 0380,0857 -- */

/* ---------------------------- Public Variables --------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var buffu16ADC_READING
    \brief used for keeping successive pwm time based convertion results.\n
    \note This array is placed in \b _DATA_ZEROPAGE segment because of managing 
    in interrupt routine.
    \warning None.
    \details None.
*/
volatile u16 buffu16ADC_READING[ADC_READING_LENGHT];

#pragma DATA_SEG DEFAULT

/** 
    \var u8InverterStatus
    \brief used for storing current inverter state.\n
    \note This variable is placed in \b DEFAULT segment.
    \warning None.
    \details None.
*/
u8 u8InverterStatus = 0;

/* ---------------------------- Private Variables --------------------------- */

/*-----------------------------------------------------------------------------*/
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
