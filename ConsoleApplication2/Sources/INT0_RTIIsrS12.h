/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_RTIIsrS12.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_RTIIsrS12.h
    \version see \ref FMW_VERSION 
    \brief Real Time Interrupt routines.
    \details These routines manage RTI interrupts.\n
    \note none
    \warning none
*/

#ifndef INT0_RTIIsrS12_H
#define INT0_RTIIsrS12_H  


/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */
#ifdef KEY_SHORT_LOGIC
  typedef enum
  {
    DISABLE,
    INIT,
    CHECK,
    REFRESH
  } enumKeyShort;
#endif  /* KEY_SHORT_LOGIC */

/* Public Variables -------------------------------------------------------- */

#pragma DATA_SEG DEFAULT

/* Public Functions prototypes --------------------------------------------- */
__interrupt void INT0_isrVrti(void);
void INT0_FailSafeMonitor(void);
void INT0_FailSafeMonitor_rst(void);
#ifdef KEY_SHORT_LOGIC
void INT0_Set_u16KeyRefreshCount(u16 u16Value);
u16  INT0_Get_u16KeyRefreshCount(void);
void INT0_Set_enKeyShort(enumKeyShort value);
enumKeyShort INT0_Get_enKeyShort(void);
#endif
#if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
  BOOL INT0_Get_UpdateVbattFilt(void);
  void INT0_Set_UpdateVbattFilt(BOOL BOOLValue);
#endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */
u8   INT0_Get_u8LockStatus(void);
void INT0_Set_u8LockStatus(u8 u8Value);
BOOL INT0_Get_BoolCPSTimeBase(void);
void INT0_Set_BoolCPSTimeBase(BOOL value);
void INT0_Set_u8CounterMillisecond(u8 u8Value);
void INT0_Set_u8RegCounter(u8 u8Value);
void INT0_Set_u8RegTimeBase(u8 u8Value);
void INT0_Set_u16TimeOutCounter(u16 u16Value);
void INT0_Set_u16TimeOutPeriod(u16 u16Value);
void INT0_Set_u8TempFiltTimer(u8 u8Value);
u8   INT0_Get_u8TempFiltTimer(void);
void INT0_Set_u16MainCounter(u16 u16Value);
void INT0_Set_u16MainTimeBase(u16 u16Value);

#ifdef LOG_DATI
void INT0_Set_u16LogDatiTimer(u16 u16Value);
void INT0_Set_u16LogDatiPeriod(u16 u16Value);
#endif

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
void INT0_Set_u8DiagShortCounter(u8 u8Value);
void INT0_Set_u8DiagShortPeriod(u8 u8Value);
#endif

void INT0_Set_u8TimeBaseStatus(u8 u8Value);
u8   INT0_Get_u8TimeBaseStatus(void);

#endif  /* INT0_RTIIsrS12_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/

