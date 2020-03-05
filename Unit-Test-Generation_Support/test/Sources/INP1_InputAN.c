/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputAN.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INP1_InputAN.c
    \version see \ref FMW_VERSION 
    \brief Basic analog input function.
    \details Routines for pwm input operation and variables related managing.\n
    \note none
    \warning none
*/

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register    // Needed for macro definitions
#include ADM0_ADC
#include "main.h"           // Per strDebug
#include "VBA1_Vbatt.h"
#include "MCN1_acmotor.h" // it has to stand before MCN1_mtc.h because of StartStatus_t definition
#include "MCN1_mtc.h"	  	// Needed for u16PWMCount
#include "INP1_InputSP.h"
#include "TIM1_RTI.h"		  // Public general purpose Timebase function prototypes
#include "FIR1_FIR.h"
#include "SOA1_SOA.h"
#include "TMP1_Temperature.h"
#include "INP1_InputAN.h"
#include "WTD1_Watchdog.h"
#include "PARAM1_Param.h"
#include "AUTO1_AutoTest.h"

#ifdef AN_INPUT


static const volatile u8 AN_PARAM V_ANALOG_PARAM_ADDRESS = (u8)AN_PARAM_REF;
static const volatile u16 PWM_TO_AN_PARAM PWM_TO_AN_PARAM_ADDRESS = PWM_TO_AN_PARAM_REF;




/* -------------------------- Private Constants ----------------------------*/
// C_D_ISR_Status bitfield
#if((defined(POTENTIOMETER))||(defined(NTC)))
#define ANALOG_HYST_ON_BIT             (u8)1
#define ANALOG_HYST_ON                 ((u8)((u8)1<<ANALOG_HYST_ON_BIT))
#endif

#if defined(PWM_TO_ANALOG_IN)
#define PWM_TO_ANALOG_LOW_HYST_ON_BIT  (u8)3
#define PWM_TO_ANALOG_LOW_HYST_ON      ((u8)((u8)1<<PWM_TO_ANALOG_LOW_HYST_ON_BIT))

#define PWM_TO_ANALOG_HIGH_HYST_ON_BIT (u8)4
#define PWM_TO_ANALOG_HIGH_HYST_ON     ((u8)((u8)1<<PWM_TO_ANALOG_HIGH_HYST_ON_BIT))
#endif

/* -------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables --------------------------- */
#pragma DATA_SEG DEFAULT
#if(defined(AN_INPUT) && !(defined(PWM_TO_ANALOG_IN)))
u8 AnalogStatus;
#else
static u8 AnalogStatus;
#endif
static u8 u8ParamAN;
#if (defined(PWM_TO_ANALOG_IN))
static u16 u16ParamPWMToAN;
#endif
/* -------------------------- Private functions --------------------------- */
#ifdef POTENTIOMETER
static u16 InputAN_GetAnalog(void);
#endif

#if defined(PWM_TO_ANALOG_IN)
static BOOL InputAN_PwmToAnalogDisappeared(void);
static u16 InputAN_TempCompensation(u16 u16ADSample);
#endif
/* --------------------------- Private macros ------------------------------ */



/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 InputAN_GetANparam(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Save and returns analog parameter trimming value Analog .  
    \details This function return temperature compensating value.\n
    It has been computed as following:\n
    \f[
        T_{param} = ( 25 - T_{amb} ) + T_{ad}
    \f]
    where:\n
    Tparam = stored parameter at \ref T_PARAM location.\n
    Tamb = Ambient temeprature measured by external tool.\n
    Tad = Temperature converted by A/D.\n
                            \par Used Variables
    \return \b u16 T_PARAM temperature compensating value.\n
    \note None.
    \warning None
*/
u8 InputAN_GetANparam(void)
{ 
  u8ParamAN = AN_PARAM;
  return ( (u8)u8ParamAN );
}  

void InputAN_SetDefaultANparam(void)
{
  u8ParamAN = (u8)AN_PARAM_REF;
}
 

/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputAN_InitANInput(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Resets AN input decoding timer and related variables.  
    \details None.\n
                            \par Used Variables
    \return \b void no value return.
    \note None.
    \warning None.
*/
void InputAN_InitANInput(void)
{   
  #ifdef NTC
  INPUTNTC_SET_PULLUP;
  #endif  // NTC
  
  INPUTAN_SET_FALLING_EDGE_SENS;                              // Just after reset, u8PWMindex=0 and so INPUTAN_SET_FALLING_EDGE_SENS will be selected after getting into INT0_InputAN_PWMInputISR.
  AnalogStatus = 0;
  
  INPUTAN_DISABLE_DIGITAL_INPUT;
  
}



#ifdef POTENTIOMETER
/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputAN_GetAnalog(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns analog channel read.  
    \details This function returns analog channel voltage read from ADC.\n
    It takes into account possible tacho happening at function execution time so 
    first assegnation to local variable is done: 
    u16AnVolt = (u16)\ref buffu16ADC_READING[\ref V_BUS_CONV];   
                            \par Used Variables
    \return \b u8 Analog voltage read from ADC.
    \note None.
    \warning None
*/
static u16 InputAN_GetAnalog(void)
{
  //return((u8)(buffu16ADC_READING[AN_CONV]>>2));                                   // This instruction is not good since buffu16ADC_READING:4 and buffu16ADC_READING:5
                                                                                    // could be done in different time so if interrupt occours during this two reading -> an error can happen!
  u32 u32AnVolt;
  u32AnVolt = (u16)buffu16ADC_READING[AN_CONV];                                     // Disassemble:
  u32AnVolt *= u8ParamAN;                                                                                  // LDHX  buffu16ADC_READING:4
  u32AnVolt >>=7;                                                                                 // PSHH
  
  if(u32AnVolt>MAX_ANALOG_AD_INPUT) 
  {
   u32AnVolt=MAX_ANALOG_AD_INPUT;   
  }
  else
  {
    //Nop 
  }
                                                                                    // Even if a writing interrupt occours between the two instruction: no problem.
  return((u16)u32AnVolt);
}
#endif

#ifdef NTC
/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputAN_GetNTCSetPoint(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 30/05/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return analog voltage adc translated.  
    \details By taking into account analog partition ration, defined Fdt is applied .\n
                            \par Used Variables
    \ref AnalogStatus. Used for hysteresys.\n
    \return \b u8 set point translated.
    \note None.
    \warning None.
*/
u8 InputAN_GetNTCSetPoint(void)
{   
  StatePwmIn_T enumPwmInState_tmp;
  u8 ret = (u8)0;

  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {   
    u16 u16AnADC;
    
    u16AnADC = (u16)((u32)((u32)buffu16ADC_READING[AN_CONV]*V_BUS_REFERENCE_BIT)/buffu16ADC_READING[V_BUS_CONV]);

    if ( ( u16AnADC > NTC_TURN_OFF_BIT ) || ( ( u16AnADC >  NTC_TURN_ON_BIT ) && ((BitTest((ANALOG_HYST_ON), AnalogStatus)) == (BOOL)FALSE) ) )
		{
      BitClear((ANALOG_HYST_ON), AnalogStatus);
      ret = (u8)0;
      /*return ( 0 );*/
		} 
		else if (  ( u16AnADC >  NTC_TURN_ON_BIT ) /*&& (BitTest((ANALOG_HYST_ON), AnalogStatus) != (BOOL)FALSE)*/ )  /* Gives  PRQA 2995 (logical operation always TRUE) without comments */
		{
      BitSet((ANALOG_HYST_ON), AnalogStatus);
      ret = (u8)SP_MIN_SPEED_VALUE;
      /*return ( SP_MIN_SPEED_VALUE );*/
		}   
		else if ( u16AnADC >  NTC_MAX_SP_BIT )
		{
		  u8 u8LimitedSlope;
		  
      BitSet((ANALOG_HYST_ON), AnalogStatus);
      
      u8LimitedSlope = (u8)(((u32)(SP_MAX_SPEED_VALUE - SP_MIN_SPEED_VALUE)<<ANALOG_FDT_SLOPE_SHIFT)/((u16)(NTC_TURN_ON_BIT - NTC_MAX_SP_BIT))); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/      
      
      ret = ((u8)(SP_MAX_SPEED_VALUE - (u8)((u16)((u16)u8LimitedSlope*(u16)(u16AnADC-NTC_MAX_SP_BIT))>>NTC_FDT_SLOPE_SHIFT)));
      /*return ((u8)(SP_MAX_SPEED_VALUE - (u8)((u16)((u16)u8LimitedSlope*(u16)(u16AnADC-NTC_MAX_SP_BIT))>>NTC_FDT_SLOPE_SHIFT)));*/
      //return ((u8)(SP_MAX_SPEED_VALUE - (u8)((u16)((u16)NTC_FDT_SLOPE*(u16)(u16AnADC-NTC_MAX_SP_BIT))>>NTC_FDT_SLOPE_SHIFT))); 
		}   
		else
		{
      BitSet((ANALOG_HYST_ON), AnalogStatus);
      ret = (u8)SP_MAX_SPEED_VALUE;
      /*return ( SP_MAX_SPEED_VALUE );*/
		}   
  }
  else
  {
    if (( BitTest((ANALOG_PWM_EDGE_ON_AN), AnalogStatus) ) != (BOOL)FALSE)
    {                                                  
      InputSP_GetPWMDutyAndPeriod();                                                    // Update u32TempDuty and u32TempPeriod

      if (( InputSP_PerformPlausabilityCheck() ) != (BOOL)FALSE)                                         // TREQ044
      {                                                                                 // If plausibility has good result ( even during TEST MODE) set enumOperatingMode to NORMAL_OPERATION and thi is valid 
        InputSP_SetOperatingMode((OperatingMode_t)NORMAL_OPERATION);                    // if only one good period and duty has found.  
      }
      else
      {
        Nop();                                                                          // Misra rule. 60(adv).
      }

      enumPwmInState_tmp = InputSP_Get_enumPwmInState();
      ret = ( InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)(~((u8)InputSP_Get_u32TempDuty()))) );
      /*return ( InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)(~((u8)InputSP_Get_u32TempDuty))) );*/  /* Just use actual duty computed.*/
    }
    else
    {
      Nop();                                                                            // Misra rule. 60(adv).
    }
  }
  return(ret);
}
#endif

#ifdef POTENTIOMETER
/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputAN_GetAnalogSetPoint(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 30/05/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return analog voltage adc translated.  
    \details By taking into account analog partition ration, defined Fdt is applied .\n
                            \par Used Variables
    \ref AnalogStatus. Used for hysteresys.\n
    \return \b u8 set point translated.
    \note None.
    \warning None.
*/
u8 InputAN_GetAnalogSetPoint(void)
{   
  StatePwmIn_T enumPwmInState_tmp;
  u8 ret = (u8)0;

  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {   
    u16 u16AnADC;
    
    //u16AnADC = (u16)((u32)((u32)buffu16ADC_READING[AN_CONV]*V_BUS_REFERENCE_BIT)/buffu16ADC_READING[V_BUS_CONV]);
    u16AnADC = InputAN_GetAnalog();

    if ( ( u16AnADC < ANALOG_TURN_OFF_BIT ) || ( ( u16AnADC <  ANALOG_TURN_ON_BIT ) && ((BitTest((ANALOG_HYST_ON), AnalogStatus)) == (BOOL)FALSE)) )
		{
      BitClear((ANALOG_HYST_ON), AnalogStatus);
      ret = (u8)0;
      /*return ( 0 );*/
		} 
		else if (  ( u16AnADC <  ANALOG_TURN_ON_BIT ) /*&& (BitTest((ANALOG_HYST_ON), AnalogStatus) != (BOOL)FALSE)*/ ) /* Gives  PRQA 2995 (logical operation always TRUE) without comments */
		{
      BitSet((ANALOG_HYST_ON), AnalogStatus);
      ret = (u8)SP_MIN_SPEED_VALUE;
      /*return ( SP_MIN_SPEED_VALUE );*/
		}   
		else if ( u16AnADC <  ANALOG_MAX_SP_BIT )
		{
		  u8 u8LimitedSlope;
		  
      BitSet((ANALOG_HYST_ON), AnalogStatus);
      
      u8LimitedSlope = (u8)(((u32)(SP_MAX_SPEED_VALUE - SP_MIN_SPEED_VALUE)<<ANALOG_FDT_SLOPE_SHIFT)/((u16)(ANALOG_MAX_SP_BIT - ANALOG_TURN_ON_BIT))); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/      
      ret = ((u8)(SP_MIN_SPEED_VALUE + (u8)((u16)((u16)u8LimitedSlope*(u16)(u16AnADC-ANALOG_TURN_ON_BIT))>>ANALOG_FDT_SLOPE_SHIFT)) );
      /*return ((u8)(SP_MIN_SPEED_VALUE + (u8)((u16)((u16)u8LimitedSlope*(u16)(u16AnADC-ANALOG_TURN_ON_BIT))>>ANALOG_FDT_SLOPE_SHIFT)) );*/
      //return ((u8)(SP_MIN_SPEED_VALUE + (u8)((u16)((u16)ANALOG_FDT_SLOPE*(u16)(u16AnADC-ANALOG_TURN_ON_BIT))>>ANALOG_FDT_SLOPE_SHIFT)) ); 
		}   
		else
		{
      BitSet((ANALOG_HYST_ON), AnalogStatus);
      ret =  ( (u8)SP_MAX_SPEED_VALUE );
      /*return ( (u8)SP_MAX_SPEED_VALUE );*/
		}   
  }
  else
  {
    if (( BitTest((ANALOG_PWM_EDGE_ON_AN), AnalogStatus) ) != (BOOL)FALSE)
    {                                                  
      InputSP_GetPWMDutyAndPeriod();                                                    // Update u32TempDuty and u32TempPeriod

      if (( InputSP_PerformPlausabilityCheck() ) != (BOOL)FALSE)                        // TREQ044
      {                                                                                 // If plausibility has good result ( even during TEST MODE) set enumOperatingMode to NORMAL_OPERATION and thi is valid 
        InputSP_SetOperatingMode((OperatingMode_t)NORMAL_OPERATION);                    // if only one good period and duty has found.  
      }
      else
      {
        Nop();                                                                          // Misra rule. 60(adv).
      }

      enumPwmInState_tmp = InputSP_Get_enumPwmInState();
      ret = ( InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)(~((u8)InputSP_Get_u32TempDuty()))) );
      /*return ( InputSP_StatePwmInFunction[enumPwmInState_tmp]((u8)(~((u8)InputSP_Get_u32TempDuty()))) );*/  /* Just use actual duty computed.*/
    }
    else
    {
      Nop();                                                                            // Misra rule. 60(adv).
    }
  }
   return ( ret );
}
#endif

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputAN_GetPwmToAnalogSetPoint(void)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 30/05/2011
    
    \par Starting SW requirement
    "link al requirement"
    \brief Return PWM filered and converted to analog voltage adc translated.  
    \details .\n
    
    \par Used Variables
    \ref AnalogStatus. Used for hysteresys.\n
    \return \b u8 set point translated.
    \note None.
    \warning None.
*/

#if defined(PWM_TO_ANALOG_IN)


/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 InputAN_GetPWMToANparam(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
   
    \date 25/06/2014
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns parameter tuning written into flash.  
    \details This parameter store the result convertion A/D register value for voltage of PWMA input.\n

                            \par Used Variables
    \return \b u16 PWM_TO_AN_PARAM voltage compensating value.\n
    \note None.
    \warning None
*/

       
u16 InputAN_GetPWMToANparam(void)
{ 
  u16ParamPWMToAN = PWM_TO_AN_PARAM;
  return ( (u16)u16ParamPWMToAN);
}


void InputAN_SetDefaultPWMToANparam(void)
{
  u16ParamPWMToAN = PWM_TO_AN_PARAM_REF;
}    


#if defined(RVS)


u8 InputAN_GetPwmToAnalogSetPoint(void)

{  
    u16 u16AnADC;
    u8 u8Setpoint,u8PWMRead; 
    
    u16AnADC = buffu16ADC_READING[PWM_TO_AN_CONV];   
    u16AnADC = ((u16)((u32)((u32)u16AnADC*u16ParamPWMToAN)>>OFFSET_SHIFT)); //Get analog setpoint
    //u16AnADC = u16FIR_IIR(u16AnADC,U16_FIR_INDEX_REF,FIR_REF_RIGHT_SHIFT);
    u8PWMRead= (u8)(u16AnADC>>2);    
    u8Setpoint = InputSP_GetRevInputFdT(u8PWMRead);

    return (u8Setpoint);
}


#else  // RVS Not define


u8 InputAN_GetPwmToAnalogSetPoint(void)
{  
    u16 u16AnADC;
    u8 u8Setpoint; 
    
    u16AnADC = buffu16ADC_READING[PWM_TO_AN_CONV];
    u16AnADC = ((u16)((u32)((u32)u16AnADC*u16ParamPWMToAN)>>OFFSET_SHIFT));
    //u16AnADC = u16FIR_IIR(u16AnADC,U16_FIR_INDEX_REF,FIR_REF_RIGHT_SHIFT);
    
    u16AnADC = InputAN_TempCompensation(u16AnADC);
    
    if ( ( u16AnADC < PWM_TO_ANALOG_TURN_OFF_BIT ) || ( ( u16AnADC <  PWM_TO_ANALOG_TURN_ON_BIT ) && ((BitTest((PWM_TO_ANALOG_LOW_HYST_ON), AnalogStatus))==(BOOL)FALSE)))
		{
      BitClear((PWM_TO_ANALOG_LOW_HYST_ON), AnalogStatus);
      BitClear((PWM_TO_ANALOG_HIGH_HYST_ON), AnalogStatus);
      u8Setpoint = PWM_TO_AN_SP_STOP_SPEED; 
		} 
		else if ( ( u16AnADC <  PWM_TO_ANALOG_TURN_ON_BIT ) || ( ( u16AnADC <  PWM_TO_ANALOG_TURN_MIN_SP_BIT ) && ((BitTest((PWM_TO_ANALOG_LOW_HYST_ON), AnalogStatus))!=(BOOL)FALSE)))
		{
      BitSet((PWM_TO_ANALOG_LOW_HYST_ON), AnalogStatus);
      BitClear((PWM_TO_ANALOG_HIGH_HYST_ON), AnalogStatus);
      u8Setpoint = (u8)PWM_TO_AN_SP_MIN_SPEED_VALUE;                                 
		}   
	  else if (( u16AnADC <  PWM_TO_ANALOG_MAX_TO_N_SP_BIT) || (( u16AnADC < PWM_TO_ANALOG_N_TO_MAX_SP_BIT)  && ((BitTest((PWM_TO_ANALOG_HIGH_HYST_ON), AnalogStatus))==(BOOL)FALSE)))
		{
		  u8 u8LimitedSlope;
		  
      BitSet((PWM_TO_ANALOG_LOW_HYST_ON), AnalogStatus);
      BitClear((PWM_TO_ANALOG_HIGH_HYST_ON), AnalogStatus);
        
      u8LimitedSlope = (u8)(((u16)(PWM_TO_AN_SP_N_TO_MAX_SPEED_VALUE-PWM_TO_AN_SP_MIN_SPEED_VALUE)*((u16)1<<PWM_TO_ANALOG_FDT_SLOPE_SHIFT))/(PWM_TO_ANALOG_N_TO_MAX_SP_BIT-PWM_TO_ANALOG_TURN_MIN_SP_BIT));      
      u8Setpoint = (u8)(PWM_TO_AN_SP_MIN_SPEED_VALUE + (u8)((u16)((u32)u8LimitedSlope*(u16)(u16AnADC-PWM_TO_ANALOG_TURN_MIN_SP_BIT))>>PWM_TO_ANALOG_FDT_SLOPE_SHIFT));
		}   
		else
		{
      BitSet((PWM_TO_ANALOG_LOW_HYST_ON), AnalogStatus);
      BitSet((PWM_TO_ANALOG_HIGH_HYST_ON), AnalogStatus);
      
      u8Setpoint =  (u8)((u16)((u16)(PWM_TO_AN_SP_N_TO_MAX_SPEED_VALUE*PWM_TO_AN_SP_MAX_SPEED_VALUE)/PWM_TO_AN_SP_N_TO_MAX_SPEED_VALUE));   
		}  		
		
		  return (u8Setpoint);
}

#endif  // RVS

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputAN_PwmToAnalogDisappeared(void)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    "link al requirement"
    \brief Return TRUE if pwm to analog converted input disappears for more than \ref PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER period.  
    \details This function return:.\n
    - TRUE if if pwm input disappears for more than PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER period.\n
    - else it return FALSE.\n
    Returned value are valid even in test mode.\n 
                            \par Used Variables
    \return \b bool TRUE if pwm input disappears for more than \ref MAX_NUM_NO_DUTY * 8ms.
    \note None.
    \warning None.
*/

static BOOL InputAN_PwmToAnalogDisappeared(void)
{
  u16 AnalogIn;
  BOOL ret =(BOOL)FALSE;
  
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    AnalogIn = InputAN_TempCompensation(buffu16ADC_READING[PWM_TO_AN_CONV]);
    
    if ( AnalogIn  < PWM_TO_ANALOG_TURN_OFF_BIT )
    {
      ret = (BOOL)TRUE;
    }
    else
    {
      ret = (BOOL)FALSE;
    }
  }

  return(ret);
}

#endif // PWM_TO_ANALOG_IN


/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputAN_PwmDisappeared(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return TRUE if pwm signal applied at analog input during test phase disappears.  
    \details This function return:\n
    - TRUE if if pwm signal at analog input disappears for more than \ref PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER period.\n
    - else it return FALSE.\n
                            \par Used Variables
    \return \b bool TRUE if pwm input disappears for more than \ref MAX_NUM_NO_DUTY * 8ms.
    \note None.
    \warning None.
*/

BOOL InputAN_PwmDisappeared(void) /*PRQA S 2889 #multiple return due to error management*/ /* PRQA S 1503 #rcma-1.5.0 warning message: this function could be used for another interface type */
{ 
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    #if (defined(POTENTIOMETER) || defined(NTC))
    
    return ( FALSE );
    
    #elif (defined(PWM_TO_ANALOG_IN))
     return (InputAN_PwmToAnalogDisappeared());  
    #else
      #error("Define analog input type");
    #endif /* ANALOG TYPE */ 
    
  }
  else
  {
    if ( InputSP_Get_u8NumNoDuty() >= ANALOG_IN_DISAPPEAR_TEST_OP_NUM )                                // It has to be strictly major (i.e. >) since because of pwm input frequency at testing phase, can happen PWM_IN_DISAPPEAR_TEST_OP_NUM=0!  
		{   
      return ( TRUE );
    }
    else
    {
      return ( FALSE );
    }
  }
}

#ifndef PWM_TO_ANALOG_IN
/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputAN_CheckTestPhase(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 30/05/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set \ref enumOperatingMode variable basing on PWM input and frequency applied to analog input.
    \details When called just set \ref enumOperatingMode basing on relate document:\n
    Production tests on SBL 500 and SBL 300 F fan modules -> TREQ046.
                                            \par Used Variables
    \ref u32TempPeriod Period of pwm input signal.\n
    \ref u32TempDuty Duty cycle of pwm input signal.\n
    \ref enumOperatingMode Tells if motor has to run in  production or normal behaviour.\n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void InputAN_CheckTestPhase(void) /* PRQA S 1503 #rcma-1.5.0 warning message: this function could be used for another interface type */
{   
  u16PWMCount = 0; // Reset Cont

  RTI_SetMainTimeBase(ANALOG_INPUT_TEST_TIME_BASE);
  while(RTI_IsMainTimeElapsed() == (BOOL)FALSE)
  {
    if (( BitTest((ANALOG_PWM_EDGE_ON_AN), AnalogStatus) ) != 0u)
    {                                                  
      InputSP_GetPWMDutyAndPeriod();                   
      InputSP_FindOperatingMode();                     
      if( !(InputSP_GetOperatingMode() == NORMAL_OPERATION) )   
      {
        u16PWMCount++;
      }
      else
      {
        u16PWMCount = 0;
      }
      BitClear((ANALOG_PWM_EDGE_ON_AN), AnalogStatus);
    }
    else
    {
      Nop();                                            // Misra rule. 60(adv).
    }
    WTD_ResetWatchdog(); 
  }
    
  if( u16PWMCount < ANALOG_VALID_NORMAL_OPERATION_COUNT )
  {
    InputSP_SetOperatingMode(NORMAL_OPERATION);
    INPUTAN_CLEAR_PENDING_INTERRUPT;                    // A flag CCF[n] is cleared when: If AFFC=0, write “1” to CCF[n].
    ANALOG_RESET_PWM_FEATURE;                           // No automatic compare on ATDDRn
  }
  else
  {
    Nop();                                              // Misra rule. 60(adv).
  }
  u16PWMCount = 0;                                      // Misra rule. 60(adv).
}
#endif




#if defined(PWM_TO_ANALOG_IN)
/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 InputAN_TempCompensation (u16 u16ADSample)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 18/01/2012
                            \par Starting SW requirement
    "link al requirement"
    \brief
    \details 
                                            \par Used Variables
    \ref \n
    \ref \n
    \ref \n
    \return \b
    \note None.
    \warning None.
*/
static u16 InputAN_TempCompensation (u16 u16ADSample)
{

  s16 s16ActualTamb;
  /*s16 s16TambDelta;   
  
  s16TambDelta = (s16)(Temp_GetTamb() - TAMB_TLE);
  s16TambDelta /= 2;                                    // Analog setpoint vs Temperature angular coefficient: 1 bit / 3 °C
   
  s16ActualTamb = ((s16)(u16ADSample)+s16TambDelta);
  
  if (s16ActualTamb < 0)       //Check overflow conditions
     s16ActualTamb = 0;
   if (s16ActualTamb > 1023)
     s16ActualTamb = 1023;
  */
  s16ActualTamb=(s16)((u32)u16ADSample);
  return (u16)(s16ActualTamb); 
}
#endif

#endif  //AN_INPUT

/* END */

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
