/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SOA1_SOA.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file SOA1_SOA.c
    \version see \ref FMW_VERSION 
    \brief Basic SOA functions.
    \details Routines for SOA operation and variables related initialization.\n
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
#include "MCN1_acmotor.h"            /* Per strMachine.u16CurrentPU */
#include "EMR0_Emergency.h"          /* Per EMR_EmergencyDisablePowerStage() */
#include "TIM1_RTI.h"                /* Per RTI_CPSIsReadyToCheck() */
#include "CUR1_Current.h"            /* Curr_GetPeakCurrent() */
#include "SOA1_SOA.h"
#include "main.h"                    /* Per strDebug */
#include "LOG0_LogDataS12.h"         /* Per u16LogPeakCurrentPU (#include da mettere dopo main.h, per tener conto di SystStatus_t) */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* ------------------------- Private variables ---------------------------- */
/** 
    \var u8StuckOvercoming
    \brief Variable used for current start number.
    \note This variable is placed in \b DEFAULT segment.
    \warning None.
    \details None.
*/
static u8 u8StuckOvercoming;


/*-----------------------------------------------------------------------------*/
/** 
    \fn void SOA_InitSOAMachine(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function inits SOA variables and state machine.  
    \details This function performs:\n
    - Diagnostic internal variables resetting.\n
    - Diagnostic timing resetting.\n
                            \par Used Variables
    \return \b void no value return
    \note None.
    \warning None
*/
void SOA_InitSOAMachine(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8StuckOvercoming = 0;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL SOA_CheckSOA(u16 u16Speed)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function performs SOA state machine.  
    \details This function performs:\n
    - Motor stops if SOA is reached before 15sec from start has been elapsed.\n
    - Motor power reducing for at most 60sec if 15 sec has been passed from start
    and motor reaches SOA.\n
    - Motor stops previous time of 60sec has elapsed.\n
        \dot digraph ManageTacho {
        node[shape=box, style=filled, fontname=Arial, fontsize=10]; 
        edge[fontsize=8];
        Start[label="Start", shape=ellipse];
        rF[label="return FALSE", shape=doubleoctagon,fillcolor=green];
        rT[label="return TRUE", shape=doubleoctagon,fillcolor=red];
        DPS[label="Disable\n Power\n Stage", shape=doubleoctagon,fillcolor=orange];
        End[label="End", shape=ellipse];
        subgraph cluster1{
        Time[label="SOA Time is over?", fillcolor=yellow];
        Tyes[label="{Reset SOA Error |<a> SOAState=SOA_RUN|Set New Time}", shape=record];
        Tno[label="Over Ilim?", fillcolor=yellow];
        Time->Tyes[label="YES", color=green];
        Time->Tno[label="NO", color=red];
        label="SOA_RAMPUP";fontsize=10; }
        subgraph cluster2{
        Test[label="Over Ilim?", fillcolor=yellow];
        Ty[label="Correction=0?", fillcolor=yellow];
        Add[label="{Corr\<U8\_MAX| {YES\n Set Flag \n Corr++ | NO \n nop}}";shape=record];
        New[label="{Init Corr| Set Flags|Set Timeout}";shape=record];
        Ser[label="{Corr\>0?|{YES\n Corr--|NO\n Reset SOA}}", shape=record];
        Tout[label="Time is over?", fillcolor=yellow];
        New->Tout[];Add->Tout[];Ser->Tout[];
        Test->Ty->New[label="YES", color=green];
        Ty->Add[label="NO", color=red];
        Test->Ser[label="NO", color=red];
        label="SOA_RUN";fontsize=10; }
        Start->End[]; rF->End[style=dotted];
        rT->End[style=dotted]; Start->Time[label="SOAState=SOA_RAMPUP"];
        Start->Test[label="SOAState=SOA_RUN"]; Tno->DPS[label="YES", color=green];
        Tout->DPS[label="YES", color=green]; Tno->rF[label="NO", color=red];
        Tout->rF[label="NO", color=red]; Tyes->rF[]; DPS->rT[]; } \enddot
    
    \param[in] u16Speed Motor electrical frequency in Hz*128.\n
                            \par Used Variables
    \return \b bool True if SOA has reached and motor has to stop.
    \note None.
    \warning None
*/
BOOL SOA_CheckSOA(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16SOALimit;
  
  (void)Curr_GetPeakCurrent();  /* Per aggiornamento strMachine.u16CurrentPU. */
  
  if ( strMachine.u16FrequencyPU < EL_FREQ_MIN_PU_RES_BIT )
  {
    u16SOALimit = SOA_LIMIT_CURRENT_PU_LOW;
  }
  else if ( strMachine.u16FrequencyPU >= EL_FREQ_NOMINAL_PU_RES_BIT )
  {
    u16SOALimit = SOA_LIMIT_CURRENT_PU_HIGH;
  }
  else
  {
    u16SOALimit = (u16)((u16)SOA_LIMIT_CURRENT_PU_LOW + (u16)((u32)((u32)((u32)strMachine.u16FrequencyPU-EL_FREQ_MIN_PU_RES_BIT)*SOA_LIMIT_CURRENT_PU_COEFF)>>SOA_LIMIT_SHIFT));
  }
  	
  if ( strMachine.u16CurrentPU > u16SOALimit )
  {
    if ( RTI_CPSIsReadyToCheck() == (BOOL)TRUE )
    {
      u8StuckOvercoming++;
    }
    else
    {
      /* Nop(); */
    }
    
    if ( u8StuckOvercoming >= SOA_STUCK_OVERCOMING_MAX )
    {
    
      #ifdef LOG_DATI
        u16LogPeakCurrentPU = strMachine.u16CurrentPU;
      #endif  /* LOG_DATI */
    
      EMR_EmergencyDisablePowerStage();     
		  ret = (TRUE);
		}
		else
		{
		  ret = (FALSE);
		}
  }
  else
  {
    u8StuckOvercoming = 0;
    ret = (FALSE);
  }					
  return ret;
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
