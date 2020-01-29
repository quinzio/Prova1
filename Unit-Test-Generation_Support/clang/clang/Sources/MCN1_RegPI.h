/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MCN1_RegPI.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MCN1_RegPI.c
    \version see \ref FMW_VERSION
    \brief Motion Control Routines prototypes
    \details In this files all the intermidiate motion control routines are regrouped.
    In line of principle all functions that are contained in this files have to drive Level 0 routines
    and will use and provides variables flor routines in \ref MCN1_acmotor.c 
*/

#ifndef RegPI_H
#define RegPI_H


/* Public Constants -------------------------------------------------------- */

/* Regolatori PI: dimensionamento */
/* Regolatore start */
#define START_REG_COMPUTATION_RES_BIT      (u32)((u32)1<<START_REG_COMPUTATION_RES_SHIFT)
#define CURRENT_REGULATION_REF_PU          (u16)((u32)((ALIGNMENT_CURRENT_AMP_REF*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)BASE_CURRENT_AMP))
#define START_REG_BANDWIDTH_PU             (float32_t)((float32_t)START_REG_BANDWIDTH_HZ/(float32_t)BASE_FREQUENCY_HZ)  
#define INT_REG_GAIN_START_PU              (u16)((float32_t)((float32_t)SERIES_RESISTANCE_PU*(float32_t)START_REG_BANDWIDTH_PU*(float32_t)START_REG_COMPUTATION_RES_BIT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/(float32_t)CURRENT_RES_FOR_PU_BIT)
#define PROP_REG_GAIN_START_PU             (u16)((float32_t)((float32_t)INT_REG_GAIN_START_PU*(float32_t)SERIES_INDUCTANCE_PU)/(float32_t)SERIES_RESISTANCE_PU)

#define MIN_PEAK_CURR_PU                   (u16)((float32_t)(MIN_PEAK_CURR*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)BASE_CURRENT_AMP)    
#define START_REG_COMPUTATION_RES_BIT_OFF  (u32)((u32)1<<START_REG_COMP_RES_SHIFT_OFF)
#define START_REG_BANDWIDTH_PU_OFF         (float32_t)((float32_t)START_REG_BANDWIDTH_HZ_OFF/(float32_t)BASE_FREQUENCY_HZ)  
#define INT_REG_GAIN_START_PU_OFF          (u16)((float32_t)((float32_t)SERIES_RESISTANCE_PU*START_REG_BANDWIDTH_PU_OFF*(float32_t)START_REG_COMPUTATION_RES_BIT_OFF*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/(float32_t)CURRENT_RES_FOR_PU_BIT)
#define PROP_REG_GAIN_START_PU_OFF         (u16)((float32_t)((float32_t)INT_REG_GAIN_START_PU_OFF*SERIES_INDUCTANCE_PU)/(float32_t)SERIES_RESISTANCE_PU)

/* Regolatore VoF */
#define VoF_REG_COMPUTATION_RES_BIT       (u32)((u32)1<<VoF_REG_COMPUTATION_RES_SHIFT)
#define VoF_REG_BANDWIDTH_PU              (float32_t)((float32_t)VoF_REG_BANDWIDTH_HZ/(float32_t)BASE_FREQUENCY_HZ)
#define INT_REG_GAIN_VoF_PU               (u16)((float32_t)((float32_t)SERIES_RESISTANCE_PU*VoF_REG_BANDWIDTH_PU*(float32_t)VoF_REG_COMPUTATION_RES_BIT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/(float32_t)((float32_t)SERIES_INDUCTANCE_PU*(float32_t)ANGLE_RES_FOR_PU_BIT)) /* 0.869 */
#define PROP_REG_GAIN_VoF_PU              (u16)((float32_t)((float32_t)INT_REG_GAIN_VoF_PU*SERIES_INDUCTANCE_PU)/(float32_t)SERIES_RESISTANCE_PU)
#define VoF_REG_LIM_HIGH_PU               (s32)((u32)MAX_PHASE_VOLTAGE_OVERMOD_PU*VoF_REG_COMPUTATION_RES_BIT)
#define VoF_REG_LIM_LOW_PU                (s32)(-(s32)((u32)MAX_PHASE_VOLTAGE_OVERMOD_PU*VoF_REG_COMPUTATION_RES_BIT))

/* Regolatore Wind-mill */
#ifdef WM_MANAGEMENT
  #define WM_REG_COMPUTATION_RES_BIT      (u32)((u32)1<<WM_REG_COMPUTATION_RES_SHIFT)  
  #define WM_PHI_ANGLE_LIM_PU             (s16)((s32)((s32)WM_PHI_ANGLE_LIM_DEG*(s32)ANGLE_RES_FOR_PU_BIT)/(s32)BASE_ANGLE_DEG)
  #define MIN_PEAK_CURR_WM_PU             (u16)((float32_t)(MIN_PEAK_CURR_WM*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)BASE_CURRENT_AMP)      
  #define WM_REG_BANDWIDTH_PU             (float32_t)((float32_t)WM_REG_BANDWIDTH_HZ/(float32_t)BASE_FREQUENCY_HZ)  
  #define INT_REG_GAIN_WM_PU              (u16)((float32_t)((float32_t)SERIES_RESISTANCE_PU*WM_REG_BANDWIDTH_PU*(float32_t)WM_REG_COMPUTATION_RES_BIT*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)((float32_t)SERIES_INDUCTANCE_PU*Ke_CONSTANT_PU*(float32_t)ANGLE_RES_FOR_PU_BIT))
  #define PROP_REG_GAIN_WM_PU             (u16)((float32_t)((float32_t)INT_REG_GAIN_WM_PU*SERIES_INDUCTANCE_PU)/(float32_t)SERIES_RESISTANCE_PU)
  #define WM_REG_LIM_HIGH_RPM             (u16)((u16)RPM_MAX_SPEED - RPM_MIN)  /* [rpm] */
  #define WM_REG_LIM_HIGH_PU              (s32)((float32_t)((float32_t)((float32_t)WM_REG_LIM_HIGH_RPM*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_SPEED_RPM)*(float32_t)WM_REG_COMPUTATION_RES_BIT)  
#endif  /* WM_MANAGEMENT */

/* Regolatore Delta max */
#define DELTA_REG_COMPUTATION_RES_BIT     (u32)((u32)1<<DELTA_REG_COMPUTATION_RES_SHIFT)
#define DELTA_REG_GAIN_PU                 (u16)((float32_t)((float32_t)Ke_CONSTANT_PU*(float32_t)DELTA_REG_COMPUTATION_RES_BIT*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)((float32_t)SERIES_INDUCTANCE_PU*(float32_t)INERTIA_CONSTANT_PU*(float32_t)ANGLE_RES_FOR_PU_BIT))
#define DELTA_REG_LIM_HIGH_RPM            (u16)((u16)RPM_MAX_SPEED - RPM_MIN)  /* [rpm] */
#define DELTA_REG_LIM_HIGH_PU             (s32)((float32_t)((float32_t)((float32_t)DELTA_REG_LIM_HIGH_RPM*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_SPEED_RPM)*(float32_t)DELTA_REG_COMPUTATION_RES_BIT)

/* Regolatore Ipk max */
#define MAX_CURR_REG_COMPUTATION_RES_BIT  (u32)((u32)1<<MAX_CURR_REG_COMPUTATION_RES_SHIFT)    
/*#define LIM_CURRENT_BIT_PU                (u16)((float32_t)((float32_t)LIM_CURRENT_AMPERE*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)BASE_CURRENT_AMP) */
#define MAX_CURR_REG_GAIN_PU              (u16)((float32_t)((float32_t)Ke_CONSTANT_PU*(float32_t)MAX_CURR_REG_COMPUTATION_RES_BIT*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)((float32_t)INERTIA_CONSTANT_PU*(float32_t)CURRENT_RES_FOR_PU_BIT))
#define MAX_CURR_REG_LIM_HIGH_RPM         (u16)((u16)RPM_MAX_SPEED - RPM_MIN)  /* [rpm] */
#define MAX_CURR_REG_LIM_HIGH_PU          (s32)((float32_t)((float32_t)((float32_t)MAX_CURR_REG_LIM_HIGH_RPM*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_SPEED_RPM)*(float32_t)MAX_CURR_REG_COMPUTATION_RES_BIT)

/* Regolatore TOD */
#define TOD_REG_COMPUTATION_RES_BIT     (u32)((u32)1<<TOD_REG_COMPUTATION_RES_SHIFT)  
#define PSI_PER10exp4                   (float32_t)((float32_t)((float32_t)E_REF_1000RPM * (float32_t)60 * (float32_t)10000) / (float32_t)((float32_t)6.283 * (float32_t)POLE_PAIR_NUM * (float32_t)1000))
#define WN_TOD                          (float32_t)((float32_t)((float32_t)POLE_PAIR_NUM * PSI_PER10exp4) / (float32_t)RADQUAD((float32_t)((float32_t)((float32_t)FAN_ROT_INERTIA*(float32_t)10000)*(float32_t)((float32_t)Ls_REF*(float32_t)10000))))
#define WN_TOD_PU                       (float32_t)((float32_t)WN_TOD/BASE_ELECTRICAL_PULSE)
#define TOD_REG_GAIN_MIN_SPEED_PU       (u16)((float32_t)((float32_t)EFFICIENCY_AT_LOWFREQ_LIMIT*(float32_t)TOD_REG_COMPUTATION_RES_BIT)/(float32_t)((float32_t)WN_TOD_PU*(float32_t)INERTIA_CONSTANT_PU*(float32_t)EL_FREQ_MIN_PU))  
#define TOD_REG_GAIN_MAX_SPEED_PU       (u16)((float32_t)((float32_t)EFFICIENCY_AT_LOWFREQ_LIMIT*(float32_t)TOD_REG_COMPUTATION_RES_BIT)/(float32_t)((float32_t)WN_TOD_PU*(float32_t)INERTIA_CONSTANT_PU*(float32_t)EL_FREQ_NOMINAL_PU))  
#define TOD_REG_LIM_HIGH_RPM            300  /* [rpm] (N.B.: non stringere troppo questa soglia, altrimenti l'azione del TOD potrebbe essere erroneamente clampata laddove occorre piu' incisivita', i.e. in aggancio al volo - con grosse oscillazioni torsionali da compensare). */
#define TOD_REG_LIM_HIGH_PU             (s32)((float32_t)((float32_t)((float32_t)TOD_REG_LIM_HIGH_RPM*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_SPEED_RPM)*(float32_t)TOD_REG_COMPUTATION_RES_BIT)
#define TOD_REG_LIM_LOW_RPM             300  /* [rpm] */
#define TOD_REG_LIM_LOW_PU              (s32)(-(s32)((float32_t)((float32_t)((float32_t)TOD_REG_LIM_LOW_RPM*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_SPEED_RPM)*(float32_t)TOD_REG_COMPUTATION_RES_BIT))

/* Regolatore Pmax */
#define PMAX_REG_COMPUTATION_RES_BIT    (u32)((u32)1<<PMAX_REG_COMPUTATION_RES_SHIFT)  
#define SP_POWER_PU                     (u16)((float32_t)((float32_t)PMAX_REG_SETPOINT*(float32_t)CURRENT_RES_FOR_PU_BIT)/BASE_BATT_POWER_WATT)
#define PMAX_REG_GAIN_PU                (u16)((float32_t)((float32_t)EFFICIENCY_AT_HIGHFREQ_LIMIT*(float32_t)PMAX_REG_COMPUTATION_RES_BIT)/(float32_t)((float32_t)INERTIA_CONSTANT_PU*(float32_t)EL_FREQ_NOMINAL_PU))
#define PMAX_REG_LIM_LOW_RPM            (u16)((u16)RPM_MAX_SPEED - RPM_MIN)  /* [rpm] */
#define PMAX_REG_LIM_LOW_PU             (s32)(-(s32)((float32_t)((float32_t)((float32_t)PMAX_REG_LIM_LOW_RPM*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_SPEED_RPM)*(float32_t)PMAX_REG_COMPUTATION_RES_BIT))

/* Regolatore di temperatura */
#define TEMP_REG_COMPUTATION_RES_BIT    (u32)((u32)1<<TEMP_REG_COMPUTATION_RES_SHIFT)  
#define THERMAL_RC_CONSTANT_PU          (float32_t)((float32_t)THERMAL_RC_CONSTANT/(float32_t)TEMP_REG_BASE_STEP_TIME)
#define TEMP_REG_BANDWITH_PU            (float32_t)((float32_t)TEMP_REG_BANDWITH/BASE_TEMP_BANDWITH)  
#define TEMP_REG_PROP_GAIN_PU           (u16)((float32_t)THERMAL_RC_CONSTANT_PU*TEMP_REG_BANDWITH_PU*(float32_t)TEMP_REG_COMPUTATION_RES_BIT)
#define TEMP_REG_INT_GAIN_PU            (u16)((float32_t)TEMP_REG_BANDWITH_PU*(float32_t)TEMP_REG_COMPUTATION_RES_BIT)


/* Public type definition -------------------------------------------------- */

/* PI-parameter structure */
typedef struct 
 {
  s32 s32IntLimitHigh;
  s32 s32IntLimitLow;
  s32 s32RegLimitHigh;
  s32 s32RegLimitLow;
  u16 u16IntGain;
  u16 u16PropGain;
  u16 u16IntStep;    
  BOOL BOOLFreeze;
} structPIreg;

/* PI-I/O structure */
typedef struct
{
  s32 s32Integral;
  s32 s32Proportional;
  s16 s16RegInput;
  s16 s16RegFeedback;
  s32 s32RegOutput;
} structRegIO;

/* PI-index structure */
typedef enum {
  START_REG,  
  VoF_REG,
  TOD_REG,
  #ifdef WM_MANAGEMENT
    WM_REG,
  #endif  /* WM_MANAGEMENT */
  MAX_CUR_REG,
  DELTA_MAX_REG,
  PMAX_REG,
  TEMP_REG,
  VECT_PI_LENGTH
} PIenum;

/* Public variables -------------------------------------------------- */
#pragma DATA_SEG _PI_PAGE
extern structPIreg VectParPI[VECT_PI_LENGTH];
extern structRegIO VectVarPI[VECT_PI_LENGTH];

#pragma DATA_SEG DEFAULT
/* Public functions -------------------------------------------------- */
void REG_SetParPI(PIenum enumReg,u16 u16IntGain,u16 u16PropGain,s32 s32RegLimLow,s32 s32RegLimHigh,BOOL BOOLFreeze,u16 u16IntStep);
void REG_InitVarPI(PIenum enumReg,s16 s16RegInput,s16 s16RegFeedback,s32 s32Integral,s32 s32Proportional,s32 s32RegOutput);
void REG_UpdateInputPI (PIenum enumReg,s16 s16RegSp,s16 s16RegSample);
void REG_UpdateRegPI(PIenum enumReg);

#endif  /* RegPI_H */

/*** (c) 2015 SPAL Automotive ****************** END OF FILE **************/
