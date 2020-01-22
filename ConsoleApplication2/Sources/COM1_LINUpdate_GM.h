/**************** (c) 2014  SPAL Automotive **********************
     
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
#ifdef GM_LIN_ENCODING

#ifndef COM_LIN_UPDATE_GM
#define COM_LIN_UPDATE_GM

/* Public Constants -------------------------------------------------------- */
#define COMMUNICATION_ERROR                 0
#define INTERNAL_OPERATIONAL_FAULT_PRESENT  3
#define POWER_OVERVOLTAGE_FAULT_PRESENT     4
#define POWER_OVERCURRENT_FAULT_PRESENT     5
#define OVER_TEMPERATURE_FAULT_PRESENT      6
#define MALFUNCTION_SPEED_SENSING           7

#define LIN_TIMEBASE_MSEC                   8u   // 8 ms. WARNING: do not exceeed Master Frames timebase!!


/* Public type definition -------------------------------------------------- */

/* -------------------------- Private typedefs ---------------------------- */ 
typedef enum
{ //enumerativo per gli errori
  InactivError,
  ActivError,
  NotImplementedError,
  InvalidError
}ErrorType;             //Frame CTR_FAN_LIN; Target Operating Mode

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
extern u16 u16NumLinBusIdle;     //Conteggio numero di periodi (8 msec) di Lin bus in IDLE
#pragma DATA_SEG DEFAULT

extern u8 u8GM_StatusByte;

/* Public Functions prototypes --------------------------------------------- */
void LIN_LoadEEPROMParameters(void);
void LIN_InitLinVariables(void);
void LIN_ManageLinTimebase(void);
void LIN_UpdateStatusFrame(void);
u8   LIN_GetLINInputSetPoint(void);
void LIN_ExecuteLINProcedure(void);
BOOL LIN_IsReadyToSleep(void);

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/

#endif //COM_LIN_UPDATE

#endif  // GM_LIN_ENCODING
