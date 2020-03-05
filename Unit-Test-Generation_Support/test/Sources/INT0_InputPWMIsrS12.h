/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_InputPWMIsrS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_InputPWMIsrS12.h
    \version see \ref FMW_VERSION 
    \brief Real Time Interrupt routines.
    \details This routine manages PWM input capture interrupt.\n
    \note none
    \warning none
*/

#ifndef INT0_InputPWMIsrS12_H
#define INT0_InputPWMIsrS12_H


/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

#pragma DATA_SEG DEFAULT

/* Public Functions prototypes --------------------------------------------- */
#if (( defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN) ) && (!defined(LIN_INPUT)))
__interrupt void INT0_InputPWM_PWMInputISR(void);
void INT0_InputPWM_Set_u8PositiveLevelCount(u8 u8Value);
#endif  /* PWM_INPUT */

#endif  /* INT0_InputPWMIsrS12_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
