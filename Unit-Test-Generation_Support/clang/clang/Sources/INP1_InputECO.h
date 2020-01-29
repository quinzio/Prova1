/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputECO.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifndef INPUT_ECO_H
#define INPUT_ECO_H

/** 
    \file INP1_InputECO.h
    \version see \ref FMW_VERSION 
    \brief Basic Economy input function prototypes.
    \details Routines for pwm input operation and variables related managing.\n
    \note none
    \warning none
*/


#ifdef ECONOMY_INPUT

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void InputECO_InitECOInput(void);
BOOL InputECO_IsEconomyOn(void);
u8   InputECO_GetEconomySetPoint(u8);
        
#endif // ECONOMY_INPUT
#endif /* INPUT_ECO_H */


/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
