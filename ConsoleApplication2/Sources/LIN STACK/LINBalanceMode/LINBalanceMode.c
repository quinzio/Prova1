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

#include "LINBalanceMode.h"     // Prima dell'ifdef, altrimneti non si include SPAL_Setup.h e la define LIN_BAL_MODE_PASSWORD non viene vista dal linker!!

#ifdef LIN_BAL_MODE_PASSWORD

const unsigned char PasswordArray[NUM_BALANCE_PASSW_BYTES] =  PASSWORD_ARRAY;

#if CUSTOM_TIMER
  ObjTimerType   TimerInstanceName;
#else
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE
    unsigned int FreeCntTimeoutBalMode;
  #pragma DATA_SEG DEFAULT
#endif

unsigned char  BytesIdx;

enum{BALANCE_MODE_IDLE,BALANCE_MODE_REC_PASSWORD}BalanceModeState;


unsigned char BalanceModeTask(unsigned char CurrByte)
{
  unsigned char PasswRecSucces = 0; 
  
  switch(BalanceModeState)
  {
    case BALANCE_MODE_IDLE:
      
      if( CurrByte == PasswordArray[0] )
      {
        BalanceModeState = BALANCE_MODE_REC_PASSWORD;
        BytesIdx = 1;
        SetBalanceModeTimeOut(BAL_MODE_PASSW_BYTES_TIMEOUT);      //timeout intra byte
      }
      else
      {
        // Nop();
      }
    break;
    
    case BALANCE_MODE_REC_PASSWORD:
      if( BalModePasswTimeOutElapsed() )
      {
        BytesIdx = 0;
        BalanceModeState = BALANCE_MODE_IDLE;
      }
      else
      {
        if(CurrByte == PasswordArray[BytesIdx])
        {
          if(++BytesIdx >= NUM_BALANCE_PASSW_BYTES)
          {
            BalanceModeState = BALANCE_MODE_IDLE;
            BytesIdx = 0;
            PasswRecSucces = 0x01;
          } 
          else
          {
            SetBalanceModeTimeOut(BAL_MODE_PASSW_BYTES_TIMEOUT);
          }
        }
        else
        {
          BytesIdx = 0;
          BalanceModeState = BALANCE_MODE_IDLE;
        }
      }  
    break;
  }
  
  return PasswRecSucces;
}

#endif  // LIN_BAL_MODE_PASSWORD

/*** (c) 2018 SPAL Automotive ****************** END OF FILE **************/
