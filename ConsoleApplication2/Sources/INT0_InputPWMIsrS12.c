/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_InputPWMIsrS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_InputPWMIsrS12.c
    \version see \ref FMW_VERSION 
    \brief Input PWM Interrupt.
    \details These routines manage PWM input capture interrupt.\n
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
#include "main.h"           /* Per strDebug */
#include "MCN1_acmotor.h"   /* it has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"       /*Public motor control peripheral function prototypes*/ 
#include "TIM1_RTI.h"       /*Public general purpose Timebase function prototypes*/    
#include MAC0_Register      /* Needed for macro definitions */
#include "INP1_InputSP.h"
#include "INP1_InputPWM.h"
#include "INP1_Key.h"
#include "EMR0_Emergency.h"
#include "SOA1_SOA.h"
#include "WTD1_Watchdog.h"
#include "INP1_InputAN.h"   /* For InputAN_GetPwmToAnalogSetPoint prototype */
#include "INT0_RTIIsrS12.h"
#include "INT0_InputPWMIsrS12.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif

#if (( defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN) ) && (!defined(LIN_INPUT)))

/* -------------------------- Private variables --------------------------- */
#pragma DATA_SEG DEFAULT

  #ifdef KEY_SHORT_LOGIC
    static u8 u8PositiveLevelCount;    /* Counter of PWM-input positive edges. */
  #endif  /* KEY_SHORT_LOGIC */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_InputPWM_PWMInputISR(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Manages pwm input capture interrupt .  
    \details This function performs:\n
    - Reset of \ref u8NumNoDuty.\n
    - Save current timer module (TIM) value into current space of \ref bufstrPWMInData[u8PWMindex].Val.\n
    - Manages overflow vs capture priority interrupt.\n
    - Save current timer module (TIM) oveflow value into current space of \ref bufstrPWMInData[u8PWMindex].Ovf.\n
    - Reset of \ref u8NumOvfPWM.\n
    - Set new edge sensitive.\n
    
                            \par Used Variables
    \ref u8NumNoDuty. Used to manage capture vs overflow interrupt since these two interrupt have they own priority\n
    \ref u8NumOvfPWM. Used to manage timer overflow interrupt and to compute pwm input frequency and duty cycle.\n
    \ref u8PWMindex. Index of \ref bufstrPWMInData[DIM_BUFFER_PWM_IN].\n
    \ref bufstrPWMInData. Buffer of data structure used to calculate Input Set Point value.\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
__interrupt void INT0_InputPWM_PWMInputISR(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 tmp_INPUTPWM_DETECTING_EDGE_FLAG;
  u16 tmp_INPUTPWM_PIO_READING;  

  #if ( ( DIAGNOSTIC_STATE == DIAGNOSTIC_ENABLE ) && defined(KEY_SHORT_LOGIC) ) /* If diagnostic is enabeld together with the key-short detection logic... */
    if( (BOOL)DIAGNOSTIC_PORT_BIT == (BOOL)FALSE )                              /* ...and the diagnostic line is active... */
    {
      u8PositiveLevelCount++;                                                   /* ...increment the counter of positive edges. */
      if( u8PositiveLevelCount >= MAXIMUM_POSTIVE_LEVEL_COUNT )                 /* If such counter overcomes MAXIMUM_POSTIVE_LEVEL_COUNT (i.e. 2 samples)... */
      {
        u8PositiveLevelCount = 0;                                               /* ...reset its value... */
        u8NumNoDuty = 0;                                                        /* ...and then reset the the counter of no PWM-input events. */
      }                                                                         /* N.B.: these instructions avoid to reset u8NumNoDuty when a SINGLE positive edge occurs, */
      else                                                                      /* but no pwm is present; thus, PWM line remains high. Such a condition may happen */
      {                                                                         /* when the diagnostic line is active (for istance, with a PARTIAL STALL notification). */
        /* Nop(); */
      }
    }
    else
    {
      u8PositiveLevelCount = 0;
    }
  #else
    u8NumNoDuty = 0;
  #endif  /* KEY_SHORT_LOGIC  */

  bufstrPWMInData[u8PWMindex].u16Val = INPUTPWM_CAPTURE_REGISTER;   /* N.B.*/
                                                                    /* Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare*/
                                                                    /* channel (0x0010 - 0x001F) will cause the corresponding channel flag CxF to be cleared FOR S12 micro!!!*/

  if(INPUTPWM_COUNT_OVF == ON)                                      /* Here I don't menage u8TachoOverflowNumber since Tacho interrupt  */
                                                                    /* has higher priority over INT0_InputPWM_PWMInputISR and would have been called in */
                                                                    /* advance instead of this routine. */
  {
    if((bufstrPWMInData[u8PWMindex].u16Val & DISCRIMINATION_OVERFLOW_PWM_IN) == 0u)
    {
      u8NumOvfPWM++;
      boolOVFVsPWMInput = TRUE;
     } 
     else
     {
      boolOVFVsPWMInput = FALSE;
     }
  }

  bufstrPWMInData[u8PWMindex].u8Ovf = u8NumOvfPWM;
  u8NumOvfPWM = 0;

  u8PWMindex++;
  if (u8PWMindex == DIM_BUFFER_PWM_IN )
  {
    /* PWMInFlag.READY_CALC=TRUE; */
    u8PWMindex=0;
  }
  
  if ((u8PWMindex & (u8)0x01) != 0u)
  {
    INPUTPWM_SET_RISING_EDGE_SENS;
    #if defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN)
    PWMInFlag.READY_CALC=TRUE;
    #endif
  }
  else
  {
    INPUTPWM_SET_FALLING_EDGE_SENS;
  }

  tmp_INPUTPWM_DETECTING_EDGE_FLAG = INPUTPWM_DETECTING_EDGE_FLAG;
  tmp_INPUTPWM_PIO_READING         = INPUTPWM_PIO_READING;

  if (tmp_INPUTPWM_DETECTING_EDGE_FLAG == tmp_INPUTPWM_PIO_READING) /* Questo per evitare che il fronte successivo avvenga mentre */
  {                                                                 /* e' ancora in corso questo interrupt */
#ifndef _CANTATA_
    bufstrPWMInData[u8PWMindex].u16Val = INPUTPWM_CAPTURE_REGISTER;
#else
    bufstrPWMInData[u8PWMindex].u16Val = set_TC3();
#endif
    if ((u8PWMindex != 0u) && (bufstrPWMInData[u8PWMindex].u16Val < (bufstrPWMInData[u8PWMindex-1u].u16Val)))
    {
      bufstrPWMInData[u8PWMindex].u8Ovf = 1;
    }
    else
    {
      bufstrPWMInData[u8PWMindex].u8Ovf = 0;
    }
    
    if ((u8PWMindex == 0u) && (bufstrPWMInData[0].u16Val < (bufstrPWMInData[DIM_BUFFER_PWM_IN-1u].u16Val)))
    {
      bufstrPWMInData[0].u8Ovf = 1;
    }
    else
    {
      bufstrPWMInData[0].u8Ovf = 0;
    }

    u8PWMindex++;
    if (u8PWMindex == DIM_BUFFER_PWM_IN )
    {
      /* PWMInFlag.READY_CALC=TRUE; */
      u8PWMindex=0;
    }

    if ((u8PWMindex & (u8)0x01) != 0u)
    {
      INPUTPWM_SET_RISING_EDGE_SENS;
      #if defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN)
      PWMInFlag.READY_CALC=TRUE;
      #endif
    }
    else
    {
      INPUTPWM_SET_FALLING_EDGE_SENS;
    }

    INPUTPWM_CLEAR_PENDING_INTERRUPT; /* Whilst S12 interrupt flag is already cleared before, S08 interrupt isr has to be cleared here!!! */
  }
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_InputPWM_Set_u8PositiveLevelCount(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static u8PositiveLevelCount variable
                            \par Used Variables
    \ref u8PositiveLevelCount \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_InputPWM_Set_u8PositiveLevelCount(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   u8PositiveLevelCount = u8Value;
}

#endif

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
