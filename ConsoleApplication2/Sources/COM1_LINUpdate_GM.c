/**************** (c) 2018  SPAL Automotive **********************
     
PROJECT  : 3-phase PMAC sensorless motor drive
COMPILER : METROWERKS

MODULE  :  COM1_LINUpdate.c

VERSION :  see SPAL_Setup.h

CREATION DATE :	09/02/2018

AUTHORS :	Ianni Fabrizio

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

DESCRIPTION :   LIN-communication routines
              
-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

MODIFICATIONS :
 
******************************************************************************/
#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "SPAL_Setup.h"
#include "SpalBaseSystem.h"     // Per le grandezze base definite in SP_MIN_SPEED_VALUE e simili.
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register          // Per LIN_TRANSCEIVER_ON.
#include "COM1_LINUpdate_GM.h"
#include "lin_api.h"            // Per LIN-timer task.
#include "MCN1_acmotor.h"       // Per strMachine.u16FrequencyAppPU.
#include "TMP1_Temperature.h"   // Per Temp_GetOvertemperature().
#include "TIM1_RTI.h"           // Per RTI_PERIOD_MSEC.
#include "INP1_InputSP.h"       // Per SP_MIN_SPEED_VALUE e SP_MAX_SPEED_VALUE.
#include "FIR1_FIR.h"           // Per il filtro sulla velocita' via LIN.
#include "main.h"               // Per le costanti relative al filtro sulla velocita' via LIN.
#include "PARAM1_Param.h"       // Per indirizzi parametri taratura e relative tolleranze.


#ifdef GM_LIN_ENCODING

#ifdef LIN_BAL_MODE_PASSWORD
  #include "LINBalanceMode.h"
#endif  // LIN_BAL_MODE_PASSWORD


/* -------------------------- Metadata definitions ------------------------------ */
static const volatile u8 SerialNumberEEPROM[SERIAL_NUMBER_SIZE] SERIAL_NUMBER_ADDRESS ={0x04u,0x02u,0x00u,0x00u};


/* -------------------------- Private Constants ----------------------------*/
// LIN timebase (executed on the Real Time Interrupt).
#define  LIN_TIMEBASE_BIT                         (u16)(LIN_TIMEBASE_MSEC/RTI_PERIOD_MSEC)

// Scaling and offset values for LIN variables.
#define SPEED_LIN_SCALER                          10
#define OFFSET_FOR_LIN_VARIABLES                  110

// Error and warning notification delays.
#define  MAX_NUM_BUS_IDLE_MSEC                    (u16)5000   //   5 [s]
#define  MAX_NUM_BUS_IDLE_BIT                     (u16)((u16)MAX_NUM_BUS_IDLE_MSEC/LIN_TIMEBASE_MSEC)

// Speed Transfer Function.
#define u8_1st_threshold (u8)20
#define u8_2nd_threshold (u8)37
#define u8_3rd_threshold (u8)224
#define fdt_angular_shift                         (u8)3
#define fdt_angular_coeff                         (u8)((u16)((u16)((u16)SP_MAX_SPEED_VALUE-SP_MIN_SPEED_VALUE)<<fdt_angular_shift)/(u8)((u8)u8_3rd_threshold-u8_2nd_threshold))


/* -------------------------- Other definitions ----------------------------*/
#define ARC_MAXIMUM_COUNT (u8)3


/* -------------------------- Private variables --------------------------- */
/** 
    \var BOOL_LIN_WakeUp
    \brief BOOL variable for LIN wake up.
    \note This variable is set TRUE in case of LIN wake up and is set to FALSE in case of LIN sleep.
    \warning This variable is \b static.
    \details None.
*/
BOOL BOOL_LIN_WakeUp;

/** 
    \var u8_PrplCoolFn1SpdCmd
    \brief Speed setpoint in LIN control frame (CTR_FAN_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/      
u8 u8_PrplCoolFn1SpdCmd;

/** 
    \var PrplCoolFn1SpdActl
    \brief Drive actual speed in LIN status frame (ST_FAN_1_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u16 PrplCoolFn1SpdActl;

/**
    \var SerialNumber
    \brief  Metadata - serial number.
    \note 
    \warning 
    \details None.
*/
u8 SerialNumber[SERIAL_NUMBER_SIZE];

/** 
    \var u8GM_StatusByte
    \brief u8 error status variable.
    \note 
    \warning 
    \details None.
*/
u8 u8GM_StatusByte;

/**
    \var PrplCoolFn1_ARC
    \brief  Alive Rolling Counter.
    \note 
    \warning 
    \details None.
*/
u8 PrplCoolFn1_ARC;

#pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var BOOLRTI_LinTask
    \brief Flag for a single LIN timebase cycle.
    \note None.
    \warning This variable is \b static.
    \details None.
*/
BOOL BOOLRTI_LinTask;

/**
    \var BOOL_CMD_LinTask
    \brief  Flag to signalize the reception of a status frame.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_CMD_LinTask;

/** 
    \var u16LinTaskCounter
    \brief Counter for LIN timebase.
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u16 u16LinTaskCounter;

#pragma DATA_SEG DEFAULT

// Error and warnings
ErrorType PrplCoolFn1IntlOplFltPrsnt;     // 1. Frame ST_FAN_1_LIN: Internal Operational Fault.
ErrorType PrplCoolFn1PwrSysFltPrsnt;      // 2. Frame ST_FAN_1_LIN: Power System Fault.
ErrorType PrplCoolFn1OvrTempFltPrsnt;     // 3. Frame ST_FAN_1_LIN; Over-Temperature Fault.
ErrorType PrplCoolFn1ExtrnMtrOpnCkt;      // 4. Frame ST_FAN_1_LIN; Motor Open Circuit.
ErrorType PrplCoolFn1ExtrnMtrShrtPwr;     // 5. Frame ST_FAN_1_LIN; Short To Power.
ErrorType PrplCoolFn1ExtrnMtrShrtGnd;     // 6. Frame ST_FAN_1_LIN; Short To Ground.


/* -------------------------- Public variables --------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var u16NumLinBusIdle
    \brief Counter of periods (LIN timebase) during which the LIN bus is IDLE.
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u16 u16NumLinBusIdle;

#pragma DATA_SEG DEFAULT


/* -------------------------- Private functions --------------------------- */
void LIN_UpdateControlFrame(void);
void LIN_UpdateDriveActualSpeed(void);
void LIN_UpdateDriveActualErrors(void);
void LIN_ARC(void);


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_LoadEEPROMParameters (void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Load GM paramters from EEPROM.     
    \return \b None.
    \note None.
    \warning None
*/
void LIN_LoadEEPROMParameters (void)
 {
  u8 index;

  // Numero di serie del drive.
  for(index = 0 ; index < SERIAL_NUMBER_SIZE ; index++)
  {
    SerialNumber[index] = SerialNumberEEPROM[index]; 
  }
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_InitLinVariables(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Init Lin variables.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_InitLinVariables(void)
{
  u8_PrplCoolFn1SpdCmd = 0;  
  PrplCoolFn1SpdActl   = 0;          
                         
  PrplCoolFn1IntlOplFltPrsnt = InactivError;            // 1. Frame ST_FAN_1_LIN: Internal Operational Fault.
  PrplCoolFn1PwrSysFltPrsnt  = InactivError;            // 2. Frame ST_FAN_1_LIN: Power System Fault.
  PrplCoolFn1OvrTempFltPrsnt = InactivError;            // 3. Frame ST_FAN_1_LIN; Over-Temperature Fault.
  PrplCoolFn1ExtrnMtrOpnCkt  = NotImplementedError;     // 4. Frame ST_FAN_1_LIN; Motor Open Circuit.
  PrplCoolFn1ExtrnMtrShrtPwr = NotImplementedError;     // 5. Frame ST_FAN_1_LIN; Short To Power.
  PrplCoolFn1ExtrnMtrShrtGnd = NotImplementedError;     // 6. Frame ST_FAN_1_LIN; Short To Ground.
    
  u8GM_StatusByte = 0;
  PrplCoolFn1_ARC = 0;
  
  BOOL_CMD_LinTask  = FALSE;
  BOOLRTI_LinTask   = FALSE;  
  u16LinTaskCounter = LIN_TIMEBASE_BIT;  
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_ManageLinTimebase(void)
    \author	Ianni Fabrizio \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute timebase for all LIN operations. 
    \return \b None.
    \note None.
    \warning None
*/
void LIN_ManageLinTimebase(void)
{
  if( u16LinTaskCounter == 0 )                // Se il timer della base-tempi LIN e' scaduto...
  {
    BOOLRTI_LinTask = TRUE;                   // ...si abilita il flag per l'esecuzione dei task per la comunicazione LIN...
    u16LinTaskCounter = LIN_TIMEBASE_BIT;     // ...si ricarica il timer stesso...
    
    if ( u16NumLinBusIdle < U16_MAX )         // ...e si controlla che non sia giunto il timeout per mandare il drive in sleep 
    {
      u16NumLinBusIdle++;                     // ...(i.e. con bus LIN in idle).
    }
    else
    {
      // Nop();
    }
  
    #ifdef LIN_BAL_MODE_PASSWORD
      HandleBalModeTimeoutTimer();            // Gestione del timer per la modalita' BALANCE.
    #endif  // LIN_BAL_MODE_PASSWORD    
  }
  else
  {
    // Nop();
  }
  
  u16LinTaskCounter--;
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 LIN_GetLINInputSetPoint(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute the drive speed setpoint. 
    \return \b u8 value.
    \note None.
    \warning None.
*/
u8 LIN_GetLINInputSetPoint(void)
{
  u8 u8SetSpeed;
  
  // FdT GM.
  if ( u8_PrplCoolFn1SpdCmd < u8_1st_threshold )          // Se la ECU fornisce un setpoint al di sotto di u8_1st_threshold...
  {
    u8SetSpeed = 0;                                       // ...allora si arresta il drive.
  }
  else if ( ( u8_PrplCoolFn1SpdCmd >= u8_1st_threshold ) && ( u8_PrplCoolFn1SpdCmd <= u8_2nd_threshold ) )
  {
    u8SetSpeed = (u8)SP_MIN_SPEED_VALUE;                  // Tra u8_1st_threshold e u8_2nd_threshold -> velocita' minima.
  }
  else if ( (u8_PrplCoolFn1SpdCmd > u8_2nd_threshold) && (u8_PrplCoolFn1SpdCmd <= u8_3rd_threshold) )
  {
                                                          // Tra u8_2nd_threshold e u8_3rd_threshold -> interpolazione lineare.
    u8SetSpeed = (u8)((u8)SP_MIN_SPEED_VALUE + (u8)((u16)((u16)((u16)u8_PrplCoolFn1SpdCmd - u8_2nd_threshold)*fdt_angular_coeff)>>fdt_angular_shift));    
  }
  else
  {
    u8SetSpeed =(u8)SP_MAX_SPEED_VALUE;                   // Oltre u8_3nd_threshold -> velocita' massima.
  }             

  return(u8SetSpeed);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateControlFrame(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Update the LIN control frame sent by master. 
    \return \b None.
    \note None.
    \warning None
*/                                                                            
void LIN_UpdateControlFrame(void)
{                                                             
  if ( l_flg_tst_ECM_LIN1_CFM1_Cmd_MSG() == 1 )           // Se il flag di ricezione di un nuovo frame e' attivo..
  {                                                           
    l_flg_clr_ECM_LIN1_CFM1_Cmd_MSG();                    // ...lo si resetta...
    
    u8_PrplCoolFn1SpdCmd = l_u8_rd_PrplCoolFn1SpdCmd();   // ...si acquisisce il setpoint di velocita'.
  }
  else
  {                                                                       
    // Nop();
  }                                                                       
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateStatusFrame(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Update the LIN status frame to be sent to master.  
    \return \b None
    \note None.
    \warning None
*/
void LIN_UpdateStatusFrame(void)
{
  // Aggiornamento dello status frame con la velocita' attuale.  
  LIN_UpdateDriveActualSpeed();
  l_u16_wr_PrplCoolFn1SpdActl(PrplCoolFn1SpdActl);

  // Aggiornamento dell'Alive Rolling Count.
  LIN_ARC();
  l_u8_wr_PrplCoolFn1_ARC(PrplCoolFn1_ARC);

  // Aggiornamento degli errori.
  LIN_UpdateDriveActualErrors();
  l_bool_wr_PrplCoolFn1IntlOplFltPrsnt((BOOL)PrplCoolFn1IntlOplFltPrsnt);  // 1. Frame ST_FAN_1_LIN: Internal Operational Fault.
  l_bool_wr_PrplCoolFn1PwrSysFltPrsnt((BOOL)PrplCoolFn1PwrSysFltPrsnt);    // 2. Frame ST_FAN_1_LIN: Power System Fault.
  l_bool_wr_PrplCoolFn1OvrTempFltPrsnt((BOOL)PrplCoolFn1OvrTempFltPrsnt);  // 3. Frame ST_FAN_1_LIN; Over-Temperature Fault.
}
 
 
/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateDriveActualSpeed(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Speed encoding via LIN. 
    \return \b None.
    \note None.
    \warning None
*/
void LIN_UpdateDriveActualSpeed(void)
{
  u16 u16LinActualSpeed;

  // Velocita' del drive filtrata passa-basso.
  (void)FIR_UpdateFilt(LIN_SPEED_FILT,(s16)((u16)strMachine.u16FrequencyAppPU>>LIN_SPEED_PRESCALER_SHIFT));

  // Codifica LIN dell'informazione di velocita'.  
  u16LinActualSpeed = (u16)((u16)VectVarFilt[LIN_SPEED_FILT].s16NewOutput<<LIN_SPEED_PRESCALER_SHIFT);

  // Aggiornamento della variabile (LIN) di velocita'.
  if( ValBit(u8GM_StatusByte,MALFUNCTION_SPEED_SENSING) )
  {
    PrplCoolFn1SpdActl = 0;
  }
  else
  {  
    PrplCoolFn1SpdActl = (u16)((u16)OFFSET_FOR_LIN_VARIABLES/SPEED_LIN_SCALER) + (u16)((u32)((u32)u16LinActualSpeed*BASE_FREQUENCY_HZ*60)/(u32)((u32)FREQUENCY_RES_FOR_PU_BIT*POLE_PAIR_NUM*SPEED_LIN_SCALER)); 
  }
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateDriveActualErrors(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Update the drive error status to be sent via LIN. 
    \return \b None.
    \note None.
    \warning None
*/
void LIN_UpdateDriveActualErrors(void)
{
  // 1. Frame ST_FAN_1_LIN: Internal Operational Fault.
  if ( ValBit(u8GM_StatusByte, INTERNAL_OPERATIONAL_FAULT_PRESENT) )      // Se e' presente un errore sulla catena di misura (i.e. autotest)...
  {    
    PrplCoolFn1IntlOplFltPrsnt = ActivError;                              // ...allora viene attivata la relativa notifica via LIN...
  }
  else
  {
    PrplCoolFn1IntlOplFltPrsnt = InactivError;                            // ..altrimenti la si resetta.
  }   

  // 2. Frame ST_FAN_1_LIN: Power System Fault.
  if ( ValBit(u8GM_StatusByte, POWER_OVERVOLTAGE_FAULT_PRESENT) ||        // Se e' presente un errore di over/under-voltage...
       ValBit(u8GM_StatusByte, POWER_OVERCURRENT_FAULT_PRESENT) )         // ...oppure uno spegnimento per sovracorrente...
  {
    PrplCoolFn1PwrSysFltPrsnt = ActivError;                               // ...allora viene attivata la relativa notifica via LIN...
  }      
  else
  {
    PrplCoolFn1PwrSysFltPrsnt = InactivError;                             // ..altrimenti la si resetta.
  }
  
  // 3. Frame ST_FAN_1_LIN; Over-Temperature Fault.
  if ( (BOOL)Temp_GetOvertemperature() )                                  // Se e' presente un errore di sovratemperatura...
  {
    PrplCoolFn1OvrTempFltPrsnt = ActivError;                              // ...allora viene attivata la relativa notifica via LIN...
  }
  else
  {
    PrplCoolFn1OvrTempFltPrsnt = InactivError;                            // ..altrimenti la si resetta.
  }
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_ExecuteLINProcedure(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Execute LIN-communication procedure.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_ExecuteLINProcedure(void)
{
  if ( BOOLRTI_LinTask == TRUE )        // Se il flag della base-tempi LIN e' attivo (i.e. e' stato scandito un ciclo della base tempi)...
  {
    BOOLRTI_LinTask = FALSE;            // ...esso si resetta...
    LinFastTimerTask();                
    LinSlowTimerTask();                 // ...si eseguono le routine Fast e Slow-Task contenute nello stack LIN (fondamentali per implementare la comunicazione)...
    LIN_UpdateControlFrame();           // ...e si acquisisce il setpoint di velocita' dal control frame.
     
    if ( BOOL_CMD_LinTask == TRUE )     // Se e' arrivato dalla ECU il comando di fornire lo stato del drive...
    {
      BOOL_CMD_LinTask = FALSE;         // ...si resetta il flag relativo a tale notifica...    
      LIN_UpdateStatusFrame();          // ...e si aggiorna lo status frame da mandare alla centralina.
    }
    else
    {
      // Nop();
    }            
  }
  else
  {
    // Nop();
  }
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_ARC(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Execute a rolling count (on the ECU status request) to signalize the drive alive.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_ARC(void)
{
  if( PrplCoolFn1_ARC >= ARC_MAXIMUM_COUNT )    // Se il contatore PrplCoolFn1_ARC (sincrono con la richiesta dello status frame dalla centralina) e' arrivato a fondoscala...
  {
    PrplCoolFn1_ARC = 0;                        // ...esso viene resettato...
  }
  else
  {
    PrplCoolFn1_ARC++;                          // ...altrimenti si incrementa.
  }    
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_IsReadyToSleep(void)
    \author	Ianni Fabrizio \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check if the drive has to enter the sleep mode.
    \return \b None.
    \note None.
    \warning None
*/
BOOL LIN_IsReadyToSleep(void)
{
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )               // Se si e' in NORMAL_OPERATION...
  {
    if ( BOOL_LIN_WakeUp == FALSE )                                   // ...e non vi e' comunicazione sul bus LIN...
    {
      return(TRUE);                                                   // ...allora il drive deve andare in sleep.
    }  
    else
    {
      if ( u16NumLinBusIdle >= MAX_NUM_BUS_IDLE_BIT )                 // Se il bus LIN e' in idle ormai da un certo periodo di tempo (MAX_NUM_BUS_IDLE_SEC)...
      {
        return (TRUE);
      }                                                               // ...allora il drive viene messo in sleep...
      else
      {
        return (FALSE);                                               // ...altrimenti c'e' comunicazione e si e' operativi.
      }
    }
  }
  else
  {
    return(FALSE);                                                    // In TEST_OPERATION il drive resta sempre sveglio.
  }
}


/*---------------  VECTOR FUNCTIONS  --------------------------------------------------------------*/
//_________ LIN Wakeup ______________
void ApplLinWakeUp(void)
{
  LIN_TRANSCEIVER_ON;
  BOOL_LIN_WakeUp = TRUE;
}


//_________ LIN BusSleep ______________
void ApplLinBusSleep(void)
{
  BOOL_LIN_WakeUp = FALSE;
}


//____________ Target Reset J2602 ___________________
vuint8 ApplLinScTargetedReset(vuint8 bNAD)
{
  // called at the MasterReq frame checksum interrupt of a targeted reset command */
  //return  kLinSc_TargetedResetNoResponse; 
  //return kLinSc_TargetedResetPositiveResponse  ;  

  // if ( targetet reset can be performed ) 
  // {
  /* do a reset of some status variables - please clarifz the scope */
  /* TBD - set a flag here that triggers the reset of relevant variabled in the background loop */


   /* if a targeted reset command should also trigger the response_error to be set */
   //u8GM_response_error = 1;
   //     return kLinSc_TargetedResetPositiveResponse; 
  // }
  // else
  if( bNAD == 0x7F ) 
  {
    RESET_MICRO;
  } 
  else 
  {
    /* unable to perform the targeted reset */
    return kLinSc_TargetedResetNegativeResponse; 
  }
}

//_________ LIN ProtocolError ______________
void ApplLinProtocolError(vuint8 error, tLinFrameHandleType vMsgHandle )
{
  vMsgHandle = 0; // Dummy instruction (to avoid warnings)
  
  /* check if the response_error signal needs to be set */
  if ( (error == kLinSynchFieldError) ||
       (error == kLinChecksumError) ||
       (error == kLinBitError) ||
       (error == kLinFormatError) ||
       (error ==kLinErrInHeader ) ||
       (error ==kLinNoHeader ) ||
       (error ==kLinPIDParityError ))        
  {
    SetBit(u8GM_StatusByte, COMMUNICATION_ERROR);               
  }
}


/* -----------------------------------------------------------------------------
    &&&~ Prototypes of Pretransmit Functions
 ----------------------------------------------------------------------------- */
vuint8 ApplLinPreTransmit_RESP_MSG(vuint8* txBuffer)
{
  *txBuffer = 0; // Dummy instruction (to avoid warnings)
  
  //add variable to increment alive rolling count
  /* check if the response_error signal needs to be reported */
  _s_PrplCoolFn1CommErr = ValBit(u8GM_StatusByte, COMMUNICATION_ERROR);
  return kLinCopyData;
}


/* -----------------------------------------------------------------------------
    &&&~ Prototypes of Confirmation Functions
 ----------------------------------------------------------------------------- */
void ApplLinConf_RESP_MSG(tLinFrameHandleType vMsgHandle)
{
  vMsgHandle = 0; // Dummy instruction (to avoid warnings)
  
  /* response_error signal needs to be cleared */
  u8ClrBit(u8GM_StatusByte, COMMUNICATION_ERROR);
}


//_________ LIN Read By Identifier 0x01 ______________
vuint8 ApplLinScGetSerialNumber(vuint8* pbSerialNumber) 
{
  *pbSerialNumber = 0; // Dummy instruction (to avoid warnings)
  
  return( kLinSc_NoSerialNr );
}


//_________ LIN Read By Identifier User Defined 32-63______________
vuint8 ApplLinScGetUserDefinedId(vuint8 IdNumber, vuint8* pbIdValue)
{
  vuint8 bRet; //byte returned

  switch (IdNumber)
  {          
    #ifdef LIN_BAL_MODE_PASSWORD  
      case BAL_MODE_PASSW_CASE:
        if(BalanceModeTask(IdNumber))
        {
          LinSetBalanceMode();
          bRet = 1;                //lenght of the payload which is provided  
          pbIdValue [0] = BAL_MODE_PASSW_ACK;    //Ack
          pbIdValue [1] = 0xFF;    //unused
          pbIdValue [2] = 0xFF;    //unused
          pbIdValue [3] = 0xFF;    //unused
          pbIdValue [4] = 0xFF;    //unused
        }
        else
        {
          bRet =  kLinSc_SendNegativeResponse;  
        }
      break;
    #endif  
        
    default:
      bRet =  kLinSc_SendNegativeResponse;
    break;
  }
      
  return (bRet) ;
}


//_________ LIN ScSaveConfiguration ______________
void ApplLinScSaveConfiguration(void)
{
  // Nop();
}


//_________ LIN Fatal Error ______________
#if defined (LIN_ENABLE_DEBUG)
void ApplLinFatalError(vuint8 error)
{
  error = 0;    // Dummy instruction (to avoid warnings)
  while(1){};   //Attenzione !!! Watchdog deve essere abilitato
}
#endif
 

//_________ LIN CallBack ______________
// at receiving of LIN message
void ApplLinHeaderDetection(u8 FrameHandle)
{
  if( FrameHandle == 1 )      //0 Received 0x2C xx (CMD), 1 Received 0x2D (Request Status), 3 Altro
  {
    BOOL_CMD_LinTask = TRUE;
  }
  else
  {
    // Nop();
  }
}
#endif // GM_LIN_ENCODING

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
