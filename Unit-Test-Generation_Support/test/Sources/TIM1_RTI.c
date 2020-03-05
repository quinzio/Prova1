/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  TIM1_RTI.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file TIM1_RTI.c
    \version see \ref FMW_VERSION 
    \brief Basic time base function.
    \details Routines for time base operation and variables related managing.\n
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
#include SPAL_DEF
#include "TIM1_RTI.h"       /* Public RTI function prototypes */
#include "INP1_InputSP.h"
#include "INP1_InputPWM.h"  /* Per u8NumPWMHigh, InputPWM_PwmInIsHigh() */
#include MAC0_Register      /* Needed for macro definitions */
#include "main.h"           /* Per strDebug, enKeyShort, u16KeyRefreshCount */
#include "LOG0_LogDataS12.h"
#include "RVP1_RVP.h"       /* Per u16RVPTimer */
#include "INT0_RTIIsrS12.h"

#ifdef LIN_INPUT
  #if defined(BMW_LIN_ENCODING)
    #include "COM1_LINUpdate_BMW.h"
  #elif defined(GM_LIN_ENCODING)
    #include "COM1_LINUpdate_GM.h"
  #else
    #error("Define LIN stack!!");
  #endif  /* BMW_LIN_ENCODING */
#endif  /* LIN_INPUT */

#ifdef LIN_BAL_MODE_PASSWORD
  #include "LINBalanceMode.h"
#endif  /* LIN_BAL_MODE_PASSWORD */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#pragma DATA_SEG DEFAULT

/*--------------- Private Variables declaration section ---------------------*/

/* ---------------------- Private variables ----------------------------------*/

/** 
    \var BoolCPSReadyToCheck
    \brief Used for CheckPowerStage timing.\n
    \details Used to allow CheckPowerStage checks after a 8-ms timebase period.\n
    \note This variable is placed in \b DEFAULT segment.
    \warning This variable is \b STATIC. 
*/
static BOOL BoolCPSReadyToCheck;

/* Private Functions prototypes --------------------------------------------- */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void RTI_Init(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initialization of base time counters.  
    \details Fill in time related variables counters to max its max value and clear
    interrupt.\n
                            \par Used Variables
	  \ref u8TimeBaseStatus	Initialized to 0;
	  \ref u8LockStatus Initialized to 0;
	  \ref u16TimeOutCounter	Initialized to U16_MAX;
		\ref u16TimeOutPeriod	Initialized to U16_MAX;
	  \ref u8RegCounter		Initialized to U8_MAX;
	  \ref u8RegTimeBase		Initialized to U8_MAX;
	  \ref u16MainTimeBase Initialized to U16_MAX;
	  \ref u16MainCounter Initialized to U16_MAX;
	  \ref u8DiagShortCounter Initialized to U8_MAX;
	  \ref u8DiagShortPeriod Initialized to U8_MAX;
	  \ref u16LogDatiTimer Initialized to U16LOGDATIPERIOD;
	  \ref u8CounterMillisecond Initialized to 0;
    \return \b void No value return.
    \note None.
    \warning None.
*/
void RTI_Init(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  INT0_Set_u8TimeBaseStatus(0);
  INT0_Set_u8LockStatus(0);
  
  INT0_Set_u16TimeOutCounter(U16_MAX);
  INT0_Set_u16TimeOutPeriod(U16_MAX);
  INT0_Set_u8RegCounter(U8_MAX);
  INT0_Set_u8RegTimeBase(U8_MAX);
  INT0_Set_u16MainTimeBase(U16_MAX);
  INT0_Set_u16MainCounter(U16_MAX);

  #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
    INT0_Set_u8DiagShortCounter(U8_MAX);
    INT0_Set_u8DiagShortPeriod(U8_MAX);
  #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */

  #ifdef LOG_DATI
    INT0_Set_u16LogDatiTimer(U16LOGDATIPERIOD);
  #endif

  #ifdef LIN_INPUT
    u16NumLinBusIdle = 0;
  #endif  /* LIN_INPUT */

  INT0_Set_u8CounterMillisecond(0);
}


/*-----------------------------------------------------------------------------*/
/**
    \fn void RTI_SetTimeOutDuration(u16 u16Duration)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Loads related timer base time value.  
    \details Sets the related time base timer value. It uses a lock flag to avoid
    bad timer updating if rti isr is being served. It resets all related time elapsed
    flags. The following operation are done in sequence:\n
    - Lock TimeOut handling in interrupt.\n
    - Save value autoreloaded when counter = 0.\n
    - Reset counter to new value.\n
    - Clear Flag. Unlock TimeOut handling in interrupt\n
    \param[in] u16Duration Value autoreloaded when counter = 0.\n
                            \par Used Variables
    \ref u8LockStatus \n
    \ref u16TimeOutPeriod \n
    \ref u16TimeOutCounter \n
    \ref u8TimeBaseStatus \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void RTI_SetTimeOutDuration(u16 u16Duration)
{
  u16 u16Duration_tmp;
  u8  u8LockStatus_tmp;

  u16Duration_tmp = u16Duration;

  if ( u16Duration_tmp == 0u)
  {
    u16Duration_tmp = 1u; /* Min value is 1ms */
  }

  u8LockStatus_tmp = INT0_Get_u8LockStatus() | TIMEOUT_LOCK ;
  INT0_Set_u8LockStatus(u8LockStatus_tmp);            /* Lock TimeOut handling in interrupt */

  INT0_Set_u16TimeOutPeriod(u16Duration_tmp);         /* Value autoreloaded when counter = 0 */
  INT0_Set_u16TimeOutCounter(u16Duration_tmp);        /* Reset counter to new value */


  u8LockStatus_tmp = INT0_Get_u8LockStatus() & (u8)(~TIMEOUT_LOCK) ;
  INT0_Set_u8LockStatus(u8LockStatus_tmp);            /* Unlock TimeOut handling in interrupt */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void RTI_SetSpeedRegPeriod(u8 u8Period)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Loads related timer base time value.  
    \details Sets the related time base timer value. It uses a lock flag to avoid
    bad timer updating if rti isr is being served. It resets all related time elapsed
    flags. The following operation are done in sequence:\n
    - Lock TimeOut handling in interrupt.\n
    - Save value autoreloaded when counter = 0.\n
    - Reset counter to new value.\n
    - Clear Flag. Unlock TimeOut handling in interrupt\n
    \param[in] u8Period Value autoreloaded when counter = 0.\n
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \ref u8RegTimeBase \n
    \ref u8RegCounter \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void RTI_SetSpeedRegPeriod(u8 u8Period) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8 u8Period_tmp;
  u8 u8TimeBaseStatus_tmp;

  u8Period_tmp = u8Period;

  if ( u8Period_tmp == 0u)
  {
    u8Period_tmp = 1u;               /* Min value is 1ms */
  }
  INT0_Set_u8RegTimeBase(u8Period_tmp);               /* Value autoreloaded when counter = 0 */
  INT0_Set_u8RegCounter(u8Period_tmp);                /* Reset counter to new value */

  u8TimeBaseStatus_tmp = INT0_Get_u8TimeBaseStatus() & (u8)(~REG_PERIOD_ELAPSED); /* Clear Flag */
  INT0_Set_u8TimeBaseStatus(u8TimeBaseStatus_tmp);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void RTI_SetMainTimeBase(u8 u8Period)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Loads related timer base time value.  
    \details Sets the related time base timer value. It uses a lock flag to avoid
    bad timer updating if rti isr is being served. It resets all related time elapsed
    flags. The following operation are done in sequence:\n
    - Lock TimeOut handling in interrupt.\n
    - Save value autoreloaded when counter = 0.\n
    - Reset counter to new value.\n
    - Clear Flag. Unlock TimeOut handling in interrupt.\n
    \param[in] u8Period Value autoreloaded when counter = 0.\n
                            \par Used Variables
    \ref u8LockStatus \n
    \ref u16MainTimeBase \n
    \ref u16MainCounter \n
    \ref u8TimeBaseStatus \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void RTI_SetMainTimeBase(u8 u8Period)
{
  u8 u8Period_tmp;
  u8 u8LockStatus_tmp;
  u8 u8TimeBaseStatus_tmp;

  u8Period_tmp = u8Period;

  if ( u8Period_tmp == 0u)
  {
    u8Period_tmp = 1;                 /* Min value is 1ms */
  }
  u8LockStatus_tmp = INT0_Get_u8LockStatus() | MAIN_TIME_BASE_LOCK;
  INT0_Set_u8LockStatus(u8LockStatus_tmp);              /* Lock TimeOut handling in interrupt */
  INT0_Set_u16MainTimeBase(u8Period_tmp);               /* Value autoreloaded when counter = 0 */
  INT0_Set_u16MainCounter(u8Period_tmp);                /* Reset counter to new value */

  u8TimeBaseStatus_tmp = INT0_Get_u8TimeBaseStatus() & (u8)(~MAIN_TIMEBASE_ELAPSED); /* Clear Flag */
  INT0_Set_u8TimeBaseStatus(u8TimeBaseStatus_tmp);

  u8LockStatus_tmp = INT0_Get_u8LockStatus() & (u8)(~MAIN_TIME_BASE_LOCK);
  INT0_Set_u8LockStatus(u8LockStatus_tmp);              /* Unlock TimeOut handling in interrupt */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void RTI_SetDatiLogPeriod(u16 u16Period)
    \author	Davide Munari    \n <em>Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Loads related timer base time value.  
    \details Sets the related time base timer value. It uses a lock flag to avoid
    bad timer updating if rti isr is being served. It resets all related time elapsed
    flags. The following operation are done in sequence:\n
    - Lock TimeOut handling in interrupt.\n
    - Save value autoreloaded when counter = 0.\n
    - Reset counter to new value.\n
    - Clear Flag. Unlock TimeOut handling in interrupt\n
    \param[in] u16Period Value autoreloaded when counter = 0.\n
                            \par Used Variables
    \ref u8LockStatus \n
    \ref u16LogDatiPeriod \n
    \ref u16LogDatiTimer \n
    \ref u8TimeBaseStatus \n
    \return \b void no value return.
    \note None.
    \warning None.
*/

void RTI_SetDatiLogPeriod(u16 u16Period) /* PRQA S 1503 #rcma-1.5.0 warning message: this function could be used for log data management */
{
  u8 u8LockStatus_tmp;

  if ( u16Period == 0u)
  {
    u16Period = 1u;                 /* Min value is 1ms */ /* PRQA S 1338 # No side effect expected*/
  }

  INT0_Set_u16LogDatiPeriod(u16Period);               /* Value autoreloaded when counter = 0 */
  INT0_Set_u16LogDatiTimer(u16Period);                /* Reset counter to new value */


  u8LockStatus_tmp = INT0_Get_u8LockStatus() & (u8)(~LOGDATI_LOCK);
  INT0_Set_u8LockStatus(u8LockStatus_tmp);            /* Unlock TimeOut handling in interrupt */
}

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
/*-----------------------------------------------------------------------------*/
/** 
    \fn void RTI_SetDiagShortDuration(u16 u16Duration)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Loads related timer base time value.  
    \details Sets the related time base timer value. It uses a lock flag to avoid
    bad timer updating if rti isr is being served. It resets all related time elapsed
    flags. The following operation are done in sequence:\n
    - Lock TimeOut handling in interrupt.\n
    - Save value autoreloaded when counter = 0.\n
    - Reset counter to new value.\n
    - Clear Flag. Unlock TimeOut handling in interrupt\n
    \param[in] u16Duration Value autoreloaded when counter = 0.\n
                            \par Used Variables
    \return \b void no value return.
    \note None.
    \warning None.
*/
void RTI_SetDiagShortDuration(u8 u8Period) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8 u8Period_tmp;

  u8Period_tmp = u8Period;

  if ( u8Period_tmp == (u8)0)
  {
    u8Period_tmp = (u8)1;                                       /* Min value is 1ms*/
  }

  INT0_Set_u8DiagShortPeriod(u8Period_tmp);                     /* Value autoreloaded when counter = 0 */
  INT0_Set_u8DiagShortCounter(u8Period_tmp);                    /* Reset counter to new value */

}
#endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL RTI_IsTimeOutElapsed(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Indicates that the Time out period has expired.  
    \details Retrun TRUE if time out is elapsed. It resets related expired flag
    as well.\n
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \return \b TRUE if time out is elapsed
    \note None.
    \warning This function has to be polled as often as possible to 
    minimize the error on expected Time Out period.
*/
BOOL RTI_IsTimeOutElapsed(void)
{
  BOOL ret;
  u8 u8TimeBaseStatus_tmp;

  u8TimeBaseStatus_tmp = INT0_Get_u8TimeBaseStatus();

  if ( (u8TimeBaseStatus_tmp & TIMEOUT_ELAPSED) != 0u )
  {
    u8TimeBaseStatus_tmp = u8TimeBaseStatus_tmp & (u8)(~TIMEOUT_ELAPSED);  /* Clear Flag */
    INT0_Set_u8TimeBaseStatus(u8TimeBaseStatus_tmp);
    ret = (BOOL)TRUE;
  }
  else
  {
    ret = (BOOL)FALSE;
  }

  return (ret);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL RTI_IsRegPeriodElapsed(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Indicates that the Time out period has expired.  
    \details Retrun TRUE if time out is elapsed. It resets related expired flag
    as well.\n
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \return \b TRUE if time out is elapsed
    \note None.
    \warning This function has to be polled as often as possible to 
    minimize the error on expected Time Out period.
*/
BOOL RTI_IsRegPeriodElapsed(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u8 u8TimeBaseStatus_tmp;

  u8TimeBaseStatus_tmp = INT0_Get_u8TimeBaseStatus();

  if ( (u8TimeBaseStatus_tmp & REG_PERIOD_ELAPSED) != 0u )
  {
    u8TimeBaseStatus_tmp = u8TimeBaseStatus_tmp & (u8)(~REG_PERIOD_ELAPSED); /*	Clear Flag */
    INT0_Set_u8TimeBaseStatus(u8TimeBaseStatus_tmp);
    ret = (BOOL)TRUE;
  }
  else
  {
    ret = (BOOL)FALSE;
  }

  return (ret);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL RTI_IsMainTimeElapsed(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Indicates that the Time out period has expired.  
    \details Retrun TRUE if time out is elapsed. It resets related expired flag
    as well.\n
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \return \b TRUE if time out is elapsed
    \note None.
    \warning This function has to be polled as often as possible to 
    minimize the error on expected Time Out period.
*/
BOOL RTI_IsMainTimeElapsed(void)
{
  BOOL ret;
  u8 u8TimeBaseStatus_tmp;

  u8TimeBaseStatus_tmp = INT0_Get_u8TimeBaseStatus();

  if ( (u8TimeBaseStatus_tmp & MAIN_TIMEBASE_ELAPSED) != 0u )
  {
    u8TimeBaseStatus_tmp = u8TimeBaseStatus_tmp & (u8)(~MAIN_TIMEBASE_ELAPSED); /* Clear Flag */
    INT0_Set_u8TimeBaseStatus(u8TimeBaseStatus_tmp);
    ret = (BOOL)TRUE;
  }
  else
  {
    ret = (BOOL)FALSE;
  }

  return (ret);

}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL RTI_IsLogDatiElapsed(void)
    \author	Davide Munari    \n <em>Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 01/08/2014
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Indicates that the Log Dati period has expired.  
    \details Return TRUE if time out is elapsed. It resets related expired flag
    as well.\n
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \return \b TRUE if time out is elapsed
    \note None.
    \warning This function has to be polled as often as possible to 
    minimize the error on expected Time Out period.
*/
BOOL RTI_IsLogDatiElapsed(void) /* PRQA S 1503 #rcma-1.5.0 warning message: this function could be used for log data management */
{
  BOOL ret;
  u8 u8TimeBaseStatus_tmp;

  u8TimeBaseStatus_tmp = INT0_Get_u8TimeBaseStatus();

  if ( (u8TimeBaseStatus_tmp & LOGTIMER_ELAPSED) != 0u )
  {
    u8TimeBaseStatus_tmp = u8TimeBaseStatus_tmp & (u8)(~LOGTIMER_ELAPSED); /* Clear Flag */
    INT0_Set_u8TimeBaseStatus(u8TimeBaseStatus_tmp);
    ret = ( TRUE );
  }
  else
  {
    ret = ( FALSE );
  }

  return ret;
}

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL RTI_IsDiagShortElapsed(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Indicates that the Time out period has expired.  
    \details Retrun TRUE if time out is elapsed. It resets related expired flag
    as well.\n
                            \par Used Variables
    \ref u8TimeBaseStatus \n
    \return \b TRUE if time out is elapsed
    \note None.
    \warning This function has to be polled as often as possible to 
    minimize the error on expected Time Out period.
*/
BOOL RTI_IsDiagShortElapsed(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u8 u8TimeBaseStatus_tmp;

  u8TimeBaseStatus_tmp = INT0_Get_u8TimeBaseStatus();

  if ( (u8TimeBaseStatus_tmp & DIAG_TIMEBASE_ELAPSED) != (BOOL)FALSE )
  {
    u8TimeBaseStatus_tmp = u8TimeBaseStatus_tmp & (u8)(~DIAG_TIMEBASE_ELAPSED); /* Clear Flag */
    INT0_Set_u8TimeBaseStatus(u8TimeBaseStatus_tmp);
    ret = ( TRUE );
  }
  else
  {
    ret = ( FALSE );
  }

  return ret;
}
#endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */


/*-----------------------------------------------------------------------------*/
/** 
    \fn void RTI_CheckCPSTimeBase(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Clear timebase for Checkpowerstage protections and stores that time base is elapsed  
                            \par Used Variables
    \ref BoolCPSReadyToCheck \n
    \ref BoolCPSTimeBase \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void RTI_CheckCPSTimeBase(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
  {
    if (INT0_Get_BoolCPSTimeBase()==(BOOL)TRUE)
    {
      BoolCPSReadyToCheck=TRUE;
      INT0_Set_BoolCPSTimeBase(FALSE);
    }
    else
    {
      /*Nop();*/
    }
  }


/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL RTI_CPSIsReadyToCheck(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Check if timebase for Checkpowerstage is elapsed   
                            \par Used Variables
    \ref BoolCPSReadyToCheck \n
    \return \b if timebase for Checkpowerstage is elapsed.
    \note None.
    \warning None.
*/
BOOL RTI_CPSIsReadyToCheck(void)
{
   return (BoolCPSReadyToCheck); 
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void RTI_ClearCPSTimeBase(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Clear timebase for Checkpowerstage protections   
                            \par Used Variables
    \ref BoolCPSReadyToCheck \n
    \return \b void no value return.
    \note None.
    \warning None.
*/

void RTI_ClearCPSTimeBase(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
  {
    BoolCPSReadyToCheck=FALSE;
  }

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
