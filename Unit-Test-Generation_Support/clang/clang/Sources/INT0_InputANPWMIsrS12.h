/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_InputANPWMIsrS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_InputANPWMIsrS12.h
    \version see \ref FMW_VERSION 
    \brief Real Time Interrupt routines.
    \details This routine manages PWM input capture interrupt.\n
    \note none
    \warning none
*/

#ifndef INT0_InputANPWMIsrS12_H
#define INT0_InputANPWMIsrS12_H


#if(defined(AN_INPUT) && !(defined(PWM_TO_ANALOG_IN)))

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
__interrupt void INT0_InputAN_PWMInputISR(void);

#endif  //AN_INPUT

#endif  // INT0_InputANPWMIsrS12_H

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
