/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MCN1_RegPI.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MCN1_RegPI.c
    \version see \ref FMW_VERSION
    \brief Motion Control Routines
    \details In this files all the intermidiate motion control routines are regrouped.
    In line of principle all functions that are contained in this files have to drive Level 0 routines
    and will use and provides variables for routines in \ref MCN1_acmotor.c 
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include "main.h"           /* Per strDebug                                                          */
#include "MCN1_acmotor.h"   /* It has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"       /* Public motor control peripheral function prototypes                   */
#include "MCN1_RegPI.h"
#include MAC0_Register      /* Needed for macro definitions */
#include ADM0_ADC
#include "CUR1_Current.h"
#include "SOA1_SOA.h"
#include "VBA1_Vbatt.h"
#include "TIM1_RTI.h"       /* Per UPDATE_LOOP_TIME */

/*PRQA S 0380,0857 -- */

/* -------------------------- Public variables ---------------------------- */
#pragma DATA_SEG _PI_PAGE
structPIreg VectParPI[VECT_PI_LENGTH];      /* Array dei parametri dei vari regolatori PI       */
structRegIO VectVarPI[VECT_PI_LENGTH];      /* Array delle variabili I/O dei vari regolatori PI */

#pragma DATA_SEG DEFAULT


/* -------------------------- Public funcitons ---------------------------- */
void REG_SetParPI(PIenum enumReg,u16 u16IntGain,u16 u16PropGain,s32 s32RegLimLow,s32 s32RegLimHigh,BOOL BOOLFreeze,u16 u16IntStep)
{
  VectParPI[enumReg].u16IntGain = u16IntGain;
  VectParPI[enumReg].u16PropGain = u16PropGain;
  
  VectParPI[enumReg].s32IntLimitLow = s32RegLimLow;
  VectParPI[enumReg].s32RegLimitLow = s32RegLimLow;
  VectParPI[enumReg].s32IntLimitHigh = s32RegLimHigh;
  VectParPI[enumReg].s32RegLimitHigh = s32RegLimHigh;
  
  VectParPI[enumReg].BOOLFreeze = BOOLFreeze;
  VectParPI[enumReg].u16IntStep = u16IntStep;  
}


void REG_InitVarPI(PIenum enumReg,s16 s16RegInput,s16 s16RegFeedback,s32 s32Integral,s32 s32Proportional,s32 s32RegOutput)
{
  VectVarPI[enumReg].s16RegInput = s16RegInput;
  VectVarPI[enumReg].s16RegFeedback = s16RegFeedback;
  VectVarPI[enumReg].s32Integral = s32Integral;
  VectVarPI[enumReg].s32Proportional = s32Proportional;  
  VectVarPI[enumReg].s32RegOutput = s32RegOutput;
}


void REG_UpdateInputPI (PIenum enumReg,s16 s16RegSp,s16 s16RegSample)
{
	VectVarPI[enumReg].s16RegInput = s16RegSp; 
	VectVarPI[enumReg].s16RegFeedback = s16RegSample;
}


void REG_UpdateRegPI(PIenum enumReg)
{
  s32 s32ErrorValue,s32NextIntegration;

  /* Calcolo (nuovo) errore regolatore. */
  s32ErrorValue = (s32)((s32)VectVarPI[enumReg].s16RegInput-VectVarPI[enumReg].s16RegFeedback);

  /* Calcolo parte proporzionale */
  VectVarPI[enumReg].s32Proportional = (s32)((s32)s32ErrorValue*(s32)(VectParPI[enumReg].u16PropGain));

  /* Parte integrale eseguita se:                                                                                                                                        */
  /*  - il bit BOOLFreeze e' resettato (tale bit consente il congelamento dell'integrativa da parte di limitazioni e/o altri anelli esterni al regolatore                */
  /*    in atto) e l'uscita del regolatore all'istante (k-1) e' nei limiti (N.B.: la disuguaglianza su VectVarPI[enumReg].s32RegOutput deve essere NON                   */
  /*    stretta, poiche' cio' consente di resettare l'integrativa qualora si abbiano delle limitazioni che cambiano dinamicamente - i.e. regolatore di                   */
  /*    potenza durante l'uscita dal derating termico - e che passano da un valore non nullo a 0).                                                                       */
  /*  - l'errore ha cambiato segno e l'integrale deve poter evolvere, anche se l'uscita all'istante (k-1) e' gia' saturata.                                              */
  /*    Cio', ad esempio, consente all'integrativa di scaricarsi completamente, pur avendo l'output complessivo in limitazione (a causa del proporzionale, per esempio). */
  
  if (  (( (VectParPI[enumReg].BOOLFreeze == (BOOL)FALSE)) && ( ( VectVarPI[enumReg].s32RegOutput >= VectParPI[enumReg].s32RegLimitLow ) && ( VectVarPI[enumReg].s32RegOutput <= VectParPI[enumReg].s32RegLimitHigh ) ))
     || ( ( s32ErrorValue > 0 ) && ( VectVarPI[enumReg].s32Integral < 0 ) )
     || ( ( s32ErrorValue < 0 ) && ( VectVarPI[enumReg].s32Integral > 0 ) ) )
  {
    /* Aggiornamento parte integrale */
    s32NextIntegration = (s32)(VectVarPI[enumReg].s32Integral + (s32)((s32)s32ErrorValue*(s32)VectParPI[enumReg].u16IntGain*(s32)VectParPI[enumReg].u16IntStep));

    if ( s32NextIntegration > 0 )
    {
      /* Se l'aggiornamento e' al di sotto del limite superiore, si aggiorna l'integrativa. */
      if ( s32NextIntegration <= VectParPI[enumReg].s32IntLimitHigh )
      {
        VectVarPI[enumReg].s32Integral = s32NextIntegration;
      }
      else
      {
        /* Se l'aggiornamento e' al di sopra del limite superiore: */
        if ( VectParPI[enumReg].s32IntLimitHigh == 0 )
        {
          VectVarPI[enumReg].s32Integral = 0;     /* 1) se il limite superiore e' zero, si consente all'integrativa di scaricarsi completamente. */
        }
        else
        {
          /* Nop(); */                            /* 2) se il limite superiore e' un valore (strettamente) positivo, si congela l'intregrativa all'ultimo valore valido. */
        }
      }
    }
    else if ( s32NextIntegration < 0 )
    {
      /* Se l'aggiornamento e' al di sopra del limite inferiore, si aggiorna l'integrativa. */
      if ( s32NextIntegration >= VectParPI[enumReg].s32IntLimitLow )
      {
        VectVarPI[enumReg].s32Integral = s32NextIntegration;
      }
      else
      {
        /* Se l'aggiornamento e' al di sotto del limite inferiore: */
        if ( VectParPI[enumReg].s32IntLimitLow == 0 )
        {
          VectVarPI[enumReg].s32Integral = 0;     /* 1) se il limite inferiore e' zero, si consente all'integrativa di scaricarsi completamente. */
        }
        else
        {
          /* Nop(); */                            /* 2) se il limite inferiore e' un valore (strettamente) negativo, si congela l'intregrativa all'ultimo valore valido. */
        }
      }    
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

  /* Calcolo uscita regolatore PI. */
  VectVarPI[enumReg].s32RegOutput = (s32)(VectVarPI[enumReg].s32Integral + VectVarPI[enumReg].s32Proportional);

  /* Limitazione (i.e. saturazione) uscita regolatore PI. */
  if ( VectVarPI[enumReg].s32RegOutput > VectParPI[enumReg].s32RegLimitHigh )
  {
    VectVarPI[enumReg].s32RegOutput = VectParPI[enumReg].s32RegLimitHigh;
  }
  else if ( VectVarPI[enumReg].s32RegOutput < VectParPI[enumReg].s32RegLimitLow )
  {
    VectVarPI[enumReg].s32RegOutput = VectParPI[enumReg].s32RegLimitLow;
  }
  else
  {
    /* Nop(); */
  }
}

/*** (c) 2015 SPAL Automotive ****************** END OF FILE **************/
