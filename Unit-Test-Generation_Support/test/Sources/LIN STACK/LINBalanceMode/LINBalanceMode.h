/**************** (c) 2018  SPAL Automotive *********************************
PROJECT  : 3-phase PMAC sensorless motor drive
COMPILER : METROWERKS

MODULE  :  LinBalanceMode.c

VERSION :  see SPAL_Setup.h

CREATION DATE :	01/04/2016

AUTHORS :	Tondelli Maicol

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

DESCRIPTION :   Routine for Balance Mode Entering with LIN communication               

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

MODIFICATIONS :  

******************************************************************************/

#include "SPAL_Setup.h"           // Per la define LIN_BAL_MODE_PASSWORD.

#ifdef  LIN_BAL_MODE_PASSWORD

#ifndef LINBALANCEMODE_H
#define LINBALANCEMODE_H

//-----------------------------------------------------------------------------------------------------------------------------------------
//---------------------------------------------Custom includes-----------------------------------------------------------------------------
#include "SpalTypes.h"            // Per la definizione dei vari types delle variabili (unsigned char, etc..).

// Per la define LIN_TIMEBASE_MSEC.
#ifdef BMW_LIN_ENCODING
  #include "COM1_LINUpdate_BMW.h"
#else
  #include "COM1_LINUpdate_GM.h"
#endif  // BMW35up_LIN_ENCODING

//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------Configuration---------------------------------------------------------------------------------
// I bytes della password devono rimanere all'interno del range 32-63 e non devono essere uguali a quelli utilizzati per RSID del cliente.
#define PASSW_ID1              52
#define PASSW_ID2              47
#define PASSW_ID3              61
#define PASSW_ID4              55
#define PASSW_ID5              49


#define NUM_BALANCE_PASSW_BYTES             5
#define PASSWORD_ARRAY                      {PASSW_ID1, PASSW_ID2, PASSW_ID3, PASSW_ID4, PASSW_ID5}


#define BAL_MODE_PASSW_CASE                      PASSW_ID1:\
                                            case PASSW_ID2:\
                                            case PASSW_ID3:\
                                            case PASSW_ID4:\
                                            case PASSW_ID5
                                            
#define BAL_MODE_PASSW_ACK                  0xC5                           
                                            

#define BAL_MODE_PASSW_BYTES_TIMEOUT        30   //ms

// Se settato a 0 per gestire i timeout verrà utilizzato un timer definito in questo modulo e ci si dovrà solo occupare di chiamare la 
// macro "HandleBalModeTimeoutTimer()" con una periodicità definita in PERIOD_TIMEOUT_TIMER.
// Se invece a 1 sarà possibile utilizzare un oggetto timer già esistente nel progetto, in particolare verrà creata un istanza di questo
// nuovo oggetto e le macro di "SetBalanceModeTimeOut(ms)" e "BalModePasswTimeOutElapsed()" dovranno poi essere definite in modo opportuna
// per l'utilizzo di questo timer
#define CUSTOM_TIMER                         0
//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------Cutom Macros---------------------------------------------------------------------------------------------
#define LinSetBalanceMode()              {InputSP_SetOperatingMode(BALAN_TEST);} //istruzioni per entrare in balance mode nel FW applicativo
//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------


//***********************************Timer***************************************************************************************

 
#if CUSTOM_TIMER

#define ObjTimerType                     //es. RTI_TimerUser_t           //Tipo dell' oggetto timer di cui verrà creata una nuova istanza
#define TimerInstanceName                TimerForBalanceMode             //Nome della nuova istanza dell' oggetto timer da creare

#define SetBalanceModeTimeOut(ms)        //es. (void)(RTI_SetTimeoutUser(&TimerInstanceName, ms))     //macro per settare il timeout

#define BalModePasswTimeOutElapsed()     //es. RTI_IsElapsedUser(&TimerInstanceName)                  //macro per controllare se il timeout è scaduto

#else

#define PERIOD_TIMEOUT_TIMER             LIN_TIMEBASE_MSEC //ms            //periodicità con cui viene chiamata "HandleBalModeTimeoutTimer()"

#define HandleBalModeTimeoutTimer()      {if(FreeCntTimeoutBalMode > 0) FreeCntTimeoutBalMode--;}    //gsetore del timer 

#define SetBalanceModeTimeOut(ms)        {FreeCntTimeoutBalMode = (signed char)((unsigned char)ms / PERIOD_TIMEOUT_TIMER);}    //macro per settare il timeout

#define BalModePasswTimeOutElapsed()     (FreeCntTimeoutBalMode <= 0)           //macro per controllare se il timeout è scaduto

#endif  // CUSTOM_TIMER
//*******************************************************************************************************************************

//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------Extern variables-----------------------------------------------------------------------------

#if !CUSTOM_TIMER
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE
  extern unsigned int FreeCntTimeoutBalMode;
  #pragma DATA_SEG DEFAULT
#endif
//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------

//-----------------------------------------------------------------------------------------------------------------------------------------
//--------------------------------------------Functions prototypes-------------------------------------------------------------------------
unsigned char BalanceModeTask(unsigned char CurrByte);

//-----------------------------------------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------------------------------------

#endif  // LINBALANCEMODE_H

#endif  // LIN_BAL_MODE_PASSWORD

/*** (c) 2018 SPAL Automotive ****************** END OF FILE **************/
