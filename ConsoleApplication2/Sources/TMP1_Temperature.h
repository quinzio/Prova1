/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  TMP1_Temperature.h

VERSION  :  1.3

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file TMP1_Temperature.h
    \version see \ref FMW_VERSION 
    \brief Basic temperature functions prototypes.
    \details Routines for temperature operation and variables related initialization.\n
    \note none
    \warning none
*/

#ifndef TEMPERATURE_H
#define TEMPERATURE_H


/* Public Constants ---------------------------------------------------------------------------------------------- */
/* Ambient temperature costants */ 
#define TAMB_TLE                               (s16)25   /* Temperature where TLE sensor tuning is done. It means 25°C */
#define TAMB_MOS                               (s16)25   /* Temperature where MOS sensor tuning is done. It means 25°C */
#define T_MOS_TOL                              (u16)5    /* Means: 6 °C deltaT                                         */

/* Rs coefficient vs temperature */
#define Rs_VS_TEMP_CU_COEFFICIENT              (float32_t)0.004255 /* [1/°C] */
#define Rs_VS_TEMP_CU_OFFSET_PU                (u16)((float32_t)TEMPERATURE_RES_FOR_PU_BIT/(float32_t)((float32_t)Rs_VS_TEMP_CU_COEFFICIENT*(float32_t)TLE_BASE_TEMPERATURE_DEG))

/* Derating settings */
#define RPM_DERATING_SHUTDOWN                   2400  /* [rpm] */
#define EL_FREQ_DERATING_SHUTDOWN               (float32_t)((float32_t)((float32_t)RPM_DERATING_SHUTDOWN*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
#define EL_FREQ_DERATING_SHUTDOWN_PU            (float32_t)((float32_t)EL_FREQ_DERATING_SHUTDOWN/BASE_FREQUENCY_HZ)
#define EL_FREQ_DERATING_SHUTDOWN_PU_RES_BIT    (u16)((float32_t)((float32_t)EL_FREQ_DERATING_SHUTDOWN*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)

#define T_DRIVER_HIGH_DERATING_THRESHOLD        (s16)150  /* [°C] */
#define T_DRIVER_SHUTDOWN_THRESHOLD             (s16)158  /* [°C] */
#define T_DRIVER_DERATING_RESUME_THRESHOLD      (s16)140  /* [°C] */
#define T_DRIVER_HIGH_DERATING_THR_PU           (s16)((float32_t)((float32_t)T_DRIVER_HIGH_DERATING_THRESHOLD*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)
#define T_DRIVER_SHUTDOWN_THRESHOLD_PU          (s16)((float32_t)((float32_t)T_DRIVER_SHUTDOWN_THRESHOLD*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)
#define T_DRIVER_DERATING_RESUME_THR_PU         (s16)((float32_t)((float32_t)T_DRIVER_DERATING_RESUME_THRESHOLD*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)
#define T_MOSFET_HIGH_DERATING_THRESHOLD        (s16)154  /* [°C] */
#define T_MOSFET_SHUTDOWN_THRESHOLD             (s16)163  /* [°C] */
#define T_MOSFET_DERATING_RESUME_THRESHOLD      (s16)140  /* [°C] */
#define T_MOSFET_HIGH_DERATING_THR_PU           (s16)((float32_t)((float32_t)T_MOSFET_HIGH_DERATING_THRESHOLD*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)MOS_BASE_TEMPERATURE_DEG)
#define T_MOSFET_SHUTDOWN_THRESHOLD_PU          (s16)((float32_t)((float32_t)T_MOSFET_SHUTDOWN_THRESHOLD*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)MOS_BASE_TEMPERATURE_DEG)
#define T_MOSFET_DERATING_RESUME_THR_PU         (s16)((float32_t)((float32_t)T_MOSFET_DERATING_RESUME_THRESHOLD*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)MOS_BASE_TEMPERATURE_DEG)
#define T_DERATING_HYSTERESIS_TLE               (s16)0    /* [°C] */
#define T_DERATING_HYSTERESIS_TLE_PU            (s16)((float32_t)((float32_t)T_DERATING_HYSTERESIS_TLE*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)
#define T_DERATING_HYSTERESIS_MOS               (s16)0    /* [°C] */
#define T_DERATING_HYSTERESIS_MOS_PU            (s16)((float32_t)((float32_t)T_DERATING_HYSTERESIS_MOS*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)MOS_BASE_TEMPERATURE_DEG)
#define SOA_TEMP_INT_STEP_TIME_SEC              1         /* [s] */
#define SOA_TEMP_INT_STEP_TIME_BIT              (u16)((u32)(SOA_TEMP_INT_STEP_TIME_SEC*1000)/(u8)TIMEBASE_MSEC)

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  #define TAMB_ERR_COUNT_MAX                    (u8)3
#endif  /* DIAGNOSTIC_STATE */

/* Public type definition -------------------------------------------------- */
/** 
    \typedef StateSOATemp_T
    \brief Temperature derating internal state. 
    \details This enumerator describes internal temperature derating state.
*/
typedef enum  {
  NORMAL_TEMP,     /* !< Temperature in normal limits checked in \ref Temp_CheckTempAmbAtRun           */
  DERATING_TEMP,   /* !< Temperature high, so compute derating; checked in \ref Temp_CheckTempAmbAtRun */
  OVER_MAX_TEMP    /* !< Temperature over the maximum checked in \ref Temp_CheckTempAmbAtRun           */
} StateSOATemp_T;

typedef enum  {
  DISABLED,
  TLE,
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  MOSFET
  #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
} DeratingMode;

typedef struct
{
  s16 s16TempAmbReadPU;
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  s16 s16TempMOSReadPU;
  #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
} strTemperature;

/* Public Functions prototypes --------------------------------------------- */
BOOL  Temp_CheckTempAmbPreRun(void);
void  Temp_InitSoaTemperature(void);
void  Temp_InitTemp(void);
BOOL  Temp_CheckTempAmbAtRun(void);
BOOL  Temp_AutoTestTLETempAmb(void);
#if (defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE) || defined (ENABLE_RS232_DEBUG) || defined(BMW_LIN_ENCODING) || defined(LOG_DATI))
s16   Temp_GetTamb(void);
#endif
u16   Temp_GetTparam(void);
void  Temp_SetDefaultParamT(void);
#ifdef LIN_INPUT
  #ifdef GM_LIN_ENCODING
    BOOL Temp_GetOvertemperature(void);
  #endif  /* GM_LIN_ENCODING */

  #ifdef BMW_LIN_ENCODING
    StateSOATemp_T Temp_GetTemperatureState(void);
  #endif  /* BMW_LIN_ENCODING */
#endif  /* LIN_INPUT */
s32 Temp_GetTempCorr (void);
void Temp_UpdateValues(void);
#ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  BOOL Temp_AutoTestMosTemp(void);
#if (defined(ENABLE_RS232_DEBUG) || defined(LOG_DATI))
  s16  Temp_GetTMos(void);
#endif
  u16  Temp_GetTMosParam(void);
  void Temp_SetDefaultParamTMOS(void);
#endif /* MOSFET_OVERTEMPERATURE_PROTECTION */

BOOL Temp_Get_BOOLSOATemperatureCorr(void);
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  u8 Temp_Get_u8TambErrCount(void);
#endif  /* DIAGNOSTIC_STATE */

u16  Temp_Get_u16TempStepInt(void);
void Temp_Dec_u16TempStepInt(void);

#endif  /* TEMPERATURE_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
