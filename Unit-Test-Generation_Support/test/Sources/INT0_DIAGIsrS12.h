/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_DIAGIsrS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_DIAGIsrS12.h
    \version see \ref FMW_VERSION 
    \brief Diagnostic interrupt routines prototypes.
    \details These routines manage diagnostic.\n
    \note none
    \warning none
*/

#ifndef DIA0_DIAGNOSTICIsrS12_H
#define DIA0_DIAGNOSTICIsrS12_H  

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG DEFAULT

/* Public Functions prototypes --------------------------------------------- */
void INT0_Set_BOOLDiagPeriodFlag (BOOL value);
BOOL INT0_Get_BOOLDiagPeriodFlag (void);
void INT0_Set_u16DiagnosticNextIsr (u16 u16Value);
u16  INT0_Get_u16DiagnosticNextIsr (void);
void INT0_Set_u16DiagPeriodCount (u16 u16Value);
void INT0_Set_u16DiagPeriodDuty (u16 u16Value);
__interrupt void INT0_DiagnosticIsr(void);

#endif

#endif  /* DIA0_DIAGNOSTICIsrS12_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
