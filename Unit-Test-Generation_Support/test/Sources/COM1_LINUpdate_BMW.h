/**************** (c) 2018  SPAL Automotive **********************
     
PROJECT  : 3-phase PMAC sensorless motor drive
COMPILER : METROWERKS

MODULE  :  COM1_LINUpdate.h

VERSION :  see SPAL_Setup.h

CREATION DATE :	24/07/2014 

AUTHORS :	Andrea Rossi

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

DESCRIPTION :   LIN Comunication Frames Update Routines
              
-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-

MODIFICATIONS :
 
******************************************************************************/
/** 
    \file COM1_LINUpdate.h
    \version see \ref FMW_VERSION 
    \brief HL communication protocol function prototypes.
    \details Here are stored all the prototypes referred to SPAL communication protocol.
    \note none
    \warning none
*/

#ifdef BMW_LIN_ENCODING

#ifndef COM_LIN_UPDATE_BMW
#define COM_LIN_UPDATE_BMW

/* Public Constants -------------------------------------------------------- */
#define LIN_TIMEBASE_MSEC 10u   // 10 ms. WARNING: do not exceeed Master Frames timebase!!


/* Private type definition -------------------------------------------------- */
 typedef enum
 {
   //enumerativo per l'Actual Operation Mode
   VariableSpeedActualtMode,
   AirDensityCompensationActualMode,
   SoftStartActualMode,
   SelfSustainingActualMode,
   RekuperationActualMode,
   WindmillActualMode,
   StandbyActualMode,
   InvalidActualMode
} ActualModeType;        // Frame ST_FAN_1_LIN; Fan Actual Operating Mode

typedef enum
{ //enumerativo per il Target Operation Mode
  VariableSpeedTargetMode,
  AirDensityCompensationTargetMode,
  SelfSustainingTargetMode,
  RekuperationTargetMode,
  SoftStartTargetMode,
  NotImplementedTargetMode,
  SPALreservedTargetMode,
  InvalidTargetMode
}TargetModeType;         // Frame CTR_FAN_LIN; Target Operating Mode

typedef enum
{ //enumerativo per gli errori
  InactivError,
  ActivError,
  NotImplementedError,
  InvalidError
}ErrorType;             //Frame CTR_FAN_LIN; Target Operating Mode


/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
extern u16 u16NumLinBusIdle;     //Conteggio numero di periodi (10msec) di Lin bus in IDLE
#pragma DATA_SEG DEFAULT

extern BOOL BOOLWmShutDown;
extern BOOL BOOLDriveShutdown;
extern BOOL BOOL_Current_Autotest_Error;
extern BOOL BOOL_Current_Limitation_Active;

/* Public Functions prototypes --------------------------------------------- */
void LIN_LoadEEPROMParameters(void);
void LIN_InitLinVariables(void);
void LIN_ManageLinTimebase(void);
void LIN_ExecuteLINProcedure(void);
BOOL LIN_IsReadyToSleep(void);
u8   LIN_GetLINInputSetPoint(void);
u8   LIN_CheckSoftStart(void);

#endif  // COM_LIN_UPDATE_BMW

/*** (c) 2018 SPAL Automotive ****************** END OF FILE **************/

#endif //BMW_LIN_ENCODING
