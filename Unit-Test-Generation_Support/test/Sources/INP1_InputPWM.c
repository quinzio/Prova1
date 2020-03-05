/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputPWM.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INP1_InputPWM.c
    \version see \ref FMW_VERSION 
    \brief Basic pwm input function.
    \details Routines for pwm input operation and variables related managing.\n
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
#include "main.h"               /* Per strDebug                                                          */
#include "MCN1_acmotor.h"       /* It has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"           /* Public motor control peripheral function prototypes                   */
#include "TIM1_RTI.h"           /* Public general purpose Timebase function prototypes                   */
#include MAC0_Register          /* Needed for macro definitions                                          */
#include "INP1_InputSP.h"
#include "INP1_InputPWM.h"
#include "INP1_Key.h"
#include "EMR0_Emergency.h"
#include "SOA1_SOA.h"
#include "WTD1_Watchdog.h"
#include "INP1_InputAN.h"       /* For InputAN_GetPwmToAnalogSetPoint prototype */
#include "INT0_RTIIsrS12.h"
#include "INT0_InputPWMIsrS12.h"
#include "TMP1_Temperature.h"   /* For BOOLSOATemperatureCorr */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif

#if ( defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN) )

/* -------------------------- Private Constants ----------------------------*/

/* -------------------------- Private variables --------------------------- */
/* Some variables accessed in assembly located in page zero to decrease CPU load */
/* WARNING: if SensorPeriod structure is put outside Zero page, the declaration  */
/* of the pointer PSpeedMeas_s has to be modified (remove near/@tiny)            */

  #pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var PWMInFlag
    \brief Its flags define pwm input decoding state.
    \details Defined as \ref tPWMInFlag. Its flags define pwm input decoding state.
    \note This varaible is placed in \b _DATA_ZEROPAGE because of managing 
    in interrupt routine.
    \warning This variable is \b STATIC.
*/
  tPWMInFlag PWMInFlag;

/** 
    \var u8PWMIn
    \brief Decoding of pwm input signal.\n
    \note This varaible is placed in \b _DATA_ZEROPAGE because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u8 u8PWMIn;
 

  #pragma DATA_SEG DEFAULT
  
    
  #ifdef PWM_TO_ANALOG_IN
  static u8 u8PlausabilityFails;
  #endif /*PWM_TO_ANALOG_IN*/

/* -------------------------- Private functions --------------------------- */
 #ifndef PWM_TO_ANALOG_IN
 static BOOL InputPWM_PwmInIsLow(void);
 static BOOL InputPWM_PwmInIsHigh(void);
 #else
 static void InputPWM_DisablePWMInterrupt(void);
 #endif
 
 #ifdef RVS 
  u8 InputPWM_GetPWMInputFdT(u8);
 #endif  /* RVS */


/** 
    \fn void InputPWM_InitPWMInput(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Resets pwm input decoding timer and related variables.  
    \details Set timer module (TIM) edge sensitive and cleacapture flags.\n
                            \par Used Variables
    \ref PWMInFlag. Defined as \ref tPWMInFlag. Its flags define pwm input decoding state.\n
    \ref boolOVFVsPWMInput.\n
    \ref u8PWMIn. Decoding of pwm input signal.\n
    \ref u8NumOvfPWM. Used to manage timer overflow interrupt and to compute pwm input frequency and duty cycle.\n
    \ref u8PWMindex. Index of \ref bufstrPWMInData[DIM_BUFFER_PWM_IN].\n
    \ref enumPwmInState. Used to define motor behaviour over input request.\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void InputPWM_InitPWMInput(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{   
  PWMInFlag.READY_CALC=FALSE;
  PWMInFlag.SEND_EMERGENCY_STATUS=FALSE;
  
  #ifdef PWM_TO_ANALOG_IN
  u8PlausabilityFails=0;
  #endif /*PWM_TO_ANALOG_IN*/
  
  u8PWMIn=0;
  
  #ifdef KEY_SHORT_LOGIC
    INT0_InputPWM_Set_u8PositiveLevelCount((u8)0);
  #endif  /* KEY_SHORT_LOGIC */
    
  INPUTPWM_SET_FALLING_EDGE_SENS;     /* Next PWM isr on rising edge */
  
  INPUTPWM_INTERRUPT_ENABLE;

}


/*-----------------------------------------------------------------------------*/
#ifdef PWM_TO_ANALOG_IN
/** 
    \fn void InputPWM_DisablePWMInterrupt(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio  \n <em>Reviewer</em>

    \date 03/12/2013
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Disable PWM interrupt.  
    \details Call the macro to disable PWM interrupt.\n
                            \par Used Variables
    \ref
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void InputPWM_DisablePWMInterrupt(void)
{
  INPUTPWM_INTERRUPT_DISABLE;  
}
#endif



  

/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputPWM_FillBufferAndSetOperating(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Fill in \ref bufstrPWMInData[u8PWMindex] buffer.  
    \details This function performs three things every time a \ref PWMInFlag READY_CALC flag is set i.e. every time a valid falling pwm input edge is detected:
    1. Call \ref InputPWM_GetPWMInputSetPoint(void) to find duty and period and to perform plausibility check. 
    2. Call \ref InputSP_FindOperatingMode(void) in order to set normal/balancing/torque operating mode.
    3. Increment u16PWMCount variable every time \ref InputSP_FindOperatingMode(void) return \ref enumOperatingMode different from \ref NORMAL_OPERATION
    READY_CALC is set after a valid pwm input falling edge detection.
    Even if READY_CALC is managed into \ref InputPWM_GetPWMInputSetPoint(void), the flag is checked in advance here
    in order to call \ref InputSP_FindOperatingMode(void) and so to allow correct \ref enumOperatingMode i.e corrent 
    u16PWMCount computing.
    That's because of \ref InputSP_FindOperatingMode(void) HAS TO BE EVALUATED ONCE READY_CALC IS TRUE!!  
    All above functions are done for \ref PWM_IN_INPUT_TEST_TIME_BASE * 8msec in order to 
    fill in \ref bufstrPWMInData[u8PWMindex] buffer and allow plausibility check.\n
                            \par Used Variables
    \return \b void no value return.
    \note None.
    \warning None.
*/
void InputPWM_FillBufferAndSetOperating(void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/ /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16PWMCount = 0; /* Reset Cont */

  RTI_SetMainTimeBase((u8)PWM_IN_INPUT_TEST_TIME_BASE);
  while(RTI_IsMainTimeElapsed() == (BOOL)FALSE)
  {
    INT0_FailSafeMonitor();

    if ( PWMInFlag.READY_CALC != 0u )                            /* READY_CALC is set after a valid pwm input falling edge detection.                                    */
    {                                                            /* Even if READY_CALC is managed into InputPWM_GetPWMInputSetPoint, the flag is checked in advance here */
      (void)InputPWM_GetPWMInputSetPoint();                      /* in order to call InputSP_FindOperatingMode() and so to allow correct enumOperatingMode i.e corrent   */
      InputSP_FindOperatingMode();                               /* u16PWMCount computing.                                                                               */
      if( !(InputSP_GetOperatingMode() == NORMAL_OPERATION) )    /* That because InputSP_FindOperatingMode() HAS TO BE EVALUATED ONCE READY_CALC IS TRUE!!               */
      {
        u16PWMCount++;
      }
      else
      {
        u16PWMCount = 0;
      }
    }
    else
    {
      Nop(); /*PRQA S 1006 #Necessary assembly code*/
    }
   
    #ifdef PWM_TO_ANALOG_IN
    (void)InputAN_GetPwmToAnalogSetPoint();   /*To keep analog plausibility alive*/
    #endif /*PWM_TO_ANALOG_IN*/

    WTD_ResetWatchdog(); 
  }

  INT0_FailSafeMonitor_rst();

  if( u16PWMCount < PWM_IN_VALID_NORMAL_OPERATION_COUNT )
  {
    InputSP_SetOperatingMode(NORMAL_OPERATION);
  }
  else
  {
    Nop(); /*PRQA S 1006 #Necessary assembly code*/
  }
  u16PWMCount = 0; /* For later usage. */
}


#ifdef PWM_INPUT
/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputPWM_PwmDisappeared(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return TRUE if pwm input disappears for more than \ref PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER period.  
    \details This function return:.\n
    - TRUE if if pwm input disappears for more than PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER period.\n
    - else it return FALSE.\n
    Returned value are valid even in test mode.\n 
                            \par Used Variables
    \return \b bool TRUE if pwm input disappears for more than \ref MAX_NUM_NO_DUTY * 8ms.
    \note None.
    \warning None.
*/
BOOL InputPWM_PwmDisappeared(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  BOOL ret;
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    ret = (BOOL)FALSE;

    #ifdef KEY_SHORT_LOGIC
      /* NOTE: the key-short logic must be disabled when thermal derating is on.                      */
      /*       Otherwise, such procedure may alter the correct behavior of the drive during derating, */
      /*       because of the RoFs commanded every TIME_TO_REFRESH_KEY_SHORT minutes.                 */

      if ( Temp_Get_BOOLSOATemperatureCorr() == (BOOL)FALSE )                 /* If no thermal derating is active... */
      {
        if ( InputSP_Get_u8NumNoDuty() >= KEY_CHECK_WITH_NO_PWM_TIME_NUM )    /* ...and no PWM command is present... */
        {              
          if ( INT0_Get_u16KeyRefreshCount() == (u16)0 )                      /* ...and the refresh time has exhausted... */
          {
            INT0_Set_enKeyShort(INIT);                                        /* ...re-initialize the key-short logic detection. */
          }
          else
          {
            /* Nop(); */
          }
        }
        else                                                                  /* If PWM command is present... */
        {
          INT0_Set_enKeyShort(DISABLE);                                       /* ...disable the key-short detection logic... */
          INT0_Set_u16KeyRefreshCount((u16)0);                                /* ...and (then) reset the key-short refresh counter. */
        }
      }
      else                                                                    /* If thermal derating is active... */
      {
        INT0_Set_enKeyShort(DISABLE);                                         /* ...keep the key-short logic disabled... */
        INT0_Set_u16KeyRefreshCount((u16)0);                                  /* ...and reset the key-short refresh counter. */
      }
    #endif  /* KEY_SHORT_LOGIC */
  }
  else
  {
    if ( InputSP_Get_u8NumNoDuty() >= PWM_IN_DISAPPEAR_TEST_OP_NUM )          /* It has to be strictly major (i.e. >) since because of pwm input frequency at testing phase, can happen PWM_IN_DISAPPEAR_TEST_OP_NUM=0! */
    {
      ret = (BOOL)TRUE;
    }
    else
    {
      ret = (BOOL)FALSE;
    }
  }
  return (ret);
}
#endif

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputPWM_GetPWMInputSetPoint(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return duty cycle value filtered by plausibility check.
    \details This function does:\n
    Performs duty cycle computing basing on \ref bufstrPWMInData[\ref u8PWMindex] values.
    Performs plausibility check over pwm input period and duty cycle.\n
                            \par Used Variables
    \ref u32TempDuty Buffer for the calculated duty cycle \n
    \return \b u8 duty cycle value scaled from 0 to 255.
    \note None.
    \warning Attenzione occorre circa un tempo pari a 2 sec per acquisire 4 duty 
             cycle con frequenza 5Hz (vedi capitolato Porsche).
*/
#if defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN)

#if defined(PWM_IN_POSITIVE_LOGIC)
  #define PWM_DUTY (InputSP_Get_u8PWMinDuty())
#elif defined(PWM_IN_NEGATIVE_LOGIC) 
  #define PWM_DUTY (~(InputSP_Get_u8PWMinDuty()))
#else
  #error("Pwm input type is undefined. Please check SPAL_Setup.h")
#endif 

#endif/* PWM_INPUT */

#ifndef PWM_TO_ANALOG_IN
 
u8 InputPWM_GetPWMInputSetPoint(void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/
{
  u8 ret;
  u8 u8PWMinDuty_tmp;
  StatePwmIn_T enumPwmInState_tmp;

  enumPwmInState_tmp = InputSP_Get_enumPwmInState();

  if ( PWMInFlag.READY_CALC == 0u )                                             /* READY_CALC is set after a valid pwm input falling edge detection. */
  {
    if ( InputSP_Get_u8NumNoDuty() >= PWM_IN_DISAPPEAR_NORMAL_OP_NUM )          /* If PWM input disappears, InputPWM_PwmDisappeared function into Main_CheckPowerStage catches this disappearing */
    {                                                                           /* and sets enState state in WAIT. After timeout elapsed, InputSP_GetInputSetPoint is performed. */
#ifdef _CANTATA_
      Alter_u8NumNoDuty();
#endif
      if(InputPWM_PwmInIsHigh() != 0u)
      {
        if ( InputSP_Get_u8NumNoDuty() >= PWM_IN_DISAPPEAR_NORMAL_OP_NUM )
        {
           InputSP_Set_u8PWMinDuty(SP_100PERCENT_DUTY);
        }
        else
        {
          /* Nop(); */
        }
      }
      else
      {
        if(InputPWM_PwmInIsLow() != 0u) 
        {
          if ( InputSP_Get_u8NumNoDuty() >= PWM_IN_DISAPPEAR_NORMAL_OP_NUM ) 
          { 
            InputSP_Set_u8PWMinDuty(SP_0PERCENT_DUTY);
          }
          else
          {
            /* Nop(); */
          }
        }
      }

      InputSP_InitPlausabilityCheck();

      #ifdef RVS 
        if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
        { 
          u8PWMIn = InputSP_GetRevInputFdT((u8)PWM_DUTY);
        }
        else
        {
          u8PWMIn = InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)PWM_DUTY);
        }
      #else
        u8PWMIn = InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)PWM_DUTY);     /* ~u8PWMinDuty takes in account IFMA adn IFuC have same polarization                                                             */
                                                                                    /* N.B.                                                                                                                           */
                                                                                    /* CHANGE INTIAL (u8)u8PWMinDuty SETTING INTO InputSP_InitInput FUNTION IF (u8)u8PWMinDuty IS USED INSTEAD OF (u8)~u8PWMinDuty!!! */
      #endif  /* RVS */

      ret = u8PWMIn;
    }
    else
    {
      ret = u8PWMIn;
    }
  }
  else
  {
    InputSP_GetPWMDutyAndPeriod();                                              /* Update u32TempDuty and u32TempPeriod */

    if ( InputSP_PerformPlausabilityCheck() != 0u )                             /* TREQ044 */
    {                                                                           /* If plausibility has good result ( even during TEST MODE) set enumOperatingMode to NORMAL_OPERATION and thi is valid */
      InputSP_SetOperatingMode((OperatingMode_t)NORMAL_OPERATION);              /* if only one good period and duty has found. */
    }
    else
    {
      Nop(); /*PRQA S 1006 #Necessary assembly code*/
    }

    if ( InputSP_GetOperatingMode() != NORMAL_OPERATION )
    {                                                                           /* If plausibility fails just use actual duty computed. */
      u8PWMinDuty_tmp = (u8)InputSP_Get_u32TempDuty();
      InputSP_Set_u8PWMinDuty(u8PWMinDuty_tmp);
    }
    else
    {
      Nop(); /*PRQA S 1006 #Necessary assembly code*/                           /* If plausibility is good use u8PWMinDuty set point computed into InputSP_PerformPlausabilityCheck() funtion */
    }
    
    #ifdef RVS
      if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
      { 
        u8PWMIn = InputSP_GetRevInputFdT((u8)PWM_DUTY);
      }
      else
      {
        u8PWMIn = InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)PWM_DUTY);
      }
    #else
      u8PWMIn = InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)PWM_DUTY);   /* ~u8PWMinDuty takes in account IFMA adn IFuC have same polarization */
                                                                                /* N.B. */
                                                                                /* CHANGE INTIAL (u8)u8PWMinDuty SETTING INTO InputSP_InitInput FUNTION IF (u8)u8PWMinDuty IS USED INSTEAD OF (u8)~u8PWMinDuty!!! */
    #endif  /* RVS */
    
    PWMInFlag.READY_CALC = FALSE;
    
    ret = u8PWMIn;
  }
  return ret;
}

#else /*PWM_TO_ANALOG*/

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputPWM_GetPWMInputSetPoint(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return duty cycle value filtered by plausibility check.
    \details This function does:\n
    Performs duty cycle computing basing on \ref bufstrPWMInData[\ref u8PWMindex] values.
    Performs plausibility check over pwm input period and duty cycle.\n
                            \par Used Variables
    \ref u32TempDuty Buffer for the calculated duty cycle \n
    \return \b u8 duty cycle value scaled from 0 to 255.
    \note None.
    \warning Attenzione occorre circa un tempo pari a 2 sec per acquisire 4 duty 
             cycle con frequenza 5Hz (vedi capitolato Porsche).
*/

u8 InputPWM_GetPWMInputSetPoint(void) 
{
  u8 u8PWMIn_ret;
  StatePwmIn_T enumPwmInState_tmp;

  enumPwmInState_tmp = InputSP_Get_enumPwmInState();

  if ( PWMInFlag.READY_CALC == (u16)0 )                                         /* READY_CALC is set after a valid pwm input falling edge detection. */
  {
    u8PWMIn_ret = u8PWMIn;
  }
  else
  {
    InputSP_GetPWMDutyAndPeriod();                                              /* Update u32TempDuty and u32TempPeriod */

    if ( (u8)InputSP_CheckPeriodCount() >= PWM_IN_PERIOD_EVALUATING_NUMBER )    /* Only when PWM_IN_PERIOD_EVALUATING_NUMBER periods have been checked and plausibility returns its result */
    {                                                                           /* on the entire PWM_IN_PERIOD_EVALUATING_NUMBER sequence, it is possible to evaluate if PWM frequency and */
      if ( InputSP_PerformPlausabilityCheck() != (BOOL)FALSE )                  /* duty are within production ranges. */
      {                                                                         /* If plausibility has good result (i.e. 7 Hz < PWM freq < 10 Hz ), set enumOperatingMode = xxx_TEST */
        InputSP_CheckDutyAndSetMode();
        u8PlausabilityFails=0;
      }
      else
      {
        u8PlausabilityFails++;
        if (u8PlausabilityFails>= PWM_TO_ANALOG_MAX_PLAUSIBILITY_FAILS)
        {
          InputSP_SetOperatingMode((OperatingMode_t)NORMAL_OPERATION);          /* If plausibility fails, set enumOperatingMode = NORMAL_OPERATION */
          InputPWM_DisablePWMInterrupt();                                       /*  and disable PWM interrupt (i.e. setpoint from PWM_TO_ANALOG channel). */
        }
        else
        {
         /*Nop();*/
        }
      }
    }
    else
    {
      (void)InputSP_PerformPlausabilityCheck();
    }                                                                           /* Keep the plausibility check active. */

    u8PWMinDuty = (u8)InputSP_Get_u32TempDuty();
    u8PWMIn = InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)PWM_DUTY);     /* ~u8PWMinDuty takes in account IFMA adn IFuC have same polarization                                                             */
                                                                                /* N.B.                                                                                                                           */
                                                                                /* CHANGE INTIAL (u8)u8PWMinDuty SETTING INTO InputSP_InitInput FUNTION IF (u8)u8PWMinDuty IS USED INSTEAD OF (u8)~u8PWMinDuty!!! */

    PWMInFlag.READY_CALC = FALSE;
    u8PWMIn_ret = u8PWMIn ;
  }
  return(u8PWMIn_ret);
}
#endif /* PWM_TO_ANALOG */

#ifndef PWM_TO_ANALOG_IN
/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputPWM_PwmInIsHigh()
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010

*/

static BOOL InputPWM_PwmInIsHigh(void)
  {
    BOOL ret;
    u8 i;
    i=0;
    if (PWM_IN_IS_HIGH)
    {
      i++;
    }
    if (PWM_IN_IS_HIGH)
    {
      i++;
    }
    if (PWM_IN_IS_HIGH)
    {
      i++;
    }
    
    if (i>=(u8)2) 
    { 
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
    \fn BOOL InputPWM_PwmInIsLow()
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010

*/
  
static BOOL InputPWM_PwmInIsLow(void)
  {
    BOOL ret;
    u8 i;
    i=0;
    if (PWM_IN_IS_LOW)
    {
      i++;
    }
    if (PWM_IN_IS_LOW)
    {
      i++;
    }
    if (PWM_IN_IS_LOW)
    {
      i++;
    }
    
    if (i>=(u8)2)
    { 
      ret = (BOOL)TRUE;
    }
    else 
    {
      ret = (BOOL)FALSE;
    }
    return (ret);
  }  
   
#endif /*PWM_TO_ANALOG_IN */

#endif  /*PWM_INPUT*/

/* END */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
