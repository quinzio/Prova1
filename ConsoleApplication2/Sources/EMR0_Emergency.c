/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  EMR0_Emergency.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file EMR0_Emergency.c
    \version see \ref FMW_VERSION 
    \brief Emergency routines for secure bridge turn off.
    \details Inside routines have to be used to secure turn off three phase 
    bridge and to turn off micro.\n
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
#include "MCN1_acmotor.h"
#include "MCN1_mtc.h"	   
#include "TIM1_RTI.h"		 
#include "TLE7184F.h"
#include MAC0_Register   
#include "INP1_InputSP.h"
#include "SCI.h"
#include "COM1_COM.h"
#include "WTD1_Watchdog.h"
#include "main.h"               /* Per SystStatus_t e per strDebug */
#include "DIA1_Diagnostic.h"
#include "LOG0_LogDataS12.h"
#include "EMR0_Emergency.h"     /* Per costanti BALAN_BRAKING_TIME e BALAN_DECREASING_SPEED_TIME */
#include "PBATT1_Power.h"       /* Per Power_InitPowerStruct()                                   */
#include "RVP1_RVP.h"           /* Per RVP_SetRVPOff()                                           */
#include "VBA1_Vbatt.h"         /* Per Vbatt_DisableVbattOn()                                    */


#ifdef LIN_INPUT
  #if defined(BMW_LIN_ENCODING)
    #include "COM1_LINUpdate_BMW.h"
  #elif defined(GM_LIN_ENCODING)
    #include "COM1_LINUpdate_GM.h"
  #else
    #error("Define LIN stack!!");
  #endif  /* BMW_LIN_ENCODING */
#endif  /* LIN_INPUT */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* --------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables ---------------------------- */

/* -------------------------- Public variables ----------------------------- */
  
/* --------------------------- Private functions --------------------------- */

/* --------------------------- Private macros ------------------------------ */


/*-----------------------------------------------------------------------------*/
/** 
    \fn void EMR_EmergencyDisablePowerStage(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function disables three phase bridge by getting gate driver in error 
    \details This function disables pwm generation and makes gate driver in error.
    Before coming out re initialization of pwm timer is done.\n
                            \par Used Variables
    None
    
    \return \b void no value return
    \note Before coming out re initialization of pwm timer is done.
    \warning None
*/
void EMR_EmergencyDisablePowerStage(void)
{
  HI_Z_BRIDGE_ON;           /* Set HI_Z_BRIDGE as output high */
  HI_Z_BRIDGE_SET_OUT;      /* In order to get SCDL high and to get TLE7184F in error */

  /* INTERRUPT_PWM_DISABLE; */  /* N.B. */

  #ifdef RVP_SW_MANAGEMENT_ENABLE
    (void)RVP_SetRVPOff();
  #endif  /* RVP_SW_MANAGEMENT_ENABLE */
                            /* Since Load dump is managed in sw way, PWM interrupt has to be keep alway on. */

  u16SineMagScaled = 0;     /* Inizializzo le variabili di ampiezza per mettere le uscite del timer PWM  a 0% duty in una condizione Safe */
  u16SineMagBuf = 0;
  u16SineMag = 0;

  (void)Power_InitPowerStruct();           /* Per resettare le misure di potenza. */
  ACM_Set_u16FreqRampSmoothPU((u16)0);     /* Clear u16FreqRampSmoothPU in order to restart */

  #ifdef BMW_LIN_ENCODING
    strMachine.u16FrequencyPU = 0;         /* Per avere congruenza con u16FreqRampSmoothPU nella funzione ACM_CheckWindmillShutdown(). */
  #endif  /* BMW_LIN_ENCODING */
  
  #if (!defined( LIN_INPUT ))
    INTERRUPT_TACHO_DISABLE;               /* N.B.:Nel caso di LIN-feedback, la frequenza restituita in evoluzione libera e' gestita dalla procedura Main_UpdateLinSpeedFeedback() */
    strMachine.u16FrequencyAppPU = 0;      /* e non u16FrequencyAppPU non va azzerata (affinche' la velocita' restituita evolva con continuita'). Senza LIN, ad un arresto del     */
                                           /* drive corrisponde un azzeramento immediato dell'informazione di frequenza elettrica imposta.                                         */
                                           /* Lo stesso vale per l'interrupt di Tacho, che deve rimanere attivo per consentire di ricavare la velocita' attuale dalla bemf.        */
  #endif  /* LIN_INPUT */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void EMR_SetStopMode(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function disables three phase bridge and turn off micro 
    \details This function: \n - Disables pwm generation \n - Get gate drive in error \n - Turn off micro \n
    If motor is running on balancing test, braking timing has done i.e. motor has left in free running
    for 4sec. before braking by turning on low side mos. After that wait indefinitely.\n
                            \par Used Variables
    None
    
    \return \b void no value return
    \note If motor is running on balancing test, braking timing has done i.e. motor has left in free running
    for 4sec. before braking by turning on low side mos.
    \warning None
*/
void EMR_SetStopMode(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  #ifdef LIN_INPUT
    INTERRUPT_TACHO_DISABLE;                                /* Clear pending interrupt, Disable tacho interrupt */

    LIN_TRANSCEIVER_OFF;

    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      DIAG_DiagnosticShutDown();                            /* To send parameters */
    #endif  /* DIAGNOSTIC_STATE */

    if ( InputSP_GetOperatingMode() == BALAN_TEST ) 
    {
      MTC_SettingForParkingPhase();
      RTI_SetTimeOutDuration(BALAN_BRAKING_TIME);
      BRAKING_ON_LOW;                                       /* In order to set dc to 0 i.e. low MOS on. */
      TLE_EnableOutputs();                                  /* PWM is present on MCO outputs */

      while (!RTI_IsTimeOutElapsed())
      {
        WTD_ResetWatchdog();
      }
    }

    #ifdef LOG_DATI
      LOG0_DataSaveOnly();
    #endif  /* LOG_DATI */

    HI_Z_BRIDGE_ON;                                         /* Set HI_Z_BRIDGE as output high */
    HI_Z_BRIDGE_SET_OUT;                                    /* In order to get SCDL high and to get TLE7184F in error */

    STAND_BY_OFF;                                           /* Reset STAND_BY as input Hi Z */
    STAND_BY_SET_OUT;                                       /* Set STAND_BY as output. */

    #ifdef RVP_SW_MANAGEMENT_ENABLE
      (void)RVP_SetRVPOff();
    #endif  /* RVP_SW_MANAGEMENT_ENABLE */

    RTI_SetTimeOutDuration(BALAN_BRAKING_TIME);             /* Wait 1sec resetting watchdog */
    while (!RTI_IsTimeOutElapsed())
    WTD_ResetWatchdog();  

    RESET_MICRO;                                            /* If after watchdog time micro isn't expired -> reset! */

  #else /* LIN_INPUT */

    if ( (OperatingMode_t)InputSP_GetOperatingMode() == BALAN_TEST )
    {
      INTERRUPT_PWM_DISABLE;                                /* Clear pending interrupt, disable PWM Interrupt */

      INTERRUPT_TACHO_DISABLE;                              /* Clear pending interrupt, Disable tacho interrupt */

      #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
        DIAG_DiagnosticShutDown();                          /* To send parameters */
      #endif  /* DIAGNOSTIC_STATE */

      HI_Z_BRIDGE_ON;                                       /* Set HI_Z_BRIDGE as output high */
      HI_Z_BRIDGE_SET_OUT;                                  /* In order to get SCDL high and to get TLE7184F in error */
     
      #ifdef RVP_SW_MANAGEMENT_ENABLE
        /*(void)RVP_SetRVPOff();*/ /* Gives warning C12056 better use the define below */
        RVP_OFF;
      #endif  /* RVP_SW_MANAGEMENT_ENABLE */

      SCI_Init();
      Com_SendVerificationBytes();                          /* Before turning off send bytes for TREQ027, TREQ029, TREQ035 compliance. */

      RTI_SetTimeOutDuration(BALAN_BRAKING_TIME);
      BRAKING_ON_LOW;                                       /* In order to set dc to 0 i.e. low MOS on. */
      TLE_EnableOutputs();                                  /* PWM is present on MCO outputs */

      while (RTI_IsTimeOutElapsed() == (BOOL)FALSE)
      {
        WTD_ResetWatchdog();  
      }

      HI_Z_BRIDGE_ON;                                       /* Set HI_Z_BRIDGE as output high */
      HI_Z_BRIDGE_SET_OUT;                                  /* In order to get SCDL high and to get TLE7184F in error */
     
      STAND_BY_OFF;                                         /* Reset STAND_BY as input Hi Z */
      STAND_BY_SET_OUT;                                     /* Set STAND_BY as output. */

      #ifdef LIN_TRANSCEIVER
        LIN_TRANSCEIVER_OFF;
      #endif  /* LIN_TRANSCEIVER */

      #if ( ( PCB == PCB262A ) || ( PCB == PCB258A ) )
        Vbatt_DisableVbattOn();
      #endif  /* PCB */
#ifdef _CANTATA_
        return;
#else
      for(;;){}                                              /* Wait indefinitely with STAND_BY low -> Get TLE to sleep. */
#endif
  	}
  	else if ( (OperatingMode_t)InputSP_GetOperatingMode() == DEBUG_TEST )
  	{
      INTERRUPT_PWM_DISABLE;                                /* Clear pending interrupt, disable PWM Interrupt */

      INTERRUPT_TACHO_DISABLE;                              /* Clear pending interrupt, Disable tacho interrupt */
      
      #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
        DIAG_DiagnosticShutDown();                          /* To send parameters */
      #endif  /* DIAGNOSTIC_STATE */

      HI_Z_BRIDGE_ON;                                       /* Set HI_Z_BRIDGE as output high */
      HI_Z_BRIDGE_SET_OUT;                                  /* In order to get SCDL high and to get TLE7184F in error */

      #ifdef RVP_SW_MANAGEMENT_ENABLE
        /*(void)RVP_SetRVPOff();*/ /* Gives warning C12056 better use the define below */
        RVP_OFF;
      #endif  /* RVP_SW_MANAGEMENT_ENABLE */

      SCI_Init();
      Com_SendParameterBytes();                             /* Before turning off send bytes for TREQ027, TREQ029, TREQ035 compliance. */

      RTI_SetTimeOutDuration(BALAN_BRAKING_TIME);
      BRAKING_ON_LOW;                                       /* In order to set dc to 0 i.e. low MOS on. */
      TLE_EnableOutputs();                                  /* PWM is present on MCO outputs */  

      while (RTI_IsTimeOutElapsed() == (BOOL)FALSE)
      {
        WTD_ResetWatchdog();  
      }

      HI_Z_BRIDGE_ON;                                       /* Set HI_Z_BRIDGE as output high */
      HI_Z_BRIDGE_SET_OUT;                                  /* In order to get SCDL high and to get TLE7184F in error */

      STAND_BY_OFF;                                         /* Reset STAND_BY as input Hi Z */
      STAND_BY_SET_OUT;                                     /* Set STAND_BY as output. */

      #ifdef LIN_TRANSCEIVER
        LIN_TRANSCEIVER_OFF;
      #endif  /* LIN_TRANSCEIVER */

      #if ( ( PCB == PCB262A ) || ( PCB == PCB258A ) )
        Vbatt_DisableVbattOn();
      #endif  /* PCB */

#ifdef _CANTATA_
      return;
#else
      for(;;){}                                             /* Wait indefinitely with STAND_BY low -> Get TLE to sleep. */
#endif
    }
    else if ( (OperatingMode_t)InputSP_GetOperatingMode() != NORMAL_OPERATION )
    {
      INTERRUPT_PWM_DISABLE;                                /* Clear pending interrupt, disable PWM Interrupt */

      INTERRUPT_TACHO_DISABLE;                              /* Clear pending interrupt, Disable tacho interrupt */

      #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
        DIAG_DiagnosticShutDown();                          /* To send parameters */
      #endif  /* DIAGNOSTIC_STATE */

      HI_Z_BRIDGE_ON;                                       /* Set HI_Z_BRIDGE as output high */
      HI_Z_BRIDGE_SET_OUT;                                  /* In order to get SCDL high and to get TLE7184F in error */

      #ifdef RVP_SW_MANAGEMENT_ENABLE
        (void)RVP_SetRVPOff();
      #endif  /* RVP_SW_MANAGEMENT_ENABLE */

      SCI_Init();
      Com_SendVerificationBytes();                          /* Before turning off send bytes for TREQ027, TREQ029, TREQ035 compliance. */

      STAND_BY_OFF;                                         /* Reset STAND_BY as input Hi Z */
      STAND_BY_SET_OUT;                                     /* Set STAND_BY as output. */

      #ifdef LIN_TRANSCEIVER
        LIN_TRANSCEIVER_OFF;
      #endif  /* LIN_TRANSCEIVER */

      #if ( ( PCB == PCB262A ) || ( PCB == PCB258A ) )
        Vbatt_DisableVbattOn();
      #endif  /* PCB */

#ifdef _CANTATA_
      return;
#else
      for(;;){}                                             /* Wait indefinitely with STAND_BY low -> Get TLE to sleep.*/
#endif
    }
    else
    {
      INTERRUPT_PWM_DISABLE;

      INTERRUPT_TACHO_DISABLE;                              /* Clear pending interrupt, Disable tacho interrupt */

      #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
        DIAG_DiagnosticShutDown();                          /* To send parameters */
      #endif  /* DIAGNOSTIC_STATE */

      #ifdef LOG_DATI
        LOG0_DataSaveOnly();
      #endif  /* LOG_DATI */

      HI_Z_BRIDGE_ON;                                       /* Set HI_Z_BRIDGE as output high */
      HI_Z_BRIDGE_SET_OUT;                                  /* In order to get SCDL high and to get TLE7184F in error */

      #ifdef RVP_SW_MANAGEMENT_ENABLE
        (void)RVP_SetRVPOff();
      #endif  /* RVP_SW_MANAGEMENT_ENABLE */
     
      STAND_BY_OFF;                                         /* Reset STAND_BY as input Hi Z */
      STAND_BY_SET_OUT;                                     /* Set STAND_BY as output. */

      #ifdef LIN_TRANSCEIVER
        LIN_TRANSCEIVER_OFF;
      #endif  /* LIN_TRANSCEIVER */

      #if ( ( PCB == PCB262A ) || ( PCB == PCB258A ) )
        Vbatt_DisableVbattOn();
      #endif  /* PCB */

      RESET_MICRO;                                          /* If after watchdog time micro isn't expired -> reset! */
    }
  #endif /*LIN_INPUT */
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
