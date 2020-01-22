/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  COM1_COM.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include <string.h>            /* for EnableInterrupts macro */
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include "MCN1_acmotor.h"      /* it has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"
#include "SCI.h"
#include "MCN1_Tacho.h"
#include "INP1_InputPWM.h"
#include MAC0_Register         /* Needed for macro definitions */
#include "TMP1_Temperature.h"
#include "VBA1_Vbatt.h"
#include "CUR1_Current.h"
#include "SOA1_SOA.h"
#include "WTD1_Watchdog.h"
#include "PARAM1_Param.h"      /* Per indirizzi parametri taratura */
#include "COM1_COM.h"
#include "PBATT1_Power.h"      /* Per structPowerMeas.u16VbattPU */
#include "POWER1_MaxPower.h"
#include "main.h"              /* Per i flag di errore */


/** 
    \file COM1_COM.c
    \version see \ref FMW_VERSION 
    \brief Communication SPAL protocol.
    \details Routines that implement the SPLA communication protocol with no reference to HW devices.\n
    \note none
    \warning none
*/

/* -------------------------- Private Constants ----------------------------*/

/* -------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables --------------------------- */
#ifdef ENABLE_RS232_DEBUG
static u8 UartBuffer[40];
/* -------------------------- Private functions --------------------------- */
void Com_SendSystemByteRS232(u16);
#endif  // ENABLE_RS232_DEBUG

/* -------------------------- Private macros ------------------------------ */

/*-----------------------------------------------------------------------------
ROUTINE Name : Com_SendVerificationBytes

Description : send byte via RS232
-----------------------------------------------------------------------------*/
void Com_SendVerificationBytes(void)
{
  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)CUSTOMER);

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)PROJECT);

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)SW_VERSION);

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)((u16)Temp_GetTparam()>>8));

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)Temp_GetTparam());

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)((u16)Vbatt_GetVparam()>>8));

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)Vbatt_GetVparam());

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)((u16)Curr_GetIparam()>>8));

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)Curr_GetIparam());
}

/*----------------------------------------------------------------------------*/
/** 

    \fn Com_SendParameterBytes(void)
    \brief Send EEPROM parameters during Drive test  
    \details Send EEPROM parameters during Drive test:
               - SW VERSION Bytes (CUSTOMER - PROJECT - SW VERSION)
               - EEPROM trimming bytes with checksum (from PARAM_INIT to PARAM_END address)
               - BOOTLOADER VERSION (stored into BTL_VERS_ADDR byte)
               - DATA MATRIX Bytes (from DATAMATRIX_INIT to DATAMATRIX_END address)
                
    \note none
    \warning none
    \param none
    \return none

*/
/*PRQA S 0306 ++ #mandatory for memory management*/
void Com_SendParameterBytes(void)
{
  u8 *parameter;
  /*--- SW VERSION Bytes*/
  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)CUSTOMER);

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)PROJECT);

  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }
  SCI_PutByte((u8)SW_VERSION);

  /*--- EEPROM TRIMMIG Bytes*/
  parameter = PARAM_INIT; 

  while ( parameter <= PARAM_END )
  {
    while(SCI_IsTransmitCompleted() == 0u)
    {
       WTD_ResetWatchdog();
    }
    SCI_PutByte((u8)*parameter);
    parameter++;
  }

  /*--- Bootloader version Byte*/
  while(SCI_IsTransmitCompleted() == 0u)
  {
     WTD_ResetWatchdog();
  }

  parameter = BTL_VERS_ADDR;
  SCI_PutByte((u8)*parameter);

  /*--- DATA MATRIX Bytes */
  parameter = DATAMATRIX_INIT;

  while ( parameter <= DATAMATRIX_END )
  {
    while(SCI_IsTransmitCompleted() == 0u)
    {
       WTD_ResetWatchdog();
    }

    SCI_PutByte((u8)*parameter);
    parameter++;
  }
 }
/*PRQA S 0306 -- */

#ifdef ENABLE_RS232_DEBUG
/*-----------------------------------------------------------------------------
ROUTINE Name : Com_SendSystemDataRS232

Description : upload internal data to the PC via RS232 and hyperterminal for 
debug purposes
-----------------------------------------------------------------------------*/
void Com_SendSystemDataRS232(void)
{
  u16 u16ErrorType_tmp;

  if(SCI_IsTransmitCompleted())
  {
    u16ErrorType_tmp = Main_Get_u16ErrorType();

    strcpy((u8 *)(UartBuffer), (const u8 *)("") );

    #ifdef RS232_PHASE
      strcatu16(UartBuffer,(u16)strMachine.s16AngleDeltaPU);
      strcat(UartBuffer, " ");
      /* strcatu16(UartBuffer,(u16)strMachine.s16AnglePhiPU); */
      /* strcat(UartBuffer, " ");                             */
    #endif

    #ifdef RS232_SPEED
      strcatu16(UartBuffer,(u16)strMachine.u16FrequencyPU);
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_VOLTAGE
      /* strcatu16(UartBuffer, MTC_GetScaledVoltage()); */
      /* strcat(UartBuffer, " ");                       */
      strcatu16(UartBuffer,(u16)strMachine.u16VoltagePU);
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_BUS_VOLTAGE
      strcatu16(UartBuffer,(u16)Power_Get_structPowerMeas_u16VbattPU());
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_ERROR_TYPE
      strcatu16(UartBuffer,(u16)u16ErrorType_tmp);
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_EXTVAR
      strcatu16(UartBuffer,(u16)Temp_Get_BOOLSOATemperatureCorr());
      strcat(UartBuffer, " ");
      strcatu16(UartBuffer,(u16)MaxPower_GetPowerSetPoint());
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_CURRENT
      strcatu16(UartBuffer,(u16)strMachine.u16CurrentPU);
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_OFFSET_CURRENT
      strcatu16(UartBuffer,(u16)Curr_GetOffsetCurrent());
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_TAMB
      strcatu16(UartBuffer,(u16)Temp_GetTamb());
      strcat(UartBuffer, " ");
    #endif

    #ifdef RS232_TMOS
      strcatu16(UartBuffer,(u16)Temp_GetTMos());
      strcat(UartBuffer, " ");
    #endif

    strcat(UartBuffer, "\n\r"); /* Go to next line */

    SCI_PutString(UartBuffer);
  }
}
#endif  /* ENABLE_RS232_DEBUG */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
