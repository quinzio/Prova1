/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_InputANPWMIsrS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_InputANPWMIsrS12.c
    \version see \ref FMW_VERSION 
    \brief Input PWM Interrupt.
    \details These routines manage PWM input capture interrupt.\n
    \note none
    \warning none
*/

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register    // Needed for macro definitions
#include ADM0_ADC
#include "main.h"           // Per strDebug
#include "VBA1_Vbatt.h"
#include "MCN1_acmotor.h" // it has to stand before MCN1_mtc.h because of StartStatus_t definition
#include "MCN1_mtc.h"	  	// Needed for u16PWMCount
#include "INP1_InputSP.h"
#include "TIM1_RTI.h"		  // Public general purpose Timebase function prototypes
#include "FIR1_FIR.h"
#include "SOA1_SOA.h"
#include "TMP1_Temperature.h"
#include "INP1_InputAN.h"
#include "WTD1_Watchdog.h"
#include "INT0_InputANPWMIsrS12.h"

#if(defined(AN_INPUT) && !(defined(PWM_TO_ANALOG_IN)))

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_InputAN_PWMInputISR(void)
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
__interrupt void INT0_InputAN_PWMInputISR(void)
{
  u8NumNoDuty=0;

  bufstrPWMInData[u8PWMindex].u16Val = TIM_TIMER_COUNT;

  if(TIM_TIMER_OVF == ON)                                           // Here I don't menage u8TachoOverflowNumber since Tacho interrupt  
                                                                    // has higher priority over INT0_InputAN_PWMInputISR and would have been called in 
                                                                    // advance instead of this routine.
  {
    if((bufstrPWMInData[u8PWMindex].u16Val & DISCRIMINATION_OVERFLOW_PWM_IN) == (BOOL)FALSE)
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
    u8PWMindex=0;
  }
  
  if ((u8PWMindex & (u8)0x01) != (BOOL)FALSE)
  {
    INPUTAN_SET_RISING_EDGE_SENS;                               // Write compare value to ATDDRn result register. If result of conversion n is lower or same than compare value in ATDDRn, this is flagged in ATDSTAT2
  }
  else
  {
    INPUTAN_SET_FALLING_EDGE_SENS;                              // Write compare value to ATDDRn result register. If result of conversion n is lower or same than compare value in ATDDRn, this is flagged in ATDSTAT2
  }

  BitSet((ANALOG_PWM_EDGE_ON_AN), AnalogStatus);

  INPUTAN_CLEAR_PENDING_INTERRUPT;                              // A flag CCF[n] is cleared when: If AFFC=0, write “1” to CCF[n].
}

#endif  //AN_INPUT

/* END */

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/

