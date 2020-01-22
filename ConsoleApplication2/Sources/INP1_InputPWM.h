/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputPWM.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifndef INPUT_PWM_H
#define INPUT_PWM_H

/** 
    \file INP1_InputPWM.h
    \version see \ref FMW_VERSION 
    \brief Basic pwm input function prototypes.
    \details Routines for pwm input operation and variables related managing.\n
    \note none
    \warning none
*/

#if ( defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN) )



/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/** 
    \typedef tPWMInFlag
    \brief Used for managing pwm input decoding. 
    \details It gathers flags telling pwm input decoding state..
*/

typedef struct
{
      unsigned int DUTYACQ               :1; /* !< Sto acquisendo duty o periodo? DUTYACQ==0->PERIODO DUTYACQ==1->DUTY                                                                                           */
      unsigned int READY_FIRST_DUTY      :1; /* !< E' pronto il valore iniziale del Duty? READY_FIRST_DUTY == 0 ->lettura inizio timer per duty READY_FIRST_DUTY == 1 ->lettura fine timer per duty              */
      unsigned int READY_FIRST_PERIOD    :1; /* !< E' pronto il valore iniziale del Periodo? READY_FIRST_PERIOD == 0 ->lettura inizio timer per periodo READY_FIRST_PERIOD == 1 ->lettura fine timer per periodo */
      unsigned int READY_CALC            :1; /* !< Sono pronti i valori per il calcolo del Duty? READY_CALC == 0 ->valori non pronti per il calcolo READY_CALC == 1 ->valori pronti per il calcolo               */
      unsigned int BUFFER_READY          :1; /* !< Abilita il polling dell'ingresso POLLING == 0 ->Valore di PWMin ottenuto dal timer POLLING == 1 ->valore di PWMin ottenuto dal polling                        */
      unsigned int SEND_EMERGENCY_STATUS :1; /* !< Segnala se e' pronto il valore del polling SEND_EMERGENCY_STATUS == 0 -> SEND_EMERGENCY_STATUS == 1 ->                                                        */
      unsigned int PWM_IN_ERROR          :1; /* !< Segnala se ci sono errori nella lettura dell'ingresso PWM_IN_ERROR == 0  --> Lettura OK PWM_IN_ERROR == 1  --> Errore nella lettura                           */
      unsigned int Bit7                  :1; /* !<  Not used                                                                                                                                                     */
}tPWMInFlag;

/* Public Variables -------------------------------------------------------- */

 #pragma DATA_SEG SHORT _DATA_ZEROPAGE
  extern tPWMInFlag PWMInFlag;

/* Public Functions prototypes --------------------------------------------- */
void InputPWM_InitPWMInput(void);
void InputPWM_FillBufferAndSetOperating(void);
#ifdef PWM_INPUT
BOOL InputPWM_PwmDisappeared(void);
#endif
u8 InputPWM_GetPWMInputSetPoint(void);

#endif  /* INPUT_PWM_H */

#endif  /* PWM_INPUT */
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
