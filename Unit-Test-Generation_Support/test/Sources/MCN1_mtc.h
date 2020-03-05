/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MCN1_mtc.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MCN1_mtc.h
    \version see \ref FMW_VERSION
    \brief Motion Control Routines prototypes
    \details In this files all the intermidiate motion control routines are regrouped.
    In line of principle all functions that are contained in this files have to drive Level 0 routines
    and will use and provides variables flor routines in \ref MCN1_acmotor.c 
*/

#ifndef MTC_AC_H
#define MTC_AC_H  


/* Public Constants -------------------------------------------------------- */
/* Freq initialisation at MCU start-up */
#define FREQ_INIT													  (u16)50

/* Parcheggio: tempistiche e sweep di angolo */
#define PRE_ALIGNMENT_BRAKING_TIME           (u8)((float32_t)PRE_ALIGNMENT_BRAKING_TIME_SEC/REG_LOOP_TIME_SEC)

#define PARKING_TIME_25DEG_TEMP_PU    (s16)((float32_t)((float32_t)PARKING_TIME_25DEG_TEMPERATURE*TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)
#define PARKING_TIME_160DEG_TEMP_PU   (s16)((float32_t)((float32_t)PARKING_TIME_160DEG_TEMPERATURE*TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)

#define KT_NUM                            (u16)246    /* 246/(2^14) = 246/16384 <=> 15/10000 = 0.015 */
#define KT_SHIFT                          (u8)14
#define PARKING_TIME_25DEG                (u16)((float32_t)((float32_t)((float32_t)/*43860*/58480 * FAN_ROT_INERTIA * (float32_t)(Rs_REF + R_MOS_REF))/(float32_t)(E_REF_1000RPM*E_REF_1000RPM)) / (float32_t)UPDATE_LOOP_TIME)
#define PARKING_TIME_160DEG               (u16)(PARKING_TIME_25DEG + (u16)((u32)((u32)((u32)((u32)PARKING_TIME_25DEG * KT_NUM * (u16)(PARKING_TIME_160DEG_TEMP_PU - PARKING_TIME_25DEG_TEMP_PU))>>TEMPERATURE_RES_FOR_PU_SHIFT)*TLE_BASE_TEMPERATURE_DEG)>>KT_SHIFT))

#define START_SWEEP_ANGLE                 (u32)((float32_t)((float32_t)START_SWEEP_ANGLE_DEG/(float32_t)360)*(float32_t)U16_MAX/*U32_MAX*/)   /* Angolo in u32, non PU (ancora in uso nell'interrupt di modulazione) */
#define TARGET_SWEEP_ANGLE                (u32)((float32_t)((float32_t)TARGET_SWEEP_ANGLE_DEG/(float32_t)360)*(float32_t)U16_MAX/*U32_MAX*/)  /* Angolo in u32, non PU (ancora in uso nell'interrupt di modulazione) */

/* Run on Fly */
#if ( ROTATION == CW )
  #if ( ( PCB == PCB211A_HY ) || ( PCB == PCB211A ) || ( PCB == PCB221A ) || ( PCB == PCB221B ) || ( PCB == PCB224A) || ( PCB == PCB224B) || ( PCB == PCB228B ) )
    #define	K_SOF_OFFSET_FIXED              (u16)60074     /* 330 °el (N.B.: ZC is on U phase) */
  #elif ( PCB == PCB237E )
    #define	K_SOF_OFFSET_FIXED              (u16)5461      /* 30 °el (N.B.: ZC is on W phase) */
  #elif ( ( PCB == PCB250A ) || ( PCB == PCB262A ) )
    #ifdef DUAL_LINE_ZERO_CROSSING
      #define K_SOF_OFFSET_FIXED            (u16)38228     /* 30 °el + 180 °el (dovuto alla presenza del mosfet all'uscita della linea differenziale) tra V e W */
    #else
      #define K_SOF_OFFSET_FIXED            (u16)60074     /* 330 °el (N.B.: ZC is on W phase) */
    #endif  /* DUAL_LINE_ZERO_CROSSING */
  #elif ( ( PCB == PCB211B ) || ( PCB == PCB213A ) || ( PCB == PCB213B )  || ( PCB == PCB213C ) || ( PCB == PCB220A ) || ( PCB == PCB220B ) || ( PCB == PCB220C ) || ( PCB == PCB213D ) || ( PCB == PCB224C ) || ( PCB == PCB224D ) || ( PCB == PCB225A ) || ( PCB == PCB225B ) || ( PCB == PCB225C ) || ( PCB == PCB233C ) || ( PCB == PCB228A ) || ( PCB == PCB230A ) || ( PCB == PCB240A ) || ( PCB == PCB236B ) || ( PCB == PCB258A ) )
    #ifdef DUAL_LINE_ZERO_CROSSING
      #define K_SOF_OFFSET_FIXED            (u16)49152     /* 90 °el + 180 °el (dovuto alla presenza del mosfet all'uscita della linea differenziale) tra U e V */
    #else
      #define K_SOF_OFFSET_FIXED            (u16)16384     /* 90 °el (N.B.: ZC is on W phase) */
    #endif  /* DUAL_LINE_ZERO_CROSSING */
  #endif  /* PCB */
#elif ( ROTATION == CCW )
  #if ( ( PCB == PCB211A_HY ) || ( PCB == PCB211A ) || ( PCB == PCB221A ) || ( PCB == PCB221B ) || ( PCB == PCB224A) || ( PCB == PCB224B) || ( PCB == PCB228B ) )
    #define	K_SOF_OFFSET_FIXED              (u16)16384     /* 90 °el (N.B.: ZC is on W phase) */
  #elif ( PCB == PCB237E )
    #define	K_SOF_OFFSET_FIXED              (u16)18080     /* 99 °el (N.B.: ZC is on U phase) */
  #elif ( ( PCB == PCB250A ) || ( PCB == PCB262A ) )
    #ifdef DUAL_LINE_ZERO_CROSSING
      #define K_SOF_OFFSET_FIXED            (u16)50848     /* 99 °el + 180 °el (dovuto alla presenza del mosfet all'uscita della linea differenziale) tra V e W */
    #else
      #define K_SOF_OFFSET_FIXED            (u16)16384     /* 90 °el (N.B.: ZC is on U phase) */
    #endif  /* DUAL_LINE_ZERO_CROSSING */
  #elif ( ( PCB == PCB211B ) || ( PCB == PCB213A ) || ( PCB == PCB213B )  || ( PCB == PCB213C ) || ( PCB == PCB220A ) || ( PCB == PCB220B ) || ( PCB == PCB220C ) || ( PCB == PCB213D ) || ( PCB == PCB224C ) || ( PCB == PCB224D ) || ( PCB == PCB225A ) || ( PCB == PCB225B ) || ( PCB == PCB225C ) || ( PCB == PCB233C ) || ( PCB == PCB228A ) || ( PCB == PCB230A ) || ( PCB == PCB240A ) || ( PCB == PCB236B ) || ( PCB == PCB258A ) )
    #ifdef DUAL_LINE_ZERO_CROSSING
      #define K_SOF_OFFSET_FIXED            (u16)27307     /* 330 °el + 180 °el (dovuto alla presenza del mosfet all'uscita della linea differenziale) tra U e V */
    #else
      #define K_SOF_OFFSET_FIXED            (u16)60074     /* 330 °el (N.B.: ZC is on W phase) */
    #endif  /* DUAL_LINE_ZERO_CROSSING */
  #endif  /* PCB */
#endif  /* ROTATION */

/* Possibles motor control status: */
/* C_D_ISR_Status bitfield */
/**
    \def PHASE_INITIAL_SPEED_BIT
    \brief Set during run on fly phase. This flag is used as positioning.
    \details Before running from stop, motor speed is checked speed is needed. PHASE_INITIAL_SPEED_BIT tells
    motor runs into run on fly phase.

    \def PHASE_INITIAL_SPEED
    \brief Set during run on fly phase. This flag is used as weight.
    \details Before running from stop, motor speed is checked speed is needed. PHASE_INITIAL_SPEED_BIT tells
    motor runs into run on fly phase.

    \def PHASE_LOCKED_BIT
    \brief Set during normal running ( speed > 500rpm) operation. This flag is used as positioning.
    \details After a blinding rampup closed loops are performed. PHASE_NOT_LOCKED_BIT tells
    motor runs into closed loops.

    \def PHASE_LOCKED
    \brief Set during normal running ( speed > 500rpm) operation. This flag is used as weight.
    \details After a blinding rampup closed loops are performed. PHASE_NOT_LOCKED_BIT tells
    motor runs into closed loops.

    \def HALL_CHECKED_BIT
    \brief Set during normal running ( speed > 500rpm) operation. This flag is used as positioning.
    \details After a blinding rampup closed loops are performed. HALL_CHECKED_BIT tells
    motor to compute closing loops.  

    \def HALL_CHECKED
    \brief Set during normal running ( speed > 500rpm) operation. This flag is used as weight.
    \details After a blinding rampup closed loops are performed. HALL_CHECKED_BIT tells
    motor to compute closing loops.  

    \def MANAGE_TACHO_BIT
    \brief Set during normal running ( speed > 500rpm) operation. This flag is used as positioning.
    \details After a blinding rampup closed loops are performed. MANAGE_TACHO_BIT tells
    motor to compute phi and so gamma measuring.

    \def MANAGE_TACHO
    \brief Set during normal running ( speed > 500rpm) operation. This flag is used as weight.
    \details After a blinding rampup closed loops are performed. MANAGE_TACHO_BIT tells
    motor to compute phi and so gamma measuring.

    \def DIAG_SPEED_FEEDBACK_BIT
    \brief Set when speed feedback is required. This flag is used as positioning.
    \details When speed feedback signal is required, this flag tells to toggle speed output..

    \def DIAG_SPEED_FEEDBACK
    \brief Set when speed feedback is required. This flag is used as weight.
    \details When speed feedback signal is required, this flag tells to toggle speed output..

    \def UPDATE_LOOP_BIT
    \brief Set when control loops have to be updated. This flag is used as positioning.
    \details This flag tells to update all the PI regulators and the V/f modulator inputs.

    \def UPDATE_LOOP
    \brief Set when control loops have to be updated. This flag is used as weight.
    \details This flag tells to update all the PI regulators and the V/f modulator inputs.
*/
#define PHASE_INITIAL_SPEED_BIT     1
#define PHASE_INITIAL_SPEED         ((u8)(1u<<PHASE_INITIAL_SPEED_BIT))
#define PHASE_LOCKED_BIT            2
#define PHASE_LOCKED                ((u8)(1u<<PHASE_LOCKED_BIT))
#define MANAGE_TACHO_BIT            4
#define MANAGE_TACHO                ((u8)(1u<<MANAGE_TACHO_BIT))
#define DIAG_SPEED_FEEDBACK_BIT     5
#define DIAG_SPEED_FEEDBACK         ((u8)(1u<<DIAG_SPEED_FEEDBACK_BIT))
#define UPDATE_LOOP_BIT             7
#define UPDATE_LOOP                 ((u8)(1u<<UPDATE_LOOP_BIT))

/* Delta measure from ZC Vf to Curr */
/**
    \def ZCVF_OVFinPWMisrBit
    \brief Used to take into account PWM input and timer overflow concurrent interrupt. This flag is used as positioning.
    \details When PWM input and timer overflow interrupt happen at the same time, this flag is used to avoid bad phi computation.

    \def ZCVF_OVFinPWMisr
    \brief Used to take into account PWM input and timer overflow concurrent interrupt. This flag is used as weight.
    \details When PWM input and timer overflow interrupt at the same time, this flag is used to avoid bad computation.

    \def ZCVF_TachoisrBit
    \brief Used to take into account PWM input and timer overflow concurrent interrupt. This flag is used as positioning.
    \details When PWM bridge and tacho capture interrupt happen at the same time, this flag is used to avoid bad phi computation.

    \def ZCVF_Tachoisr
    \brief Used to take into account PWM input and timer overflow concurrent interrupt. This flag is used as weight.
    \details When PWM bridge and tacho capture interrupt happen at the same time, this flag is used to avoid bad phi computation.

    \def ZCVFBeforeZCCurrBit
    \brief Used to take into account Vf zc before current zc. This flag is used as positioning.
    \details When voltage zero crossing happen before current zc special case has to considered. Not used.

    \def ZCVFBeforeZCCurr
    \brief Used to take into account Vf zc before current zc. This flag is used as weight.
    \details When voltage zero crossing happen before current zc special case has to considered. Not used.

    \def ZCVFEnableBit
    \brief Used to let Vf zc taking into account. This flag is used as positioning.
    \details In order to compute phi, vf zc has to be considered. This flag tells to start Vf zc evaluation.

    \def ZCVFEnable
    \brief Used to let Vf zc taking into account. This flag is used as weight.
    \details In order to compute phi, vf zc has to be considered. This flag tells to start Vf zc evaluation.

    \def ZCCurrEnableBit
    \brief Used to let current zc taking into account. This flag is used as positioning.
    \details In order to compute phi, current zc has to be considered. This flag tells to start current zc evaluation.

    \def ZCCurrEnable
    \brief Used to let current zc taking into account. This flag is used as weight.
    \details In order to compute phi, current zc has to be considered. This flag tells to start current zc evaluation.

    \def ZCCurrFilterBit
    \brief In order to avoid bad current zc evaluation, after good front capturing, capturing is disabled for a fixed time. This flag is used as positioning.
    \details After good zc current capturing, this capturing is disabled for a fixed time in order to avoid bad evaluation due to ringing.

    \def ZCCurrFilter
    \brief In order to avoid bad current zc evaluation, after good front capturing, capturing is disabled for a fixed time. This flag is used as weight.
    \details After good zc current capturing, this capturing is disabled for a fixed time in order to avoid bad evaluation due to ringing.
*/
#define ZCVF_OVFinPWMisrBit (u8)0
#define ZCVF_OVFinPWMisr    ((u8)((u8)1<<ZCVF_OVFinPWMisrBit))
#define ZCVF_TachoisrBit    (u8)1
#define ZCVF_Tachoisr       ((u8)((u8)1<<ZCVF_TachoisrBit))
#define ZCVFBeforeZCCurrBit (u8)2
#define ZCVFBeforeZCCurr    ((u8)((u8)1<<ZCVFBeforeZCCurrBit))
#define ZCVFEnableBit       (u8)3
#define ZCVFEnable          ((u8)((u8)1<<ZCVFEnableBit))
#define ZCCurrEnableBit     (u8)4
#define ZCCurrEnable        ((u8)((u8)1<<ZCCurrEnableBit))
#define ZCCurrFilterBit     (u8)5
#define ZCCurrFilter        ((u8)((u8)1<<ZCCurrFilterBit))

/* u8MTCStatus bitfield */
/**
    \def FREQ_CHANGE_BIT
    \brief Tells motor to change speed. This flag is used as positioning.
    \details When set, this flag causes to compute new electrical frequency for pwm modulation.

    \def FREQ_CHANGE
    \brief Tells motor to change speed. This flag is used as weight.
    \details When set, this flag causes to compute new electrical frequency for pwm modulation.

    \def SINEMAG_CHANGE_BIT
    \brief Tells motor to change phase voltage. This flag is used as positioning.
    \details When set, this flag causes to compute new phase voltage for pwm modulation.

    \def SINEMAG_CHANGE
    \brief Tells motor to change phase voltage. This flag is used as weight.
    \details When set, this flag causes to compute new phase voltage for pwm modulation.

    \def VBUS_SAMPLED_BIT
    \brief Tells new battery voltage has been converted. This flag is used as positioning.
    \details Battery voltage is used for battery compensation. Once new battery voltage is converted,
    new phase voltage has been compensated.

    \def VBUS_SAMPLED
    \brief Tells new battery voltage has been converted. This flag is used as weight.
    \details Battery voltage is used for battery compensation. Once new battery voltage is converted,
    new phase voltage has been compensated.

    \def PHASE_RESET_BIT
    \brief Used between run on fly and closed loop. This flag is used as positioning.
    \details After run on fly and before turning bridge on, electrical phase angle has been imposed.

    \def PHASE_RESET
    \brief Used between run on fly and closed loop. This flag is used as weight.
    \details After run on fly and before turning bridge on, electrical phase angle has been imposed.

    \def FWD_BWD_DIR_BIT
    \brief Used to define wise direction in RVS is defined.
    \details Used to detect wise direction and to set right PWM bridge register.

    \def FWD_BWD_DIR
    \brief Used to define wise direction in RVS is defined.
    \details Used to detect wise direction and to set right PWM bridge register.

    \def SINE_GEN_u8MTCStatus_FLAGS
    \brief Used to reset u8MTCStatus. 
    \details If RVS is used, FWD_BWD_DIR_BIT bit into u8MTCStatus has to keep its own value from different starting.
*/
/* N.B.                                                                                             */
/*   Pay attention at INT0_PWMIsr.c line 240:                                                       */
/*   VbusCompensation:                                                                              */
/*   lda u8MTCStatus            Did u8VbusSampledAtPWM or u16SineMag change?                        */
/*   bit #0x06                  Pay attention at this constant if following bitfield are changed!!! */
/*   beq CalcSineFreqBuf                                                                            */
 
#define FREQ_CHANGE_BIT      0
#define FREQ_CHANGE          ((u8)(1u<<FREQ_CHANGE_BIT))
#define SINEMAG_CHANGE_BIT   1
#define SINEMAG_CHANGE       ((u8)(1u<<SINEMAG_CHANGE_BIT))
#define VBUS_SAMPLED_BIT     2
#define VBUS_SAMPLED         ((u8)(1u<<VBUS_SAMPLED_BIT))
#define PHASE_RESET_BIT      3
#define PHASE_RESET          ((u8)(1u<<PHASE_RESET_BIT))
#ifdef PULSE_ERASING
  #define PULSE_ERASE_BIT    (u8)4
  #define PULSE_ERASE        ((u8)((u8)1<<PULSE_ERASE_BIT))
#endif  /* PULSE_ERASING */
#define FWD_BWD_DIR_BIT      5
#define FWD_BWD_DIR          ((u8)(1<<FWD_BWD_DIR_BIT))

#ifdef PULSE_ERASING
  #define SINE_GEN_u8MTCStatus_FLAGS   (u8)(~((u8)(FREQ_CHANGE + SINEMAG_CHANGE + VBUS_SAMPLED + PHASE_RESET + PULSE_ERASE))) /* Before starting reset u8MTCStatus flags but FWD_BWD_DIR_BIT */
#else
  #define SINE_GEN_u8MTCStatus_FLAGS   (u8)(~((u8)(FREQ_CHANGE + SINEMAG_CHANGE + VBUS_SAMPLED + PHASE_RESET))) /* Before starting reset u8MTCStatus flags but FWD_BWD_DIR_BIT */
#endif  /* PULSE_ERASING */

/* Public type definition -------------------------------------------------- */
/** 
    \typedef C_D_ISR_Status_t
    \brief Variable for Interrupt service Routines Flags
    \details This u8 byte is defined in a typedef to fast identify a flag reserved variable.
    Here, each bit is associated to a specific flag used in interrupt service routines. 
      -\ref PHASE_NOT_LOCKED_BIT
      -\ref PHASE_INITIAL_SPEED_BIT
      -\ref PHASE_LOCKED_BIT
      -\ref HALL_CHECKED_BIT
      -\ref MANAGE_TACHO_BIT
      -\ref DIAG_SPEED_FEEDBACK_BIT
      -\ref PLATEAU_ON_BIT
*/
typedef u8 C_D_ISR_Status_t;

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
extern volatile C_D_ISR_Status_t C_D_ISR_Status;
extern u8 ZCStatusBitfield;
extern u8 u8MTCStatus;
extern volatile u16 u16PWMCount;
extern volatile u16 u16UpdateLoopTimer;
extern volatile u16 u16UpdateLoopTime;
extern volatile u16 u16SineMag;
extern volatile u16 u16SineMagBuf;
extern volatile u16 u16SineMagScaled;
extern volatile u16 u16SineFreq;
extern volatile u32 u32SineFreqBuf;
extern volatile u16 u16Phase;
extern volatile u16 u16PhaseBuff;
extern volatile u16 u16MaxCurr;
extern volatile u16 u16PeakCurr;

#ifdef GROUND_BALANCE
  extern volatile u16 u16HwRefTimer;
  extern BOOL BOOLHwRefFlag;
#endif  /* GROUND_BALANCE */
  
#if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
  extern volatile u16 u16VbattAvg;                /* Tensione di batteria (Vmon) filtrata RC. */
#endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */
   
#pragma DATA_SEG DEFAULT

/* Public Functions prototypes --------------------------------------------- */
void  MTC_ResetZCECntrl(void);
void  MTC_InitMTCVariables(void);
void  MTC_InitSineGen(void);
void  MTC_ForceSineGen(u16 u16NewFrequency);
/*void  MTC_UpdateSineAmplitude(u16 u16Target);*/
/*static void MTC_RegParkAmplitude(u16 u16ParkingFreq);*/
BOOL  MTC_UpdateSine(u16 u16NewVoltage, u16 u16NewFrequency);
/*u16   MTC_GetVoltage(void);*/
u16   MTC_GetScaledVoltage(void);
void  MTC_ComputeStartOnFlyPhase(void);
/*void MTC_SettingForAlignmentPhase(void);*/
void  MTC_SettingForParkingPhase(void);
u16   MTC_ComputeParkingTime(void);
u16   MTC_GetRunOnFlySinAmplitude(u16 u16ElectricalFreq);
void  MTC_ParkFailSafeMonitor_rst(void);
BOOL  MTC_ParkAngleSweep(void);

#if ( !defined(RVS) || (defined(RVS) && defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE)) )
  void MTC_SetupCheckStartingSpeed(void);
  StartStatus_t MTC_CheckStartingSpeed(u16 u16SetPoint);
#endif  /* !RVS || RVS && REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

#ifdef BATTERY_DEGRADATION
  void MTC_InitBatteryDegragation (void);
#endif  /* BATTERY_DEGRADATION */
 
#endif  /* MTC_AC_H */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
