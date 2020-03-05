/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputAN.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INP1_InputAN.h
    \version see \ref FMW_VERSION 
    \brief Basic analog input function prototypes.
    \details Routines for pwm input operation and variables related managing.\n
    \note none
    \warning none
*/


#ifndef INPUT_AN_H
#define INPUT_AN_H  

#ifdef AN_INPUT

/* Public Constants -------------------------------------------------------- */

#if(((defined(POTENTIOMETER))||(defined(NTC)))&&(!defined(PWM_TO_ANALOG_IN)))
#define ANALOG_PWM_EDGE_ON_AN_BIT      (u8)2
#define ANALOG_PWM_EDGE_ON_AN          ((u8)((u8)1<<ANALOG_PWM_EDGE_ON_AN_BIT))
#endif

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */
#if(defined(AN_INPUT) && !(defined(PWM_TO_ANALOG_IN)))
extern u8 AnalogStatus;
#endif
/* Public Functions prototypes --------------------------------------------- */
void InputAN_InitANInput(void);

#ifdef NTC
u8 InputAN_GetNTCSetPoint(void);
#endif

#ifdef POTENTIOMETER
u8 InputAN_GetAnalogSetPoint(void);
#endif

#if defined(PWM_TO_ANALOG_IN)
  u8 InputAN_GetPwmToAnalogSetPoint(void);
  u16  InputAN_GetPWMToANparam(void);  
  void InputAN_SetDefaultPWMToANparam(void); 
#endif // PWM_TO_ANALOG_IN
BOOL InputAN_PwmDisappeared(void);

#ifndef PWM_TO_ANALOG_IN
void InputAN_CheckTestPhase(void);
#endif

u8 InputAN_GetANparam(void);
void InputAN_SetDefaultANparam(void);

#endif  // INPUT_AN_H

#endif  //AN_INPUT

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
