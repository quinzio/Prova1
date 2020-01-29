/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  CUR1_Current.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file CUR1_Current.h
    \version see \ref FMW_VERSION 
    \brief Basic current functions prototypes.
    \details Routines for current operation and variables related initialization.\n
    \note none
    \warning none
*/

#ifndef CURR_H
#define CURR_H  

#include "SpalTypes.h"
#include "SpalBaseSystem.h"

/* Public Constants -------------------------------------------------------- */
	
/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
BOOL Curr_AutoTestIBus(void);
BOOL Curr_AutoTestIBatt(void);
#ifdef ENABLE_RS232_DEBUG
u16  Curr_GetOffsetCurrent(void);
#endif
u16  Curr_GetPeakCurrent(void);
u16  Curr_GetCurrent(void);
u16  Curr_GetBattCurrent(void);
u16  Curr_GetIparam(void);
void Curr_SetDefaultParamI (void);

#endif  /* CURR_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
