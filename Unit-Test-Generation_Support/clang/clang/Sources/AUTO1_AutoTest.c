/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  AUTO1_AutoTest.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file AUTO1_AutoTest.c
    \version see \ref FMW_VERSION 
    \brief Initial drive test functions.
    \details Test functions for intial readings of bus current, bus voltage and TLE temperature\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register      /* Needed for Watchdog */
#include "CUR1_Current.h"
#include "VBA1_Vbatt.h" 
#include "TMP1_Temperature.h"
#include "SOA1_SOA.h"       /* Needed for FAULT_FLAGs */
#include "WTD1_Watchdog.h"
#include "INP1_InputAN.h"
#include "main.h"           /* Per strDebug e u16ErrorType */
#include "AUTO1_AutoTest.h"
#include "PARAM1_Param.h"   /* Per indirizzi parametri taratura e relative tolleranze */

/*PRQA S 0380,0857 -- */

/* ---------------------------- Private Constants  ------------------------ */

/* ---------------------------- Public Variables   ------------------------ */

/* ---------------------------- Private Variables  ------------------------ */

/* ---------------------------- Public prototipes  -------------------------*/




#if ((BOOTLOAD_VERS == BOOTLOADER_LATER_15) || (BOOTLOAD_VERS == BOOTLOADER_LATER_20)) 

/* BOOTLOADER_LOCK_VALUE  */
/* In LOCK_BOOTL_ADDRESS is stored a Byte that define if Bootloader feature (reprogrammability) */
/* ..is enabled or disabled. */


#pragma CODE_SEG DEFAULT

static const volatile u8 BOOTL_LOCK_STATUS LOCK_BOOTL_ADDRESS = BOOTLOADER_LOCK_VALUE; /*PRQA S 1019 #mandatory for linker behavior*/

#endif /*BOOTLOAD_VERS*/
  
/* ---------------------------- Private prototipes -------------------------*/  

static void Auto_CheckParamRange( void );
static void Auto_SetDefaultParam(void);
static BOOL Auto_AutoTestParam ( void );
static BOOL Auto_ChecksumOk (void);

/**
    \fn void Auto_InitProtections( void )

    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 08/02/2012
                            \par Starting SW requirement
    "link al requirement"
    \brief Init protections 
    \details This function has the purpose to init overvoltage protection variables.\n
                            \par Used Variables
    ref var_name description \n
    \return void
    \note none
    \warning none
*/
void Auto_InitProtections(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   Vbatt_InitOverVoltage();
}


/** 
    \fn BOOL Auto_AutoTestChecks( void )

    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 08/02/2012
                            \par Starting SW requirement
    "link al requirement"
    \brief Check Hardware before running 
    \details This function has the purpose to check all the hardware before running the motor.\n
    This is done checking all the designed I/O pins and the AD channels value; if read value are in error ranges,
    the function returns true and stops the motion initialization.\n
    \dot 
     digraph Auto_AutoTestChecks {
         node [shape=box, style=filled, fontname=Arial, fontsize=10];  
     compound=true;
     subgraph cluster0{
      	TEST1[label="VbattVoltage\n HW Check FAIL?"];
    		TEST2[label="TLE Tamb\n HW Check FAIL?"];
    		TEST3[label="Ibus\n HW Check FAIL?"];
     }
     FALSE[label="return \n FALSE", shape=Msquare];
     TRUE[label="return \n TRUE", shape=Msquare];

     TEST1->TEST2->TEST3->FALSE[label="NO", fontsize=8];
     TEST1->TRUE[label="YES", fontsize=8];
     TEST2->TRUE[label="YES", fontsize=8];
     TEST3->TRUE[label="YES", fontsize=8];
    }     
    \enddot
                            \par Used Variables
    ref var_name description \n
    \return \b BOOL Boolean Value: TRUE if some Error is occurred FALSE if everything is OK
    \note This funtion is execute only once before the for() never ending loop and once in the Wait Case.
    \warning none
*/

BOOL Auto_AutoTestChecks(void) /*PRQA S 2889 #multiple return due to error management*/ /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  WTD_ResetWatchdog();
  (void)Auto_AutoTestParam();
  Main_SetBit_u16ErrorType(FAULT_ERROR_FLAG);

  #ifndef BMW_LIN_ENCODING                          /*Inibito per prodotto BMW visto che la condizione di voltage monitor =0V */
    WTD_ResetWatchdog();                            /*.. e' plausibile quando il microcontrollore e' acceso */
    if (  Vbatt_AutoTestVoltage() == (BOOL)TRUE )
    {
      return (TRUE);
    }
    else
    {
      /* Nop(); */
    }
  #endif  /* BMW_LIN_ENCODING */

  WTD_ResetWatchdog();
  if ( Temp_AutoTestTLETempAmb() == (BOOL)TRUE )
  {
    return (TRUE);
  }
  else
  {
    /* Nop(); */
  }
 
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
    WTD_ResetWatchdog();
    if ( Temp_AutoTestMosTemp() == (BOOL)TRUE )
    {
      return (TRUE);
    }
    else
    {
      /* Nop(); */
    }
  #endif /* MOSFET_OVERTEMPERATURE_PROTECTION */

  WTD_ResetWatchdog();
  if ( Curr_AutoTestIBus() == (BOOL)TRUE )
  {
    return (TRUE);
  }
  else
  {
    /* Nop(); */
  }

  WTD_ResetWatchdog();
  if ( Curr_AutoTestIBatt() == (BOOL)TRUE )
  {
    return (TRUE);
  }
  else
  {
    /* Nop(); */
  }

  WTD_ResetWatchdog();
  Main_ClrBit_u16ErrorType(FAULT_ERROR_FLAG);
  return (FALSE);
}


/** 
    \fn static void Auto_CheckParamRange( void )

    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 08/02/2012
                            \par Starting SW requirement
    "link al requirement"
    \brief Plausibility check on foundamental parameters.
    \details This function verifies ROM parameters (I_PARAM, V_PARAM, T_PARAM, T_MOS_PARAM_REF) to
    be within ranges defined by related tolerances.
    If the value is outside of its typical tolerances, a specific function loads default value.

    \par Used Variables
    \ref u16TestParam   Parameter to check \n
    \return void
    \note none
    \warning none
*/

static void Auto_CheckParamRange( void )
{
  u16 u16TestParam;

  u16TestParam = Curr_GetIparam();
  if ( ( u16TestParam < ( I_PARAM_REF - I_PARAM_TOL ) ) || ( u16TestParam > ( I_PARAM_REF + I_PARAM_TOL ) ) )
  {
    Curr_SetDefaultParamI();
  }

  u16TestParam = Vbatt_GetVparam();
  if ( ( u16TestParam < ( V_PARAM_REF - V_PARAM_TOL ) ) || ( u16TestParam > ( V_PARAM_REF + V_PARAM_TOL ) ) )
  {
    Vbatt_SetDefaultParamV();
  }

  u16TestParam = Temp_GetTparam();
  if ( ( u16TestParam < ( T_PARAM_REF - T_PARAM_TOL ) ) || ( u16TestParam > ( T_PARAM_REF + T_PARAM_TOL ) ) )
  {
    Temp_SetDefaultParamT();
  }

  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION 
  u16TestParam = Temp_GetTMosParam();
  if ( ( u16TestParam < ( T_MOS_PARAM_REF - T_MOS_PARAM_TOL ) ) || ( u16TestParam > ( T_MOS_PARAM_REF + T_MOS_PARAM_TOL ) ) )
  {
    Temp_SetDefaultParamTMOS();
  }
  #endif

  #ifdef AN_INPUT
  u16TestParam = InputAN_GetANparam();
  if ( ( u16TestParam < ( AN_PARAM_REF - AN_PARAM_TOL ) ) || ( u16TestParam > ( AN_PARAM_REF + AN_PARAM_TOL ) ) )
  {
    InputAN_SetDefaultANparam();
  }
  #endif

  #ifdef PWM_TO_ANALOG_IN
  u16TestParam = InputAN_GetPWMToANparam();
  if ( ( u16TestParam < ( PWM_TO_AN_PARAM_REF - PWM_TO_AN_PARAM_TOL ) ) || ( u16TestParam > (PWM_TO_AN_PARAM_REF + PWM_TO_AN_PARAM_TOL) ) )
  {
    InputAN_SetDefaultPWMToANparam();
  }
  #endif
}


/** 
    \fn static void Auto_SetDefaultParam(void)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 08/02/2013
    \par Starting SW requirement
    "link al requirement"
    \brief Load default parameters
    \details When this function is called, it sets all calibration parameters 
    to its default values

    \return void
    \note none
    \warning none
*/

static void Auto_SetDefaultParam(void)
{
  Curr_SetDefaultParamI();

  Vbatt_SetDefaultParamV();

  Temp_SetDefaultParamT();

  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
    Temp_SetDefaultParamTMOS();
  #endif /* MOSFET_OVERTEMPERATURE_PROTECTION */

  #ifdef AN_INPUT
    InputAN_SetDefaultANparam();
  #endif
}


/** 
    \fn static BOOL Auto_ChecksumOk(void)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 08/02/2013
    \par Starting SW requirement
    "link al requirement"
    \brief Perform xor checksum on a specific range of memory.
    \details Perform xor checksum on a specific range of memory defined by 
     PARAM_INIT and PARAM_END addresses.
     If the checksum is correct return TRUE
     If the checksum is wrong return FALSE
     
    \return TRUE Parameters OK FALSE Parameters damaged
    \note none
    \warning into PARAM_END location must be stored with the results of checksum of
      previous  bytes.
*/

#define CHECKSUM_LENGHT    ((u8)(PARAM_END - PARAM_INIT))

/*PRQA S 0306,0488 ++ #mandatory memory management*/
static BOOL Auto_ChecksumOk(void)
{
  u8 ChecksumResult;
  u8 index;
  u8 *ParamAddr ;
  BOOL ret;
  ParamAddr = PARAM_INIT;
  /*WTD_ResetWatchdog(); */
  ChecksumResult=0u;
  for (index=(u8)0; index <= CHECKSUM_LENGHT;index++)
  {
    ChecksumResult=(u8)(ChecksumResult^(u8)(*ParamAddr));
    ParamAddr++;
  }
  if(ChecksumResult == 0u)
  {
    ret = TRUE;
  }
  else
  {
    ret = FALSE;
  }
  return (ret);
}
/*PRQA S 0306,0488 --*/


/** 
    \fn static BOOL Auto_AutoTestParam( void )
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio   \n <em>Reviewer</em>

    \date 08/02/2013
    \par Starting SW requirement
    "link al requirement"
    \brief Load parameter calibration and check coherency with expected values
    \details If the checksum of the parameters is ok, this function check if every 
    parameter is into the expected range
    If the checksum of the parameters fails, default parameters will be used.
    \return always FALSE to perform MAXIMUM AVAILABILITY REQUIREMENT 
    (this function is called by Autotest -> it shall not return TRUE otherwise 
      the state machine enter in FAULT)
    \note none
    \warning none
*/

static BOOL Auto_AutoTestParam( void )
{
  if(Auto_ChecksumOk() == (BOOL)TRUE)
  {
    Auto_CheckParamRange();
  }
  else
  {
    Auto_SetDefaultParam();
  }

  return(FALSE);
}

/*** (c) 2012 SPAL Automotive ****************** END OF FILE **************/
