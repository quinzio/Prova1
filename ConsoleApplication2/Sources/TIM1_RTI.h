/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  TIM1_RTI.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file TIM1_RTI.h
    \version see \ref FMW_VERSION 
    \brief Basic time base function prototypes.
    \details Routines for time base operation and variables related managing.\n
    \note none
    \warning none
*/

#ifndef RTI_H
#define RTI_H


/* Public Constants -------------------------------------------------------- */
/* Timebase */
#define TIMEBASE_MSEC   (u8)8 /* Main timebase milliseconds */

/* Control Timing */
#define	REG_LOOP_TIME_MSEC        (u8)TIMEBASE_MSEC                            /* Close loop regulation: 1*8 ms = 8 ms */
#define	REG_LOOP_TIME_SEC         (float32_t)((float32_t)REG_LOOP_TIME_MSEC/(float32_t)1000)
#define REG_LOOP_TIME             ((u8)(REG_LOOP_TIME_MSEC/TIMEBASE_MSEC))
#define RTI_PERIOD_MSEC           2u                                           /* Real time interrupt period (WARNING: max 16msec) */

#define UPDATE_LOOP_TIME          (float32_t)0.0025                            /* [s] */
#define UPDATE_LOOP_FREQ          (float32_t)((float32_t)1/UPDATE_LOOP_TIME)   /* [Hz] */
#define BASE_INTERRUPT_STEP       (float32_t)0.00005                           /* [s] */
#define UPDATE_LOOP_TIME_PU       (u16)((float32_t)UPDATE_LOOP_TIME/BASE_INTERRUPT_STEP)

/**
    \def TIMEOUT_ELAPSED
    \brief
    \details Flag setted when the main loop is ended and a new main loop timer is stored

    \def REG_PERIOD_ELAPSED
    \brief
    \details

    \def SEQUENCE_COMPLETED	
    \brief
    \details

    \def MAIN_TIMEBASE_ELAPSED
    \brief
    \details

    \def TIMEOUT_LOCK
    \brief Interrupt protection for Timeout
    \details Used to protect operation from interrupt

    \def MAIN_TIME_BASE_LOCK
    \brief Interrupt protection for main Time Base
    \details Used to protect operation from interrupt
*/

/* TimeBaseStatus flags description */
#define TIMEOUT_ELAPSED              ((u8)0x01)
#define REG_PERIOD_ELAPSED           ((u8)0x02)
#define MAIN_TIMEBASE_ELAPSED        ((u8)0x10)
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#define DIAG_TIMEBASE_ELAPSED        ((u8)0x40)
#endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */
#define LOGTIMER_ELAPSED             ((u8)0x80)

/* LockStatus flag */
#define TIMEOUT_LOCK                 ((u8)0x01)
#define LOGDATI_LOCK                 ((u8)0x04)
#define MAIN_TIME_BASE_LOCK          ((u8)0x08)

#define RTI_FAILSAFE_CNT_MAX  (u16)1000 /* Max counts between two consecutive INT0_isrVrti isr */ 

/* Public Variables -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void RTI_Init(void);
void RTI_SetTimeOutDuration(u16 u16Duration);
BOOL RTI_IsTimeOutElapsed(void);
void RTI_SetSpeedRegPeriod(u8 u8Period);
BOOL RTI_IsRegPeriodElapsed(void);
void RTI_SetMainTimeBase(u8 u8Period);
BOOL RTI_IsMainTimeElapsed(void);
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
  void RTI_SetDiagShortDuration(u8 u8Period);
  BOOL RTI_IsDiagShortElapsed(void);
#endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */
void RTI_CheckCPSTimeBase(void);
BOOL RTI_CPSIsReadyToCheck(void);
void RTI_ClearCPSTimeBase(void);  

BOOL RTI_IsLogDatiElapsed(void);
void RTI_SetDatiLogPeriod(u16 u16Period);

#endif	/* RTI_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
