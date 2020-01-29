/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputECO.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INP1_InputECO.c
    \version see \ref FMW_VERSION 
    \brief Basic Economy input function.
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
#include "INP1_InputAN.h"
#include "TMP1_Temperature.h"
#include "INP1_InputECO.h"  
#include "WTD1_Watchdog.h"




#if defined(ECONOMY_INPUT)   


/* -------------------------- Private Constants ----------------------------*/

// C_D_ISR_Status bitfield
/**
    \def ECONOMY_HYST_ON_BIT
    \brief This flag is used as positioning.
    \details 

    \def ECONOMY_HYST_ON
    \brief This flag is used as weight. Set and clear in the function InputECO_IsEconomyOn().
    \details 
 
*/
         
#define ECONOMY_HYST_ON_BIT 5
#define ECONOMY_HYST_ON ((u8)(1<<ECONOMY_HYST_ON_BIT))

                                                           
/* -------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables --------------------------- */

u8 EconomyStatus;

/* -------------------------- Private functions --------------------------- */

/* --------------------------- Private macros ------------------------------ */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void InputECO_InitECOInput(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 
    \details None.\n
                            \par Used Variables
    \return \b void no value return.
    \note None.
    \warning None.
*/

void InputECO_InitECOInput(void)  
{                                   
 
  #ifdef ECONOMY_INPUT 
      INPUTECON_INIT;
      EconomyStatus = 0;
  #endif //ECONOMY_INPUT
}   

  

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL InputECO_IsEconomyOn(void)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    "link al requirement"
    \brief Return TRUE if economy input signal is on\n
    - else it return FALSE.\n
    \par Used Variables
    \return \b bool TRUE if economy input is present
    \note None.
    \warning None. */
    

BOOL InputECO_IsEconomyOn(void)
{
  u16 u16EcoADC;
   
    u16EcoADC = (u16)((u32)((u32)buffu16ADC_READING[ECONOMY_CONV]));
    
    if ((u16EcoADC < ECONOMY_IS_OFF_THRESHOLD_BIT) || ((u16EcoADC < ECONOMY_IS_ON_THRESHOLD_BIT) && !(BitTest(ECONOMY_HYST_ON,  EconomyStatus)))) 
    {
       BitClear(ECONOMY_HYST_ON,  EconomyStatus);
       return(FALSE);
    } 
    else 
    {
      BitSet(ECONOMY_HYST_ON,  EconomyStatus);
      return(TRUE);
    }
}       

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 InputECO_GetEconomySetPoint(void);
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    "link al requirement"
    \brief Return the value of the economy speed if economy input signal is on\n
    - else it return 0 \n
    \par Used Variables
    \return \b u8 Economy speed value
    \note None.
    \warning None.
*/   

u8 InputECO_GetEconomySetPoint(u8 u8SP)
{
  if ((u8SP!=0) && (InputECO_IsEconomyOn()))
      return ((u8) ECONOMY_SPEED);
  else 
      return((u8)u8SP);     
}

#endif  // ECONOMY_INPUT


/* END */

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
