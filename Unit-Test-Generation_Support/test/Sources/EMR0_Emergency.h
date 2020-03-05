/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  EMR0_Emergency.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file EMR0_Emergency.h
    \version see \ref FMW_VERSION 
    \brief Emergency routines prototypes for secure bridge turn off.
    \details Inside routines have to be used to secure turn off three phase 
    bridge and to turn off micro.\n
    \note none
    \warning none
*/

#ifndef EMR_H
#define EMR_H  


/* Public Constants -------------------------------------------------------- */
#define BALAN_BRAKING_TIME_SEC            (float32_t)4
#define	BALAN_BRAKING_TIME                (u16)((float32_t)BALAN_BRAKING_TIME_SEC/REG_LOOP_TIME_SEC)             
#define BALAN_DECREASING_SPEED_TIME_SEC   (float32_t)4
#define	BALAN_DECREASING_SPEED_TIME       (u16)((float32_t)BALAN_DECREASING_SPEED_TIME_SEC/REG_LOOP_TIME_SEC)    
                                                                                                             
/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void EMR_EmergencyDisablePowerStage(void);
void EMR_SetStopMode(void);

#endif  /* EMR_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
