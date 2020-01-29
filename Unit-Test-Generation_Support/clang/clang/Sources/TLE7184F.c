/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  TLE7184F.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file TLE7184F.c
    \version see \ref FMW_VERSION 
    \brief Basic TLE7184F functions.
    \details Routines for basic TLE7184F operation and variables related initialization.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include <hidef.h>          /* To disable and enable interrupts */
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register
#include ADM0_ADC           /* Per costante AD_MAXIMUM_RES_BIT contenuta in BASE_CURRENT_CONV_FACT_RES */
#include "TIM1_RTI.h"       /* Per RTI_CPSIsReadyToCheck()                                             */
#include "CUR1_Current.h"   /* Curr_GetCurrent()                                                       */
#include "TLE7184F.h"
#include "main.h"           /* Per strDebug */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* -------------------------- Private constants ---------------------------- */

#pragma DATA_SEG DEFAULT

static u8 u8GDErrorCount;

/* -------------------------- Private variables -----------------------------*/
/*-----------------------------------------------------------------------------*/
/** 
    \fn void TLE7184F_ResetTLEAndForceHiZ(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function resets TLE7184F and gets it in error mode .  
    \details This function performs:\n
    - Reset STAND_BY i.e. RGS TLE pin to 1.\n
    - Set HI_Z_BRIDGE as output high in order to get SCDL high and to get TLE7184F in error.\n
                            \par Used Variables
    \return \b void no value return
    \note None.
    \warning None
*/
void TLE7184F_ResetTLEAndForceHiZ(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8 u8ForIndex;
  
  ZCE_CNTRL_OFF;                                    /* N.B.                                                                    */
  ZCE_CNTRL_SET_OUT;                                /* IT'S MANDATORY TO SET HERE ZCE_CNTRL AS OUTPUT AND SET IT TO ZERO.      */
                                                    /* WITHOUT DOING THIS, HW CRASHES AT MTC_SetupCheckStartingSpeed() FUNTION */
                                                    /* WHEN DOING MTC_SetZCECntrl() INSTRUCTION!!!                             */

  STAND_BY_OFF;
  STAND_BY_SET_OUT;

  for(u8ForIndex=(u8)0;u8ForIndex<(u8)5;u8ForIndex++){}  /* Mandatory in order to let STAND_BY at zero for 10uS. */
  ;                                                      /* N.B. Master clock set!!! */

  STAND_BY_ON;                                      /* Reset STAND_BY i.e. RGS TLE pin to 1. */

  HI_Z_BRIDGE_REMOVE_PULL_RESISTOR;                 /* Remove Pull device on HI_Z_BRIDGE */
  HI_Z_BRIDGE_ON;                                   /* Set HI_Z_BRIDGE as output high */
  HI_Z_BRIDGE_SET_OUT;                              /* In order to get SCDL high and to get TLE7184F in error */

  GD_SET_ERROR_LINE_INPUT;                          /* Fault line is put as input. */
                                                    /* GD error line will be checked in polling at the and of main. */

  #if ( PCB == PCB213A )                            

  VDH_CTRL_OFF;
  SET_VDH_CTRL_OUT;                                 /* VDH_CTRL line set to ground. */

  I_GAIN_OFF;
  SET_I_GAIN_OUT;                                   /* I_GAIN line set to ground. */

  #elif ( ( PCB == PCB213B ) || ( PCB == PCB213C ) || ( PCB == PCB213D ) )

  I_GAIN_OFF;
  SET_I_GAIN_OUT;                                   /* I_GAIN line set to ground. */
  
  SCDL_HI_LEV_REMOVE_PULL_RESISTOR;
  SCDL_HI_LEV_SET_INPUT;

  #endif  /* PCB213A */

  DIAGNOSTIC_RESET_PORT;                            /* Turn off diangostic transistor by setting port as output low. That's because of boot loader leaves port opened. */
  RESET_BOOTLOADER_SETTINGS;                        /* Turn off interrupts and peripheral used by bootloader */
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void TLE_EnableOutputs(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function enables TLE7184F outputs.  
    \details This function performs:\n
    - Set HI_Z_BRIDGE as input hi z in order to remove TLE7184F SCDL error.
    Mandatory in order to let HI_Z_BRIDGE transition from 5V to 0.23V!!.\n
    - Reset STAND_BY i.e. RGS TLE pin to 1.\n
                            \par Used Variables
    \return \b void no value return
    \note None.
    \warning None
*/
void TLE_EnableOutputs(void)
{
  u8 u8ForIndex;

  HI_Z_BRIDGE_OFF;                                                        /* Set HI_Z_BRIDGE as input hi z */
  HI_Z_BRIDGE_RESET_OUT;                                                  /* In order to remove TLE7184F SCDL error */

  for(u8ForIndex=(u8)0;u8ForIndex<(u8)50;u8ForIndex++){}                  /* Mandatory in order to let HI_Z_BRIDGE transition from 5V to 0.23V!! */
  ;

  DisableInterrupts; /*PRQA S 1006 #Necessary assembly code*/             /* TO AVOID INTERRUPT WHEN RGS is low --> if RSG is low for t>20usec --> TLE goes into sleep mode */
                                                                          /* u8ForIndex<16 --> New RGS timing = 4,2usec because tmin=3usec */
  STAND_BY_OFF;                                                           /* Reset STAND_BY as input Hi Z */
  STAND_BY_SET_OUT;                                                       /* Set STAND_BY as output. */

  for(u8ForIndex=(u8)0;u8ForIndex<(u8)20;u8ForIndex++){}                  /* Mandatory in order to let STAND_BY at zero for 10uS. */
  ;                                                                       /* N.B. Master clock set!!! */
  STAND_BY_ON;                                                            /* Reset STAND_BY i.e. RGS TLE pin to 1. */
  EnableInterrupts; /*PRQA S 1006 #Necessary assembly code*/              /* TO AVOID INTERRUPT WHEN RGS is low --> if RSG is low for t>20usec --> TLE goes into sleep mode */

  u8GDErrorCount = 0;                                                     /* Reset error count to detect gate driver error. */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void TLE_CheckError(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 16/02/2011
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function manages TLE7184F error signal outputs.  
    \details This function check TLE7184F error line and if error is detected return TRUE
    as keyboar interrupt after a KBI interrupt i.e. TLE7184F fault:\n
    - Disable PWM Interrupt.\n
    - Disable tacho interrupt.\n
    - Set HI_Z_BRIDGE as output high in order to get SCDL high and to get TLE7184F in error.\n
    - Reset interrupt flag and disable KBI interrupt.\n
                            \par Used Variables
    \ref u16ErrorType
    \return \b void no value return
    \note None.
    \warning None
*/
BOOL TLE_CheckError(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16IpkPU;

  u16IpkPU = (u16)((u16)Curr_GetCurrent()*BASE_CURRENT_CONV_FACT_RES);

  if ( (GD_CHECK_ERROR_LINE==0u) && ( u16IpkPU < TLE_CURR_ERROR_DETECT_LEVEL_PU) )
  {
     if ( RTI_CPSIsReadyToCheck() ==(BOOL)TRUE )
     {
       u8GDErrorCount++;
     }
     else
     {
        /* Nop(); */
     }
  }
  else
  {
    u8GDErrorCount = 0;
  }

  if( u8GDErrorCount >= TLE_ERROR_COUNT )                               /* If TLE error line is off and consegutive convertion current are less than TLE_CURRENT_ERROR_DETECTION_LEVEL, */
  {                                                                     /* after TLE_ERROR_COUNT ticks -> GD error!                                                                     */
    /*INTERRUPT_PWM_DISABLE;*/                                          /* N.B.                                                                                                         */
    u8GDErrorCount = TLE_ERROR_COUNT;                                   /* Since Load dump is managed in sw way, PWM interrupt has to be keep always on.                                */
    
    INTERRUPT_TACHO_DISABLE;                                            /* Disable tacho interrupt */

    HI_Z_BRIDGE_ON;                                                     /* Set HI_Z_BRIDGE as output high                         */
    HI_Z_BRIDGE_SET_OUT;                                                /* In order to get SCDL high and to get TLE7184F in error */

    ret = (TRUE);
  }
  else
  {
    ret = (FALSE);
  }
  return ret;
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
