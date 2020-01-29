/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  POWER1_MaxPower.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file POWER1_MaxPower.h
    \version see \ref FMW_VERSION
    \brief Motion Control Routines prototypes
    \details In this files all the intermidiate motion control routines are regrouped.
    In line of principle all functions that are contained in this files have to drive Level 0 routines
    and will use and provides variables flor routines in \ref MCN1_acmotor.c 
*/

#ifndef MAXPOWER_H
#define MAXPOWER_H 


/* Private Constants -------------------------------------------------------- */
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  #define POWER_DERATING_ON_NUM      (u8)122  /* <=> 95 % */
  #define POWER_DERATING_OFF_NUM     (u8)124  /* <=> 97 % */
  #define POWER_DERATING_SHIFT       (u8)7
#endif  /* DIAGNOSTIC_STATE */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
/*void MaxPower_PowerRegInit(void);*/
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  void MaxPower_Set_BOOLPartStallDerFlag(BOOL value);
  BOOL MaxPower_Get_BOOLPartStallDerFlag(void);
#endif  /* DIAGNOSTIC_STATE */

void MaxPower_PowerSetPoint (void);
u32  MaxPower_GetPowerSetPoint (void);

#endif  /* MAXPOWER_H */
 
/*** (c) 2015 SPAL Automotive ****************** END OF FILE **************/
