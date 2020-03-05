/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_TachoIsrS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_TachoIsrS12.c
    \version see \ref FMW_VERSION 
    \brief Tacho interrupt routines.
    \details These routines manage tacho interrupts.\n
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
#include SPAL_DEF           /* needed for const def                                                     */
#include "INP1_InputSP.h"   /* Needed for boolOVFVsPWMInput variable                                    */
#include "MCN1_Tacho.h"     /* Needed for u8TachoOverflowNumber variable                                */
#include "MCN1_acmotor.h"   /* it has to stand before MCN1_mtc.h because of StartStatus_t definition    */
#include "MCN1_mtc.h"       /* Needed for u8MTCStatus bitfield                                          */
#include MAC0_Register      /* Needed for macro definitions                                             */
#include "main.h"           /* Per strDebug                                                             */
#include "INT0_TachoIsrS12.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif

/* -------------------------- Private Constants ----------------------------*/

/* -------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables --------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var u16LastTimerValue
    \brief used for keeping last timer 2 value captured at tacho happening. 
    \details Each tacho (chosen edge) happening, timer module (TIM) value captured is 
    frozen into this variables.\n
    \note This varaible is placed in _DATA_ZEROPAGE because of managing 
    in interrupt routine.
    \warning This variable is static.
*/
static u16 u16LastTimerValue;

#pragma DATA_SEG DEFAULT
/* -------------------------- Private variables --------------------------- */

/* -------------------------- Private functions --------------------------- */

/* --------------------------- Private macros ------------------------------ */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_TachoOverflow(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This interrupt manages tacho timing.  
    \details This function manages timer3 overflow interrupt. It has to take into 
    account previous higher priority interrupt (if happened at the same time)that 
    could have already managed this overflow. That's the reason of used flag 
    \ref boolOVFVsPWMInput and \ref boolOVFVsTacho. If pwm input or tacho channel 
    happens at the same time of this overflow interrupt, since overflow interrupt 
    have lower priority, overflow has already managed into previous interrupt.\n
                            \par Used Variables
    \ref boolOVFVsPWMInput. If true overflow has been already managed in INT0_InputPWM_PWMInputISR
    interrupt service routine.\n
    \ref boolOVFVsTacho. If true overflow has been already managed in INT0_TachoCapture
    interrupt service routine.\n
    \ref pstrElectricFreqBuff_Index. Using ZCStatusBitfield on it.\n
    \return \b void no value return
    \note None.
    \warning None
*/
__interrupt void INT0_TachoOverflow(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  if( boolOVFVsPWMInput == 0u )
  {
    u8NumOvfPWM++;
  }
  else  /* Misra roules */
  {
    boolOVFVsPWMInput = FALSE;
  }

  if( boolOVFVsTacho == 0u )
  {
    u8TachoOverflowNumber++;
  }
  else  /* Misra roules */
  {
    boolOVFVsTacho = FALSE;
  }

  if( (BitTest((ZCVF_OVFinPWMisr), ZCStatusBitfield)) == 0u ) /* Take care of brackets between ! and BitTest. */
  {
    (pstrElectricFreqBuff_Index -> ZCVfOvfTemp)++;  /* Manage delta measuring isr priority */
  }
  else  /* Misra roules */
  {
    BitClear((ZCVF_OVFinPWMisr), ZCStatusBitfield);
  }

  TIM_CLEAR_PENDING_OF_INTERRUPT;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_TachoCapture(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This interrupt manages tacho timing.  
    \details This function manages timer3 capture interrupt. It has to manage lower
    priority interrupt (if happened at the same time) e.g. timer overflow. 
    That's the reason of used flag \ref boolOVFVsTacho. If overflow interrupt happens at
    the same time of this channel capture interrupt, since overflow interrupt have lower
    priority, overflow has previously managed into current interrupt.\n
    Moreover, this routine manages peak current value over electrical period. Partial
    peak current value found each pwm isr (i.e. over 50uS) are here stored into \ref u16PeakCurr
    variable. 
                            \par Used Variables
    \ref u16LastTimerValue. Last timer module (TIM captured value.\n
    \ref pstrElectricFreqBuff_Index. Pointer to PElectricFreq_s struct.\n
    \ref u8TachoOverflowNumber. Timer overflow number amoung two consegutive timer module (TIM capture.\n
    \ref u16MaxCurr. Partial peak current value from pwm isr routine.\n
    \ref u16PeakCurr. Current peak value over electrical period.\n
    \ref C_D_ISR_Status. Used for following flags:\n
    - \ref PHASE_INITIAL_SPEED\n
    - \ref HALL_CHECKED\n
    - \ref MANAGE_TACHO\n
    \ref u8MTCStatus. Used for following flags:\n
    - \ref PHASE_RESET\n
    \return \b void no value return
    \note None.
    \warning None
*/
__interrupt void INT0_TachoCapture(void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/ /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  pstrElectricFreqBuff_Index -> u16LastCapture = u16LastTimerValue;

  u16LastTimerValue = (u16)TACHO_CAPTURE_REGISTER;                                            /* N.B.                                                                                                    */
                                                                                              /* Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare */
                                                                                              /* channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared                     */

  pstrElectricFreqBuff_Index -> u16ActualCapture = u16LastTimerValue;

  if ( (TFLG2_TOF != 0u) && (((u8)((u16)u16LastTimerValue>>8)) == 0u) )                       /* If tacho overflow flag is set and                                */
  {                                                                                           /* and                                                              */
    u8TachoOverflowNumber++;                                                                  /* If TACHO_CAPTURE_REGISTER is closed to zero from positive values */
    boolOVFVsTacho = TRUE;
    (pstrElectricFreqBuff_Index -> ZCVfOvfTemp)++;
  }
  else
  {
    Nop(); /*PRQA S 1006 #Necessary assembly code*/
  }

  pstrElectricFreqBuff_Index -> u16NumOverflow = (u16)u8TachoOverflowNumber;
  u8TachoOverflowNumber = 0;

  /*BitSet(HALL_CHECKED, C_D_ISR_Status); */
  pstrElectricFreqBuff_Index -> ZCCurrSampled = (u16)(u16LastTimerValue);
  pstrElectricFreqBuff_Index -> ZCVfNumOverflow = pstrElectricFreqBuff_Index -> ZCVfOvfTemp;  

  if ( (BitTest((PHASE_INITIAL_SPEED), C_D_ISR_Status) == 0u) )                               /* Take care of parentesis between ! and BitTest. */
  {                                                                                           /* N.B.                                           */
    BitSet((ZCCurrFilter), ZCStatusBitfield);
    INTERRUPT_TACHO_DISABLE;
    u16PWMCount = 0;
  }

  #ifdef DIAGNOSTIC_STAND_ALONE
    if (( BitTest((DIAG_SPEED_FEEDBACK), C_D_ISR_Status) ) != (BOOL)FALSE)                    /* When main state machine turns out from wait state, tacho interrupt is enabled       */
    {                                                                                         /* in order to check for run on fly but diagnostic output hasn't to report motor speed */
      DIAGNOSTIC_TOGGLE_PORT;                                                                 /* but error causing wait state jump into. In this case:                               */
    }                                                                                         /* don't toggle diagnostic.                                                            */
    else
    {
      Nop(); /* Misra roules */
    }
  #endif  /* DIAGNOSTIC_STAND_ALONE */

  if ( BitTest((PHASE_RESET), u8MTCStatus) != 0u )                                            /* if(u8MTCStatus & PHASE_RESET)                                                              */
  {                                                                                           /*                                                                                            */
    u16Phase = u16PhaseBuff;  /*(u32)(((u32)u16PhaseBuff)<<16);*/                             /* set u32Phase value basing on u16PhaseBuff value computed into MTC_ComputeStartOnFlyPhase!! */
    BitClear((PHASE_RESET), u8MTCStatus);                                                     /* At this point, Tacho_ManageTachoCapture function is called.                                */
  }
  else
  {
    Nop(); /*PRQA S 1006 #Necessary assembly code*/
  }

  if ( pstrElectricFreqBuff_Index < &bufstrElectricFreqBuff[SPEED_FIFO_SIZE-1u])
  {
    pstrElectricFreqBuff_Index++;
  }
  else
  {
    pstrElectricFreqBuff_Index = (PElectricFreq_s)bufstrElectricFreqBuff;
  }

  u16PeakCurr = u16MaxCurr;                                                                   /* Save peak current                                                    */
  u16MaxCurr = 0;                                                                             /* Reset max current (partial pek current value over electrical period) */

  BitSet((MANAGE_TACHO), C_D_ISR_Status);

  /*INTERRUPT_TACHO_CLEAR;*/                                                                  /* Clear capture flag */
}


#ifdef DUAL_LINE_ZERO_CROSSING
__interrupt void INT0_TachoBemfCapture(void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/ /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  pstrElectricFreqBuff_Index -> u16LastCapture = u16LastTimerValue;

  u16LastTimerValue = (u16)BEMF_CAPTURE_REGISTER;                                             /* N.B.                                                                                                    */
                                                                                              /* Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare */
                                                                                              /* channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared                     */

  pstrElectricFreqBuff_Index -> u16ActualCapture = u16LastTimerValue;

  if ( (TFLG2_TOF != 0u) && (((u8)((u16)u16LastTimerValue>>8)) == 0u) )                       /* If tacho overflow flag is set and                                */
  {                                                                                           /* and                                                              */
    u8TachoOverflowNumber++;                                                                  /* If TACHO_CAPTURE_REGISTER is closed to zero from positive values */
    boolOVFVsTacho = TRUE;
    (pstrElectricFreqBuff_Index -> ZCVfOvfTemp)++;
  }
  else
  {
    Nop(); /*PRQA S 1006 #Necessary assembly code*/
  }

  pstrElectricFreqBuff_Index -> u16NumOverflow = (u16)u8TachoOverflowNumber;
  u8TachoOverflowNumber = 0;

  /*BitSet(HALL_CHECKED, C_D_ISR_Status);*/
  pstrElectricFreqBuff_Index -> ZCCurrSampled = (u16)(u16LastTimerValue);
  pstrElectricFreqBuff_Index -> ZCVfNumOverflow = pstrElectricFreqBuff_Index -> ZCVfOvfTemp;  

  #ifdef DIAGNOSTIC_STAND_ALONE
    if (( BitTest((DIAG_SPEED_FEEDBACK), C_D_ISR_Status) ) != (BOOL)FALSE)                    /* When main state machine turns out from wait state, tacho interrupt is enabled       */
    {                                                                                         /* in order to check for run on fly but diagnostic output hasn't to report motor speed */
      DIAGNOSTIC_TOGGLE_PORT;                                                                 /* but error causing wait state jump into. In this case:                               */
    }                                                                                         /* don't toggle diagnostic.                                                            */
    else
    {
      Nop(); /* Misra roules */
    }
  #endif  /* DIAGNOSTIC_STAND_ALONE */

  if ( (BitTest((PHASE_RESET), u8MTCStatus)) != 0u )                                          /* if(u8MTCStatus & PHASE_RESET)                                                              */
  {                                                                                           /*                                                                                            */
    u16Phase = u16PhaseBuff;  /*(u32)(((u32)u16PhaseBuff)<<16);*/                             /* set u32Phase value basing on u16PhaseBuff value computed into MTC_ComputeStartOnFlyPhase!! */
    BitClear((PHASE_RESET), u8MTCStatus);                                                     /* At this point, Tacho_ManageTachoCapture function is called.                                */
  }
  else
  {
    Nop(); /*PRQA S 1006 #Necessary assembly code*/
  }

  if ( pstrElectricFreqBuff_Index < &bufstrElectricFreqBuff[SPEED_FIFO_SIZE-1u])
  {
    pstrElectricFreqBuff_Index++;
  }
  else
  {
    pstrElectricFreqBuff_Index = (PElectricFreq_s)bufstrElectricFreqBuff;
  } 

  BitSet((MANAGE_TACHO), C_D_ISR_Status);

  /*INTERRUPT_TACHO_CLEAR;*/ /*Clear capture flag */
}
#endif  /* DUAL_LINE_ZERO_CROSSING */
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
