/************************* (c) 2019  SPAL Automotive *************************
PROJECT  : JLR - MLA

MODULE   :  MCN1_acmotor.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifndef ACM_H
#define ACM_H  

/* -------------------------- Private constants ---------------------------- */
/* ACM timeouts */
#define UNDER_MIN_CURR_ERR_CHECK_NUMBER (u8)10       /* Per la gestione del Wind-mill, nella funzione ACM_CheckWindmillRecovery. */

/* Delta-limiter loop */
#define DELTA_LIMIT_NUM                 (u32)2
#define DELTA_LIMIT_DEN                 (u32)3

#define TAN_PU_RES_1ST_SEG              (u32)5035    /* tan(44°el) = 0.97  */
#define TAN_PU_RES_2ND_SEG              (u32)11711   /* tan(66°el) = 2.25  */
#define TAN_PU_RES_3RD_SEG              (u32)42467   /* tan(83°el) = 8.14  */
#define TAN_PU_RES_4TH_SEG              (u32)298725  /* tan(89°el) = 57.29 */

/* Spezzata per approssimazione arcotangente */
#define ARCTAN_PU_1ST_SEG_COEFF         (u16)6733    /* 6733/8192 = 0.8218 */
#define ARCTAN_PU_1ST_SEG_SHIFT         (u8)13
#define ARCTAN_PU_1ST_SEG_OFFSET        (u16)166

#define ARCTAN_PU_2ND_SEG_COEFF         (u16)2567    /* 2567/8192 = 0.3133 */
#define ARCTAN_PU_2ND_SEG_SHIFT         (u8)13
#define ARCTAN_PU_2ND_SEG_OFFSET        (u16)2582

#define ARCTAN_PU_3RD_SEG_COEFF         (u16)474     /*  474/8192 = 0.0578 */
#define ARCTAN_PU_3RD_SEG_SHIFT         (u8)13
#define ARCTAN_PU_3RD_SEG_OFFSET        (u16)5562

#define ARCTAN_PU_4TH_SEG_COEFF         (u16)16      /*   16/8192 = 0.0019 */
#define ARCTAN_PU_4TH_SEG_SHIFT         (u8)13
#define ARCTAN_PU_4TH_SEG_OFFSET        (u16)7622

#define ARCTAN_PU_5TH_SEG_VALUE         (u16)8101

#define TEMP_REF_FOR_RS_COMPUTATION     (s16)25   /* [°C] */
#define TEMP_REF_FOR_RS_COMPUTATION_PU   (s16)((float32_t)((float32_t)TEMP_REF_FOR_RS_COMPUTATION*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)

/* Spezzata per approssimazione arcoseno */
#define SIN_LIMIT_VALUE_1ST_SEG         (u32)22762
#define ARCSIN_1ST_SEG_COEFF            (s32)45
#define ARCSIN_1ST_SEG_OFFSET           (s32)79

#define SIN_LIMIT_VALUE_2ND_SEG         (u32)29451
#define ARCSIN_2ND_SEG_COEFF            (s32)70
#define ARCSIN_2ND_SEG_OFFSET           (s32)2307

#define SIN_LIMIT_VALUE_3RD_SEG         (u32)32269
#define ARCSIN_3RD_SEG_COEFF            (s32)134
#define ARCSIN_3RD_SEG_OFFSET           (s32)9709

#define SIN_LIMIT_VALUE_4TH_SEG         (u32)32642
#define ARCSIN_4TH_SEG_COEFF            (s32)332
#define ARCSIN_4TH_SEG_OFFSET           (s32)34615

#define SIN_LIMIT_VALUE_5TH_SEG         (u32)32747
#define ARCSIN_5TH_SEG_COEFF            (s32)771
#define ARCSIN_5TH_SEG_OFFSET           (s32)90674

#define SIN_LIMIT_VALUE_6TH_SEG         (u32)32767
#define ARCSIN_6TH_SEG_COEFF            (s32)4669
#define ARCSIN_6TH_SEG_OFFSET           (s32)589422

#define ARCSIN_SEGMENT_DIVIDER          (s32)256

/* Limitazione funzione sin(Gamma) */
#define SIN_GAMMA_MAX_VALUE_BIT         (u32)32767

#define BASE_LS_VOLTAGE_COEFF           (float32_t)((float32_t)BASE_ELECTRICAL_PULSE*BASE_SERIES_INDUCTANCE*BASE_CURRENT_AMP/BASE_VOLTAGE_VOLT)

#define MAXIMUM_ABSOLUTE_ANGLE          90 /* [°el] */
#define MAXIMUM_ABSOLUTE_ANGLE_PU       (float32_t)((float32_t)MAXIMUM_ABSOLUTE_ANGLE / (float32_t)BASE_ANGLE_DEG)
#define MAXIMUM_ABSOLUTE_ANGLE_PU_RES   (s16)((float32_t)MAXIMUM_ABSOLUTE_ANGLE_PU * (float32_t)ANGLE_RES_FOR_PU_BIT)

/* Setpoint per Max Ipk */
#define MAX_IPK_LIMIT_PU_LOW            (u16)((u32)((float32_t)SOA_LIMIT_CURRENT_PU_LOW*(float32_t)0.9))
#define MAX_IPK_LIMIT_PU_HIGH           (u16)((u32)((float32_t)SOA_LIMIT_CURRENT_PU_HIGH*(float32_t)0.9))
#define MAX_IPK_LIMIT_SHIFT             (u16)8
#define MAX_IPK_LIMIT_SCALER            (u16)((u16)1<<MAX_IPK_LIMIT_SHIFT)
#define MAX_IPK_LIMIT_PU_COEFF          (u16)((float32_t)((float32_t)((float32_t)MAX_IPK_LIMIT_PU_HIGH-(float32_t)MAX_IPK_LIMIT_PU_LOW)*(float32_t)MAX_IPK_LIMIT_SCALER)/(float32_t)((float32_t)EL_FREQ_NOMINAL_PU_RES_BIT-(float32_t)EL_FREQ_MIN_PU_RES_BIT))

/* Public Constants -------------------------------------------------------- */
/* Setpoint di velocita' */
#define EL_FREQ_MIN                         (float32_t)((float32_t)((float32_t)RPM_MIN*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
#define EL_FREQ_MIN_PU                      (float32_t)((float32_t)EL_FREQ_MIN/(float32_t)BASE_FREQUENCY_HZ)
#define EL_FREQ_MIN_PU_RES_BIT              (u16)((float32_t)((float32_t)EL_FREQ_MIN*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)

#define EL_FREQ_NOMINAL                     (float32_t)((float32_t)((float32_t)RPM_NOMINAL*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
#define EL_FREQ_NOMINAL_PU                  (float32_t)((float32_t)EL_FREQ_NOMINAL/(float32_t)BASE_FREQUENCY_HZ)
#define EL_FREQ_NOMINAL_PU_RES_BIT          (u16)((float32_t)((float32_t)EL_FREQ_NOMINAL*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)

#define EL_FREQ_MAX                         (float32_t)((float32_t)((float32_t)RPM_MAX_SPEED*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
#define EL_FREQ_MAX_PU                      (float32_t)((float32_t)EL_FREQ_MAX/(float32_t)BASE_FREQUENCY_HZ)
#define EL_FREQ_MAX_PU_RES_BIT              (u16)((float32_t)((float32_t)EL_FREQ_MAX*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)

#define EL_FREQ_TO_CHANGE_MOD               (float32_t)((float32_t)((float32_t)RPM_TO_CHANGE_MOD*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
#define EL_FREQ_TO_CHANGE_MOD_PU            (float32_t)((float32_t)EL_FREQ_TO_CHANGE_MOD/(float32_t)BASE_FREQUENCY_HZ)
#define EL_FREQ_TO_CHANGE_MOD_PU_RES_BIT    (u16)((float32_t)((float32_t)EL_FREQ_TO_CHANGE_MOD*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)

#define EL_FREQ_RESET_START_NUM             (float32_t)((float32_t)EL_FREQ_MIN*(float32_t)0.9)
#define EL_FREQ_RESET_START_NUM_PU          (float32_t)((float32_t)EL_FREQ_RESET_START_NUM/(float32_t)BASE_FREQUENCY_HZ)
#define EL_FREQ_RESET_START_NUM_PU_RES_BIT  (u16)((float32_t)((float32_t)EL_FREQ_RESET_START_NUM*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)

#define EL_FREQ_NOMINAL_TOD                 (float32_t)((float32_t)((float32_t)RPM_NOMINAL_TOD*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
#define EL_FREQ_NOMINAL_TOD_PU              (float32_t)((float32_t)EL_FREQ_NOMINAL_TOD/(float32_t)BASE_FREQUENCY_HZ)
#define EL_FREQ_NOMINAL_TOD_PU_RES_BIT      (u16)((float32_t)((float32_t)EL_FREQ_NOMINAL_TOD*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)

#ifdef BATTERY_DEGRADATION
  #define MIN_RPM_FOR_DEGRADATION             (float32_t)RPM_MIN 
  #define EL_FREQ_FOR_DEGRADATION             (float32_t)((float32_t)((float32_t)MIN_RPM_FOR_DEGRADATION*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
  #define EL_FREQ_FOR_DEGRADATION_PU          (float32_t)((float32_t)EL_FREQ_FOR_DEGRADATION/(float32_t)BASE_FREQUENCY_HZ)
  #define EL_FREQ_FOR_DEGRADATION_PU_RES_BIT  (u16)((float32_t)((float32_t)EL_FREQ_FOR_DEGRADATION*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)
#endif  /* BATTERY_DEGRADATION */

#ifdef BMW_LIN_ENCODING
  #define RPM_CURRENT_LIM_FEEDBAKCK_ON                    80  /* [rpm] */
  #define EL_FREQ_CURRENT_LIM_FEEDBAKCK_ON                (float32_t)((float32_t)((float32_t)RPM_CURRENT_LIM_FEEDBAKCK_ON*POLE_PAIR_NUM)/60)
  #define EL_FREQ_CURRENT_LIM_FEEDBAKCK_ON_PU             (float32_t)((float32_t)EL_FREQ_CURRENT_LIM_FEEDBAKCK_ON/BASE_FREQUENCY_HZ)
  #define EL_FREQ_CURRENT_LIM_FEEDBAKCK_ON_PU_RES_BIT     (u16)((float32_t)((float32_t)EL_FREQ_CURRENT_LIM_FEEDBAKCK_ON*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)

  #define DELTA_RPM_FOR_WM_SHUTDOWN                       220  /* [rpm] -> Se la velocita' in caso di windmill supera la velocita' di set point di SHUTDOWN_DELTA_WM_SPEED, allora il motore viene spento e lasciato in free running finche' la velocita' non torna pari al set point. */
  #define DELTA_FREQ_FOR_WM_SHUTDOWN                      (float32_t)((float32_t)((float32_t)DELTA_RPM_FOR_WM_SHUTDOWN*POLE_PAIR_NUM)/60)
  #define DELTA_FREQ_FOR_WM_SHUTDOWN_PU                   (float32_t)((float32_t)DELTA_FREQ_FOR_WM_SHUTDOWN/BASE_FREQUENCY_HZ)
  #define DELTA_FREQ_FOR_WM_SHUTDOWN_PU_RES_BIT           (u16)((float32_t)((float32_t)DELTA_FREQ_FOR_WM_SHUTDOWN*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)

  #define MAX_RPM_FOR_WM_SHUTDOWN                         2200 /* [rpm] -> Se la velocita' in caso di windmill supera questa soglia, non viene fatto lo spegnimento, ma il rallentamento controllato. */
  #define MAX_FREQ_FOR_WM_SHUTDOWN                        (float32_t)((float32_t)((float32_t)MAX_RPM_FOR_WM_SHUTDOWN*POLE_PAIR_NUM)/60)
  #define MAX_FREQ_FOR_WM_SHUTDOWN_PU                     (float32_t)((float32_t)MAX_FREQ_FOR_WM_SHUTDOWN/BASE_FREQUENCY_HZ)
  #define MAX_FREQ_FOR_WM_SHUTDOWN_PU_RES_BIT             (u16)((float32_t)((float32_t)MAX_FREQ_FOR_WM_SHUTDOWN*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)    
#endif  /* BMW_LIN_ENCODING */

#define MAX_FREQ_TO_BRAKE                   (float32_t)((float32_t)((float32_t)MAX_RPM_TO_BRAKE*(float32_t)POLE_PAIR_NUM)/(float32_t)60)
#define MAX_FREQ_TO_BRAKE_PU                (float32_t)((float32_t)MAX_FREQ_TO_BRAKE/(float32_t)BASE_FREQUENCY_HZ)
#define	MAX_FREQ_TO_BRAKE_PU_RES_BIT        (u16)((float32_t)((float32_t)MAX_FREQ_TO_BRAKE*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)  

#ifdef REDUCED_SPEED_BRAKE_VS_TEMPERATURE
  #define RVS_BRAKE_FREQ_MIN_PU               (u16)((float32_t)((float32_t)((float32_t)((float32_t)RPM_BRAKE_MIN*POLE_PAIR_NUM)/60)*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)
  #define RVS_TEMPERATURE_TRESHOLD_PU         (s16)((float32_t)((float32_t)RVS_TEMPERATURE_TRESHOLD*TEMPERATURE_RES_FOR_PU_BIT)/TLE_BASE_TEMPERATURE_DEG)
#endif /* REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

#if( FAN_TYPE == BLOWER )
  #define BLOWER_FREE_AIR_MAX_FREQ            (u16)((u32)((u32)BLOWER_FREE_AIR_MAX_RPM*POLE_PAIR_NUM)/60)
  #define BLOWER_FREE_AIR_MAX_FREQ_PU         (u16)((u32)((u32)BLOWER_FREE_AIR_MAX_FREQ*BASE_FREQUENCY_HZ)/FREQUENCY_RES_FOR_PU_BIT)
  #define I_FEEDBACK_LIM_PU                   (u16)((u32)((u32)I_FEEDBACK_LIM*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP) 
  #define I_CHECK_TRANSIENT_THRESHOLD_PU      (u16)((u32)((u32)I_CHECK_TRANSIENT_THRESHOLD*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP)
  #define I_DELTA_PU                          (u16)((u32)((u32)I_DELTA*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP)
  #define I_LIMIT_PU                          (u16)((u32)((u32)I_LIMIT*CURRENT_RES_FOR_PU_BIT)/BASE_CURRENT_AMP) 
#endif  /* FAN_TYPE */

#if ( ( FAN_TYPE == BLOWER ) || defined(OVERCURRENT_ROF) )  /* Per il RoF "semplice" del blower oppure per la procedura di Overcurrent-RoF. */
  #define IPK_TRANSIENT_CONTROL_RPM           VoF_END_FREQ_RPM  /* La procedura di ACM_CheckCurrentTransient() e' attiva dalla velocita' di bocche aperte (at 0 Pa) in su. */
  #define IPK_TRANSIENT_CONTROL_FREQ          (float32_t)((float32_t)((float32_t)IPK_TRANSIENT_CONTROL_RPM*POLE_PAIR_NUM)/60)        
  #define IPK_TRANSIENT_CONTROL_PU            (float32_t)((float32_t)IPK_TRANSIENT_CONTROL_FREQ/BASE_FREQUENCY_HZ)
  #define IPK_TRANSIENT_CONTROL_PU_RES_BIT    (u16)((float32_t)((float32_t)IPK_TRANSIENT_CONTROL_FREQ*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)    
#endif  /* ( FAN_TYPE == BLOWER ) || defined(OVERCURRENT_ROF) */

/* Step di incremento/decremento rampa di frequenza */
#define	VoF_INIT_FREQ                   (u16)((u32)((u32)VoF_INIT_FREQ_RPM*POLE_PAIR_NUM)/60u)	
#define VoF_INIT_FREQ_PU                (float32_t)((float32_t)VoF_INIT_FREQ/(float32_t)BASE_FREQUENCY_HZ)
#define VoF_INIT_FREQ_PU_RES_BIT        (u16)((float32_t)((float32_t)VoF_INIT_FREQ*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)    

#define VoF_END_FREQ                    (u16)((u32)((u32)VoF_END_FREQ_RPM*POLE_PAIR_NUM)/60u)
#define VoF_END_FREQ_PU                 (float32_t)((float32_t)VoF_END_FREQ/(float32_t)BASE_FREQUENCY_HZ)
#define VoF_END_FREQ_PU_RES_BIT         (u16)((float32_t)((float32_t)VoF_END_FREQ*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)

#define FREQ_INC_PU_FLOAT               (float32_t)((float32_t)((float32_t)((float32_t)VoF_END_FREQ_PU - VoF_INIT_FREQ_PU)*(float32_t)FREQUENCY_RES_FOR_PU_BIT*(float32_t)REG_LOOP_TIME*REG_LOOP_TIME_SEC)/(float32_t)FREQ_RAMP_TIME)
#define FREQ_INC_PU_INT                 (u16)((float32_t)((float32_t)((float32_t)VoF_END_FREQ_PU - VoF_INIT_FREQ_PU)*(float32_t)FREQUENCY_RES_FOR_PU_BIT*(float32_t)REG_LOOP_TIME*REG_LOOP_TIME_SEC)/(float32_t)FREQ_RAMP_TIME)
#define FREQ_INC_PU                     ( ( FREQ_INC_PU_FLOAT > (float32_t)FREQ_INC_PU_INT ) ? (u16)(FREQ_INC_PU_INT+(u16)1) : FREQ_INC_PU_INT )

#ifdef BMW_LIN_ENCODING
  #define FREQ_INC_SLOW_PU_FLOAT        (float32_t)((float32_t)((float32_t)((float32_t)VoF_END_FREQ_PU - VoF_INIT_FREQ_PU)*FREQUENCY_RES_FOR_PU_BIT*REG_LOOP_TIME*REG_LOOP_TIME_SEC)/(float32_t)((float32_t)FREQ_RAMP_TIME*2))
  #define FREQ_INC_SLOW_PU_INT          (u16)((float32_t)((float32_t)((float32_t)VoF_END_FREQ_PU - VoF_INIT_FREQ_PU)*FREQUENCY_RES_FOR_PU_BIT*REG_LOOP_TIME*REG_LOOP_TIME_SEC)/(float32_t)((float32_t)FREQ_RAMP_TIME*2))
  #define FREQ_INC_SLOW_PU              ( ( FREQ_INC_SLOW_PU_FLOAT > FREQ_INC_SLOW_PU_INT ) ? (u16)(FREQ_INC_SLOW_PU_INT+1) : FREQ_INC_SLOW_PU_INT )
#endif  /* BMW_LIN_ENCODING */

#define FREQ_DEC_PU_FLOAT               (float32_t)((float32_t)((float32_t)((float32_t)VoF_END_FREQ_PU - VoF_INIT_FREQ_PU)*(float32_t)FREQUENCY_RES_FOR_PU_BIT*(float32_t)REG_LOOP_TIME*REG_LOOP_TIME_SEC)/(float32_t)FREQ_RAMP_TIME)
#define FREQ_DEC_PU_INT                 (u16)((float32_t)((float32_t)((float32_t)VoF_END_FREQ_PU - VoF_INIT_FREQ_PU)*(float32_t)FREQUENCY_RES_FOR_PU_BIT*(float32_t)REG_LOOP_TIME*REG_LOOP_TIME_SEC)/(float32_t)FREQ_RAMP_TIME)
#define FREQ_DEC_PU                     ( ( FREQ_DEC_PU_FLOAT > (float32_t)FREQ_DEC_PU_INT ) ? (u16)(FREQ_DEC_PU_INT+(u16)1) : FREQ_DEC_PU_INT )

/* Rampa di tensione VoF */
#define K_VoF_RES_FOR_PU_SHIFT        (u16)10
#define K_VoF_RES_FOR_PU_BIT          (u16)((u16)1<<K_VoF_RES_FOR_PU_SHIFT)
#define K_VoF_PU_SCALED               (u16)((float32_t)Ke_CONSTANT_PU*(float32_t)K_VoF_RES_FOR_PU_BIT)

/* Limiti di tensione di fase */
#define VAPP_MAX_VALUE_VOLT           (float32_t)(((float32_t)V_BUS_REFERENCE_VOLT/(float32_t)2)*(float32_t)1.15)   /* (Vbus/2)*1.15 <=> Vbus / sqrt(3) => Vbus = 13 V => 13 V / 1.732 = 7.5 V */
#define VAPP_MAX_OVERMOD_VOLT         (float32_t)(VAPP_MAX_VALUE_VOLT*(float32_t)1.37)                              /* Vbus = 13 V => 7.5 V * 1.37 = 10.275 V  (Per poter andare in sovramodulazione!!) */

#define VAPP_MAX_LINEAR_MOD_BIT       (u16)512

#define VAPP_OVERMOD_GAINSHIFT        (u16)5

#define MAX_PHASE_VOLTAGE_PU          (u16)((float32_t)((float32_t)VAPP_MAX_VALUE_VOLT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/(float32_t)BASE_VOLTAGE_VOLT)
#define MAX_PHASE_VOLTAGE_OVERMOD_PU  (u16)((float32_t)((float32_t)VAPP_MAX_OVERMOD_VOLT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/(float32_t)BASE_VOLTAGE_VOLT)

/* Public type definition -------------------------------------------------- */
/** 
    \typedef StartStatus_t
    \brief Status returned by mtc and acmotor functions
    \details This enumerator define names for returned state in mtc start up and Run_On_Fly. This enumerator is used in different functions
    to describe how the Start up is going on.
*/
typedef enum {
START_WAIT_FOR_RIGHT_W,   /* !< Set when motor speed is out of run on fly window values.                 */
START_UNDER_W_MIN,        /* !< Set if motor speed is under minimum motor frequency. Start from parking. */
START_FROM_RUN            /* !< Set if motor speed is over setting speed. Start from run on fly.         */
} StartStatus_t;

typedef struct 
{
  u16 u16VoFVoltagePU;
  u16 u16VoltagePU;
  u16 u16CurrentPU;
  u16 u16FrequencyPU;
  u16 u16FrequencyAppPU;
  s16 s16AngleDeltaPU;
  s16 s16AnglePhiPU;  
} PMSMstructPU;

typedef struct 
{
  u16 u16FreqRampPU;
  u16 u16FreqRampSmoothPU;
} rampStruct; 

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
  extern PMSMstructPU strMachine;
  
#pragma DATA_SEG DEFAULT

/* Public Functions prototypes --------------------------------------------- */
void ACM_Init (void);
void ACM_InitOnFly (void);
void ACM_DoMotorControl (void);
void ACM_ManageParking (void);
void ACM_UpdateControlStruct (u16 u16ActualBemf, u16 u16ActualFreq);
#ifdef BMW_LIN_ENCODING
  void ACM_CheckWindmillShutdown (void);
#endif  /* BMW_LIN_ENCODING */
extern u16 (*ACM_SetFrequencySetPoint)(u8 InRef);

#ifdef RVS
u16  ACM_Get_u16FreqRampPU(void);
#endif
void ACM_Set_u16FreqRampSmoothPU(u16 value);
u16  ACM_Get_u16FreqRampSmoothPU(void);


#endif  /* ACM_H */

/*** (c) 2017 SPAL Automotive ****************** END OF FILE **************/
