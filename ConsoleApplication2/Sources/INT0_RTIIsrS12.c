/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_RTIIsrS12.c

VERSION  :  1.4

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_RTIIsrS12.c
    \version see \ref FMW_VERSION 
    \brief Real Time Interrupt.
    \details These routines manage RTI interrupt.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif

#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF              /* needed for const def */
#include "TIM1_RTI.h"
#include "INT0_RTIIsrS12.h"
#include "INP1_InputSP.h"
#include "MAC0_RegisterS12.h"
#include "main.h"              /* Per strDebug, enKeyShort, u16KeyRefreshCount */
#include "TMP1_Temperature.h"  /* Per Temp_Get_u16TempStepInt() e Temp_Dec_u16TempStepInt() */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#pragma DATA_SEG SHORT _DATA_ZEROPAGE
    static BOOL RTI_failsafe_flg;
    static volatile u16 RTI_failsafe_cnt;
#pragma DATA_SEG DEFAULT

    /** 
        \var BOOLUpdateVbattFilt
        \brief Used as time base flag for battery voltage filtering.\n
        \details Set every time base.\n
        \note none.
        \warning none. 
    */
    #if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
     static BOOL BOOLUpdateVbattFilt;
    #endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */

    #ifdef KEY_SHORT_LOGIC
     static u16 u16KeyRefreshCount;   /* Counter to refresh the key-short detection */
     static enumKeyShort enKeyShort;  /* Key-short logic state machine              */
    #endif  /* KEY_SHORT_LOGIC */

    /** 
        \var u8LockStatus
        \brief Used for time base lock status to avoid bad counter updating.\n
        \details Used for time base lock status to avoid bad counter updating.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC.
    */
    static u8 u8LockStatus;

    /** 
        \var BoolCPSTimeBase
        \brief Used for CheckPowerStage timing.\n
        \details Used for time base re-load value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static BOOL BoolCPSTimeBase;

    /** 
        \var u8CounterMillisecond
        \brief Count variable to determine the time base.\n
        \details Count variable to determine the time base.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static u8 u8CounterMillisecond;

    /** 
        \var u8RegCounter
        \brief Used for time base actual counter value.\n
        \details  Used for time base actual counter value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC.
    */
    static u8 u8RegCounter;

    /** 
        \var u8RegTimeBase
        \brief Used for time base re-load value.\n
        \details Used for time base re-load value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC.
    */
    static u8 u8RegTimeBase;

    /** 
        \var u16TimeOutCounter
        \brief Used for time base actual counter value.\n
        \details Used for time base actual counter value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC.
    */
    static u16 u16TimeOutCounter;

    /** 
        \var u16TimeOutPeriod
        \brief Used for time base re-load value.\n
        \details Used for time base re-load value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static u16 u16TimeOutPeriod;

    /** 
        \var u8TempFiltTimer
        \brief Used as timer for temperature filter.\n
        \details Used as timer for temperature filter.\n
        \note none.
        \warning none. 
    */
    static u8 u8TempFiltTimer;         /* Timer per filtro su temperatura */

    /** 
        \var u16MainCounter
        \brief Used for time base actual counter value.\n
        \details Used for time base actual counter value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static u16 u16MainCounter;

    /** 
        \var u16MainTimeBase
        \brief Used for time base re-load value.\n
        \details Used for time base re-load value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static u16 u16MainTimeBase;

    /** 
        \var u16LogDatiTimer
        \brief Used to call DataLog update.\n
        \details 
        \note 
        \warning This variable is \b STATIC. 
    */
    static u16 u16LogDatiTimer;

    /** 
        \var u16LogDatiPeriod
        \brief Used for log dati timer.\n
        \details Used for log dati timer.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static u16 u16LogDatiPeriod;

    #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION

    /** 
        \var u8DiagShortCounter
        \brief Used for time base actual counter value.\n
        \details Used for time base actual counter value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static u8 u8DiagShortCounter;

    /** 
        \var u8DiagShortPeriod
        \brief Used for time base re-load value.\n
        \details Used for time base re-load value.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC. 
    */
    static u8 u8DiagShortPeriod;

    #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */

    /** 
        \var u8TimeBaseStatus
        \brief Used for time base counter.\n
        \details Used for time base counter.\n
        \note This variable is placed in \b DEFAULT segment.
        \warning This variable is \b STATIC.
    */
    static u8 u8TimeBaseStatus;


/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8TimeBaseStatus(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8TimeBaseStatus
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8TimeBaseStatus(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8TimeBaseStatus = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 INT0_Get_u8TimeBaseStatus(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of the static variable u8TimeBaseStatus
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
u8 INT0_Get_u8TimeBaseStatus(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return(u8TimeBaseStatus);
}

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8DiagShortCounter(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8DiagShortCounter
                            \par Used Variables
    \ref u8DiagShortCounter \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8DiagShortCounter(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8DiagShortCounter = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8DiagShortPeriod(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8DiagShortPeriod
                            \par Used Variables
    \ref u8DiagShortPeriod \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8DiagShortPeriod(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8DiagShortPeriod = u8Value;
}
#endif

#ifdef LOG_DATI
/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16LogDatiPeriod(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16LogDatiPeriod
                            \par Used Variables
    \ref u16LogDatiPeriod \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16LogDatiPeriod(u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u16LogDatiPeriod = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16LogDatiTimer(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16LogDatiTimer
                            \par Used Variables
    \ref u16LogDatiTimer \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16LogDatiTimer(u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u16LogDatiTimer = u16Value;
}
#endif

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16MainTimeBase(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16MainTimeBase
                            \par Used Variables
    \ref u16MainTimeBase \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16MainTimeBase(u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u16MainTimeBase = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16MainCounter(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16MainCounter
                            \par Used Variables
    \ref u16MainCounter \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16MainCounter(u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u16MainCounter = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8TempFiltTimer(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8TempFiltTimer
                            \par Used Variables
    \ref u8TempFiltTimer \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8TempFiltTimer(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8TempFiltTimer = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 INT0_Get_u8TempFiltTimer(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of the static variable u8TempFiltTimer
                            \par Used Variables
    \ref u8TempFiltTimer \n
    \return \b The value of the static variable u8TempFiltTimer.
    \note None.
    \warning None.
*/
u8 INT0_Get_u8TempFiltTimer(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return(u8TempFiltTimer);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16TimeOutPeriod(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16TimeOutPeriod
                            \par Used Variables
    \ref u16TimeOutPeriod \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16TimeOutPeriod(u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u16TimeOutPeriod = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16TimeOutCounter(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16TimeOutCounter
                            \par Used Variables
    \ref u16TimeOutCounter \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16TimeOutCounter(u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u16TimeOutCounter = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8RegTimeBase(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8RegTimeBase
                            \par Used Variables
    \ref u8RegTimeBase \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8RegTimeBase(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8RegTimeBase = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8RegCounter(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8RegCounter
                            \par Used Variables
    \ref u8RegCounter \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8RegCounter(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8RegCounter = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8CounterMillisecond(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8CounterMillisecond
                            \par Used Variables
    \ref u8CounterMillisecond \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8CounterMillisecond(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8CounterMillisecond = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn BOOL INT0_Get_BoolCPSTimeBase(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of the static variable BoolCPSTimeBase
                            \par Used Variables
    \ref BoolCPSTimeBase \n
    \return \b The value of the static variable BoolCPSTimeBase.
    \note None.
    \warning None.
*/
BOOL INT0_Get_BoolCPSTimeBase(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (BoolCPSTimeBase);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_BoolCPSTimeBase(BOOL value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable BoolCPSTimeBase
                            \par Used Variables
    \ref BoolCPSTimeBase \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_BoolCPSTimeBase(BOOL value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    BoolCPSTimeBase = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 INT0_Get_u8LockStatus(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of the static variable u8LockStatus
                            \par Used Variables
    \ref u8LockStatus \n
    \return \b The value of the static variable u8LockStatus.
    \note None.
    \warning None.
*/
u8 INT0_Get_u8LockStatus(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (u8LockStatus);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u8LockStatus(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u8LockStatus
                            \par Used Variables
    \ref u8LockStatus \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u8LockStatus(u8 u8Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u8LockStatus = u8Value;
}

#if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
/*-----------------------------------------------------------------------------*/
/**
    \fn BOOL INT0_Get_UpdateVbattFilt(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Get BOOLUpdateVbattFilt flag for battery voltage filtering   
                            \par Used Variables
    \ref BOOLUpdateVbattFilt \n
    \return \b the flag for battery voltage filtering.
    \note None.
    \warning None.
*/
BOOL INT0_Get_UpdateVbattFilt(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (BOOLUpdateVbattFilt);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_UpdateVbattFilt(BOOL BOOLValue)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Set BOOLUpdateVbattFilt flag for battery voltage filtering  
                            \par Used Variables
    \ref BOOLUpdateVbattFilt \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_UpdateVbattFilt(BOOL BOOLValue) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    BOOLUpdateVbattFilt = BOOLValue;
}
#endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */

#ifdef KEY_SHORT_LOGIC
/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16KeyRefreshCount(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static variable u16KeyRefreshCount
                            \par Used Variables
    \ref u16KeyRefreshCount \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16KeyRefreshCount(u16 u16Value)
{
    u16KeyRefreshCount = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u16 INT0_Get_u16KeyRefreshCount(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static variable u16KeyRefreshCount
                            \par Used Variables
    \ref u16KeyRefreshCount \n
    \return \b The value of static variable u16KeyRefreshCount.
    \note None.
    \warning None.
*/
u16 INT0_Get_u16KeyRefreshCount(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return(u16KeyRefreshCount);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_enKeyShort(enumKeyShort value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static variable enKeyShort
                            \par Used Variables
    \ref enKeyShort \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_enKeyShort(enumKeyShort value)
{
    enKeyShort = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn enumKeyShort INT0_Get_enKeyShort(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static variable enKeyShort
                            \par Used Variables
    \ref enKeyShort \n
    \return \b The value of static variable enKeyShort.
    \note None.
    \warning None.
*/
enumKeyShort INT0_Get_enKeyShort(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return(enKeyShort);
}
#endif

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_FailSafeMonitor( void )

    \author	Ruosi Gabriele  \n <em>Reviewer</em> 

    \date 21/02/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief

    \par No variables used
    \return \b void No value returned from this function
    \note none
    \warning none
*/

void INT0_FailSafeMonitor(void)
{
  /*DEBUG2_ON;*/

  if(RTI_failsafe_flg == (BOOL)FALSE)
  {
    RTI_failsafe_cnt++;
  }
  else
  {
    RTI_failsafe_flg = (BOOL)FALSE;
    RTI_failsafe_cnt = (u16)0;
  }

  if(RTI_failsafe_cnt >= RTI_FAILSAFE_CNT_MAX)
  {
    #ifndef _CANTATA_
    for(;;){}                       /* Never ending loop that causes WTD reset */
    #endif /* _CANTATA_ */
  }
  else
  {
    /*Nop();*/
  }

  /*DEBUG2_OFF;*/
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_FailSafeMonitor_rst( void )

    \author	Ruosi Gabriele  \n <em>Reviewer</em> 

    \date 21/02/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief

    \par No variables used
    \return \b void No value returned from this function
    \note none
    \warning none
*/

void INT0_FailSafeMonitor_rst(void)
{
  RTI_failsafe_flg = (BOOL)FALSE;
  RTI_failsafe_cnt = (u16)0;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_isrVrti(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Manages RTI interrupt every 2ms..  
    \details Decrement related time base timer variables and if they reach zero value
    it turns on related flag.\n
                            \par Used Variables
	  \ref u8CounterMillisecond	description\n
	  \ref u8TimeBaseStatus	description\n
	  \ref u8RegCounter	description\n
	  \ref u16MainCounter	description\n
	  \ref u16LogDatiTimer description\n
	  \ref u8DiagShortCounter description\n 
	  \ref u8LockStatus description\n
	  \ref u8NumNoDuty description\n
	  \ref BOOLUpdateVbattFilt description\n
	  \ref u8TempFiltTimer description\n  
	  \ref BoolCPSTimeBase description\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
#define RTI_LEGACY_PERIOD    (TIMEBASE_MSEC/RTI_PERIOD_MSEC)     /* 8msec / RTI_PERIOD */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_isrVrti(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Manages RTI interrupt every 2ms..  
    \details Decrement related time base timer variables and if they reach zero value
    it turns on related flag.\n
                            \par Used Variables
	  \ref u8CounterMillisecond	description\n
	  \ref u8TimeBaseStatus	description\n
	  \ref u8NumNoDuty description\n
	  \ref BOOLUpdateVbattFilt description\n
	  \ref u8TempFiltTimer description\n  
	  \ref BoolCPSTimeBase description\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
#define RTI_LEGACY_PERIOD    (TIMEBASE_MSEC/RTI_PERIOD_MSEC)     /* 8msec / RTI_PERIOD */

__interrupt void INT0_isrVrti(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  #ifdef LIN_INPUT
    LIN_ManageLinTimebase();
  #endif  /* LIN_INPUT */

  /*PORTE_PE0 = 1;*/ /* Only for debug */

  ++u8CounterMillisecond;

  if( u8CounterMillisecond >= RTI_LEGACY_PERIOD )
  {
    u8CounterMillisecond = 0;

    u8RegCounter--;
    if (u8RegCounter == 0u)
    {
      u8TimeBaseStatus |= REG_PERIOD_ELAPSED;                       /* Used in IsTestTimeBaseElapsed 8ms. */
      u8RegCounter = u8RegTimeBase;                                 /* Continue with same period          */
    }

    if ( (u8LockStatus & MAIN_TIME_BASE_LOCK) == 0u)                /* No Main Time base modification on-going */
    {
      u16MainCounter--;
      if (u16MainCounter == 0u)
      {
        u8TimeBaseStatus |= MAIN_TIMEBASE_ELAPSED;                  /* Used in TimeBase_IsMainTimeElapsed */
        u16MainCounter = u16MainTimeBase;                           /* Continue with same period          */
      }
    }

    if ( (u8LockStatus & TIMEOUT_LOCK) == 0u)	                    /* No TimeOut modification on-going */
    {
      u16TimeOutCounter--;
      if (u16TimeOutCounter == 0u)
      {
        u8TimeBaseStatus |= TIMEOUT_ELAPSED;                        /* Used in TimeBase_IsTimeOutElapsed */
        u16TimeOutCounter = u16TimeOutPeriod;                       /* Reload programmed period          */
      }
      else
      {
        u8TimeBaseStatus &= (u8)(~TIMEOUT_ELAPSED);
      }
    }

    #ifdef LOG_DATI
    if ( (u8LockStatus & LOGDATI_LOCK) == 0u)                       /* No Sequence modification on-going */
    {
      u16LogDatiTimer--;
      if (u16LogDatiTimer == 0u)
      {
        u8TimeBaseStatus |= LOGTIMER_ELAPSED;                       /* Used in RTI_IsLogDatiElapsed */
        u16LogDatiTimer = u16LogDatiPeriod;                         /* Reload programmed period     */
      }
      else
      {
        u8TimeBaseStatus &= (u8)(~LOGTIMER_ELAPSED);
      }
    }
    #endif /* LOG_DATI */

    #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
    u8DiagShortCounter--;
    if (u8DiagShortCounter == (u8)0)
    {
      u8TimeBaseStatus |= DIAG_TIMEBASE_ELAPSED;                    /* Used in TimeBase_IsTimeOutElapsed */
      u8DiagShortCounter = u8DiagShortPeriod;                       /* Reload programmed period          */
    }
    else
    {
      u8TimeBaseStatus &= (u8)(~DIAG_TIMEBASE_ELAPSED);
    }
    #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */

    #if ( ( DIAGNOSTIC_STATE == DIAGNOSTIC_ENABLE ) && defined(KEY_SHORT_LOGIC) )  /* If diagnostic is enabeld together with the key-short detection logic... */
      if( (BOOL)DIAGNOSTIC_PORT_BIT == (BOOL)FALSE )                               /* ...if diagnostic line is active...                                      */
      {
        u8NumNoDuty++;                                                             /* ...increment u8NumNoDuty ONLY when no PWM input is present. */
      }
      else
      {
        /* Nop(); */
      }  
    #else
      u8NumNoDuty++;
    #endif

    if ( u8NumNoDuty > PWM_TO_QUIESCENT_NORMAL_OP_NUM )
    {
      u8NumNoDuty = PWM_TO_QUIESCENT_NORMAL_OP_NUM; 
    }
    else
    {
      /* Nop(); */
    }

    #if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
      BOOLUpdateVbattFilt = TRUE; 
    #endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */

    /* Timer per filtro su temperatura */
    if ( u8TempFiltTimer != 0u )
    {
      u8TempFiltTimer--;
    }
    else
    {
      /* Nop(); */
    }

    #ifdef KEY_SHORT_LOGIC  
      if ( enKeyShort == REFRESH )
      {
        if ( u16KeyRefreshCount > (u16)0 )
        {
          u16KeyRefreshCount--;
        }
        else
        {
          /* Nop(); */
        }
      }
      else
      {
        /* Nop(); */
      }
    #endif  /* KEY_SHORT_LOGIC */

    if ( (u16)Temp_Get_u16TempStepInt() > (u16)0 )
    {
      (void)Temp_Dec_u16TempStepInt();
    }
    else
    {
      /* Nop(); */
    }

    BoolCPSTimeBase = TRUE;
  }
  /* u8CPSTimeBase Is time base for checkpowerstage controls. */

  RTI_failsafe_flg = (BOOL)TRUE; /* OK -> INT0_isrVrti is running */

  RTI_ACK;
  /*PORTE_PE0 = 0;*/ /* Only for debug */
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
