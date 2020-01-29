/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_DIAGIsrS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_DIAGIsrS12.c
    \version see \ref FMW_VERSION 
    \brief Diagnostic interrupt routines.
    \details These routines manage diagnostic.\n
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
#include SPAL_DEF             /* needed for const def */
#include "main.h"             /* Needed for SystStatus_t def */
#include "DIA1_Diagnostic.h"
#include "SOA1_SOA.h"         /* Needed for errors definitions */
#include MAC0_Register        /* Needed for macro definitions */
#include "main.h"             /* Per strDebug */
#include "INT0_DIAGIsrS12.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
/* -------------------------- Public variables ----------------------------- */

#pragma DATA_SEG DEFAULT
/* ------------------------- Private variables --------------------------- */
static u16 u16DiagnosticNextIsr;
/** 
    \var u16DiagPeriodCount
    \brief Used to generate diagnotic error frequency.\n
    \details This variable is incremented every timer tick i.e. at 5MHz.
    \note This variable is placed in \b DEFAULT segment.
    \warning This variable is \b STATIC.
*/
static u16 u16DiagPeriodCount;

/** 
    \var u16DiagPeriodDuty
    \brief Used to generate diagnotic error duty.\n
    \details This variable is incremented every timer tick i.e. @ 5MHz.
    \note This variable is placed in \b DEFAULT segment.
    \warning This variable is \b STATIC.
*/
static u16 u16DiagPeriodDuty;

/** 
    \var BOOLDiagPeriodFlag
    \brief Used to signalize that a diagnostic period is elapsed.\n
    \details None.
    \note This variable is placed in \b DEFAULT segment.
    \warning This variable is \b STATIC.
*/
static BOOL BOOLDiagPeriodFlag;


/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_BOOLDiagPeriodFlag(BOOL value)
    \author	Gabriele Ruosi  \n <em>Reviewer</em> 

    \date 16/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Sets the value of the static variable BOOLDiagPeriodFlag
                            \par Used Variables
    \ref BOOLDiagPeriodFlag \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_BOOLDiagPeriodFlag (BOOL value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOLDiagPeriodFlag = value;
}


/*-----------------------------------------------------------------------------*/
/**
    \fn BOOL INT0_Get_BOOLDiagPeriodFlag(void)
    \author	Gabriele Ruosi  \n <em>Reviewer</em> 

    \date 16/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Gets the value of the static variable BOOLDiagPeriodFlag
                            \par Used Variables
    \ref BOOLDiagPeriodFlag \n
    \return \b The value of the static variable BOOLDiagPeriodFlag.
    \note None.
    \warning None.
*/
BOOL INT0_Get_BOOLDiagPeriodFlag (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  return(BOOLDiagPeriodFlag);
}


/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16DiagnosticNextIsr(u16 u16Value)
    \author	Gabriele Ruosi  \n <em>Reviewer</em> 

    \date 16/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Sets the value of the static variable u16DiagnosticNextIsr
                            \par Used Variables
    \ref u16DiagnosticNextIsr \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16DiagnosticNextIsr (u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16DiagnosticNextIsr = u16Value;
}


/*-----------------------------------------------------------------------------*/
/**
    \fn u16 INT0_Get_u16DiagnosticNextIsr(void)
    \author	Gabriele Ruosi  \n <em>Reviewer</em> 

    \date 08/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Gets the value of the static variable u16DiagnosticNextIsr
                            \par Used Variables
    \ref u16DiagnosticNextIsr \n
    \return \b The value of the static variable u16DiagnosticNextIsr.
    \note None.
    \warning None.
*/
u16 INT0_Get_u16DiagnosticNextIsr (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  return(u16DiagnosticNextIsr);
}


/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16DiagPeriodCount(void)
    \author	Gabriele Ruosi  \n <em>Reviewer</em> 

    \date 08/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set the value of the variable used to generate diagnotic error frequency  
                            \par Used Variables
    \ref u16DiagPeriodCount \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16DiagPeriodCount (u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16DiagPeriodCount = u16Value;
}


/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16DiagPeriodDuty(void)
    \author	Gabriele Ruosi  \n <em>Reviewer</em> 

    \date 08/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set the value of the variable used to generate diagnotic error duty  
                            \par Used Variables
    \ref u16DiagPeriodDuty \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16DiagPeriodDuty (u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16DiagPeriodDuty = u16Value;
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_DiagnosticIsr(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This interrupt manages diagnostic output by setting right freq and duty values.  
    \details This function manages timer3 channel interrupt. It sets right freq and duty
    values into timer1 channel 0 used for dignostic timing. These value are set into 
    \ref DIAGNOSTIC_SET_SIGNAL macro.\n 
    If any error is detected, diagnostic output will supply 10 hz frequency output with
    duty cycle depending on error type according to the following table:\n
    DutyCycle = 7% * strErrorStatus.enumDiagError @  strErrorStatus.enumDiagError = 0...10 set into 
    \ref DIAG_SetDiagnosticError funtion.\n
    variables.\n
                            \par Used Variables
    \return \b void no value return
    \note This routine is currently not used because of timer3 problem observed.
    \warning None
*/
__interrupt void INT0_DiagnosticIsr(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 u16DiagnosticNextIsr_tmp;
  
  if ( u16DiagPeriodCount >= DIAGNOSTIC_END_PERIOD )
  {
    u16DiagPeriodCount = 0;
    BOOLDiagPeriodFlag = TRUE;
  }
  if ( u16DiagPeriodCount < u16DiagPeriodDuty )
  {
    DIAGNOSTIC_ON;
  }
  else
  {
    DIAGNOSTIC_OFF;
  }

   u16DiagPeriodCount++;

  u16DiagnosticNextIsr_tmp = u16DiagnosticNextIsr;

  DIAGNOSTIC_NEW_INTERRUPT_CAPTURE((u16DiagnosticNextIsr_tmp));  /* N.B.                                                                                                    */
                                                                 /* Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare */
                                                                 /* channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared                     */
}

#endif  /* DIAGNOSTIC_STATE */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
