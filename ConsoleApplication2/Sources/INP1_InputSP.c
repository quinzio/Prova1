/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputSP.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INP1_InputSP.c
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
#include "main.h"           /* Per strDebug */
#include "MCN1_acmotor.h"   /* it has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"       /* Public motor control peripheral function prototypes*/ 
#include MAC0_Register
#include "INP1_InputSP.h"
#include "INP1_InputAN.h"
#include "INP1_InputECO.h"
#include "INP1_InputPWM.h"
#include "INP1_Key.h"
#include "VBA1_Vbatt.h"
#include "SOA1_SOA.h"
#include "FIR1_FIR.h"
#include "TMP1_Temperature.h" 
#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif
#ifdef LIN_INPUT
  #if defined(BMW_LIN_ENCODING)
    #include "COM1_LINUpdate_BMW.h"
  #elif defined(GM_LIN_ENCODING)
    #include "COM1_LINUpdate_GM.h"
  #else
    #error("Define LIN stack!!");
  #endif  /* BMW_LIN_ENCODING */
#endif  /* LIN_INPUT */
#include "DIA1_Diagnostic.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* --------------------------- Private macros ------------------------------ */

     /**** RVS ****/
    #ifdef RVS

    #define HYST_STOP_TO_BWD_FLAG_BIT         0
    #define HYST_STOP_TO_BWD_FLAG             ((u8)(1<<HYST_STOP_TO_BWD_FLAG_BIT))

    /* #define HYST_STOP_TO_FWD_FLAG_BIT      1                                    */
    /* #define HYST_STOP_TO_FWD_FLAG          ((u8)(1<<HYST_STOP_TO_FWD_FLAG_BIT)) */

    #define HYST_FWD_TO_BWD_FLAG_BIT          2
    #define HYST_FWD_TO_BWD_FLAG              ((u8)(1<<HYST_FWD_TO_BWD_FLAG_BIT))

    /* #define HYST_BWD_TO_DEF_FLAG_BIT       3                                   */
    /* #define HYST_BWD_TO_DEF_FLAG           ((u8)(1<<HYST_BWD_TO_DEF_FLAG_BIT)) */

    #define HYST_MAX_TO_EMG_FLAG_BIT          4
    #define HYST_MAX_TO_EMG_FLAG              ((u8)(1<<HYST_MAX_TO_EMG_FLAG_BIT))

    #endif

/* -------------------------- Private Constants ----------------------------*/

/* -------------------------- Private typedefs ---------------------------- */

/** 
    \typedef SpeedStatus_t
    \brief Used for define the states of the diagnostic state machine. 
*/
 #ifdef RVS 
    typedef enum
    {
    SPEED_STOP, SPEED_RUN, SPEED_REV
    } SpeedStatus_t;
 #endif  /* RVS */
 
/* --------------------------- Public variables -----------------*/
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE

/** 
    \var bufstrPWMInData[DIM_BUFFER_PWM_IN]
    \brief  Used to decode pwm input in terms of frequency and duty cycle.
    \details Defined as a vector of \ref tPWMInValue. Used to decode pwm input in terms of frequency and duty cycle.
    \note This varaible is placed in \b _DATA_ZEROPAGE because of managing 
    in interrupt routine.
    \warning This variable is \b static.
*/
  volatile tPWMInValue bufstrPWMInData[DIM_BUFFER_PWM_IN];

/** 
    \var u8PWMindex
    \brief Index of \ref bufstrPWMInData[DIM_BUFFER_PWM_IN].
    \note This varaible is placed in \b _DATA_ZEROPAGE because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  u8 u8PWMindex = 0;

/** 
    \var u8NumOvfPWM
    \brief Used to manage timer overflow interrupt and to compute pwm input frequency and duty cycle.
    \note This varaible is placed in \b _DATA_ZEROPAGE because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  u8 u8NumOvfPWM = 0;

/** 
    \var boolOVFVsPWMInput
    \brief Used to manage capture vs overflow interrupt since these two interrupt have they own priority.
    \note This varaible is placed in \b _DATA_ZEROPAGE because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  BOOL boolOVFVsPWMInput;

  #pragma DATA_SEG DEFAULT
/**
    \var enumPwmInState
    \brief Used to define motor behaviour over input request.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static StatePwmIn_T enumPwmInState;  

/**
    \var u8NumNoDuty
    \brief Number of RTI ovf where the Input PWM signal is absent.
    \details Incremented every RTI ovf it counts where the Input PWM signal is absent. When too big means no longer pwm input.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
*/
  volatile u8 u8NumNoDuty=0;

/**
    \var enumOperatingMode
    \brief Tells if motor has to run in  production or normal behaviour.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static OperatingMode_t enumOperatingMode;

/**
    \var u32TempPeriod
    \brief Period of pwm input signal. Needed in main in order to decide from production or normal behaviour.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u32 u32TempPeriod;

/**
    \var u32TempDuty
    \brief Duty cycle of pwm input signal. Needed in main in order to decide from production or normal behaviour.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u32 u32TempDuty;

/**
    \var u8PWMinDuty
    \brief Duty cycle of pwm input signal resulting from plausibility check.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  #if( defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN))
  static u8 u8PWMinDuty;
  #else
  u8 u8PWMinDuty;
  #endif

/**
    \var u8PWMinDutyZero
    \brief Duty cycle of pwm input signal sampled at plausability check entrance.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u8 u8PWMinDutyZero;

/**
    \var u16PWMDutyAmount
    \brief Amount value of duty cycle filtered by plausability check entrance.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u16 u16PWMDutyAmount;

/**
    \var u8GoodDutyCount
    \brief Number of good duty cycle of pwm input signal.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u8 u8GoodDutyCount;

/**
    \var u8GoodPeriodCount
    \brief Number of good period of pwm input signal.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u8 u8GoodPeriodCount;

/**
    \var u8PeriodCount
    \brief Actual amount number of pwm input signal period sampled.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
  static u8 u8PeriodCount;

/**
    \var u8BoolPlausCount
    \brief Counter of first 3 duty samples, sent to the plausability function.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/  
  static u8 u8BoolPlausCount;

 /**
    \var boolPlausResult
    \brief Result of plausibility check.
    \details Result of InputSP_PerformPlausabilityCheck function.
    \note This varaible is placed in \b DEFAULT.
    \warning This variable is \b static.
*/
static BOOL boolPlausResult; /*PRQA S 3218 #Gives better visibility here*/

#ifdef RVS
/**
    \var enumSpeedStatus
    \brief Used in reverse speed.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
SpeedStatus_t enumSpeedStatus;

/** 
    \var u16TimeToBrake
    \brief Used in reverse speed to define decreasing speed time.
    \note This varaible is placed in \b DEFAULT because of managing 
    in interrupt routine.
    \warning This variable is \b static.
    \details None.
*/
u16 u16TimeToBrake;

/**
    \var u8SetpointHystFlags
    \brief Hyst flags to manage STOP state hysteresis for RVS condition
     \warning This variable is \b static.
    \details None.
*/
 u8 u8SetpointHystFlags; 
 #endif  /* RVS */


/* -------------------------- Private variables --------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE             /* If s12 using place all following variables into _DATA_ZEROPAGE */


#pragma DATA_SEG DEFAULT                          /* If s12 using place all previous variables into _DATA_ZEROPAGE */


#if (defined(PWM_INPUT) && defined(AN_INPUT))
typedef enum {
    PWM,
    ANALOG
} InputMode;

static InputMode enumInputStatus;

/** 
    \var u8ANAInput
    \brief Analog input setpoint.
    \note This varaible is placed in \b DEFAULT.
    \warning This variable is \b static.
*/ 
static u8 u8ANAInput;

/** 
    \var u8PWMInput
    \brief PWM input setpoint.
    \note This varaible is placed in \b DEFAULT.
    \warning This variable is \b static.
*/ 
static u8 u8PWMInput;

#endif /* PWM_INPUT && AN_INPUT */

static u8 u8InputSetPoint;


/* -------------------------- Private functions --------------------------- */
#if (defined(PWM_INPUT) && defined(AN_INPUT))
static void  InputSP_CheckInputStatus (void);
#endif  /* PWM_INPUT && AN_INPUT */

#ifndef PWM_TO_ANALOG_IN
static void InputSP_CheckDutyAndSetMode(void);
#endif

static void InputSP_Set_u32TempDuty(u32 u32Value);
static void InputSP_Set_u8NumNoDuty(u8 u8Value);

static void InputSP_Set_enumPwmInState(StatePwmIn_T value);

/* --------------------------- Private macros ----------------------------- */

/* -------------------------- Public functions --------------------------- */
  static u8 InputSP_PwmInStopFn(u8 u8MeanPWM);
  static u8 InputSP_PwmInMinFn(u8 u8MeanPWM);
  static u8 InputSP_PwmInNominalFn(u8 u8MeanPWM);
  static u8 InputSP_PwmInMaxFn(u8 u8MeanPWM);
  static u8 InputSP_PwmInEmergencyFn(u8 u8MeanPWM);
/**
  \var InputSP_StatePwmInFunction
  \brief Array of function pointers to fast jump to subroutine
  \details This array contains pointers to subroutines used to implement the Input PWM setpoint machine state.
  \ref InputPWM_GetPWMInputSetPoint
*/

u8 (*InputSP_StatePwmInFunction[])(u8 InRef)= { /*PRQA S 1533 #rcma-1.5.0 warning message: this function is correctly defined here */
      InputSP_PwmInStopFn,
      InputSP_PwmInMinFn,
      InputSP_PwmInNominalFn,
      InputSP_PwmInMaxFn,
      InputSP_PwmInEmergencyFn
   }; /*static*/



/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputSP_InitInput(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Resets analog and pwm input decoding timer and related variables.  
    \details None.\n
                            \par Used Variables
    \ref boolOVFVsPWMInput.\n
    \ref u8NumOvfPWM. Used to manage timer overflow interrupt and to compute pwm input frequency and duty cycle.\n
    \ref u8PWMindex. Index of \ref bufstrPWMInData[DIM_BUFFER_PWM_IN].\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void InputSP_InitInput(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{   
  boolOVFVsPWMInput = FALSE;
  u8NumOvfPWM = 0;
  u8PWMindex  = 0;
  InputSP_Set_enumPwmInState(HALTED);
  InputSP_SetOperatingMode(NORMAL_OPERATION);

  u8InputSetPoint   = 0;
  u8PeriodCount     = 0;                  /* Settings for plausibility check. */
  u8GoodPeriodCount = 0;
  u8GoodDutyCount   = 0;
  u8PWMinDutyZero   = 0;
  /*InputSP_Set_u8PWMinDuty((u8)255); */  /* N.B.                                                                                                                                     */
                                          /* CHANGE TO ZERO IF (u8)u8PWMinDuty IS USED INSTEAD OF (u8)~u8PWMinDuty INTO InputSP_StatePwmInFunction[enumPwmInState]((u8)~u8PWMinDuty). */
                                          /* CheckTestPhase() function calls InputPWM_FillBufferAndSetOperating() function that calls InputPWM_GetPWMInputSetPoint().                 */
                                          /* Under this funtion, following function are called:                                                                                       */
                                          /* 1. InputSP_GetPWMDutyAndPeriod()                                                                                                         */
                                          /* 2. InputSP_PerformPlausabilityCheck()                                                                                                    */
                                          /* 3. InputSP_StatePwmInFunction[enumPwmInState]((u8)~u8PWMinDuty)                                                                          */
                                          /* Since u8PWMinDuty is here fixed to 255, at previous point 3 state machine will stuck at HALTED.                                          */
  #ifdef RVS 
    enumSpeedStatus = SPEED_STOP;

    BitClear(HYST_STOP_TO_BWD_FLAG,u8SetpointHystFlags);     /*set here the initial status for u8SetpointHystFlags*/
    /*BitClear(HYST_STOP_TO_FWD_FLAG,u8SetpointHystFlags);*/
    BitSet(HYST_FWD_TO_BWD_FLAG,u8SetpointHystFlags);
    /*BitClear(HYST_BWD_TO_DEF_FLAG,u8SetpointHystFlags);*/
  #endif  /* RVS */

  #if defined(PWM_IN_POSITIVE_LOGIC)
    InputSP_Set_u8PWMinDuty((u8)0);
  #elif defined(PWM_IN_NEGATIVE_LOGIC) 
    InputSP_Set_u8PWMinDuty((u8)255);
  #else
    #error("Pwm input type is undefined. Please check SPAL_Setup.h")
  #endif

  u16PWMDutyAmount = 0;
  InputSP_Set_u8NumNoDuty(0);

  u8BoolPlausCount = 0;

  #if (defined(PWM_INPUT) && defined(AN_INPUT))
  u8PWMInput = 0;
  u8ANAInput = 0;
  enumInputStatus = PWM;
  #endif /* PWM_INPUT && AN_INPUT */

  #ifdef PWM_INPUT
  InputPWM_InitPWMInput();
  #endif   /* PWM_INPUT */

  #ifdef AN_INPUT
    InputAN_InitANInput();                /* If PWM_INPUT is selected ATDCMPE = 0 into Init_InitADC */

    #ifdef PWM_TO_ANALOG_IN
      InputPWM_InitPWMInput();            /* Enable PWM interrupt to detect the xxx_TEST mode */
    #endif  /* PWM_TO_ANALOG_IN */
  #endif  /* AN_INPUT */

  #ifdef ECONOMY_INPUT
      InputECO_InitECOInput(); 
  #endif  /* ECONOMY_INPUT */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn OperatingMode_t InputSP_GetOperatingMode(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return operating mode.
    \details Return \ref enumOperatingMode variable value.\n
                                            \par Used Variables
    \return \b OperatingMode_t \ref enumOperatingMode variable value.
    \note None.
    \warning None.
*/
OperatingMode_t InputSP_GetOperatingMode(void)
{
  return(enumOperatingMode); 
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputSP_SetOperatingMode(OperatingMode_t OperatingMode)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set operating mode.
    \details none.\n
                            \par Used Variables
    \return \b void.
    \note None.
    \warning None.
*/
void InputSP_SetOperatingMode(OperatingMode_t OperatingMode)
{
  enumOperatingMode = (OperatingMode_t)OperatingMode; 
}

/*--------------------------------------------------------------------------*/
/** 
    \fn u8 InputSP_GetInputSetPoint(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>

    \date 30/05/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return set point [0,255] basing on pwm and analog input.  
    \details If pwm and analog input are both presents just return highest value of them while
    if one of those ore present just return its value.\n
                            \par Used Variables
    \return \b set point [0,255] value.
    \note None.
    \warning None.
*/
u8 InputSP_GetInputSetPoint(void)
{
  #if( defined(PWM_INPUT) || defined(LIN_INPUT) )
    #ifdef AN_INPUT      

      u8PWMInput = InputPWM_GetPWMInputSetPoint();

      #ifdef POTENTIOMETER 
        u8ANAInput = InputAN_GetAnalogSetPoint();
      #else
        #ifdef NTC 
          u8ANAInput = InputAN_GetNTCSetPoint();
        #else
          #ifdef PWM_TO_ANALOG_IN   
            u8ANAInput = InputAN_GetPwmToAnalogSetPoint();
          #endif  /* PWM_TO_ANALOG_IN */
        #endif  /* NTC */
      #endif  /* POTENTIOMETER */

      InputSP_CheckInputStatus();

      if ( enumInputStatus == PWM )
      {
        u8InputSetPoint = u8PWMInput;
      }
      else
      {
        u8InputSetPoint = u8ANAInput;
      }
    #else
      #ifdef LIN_INPUT
        u8InputSetPoint = (u8)LIN_GetLINInputSetPoint();
      #else
        #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
          if ( DIAG_IsDiagnosticEnable() == (BOOL)FALSE )
          {
            u8InputSetPoint = (u8)InputPWM_GetPWMInputSetPoint();
          }
          else
          {
            /* Nop(); */
          }
        #else
          u8InputSetPoint = (u8)InputPWM_GetPWMInputSetPoint();
        #endif  /* DIAGNOSTIC_STATE */
      #endif  /* LIN_INPUT */
    #endif  /* AN_INPUT */
  #else
    #ifdef AN_INPUT 
      #ifdef POTENTIOMETER 
        u8InputSetPoint = InputAN_GetAnalogSetPoint();
      #else
        #ifdef NTC 
          u8InputSetPoint = InputAN_GetNTCSetPoint();
        #else
          #ifdef PWM_TO_ANALOG_IN
            if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
            {
              u8InputSetPoint = InputAN_GetPwmToAnalogSetPoint();        /* If NORMAL_OPERATION is set, get setpoint from ANALOG input */
            }
            else
            {
              u8InputSetPoint = InputPWM_GetPWMInputSetPoint();          /* If xxx_TEST is set, get setpoint from PWM input */
            }
          #endif  /* PWM_TO_ANALOG_IN */
        #endif  /* NTC */
      #endif  /* POTENTIOMETER */
    #else
      u8InputSetPoint = SP_MAX_SPEED_VALUE;
    #endif  /* AN_INPUT */
  #endif  /* PWM_INPUT */

  #ifdef ECONOMY_INPUT
   u8InputSetPoint = InputECO_GetEconomySetPoint(u8SetPoint);
  #endif /* ECONOMY_INPUT */
      
  return ( u8InputSetPoint );
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputSP_ReadyToSleep(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return TRUE when motor has to stop.  
    \details Function behaviour depends on Key present or not:\n
    1. Key present:\n
    - TRUE if Key is set to 0.\n
    - else it return FALSE.\n
    2. Key NOT present:\n
    - TRUE if if pwm input disappears for more than \ref MAX_NUM_NO_DUTY * 8ms.\n
    - else it return FALSE.\n
                            \par Used Variables
    \return \b bool TRUE if motor has to be stopped.
    \note None.
    \warning None.
*/
BOOL InputSP_ReadyToSleep(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  #if(defined(AN_INPUT) && defined(PWM_INPUT))
  BOOL ret;
  #endif

  #ifdef LIN_INPUT
   return ( (BOOL)LIN_IsReadyToSleep() );
  #else  /* LIN_INPUT */
    #ifdef KEY_INPUT
      return ( INP1_KeyIsOff() );
    #else
     #ifdef PWM_INPUT
         #ifdef AN_INPUT
          #if (defined(POTENTIOMETER) || defined(NTC))
            if ( InputPWM_PwmDisappeared() != (BOOL)FALSE )
          #elif (defined(PWM_TO_ANALOG_IN))
            if ( InputPWM_PwmDisappeared() && InputAN_PwmDisappeared() )
          #else
            #error("Define analog input type");
          #endif /* ANALOG TYPE */
            {
              ret = (BOOL)TRUE;
              /*return ( TRUE );*/
            }
            else
            {
              ret = (BOOL)FALSE;
              /*return ( FALSE );*/
            }
            return(ret);
         #else
          return ( InputPWM_PwmDisappeared() );
         #endif  /* AN_INPUT */
     #else
         #ifdef AN_INPUT
          return ( InputAN_PwmDisappeared() );
         #else
          return ( FALSE );
         #endif  /* AN_INPUT */
     #endif  /* PWM_INPUT */
    #endif  /* KEY_INPUT */
  #endif  /* LIN_INPUT */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputSP_PwmDisappeared(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return TRUE if pwm input disappears.
    \details This function return TRUE is pwm signal disappear even if pwm signal is applied at analog input.\n
    Returned value is:\n
    - TRUE if pwm input disappears for more than \ref PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER period.\n
    - else it return FALSE.\n
    Returned value are valid even in test mode.\n 
                            \par Used Variables
    \return \b bool TRUE if pwm input disappears for more than \ref MAX_NUM_NO_DUTY * 8ms.
    \note None.
    \warning None.
*/
BOOL InputSP_PwmDisappeared(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  #if(defined(AN_INPUT)&&(defined(PWM_INPUT) || defined(LIN_INPUT)))
  BOOL ret;
  #endif

  #if ( defined(PWM_INPUT) || defined(LIN_INPUT) )
    #ifdef AN_INPUT
      #if (defined(POTENTIOMETER) || defined(NTC))
       if ( InputPWM_PwmDisappeared() != (BOOL)FALSE )
      #elif (defined(PWM_TO_ANALOG_IN))
       if ( InputPWM_PwmDisappeared() && InputAN_PwmDisappeared() )
      #else
       #error("Define analog input type");
      #endif /*ANALOG TYPE*/
       {
         ret = (BOOL)TRUE;
         /*return ( TRUE );*/
       }
       else
       {
         ret = (BOOL)FALSE;
        /*return ( FALSE );*/
       }
       return(ret);
    #else
      #ifdef LIN_INPUT
       return( (BOOL)LIN_IsReadyToSleep() );
      #else
       return ( InputPWM_PwmDisappeared() );
      #endif  /* LIN_INPUT */
    #endif  /* AN_INPUT */
  #else
    #ifndef PWM_INPUT                         /* PWM input over analog pin has to be checked only if PWM_INPUT is undefined! If PWM_INPUT and AN_INPUT are both defined, test phase has checked at pwm input */
      #ifdef AN_INPUT
       return ( InputAN_PwmDisappeared() );
      #else
       return ( FALSE );                      /* PWM_INPUT and AN_INPUT are both undefined. */
      #endif  /* PWM_INPUT */
    #endif  /* AN_INPUT */
  #endif  /* PWM_INPUT */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputSP_GetPWMDutyAndPeriod(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Manages frequency and duty cycle computation.  
    \details This function performs:\n
    Performs duty cycle computing basing on \ref bufstrPWMInData[\ref u8PWMindex] values.
    Over this buffer means is first execute before sending duty cycle computation result.
    If this interrupt is not colled for more than \ref MAX_NUM_NO_DUTY * 8msec
    (RTI time based) pwm input is lost.
    Calling \ref InputSP_StatePwmInFunction is a way to implement a little state machine that rules the Input PWM transfert function.
    This state machine is the following.
    
     \dot    digraph GetSetPoint {

          	node [shape=box, style=filled, fontname=Arial, fontsize=10];  
        	compound=true;
		subgraph cluster0{
	            STOP[ label="STOP", color="red",shape=doubleoctagon];
			STOP->STOP[label=" u8MeanPWM<=PWM_IN_HALT_TO_MIN\n return 0", fontsize=8];         	

			label="PwmInStopFn";fontsize=10;
            }                  
		subgraph cluster1{
		    	MIN[ label="MIN", color="blue",shape=doubleoctagon];  
			MIN->MIN[label=" u8MeanPWM>=PWM_IN_MIN_TO_HALT &\n u8MeanPWM<=PWM_IN_MIN_TO_NOMINAL \n return 0", fontsize=8];         	

			label="PwmInMinFn";fontsize=10;
     		}
		subgraph cluster2{
		    	NORMAL[ label="NORMAL", color="darkgreen",shape=doubleoctagon];  
			NORMAL->NORMAL[label=" u8MeanPWM>=PWM_IN_NOMINAL_TO_MIN &\n u8MeanPWM<=PWM_IN_NOMINAL_TO_MAX \n return f(A*u8MeanPWM+B)", fontsize=8];         	
			label="PwmInNominalFn";fontsize=10;
     		}
		subgraph cluster3{
		    	MAX[ label="MAX", color="blue",shape=doubleoctagon];  
			MAX->MAX[label=" u8MeanPWM>=PWM_IN_MAX_TO_NOMINAL &\n u8MeanPWM<=PWM_IN_MAX_TO_EMERGENCY \n return SP_MAX_SPEED_VALUE", fontsize=8];         	

			label="PwmInMinFn";fontsize=10;
     		}
		subgraph cluster4{
		    	EMER[ label="EMERGENCY", color="red",shape=doubleoctagon];  
			EMER->EMER[label=" u8MeanPWM>=PWM_IN_EMERGENCY_TO_MAX \n return 0", fontsize=8];         	
			label="PwmInNominalFn";fontsize=10;
     		}
 			STOP->MIN[label=" u8MeanPWM>PWM_IN_HALT_TO_MIN\n return SP_MIN_SPEED_VALUE", fontsize=8];         	
			MIN->STOP[label=" u8MeanPWM<PWM_IN_MIN_TO_HALT\n return 0", fontsize=8];         	
			MIN->NORMAL[label=" u8MeanPWM>PWM_IN_MIN_TO_NOMINAL\n return SP_MIN_SPEED_VALUE", fontsize=8];         	
			NORMAL->MIN[label=" u8MeanPWM<PWM_IN_NOMINAL_TO_MIN\n return SP_MIN_SPEED_VALUE", fontsize=8];
			NORMAL->MAX[label=" u8MeanPWM>PWM_IN_NOMINAL_TO_MAX\n return SP_MAX_SPEED_VALUE", fontsize=8]:
			MAX->NORMAL[label=" u8MeanPWM<PWM_IN_MAX_TO_NOMINAL\n return SP_MAX_SPEED_VALUE", fontsize=8]:
			MAX->EMER[label=" u8MeanPWM>PWM_IN_MAX_TO_EMERGENCY\n return 0", fontsize=8]:
			EMER->MAX[label=" u8MeanPWM<PWM_IN_EMERGENCY_TO_MAX\n return SP_MAX_SPEED_VALUE", fontsize=8]:
       }\enddot
                            \par Used Variables
    \ref PWMInFlag This variable is a union used to manage PWM measurements \n
    \ref u32TempPeriod Buffer for the measured period \n
    \ref u32TempDuty Buffer for the calculated duty cycle \n
    
    \return \b u8 duty cycle value scaled from 0 to 255.
    \note None.
    \warning Attenzione occorre circa un tempo pari a 2 sec per acquisire 4 duty 
             cycle con frequenza 5Hz (vedi capitolato Porsche).
*/
void InputSP_GetPWMDutyAndPeriod(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8  u8Tempindex;
  u32 u32TempDuty_tmp;

  u8Tempindex = u8PWMindex;                         /* Save array index to avoid changing by interrupt */

  do                                                /* Find previous odd */
  {
    u8Tempindex--;
    u8Tempindex&=(DIM_BUFFER_PWM_IN-(u8)1); 
  }
  while ((u8Tempindex & ((u8)(0x01))) == (u8)0);   

  u32TempPeriod=((u32)(bufstrPWMInData[u8Tempindex].u8Ovf)<<16);
  u32TempPeriod+=bufstrPWMInData[u8Tempindex].u16Val;

  u8Tempindex--;
  /*u8Tempindex&=(DIM_BUFFER_PWM_IN-(u8)1);*/ /* The result is always that of the left-hand operand so this operation is not necessary*/

  InputSP_Set_u32TempDuty(((u32)(bufstrPWMInData[u8Tempindex].u8Ovf)<<16));
  u32TempDuty_tmp = InputSP_Get_u32TempDuty()+ bufstrPWMInData[u8Tempindex].u16Val;
  InputSP_Set_u32TempDuty(u32TempDuty_tmp);

  u32TempPeriod-=bufstrPWMInData[u8Tempindex].u16Val;

  u8Tempindex--;
  u8Tempindex&=(DIM_BUFFER_PWM_IN-(u8)1);

  u32TempDuty_tmp = InputSP_Get_u32TempDuty() - bufstrPWMInData[u8Tempindex].u16Val;
  InputSP_Set_u32TempDuty(u32TempDuty_tmp);
  u32TempDuty_tmp = InputSP_Get_u32TempDuty() + u32TempPeriod;
  u32TempPeriod   = u32TempDuty_tmp;

  u32TempDuty_tmp = InputSP_Get_u32TempDuty() * (u8)255;
  InputSP_Set_u32TempDuty(u32TempDuty_tmp);
  u32TempDuty_tmp = InputSP_Get_u32TempDuty() / u32TempPeriod;
  InputSP_Set_u32TempDuty(u32TempDuty_tmp);
}

#ifndef NO_INPUT
/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputSP_FindOperatingMode(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set \ref enumOperatingMode variable basing on PWM input and frequency.
    \details When called just set \ref enumOperatingMode basing on following table:\n
    1. \ref enumOperatingMode = \ref BALAN_TEST if 1080Hz<Fpwm<1120 58%<duty<62% is recognize.\n
    2. \ref enumOperatingMode = \ref TORQUE_TEST if 1080Hz<Fpwm<1120 28%<duty<32% is recognize.\n
    3. \ref enumOperatingMode = \ref NORMAL_OPERATION otherwise.\n
                                            \par Used Variables
    \ref u32TempPeriod Period of pwm input signal.\n
    \ref u32TempDuty Duty cycle of pwm input signal.\n
    \ref enumOperatingMode Tells if motor has to run in  production or normal behaviour.\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void InputSP_FindOperatingMode(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  if( ( u32TempPeriod > SP_IN_FREQ_PRODUCTION_LOW_BIT ) && ( u32TempPeriod < SP_IN_FREQ_PRODUCTION_HIGH_BIT ) )
  {
    InputSP_CheckDutyAndSetMode();
  }
  else
  {
    InputSP_SetOperatingMode(NORMAL_OPERATION);
  }
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputSP_CheckDutyAndSetMode(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio  \n <em>Reviewer</em>

    \date 04/12/2013
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set \ref enumOperatingMode variable basing on the PWM duty cycle.
    \details When called just set \ref enumOperatingMode basing on the following table:\n
    1. \ref enumOperatingMode = \ref TORQUE_TEST if 10 % < duty < 15% is recognize.\n
    2. \ref enumOperatingMode = \ref BALAN_TEST  if 20 % < duty < 80% is recognize.\n
    2. \ref enumOperatingMode = \ref DEBUG_TEST  if 90 % < duty < 95% is recognize.\n
    4. \ref enumOperatingMode = \ref NORMAL_OPERATION otherwise.\n
                                            \par Used Variables
    \ref u32TempPeriod Period of pwm input signal.\n
    \ref u32TempDuty Duty cycle of pwm input signal.\n
    \ref enumOperatingMode Tells if motor has to run in  production or normal behaviour.\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
#ifndef PWM_TO_ANALOG_IN
static void InputSP_CheckDutyAndSetMode(void)
#else
void InputSP_CheckDutyAndSetMode(void)
#endif
{
  u32 u32TempDuty_tmp;

  u32TempDuty_tmp = InputSP_Get_u32TempDuty();

  if( ( u32TempDuty_tmp > SP_IN_DUTY_TORQUE_LOW_BIT ) && ( u32TempDuty_tmp < SP_IN_DUTY_TORQUE_HIGH_BIT ) )
  {
    InputSP_SetOperatingMode(TORQUE_TEST);
  }
  else
  {
    if( ( u32TempDuty_tmp > SP_IN_HALT_TO_MIN_TEST_MODE2 ) && ( u32TempDuty_tmp < SP_IN_MAX_TO_OFF_TEST_MODE2 ) )
    {
      InputSP_SetOperatingMode(BALAN_TEST);
    }
    else
    {
      if( ( u32TempDuty_tmp > SP_IN_DUTY_DEBUG_LOW_BIT ) && ( u32TempDuty_tmp < SP_IN_DUTY_DEBUG_HIGH_BIT ) )
      {
        InputSP_SetOperatingMode(DEBUG_TEST);
      }
      else
      {
        InputSP_SetOperatingMode(NORMAL_OPERATION);
      }
    }
  }
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputSP_PwmInStopFn(u8 u8MeanPWM)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function is called to manage pwm and so motor running state ( \ref enumPwmInState ).  
    \details This function return:\n
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is greater than \ref SP_IN_HALT_TO_MIN. In this case \ref enumPwmInState is set to \ref MIN_SPEED\n 
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == BALAN_TEST 
      and duty cycle is greater than \ref SP_IN_HALT_TO_MIN_TEST_MODE2. In this case \ref enumPwmInState is set to \ref MIN_SPEED\n 
    - \ref SP_MAX_SPEED_VALUE if \ref enumOperatingMode == TORQUE_TEST. In this case \ref enumPwmInState is set to \ref MAX_SPEED\n  
    - zero otherwise.\n
    \param[in] u8MeanPWM Duty cycle converted value \n
                            \par Used Variables
    \return \b u8 Duty cycle min value.
    \note None.
    \warning None.
*/
static u8 InputSP_PwmInStopFn(u8 u8MeanPWM)
{
  u8 ret;
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    if ( u8MeanPWM > SP_IN_HALT_TO_MIN )
    {
      InputSP_Set_enumPwmInState(MIN_SPEED);
      ret = (u8)SP_MIN_SPEED_VALUE;
    }
    else if ( u8MeanPWM <= SP_IN_HALT_TO_EMERGENCY )
    {
      InputSP_Set_enumPwmInState(EMERGENCY);
      ret = (u8)SP_MAX_SPEED_VALUE;
    }
    else
    {
      ret = (u8)0;
    }
  }
  else if ( InputSP_GetOperatingMode() == BALAN_TEST )
  {
    if ( u8MeanPWM > SP_IN_HALT_TO_MIN_TEST_MODE2 )
    {
      InputSP_Set_enumPwmInState(MIN_SPEED);
      ret = (u8)SP_MIN_SPEED_VALUE_BALANCE;
    }
    else
    {
      ret = (u8)0;
    }
  }
  else if ( InputSP_GetOperatingMode() == DEBUG_TEST )
  {
      ret = (u8)0;
  }
  else /* enumOperatingMode == TORQUE_TEST -> set max speed */
  {
    InputSP_Set_enumPwmInState(MAX_SPEED);
    ret = (u8)SP_MAX_SPEED_VALUE;
  }
  return (ret);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputSP_PwmInMinFn(u8 u8MeanPWM)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function is called to manage pwm and so motor running state ( \ref enumPwmInState ).  
    \details This function return:\n
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is greater than \ref SP_IN_MIN_TO_NOMINAL. In this case \ref enumPwmInState is set to \ref NOMINAL_SPEED.\n 
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is amoung \ref SP_IN_MIN_TO_HALT \ref SP_IN_MIN_TO_NOMINAL.
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == BALAN_TEST 
      and duty cycle is greater than \ref SP_IN_MIN_TO_NOMINAL_TEST_MODE2. In this case \ref enumPwmInState is set to \ref NOMINAL_SPEED.\n 
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == BALAN_TEST 
      and duty cycle is amoung \ref SP_IN_HALT_TO_MIN_TEST_MODE2 \ref SP_IN_MIN_TO_NOMINAL_TEST_MODE2.\n 
    - \ref SP_MAX_SPEED_VALUE if \ref enumOperatingMode == TORQUE_TEST. In this case \ref enumPwmInState is set to \ref MAX_SPEED.\n  
    - zero otherwise. In this case \ref enumPwmInState is set to \ref HALTED.\n
    \param[in] u8MeanPWM Duty cycle converted value \n
                            \par Used Variables
    \return \b u8 duty cycle min value.
    \note None.
    \warning None.
*/
static u8 InputSP_PwmInMinFn(u8 u8MeanPWM)
{
  u8 ret;
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    if ( u8MeanPWM < SP_IN_MIN_TO_HALT )
    {
      if ( u8MeanPWM <= SP_IN_HALT_TO_EMERGENCY )
      {
        InputSP_Set_enumPwmInState(EMERGENCY);
        ret = (u8)SP_EMERGENCY_SPEED;
      }
      else
      {
        InputSP_Set_enumPwmInState(HALTED);
        ret = (u8)0; 
      }
    }
    else if ( u8MeanPWM > SP_IN_MIN_TO_NOMINAL )
    {
      InputSP_Set_enumPwmInState(NOMINAL_SPEED);
      ret = (u8)SP_MIN_SPEED_VALUE;
    }
    else
    {
      ret = (u8)SP_MIN_SPEED_VALUE;
    }
  }
  else if ( InputSP_GetOperatingMode() == BALAN_TEST )
  {
    if ( u8MeanPWM < SP_IN_HALT_TO_MIN_TEST_MODE2 )
    {
      InputSP_Set_enumPwmInState(HALTED);
      ret = (u8)0; 
    }
    else if ( u8MeanPWM > SP_IN_MIN_TO_NOMINAL_TEST_MODE2 )
    {
      InputSP_Set_enumPwmInState(NOMINAL_SPEED);
      ret = (u8)SP_MIN_SPEED_VALUE_BALANCE; 
    }
    else
    {
      ret = (u8)SP_MIN_SPEED_VALUE_BALANCE;
    }
  }
  else if ( InputSP_GetOperatingMode() == DEBUG_TEST )
  {
    ret = (u8)0;
  }
  else /* enumOperatingMode == TORQUE_TEST -> set max speed */
  {
    InputSP_Set_enumPwmInState(MAX_SPEED);
    ret = (u8)SP_MAX_SPEED_VALUE;
  }
  return (ret);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputSP_PwmInNominalFn(u8 u8MeanPWM)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function is called to manage pwm and so motor running state ( \ref enumPwmInState ).  
    \details This function return:\n
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is less than \ref SP_IN_NOMINAL_TO_MIN. In this case \ref enumPwmInState is set to \ref MIN_SPEED.\n 
    - \ref SP_MIN_SPEED_VALUE if \ref enumOperatingMode == BALAN_TEST 
      and duty cycle is less than \ref SP_IN_MIN_TO_NOMINAL_TEST_MODE2. In this case \ref enumPwmInState is set to \ref MIN_SPEED.\n 
    - \ref SP_MAX_SPEED_VALUE if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is greater than \ref SP_IN_NOMINAL_TO_MAX. In this case \ref enumPwmInState is set to \ref MAX_SPEED.\n  
    - \ref SP_MAX_SPEED_VALUE if \ref enumOperatingMode == BALAN_TEST
      and duty cycle is greater than \ref SP_IN_NOMINAL_TO_MAX_TEST_MODE2. In this case \ref enumPwmInState is set to \ref MAX_SPEED.\n  
    - Linear interpolation amoung \ref SP_MIN_SPEED_VALUE to \ref SP_MAX_SPEED_VALUE (y axis) and \ref SP_IN_NOMINAL_TO_MIN to \ref SP_IN_NOMINAL_TO_MAX (x axis)
      if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST.
    - Linear interpolation amoung \ref SP_MIN_SPEED_VALUE to \ref SP_MAX_SPEED_VALUE (y axis) and \ref SP_IN_MIN_TO_NOMINAL_TEST_MODE2 to \ref SP_IN_NOMINAL_TO_MAX_TEST_MODE2 (x axis)
      if \ref enumOperatingMode == BALAN_TEST.
    - \ref SP_MAX_SPEED_VALUE otherwise. In this case \ref enumPwmInState is set to \ref MAX_SPEED.\n
    \param[in] u8MeanPWM Duty cycle converted value \n
                            \par Used Variables
    \return \b u8 duty cycle min value.
    \note None.
    \warning None.
*/
static u8 InputSP_PwmInNominalFn(u8 u8MeanPWM)
{
  u8 ret;
  u8 u8LimitedSlope;

  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    if ( u8MeanPWM < SP_IN_NOMINAL_TO_MIN )
    {
      InputSP_Set_enumPwmInState(MIN_SPEED);
      ret = (u8)SP_MIN_SPEED_VALUE;
    }
    else if ( u8MeanPWM > SP_IN_NOMINAL_TO_MAX )
    {
      InputSP_Set_enumPwmInState(MAX_SPEED);
      ret = (u8)SP_MAX_SPEED_VALUE;
    }
    else
    {
      u8LimitedSlope = ((u8)(((u16)((SP_MAX_SPEED_VALUE - SP_MIN_SPEED_VALUE)*(u16)128))/((u8)(SP_IN_NOMINAL_TO_MAX - SP_IN_NOMINAL_TO_MIN)))); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/
      ret = (u8)(SP_MIN_SPEED_VALUE + (u8)((u16)u8LimitedSlope*((u16)u8MeanPWM-(u16)SP_IN_NOMINAL_TO_MIN)/(u16)128) );
    }
  }
  else if ( InputSP_GetOperatingMode() == BALAN_TEST )
  {
    if ( u8MeanPWM < SP_IN_MIN_TO_NOMINAL_TEST_MODE2 )
    {
      InputSP_Set_enumPwmInState(MIN_SPEED);
      ret = (u8)SP_MIN_SPEED_VALUE_BALANCE; 
    }
    else if ( u8MeanPWM > SP_IN_NOMINAL_TO_MAX_TEST_MODE2 )
    {
      InputSP_Set_enumPwmInState(MAX_SPEED);
      ret = (u8)SP_MAX_SPEED_VALUE_BALANCE; 
    }
    else
    {
      u8LimitedSlope = ((u8)(((u16)((SP_MAX_SPEED_VALUE_BALANCE - SP_MIN_SPEED_VALUE_BALANCE)*(u16)128))/((u8)(SP_IN_NOMINAL_TO_MAX_TEST_MODE2 - SP_IN_MIN_TO_NOMINAL_TEST_MODE2)))); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/ 
      ret = (u8)((u8)SP_MIN_SPEED_VALUE_BALANCE + (u8)((u16)u8LimitedSlope*((u16)u8MeanPWM-(u16)SP_IN_MIN_TO_NOMINAL_TEST_MODE2)/(u16)128) );
    }
  }
  else if ( InputSP_GetOperatingMode() == DEBUG_TEST )
  {
    ret = (u8)0;
  }
  else /* enumOperatingMode == TORQUE_TEST -> set max speed */
  {
    InputSP_Set_enumPwmInState(MAX_SPEED);
    ret = (u8)SP_MAX_SPEED_VALUE;
  }
  return (ret);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputSP_PwmInMaxFn(u8 u8MeanPWM)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function is called to manage pwm and so motor running state ( \ref enumPwmInState ).  
    \details This function return:\n
    - \ref SP_MAX_SPEED_VALUE if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is less than \ref SP_IN_MAX_TO_NOMINAL. In this case \ref enumPwmInState is set to \ref NOMINAL_SPEED.\n 
    - \ref SP_MAX_SPEED_VALUE if \ref enumOperatingMode == BALAN_TEST 
      and duty cycle is less than \ref SP_IN_NOMINAL_TO_MAX_TEST_MODE2. In this case \ref enumPwmInState is set to \ref NOMINAL_SPEED.\n 
    - Zero if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is greater than \ref SP_IN_MAX_TO_EMERGENCY. In this case \ref enumPwmInState is set to \ref EMERGENCY.\n  
    - Zero if \ref enumOperatingMode == BALAN_TEST
      and duty cycle is greater than \ref SP_IN_MAX_TO_OFF_TEST_MODE2. In this case \ref enumPwmInState is set to \ref HALTED.\n
    - \ref SP_MAX_SPEED_VALUE otherwise. If \ref enumOperatingMode == TORQUE_TEST \ref enumPwmInState is set to \ref MAX_SPEED.\n
    \param[in] u8MeanPWM Duty cycle converted value \n
                            \par Used Variables
    \return \b u8 duty cycle min value.
    \note None.
    \warning None.
*/
static u8 InputSP_PwmInMaxFn(u8 u8MeanPWM)
{
  u8 ret;
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    if ( u8MeanPWM < SP_IN_MAX_TO_NOMINAL )
    {
      InputSP_Set_enumPwmInState(NOMINAL_SPEED);
      ret = (u8)SP_MAX_SPEED_VALUE; 
    }
    /*
    else if ( u8MeanPWM > SP_IN_MAX_TO_EMERGENCY )
    {
      InputSP_Set_enumPwmInState(EMERGENCY);
      return ( SP_EMERGENCY_SPEED ); 
    }
    */
    else
    {
      ret = (u8)SP_MAX_SPEED_VALUE; 
    }
  }
  else if ( InputSP_GetOperatingMode() == BALAN_TEST )
  {
    if ( u8MeanPWM < SP_IN_NOMINAL_TO_MAX_TEST_MODE2 )
    {
      InputSP_Set_enumPwmInState(NOMINAL_SPEED);
      ret = (u8)SP_MAX_SPEED_VALUE_BALANCE; 
    }
    else if ( u8MeanPWM > SP_IN_MAX_TO_OFF_TEST_MODE2 )
    {
      InputSP_Set_enumPwmInState(HALTED);
      ret = (u8)0; 
    }
    else
    { 
      ret = (u8)SP_MAX_SPEED_VALUE_BALANCE;
    }
  }
  else if ( InputSP_GetOperatingMode() == DEBUG_TEST )
  {
    ret = (u8)0;
  }
  else /* enumOperatingMode == TORQUE_TEST -> set max speed */
  {
    InputSP_Set_enumPwmInState(MAX_SPEED);
    ret = (u8)SP_MAX_SPEED_VALUE; 
  }
  return (ret);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 InputSP_PwmInEmergencyFn(u8 u8MeanPWM)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function is called to manage pwm and so motor running state ( \ref enumPwmInState ).  
    \details This function return:.\n
    - \ref SP_MAX_SPEED_VALUE if \ref enumOperatingMode == NORMAL_OPERATION or \ref enumOperatingMode == DEBUG_TEST 
      and duty cycle is less than \ref SP_IN_EMERGENCY_TO_MAX. In this case \ref enumPwmInState is set to \ref MAX_SPEED.\n 
    - zero otherwise. In this case \ref enumPwmInState is set to \ref HALTED if \ref enumOperatingMode == TORQUE_TEST or \ref enumOperatingMode == BALAN_TEST.\n
    It manages changing state ( \ref enumPwmInState ) as well.\n
    \param[in] u8MeanPWM Duty cycle converted value.\n
                            \par Used Variables
    \return \b u8 duty cycle min value.
    \note None.
    \warning None.
*/
static u8 InputSP_PwmInEmergencyFn(u8 u8MeanPWM)
{
  u8 ret;
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    if ( u8MeanPWM > SP_IN_EMERGENCY_TO_HALT )
    {
      if ( u8MeanPWM > SP_IN_HALT_TO_MIN )
      {
        InputSP_Set_enumPwmInState(MAX_SPEED);
        ret = (u8)SP_MAX_SPEED_VALUE; 
      }
      else
      {
        InputSP_Set_enumPwmInState(HALTED);
        ret = (u8)0;
      }
    }
    else
    {
      ret = (u8)SP_EMERGENCY_SPEED;
    }
  }
  else /* enumOperatingMode == TORQUE_TEST -> set max speed */
  {
    InputSP_Set_enumPwmInState(HALTED);
    ret = (u8)0;
  }
  return (ret);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputSP_PerformPlausabilityCheck(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>

    \date 30/05/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Perform plausibility check over pwm input period and duty cycle.  
    \details Perform plausibility check over pwm input period and duty cycle.\n
    <a href="file://///Galileo/elettronica/_RUBINI/Pwm in Fdt/PWM input.vsd"> PWM input.vsd </a>
                            \par Used Variables
    \ref PWMInFlag. Defined as \ref tPWMInFlag. Its flags define pwm input decoding state.\n
    \return \b TRUE if plausibility has good result.
    \note None.
    \warning None.
*/
BOOL InputSP_PerformPlausabilityCheck(void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/ /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u32 u32TempDuty_tmp;

  boolPlausResult = FALSE;

  if ( u8BoolPlausCount < PWM_IN_DISCARDED_DUTY )
  {
    u8BoolPlausCount++;
    u8PeriodCount++;
    boolPlausResult = FALSE; /*return (FALSE);*/
  }
  else if ( u8BoolPlausCount == PWM_IN_DISCARDED_DUTY )
  {
    u8PWMinDutyZero = (u8)InputSP_Get_u32TempDuty(); /* Update duty. */
    u8BoolPlausCount++;
  } else
  {
    Nop(); /*PRQA S 1006 #Necessary assembly code*/
  }

  if( u8PeriodCount >= PWM_IN_PERIOD_EVALUATING_NUMBER )
  {
    if ( ( u8GoodPeriodCount >= PWM_IN_VALID_PERIOD_NUMBER ) && ( u8GoodDutyCount >= PWM_IN_VALID_DUTY_NUMBER ) )
    {
      InputSP_Set_u8PWMinDuty((u8)((u16)u16PWMDutyAmount/(u8)u8GoodDutyCount)); /* Update duty with mean value over good duty found. */
      boolPlausResult = TRUE;
    }
    else
    {
      Nop(); /*PRQA S 1006 #Necessary assembly code*/
    }

    u8PeriodCount     = 0;
    u8GoodPeriodCount = 0;
    u8GoodDutyCount   = 0;
    u16PWMDutyAmount  = 0;
    u8PWMinDutyZero   = (u8)InputSP_Get_u32TempDuty(); /* Update duty. */
  }

  if ( ( u32TempPeriod > PWM_IN_MIN_PERIOD_TIMER_COUNT ) && ( u32TempPeriod < PWM_IN_MAX_PERIOD_TIMER_COUNT ) )
  {
    u8 u8PWMinDutyMin;

    u8GoodPeriodCount++;

    if ( u8PWMinDutyZero > PWM_IN_DUTY_TOL_NUMBER )
    {
      u8PWMinDutyMin = (u8)((u8)u8PWMinDutyZero-PWM_IN_DUTY_TOL_NUMBER);
    }
    else
    {
      u8PWMinDutyMin = 0;
    }

    u32TempDuty_tmp = InputSP_Get_u32TempDuty();

    if ( ( (u8)u32TempDuty_tmp >= u8PWMinDutyMin ) && ( (u8)u32TempDuty_tmp < (u16)((u16)u8PWMinDutyZero+PWM_IN_DUTY_TOL_NUMBER) ) )
    {
      u32TempDuty_tmp = InputSP_Get_u32TempDuty() + u16PWMDutyAmount;
      u16PWMDutyAmount = (u16)u32TempDuty_tmp;
      u8GoodDutyCount++;
    }
    else
    {
       Nop(); /*PRQA S 1006 #Necessary assembly code*/
    }
  }
  else
  {
      Nop(); /*PRQA S 1006 #Necessary assembly code*/
  }

  u8PeriodCount++; /* u8PeriodCount is incremented only if READY_CALC is true i.e. after a valid pwm input falling edge detection. */

  return (boolPlausResult);
}

#if ((defined(PWM_INPUT)) && !(defined(PWM_TO_ANALOG_IN)))
/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputSP_InitPlausabilityCheck(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>

    \date 30/05/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief reset plausibility check over pwm input period and duty cycle.  
    \details Perform plausibility check over pwm input period and duty cycle.\n
    <a href="file://///Galileo/elettronica/_RUBINI/Pwm in Fdt/PWM input.vsd"> PWM input.vsd </a>
                            \par Used Variables
    \ref PWMInFlag. Defined as \ref tPWMInFlag. Its flags define pwm input decoding state.\n
    \return \b TRUE if plausibility has good result.
    \note None.
    \warning None.
*/
void InputSP_InitPlausabilityCheck(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
  {
    u8PeriodCount     = 0;
    u8GoodPeriodCount = 0;
    u8GoodDutyCount   = 0;
    u16PWMDutyAmount  = 0;
    u8BoolPlausCount  = 0;
  }

#endif

#ifdef PWM_TO_ANALOG_IN
u8 InputSP_CheckPeriodCount (void)
{
  return((u8)u8PeriodCount);
}
#endif

#if (defined(PWM_INPUT) && defined(AN_INPUT))
/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputSP_CheckInputStatus(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 22/11/2012
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Select PWM or ANALOG channel, according to input signal type.  
    \details If plausibility check fails and there are no edges, this function switches for PWM to ANALOG INPUT.\n
    <a href="file://///Galileo/elettronica/_RUBINI/Pwm in Fdt/PWM input.vsd"> PWM input.vsd </a>
                            \par Used Variables
    \ref enumInputStatus     Defined as \ref InputMode. Input type.\n
    \ref u8NumNoDuty         Defined as \ref u8. Number of RTI ovf where the Input PWM signal is absent.\n
    \return \b void          No value returned by this function
    \note None.
    \warning None.
*/
static void  InputSP_CheckInputStatus (void)
{
  switch(enumInputStatus)
  {
    case PWM:
      if ( ( InputSP_Get_u8NumNoDuty() >= PWM_IN_DISAPPEAR_NORMAL_OP_NUM) && (boolPlausResult == (BOOL)FALSE) )
      {
         enumInputStatus = ANALOG;
         InputSP_InitPlausabilityCheck();
      }
    break;

    case ANALOG:
      if ( boolPlausResult != (BOOL)FALSE )
      {
        enumInputStatus = PWM;
      }
    break;

    default:
      /*Nop();*/
    break;
   }
}
#endif  /* PWM_INPUT && AN_INPUT */

#ifdef RVS 

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputPWM_GetPWMInputFdT(u8)
    \author Boschesi Roberto \n <em>Developer</em> 
    \author Guazzetti Alice \n  <em>Developer</em> 

    \date 03/12/2013
                            \par Starting SW requirement
    <a href="file://///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return duty cycle value filtered by the mapping of relative transfer function
    \details This function does:\n
    In the first part of the function there's the personalization according to the transfer function to be implemented; 
    a certain number of flags of hysteresis must be defined for the management of this part.   
    In the second part of the function there's the state machine that manages the diagnostic's states. 
    \par Used Variables
    \ref u8PWMRead: u8 input duty cycle value \n
    \ref u8RequestSpeed: Variable that identifies the value of the duty cycle 
     already filtered by the implentation of the transfer function \n
    \ref u8SpeedForward: Variable that identifies the direction of rotation of the motor \n
    \return \b u8 duty cycle value scaled from 0 to 255.
    \note None.

*/
#define FORWARD_DIRECTION ((BOOL)1)
#define BACKWARD_DIRECTION ((BOOL)0)

u8 InputSP_GetRevInputFdT(u8 u8PWMRead) 
{
  u8 u8LimitedSlope;
  u8 u8RequestSpeed;
  u8 u8SpeedForward;
  u16 rampGenerator_u16FreqRampPU_tmp;

  /*Personalize your FdT*/
  if ((u8PWMRead <=  PWM_LIM_0) || ((u8PWMRead < PWM_LIM_1) && !BitTest(HYST_STOP_TO_BWD_FLAG ,u8SetpointHystFlags)))
  {
    /*STOP*/
    BitClear(HYST_STOP_TO_BWD_FLAG,u8SetpointHystFlags);
    BitSet(HYST_FWD_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_MAX_TO_EMG_FLAG,u8SetpointHystFlags);
    u8RequestSpeed=0;
    u8SpeedForward=FORWARD_DIRECTION;
  }
  else if ((u8PWMRead <= PWM_LIM_2) || ((u8PWMRead < PWM_LIM_3) && (BitTest(HYST_FWD_TO_BWD_FLAG ,u8SetpointHystFlags))))
  {
    /*BACKWARD*/
    BitSet(HYST_STOP_TO_BWD_FLAG,u8SetpointHystFlags);
    BitSet(HYST_FWD_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_MAX_TO_EMG_FLAG,u8SetpointHystFlags);
    u8RequestSpeed=((u8)(I0_2));
    u8SpeedForward=BACKWARD_DIRECTION;
  }
  else if (u8PWMRead <= PWM_LIM_4)
  {
    /*MIN_SPEED*/
    BitSet(HYST_STOP_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_FWD_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_MAX_TO_EMG_FLAG,u8SetpointHystFlags);
    u8RequestSpeed=((u8)(I0_0)); 
    u8SpeedForward=FORWARD_DIRECTION;
  }
  else if (u8PWMRead < PWM_LIM_5)
  {
    /*LINEAR*/
    BitSet(HYST_STOP_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_FWD_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_MAX_TO_EMG_FLAG,u8SetpointHystFlags);
    u8LimitedSlope = ((u8)(((u16)((I0_1 - I0_0)*64))/((u8)(PWM_LIM_5 - PWM_LIM_4))));
    u8RequestSpeed=((u8)(I0_0 + (u8)((u16)u8LimitedSlope*(u8PWMRead-PWM_LIM_4)/64)));
    u8SpeedForward=FORWARD_DIRECTION;
  }
  else if ((u8PWMRead <= PWM_LIM_6) || ((u8PWMRead < PWM_LIM_5) && (!BitTest(HYST_MAX_TO_EMG_FLAG ,u8SetpointHystFlags))))
  {
    /*MAX*/
    BitSet(HYST_STOP_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_FWD_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_MAX_TO_EMG_FLAG,u8SetpointHystFlags);
    u8RequestSpeed=((u8)(I0_1));
    u8SpeedForward=FORWARD_DIRECTION;
  }
  else
  {
    /*EMERGENCY*/
    BitSet(HYST_STOP_TO_BWD_FLAG,u8SetpointHystFlags);
    BitClear(HYST_FWD_TO_BWD_FLAG,u8SetpointHystFlags);
    BitSet(HYST_MAX_TO_EMG_FLAG,u8SetpointHystFlags);
    u8RequestSpeed=((u8)0);
    u8SpeedForward=FORWARD_DIRECTION;
  }

  /* New implementation of the State Machine, indipendent of the FdT implemented */
  /* First check to control if it's required to stop the motor                   */
  if (u8RequestSpeed == 0)
  {
    enumSpeedStatus = SPEED_STOP; 
  }
  else
  {
    /*Nop*/
  }

  switch ( enumSpeedStatus )
  {
    case SPEED_RUN:

      /*Check if it's required to change the direction of the rotation while the motor running*/
      if ((u8SpeedForward==FORWARD_DIRECTION  && (!BitTest(FWD_BWD_DIR,u8MTCStatus))) || 
      (u8SpeedForward==BACKWARD_DIRECTION  && (BitTest(FWD_BWD_DIR,u8MTCStatus))) )
      {
        enumSpeedStatus = SPEED_REV;
        return(u8RequestSpeed);
      }
      else
      {
        return(u8RequestSpeed);
      }
    break;

    case SPEED_REV:
     
      /* Check if the direction is back concordant with the before direction */
      /* If so, return directly to SPEED_RUN without stopping                */

      rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();

      if (
             (u8SpeedForward==FORWARD_DIRECTION  && (BitTest(FWD_BWD_DIR,u8MTCStatus)))
           ||(u8SpeedForward==BACKWARD_DIRECTION  && (!BitTest(FWD_BWD_DIR,u8MTCStatus)))
      )
      {
        enumSpeedStatus = SPEED_RUN;
        return(u8RequestSpeed); 
      }
      else if ( ( rampGenerator_u16FreqRampPU_tmp >= EL_FREQ_MIN_PU_RES_BIT) && ( rampGenerator_u16FreqRampPU_tmp < MAX_FREQ_TO_BRAKE_PU_RES_BIT) )
      {
        enumSpeedStatus = SPEED_STOP;
        return(0);
      }
      else
      {
        return(I0_0);
      }
    break;

    case SPEED_STOP:

      if (u8RequestSpeed == 0)
      {
        return((u8)0);
      }

      /*Check in which direction the motor must start*/
      else
      {
        if ((u8SpeedForward == FORWARD_DIRECTION))
        {
          SetBit(u8MTCStatus,FWD_BWD_DIR_BIT);
        }
        else
        {
          u8ClrBit(u8MTCStatus,FWD_BWD_DIR_BIT);
        }
      
        /*In any case --> SPEED_RUN*/
        enumSpeedStatus = SPEED_RUN;
        return(u8RequestSpeed); 
      }
    break;

    default:
      EMR_EmergencyDisablePowerStage();
      RESET_MICRO;
    break;
  } /* switch */
}
#endif  /* RVS */

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 InputSP_Get_u8NumNoDuty(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return the number of base interval without input signal
                            \par Used Variables
    \ref u8NumNoDuty \n
    \return \b The number of base interval without input signal.
    \note None.
    \warning None.
*/
u8 InputSP_Get_u8NumNoDuty(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (u8NumNoDuty);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn static void InputSP_Set_u8NumNoDuty(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Set the number of base interval without Input signal  
                            \par Used Variables
    \ref u8NumNoDuty \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void InputSP_Set_u8NumNoDuty(u8 u8Value)
{
    u8NumNoDuty = u8Value;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void InputSP_Set_u32TempDuty(u32 u32Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the number of static u32TempDuty variable (Duty cycle of pwm input signal)
                            \par Used Variables
    \ref u32TempDuty \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void InputSP_Set_u32TempDuty(u32 u32Value) 
{
    u32TempDuty = u32Value;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u32 InputSP_Get_u32TempDuty(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Gets the value of static u32TempDuty variable (Duty cycle of pwm input signal)  
                            \par Used Variables
    \ref u32TempDuty \n
    \return \b The value of static u32TempDuty variable.
    \note None.
    \warning None.
*/
u32 InputSP_Get_u32TempDuty(void)
{
    return (u32TempDuty);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void InputSP_Set_u8PWMinDuty(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the number of static u8PWMinDuty variable (Duty cycle of pwm input signal resulting from plausibility check)
                            \par Used Variables
    \ref u8PWMinDuty \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void InputSP_Set_u8PWMinDuty(u8 u8Value) /*PRQA S 1505 #Could be used when different interfaces types are enabled*/
{
    u8PWMinDuty = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 InputSP_Get_u8PWMinDuty(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Gets the value of static u8PWMinDuty variable (Duty cycle of pwm input signal resulting from plausibility check)  
                            \par Used Variables
    \ref u8PWMinDuty \n
    \return \b The value of static u8PWMinDuty variable.
    \note None.
    \warning None.
*/
u8 InputSP_Get_u8PWMinDuty(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (u8PWMinDuty);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn static void InputSP_Set_enumPwmInState(StatePwmIn_T value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable enumPwmInState
                            \par Used Variables
    \ref enumPwmInState \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void InputSP_Set_enumPwmInState(StatePwmIn_T value)
{
    enumPwmInState = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn StatePwmIn_T InputSP_Get_enumPwmInState(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Gets the value of the static variable enumPwmInState
                            \par Used Variables
    \ref enumPwmInState \n
    \return \b The value of static enumPwmInState variable.
    \note None.
    \warning None.
*/
StatePwmIn_T InputSP_Get_enumPwmInState(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (enumPwmInState);
}

#endif /* NO_INPUT */

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
