/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_Key.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifndef INP1_KEY_H
#define INP1_KEY_H

/** 
    \file INP1_Key.h
    \version see \ref FMW_VERSION 
    \brief Basic key function.
    \details Routines for key input operation and variables related managing.\n
    \note none
    \warning none
*/

#ifdef KEY_INPUT

/* Public Constants -------------------------------------------------------- */
#define MAX_KEY_OFF    (u8)4       /* After four consecutive key off turn off motor */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */
 
/* Public Functions prototypes --------------------------------------------- */
void INP1_InitKey(void);
BOOL INP1_KeyIsOff(void);

#endif  /* INP1_KEY_H */

#endif  /* KEY_INPUT */

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
