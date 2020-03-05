/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  FIR1_FIR.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file FIR1_FIR.c
    \version see \ref FMW_VERSION 
    \brief Basic filtering function.
    \details Routines for filtering operation and variables related managing.\n
    \note none
    \warning none
*/

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include SPAL_DEF
#include "main.h"           /* Per strDebug */
#include "FIR1_FIR.h"
 
 
/* ---------------------------- Public Variables --------------------------- */
#pragma DATA_SEG _FILT_PAGE
static structParFilt VectParFilt[VECT_FILT_LENGHT];
structVarFilt VectVarFilt[VECT_FILT_LENGHT];


#pragma DATA_SEG DEFAULT
/* ---------------------------- Public Functions --------------------------- */
void FIR_InitVarFilt (FiltEnum enumFilt,s16 s16OldInput,s16 s16NewOutput)
{
  VectVarFilt[enumFilt].s16OldInput  = s16OldInput;
  VectVarFilt[enumFilt].s16NewOutput = s16NewOutput;
}


void FIR_SetParFilt (FiltEnum enumFilt,u16 u16SampleFreq,u16 u16PoleFreq,u16 u16ZeroFreq,s16 s16Hyst,u8 u8FiltType)
{
  VectParFilt[enumFilt].u16SampleFreq = u16SampleFreq;
  VectParFilt[enumFilt].u16PoleFreq  = u16PoleFreq;
  VectParFilt[enumFilt].u16ZeroFreq = u16ZeroFreq;
  VectParFilt[enumFilt].s16Hyst = s16Hyst; 
  VectParFilt[enumFilt].u8FiltType = u8FiltType; 
}


void FIR_UpdateFilt (FiltEnum enumFilt,s16 s16NewInput)
{  
  s16 s16FilterOutOld;

  s16FilterOutOld = VectVarFilt[enumFilt].s16NewOutput;
  

  /* Aggiornamento filtro. */
  /* N.B.: nel caso del TOD non occorre dividere per FILTER_RES_FACT il risultato finale, poiche' tale fattore compare sia al */
  /* numeratore che al denominatore dell'equazione di aggiornamento e s16NewOutput non va riscalato!! */
  VectVarFilt[enumFilt].s16NewOutput = (s16)((s32)((s32)((s32)((s32)s16FilterOutOld - ((s32)VectVarFilt[enumFilt].s16OldInput*(s32)VectParFilt[enumFilt].u8FiltType))*(s32)VectParFilt[enumFilt].u16SampleFreq) + (s32)((s32)s16NewInput*(s32)((s32)((s32)VectParFilt[enumFilt].u16SampleFreq*(s32)VectParFilt[enumFilt].u8FiltType) + (s32)VectParFilt[enumFilt].u16ZeroFreq)))/(s32)((u32)((u32)VectParFilt[enumFilt].u16SampleFreq+(u32)VectParFilt[enumFilt].u16PoleFreq)));
   
  /* Limitazione uscita filtro  */
  if ( ( (s16)VectVarFilt[enumFilt].s16NewOutput < (s16)((s16)s16FilterOutOld - VectParFilt[enumFilt].s16Hyst) ) || ( (s32)VectVarFilt[enumFilt].s16NewOutput > (s16)((s16)s16FilterOutOld + VectParFilt[enumFilt].s16Hyst) ) )
  {
    VectVarFilt[enumFilt].s16NewOutput = s16FilterOutOld;
  }    
  else
  {
    /* Nop(); */
  }
      
  /* Memorizzazione ingresso attuale. */
  VectVarFilt[enumFilt].s16OldInput = s16NewInput;   	   	  
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/

