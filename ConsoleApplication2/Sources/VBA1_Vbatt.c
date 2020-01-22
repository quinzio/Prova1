/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  VBA1_Vbatt.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file VBA1_Vbatt.c
    \version see \ref FMW_VERSION 
    \brief Basic battery voltage functions.
    \details Routines for voltage operation and variables related initialization.\n
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
#include "main.h"             /* Per strDebug                                                     */
#include "TIM1_RTI.h"         /* Per RTI_CPSIsReadyToCheck()                                      */
#include "MCN1_acmotor.h"     /* Pima di MCN1_mtc.h, a causa della definizione di StartStatus_t!! */
#include "MCN1_mtc.h"
#include MAC0_Register        /* Per enumerativo V_BUS_CONV */
#include "VBA1_Vbatt.h"
#include ADM0_ADC             /* Per u8InverterStatus */
#include "DIA1_Diagnostic.h"  /* Per enumDiagVoltage  */
#include "AUTO1_AutoTest.h"
#include "RVP1_RVP.h"
#include "PBATT1_Power.h"     /* Per structPowerMeas.u16VbattPU   */
#include "PARAM1_Param.h"     /* Per indirizzi parametri taratura */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* ---------------------- Private Constants ----------------------------------*/
/** 
    \var V_PARAM
    \brief Tuning parameter for voltage measuring 
    \details This constant has to be written at tuning (test) time and used to compensate
    hw tolerances.
    \note none
    \warning none.
*/
static const volatile u16 V_PARAM V_PARAM_ADDRESS = V_PARAM_REF; /*PRQA S 1019 #mandatory for linker behavior*/ /*PRQA S 3218 #Gives better visibility here*/

#pragma DATA_SEG DEFAULT

static u16 u16ParamV;
static u8 u8OvervoltageCounter;

/* ----------------------------- Private Const ------------------------------ */

/* ---------------------------- Public Variables --------------------------- */

/* ---------------------------- Private Variables --------------------------- */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Vbatt_InitOverVoltage(void)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    "link al requirement"
    \brief Init overvoltage protection variables
                  
  
*/

void Vbatt_InitOverVoltage(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   u8OvervoltageCounter = VBATT_MAX_OVERVOLTAGE_EVENTS;      /* after reset, when Vbatt_CheckOverVoltage is called for the first time, return true if                             */
                                                             /* voltage is out of operative ranges. If u8OvervoltageCounter == 0 , Vbatt_CheckOverVoltage returns TRUE only after */
                                                             /* ..VBATT_MAX_OVERVOLTAGE_EVENTS consecutive events.                                                                */
}

void Vbatt_InitVbattOn (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  V_BATT_ON_ENABLE;
  SET_V_BATT_ON_OUT;
}

void Vbatt_EnableVbattOn (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  V_BATT_ON_ENABLE;
}

void Vbatt_DisableVbattOn (void)
{
  V_BATT_ON_DISABLE;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Vbatt_CheckOverVoltage(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns TRUE when bus voltage reading is out of range.  
    \details Return TRUE if:
		-the voltage of the HVBUS connected to right ADC channel has reached 
		 the high threshold level (\ref VBUS_HIGH_THRESHOLD_BIT) or the voltage has
		 previously reached this value and still over \ref VBUS_HIGH_THRESHOLD_BIT - \ref VBUS_HYSTERESIS.
		-the voltage of the HVBUS connected to right ADC channel is below 
		 the low threshold level (\ref VBUS_LOW_THRESHOLD_BIT) or the voltage has
		 previously gone under this value and still under \ref VBUS_LOW_THRESHOLD_BIT + \ref VBUS_HYSTERESIS.
                            \par Used Variables
    \return \b bool Returns TRUE when Bus voltage reading is out of the ranges.
    \note None.
    \warning None
*/
BOOL Vbatt_CheckOverVoltage(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    BOOL ret;
    static BOOL BoolVbattOutLimit;       /* static per renderlo piu' rapido (salvato in Ram) */
    u16 u16VbattAvgPU;

    (void)Vbatt_GetBusVoltage();         /* Per aggiornare structPowerMeas.u16VbattPU */

    #ifdef VBATT_FILTERING
      u16VbattAvgPU = (u16)((u16)((u16)u16VbattAvg * BASE_VOLTAGE_CONV_FACT_RES) + (u16)V_DROP_VDHS_PU + (u16)V_GAMMA_VDH_RVP_DIODE_PU);

      if ( (BOOL)RVP_IsRVPOn() == (BOOL)FALSE )
      {
        u16VbattAvgPU += VBUS_HYSTERESIS_RVP_OFF_PU;   /* Quando l'RVP e' spento, la relativa caduta va compensata sulla Vmon!! */
      }
      else
      {
        /* Nop(); */
      }

    #else
      u16VbattAvgPU = Power_Get_structPowerMeas_u16VbattPU();
    #endif

    if ( ((u8InverterStatus & UNDER_VOLTAGE) != 0u) || ((u8InverterStatus & OVER_VOLTAGE) != 0u) )
    {
        if (
               ( u16VbattAvgPU <= (u16)((u16)MIN_OPERATING_VOLTAGE_PU + VBUS_HYSTERESIS_LOW_PU) )
            || ( u16VbattAvgPU >= (u16)((u16)MAX_OPERATING_VOLTAGE_PU - VBUS_HYSTERESIS_HIGH_PU) )
        )
        {
          BoolVbattOutLimit = TRUE;

          #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
          if ( u16VbattAvgPU <= (u16)((u16)MIN_OPERATING_VOLTAGE_PU + VBUS_HYSTERESIS_LOW_PU) )
          {
            DIAG_Set_enumDiagVoltage(DIAG_UNDER_VOLTAGE);
          }
          else
          {
            DIAG_Set_enumDiagVoltage(DIAG_OVER_VOLTAGE);
          }
          #endif  /* DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE */
        }
        else
        {
          BoolVbattOutLimit = FALSE;
          #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
            DIAG_Set_enumDiagVoltage(DIAG_VOLTAGE_NO_ERR);
          #endif  /* DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE */
        }
    }
    else
    {
      if ( ( u16VbattAvgPU <= MIN_OPERATING_VOLTAGE_PU )
        || ( u16VbattAvgPU >= MAX_OPERATING_VOLTAGE_PU ) )
      {
        BoolVbattOutLimit = TRUE;

        #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
          if ( u16VbattAvgPU <= MIN_OPERATING_VOLTAGE_PU )
          {
            DIAG_Set_enumDiagVoltage(DIAG_UNDER_VOLTAGE);
          }
          else
          {
            DIAG_Set_enumDiagVoltage(DIAG_OVER_VOLTAGE);
          }
        #endif  /* DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE */
      }
      else
      {
        BoolVbattOutLimit = FALSE;
        #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
          DIAG_Set_enumDiagVoltage(DIAG_VOLTAGE_NO_ERR);
        #endif  /* DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE */
      }  
    }

    /* Count the number of overvoltage events */
    if ( RTI_CPSIsReadyToCheck() == (BOOL)TRUE )
    {
      if (BoolVbattOutLimit == (BOOL)TRUE ) 
      {
        u8OvervoltageCounter++;
      }
      else
      {
        u8OvervoltageCounter = 0;
      }
    }
    else
    {
      /* Nop(); */
    }
    
    /* ASIMMETRICO                                                                 */
    /* Basta 1 solo campione entro i limiti di tensione per far partire il motore. */
    /* Occorrono VBATT_MAX_OVERVOLTAGE_EVENTS campioni per fermare il motore       */

    if (u8OvervoltageCounter >= VBATT_MAX_OVERVOLTAGE_EVENTS)
    {
       u8OvervoltageCounter = VBATT_MAX_OVERVOLTAGE_EVENTS;
       u8InverterStatus |= OVER_VOLTAGE;
       u8InverterStatus |= UNDER_VOLTAGE;
    }
    else
    {
      u8InverterStatus &= (u8)(~OVER_VOLTAGE);
      u8InverterStatus &= (u8)(~UNDER_VOLTAGE);
    }

    if((u8InverterStatus & OVER_VOLTAGE) != 0u)
    {
      ret = TRUE;
    }
    else
    {
      ret = FALSE;
    }
    return (ret);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Vbatt_AutoTestVoltage(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns TRUE when Bus voltage reading is corrupted.  
    \details This function is performed when the micro wakes up after the boot phase. His main aim is to check the Vbus pin
    and detects, if possible, if the hw device outside the micro is corrupted.
    This function performs:\n
    - 10 bit bus voltage channel convertion.\n
    - Filling in \ref buffu16ADC_READING with voltage value just read.\n
    - Returns TRUE if voltage < AUTOTEST_LVBUS_THRESHOLD\n
    
    This is a fault condition because the hardware device that perform the peak current measure, has been projected to ensure a minimum voltage offset
    when the current is equal to zero and the bridge is turned off. If the AD converter return a very low value, less than \ref AUTOTEST_LVBUS_THRESHOLD,
    the sensing hardware chain is corrupted or broken and it's impossible to perform a safe motor drive with this hardware failure.
                            \par Used Variables
    \return \b bool Returns TRUE when Bus voltage reading is out of the ranges.
    \note None.
    \warning None
*/
BOOL Vbatt_AutoTestVoltage(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16LocalBusVoltage;

  /*buffu16ADC_READING[V_BUS_CONV] = u16LocalBusVoltage;*/

  u16LocalBusVoltage = buffu16ADC_READING[V_BUS_CONV];             /* PWM peripherals turn on but TLE still in error so NO pwm on MOS appears!                                                           */
                                                                   /* Disassemble:                                                                                                                       */
                                                                   /* LDD   buffu16ADC_READING:2                                                                                                         */
                                                                   /* ...                                                                                                                                */
                                                                   /* This instruction is good since u16PeakCurr reading in done in single time so if interrupt occours during this reading -> no error! */
                                                                   /* Even if a writing interrupt occours between the two instruction: no problem.                                                       */
  u16LocalBusVoltage >>=2;

  if (u16LocalBusVoltage <= AUTOTEST_LVBUS_THRESHOLD)
  {
    u8InverterStatus |= UNDER_VOLTAGE;
    ret = (TRUE);
  }
  else
  {
    ret = (FALSE);	/* Normal operation */
  }
  return ret;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Vbatt_GetBusVoltage(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns Bus voltage read.  
    \details This function returns Bus voltage read from ADC.\n
    It takes into account possible tacho happening at function execution time so 
    first assegnation to local variable is done: 
    u16BuffVolt = (u16)\ref buffu16ADC_READING[\ref V_BUS_CONV];   
                            \par Used Variables
    \return \b u8 Bus voltage read from ADC.
    \note None.
    \warning None
*/
u16 Vbatt_GetBusVoltage(void)
{
  u16 structPowerMeas_u16VbattPU_tmp;

  u16 u16BuffVolt;
  u16BuffVolt = (u16)buffu16ADC_READING[V_BUS_CONV];                                /* Disassemble:                                                                 */
                                                                                    /* LDHX  buffu16ADC_READING:2                                                   */
                                                                                    /* PSHH                                                                         */
                                                                                    /* PULA...                                                                      */
                                                                                    /* Even if a writing interrupt occours between the two instruction: no problem. */

  u16BuffVolt = (u16)((u32)((u32)u16BuffVolt*u16ParamV)>>OFFSET_SHIFT);

  structPowerMeas_u16VbattPU_tmp = (u16)((u16)((u16)u16BuffVolt * BASE_VOLTAGE_CONV_FACT_RES) + (u16)V_DROP_VDHS_PU + (u16)V_GAMMA_VDH_RVP_DIODE_PU);
  Power_Set_structPowerMeas_u16VbattPU(structPowerMeas_u16VbattPU_tmp);

  #ifdef RVP_SW_MANAGEMENT_ENABLE
  if ( (BOOL)RVP_IsRVPOn() == (BOOL)FALSE )
  {
    structPowerMeas_u16VbattPU_tmp = Power_Get_structPowerMeas_u16VbattPU() + VBUS_HYSTERESIS_RVP_OFF_PU;   /* Quando l'RVP e' spento, la relativa caduta va compensata sulla Vmon */
    Power_Set_structPowerMeas_u16VbattPU(structPowerMeas_u16VbattPU_tmp); 
  }
  else
  {
    /* Nop(); */
  }
  #endif  /* RVP_SW_MANAGEMENT_ENABLE */

  return((u16)u16BuffVolt);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Vbatt_GetVparam(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns voltage parameter tuning written into flash.  
    \details This parameter store the result convertion A/D register value for voltage.\n
    It has been computed as following:\n
    \f[
        V_{param} = [ \frac{V_{mis}-V_{th}}{(V_{MCU}+I_{leak}R_{veq})*div} * 2^15
    \f]
    where:\n
    Vparam = stored parameter at \ref V_PARAM location.\n
    Vmcu = voltage measured by A/D.\n
    Vmis = voltage measured by external volmeter.\n
    Vth = Diode voltage drop (0,55V).\n
    Ileak = Micro pin leakage current (1ua from datasheet).\n
    Rveq = Equivalent resistor seen from micro pin (7kohm).\n
    Div = Partition ratio (7,226).\n
                            \par Used Variables
    \return \b u16 V_PARAM voltage compensating value.\n
    \note None.
    \warning None
*/
u16 Vbatt_GetVparam(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  u16ParamV = V_PARAM;
  return ( (u16)u16ParamV);
}

void Vbatt_SetDefaultParamV(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16ParamV = V_PARAM_REF;
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
