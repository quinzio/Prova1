/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  FIR1_FIR.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file FIR1_FIR.h
    \version see \ref FMW_VERSION 
    \brief Basic filtering function prototypes with related constants and structures.
    \details none
    \note none
    \warning none
*/

#ifndef FIR_H
#define FIR_H


/* Public Constants ---------------------------------------------------------------------------- */
/* Filter resolution factor */
#define FILTER_RES_SHIFT    5
#define FILTER_RES_FACT     (u16)((u16)1<<FILTER_RES_SHIFT)

/* Filter time costants and limitation factors */
/* Frequency setpoint */
#define SMOOTH_RAMP_PRESCALER_SHIFT     (u8)1 
#define SMOOTH_RAMP_PRESCALER_FACT      (u8)((u8)1<<SMOOTH_RAMP_PRESCALER_SHIFT)
#define SMOOTH_RAMP_SCALER_BIT          (u8)5
#define SMOOTH_RAMP_SCALER_FACT         (u8)((u8)1<<SMOOTH_RAMP_SCALER_BIT)  
#define SMOOTH_RAMP_INT_STEP_FREQ       (u16)((u32)((u32)1000*SMOOTH_RAMP_SCALER_FACT)/(u16)((u16)REG_LOOP_TIME*(u16)8))    /* REG_LOOP_TIME * 8 ms timebase */
#define SMOOTH_RAMP_FILT_TIME_CONST     (float32_t)0.3 /* RC = 0.3 s */
#define SMOOTH_RAMP_FILT_FREQ_CONST     (u16)((float32_t)SMOOTH_RAMP_SCALER_FACT/SMOOTH_RAMP_FILT_TIME_CONST)
#define SMOOTH_RAMP_FILT_HYST_RPM       500 /* [rpm] */
#define SMOOTH_RAMP_FILT_HYST_FREQ      (float32_t)((float32_t)((float32_t)SMOOTH_RAMP_FILT_HYST_RPM*(float32_t)POLE_PAIR_NUM)/(float32_t)60)    
#define SMOOTH_RAMP_FILT_HYST_PU        (s16)((s16)((float32_t)((float32_t)SMOOTH_RAMP_FILT_HYST_FREQ*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)/(s16)SMOOTH_RAMP_PRESCALER_FACT)


/* Delta angle */
#define DELTA_FILT_TIME_CONST           (float32_t)0.4  /* RC = 0.4 s */
#define DELTA_FILT_FREQ_CONST           (u16)((float32_t)FILTER_RES_FACT/DELTA_FILT_TIME_CONST)
#define DELTA_FILT_HYST_ANGLE           20 /* [°el] */
#define DELTA_FILT_HYST_ANGLE_PU        (s16)((float32_t)((float32_t)DELTA_FILT_HYST_ANGLE*(float32_t)ANGLE_RES_FOR_PU_BIT)/(float32_t)BASE_ANGLE_DEG)  


/* Phi angle */
#define PHI_FILT_TIME_CONST             (float32_t)0.4  /* RC = 0.4 s */
#define PHI_FILT_FREQ_CONST             (u16)((float32_t)FILTER_RES_FACT/PHI_FILT_TIME_CONST)
#define PHI_FILT_HYST_ANGLE             20 /* [°el] */
#define PHI_FILT_HYST_ANGLE_PU          (s16)((float32_t)((float32_t)PHI_FILT_HYST_ANGLE*(float32_t)ANGLE_RES_FOR_PU_BIT)/(float32_t)BASE_ANGLE_DEG)

/* TOD */
#define PBATT_FILT_ZERO_TIME_CONST      (u16)14      /* tau_low = 14 s => fbw_low_pbatt = 0.07 Hz */
#define PBATT_FILT_ZERO_FREQ_CONST      (u16)(FILTER_RES_FACT/PBATT_FILT_ZERO_TIME_CONST)
#define PBATT_FILT_POLE_TIME_CONST      (float32_t)0.7     /* tau_high = 0.7 s => fbw_high_pbatt = 1.43 Hz */
#define PBATT_FILT_POLE_FREQ_CONST      (u16)((float32_t)FILTER_RES_FACT/PBATT_FILT_POLE_TIME_CONST)
#define PBATT_FILT_HYST_WATT            (float32_t)((float32_t)6 / UPDATE_LOOP_TIME)    /* [W/s] */
#define PBATT_FILT_HYST_WATT_PU         (s16)((float32_t)((float32_t)PBATT_FILT_HYST_WATT*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)BASE_BATT_POWER_WATT)

/* Temperature */
#define TEMP_ADC_SHIFT                  (u16)4 /* *16 */
#define TEMP_SAMPLING_MSEC              100
#define TEMP_SAMPLING_BIT               (u8)((u16)TEMP_SAMPLING_MSEC / (u16)8)   /* 8 ms basetime */
#define TEMP_RC_TIME_CONST              (float32_t)0.6 /* [s] */
#define TEMP_RC_FREQ_CONST              (u16)((float32_t)6/TEMP_RC_TIME_CONST)
#define TEMP_STEP_FREQ_CONST            (u16)((u32)((u32)1000*(u32)6)/(u32)TEMP_SAMPLING_MSEC)
#define TEMP_FILT_HYST_DEG              20 /* [°C] */
#define TEMP_FILT_HYST_SCALED           (s16)((u16)TEMP_FILT_HYST_DEG<<TEMP_ADC_SHIFT)

/* Over-current */
#if ( ( FAN_TYPE == BLOWER ) || defined(OVERCURRENT_ROF) )  /* Per il RoF "semplice" del blower oppure per la procedura di Overcurrent-RoF. */
  #define IPK_HIGH_FILT_SCALER_BIT           0
  #define IPK_HIGH_FILT_SCALER_FACT          (u8)(1<<IPK_HIGH_FILT_SCALER_BIT)  
  #define IPK_HIGH_STEP_FREQ_CONST           (u16)((u16)UPDATE_LOOP_FREQ*FILTER_RES_FACT)
  
  #define IPK_HIGH_FILT_POLE_TIME_CONST      (float32_t)((float32_t)2/(float32_t)((float32_t)((float32_t)VoF_END_FREQ_RPM*POLE_PAIR_NUM)/60)) /* La banda passante del filtro passa-alto sulla Ipk e' legata alla frequenza di bocche aperte. I transitori per prese improvvise di carico, di solito, operano sopra tale frequenza. */
  #define IPK_HIGH_FILT_POLE_FREQ_CONST      (u16)((float32_t)FILTER_RES_FACT/IPK_HIGH_FILT_POLE_TIME_CONST)
  #define IPK_HIGH_FILT_ZERO_TIME_CONST      (float32_t)((float32_t)IPK_HIGH_FILT_POLE_TIME_CONST*100)   /* Lo zero del filtro passa-alto e' a una frequenza 100 volte piu' piccola di quella del polo. */
  #define IPK_HIGH_FILT_ZERO_FREQ_CONST      (u16)((float32_t)FILTER_RES_FACT/IPK_HIGH_FILT_ZERO_TIME_CONST)
  #define IPK_HIGH_FILT_HYST                 37 /* 37 [A] (Circa il valore di fondoscala dell'A/D). */
  #define IPK_HIGH_FILT_HYST_PU              (u16)((float32_t)((float32_t)IPK_HIGH_FILT_HYST*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP)   
#endif  /* ( FAN_TYPE == BLOWER ) || defined(OVERCURRENT_ROF) */

/* Battery current */
#ifdef DC_LIKE_BEHAVIOUR  
  #define IAVG_FILT_SCALER_BIT          0
  #define IAVG_FILT_SCALER_FACT         (u8)(1<<IAVG_FILT_SCALER_BIT)  
  #define IAVG_STEP_FREQ_CONST          (u16)((float32_t)((float32_t)1000*FILTER_RES_FACT)/TIMEBASE_MSEC)  /* base-tempi 8 ms. */
  #define IAVG_FILT_TIME_CONST          0.1 /* RC = 0.1 s */
  #define IAVG_FILT_FREQ_CONST          (u16)((float32_t)FILTER_RES_FACT/IAVG_FILT_TIME_CONST)
  #define IAVG_FILT_HYST                37 /* 37 [A] (Circa il valore di fondoscala dell'A/D). */
  #define IAVG_FILT_HYST_PU             (u16)((float32_t)((float32_t)IAVG_FILT_HYST*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP)   
#endif  /* DC_LIKE_BEHAVIOUR */


/* Public type definition ---------------------------------------------------------------------------- */

/* Filter-parameter structure */
typedef struct 
 {
  u16 u16SampleFreq;
  s16 s16Hyst;
  u16 u16PoleFreq;
  u16 u16ZeroFreq;
  u8 u8FiltType;
} structParFilt;

/* Filter I/O structure */
typedef struct 
{
  s16 s16OldInput;
  s16 s16NewOutput;
} structVarFilt;

/* Filter-index structure */
typedef enum {
  SIGMA_FILT,
  PBATT_FILT,
  DELTA_FILT,
  PHI_FILT,  
  TEMP_TLE_FILT,
  TEMP_MOS_FILT,
  #ifdef DC_LIKE_BEHAVIOUR
    IAVG_FILT,
  #endif  /* DC_LIKE_BEHAVIOUR */
  #ifdef OVERCURRENT_ROF  
    IPK_HIGH_FILT,
  #endif  /* OVERCURRENT_ROF */
  #if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
    VBATT_FILT,  /* Filtro sulla Vmon. */
  #endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */
  #ifdef LIN_INPUT
    LIN_SPEED_FILT,
  #endif  /* LIN_INPUT */
  VECT_FILT_LENGHT
} FiltEnum;

/*  Public Variables ---------------------------------------------------------------------------- */
#pragma DATA_SEG _FILT_PAGE
/*extern structParFilt VectParFilt[VECT_FILT_LENGHT];*/
extern structVarFilt VectVarFilt[VECT_FILT_LENGHT];

#pragma DATA_SEG DEFAULT

/* Public Functions ---------------------------------------------------------------------------- */
void FIR_InitVarFilt (FiltEnum enumFilt,s16 s16OldInput,s16 s16NewOutput);
void FIR_SetParFilt (FiltEnum enumFilt,u16 u16SampleFreq,u16 u16PoleFreq,u16 u16ZeroFreq,s16 s16Hyst,u8 u8FiltType);
void FIR_UpdateFilt (FiltEnum enumFilt,s16 s16NewInput);

#endif  /* FIR_H */

/*** (c) 2016 SPAL Automotive ****************** END OF FILE **************/
