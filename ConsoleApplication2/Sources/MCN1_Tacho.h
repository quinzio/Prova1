/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MCN1_Tacho.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MCN1_Tacho.h
    \version see \ref FMW_VERSION
    \brief Tacho high level headerfile.
    \details Here are regrouped all functions prototypes related to Tacho managing and calculation
    Here there are also structures definitions and the extern declaration for global variables.
*/

#ifndef TACHO_H
#define TACHO_H  


/* Public Constants -------------------------------------------------------- */
/* Speed Sensor */
#define SPEED_FIFO_SIZE     (u8)4

/* Tacho masking */
#define TACHO_MASK_NUM_VAL          5        /* [5/(2^3)] -> Mask (5/8)*freq_app */
#define TACHO_MASK_NUM              ((u32)TACHO_MASK_NUM_VAL * PWM_FREQ)   
#define TACHO_MASK_DEN              (u32)8

/* Tacho computation */
#define TACHOPRESC                          (u8)2        /* Tacho freq count = MTC_CLOCK/(1<<TACHOPRESC) */
#define TACHOFREQCOUNT                      (u32)((u32)(MTC_CLOCK)/(u32)((u32)1<<(TACHOPRESC)))

#define READ_TACHO_TIME                     (u8)38       /* Wait 0.3 s (8 ms timebase) to check starting on fly               */
#define RUN_ON_FLY_VALID_TACHO_NUMBER       (u8)5        /* Wait 5 consecutive good tacho edge before passing in running mode */

#define RUN_ON_FLY_LUT_SIZE	                (u8)16       /* Run on fly LUT size */

#ifdef REDUCED_SPEED_BRAKE_VS_TEMPERATURE
  #define COUNT_TO_RUN_MAX	                  ((u32)TACHOFREQCOUNT/(u16)((u32)((u32)RPM_BRAKE_MIN*POLE_PAIR_NUM)/60u))
#else
  #define COUNT_TO_RUN_MAX	                  ((u32)TACHOFREQCOUNT/(u16)((u32)((u32)RPM_TO_ROF*POLE_PAIR_NUM)/60u))	
#endif /*REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

/* Tacho defines for speed-range checks */
#define F_E_MIN                             (float32_t)0.3        /* Means 0.3 Hz. If any frequency below this limit is found -> error. */
#define EL_FREQ_ERR                         (u16)((u32)((u32)((u32)RPM_NOMINAL*2)*POLE_PAIR_NUM)/60)
#define	F_E_STUCK_TOLERANCE                 (u8)10
#define STARTUP_ANTI_RINGING                (u16)((u32)((u32)PWM_FREQ*5u)/(u16)((u16)((u8)((u32)((u32)VoF_INIT_FREQ_RPM*POLE_PAIR_NUM)/60u)+F_E_STUCK_TOLERANCE)*8u))
#define COUNT_AT_F_E_MAX                    (u32)((u32)TACHOFREQCOUNT/(float32_t)F_E_MIN)
#define COUNT_AT_F_E_MIN                    (u32)((u32)TACHOFREQCOUNT/(u16)EL_FREQ_ERR)
#define NUM_OVF_AT_F_MIN                    (u32)((u32)TACHOFREQCOUNT/(u32)((float32_t)F_E_MIN*(float32_t)U16_MAX))

/* Public type definition -------------------------------------------------- */
/** 
    \struct ElectricFreq_s
    \brief Data struct to keep trace of timing between Tacho IC events
    \details To calculate the electrical period the alghoritm has to make the difference between two time events.
    Needed data to make this calc are stored in this structure that is optimized for speed and not for space.
*/
typedef struct 
{
	u16 u16ActualCapture;   /* !< Here is stored the actual timer value when a new IC Tacho event occurs. Number of timer ticks (at 5Mhz) at actual current zc happening.                                             */
	u16 u16LastCapture;     /* !< Here is stored the last timer value when a new IC Tacho event occurs. Number of timer ticks (at 5Mhz) at previous current zc happening.                                             */
	u16 u16NumOverflow;     /* !< Here is stored the timer overflows number between two consecutive IC Tacho event. Number of timer overflows (at 5Mhz) between previous and actual current zc happening.             */
	u32 DeltaZCVfCurr;      /* !< Number of timer ticks (at 5Mhz) between phase voltage zero crossing and phase current zero crossing.                                                                                */
	u16 ZCVfSampled;        /* !< Number of timer ticks (at 5Mhz) at phase voltage zero crossing.                                                                                                                     */
	u16 ZCVfOffset;         /* !< Number of timer ticks (at 5Mhz) between phase voltage ( at zero crossing happening sampled at 50us pwm time base ) and real phase voltage zero crossing.                            */
	u16 ZCCurrSampled;      /* !< Number of timer ticks (at 5Mhz) at phase current zero crossing.                                                                                                                     */
	u8 ZCVfNumOverflow;     /* !< Number of timer overflows (at 5Mhz) between phase voltage zero crossing and phase current zero crossing. Definiteve value.                                                          */
	u8 ZCVfOvfTemp;         /* !< Number of timer overflows (at 5Mhz) between phase voltage zero crossing and phase current zero crossing. Temporary value.                                                           */
	u16 DeltaVfOffset;      /* !< Electrical degrees between phase voltage ( at zero crossing happening sampled at 50us pwm time base ) and real phase voltage zero crossing.                                         */
	u16 TPM2CNTBuff;        /* !< Number of timer ticks (at 5Mhz) at zero crossing happening ( sampled at 50us pwm time base).                                                                                        */
	u16 SineFreqStored;     /* !< Electrical angle frequency related value to compute degrees between phase voltage ( at zero crossing happening sampled at 50us pwm time base ) and real phase voltage zero crossing */
} ElectricFreq_s;

/** 
    \def PElectricFreq_s
    \brief Pointer to a \ref ElectricFreq_s struct type
    \details This is a pointer that identify the address to the first cell of a struct array
*/
typedef ElectricFreq_s* PElectricFreq_s;

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
extern  volatile u16 u16PwmTicksToUnmaskTacho;
extern  u8 u8TachoOverflowNumber;
extern	ElectricFreq_s bufstrElectricFreqBuff[SPEED_FIFO_SIZE];
extern  volatile PElectricFreq_s pstrElectricFreqBuff_Index;
extern  BOOL boolOVFVsTacho;

#pragma DATA_SEG DEFAULT
/* Public Functions prototypes --------------------------------------------- */
void Tacho_InitTachoVariables(void);
void Tacho_ResetTachoOverflowNumber(void);
void Tacho_SetFirstRofMaskingTime(void);
void Tacho_SetTachoCount(u16 u16SetPoint);
u8 Tacho_GetOverWminTachoPeriod(void);
void Tacho_ClearOverUnderWminTachoCount(void);
BOOL Tacho_CheckTachoOverflow(void);
void Tacho_ManageTachoCapture(void);
u16 Tacho_GetActualRotorSpeed(void);

#endif  /* TACHO_H */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
