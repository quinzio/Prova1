/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  POWER1_MaxPower.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include "MCN1_RegPI.h"
#include "TMP1_Temperature.h"  /* Per BOOLSOATemperatureCorr */
#include "POWER1_MaxPower.h"
#include "PBATT1_Power.h"
#include "main.h"              /* Per strDebug */
#include "VBA1_Vbatt.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/** 
    \file POWER1_MaxPower.c
    \version see \ref FMW_VERSION
    \brief Motion Control Routines
    \details In this files all the intermidiate motion control routines are regrouped.
    In line of principle all functions that are contained in this files have to drive Level 0 routines
    and will use and provides variables for routines in \ref MCN1_acmotor.c 
*/

/* -------------------------- Public variables --------------------------- */
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  static BOOL BOOLPartStallDerFlag;
#endif  /* DIAGNOSTIC_STATE */

/* -------------------------- Private variables --------------------------- */
static u32 u32PowerRamp;
static u32 u32PowerRampFreeze; /*PRQA S 3218 #Gives better visibility here*/
/*s32 s32PowerLowLimit; */


/* -------------------------- Public functions --------------------------- */
/*
void MaxPower_PowerRegInit(void)
{
  u32PowerRamp = SP_POWER_PU;
  u32PowerRampFreeze = 0; 
}
*/

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
/*-----------------------------------------------------------------------------*/
/**
    \fn void MaxPower_Set_BOOLPartStallDerFlag(BOOL value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Set the value of the static variable BOOLPartStallDerFlag
                            \par Used Variables
    \ref BOOLPartStallDerFlag \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void MaxPower_Set_BOOLPartStallDerFlag(BOOL value)
{
   BOOLPartStallDerFlag = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn BOOL MaxPower_Get_BOOLPartStallDerFlag(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return the value of the static variable BOOLPartStallDerFlag
                            \par Used Variables
    \ref BOOLPartStallDerFlag \n
    \return \b The value of the static variable BOOLPartStallDerFlag.
    \note None.
    \warning None.
*/
BOOL MaxPower_Get_BOOLPartStallDerFlag(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (BOOLPartStallDerFlag);
}
#endif  /* DIAGNOSTIC_STATE */


void MaxPower_PowerSetPoint (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  if ( Temp_Get_BOOLSOATemperatureCorr() == (BOOL)FALSE )    /* Se il derating termico e' inattivo, il generatore di rampa gestisce solo la parte per il controllo a potenza massima (Pmax). */
  {
    /* Se il derating e' appena terminato (u32PowerRampFreeze != 0), allora: */
    if ( u32PowerRampFreeze != 0u )
    {
      u32PowerRampFreeze = 0;                                /* - si resetta il valore di potenza di inizio derating; */
    }
    else
    {
      /* Nop(); */
    }

    u32PowerRamp = SP_POWER_PU;

    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      MaxPower_Set_BOOLPartStallDerFlag(FALSE);
    #endif  /* DIAGNOSTIC_STATE */
  }
  else
  {
    s32 s32TempResult;
    
    if ( u32PowerRampFreeze == 0u )                                                                                                                                                   /* Se il derating termico e' attivo, allora: */
    {
      u32PowerRampFreeze = (u32)((u32)Power_Get_structPowerMeas_u32PbattPU()/VOLTAGE_RES_FOR_PU_BIT);                                                                                 /* - si memorizza la Pbatt da cui si inizia a deratare; */
      u32PowerRamp = u32PowerRampFreeze;                                                                                                                                              /* - si setta il generatore di rampa di potenza al valore appena memorizzato; */
      (void)REG_SetParPI(TEMP_REG,TEMP_REG_INT_GAIN_PU,TEMP_REG_PROP_GAIN_PU,(s32)(-((s32)u32PowerRampFreeze*(s32)TEMP_REG_COMPUTATION_RES_BIT)),0,VectParPI[TEMP_REG].BOOLFreeze,1); /* - si aggiornano i limiti del regolatore di temperatura (il limite inferiore e' esattamente il */
    }                                                                                                                                                                                 /*   valore di potenza da cui parte il derating). */
    else
    {
      /* Nop(); */
    }

    s32TempResult = (s32)((s32)((s32)((s32)Temp_GetTempCorr()/(s32)BASE_BATT_POWER_WATT)*(s32)CURRENT_RES_FOR_PU_BIT)/(s32)TEMP_REG_COMPUTATION_RES_BIT);

    if ( u32PowerRampFreeze >= (u32)(-s32TempResult) )
    {
      u32PowerRamp = (u32)(u32PowerRampFreeze + (u32)s32TempResult);   /* Il generatore di rampa parte da u32PowerRampFreeze ed evolve con la correzione proveniente dalla funzione di derating termico. */
    }
    else
    {
      u32PowerRamp = 0;                                                /* Plateau a zero se, a bassissime potenze, con derating attivo, la correzione del regolatore di temperatura e' addirittura */
    }

                                                                       /* superiore del valore di inizio rampa (u32PowerRampFreeze). */

    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      if ( u32PowerRamp < (u32)((u32)((u32)u32PowerRampFreeze*POWER_DERATING_ON_NUM)>>POWER_DERATING_SHIFT) )
      {
        MaxPower_Set_BOOLPartStallDerFlag(TRUE);
      }
      else if ( u32PowerRamp >= (u32)((u32)((u32)u32PowerRampFreeze*POWER_DERATING_OFF_NUM)>>POWER_DERATING_SHIFT) )

      {
        MaxPower_Set_BOOLPartStallDerFlag(FALSE);
      }
      else
      {
        /* Nop(); */
      }
    #endif  /* DIAGNOSTIC_STATE */
  }
}


u32 MaxPower_GetPowerSetPoint (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  return((u32)u32PowerRamp);
} 

/*** (c) 2015 SPAL Automotive ****************** END OF FILE **************/
