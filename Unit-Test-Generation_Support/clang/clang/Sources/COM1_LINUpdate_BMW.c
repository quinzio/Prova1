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
#include "COM1_LINUpdate_BMW.h"
#include "lin_api.h"            // Per LIN-timer task.
#include "MCN1_acmotor.h"       // Per strMachine.u16FrequencyAppPU.
#include "TMP1_Temperature.h"   // Per Temp_GetTamb().
#include "TIM1_RTI.h"           // Per RTI_PERIOD_MSEC.
#include "INP1_InputSP.h"       // PEr InputSP_GetInputSetPoint().
#include "CUR1_Current.h"       // Per Curr_GetBattCurrent() e Curr_GetPeakCurrent().
#include "PBATT1_Power.h"       // Per structPowerMeas.
#include "FIR1_FIR.h"           // Per il filtro sulla velocita' via LIN.
#include "main.h"               // Per le costanti relative al filtro sulla velocita' via LIN.
#include "PARAM1_Param.h"       // Per indirizzi parametri taratura e relative tolleranze.
#include "INT0_PWMIsrS12.h"     // Per IbattReg.u16IbattRegOut.


#ifdef BMW_LIN_ENCODING


#ifdef LIN_BAL_MODE_PASSWORD
  #include "LINBalanceMode.h"
#endif  // LIN_BAL_MODE_PASSWORD


/* -------------------------- Metadata definitions ------------------------------ */
const volatile u8  YEAR_PRODUCTION YEAR_PRODUCTION_ADDRESS                       = 0x15;                    // BCD format - see BMW requirements.
const volatile u8  MONTH_PRODUCTION MONTH_PRODUCTION_ADDRESS                     = 0x08;                    // BCD format - see BMW requirements
const volatile u8  DAY_PRODUCTION DAY_PRODUCTION_ADDRESS                         = 0x06;                    // BCD format - see BMW requirements.
const volatile u8  SerialNumberEEPROM[SERIAL_NUMBER_SIZE] SERIAL_NUMBER_ADDRESS  = {0x04,0x02,0x00,0x00};   // BCD format - 4 bytes - see BMW requirements.
const volatile u16 HW_REFERERENCE_NUMBER HW_REFERERENCE_NUMBER_ADDRESS           = 0x0301;                  // BCD format - see BMW requirements.
const volatile u16 SPAL_HW_REFERERENCE_NUMBER SPAL_HW_REFERERENCE_NUMBER_ADDRESS = 0x380A;                  // BCD format - see BMW requirements.


/* -------------------------- Private Constants ----------------------------*/
// LIN timebase (executed on the Real Time Interrupt).
#define  LIN_TIMEBASE_BIT                            (u16)(LIN_TIMEBASE_MSEC/RTI_PERIOD_MSEC)

// Scaling and offset values for LIN variables.
#define  OFFSET_FOR_LIN_VARIABLES                    (u8)55
#define  VBUS_LIN_SCALER                             (u8)4
#define  SPEED_LIN_SCALER                            (u8)20

// Implausible voltage thresholds.
#define  IMPLAUSIBLE_VOLTAGE_THRESHOLD               (float32_t)30  // [V]
#define  IMPLAUSIBLE_VOLTAGE_THRESHOLD_PU_RES_BIT    (u16)((float32_t)((float32_t)IMPLAUSIBLE_VOLTAGE_THRESHOLD/BASE_VOLTAGE_VOLT)*VOLTAGE_RES_FOR_PU_BIT)
#define  IMPLAUSIBLE_LD_SUPY_VOLTAGE                 (float32_t)7   // [V]
#define  IMPLAUSIBLE_LD_SUPY_VOLTAGE_PU_RES_BIT      (u16)((float32_t)((float32_t)IMPLAUSIBLE_LD_SUPY_VOLTAGE/BASE_VOLTAGE_VOLT)*VOLTAGE_RES_FOR_PU_BIT)

// Error and warning notification delays.
#define  ERROR_NOTIFICATION_DELAY_MSEC               (u16)20000  //  20 [s]
#define  ERROR_NOTIFICATION_DELAY_BIT                (u16)((u16)ERROR_NOTIFICATION_DELAY_MSEC / LIN_TIMEBASE_MSEC)      
#define  SLUGGISH_NOTIFICATION_DELAY_MSEC            (u16)900    // 0.9 [s]
#define  SLUGGISH_NOTIFICATION_DELAY_BIT             (u16)((u16)SLUGGISH_NOTIFICATION_DELAY_MSEC / LIN_TIMEBASE_MSEC)
#define  NON_SPECIFIC_ERROR_NOTIFICATION_DELAY_MSEC  (u16)5000   //   5 [s]
#define  NON_SPECIFIC_ERROR_NOTIFICATION_DELAY       (u16)((u16)NON_SPECIFIC_ERROR_NOTIFICATION_DELAY_MSEC / LIN_TIMEBASE_MSEC)
#define  MAX_NUM_BUS_IDLE_MSEC                       (u16)5000   //   5 [s]
#define  MAX_NUM_BUS_IDLE_BIT                        (u16)((u16)MAX_NUM_BUS_IDLE_MSEC/LIN_TIMEBASE_MSEC)
#define  SELF_SUSTAINING_BASE_DELAY_MSEC             (u16)6000   //   6 [s]
#define  SELF_SUSTAINING_BASE_DELAY_BIT              (u16)((u16)SELF_SUSTAINING_BASE_DELAY_MSEC / LIN_TIMEBASE_MSEC)      

// Speed Transfer Function.
#define  u8_1st_threshold                            (u8)3 
#define  u8_2nd_threshold                            (u8)3
#define  u8_3rd_threshold                            (u8)250
#define  u8_4th_threshold                            (u8)254
#define  fdt_angular_shift                           (u8)3
#define  fdt_angular_coeff                           (u8)((u16)((u16)((u16)SP_MAX_SPEED_VALUE-SP_MIN_SPEED_VALUE)<<fdt_angular_shift)/(u8)((u8)u8_3rd_threshold-u8_2nd_threshold))
#define  LIN_INVALID_SPEED_SETPOINT                  (u8)255

// Speed limits and thresholds.
#define LIN_MIN_SPEED                                (u8)((u16)RPM_MIN/SPEED_LIN_SCALER)
#define LIN_MAX_SPEED                                (u8)((u16)RPM_MAX_SPEED/SPEED_LIN_SCALER)

#define DELTA_RPM_TO_NOTIFY_WINDMILL                 (float32_t)80  // [rpm]
#define DELTA_FREQ_TO_NOTIFY_WINDMILL                (float32_t)((float32_t)((float32_t)DELTA_RPM_TO_NOTIFY_WINDMILL*POLE_PAIR_NUM)/60)
#define DELTA_FREQ_TO_NOTIFY_WINDMILL_PU_RES_BIT     (u16)((float32_t)((float32_t)DELTA_FREQ_TO_NOTIFY_WINDMILL*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)
 
#define MIN_THRESHOLD_TO_SHUTDOWN_SPEED_RPM          (u16)1200  // [rpm]
#define MIN_THRESHOLD_TO_SHUTDOWN_SPEED              (u8)((u16)MIN_THRESHOLD_TO_SHUTDOWN_SPEED_RPM/SPEED_LIN_SCALER)

// Slugghishness thresholds.
#define MIN_CURRENT_AMPERE                           (float32_t)53
#define MIN_CURRENT_RES_PU                           (u16)((float32_t)((float32_t)MIN_CURRENT_AMPERE/BASE_CURRENT_AMP)*CURRENT_RES_FOR_PU_BIT)
#define MAX_CURRENT_AMPERE                           (float32_t)100
#define MAX_CURRENT_RES_PU                           (u16)((float32_t)((float32_t)MAX_CURRENT_AMPERE/BASE_CURRENT_AMP)*CURRENT_RES_FOR_PU_BIT)

#define SLUGGISH_SHIFT                               (u8)8
#define SLUGGISH_SLOPE                               (u16)((u32)((u32)((u32)MAX_CURRENT_RES_PU - MIN_CURRENT_RES_PU)<<SLUGGISH_SHIFT) / (u32)((u32)LIN_MAX_SPEED - (u32)LIN_MIN_SPEED))
#define HYST_SLUGGISH_NUM                            (u16)31     // 31>>SLUGGISH_SHIFT <=> 31/256 <=> 12/100

// Temperature thresholds.
#define MINIMUM_TEMPERATURE_THRESHOLD                (s16)(-55)  // °C
#define MAXIMUM_TEMPERATURE_THRESHOLD                (s16)200    // °C


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
    \var u8_tarvl_v_fan_1_lin
    \brief Speed setpoint in LIN control frame (CTR_FAN_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/      
u8 u8_tarvl_v_fan_1_lin;

/** 
    \var u8_tarvl_t_fan_1_lin
    \brief Self-sustaining (after-run) timeout in LIN control frame (CTR_FAN_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u8 u8_tarvl_t_fan_1_lin; 

/** 
    \var u8_avl_rpm_fan_1_lin
    \brief Drive actual speed in LIN status frame (ST_FAN_1_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u8 u8_avl_rpm_fan_1_lin;

/** 
    \var u8_avl_u_fan_1_lin
    \brief Drive actual battery voltage in LIN status frame (ST_FAN_1_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u8 u8_avl_u_fan_1_lin;

/** 
    \var u8_avl_i_fan_1_lin
    \brief Drive actual battery current in LIN status frame (ST_FAN_1_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u8 u8_avl_i_fan_1_lin ;

/** 
    \var u8_avl_temp_fan_1_lin
    \brief Drive actual temperature in LIN status frame (ST_FAN_1_LIN).
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u8 u8_avl_temp_fan_1_lin; 


#pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var u16LinTaskCounter
    \brief Counter for LIN timebase.
    \note None.
    \warning This variable is \b static.
    \details None.
*/
u16 u16LinTaskCounter;

/** 
    \var BOOLRTI_LinTask
    \brief Flag for a single LIN timebase cycle.
    \note None.
    \warning This variable is \b static.
    \details None.
*/
BOOL BOOLRTI_LinTask;


#pragma DATA_SEG DEFAULT
/**
    \var u16TicsInto6sec
    \brief Counter (LIN timebase) of tics within a single 6-second period (self-sustaining mode).
    \note 
    \warning 
    \details None.
*/
u16 u16TicsInto6sec;

/**
    \var u8Num6Secs
    \brief Counter of 6-second periods within self-sustaining timeout.
    \note 
    \warning 
    \details None.
*/
u8 u8Num6Secs;

/**
    \var u16TicsInto20secForGD_ERROR
    \brief  Delay timer (LIN timebase) to signalize Bridge / TLE Error.
    \note 
    \warning 
    \details None.
*/
u16 u16TicsInto20secForGD_ERROR;

/**
    \var u16TicsInto20secForI_SWO
    \brief  Delay timer (LIN timebase) to signalize Overcurrent / Blocked rotor Error.
    \note 
    \warning 
    \details None.
*/
u16 u16TicsInto20secForI_SWO;

/**
    \var u16TicsInto20secForU_IPLB
    \brief  Delay timer (LIN timebase) to signalize Implausible Voltage Error.
    \note 
    \warning 
    \details None.
*/
u16 u16TicsInto20secForU_IPLB;

/**
    \var u16TicsInto5secForNonSpecificError
    \brief  Delay timer (LIN timebase) to signalize non-specific / speed not reached Error.
    \note 
    \warning 
    \details None.
*/
u16 u16TicsInto5secForNonSpecificError;

/**
    \var u16TicsInto20secForI_OFFS
    \brief  Delay timer (LIN timebase) to signalize (Peak) current offset Error.
    \note 
    \warning 
    \details None.
*/
u16 u16TicsInto20secForI_OFFS;

/**
    \var u16TicsInto20secForLD_SUPY_IPLB
    \brief  Delay timer (LIN timebase) to signalize Implausible Load Voltage Error.
    \note 
    \warning 
    \details None.
*/
u16 u16TicsInto20secForLD_SUPY_IPLB;

/**
    \var u16TicsDelaysecForSluggish
    \brief  Delay timer (LIN timebase) to signalize slugghishness Warning.
    \note 
    \warning 
    \details None.
*/
u16 u16TicsDelaysecForSluggish;

/**
    \var u16SluggishLowThresh
    \brief  Low threshold for slugghishness hysteresys.
    \note 
    \warning 
    \details None.
*/
u16 u16SluggishLowThresh;

/**
    \var u16SluggishHighThresh
    \brief  Low threshold for slugghishness hysteresys.
    \note 
    \warning 
    \details None.
*/
u16 u16SluggishHighThresh;

/**
    \var SWO_Overcurrent_Flag
    \brief  Flag to distinguish blocked rotor and overcurrent shutdown.
    \note 
    \warning 
    \details None.
*/
BOOL SWO_Overcurrent_Flag;

/**
    \var BOOL_ErrorTimeDelayCounterONForGD_ERROR
    \brief  Flag to activate bridge / TLE error timer.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_ErrorTimeDelayCounterONForGD_ERROR         = FALSE;

/**
    \var BOOL_ErrorTimeDelayCounterONForU_IPLB
    \brief  Flag to activate implausible-voltage error timer.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_ErrorTimeDelayCounterONForU_IPLB           = FALSE;

/**
    \var BOOL_ErrorTimeDelayCounterONForI_SWO
    \brief  Flag to activate blocked rotor / overcurrent error timer.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_ErrorTimeDelayCounterONForI_SWO            = FALSE;

/**
    \var BOOL_ErrorTimeDelayCounterONForNonSpecificError
    \brief  Flag to activate non-specific error timer.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_ErrorTimeDelayCounterONForNonSpecificError = FALSE;

/**
    \var BOOL_ErrorTimeDelayCounterONForI_OFFS
    \brief  Flag to activate current offset error timer.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_ErrorTimeDelayCounterONForI_OFFS           = FALSE;

/**
    \var BOOL_ErrorTimeDelayCounterONForLD_SUPY_IPLB
    \brief  Flag to activate implausible-load voltage error timer.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_ErrorTimeDelayCounterONForLD_SUPY_IPLB     = FALSE;

/**
    \var BOOL_ErrorTimeDelayCounterONForSluggish
    \brief  Flag to activate slugghishness warning timer.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_ErrorTimeDelayCounterONForSluggish         = FALSE;
 
/**
    \var u8YearProduction
    \brief  Metadata - year of production.
    \note 
    \warning 
    \details None.
*/  
u8 u8YearProduction;

/**
    \var u8MonthProduction
    \brief  Metadata - month of production.
    \note 
    \warning 
    \details None.
*/
u8 u8MonthProduction;

/**
    \var u8DayProduction
    \brief  Metadata - day of production.
    \note 
    \warning 
    \details None.
*/
u8 u8DayProduction;

/**
    \var SerialNumber
    \brief  Metadata - serial number.
    \note 
    \warning 
    \details None.
*/
u8 SerialNumber[SERIAL_NUMBER_SIZE];

/**
    \var u16HwReferenceNumber
    \brief  Metadata - BMW hardware reference number.
    \note 
    \warning 
    \details None.
*/
u16 u16HwReferenceNumber;

/**
    \var u16SPALHwReferenceNumber
    \brief  Metadata - SPAL hardware reference number.
    \note 
    \warning 
    \details None.
*/
u16 u16SPALHwReferenceNumber;

/**
    \var tar_opmo_fan_1_lin
    \brief  Target drive operating mode.
    \note 
    \warning 
    \details None.
*/
TargetModeType tar_opmo_fan_1_lin;

/**
    \var avl_opmo_fan_1_lin
    \brief  Actual drive operating mode.
    \note 
    \warning 
    \details None.
*/
ActualModeType avl_opmo_fan_1_lin;


// LIN error and warning definitions.
ErrorType  st_err_bri_fan_1_lin;                //  1. Frame ST_FAN_1_LIN: drive actual bridge/TLE error.     
ErrorType  st_err_bri_sc_fan_1_lin;             //  2. Frame ST_FAN_1_LIN: drive actual short-circuit error.     
ErrorType  st_err_temp_swo_fan_1_lin;           //  3. Frame ST_FAN_1_LIN: drive actual temperature error.     
ErrorType  st_err_i_swo_fan_1_lin;              //  4. Frame ST_FAN_1_LIN: drive actual current (switch-off) error.     
ErrorType  st_err_u_iplb_fan_1_lin;             //  5. Frame ST_FAN_1_LIN: drive actual implausible voltage error.     
ErrorType  st_err_fan_1_lin;                    //  6. Frame ST_FAN_1_LIN: drive actual non specific error (target speed is not reached).     
//communication error                           //  7. LIN-protocol dependent error, not managed by SPAL but by Vector API.     
ErrorType  st_err_i_offs_fan_1_lin;             //  8. Frame ST_FAN_1_LIN: drive actual current offset error.     
ErrorType  st_err_blk_fan_1_lin;                //  9. Frame ST_FAN_1_LIN: drive actual blocked fan error.     
ErrorType  st_err_temp_fan_1_lin;               // 10. Frame ST_FAN_1_LIN: drive actual temperature warning.     
ErrorType  st_err_i_fan_1_lin;                  // 11. Frame ST_FAN_1_LIN: drive actual overcurrent degradation warning - i.e. demagnetization.     
ErrorType  st_err_slu_fan_1_lin;                // 12. Frame ST_FAN_1_LIN: drive actual sluggishness warning.     
ErrorType  st_err_ld_supy_iplb_fan_1_lin;       // 13. Frame ST_FAN_1_LIN: drive actual implausible load supply (error).
ErrorType  st_err_gnd_lss_fan_1_lin;            // 14. Frame ST_FAN_1_LIN: ground loss - only for 48 V applications.     


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

/**
    \var BOOLWmShutDown
    \brief  Flag for Windmill shutdown.
    \note 
    \warning 
    \details None.
*/
BOOL BOOLWmShutDown;

/**
    \var BOOLDriveShutdown
    \brief  Flag for shutdown due to a drive error.
    \note 
    \warning 
    \details None.
*/
BOOL BOOLDriveShutdown;

/**
    \var BOOL_ErrorTimeDelayCounterONForSluggish
    \brief  Flag to get the offset current error during auto-test.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_Current_Autotest_Error    = FALSE;

/**
    \var BOOL_ErrorTimeDelayCounterONForSluggish
    \brief  Flag to get the activation of the overload control loop.
    \note 
    \warning 
    \details None.
*/
BOOL BOOL_Current_Limitation_Active = FALSE;


/* -------------------------- Private functions --------------------------- */
void LIN_PerformSelfSustainingTimebase(void);
void LIN_PerformDelayTimebases(void);
void LIN_ComputeSluggishThresholds(void);
void LIN_UpdateControlFrame(void);
void LIN_UpdateStatusFrame(void);
void LIN_UpdateDriveActualSpeed(void);
void LIN_UpdateDriveActualBusVoltage(void);
void LIN_UpdateDriveActualBusCurrent(void);
void LIN_UpdateDriveActualTemperature(void);
void LIN_UpdateDriveActualOPMO(void);
void LIN_UpdateDriveActualErrors(void);
void LIN_UpdateDriveActualWarnings(void);
void LIN_ResetErrors(void);
void LIN_ResetWarnings(void);
BOOL LIN_CheckErrorStatus (void);


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_LoadEEPROMParameters (void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Load BMW paramters from EEPROM.     
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

  // Anno/Mese/Giorno di produzione del drive.
  u8YearProduction  = YEAR_PRODUCTION;
  u8MonthProduction = MONTH_PRODUCTION;
  u8DayProduction   = DAY_PRODUCTION;

  // Numero identificativo dell'hardware. 
  u16HwReferenceNumber     = HW_REFERERENCE_NUMBER;
  u16SPALHwReferenceNumber = SPAL_HW_REFERERENCE_NUMBER;  
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
  u8_tarvl_v_fan_1_lin = 0;          
  u8_tarvl_t_fan_1_lin = 0;          
  tar_opmo_fan_1_lin   = InvalidTargetMode;
  avl_opmo_fan_1_lin   = InvalidActualMode;  
  
  u8_avl_rpm_fan_1_lin  = 0;          
  u8_avl_temp_fan_1_lin = 0;         
  u8_avl_i_fan_1_lin    = 0;           
  u8_avl_u_fan_1_lin    = 0;            
                                                      // Errori e warnings, secondo la mappatura BMW:
  st_err_bri_fan_1_lin          = InvalidError;       //  1. Frame ST_FAN_1_LIN: drive actual bridge/TLE error.
  st_err_bri_sc_fan_1_lin       = InvalidError;       //  2. Frame ST_FAN_1_LIN: drive actual short-circuit error.
  st_err_temp_swo_fan_1_lin     = InvalidError;       //  3. Frame ST_FAN_1_LIN: drive actual temperature error.
  st_err_i_swo_fan_1_lin        = InvalidError;       //  4. Frame ST_FAN_1_LIN: drive actual current (switch-off) error.
  st_err_u_iplb_fan_1_lin       = InvalidError;       //  5. Frame ST_FAN_1_LIN: drive actual implausible voltage error.
  st_err_fan_1_lin              = InvalidError;       //  6. Frame ST_FAN_1_LIN: drive actual non specific error (target speed is not reached).
  //communication error                               //  7. LIN-protocol dependent error, not managed by SPAL but by Vector API.
  st_err_i_offs_fan_1_lin       = InvalidError;       //  8. Frame ST_FAN_1_LIN: drive actual current offset error.
  st_err_blk_fan_1_lin          = InvalidError;       //  9. Frame ST_FAN_1_LIN: drive actual blocked fan error.
  st_err_temp_fan_1_lin         = InvalidError;       // 10. Frame ST_FAN_1_LIN: drive actual temperature warning.
  st_err_i_fan_1_lin            = InvalidError;       // 11. Frame ST_FAN_1_LIN: drive actual overcurrent degradation warning - i.e. demagnetization.
  st_err_slu_fan_1_lin          = InvalidError;       // 12. Frame ST_FAN_1_LIN: drive actual sluggishness warning.
  st_err_ld_supy_iplb_fan_1_lin = InvalidError;       // 13. Frame ST_FAN_1_LIN: drive actual implausible load supply (error).
  st_err_gnd_lss_fan_1_lin      = InvalidError;       // 14. Frame ST_FAN_1_LIN: ground loss - only for 48 V applications.

  u16TicsInto20secForGD_ERROR        = 0;
  u16TicsInto20secForI_SWO           = 0;
  u16TicsInto20secForU_IPLB          = 0;
  u16TicsInto20secForI_OFFS          = 0;
  u16TicsInto20secForLD_SUPY_IPLB    = 0;
  u16TicsInto5secForNonSpecificError = 0;  
  u16TicsDelaysecForSluggish         = 0;
  u16TicsInto6sec                    = 0;
  u8Num6Secs                         = 0;  
  
  u16SluggishLowThresh  = 0;
  u16SluggishHighThresh = 0;

  BOOLRTI_LinTask   = FALSE;  
  u16LinTaskCounter = LIN_TIMEBASE_BIT;
  
  BOOLWmShutDown    = FALSE;
  BOOLDriveShutdown = FALSE;   
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
    \fn void LIN_PerformSelfSustainingTimebase(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute the self-sustaining timeout on a 6-second timebase.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_PerformSelfSustainingTimebase(void)
{
  if ( tar_opmo_fan_1_lin == SelfSustainingTargetMode )                 // Se si e' in Self-sustaining mode (i.e. after-run)...
  {
    if ( u16TicsInto6sec < SELF_SUSTAINING_BASE_DELAY_BIT )             // ...e non sono passati 6 secondi...
    {
      u16TicsInto6sec++;                                                // ...allora si incrementa il timer (a base-tempi LIN) con cui scandire il suddetto intervallo.
    }
    else
    {
      if ( u8Num6Secs < u8_tarvl_t_fan_1_lin )                          // Se il numero di intervalli da 6 secondi e' inferiore al timeout del Self-sustaining mode...
      {
         u8Num6Secs++;
      }                                                                 // ...allora si incrementa il relativo contatore.
      else
      {
         // Nop();
      }
      u16TicsInto6sec = 0;                                              // Dopo ogni intervallo da 6 secondi, il relativo timer viene resettato. 
    }
  }
  else
  {
    u8Num6Secs = 0;                                                     // In qualsiasi altra modalita' operativa, si resettano sia il timer dei 6 secondi...
    u16TicsInto6sec = 0;                                                // ...che il contatore del numero di intervalli di durata 6 secondi.
  }    
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_PerformDelayTimebases(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute the error/warning notification timings.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_PerformDelayTimebases(void)
{
  if ( BOOL_ErrorTimeDelayCounterONForGD_ERROR == TRUE )                        // Nel caso di errore per drive/TLE...
  {
    if ( u16TicsInto20secForGD_ERROR < ERROR_NOTIFICATION_DELAY_BIT )           
    {
      u16TicsInto20secForGD_ERROR++;                                            // ...si incrementa (a base-tempi LIN) il relativo timer di notifica.
    }
    else
    {
      u16TicsInto20secForGD_ERROR = ERROR_NOTIFICATION_DELAY_BIT;               // Tale timer e' tenuto a fondoscala una volta raggiunto il timeout.
    }
  }
  else
  {
    u16TicsInto20secForGD_ERROR = 0;                                            // Se non vi e' alcun errore di drive/TLE, il relativo timer di notifica e' resettato.
  }
  
  
  if ( BOOL_ErrorTimeDelayCounterONForI_SWO == TRUE )                           // Nel caso di errore per overcurrent/drive bloccato...
  {    
    if ( u16TicsInto20secForI_SWO < ERROR_NOTIFICATION_DELAY_BIT )
    {
      u16TicsInto20secForI_SWO++;                                               // ...si incrementa (a base-tempi LIN) il relativo timer di notifica.
    }
    else
    {
      u16TicsInto20secForI_SWO = ERROR_NOTIFICATION_DELAY_BIT;                  // Tale timer e' tenuto a fondoscala una volta raggiunto il timeout.
    }
  }
  else
  {
     u16TicsInto20secForI_SWO  = 0;                                             // Se non vi e' alcun errore di overcurrent/drive bloccato, il relativo timer di notifica e' resettato.
  }

  
  if ( BOOL_ErrorTimeDelayCounterONForU_IPLB == TRUE )                          // Nel caso di errore per tensione (di batteria) oltre il limite ammissibile...
  {
    if ( u16TicsInto20secForU_IPLB < ERROR_NOTIFICATION_DELAY_BIT )
    {
      u16TicsInto20secForU_IPLB++;                                              // ...si incrementa (a base-tempi LIN) il relativo timer di notifica.
    }
    else
    {
      u16TicsInto20secForU_IPLB = ERROR_NOTIFICATION_DELAY_BIT;                 // Tale timer e' tenuto a fondoscala una volta raggiunto il timeout.
    }
  }
  else
  {
    u16TicsInto20secForU_IPLB  = 0;                                             // Se non vi e' alcun errore per tensione di batteria, il relativo timer di notifica e' resettato.
  }

  
  if ( BOOL_ErrorTimeDelayCounterONForLD_SUPY_IPLB == TRUE )                    // Nel caso di errore per tensione (di batteria) sotto il limite ammissibile...
  {     
    if ( u16TicsInto20secForLD_SUPY_IPLB < ERROR_NOTIFICATION_DELAY_BIT )
    {
      u16TicsInto20secForLD_SUPY_IPLB++;                                        // ...si incrementa (a base-tempi LIN) il relativo timer di notifica.
    }
    else
    {
      u16TicsInto20secForLD_SUPY_IPLB = ERROR_NOTIFICATION_DELAY_BIT;           // Tale timer e' tenuto a fondoscala una volta raggiunto il timeout.
    }
  }
  else
  {
    u16TicsInto20secForLD_SUPY_IPLB  = 0;                                       // Se non vi e' alcun errore per tensione di batteria, il relativo timer di notifica e' resettato.
  }


  if ( BOOL_ErrorTimeDelayCounterONForNonSpecificError == TRUE )                                                                        // Nel caso di errore non specifico...
  {
    if ( u16TicsInto5secForNonSpecificError < (u16)(NON_SPECIFIC_ERROR_NOTIFICATION_DELAY*(u8)((u8)LIN_CheckSoftStart()+1)) )           
    {
      u16TicsInto5secForNonSpecificError++;                                                                                             // ...si incrementa (a base-tempi LIN) il relativo timer di notifica.
    }
    else
    {
      u16TicsInto5secForNonSpecificError = (u16)(NON_SPECIFIC_ERROR_NOTIFICATION_DELAY*(u8)((u8)LIN_CheckSoftStart()+1));               // Tale timer e' tenuto a fondoscala una volta raggiunto il timeout (legato al tempo di startup, in base alla modalita' operativa selezionata).
    }
  }
  else
  {
    u16TicsInto5secForNonSpecificError = 0;                                                                                             // Se non vi e' alcun errore di tensione di batteria, il relativo timer di notifica e' resettato.
  }

  
  if ( BOOL_ErrorTimeDelayCounterONForI_OFFS == TRUE )                          // Nel caso di offset di corrente errato...
  {    
    if ( u16TicsInto20secForI_OFFS < ERROR_NOTIFICATION_DELAY_BIT )
    {
      u16TicsInto20secForI_OFFS++;                                              // ...si incrementa (a base-tempi LIN) il relativo timer di notifica d'errore.
    }
    else
    {
      u16TicsInto20secForI_OFFS = ERROR_NOTIFICATION_DELAY_BIT;                 // Tale timer e' tenuto a fondoscala una volta raggiunto il timeout.
    }
  }
  else
  {
    u16TicsInto20secForI_OFFS = 0;                                              // Se non vi e' alcun errore per offset di corrente, il relativo timer di notifica e' resettato.
  }

    
  if ( BOOL_ErrorTimeDelayCounterONForSluggish == TRUE )                        // Nel caso di warning per sovraccarico meccanico (slugghishness)...
  {          
    if ( u16TicsDelaysecForSluggish < SLUGGISH_NOTIFICATION_DELAY_BIT )
    {
      u16TicsDelaysecForSluggish++;                                             // ...si incrementa (a base-tempi LIN) il relativo timer di notifica.
    }
    else
    {
      u16TicsDelaysecForSluggish = SLUGGISH_NOTIFICATION_DELAY_BIT;             // Tale timer e' tenuto a fondoscala una volta raggiunto il timeout.
    }
  }
  else
  {
    u16TicsDelaysecForSluggish  = 0;                                            // Se non vi e' alcun sovraccarico, il relativo timer di notifica e' resettato.
  }     
}


/*-----------------------------------------------------------------------------*/
 /** 
    \fn void LIN_ComputeSluggishThresholds (void)
    \author	Ianni Fabrizio \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute sluggishness warning thresholds.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_ComputeSluggishThresholds (void)
{
  u16 u16SluggishThresh,u16SluggishHyst;

  // Interpolazione lineare per il calcolo soglia di slugghishness (funzione lineare Corrente vs Velocita').
  u16SluggishThresh = (u16)((u16)((u32)((u32)u8_avl_rpm_fan_1_lin*SLUGGISH_SLOPE)>>SLUGGISH_SHIFT) + MIN_CURRENT_RES_PU);
  
  // Calcolo dell'isteresi.
  u16SluggishHyst = (u16)((u32)((u32)u16SluggishThresh*HYST_SLUGGISH_NUM)>>SLUGGISH_SHIFT);

  // Calcolo delle soglie inferiore e superiore della banda d'isteresi per il warning di slugghishness.
  u16SluggishLowThresh  = (u16)((u16)u16SluggishThresh - u16SluggishHyst);    
  u16SluggishHighThresh = (u16)((u16)u16SluggishThresh + u16SluggishHyst);
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
  static u8 u8SetSpeed;   // N.B.: u8SetSpeed DEVE essere static per mantenere l'ultimo valore di setpoint calcolato.
                          // Cio' e' fondamentale in SelfSustainingTargetMode, ad esempio, in cui si ritorna sempre il
                          // valore precedente fino allo scadere del timer!!  
  
  if ( tar_opmo_fan_1_lin == AirDensityCompensationTargetMode )                     // Se si e' in modalita' Air-Density Compensation...
  {
    u8SetSpeed = SP_EMERGENCY_SPEED;                                                // ...si ritorna il setpoint massimo (i.e. SP_EMERGENCY_SPEED).
  }
  else if ( tar_opmo_fan_1_lin == SelfSustainingTargetMode )                        // Se si e' in modalita' Self-Sustaining...
  {
    if ( u8Num6Secs >= u8_tarvl_t_fan_1_lin )                                       // ...e si e' arrivati al timeout...
    {
      u8SetSpeed = 0;                                                               // ...allora il drive deve arrestarsi...
    }
    else
    {
      // Nop();                                                                     // ...altrimenti si resta all'ultimo setpoint valido.
    }
  }                                                                                 // Se si e' in modalita' Variable Speed o Soft Start...
  else if ( ( tar_opmo_fan_1_lin == VariableSpeedTargetMode ) || ( tar_opmo_fan_1_lin == SoftStartTargetMode ) )
  {
    if ( BOOLWmShutDown == TRUE )                                                   // ...e occorre fermare il drive a causa del Windmill...
    {
      u8SetSpeed = 0;                                                               // ...allora si ritorna setpoint nullo.      
    } 
    else
    {
      if ( u8_tarvl_v_fan_1_lin < u8_1st_threshold )                                // Se la ECU comanda al drive di arrestarsi...
      {
        if ( u8_avl_rpm_fan_1_lin >= MIN_THRESHOLD_TO_SHUTDOWN_SPEED )              // ...e si e' sopra la velocita' di arresto del drive...
        {                                                                         
          if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )                              // ... e si e' "sul campo" (non im modalita' produzione)...
          {                                                                       
             u8SetSpeed = MIN_THRESHOLD_TO_SHUTDOWN_SPEED;                          // ...allora viene avviata una rampa di decelerazione controllata...
          }
          else
          {
             u8SetSpeed = 0;                                                        // (In modalita' produzione (i.e. equilibratura), si ferma il drive senza decelerazione controllata).
          }  
        }        
        else 
        {
          u8SetSpeed = 0;                                                           // ...e al di sotto di MIN_THRESHOLD_TO_SHUTDOWN_SPEED si arresta il drive.
        }
      }
      else
      {                                                                             // Se la ECU richiede una velocita' != 0, si esegue la FdT cliente. 
        if ( u8_tarvl_v_fan_1_lin <= u8_2nd_threshold )
        {
          u8SetSpeed = SP_MIN_SPEED_VALUE;                                          // Tra u8_1st_threshold e u8_2nd_threshold -> velocita' minima.
        }
        else if ( ( u8_tarvl_v_fan_1_lin > u8_2nd_threshold ) && ( u8_tarvl_v_fan_1_lin <= u8_3rd_threshold ) )   // Tra u8_2nd_threshold e u8_3rd_threshold -> interpolazione lineare.
        {
          u8SetSpeed = (u8)((u8)SP_MIN_SPEED_VALUE + (u8)((u16)((u16)((u16)u8_tarvl_v_fan_1_lin - u8_2nd_threshold)*fdt_angular_coeff)>>fdt_angular_shift));
        }
        else if ( (u8_tarvl_v_fan_1_lin > u8_3rd_threshold ) && ( u8_tarvl_v_fan_1_lin <= u8_4th_threshold ) )    // Tra u8_3rd_threshold e u8_4th_threshold -> Nnen(i.e. SP_MAX_SPEED_VALUE).
        {
          u8SetSpeed = SP_MAX_SPEED_VALUE;
        }
        else                                                                        // Oltre u8_4th_threshold -> setpoint nullo.
        {                                                                           // N.B.: questa condizione mappa anche target di velocita' invalido dalla centralina
          u8SetSpeed = 0;                                                           // (i.e. u8_tarvl_v_fan_1_lin == 255).
        }
      }        
    }
  }
  else
  { 
    u8SetSpeed = 0;                                                                 // In qualsiasi altra modalita' operativa, il setpoint ritornato e' nullo.
  }
 
  return((u8)u8SetSpeed);
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
  if ( l_flg_tst_CTR_FAN_LIN__MLIN_4() == 1 )                                     // Se il flag di ricezione di un nuovo frame e' attivo...
  {
    l_flg_clr_CTR_FAN_LIN__MLIN_4();                                              // ...lo si resetta...
    
    u8_tarvl_v_fan_1_lin = l_u8_rd_TARVL_V_FAN_1_LIN__MLIN_4();                   // ...si acquisisce il setpoint di velocita'...
    
    if  ( ( u16TicsInto6sec == 0 ) && ( u8Num6Secs == 0 ) )                       // ...e, se il timer di after-run non e' gia' attivo,...
    {
      u8_tarvl_t_fan_1_lin = l_u8_rd_TARVL_T_FAN_1_LIN__MLIN_4();                 // ...si acquisisce il nuovo timeout.
    }
    else
    {
       // Nop();
    }
          
    tar_opmo_fan_1_lin = (TargetModeType)(l_u8_rd_TAR_OPMO_FAN_1_LIN__MLIN_4());   // Dal frame si ottiene anche la modalita' operativa richiesta dalla centralina. 
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
  l_u8_wr_AVL_RPM_FAN_1_LIN__MLIN_4(u8_avl_rpm_fan_1_lin);

  // Aggiornamento dello status frame con la temperatura del drive.
  LIN_UpdateDriveActualTemperature();
  l_u8_wr_AVL_TEMP_FAN_1_LIN__MLIN_4(u8_avl_temp_fan_1_lin);

  // Aggiornamento dello status frame con la corrente di batteria attuale.
  LIN_UpdateDriveActualBusCurrent();
  l_u8_wr_AVL_I_FAN_1_LIN__MLIN_4(u8_avl_i_fan_1_lin);

  // Aggiornamento dello status frame con la tensione di batteria attuale.
  LIN_UpdateDriveActualBusVoltage();
  l_u8_wr_AVL_U_FAN_1_LIN__MLIN_4(u8_avl_u_fan_1_lin);

  // Aggiornamento dello status frame con la modalita' operativa attuale.
  LIN_UpdateDriveActualOPMO();
  l_u8_wr_AVL_OPMO_FAN_1_LIN__MLIN_4((u8)(avl_opmo_fan_1_lin));

  // Aggiornamento dei warnings (fatto qui, a base-tempi LIN, perche' non c'e' bisogno di intercettare dal main u16ErrorType come con gli errori).
  LIN_UpdateDriveActualWarnings();
  
   // Aggiornamento dello status frame con errori e warnings.                           // Errori e warnings, secondo la mappatura BMW:
  l_u8_wr_ST_ERR_BRI_FAN_1_LIN__MLIN_4((u8)(st_err_bri_fan_1_lin));                     //  1. Frame ST_FAN_1_LIN: drive actual bridge/TLE error.
  l_u8_wr_ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4((u8)(st_err_bri_sc_fan_1_lin));               //  2. Frame ST_FAN_1_LIN: drive actual short-circuit error.
  l_u8_wr_ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4((u8)(st_err_temp_swo_fan_1_lin));           //  3. Frame ST_FAN_1_LIN: drive actual temperature error.
  l_u8_wr_ST_ERR_I_SWO_FAN_1_LIN__MLIN_4((u8)(st_err_i_swo_fan_1_lin));                 //  4. Frame ST_FAN_1_LIN: drive actual current (switch-off) error.
  l_u8_wr_ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4((u8)(st_err_u_iplb_fan_1_lin));               //  5. Frame ST_FAN_1_LIN: drive actual implausible voltage error.
  l_u8_wr_ST_ERR_FAN_1_LIN__MLIN_4((u8)(st_err_fan_1_lin));                             //  6. Frame ST_FAN_1_LIN: drive actual non specific error (target speed is not reached).
                                                                                        //  7. LIN-protocol dependent error, not managed by SPAL but by Vector API.
  l_u8_wr_ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4((u8)(st_err_i_offs_fan_1_lin));               //  8. Frame ST_FAN_1_LIN: drive actual current offset error.
  l_u8_wr_ST_ERR_BLK_FAN_1_LIN__MLIN_4((u8)(st_err_blk_fan_1_lin));                     //  9. Frame ST_FAN_1_LIN: drive actual blocked fan error.
  l_u8_wr_ST_ERR_TEMP_FAN_1_LIN__MLIN_4((u8)(st_err_temp_fan_1_lin));                   // 10. Frame ST_FAN_1_LIN: drive actual temperature warning.
  l_u8_wr_ST_ERR_I_FAN_1_LIN__MLIN_4((u8)(st_err_i_fan_1_lin));                         // 11. Frame ST_FAN_1_LIN: drive actual overcurrent degradation warning - i.e. demagnetization.
  l_u8_wr_ST_ERR_SLU_FAN_1_LIN__MLIN_4((u8)(st_err_slu_fan_1_lin));                     // 12. Frame ST_FAN_1_LIN: drive actual sluggishness warning.
  l_u8_wr_ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4((u8)(st_err_ld_supy_iplb_fan_1_lin));   // 13. Frame ST_FAN_1_LIN: drive actual implausible load supply (error).
  l_u8_wr_ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4((u8)(st_err_gnd_lss_fan_1_lin));             // 14. Frame ST_FAN_1_LIN: ground loss - only for 48 V applications.
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
  #ifndef BATTERY_DEGRADATION
    (void)FIR_UpdateFilt(LIN_SPEED_FILT,(s16)((u16)strMachine.u16FrequencyAppPU>>LIN_SPEED_PRESCALER_SHIFT));
  #else
    (void)FIR_UpdateFilt(LIN_SPEED_FILT,(s16)((u16)((u16)strMachine.u16FrequencyAppPU - IbattReg.u16IbattRegOut)>>LIN_SPEED_PRESCALER_SHIFT));  
  #endif  // BATTERY_DEGRADATION
  
  // Codifica LIN dell'informazione di velocita'.
  u16LinActualSpeed = (u16)((u16)VectVarFilt[LIN_SPEED_FILT].s16NewOutput<<LIN_SPEED_PRESCALER_SHIFT);
  
  // Aggiornamento della variabile (LIN) di velocita'.
  u8_avl_rpm_fan_1_lin = (u8)((u32)((u32)u16LinActualSpeed*BASE_FREQUENCY_HZ*60)/(u32)((u32)FREQUENCY_RES_FOR_PU_BIT*POLE_PAIR_NUM*SPEED_LIN_SCALER)); 
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateDriveActualBusVoltage(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Bus-voltage encoding via LIN. 
    \return \b None.
    \note None.
    \warning None
*/
void LIN_UpdateDriveActualBusVoltage(void)
{
  // N.B.: qui non occorre chiamare Vbatt_GetBusVoltage() per aggiornare structPowerMeas.u16VbattPU.
  // Cio' e' fatto gia' nel main().
  
  // Aggiornamento della variabile (LIN) di tensione di batteria.
  u8_avl_u_fan_1_lin = (u8)((u32)((u32)Power_Get_structPowerMeas_u16VbattPU()*(u16)BASE_VOLTAGE_VOLT*VBUS_LIN_SCALER)/VOLTAGE_RES_FOR_PU_BIT);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateDriveActualBusCurrent(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Bus-current encoding via LIN. 
    \return \b None.
    \note None.
    \warning None
*/
void LIN_UpdateDriveActualBusCurrent(void)
{  
  (void)Curr_GetBattCurrent();  // Per avere l'aggiornamento di structPowerMeas.u16IbattPU in qualsiasi stato del drive.
  
  // Aggiornamento della variabile (LIN) di corrente di batteria.
  u8_avl_i_fan_1_lin = (u8)((u8)((u32)((u32)Power_Get_structPowerMeas_u16IbattPU()*(u16)BASE_CURRENT_AMP)/CURRENT_RES_FOR_PU_BIT) + (u8)OFFSET_FOR_LIN_VARIABLES);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateDriveActualTemperature(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Temperature encoding via LIN. 
    \return \b None.
    \note None.
    \warning None
*/
void LIN_UpdateDriveActualTemperature(void)
{
  s16 Temporary_Temperature;
  
  // Acquisizione della temperatura del drive.
  Temporary_Temperature = (s16)Temp_GetTamb();
  
  // Riporto da PU a gradi Celsius.
  Temporary_Temperature = (s16)((s32)((s32)Temporary_Temperature*TLE_BASE_TEMPERATURE_DEG)/TEMPERATURE_RES_FOR_PU_BIT);

  // Controllo dei limiti min/max (saturazione) di temperatura da segnalare via LIN.
  if ( Temporary_Temperature < MINIMUM_TEMPERATURE_THRESHOLD )
  {
    Temporary_Temperature = MINIMUM_TEMPERATURE_THRESHOLD;
  }
  else if (Temporary_Temperature >= MAXIMUM_TEMPERATURE_THRESHOLD )
  {
    Temporary_Temperature = MAXIMUM_TEMPERATURE_THRESHOLD;
  }
  else 
  {
    // Nop();
  }
  
  // Aggiornamento della variabile (LIN) di temperatura-drive.
  u8_avl_temp_fan_1_lin = (u8)((s16)Temporary_Temperature + (s16)OFFSET_FOR_LIN_VARIABLES); 
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateDriveActualOPMO(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Update the actual operation mode.   
    \return \b None.
    \note None.
    \warning None
*/
void LIN_UpdateDriveActualOPMO(void)
{
  u16 u16LinActualSetPoint,u16LinActualSpeed;

  // Si acquisice il setpoint di velocita' (dato dalla FdT) e la velocita' effettiva.
  u16LinActualSetPoint = (u16)SET_FREQUENCY_SET_POINT((u8)InputSP_GetInputSetPoint());  
  u16LinActualSpeed    = (u16)((u16)VectVarFilt[LIN_SPEED_FILT].s16NewOutput<<LIN_SPEED_PRESCALER_SHIFT);
  
  // Se la ECU indica la modalita' operativa Variable Speed o Soft-start...
  if ( ( tar_opmo_fan_1_lin == VariableSpeedTargetMode ) || ( tar_opmo_fan_1_lin == SoftStartTargetMode ) )
  {
    if ( u16LinActualSetPoint >= EL_FREQ_MIN_PU_RES_BIT )                     // ...e si ha setpoint di velocita' != 0...
    {
      if ( (u16)((u16)u16LinActualSetPoint+DELTA_FREQ_TO_NOTIFY_WINDMILL_PU_RES_BIT) >= u16LinActualSpeed )
      {                                                                       // ...e non si e' in Windmill (i.e. la velocita' effettiva del drive e' al piu' pari al setpoint + DELTA_FREQ_TO_NOTIFY_WINDMILL)...
        // ...allora si setta una delle modalita' operative richieste...
        if ( tar_opmo_fan_1_lin == VariableSpeedTargetMode )
        {
          avl_opmo_fan_1_lin = VariableSpeedActualtMode;
        }
        else
        {
          avl_opmo_fan_1_lin = SoftStartActualMode;
        }
      }
      else
      {
        avl_opmo_fan_1_lin = WindmillActualMode;                              // ...altrimenti si segnala la presenza di Windmill.
      }                                                                       // (N.B.: cio' anche con setpoint nullo e velocita' effettiva !=0, mentre il drive sta decelerando/derivando in evoluzione libera).
    }
    else                                                                      // Con setpoint nullo...
    {
      if ( u16LinActualSpeed >= DELTA_FREQ_TO_NOTIFY_WINDMILL_PU_RES_BIT )    // ...e drive in evoluzione libera (i.e. ponte spento)...
      {
        avl_opmo_fan_1_lin = WindmillActualMode;                              // ...si segnala Windmill fino a DELTA_FREQ_TO_NOTIFY_WINDMILL (i.e. drive fermo)...
      }
      else
      {
        avl_opmo_fan_1_lin = StandbyActualMode;                               // ...e poi si passa in Standby.
      }
    }
  }
  else if ( tar_opmo_fan_1_lin == SelfSustainingTargetMode )                  // Se la ECU indica la modalita' operativa Self-sustaining (i.e after-run)...
  {
    if ( u8Num6Secs < u8_tarvl_t_fan_1_lin )                                  // ...e il relativo timeout non e' ancora scaduto...
    {
      avl_opmo_fan_1_lin = SelfSustainingActualMode;                          // ..si setta la modalita' interna Self-sustaining...
    }
    else
    {
      if ( u16LinActualSpeed >= DELTA_FREQ_TO_NOTIFY_WINDMILL_PU_RES_BIT )    // ...altrimenti si e' in evoluzione libera (i.e. ponte spento)...
      {
        avl_opmo_fan_1_lin = WindmillActualMode;                              // ...per cui si segnala Windmill fino a DELTA_FREQ_TO_NOTIFY_WINDMILL (i.e. drive fermo)... 
      }
      else
      {
        avl_opmo_fan_1_lin = StandbyActualMode;                               // ...e poi si passa in Standby.
      }
    }  
  }
  else if ( tar_opmo_fan_1_lin == AirDensityCompensationTargetMode )          // Se la ECU indica la modalita' operativa Air-density compensation...
  {
    if ( u16LinActualSpeed >= DELTA_FREQ_TO_NOTIFY_WINDMILL_PU_RES_BIT )      // ...e si e' sopra DELTA_FREQ_TO_NOTIFY_WINDMILL (i.e. il drive sta girando)... 
    {
      avl_opmo_fan_1_lin = AirDensityCompensationActualMode;                  // ...si setta la modalita' interna Air-density compensation.
    }
    else
    {
      avl_opmo_fan_1_lin = StandbyActualMode;                                 // Se invece il drive e' fermo, si passa in Standby.
    }  
  }
  else if ( tar_opmo_fan_1_lin == RekuperationTargetMode )
  {
    avl_opmo_fan_1_lin = RekuperationActualMode;                              // Se la ECU indica la modalita' operativa Rekuperation, essa viene automaticamente settata.
  }
  else
  {
    avl_opmo_fan_1_lin = InvalidActualMode;                                   // Se nessuna delle precedenti modalita' operative e' stata settata, si passa in modalita' interna Invalid.
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
  u16 u16ErrorType_tmp;
  u8 u8SpecificErrorCount = 0;
    
  if ( ( tar_opmo_fan_1_lin == InvalidTargetMode ) || ( ( tar_opmo_fan_1_lin == SelfSustainingTargetMode ) && ( u8Num6Secs >= u8_tarvl_t_fan_1_lin ) ) )
  {                                                                                            // Se la modalita' target (dalla ECU) e' invalida, oppure si e' in Self-sustaining e il relativo timer e' scaduto...
    (void)LIN_ResetErrors();                                                                   // ...(i.e. il bus e' in IDLE e non arrivano piu' setpoint validi), allora tutti gli errori vengono resettati.
  }
  else
  {
    // N.B.: nota per l'if ( BOOLDriveShutdown == TRUE ) nel seguito.
    // Non si usa direttamente u8_tarvl_v_fan_1_lin poiche';
    //  1. occorrerebbe un AND aggiuntivo con la condizione u8_tarvl_v_fan_1_lin < 255
    //  2. in questo modo si intercettano gli errori anche in modalita' AirDensityCompensation,
    //     la quale puo' essere settata con u8_tarvl_v_fan_1_lin = 0
                
    if ( (u8)InputSP_GetInputSetPoint() > 0 )                                                  // Se la centralina richiede al drive di girare...
    {
      if ( BOOLDriveShutdown == TRUE )                                                         // ...e c'e' stato uno spegnimento a causa di un errore interno, allora:
      {
        u16ErrorType_tmp = Main_Get_u16ErrorType();

        // 1. Bridge error
        if ( ValBit(u16ErrorType_tmp,GD_ERROR_FLAG) )                                          // - Nel caso di errore TLE...
        {
          BOOL_ErrorTimeDelayCounterONForGD_ERROR = TRUE;                                      // ...si attiva il relativo flag...
        }
        else
        {
          // Nop();
        }
          
        if ( u16TicsInto20secForGD_ERROR >= ERROR_NOTIFICATION_DELAY_BIT )                     // ...e, allo scadere del relativo delay-timer,...
        {
          st_err_bri_fan_1_lin = ActivError;                                                   // ...si setta la notifica via LIN del bridge error...
        }
        else
        {
          st_err_bri_fan_1_lin = InactivError;                                                 // ...altrimenti si attende lo scadere del timer.
        }
        u8SpecificErrorCount += (u8)st_err_bri_fan_1_lin;                                      // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
         
        
        // 2. Phase short-circuit error - notifica di corto-circuito - sempre INATTIVA.
        st_err_bri_sc_fan_1_lin = InactivError;
        u8SpecificErrorCount += (u8)st_err_bri_sc_fan_1_lin;                                   // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
        
        
        // 3. Temperature Error
        if ( (StateSOATemp_T)Temp_GetTemperatureState() == OVER_MAX_TEMP )                     // - Nel caso di spegnimento per sovratemperatura... 
        {
          st_err_temp_swo_fan_1_lin = ActivError;
        }                                                                                      // ...si setta la notifica via LIN del temperature error..
        else
        {
          st_err_temp_swo_fan_1_lin = InactivError;                                            // ...altrimenti (anche se si e' in derating) non si notifica nulla.
        }
        u8SpecificErrorCount += (u8)st_err_temp_swo_fan_1_lin;                                 // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
        
        
        // 4. Switch-off overcurrent / 9. Blocked Fan
        if ( ValBit(u16ErrorType_tmp,I_LIM_FLAG) )                                             // - Nel caso di errore di corrente...
        {
          BOOL_ErrorTimeDelayCounterONForI_SWO = TRUE;                                         // ...si attiva il relativo flag.
        }
        else
        {
          // Nop();
        }  
        
        if ( SWO_Overcurrent_Flag == FALSE )                                                   // Se non si sa ancora se lo spegnimento sia avvenuto con drive in movimento o meno...
        {
          // N.B.: qui si usa u8_avl_rpm_fan_1_lin                                              
          // per detectare la velocita' istantantea effettiva
          // della ventola.
          if ( u8_avl_rpm_fan_1_lin >= LIN_MIN_SPEED )                                         // ...si considera la velocita' attuale rilevata e:
          {                                                                                     
            SWO_Overcurrent_Flag = TRUE;                                                       //     a. con drive in movimento, si dovra' notificare una sovracorrente
          }
          else
          {
            SWO_Overcurrent_Flag = FALSE;                                                      //     b. con drive fermo, si dovra' notificare un rotore bloccato
          }
        }
        else
        {
          // Nop();
        }
        
        if ( u16TicsInto20secForI_SWO >= ERROR_NOTIFICATION_DELAY_BIT )                        // Allo scadere del relativo timer...
        {
          if ( SWO_Overcurrent_Flag == TRUE )                                                  // ...in base allo stato del flag SWO_Overcurrent_Flag...
          { // 4. Switch-off over current
            st_err_i_swo_fan_1_lin = ActivError;  
          }                                                                                    // ...si setta la notifica via LIN di sovracorrente... 
          else
          { // 9. Blocked Fan
            st_err_blk_fan_1_lin = ActivError;                                                 // ...o di rotore bloccato.
          }
        }
        else
        {
          st_err_i_swo_fan_1_lin = InactivError;                                              // Se il  timer u16TicsInto20secForI_SWO non e' scaduto, non si attiva alcuna notifica.
          st_err_blk_fan_1_lin = InactivError;
        }
        u8SpecificErrorCount += (u8)((u8)st_err_i_swo_fan_1_lin+(u8)st_err_blk_fan_1_lin);    // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
                
        
        // 5. Implausible Voltage
        if ( Power_Get_structPowerMeas_u16VbattPU() > IMPLAUSIBLE_VOLTAGE_THRESHOLD_PU_RES_BIT )          // - Nel caso di tensione di batteria oltre il limite superiore (ammissibile)...
        {
          BOOL_ErrorTimeDelayCounterONForU_IPLB = TRUE;                                       // ...si setta il relativo flag...
          
          if ( u16TicsInto20secForU_IPLB >= ERROR_NOTIFICATION_DELAY_BIT )                    // ...e, allo scadere del relativo delay-timer,...
          {
            st_err_u_iplb_fan_1_lin = ActivError;                                             // ...si setta la notifica via LIN di implausible-voltage error...
          }
          else
          {
            st_err_u_iplb_fan_1_lin = InactivError;                                           // ...altrimenti si attende lo scadere del timer.
          }
        }
        else
        {
          st_err_u_iplb_fan_1_lin = InactivError;                                             // Se la tensione di batteria e' nei range consentiti...
          BOOL_ErrorTimeDelayCounterONForU_IPLB = FALSE;                                      // ...non si notifica nulla.
        }
        u8SpecificErrorCount += (u8)st_err_u_iplb_fan_1_lin;                                  // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
                
        
        // 7. Communication error - errore legato al protocollo LIN
        //   (gestito dall'API di Vector).
         
        
        // 8. Current Offset Error
        if ( BOOL_Current_Autotest_Error == TRUE )                                            // - Nel caso di offset errato sulla misura di corrente (di picco)...
        {
          BOOL_ErrorTimeDelayCounterONForI_OFFS = TRUE;                                       // ...si setta il relativo flag...
          
          if ( u16TicsInto20secForI_OFFS >= ERROR_NOTIFICATION_DELAY_BIT )                    // ...e, allo scadere del relativo delay-timer,...
          {
            st_err_i_offs_fan_1_lin = ActivError;                                             // ...si setta la notifica via LIN di current-offset error...
          }
          else
          {
            st_err_i_offs_fan_1_lin = InactivError;                                           // ...altrimenti si attende lo scadere del timer.
          }
        }
        else
        {
          st_err_i_offs_fan_1_lin = InactivError;                                             // Se la misura dell'offset di corrente e' valida...
          BOOL_ErrorTimeDelayCounterONForI_OFFS = FALSE;                                      // ...non si notifica nulla.
        }
        u8SpecificErrorCount += (u8)st_err_i_offs_fan_1_lin;                                  // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
        
        
        // 13. Implausible Load Supply Voltage      
        if ( Power_Get_structPowerMeas_u16VbattPU() < IMPLAUSIBLE_LD_SUPY_VOLTAGE_PU_RES_BIT )            // - Nel caso di tensione di batteria sotto il limite inferiore (ammissibile)...
        {
          BOOL_ErrorTimeDelayCounterONForLD_SUPY_IPLB = TRUE;                                 // ...si setta il relativo flag...
          
          if ( u16TicsInto20secForLD_SUPY_IPLB >= ERROR_NOTIFICATION_DELAY_BIT )              // ...e, allo scadere del relativo delay-timer,...
          {
            st_err_ld_supy_iplb_fan_1_lin = ActivError;                                       // ...si setta la notifica via LIN di load implausible-voltage error...
          }
          else
          {
            st_err_ld_supy_iplb_fan_1_lin = InactivError;                                     // ...altrimenti si attende lo scadere del timer.
          }
        }
        else
        {
          st_err_ld_supy_iplb_fan_1_lin = InactivError;                                       // Se la tensione di batteria e' nei range consentiti...
          BOOL_ErrorTimeDelayCounterONForLD_SUPY_IPLB = FALSE;                                // ...non si notifica nulla.
        }
        u8SpecificErrorCount += (u8)st_err_ld_supy_iplb_fan_1_lin;                            // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
        
        
        // 14. Ground Loss - solo per applicazioni 48 V - sempre INATTIVA.
        st_err_gnd_lss_fan_1_lin = InactivError;
        u8SpecificErrorCount += (u8)st_err_gnd_lss_fan_1_lin;                                 // La variabile locale u8SpecificErrorCount tiene conto dell'ammontare degli errori rilevati in questa routine.
        
        
        // 6. Non specific error - speed not reached
        if ( u8SpecificErrorCount == 0 )                                                      // - Se non e' stato notificato/individuato ancora nessuno degli errori precedenti...
        {
          BOOL_ErrorTimeDelayCounterONForNonSpecificError = TRUE;                             // ...si setta il flag relativo a "errore generico/velocita' target non raggiunta"...
        }
        else
        {
          BOOL_ErrorTimeDelayCounterONForNonSpecificError = FALSE;                            // ...altrimenti la causa specifica di spegnimento del drive e' gia' stata segnalata.
        }
        
        //  N.B.: il timeout per l'errore generico e' raddoppiato in modalita' Soft-start, nella quale
        // il drive impiega tempi piu' lunghi per raggiungere la velocita' richiesta.
        if ( u16TicsInto5secForNonSpecificError >= (u16)(NON_SPECIFIC_ERROR_NOTIFICATION_DELAY*((u8)LIN_CheckSoftStart()+1)) )
        {
          st_err_fan_1_lin = ActivError;                                                      // Allo scadere del relativo timer, si setta la notifica di errore generico, che ha provocato un mancato raggiungimento del setpoint target.
        }
        else
        {
          st_err_fan_1_lin = InactivError;                                                    // Non si notifica nulla, fino allo scadere del suddetto timer.
        }                            
      }
      else
      {
        (void)LIN_ResetErrors();                                                              // Se non c'e' stato alcuno spegnimento (e il drive sta operando normalmente), si resettano tutte le notifiche di errore.
      }
    }
    else
    {
      (void)LIN_ResetErrors();                                                                // Se al drive non e' richiesto di girare, si resettano tutte le notifiche di errore (utili con sistema in RUN).
    }
  }
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_UpdateDriveActualWarnings(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Update the drive warning status to be sent via LIN.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_UpdateDriveActualWarnings(void)
{ 
  if ( (BOOL)LIN_CheckErrorStatus() == FALSE )                                                // Se non si sono verificati errori (e il drive sta girando)...
  {
    if ( tar_opmo_fan_1_lin != InvalidTargetMode )                                            // ...e la modalita' operativa (dalla ECU) e' valida...
    {
      // N.B.: nota per l'if ( BOOLDriveShutdown == TRUE ) nel seguito.
      // Non si usa direttamente u8_tarvl_v_fan_1_lin poiche';
      //  1. occorrerebbe un AND aggiuntivo con la condizione
      //     u8_tarvl_v_fan_1_lin < 255
      //  2. in questo modo si intercettano gli errori anche in modalita'
      //     AirDensityCompensation, la quale puo' essere settata con
      //     u8_tarvl_v_fan_1_lin = 0
      
      if ( (u8)InputSP_GetInputSetPoint() > 0 )                                               // ...e il setpoint di velocita' e' non nullo, allora:
      {                                               
        // 10. Temperature error - derating
        if ( (StateSOATemp_T)Temp_GetTemperatureState() == DERATING_TEMP )                    // - Nel caso di derating termico...
        {
          st_err_temp_fan_1_lin = ActivError;                                                 // ...si setta il relativo warning.
        }
        else
        {
          st_err_temp_fan_1_lin = InactivError;                                               // Se la temperatura rilevata e' nei range operativi nominali, non si notifica nulla.
        }
        
        // 11. Overcurrent degradation error (i.e. demagnetization)
        if ( BOOL_Current_Limitation_Active == TRUE )                                         // - Nel caso di derating per sovracorrente...
        {
          st_err_i_fan_1_lin = ActivError;                                                    // ...si setta il relativo warning.
        }
        else
        {
          st_err_i_fan_1_lin = InactivError;                                                  // Se la corrente (di picco) e' nei range operativi nominali, non si notifica nulla.      
        }
              
        // 12. Fan overload (sluggishness fan)        
        if ( u8_avl_rpm_fan_1_lin >= LIN_MIN_SPEED )                                          // - Per velocita' rilevate almeno pari alla minima...
        {
          (void)Curr_GetPeakCurrent();  // Per aggiornare strMachine.u16CurrentPU
          (void)LIN_ComputeSluggishThresholds();                                              // ...si aggiornano le soglie di slugghishness.
        
          if ( strMachine.u16CurrentPU >= u16SluggishLowThresh )                              // Se la corrente (di picco) e' >= alla soglia minima...
          {
            if ( strMachine.u16CurrentPU > u16SluggishHighThresh )                            // ...e superiore alla massima (i.e. la corrente e' fuori l'isteresi di slugghishness)...
            {
              BOOL_ErrorTimeDelayCounterONForSluggish = TRUE;                                 // ...allora si setta il flag di sovraccarico meccanico.
              
              if ( u16TicsDelaysecForSluggish >= SLUGGISH_NOTIFICATION_DELAY_BIT )            // Se il delay-timer per il sovraccarico meccanico e' scaduto...
              {
                st_err_slu_fan_1_lin = ActivError;                                            // ...allora si notifica il relativo warning...
              }
              else
              {
                st_err_slu_fan_1_lin = InactivError;                                          // ...altrimente si attende lo scadere del timer.
              }
            }
            else
            {
              st_err_slu_fan_1_lin = InactivError;                                            
              BOOL_ErrorTimeDelayCounterONForSluggish = FALSE;                                // Se la corrente e' al piu' nell'isteresi di slugghishness, non si notifica nulla.                  
            }
          }
          else
          {
            st_err_slu_fan_1_lin = InactivError;
            BOOL_ErrorTimeDelayCounterONForSluggish = FALSE;                                  // Se la corrente e' inferiore alle soglie di slugghishness, non si notifica nulla.                  
          }
        }
        else
        {
          st_err_slu_fan_1_lin = InactivError;
          BOOL_ErrorTimeDelayCounterONForSluggish = FALSE;                                    // Se la velocita' rilevata e' sotto la minima, non si notifica nulla.
        }        
      }
      else
      {
        (void)LIN_ResetWarnings();                                                            // Se al drive non e' richiesto di girare, si resettano tutti i warning.
      }
    }
    else
    {
      (void)LIN_ResetWarnings();                                                              // Se il target operativo e' invalido, si resettano tutti i warning.
    }
  }
  else
  {
    (void)LIN_ResetWarnings();                                                                // Se si e' verificato un errore (i.e. il drive e' stato arrestato e non si e' in RUN), si resettano tutti i warning.
  }
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_ResetErrors(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Reset all drive errors.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_ResetErrors(void)
{
  // Reset delle variabili d'errore.
  st_err_bri_fan_1_lin          = InactivError;        //  1. Frame ST_FAN_1_LIN: drive actual bridge/TLE error.
  st_err_bri_sc_fan_1_lin       = InactivError;        //  2. Frame ST_FAN_1_LIN: drive actual short-circuit error.
  st_err_temp_swo_fan_1_lin     = InactivError;        //  3. Frame ST_FAN_1_LIN: drive actual temperature error.
  st_err_i_swo_fan_1_lin        = InactivError;        //  4. Frame ST_FAN_1_LIN: drive actual current (switch-off) error.
  st_err_u_iplb_fan_1_lin       = InactivError;        //  5. Frame ST_FAN_1_LIN: drive actual implausible voltage error.
  st_err_fan_1_lin              = InactivError;        //  6. Frame ST_FAN_1_LIN: drive actual non specific error (target speed is not reached).
  st_err_i_offs_fan_1_lin       = InactivError;        //  8. Frame ST_FAN_1_LIN: drive actual current offset error.
  st_err_blk_fan_1_lin          = InactivError;        //  9. Frame ST_FAN_1_LIN: drive actual blocked fan error.
  st_err_ld_supy_iplb_fan_1_lin = InactivError;        // 13. Frame ST_FAN_1_LIN: drive actual implausible load supply (error).
  st_err_gnd_lss_fan_1_lin      = InactivError;        // 14. Frame ST_FAN_1_LIN: ground loss - only for 48 V applications.
                                                                 
  // Reset dei timer per la notifica degli errori.               
  u16TicsInto20secForGD_ERROR        = 0;
  u16TicsInto20secForI_SWO           = 0;
  u16TicsInto20secForU_IPLB          = 0;
  u16TicsInto20secForI_OFFS          = 0;
  u16TicsInto20secForLD_SUPY_IPLB    = 0;
  u16TicsInto5secForNonSpecificError = 0;
                                         
  // Reset dei flag d'errore.            
  BOOL_ErrorTimeDelayCounterONForGD_ERROR         = FALSE;
  BOOL_ErrorTimeDelayCounterONForI_SWO            = FALSE;
  BOOL_ErrorTimeDelayCounterONForNonSpecificError = FALSE;
  BOOL_ErrorTimeDelayCounterONForLD_SUPY_IPLB     = FALSE;

  // Reset dei flag ausiliari per la gestione degli errori.
  BOOLDriveShutdown    = FALSE;
  SWO_Overcurrent_Flag = FALSE;  
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void LIN_ResetWarnings(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Reset all drive warnings.
    \return \b None.
    \note None.
    \warning None
*/
void LIN_ResetWarnings (void)
{
  // Reset delle variabili di warning.
  st_err_temp_fan_1_lin = InactivError;     // 10. Frame ST_FAN_1_LIN: drive actual temperature warning.
  st_err_i_fan_1_lin    = InactivError;     // 11. Frame ST_FAN_1_LIN: drive actual overcurrent degradation warning - i.e. demagnetization.
  st_err_slu_fan_1_lin  = InactivError;     // 12. Frame ST_FAN_1_LIN: drive actual sluggishness warning.
  
  // Reset dei flag ausiliari per la gestione dei warnings.
  BOOL_Current_Limitation_Active = FALSE;  
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
  if ( BOOLRTI_LinTask == TRUE )                  // Se il flag della base-tempi LIN e' attivo (i.e. e' stato scandito un ciclo della base tempi)...
  {
    BOOLRTI_LinTask = FALSE;                      // ...esso si resetta...
    LinFastTimerTask();                           
    LinSlowTimerTask();                           // ...si eseguono le routine Fast e Slow-Task contenute nello stack LIN (fondamentali per implementare la comunicazione)...
    LIN_PerformSelfSustainingTimebase();          // ...si aggiorna la base-tempi per l'after-run...
    LIN_PerformDelayTimebases();                  // ...e anche i timer per la notifica di errori e warning.
    LIN_UpdateControlFrame();                     // Si acquisiscono setpoint di velocita'/timeout di after-run/modo operativo dal control frame...
    LIN_UpdateStatusFrame();                      // ...e si aggiorna lo status frame da mandare alla centralina.
  }
  else
  {
    // Nop();
  }
  
  LIN_UpdateDriveActualErrors();                  // L'aggiornamento degli errori da segnalare e' fatto qui. Cio' per intercettare la variabile interna u16ErrorType (dal main),
                                                  // cosi' da realizzare la mappatura: errori SPAL->errori LIN (BMW).
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 LIN_CheckSoftStart(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check if the soft-start operation mode is required by master. 
    \return \b u8 value.
    \note None.
    \warning None.
*/ 
u8 LIN_CheckSoftStart (void)
{
  if ( tar_opmo_fan_1_lin == SoftStartTargetMode )
  {
    return((u8)1);    // Torna TRUE se la modalita' soft-start e' attiva (per cui i tempi di accelerazione del drive sono piu' lungi)...
  }
  else
  {
    return((u8)0);    // ...altrimenti FALSE.
  }                   // N.B.: si ritorna un u8 e non un BOOL, poiche' il risultato di questa funzione serve, piu' che come flag, come offset numerico!!
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL LIN_CheckErrorStatus(void)
    \author	Ianni Fabrizio  \n <em>Main developer</em> 

    \date 09/02/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check if any drive error is on.     
    \return \b Boolean value.
    \note None.
    \warning None
*/

BOOL LIN_CheckErrorStatus(void)
{
  if ( (st_err_bri_fan_1_lin          == ActivError)      //  1. Frame ST_FAN_1_LIN: drive actual bridge/TLE error.
     ||(st_err_bri_sc_fan_1_lin       == ActivError)      //  2. Frame ST_FAN_1_LIN: drive actual short-circuit error.
     ||(st_err_temp_swo_fan_1_lin     == ActivError)      //  3. Frame ST_FAN_1_LIN: drive actual temperature error.
     ||(st_err_i_swo_fan_1_lin        == ActivError)      //  4. Frame ST_FAN_1_LIN: drive actual current (switch-off) error.
     ||(st_err_u_iplb_fan_1_lin       == ActivError)      //  5. Frame ST_FAN_1_LIN: drive actual implausible voltage error.
     ||(st_err_fan_1_lin              == ActivError)      //  6. Frame ST_FAN_1_LIN: drive actual non specific error (target speed is not reached).
     ||(st_err_i_offs_fan_1_lin       == ActivError)      //  8. Frame ST_FAN_1_LIN: drive actual current offset error.
     ||(st_err_blk_fan_1_lin          == ActivError)      //  9. Frame ST_FAN_1_LIN: drive actual blocked fan error.
     ||(st_err_ld_supy_iplb_fan_1_lin == ActivError)      // 13. Frame ST_FAN_1_LIN: drive actual implausible load supply (error).
     ||(st_err_gnd_lss_fan_1_lin      == ActivError) )    // 14. Frame ST_FAN_1_LIN: ground loss - only for 48 V applications.
  {
    return (TRUE);
  }
  else
  {
    return (FALSE);
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
      if (avl_opmo_fan_1_lin != SelfSustainingActualMode )            // Se non si e' in modalita' operativa Self-sustaining...
      {
        if ( u16NumLinBusIdle >= MAX_NUM_BUS_IDLE_BIT )               // ...e il bus LIN e' in idle ormai da un certo periodo di tempo (MAX_NUM_BUS_IDLE_MSEC)...
        {
          return (TRUE);
        }                                                             // ...allora il drive viene messo in sleep...
        else
        {
          return (FALSE);                                             // ...altrimenti c'e' comunicazione e si e' operativi.
        }
      }
      else
      {
        return (FALSE);                                               // In after-run occorre che il drive funzioni fino al timeout, anche col bus in idle.
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


//_________ LIN ProtocolError ______________
void ApplLinProtocolError(vuint8 error, tLinFrameHandleType vMsgHandle )
{
  // Nop();
}


//_________ LIN Read By Identifier 0x01 ______________
vuint8 ApplLinScGetSerialNumber(vuint8* pbSerialNumber)
{  
  pbSerialNumber [0] = SerialNumber[0];   // Least Significant Byte of Serial Number.
  pbSerialNumber [1] = SerialNumber[1];   // Serial Number: byte 2.
  pbSerialNumber [2] = SerialNumber[2];   // Serial Number: byte 3.
  pbSerialNumber [3] = SerialNumber[3];   // Most Significant Byte of Serial Number.  
                                          // Unused bytes are always default set to 0xFF.
  
  return( kLinSc_SendSerialNr);
}


//_________ LIN Read By Identifier User Defined 32-63______________
vuint8 ApplLinScGetUserDefinedId(vuint8 IdNumber, vuint8* pbIdValue)
{
  vuint8 bRet; //byte returned

  switch (IdNumber)
  {
    //do the acceess of the EPROM during initialization and store it in a temporary viable to speed up this access
    case  32:    //Reserved ID for SPAL reference number
      bRet = 5;    //lenght of the payload which is provided  
      pbIdValue [0] = SW_VERSION;                           // Software version - See SPAL SW rule identification.
      pbIdValue [1] = PROJECT;                              // Project  version - See SPAL SW rule identification.
      pbIdValue [2] = CUSTOMER;                             // Customer version - See SPAL SW rule identification.
      pbIdValue [3] = (u8)(u16SPALHwReferenceNumber);       // HW SPAL ref. LSB.
      pbIdValue [4] = (u8)(u16SPALHwReferenceNumber>>8);    // HW SPAL ref. MSB.
    break ;
    
    case  40:  
      bRet = 3;    //lenght of the payload which is provided  
      pbIdValue [0] = u8YearProduction;     // Production date year.
      pbIdValue [1] = u8MonthProduction;    // Production date month.
      pbIdValue [2] = u8DayProduction;      // Production date day.
      pbIdValue [3] = 0xFF;                 // Unused.
      pbIdValue [4] = 0xFF;                 // Unused.
    break ;

    case  41:  
      bRet = 5;                             // Lenght of the payload which is provided  
      pbIdValue [0] = DESIGN_VOLTAGE;       // Part number byte 1 (LSB).
      pbIdValue [1] = DESING_POWER;         // Part number byte 2.
      pbIdValue [2] = MANUFACTURER;         // Part number byte 3.
      pbIdValue [3] = FABRICATION_SITE;     // Part number byte 4.
      pbIdValue [4] = PRODUCT_FAMILY;       // Part number byte 5 (MSB).
    break ;
    
    case  42:  
      bRet = 2;     //lenght of the payload which is provided  
      pbIdValue [0] = BMW_SW_VERSION_LSB;    // SW ref. LSB.
      pbIdValue [1] = BMW_SW_VERSION_MSB;    // SW ref. MSB.
      pbIdValue [2] = 0xFF;                  // Unused.
      pbIdValue [3] = 0xFF;                  // Unused.
      pbIdValue [4] = 0xFF;                  // Unused.
    break ;
    
    case  43:  
      bRet = 2;      //lenght of the payload which is provided  
      pbIdValue [0] = (u8)(u16HwReferenceNumber);       // SW ref. LSB.
      pbIdValue [1] = (u8)(u16HwReferenceNumber>>8);    // SW ref. MSB.
      pbIdValue [2] = 0xFF;                             // Unused.
      pbIdValue [3] = 0xFF;                             // Unused.
      pbIdValue [4] = 0xFF;                             // Unused.
    break ;
        
    #ifdef LIN_BAL_MODE_PASSWORD  
      case BAL_MODE_PASSW_CASE:
        if(BalanceModeTask(IdNumber))
          {
            LinSetBalanceMode();
            bRet = 1;                              // Lenght of the payload which is provided.  
            pbIdValue [0] = BAL_MODE_PASSW_ACK;    // Acknowledge.
            pbIdValue [1] = 0xFF;                  // Unused.
            pbIdValue [2] = 0xFF;                  // Unused.
            pbIdValue [3] = 0xFF;                  // Unused.
            pbIdValue [4] = 0xFF;                  // Unused.
          }
        else
          bRet =  kLinSc_SendNegativeResponse;  
      break;
    #endif  
        
    default:
    bRet =  kLinSc_SendNegativeResponse;
  }
    
  return (bRet) ;
}


//_________ LIN ScSaveConfiguration ______________
void ApplLinScSaveConfiguration(void)
{
  //  Nop();
}


//_________ LIN BusSleep ______________
void ApplLinFatalError(vuint8 error)
{
  error = 0;    // Dummy instruction (to avoid warnings).
  while(1);     // Attenzione: il Watchdog deve essere abilitato!!
}

#endif  // BMW_LIN_ENCODING
/*** (c) 2018 SPAL Automotive ****************** END OF FILE **************/
