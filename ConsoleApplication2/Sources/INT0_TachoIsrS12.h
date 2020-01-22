/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_TachoIsrS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_TachoIsrS12.h
    \version see \ref FMW_VERSION 
    \brief Tacho interrupt routines prototypes.
    \details These routines manage tacho interrupts.\n
    \note none
    \warning none
*/

#ifndef INT0_TACHOIsrS12_H
#define INT0_TACHOIsrS12_H  


/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */
 
/* Public Functions prototypes --------------------------------------------- */
__interrupt void INT0_TachoOverflow(void);
__interrupt void INT0_TachoCapture(void);
#ifdef DUAL_LINE_ZERO_CROSSING
__interrupt void INT0_TachoBemfCapture(void);
#endif  /* DUAL_LINE_ZERO_CROSSING */

#endif  /* INT0_TACHOIsrS12_H */
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
