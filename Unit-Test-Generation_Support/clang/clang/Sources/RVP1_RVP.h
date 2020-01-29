/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  RVP1_RVP.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file RVP1_RVP.h
    \version see \ref FMW_VERSION 
    \brief Funzioni di gestione del MOS di RVP..
    \details none
    \note none
    \warning none
*/

#ifndef RVP_H
#define RVP_H


#ifdef RVP_SW_MANAGEMENT_ENABLE
/* --------------------------- Private Defines ----------------------------- */
    
/* --------------------------- Public Variables ----------------------------- */
 
/* --------------------------- Public Functions prototypes ------------------ */
void RVP_InitRVP (void);
void RVP_SetRVPOn (void);
void RVP_SetRVPOff (void);
BOOL RVP_IsRVPOn (void);

#endif /* RVP_SW_MANAGEMENT_ENABLE */

#endif /* RVP_H */
