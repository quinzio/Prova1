/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  RVP1_RVP.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file RVP1_RVP.c
    \version see \ref FMW_VERSION 
    \brief Funzioni di gestione del MOS di RVP.
    \details none
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
#include MAC0_Register
#include "RVP1_RVP.h"
#include "TIM1_RTI.h"       /* Per la define TIMEBASE_MSEC */

/*PRQA S 0380,0857 -- */

#ifdef RVP_SW_MANAGEMENT_ENABLE
/* --------------------------- Public Variables ----------------------------- */

/* --------------------------- Public Functions ----------------------------- */
/**
   \function RVP_InitRVP
   \author Ianni Fabrizio
   \date 24/03/2016
   
   \brief Resetta il pin di RVP ed inizializza il timer u16RVPTimer.
   \details None.
   \param None.
   \return void
   
   
 */

void RVP_InitRVP (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  RVP_OFF; 
  SET_RVP_OUT;                                         
}

void RVP_SetRVPOn (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  RVP_ON;                                         
}


void RVP_SetRVPOff (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  RVP_OFF;                                         
}


BOOL RVP_IsRVPOn (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;

  if ( PTS_PTS4 == (u8)1 )
  {
    ret = (BOOL)TRUE;
    /*return(TRUE);*/
  }
  else
  {
    ret = (BOOL)FALSE;
    /*return(FALSE);*/
  }
return(ret);
}

/**
   \fn RVP_SetRVPForROF
   \author Boschesi Roberto
   \date 15/03/2017
   
   \brief Resetta il timer di RVP, cosi' da accendere subito il relativo MOS prima di un aggancio al volo.
   \details Cio' serve per non sollecitare (in termini di potenza dissipata) il MOS di RVP con il drive in RUN.
   \param None.
   \return void
   
   
 */
 /*
void RVP_SetRVPForROF (void)
{
  u16RVPTimer = 0;                                         
}
*/

/**
   \function RVP_ManageRVPStatus
   \author Ianni Fabrizio
   \date 24/03/2016
   \brief Accende il MOS di RVP dopo u16RVPTimer secondi di attesa.
   \details None.
   \param None.
   \return void
   
   
 */
 /*  
void RVP_ManageRVPStatus (void)
{
  if ( !u16RVPTimer )
  {
    RVP_ON; 
  }
  else
  {
    RVP_OFF;
  }
}
*/  
#endif /* RVP_SW_MANAGEMENT_ENABLE */

/*** (c) 2016 SPAL Automotive ****************** END OF FILE **************/
