/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MCN1_acmotor.c

VERSION  :  1.3

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif

#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register
#include ADM0_ADC
#include "main.h"             /* Per SystStatus_t e strDebug */
#include "MCN1_acmotor.h"     /* Messo prima di  #include "MCN1_mtc.h"  a causa della definizione di StartStatus_t */
#include "MCN1_mtc.h"
#include "MCN1_RegPI.h"
#include "FIR1_FIR.h"
#include "TIM1_RTI.h"
#include "MCN1_Tacho.h"
#include "TLE7184F.h"
#include "VBA1_Vbatt.h"
#include "CUR1_Current.h"
#include "EMR0_Emergency.h"
#include "TMP1_Temperature.h"
#include "INP1_InputSP.h"
#include "WTD1_Watchdog.h"
#include "POWER1_MaxPower.h"
#include "PBATT1_Power.h"
#include "INT0_PWMIsrS12.h"   /* Per SINEMAGSCALED_MAX */
#include "MCN1_SINPU.h"
#include "RVP1_RVP.h"
#include "DIA1_Diagnostic.h"  /* Per DIAG_ManageDiagnosticToECU(IDLE, NO_ERROR) */
#include "SOA1_SOA.h"         /* Per SOA_LIMIT_CURRENT_PU_xx */
#include "INT0_RTIIsrS12.h"
#include "INT0_PWMIsrS12.h"

#ifdef LIN_INPUT
  #if defined(BMW_LIN_ENCODING)
    #include "COM1_LINUpdate_BMW.h"
  #elif defined(GM_LIN_ENCODING)
    #include "COM1_LINUpdate_GM.h"
  #else
    #error("Define LIN stack!!");
  #endif  /* BMW_LIN_ENCODING */
#endif  /* LIN_INPUT */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* -------------------------- Public variables ---------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var strMachine
    \brief PMSM motor structure.
    \details This structure is made up of all the variables representing the electrical machine.
    \note This structure is allocated in \b _DATA_ZEROPAGE.
    \warning This is a \b GLOBAL structure.
*/
PMSMstructPU strMachine;

#pragma DATA_SEG DEFAULT

/** 
    \var s16PmaxDeltaFreq
    \brief Maximum frequency variation at maximum power comes out of the PMAX controller.
    \details This variable is set from PMAX regulator.
    \note This variable is allocated in \b DEFAULT segment.
    \warning This is a \b STATIC variable.
*/    
static s16 s16PmaxDeltaFreq;

/**
    \var BOOLSteadyStateFlag
    \brief Boolean variable
    \details Variable that enables, in the PWM signal modulation, the delay of the narrowest pulses (some us.) to limit harmful power transients.
    \note This variable is allocated in the \b DEFAULT segment
    \warning This is a \b STATIC variable
*/
#ifdef PULSE_ERASING
  static BOOL BOOLSteadyStateFlag;
#endif  /* PULSE_ERASING */


/** 
    \var rampGenerator
    \brief Structure for frequency ramps.
    \details This structure contains a smooth-frequency ramp generator.
    \note This structure is allocated in \b _DATA_ZEROPAGE.
    \warning This is a \b GLOBAL structure.
*/
static rampStruct rampGenerator;

#ifdef BMW_LIN_ENCODING
  u16 u16FrequencyMem; 
#endif  /* BMW_LIN_ENCODING */

/* -------------------------- Private prototipes ---------------------------- */
static void ACM_InitControlStruct (void);
static u16  ACM_GetMaximumVoltageApp(void);
static void ACM_DoFrequencyRampPU (u16 u16NewFreqPU);
static void ACM_SetVoltageVoverF (void);
static void ACM_ComputeDeltaAngle (void);
static void ACM_WindMillLoop (void);
static void ACM_DeltaLimiterLoop (void);
static void ACM_MaxIpkLoop (void);
static void ACM_MaxPbattLoop (void);
static void ACM_TODLoop (void);
static void ACM_PhaseVoltageLoop (void);
static BOOL ACM_CheckWindmillRecovery (void);
static u16  ACM_RsTempCorr (void);
static u16  ACM_ComputeDeltaMax (void);
static u16  SetFrequencySetPoint (u8 u8InRef);
static u16  ACM_SetFrequencySetPointFixed (u8 u8InRef);
static void ACM_ManagePulseCanc (void);

static void ACM_Set_u16FreqRampPU(u16 value);
#ifndef RVS
static u16  ACM_Get_u16FreqRampPU(void);
#endif

#ifdef DC_LIKE_BEHAVIOUR
u16 ACM_SetFrequencySetPointDClike (u8);
#endif
u16 (*ACM_SetFrequencySetPoint)(u8 InRef) =
#ifndef DC_LIKE_BEHAVIOUR
  ACM_SetFrequencySetPointFixed;
#else
  ACM_SetFrequencySetPointDClike;
#endif  /* FAN_TYPE */

#ifdef OVERCURRENT_ROF
  void ACM_ManageOvercurrentRoF (void);
  BOOL ACM_CheckCurrentTransient (void);
#endif /* OVERCURRENT_ROF */


/*-----------------------------------------------------------------------------*/
/** 
    \fn void ACM_Init (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initializations for motor startup.
    \details This routine initializes variables and parameters for Tacho, modulation, PI-regulators and software filters.
                            \par Used Variables
    \ref BOOLSteadyStateFlag This variable enables, in the PWM signal modulation, the delay of the narrowest pulses (some us.) to limit harmful power transients\n    
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void ACM_Init(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 rampGenerator_u16FreqRampPU_tmp;

  MTC_ResetZCECntrl();                   /* Selezione sensore di zero-crossing sulla corrente.       */
  MTC_InitMTCVariables();                /* Inizializzazione variabili di controllo (basso livello). */
  Tacho_InitTachoVariables();            /* Inizializzazione varibili Tacho.                         */
  C_D_ISR_Status = PHASE_LOCKED;         /* Si parte gia' col controllo agganciato.                  */

  s16PmaxDeltaFreq = 0;

  #ifdef PULSE_ERASING
    BOOLSteadyStateFlag = FALSE;
  #endif  /* PULSE_ERASING */

  #ifdef BATTERY_DEGRADATION
    (void)MTC_InitBatteryDegragation();  /* Inizializzazione delle strutture-dati per il BATTERY_DEGRADATION. */
  #endif  /* BATTERY_DEGRADATION */

  #ifdef PULSE_ERASING
    BOOLSteadyStateFlag = FALSE;
  #endif  /* PULSE_ERASING */

  /* Inizializzazione strutture dati controllo. */
  (void)ACM_InitControlStruct();  
  (void)Power_InitPowerStruct();
  /*(void)MaxPower_PowerRegInit();*/

  /* Inizializzazione parametri regolatori. */
  (void)REG_SetParPI(START_REG,INT_REG_GAIN_START_PU,PROP_REG_GAIN_START_PU,0,VoF_REG_LIM_HIGH_PU,FALSE,1);  
  (void)REG_SetParPI(VoF_REG,INT_REG_GAIN_VoF_PU,PROP_REG_GAIN_VoF_PU,VoF_REG_LIM_LOW_PU,VoF_REG_LIM_HIGH_PU,FALSE,1); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/
  (void)REG_SetParPI(TOD_REG,0,0,TOD_REG_LIM_LOW_PU,TOD_REG_LIM_HIGH_PU,FALSE,1);
  #ifdef WM_MANAGEMENT
    (void)REG_SetParPI(WM_REG,INT_REG_GAIN_WM_PU,PROP_REG_GAIN_WM_PU,0,WM_REG_LIM_HIGH_PU,FALSE,1); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/
  #endif  /* WM_MANAGEMENT */
  (void)REG_SetParPI(MAX_CUR_REG,MAX_CURR_REG_GAIN_PU,0,0,MAX_CURR_REG_LIM_HIGH_PU,FALSE,1);
  (void)REG_SetParPI(DELTA_MAX_REG,DELTA_REG_GAIN_PU,0,0,DELTA_REG_LIM_HIGH_PU,FALSE,1);
  (void)REG_SetParPI(PMAX_REG,PMAX_REG_GAIN_PU,0,PMAX_REG_LIM_LOW_PU,0,FALSE,1);
  (void)REG_SetParPI(TEMP_REG,TEMP_REG_INT_GAIN_PU,TEMP_REG_PROP_GAIN_PU,0,0,FALSE,1);

  /* Inizializzazione variabili regolatori. */
  (void)REG_InitVarPI(START_REG,0,0,0,0,0);
  (void)REG_InitVarPI(VoF_REG,0,0,0,0,0);
  (void)REG_InitVarPI(TOD_REG,0,0,0,0,0);
  #ifdef WM_MANAGEMENT
    (void)REG_InitVarPI(WM_REG,0,0,0,0,0);
  #endif  /* WM_MANAGEMENT */
  (void)REG_InitVarPI(MAX_CUR_REG,0,0,0,0,0);
  (void)REG_InitVarPI(DELTA_MAX_REG,0,0,0,0,0);
  (void)REG_InitVarPI(PMAX_REG,0,0,0,0,0);
  (void)REG_InitVarPI(TEMP_REG,0,0,0,0,0);

  /* Inizializzazione parametri filtri. */
  (void)FIR_SetParFilt(SIGMA_FILT,SMOOTH_RAMP_INT_STEP_FREQ,SMOOTH_RAMP_FILT_FREQ_CONST,SMOOTH_RAMP_FILT_FREQ_CONST,SMOOTH_RAMP_FILT_HYST_PU,0);
  (void)FIR_SetParFilt(PBATT_FILT,(u16)((u16)UPDATE_LOOP_FREQ*FILTER_RES_FACT),PBATT_FILT_POLE_FREQ_CONST,PBATT_FILT_ZERO_FREQ_CONST,PBATT_FILT_HYST_WATT_PU,1);
  (void)FIR_SetParFilt(DELTA_FILT,(u16)((u16)UPDATE_LOOP_FREQ*FILTER_RES_FACT),DELTA_FILT_FREQ_CONST,DELTA_FILT_FREQ_CONST,DELTA_FILT_HYST_ANGLE_PU,0);
  (void)FIR_SetParFilt(PHI_FILT,(u16)((u16)UPDATE_LOOP_FREQ*FILTER_RES_FACT),PHI_FILT_FREQ_CONST,PHI_FILT_FREQ_CONST,PHI_FILT_HYST_ANGLE_PU,0);    
  #ifdef OVERCURRENT_ROF
   (void)FIR_SetParFilt(IPK_HIGH_FILT,IPK_HIGH_STEP_FREQ_CONST,IPK_HIGH_FILT_POLE_FREQ_CONST,IPK_HIGH_FILT_ZERO_FREQ_CONST,IPK_HIGH_FILT_HYST_PU,1);
  #endif  /* OVERCURRENT_ROF */
  #ifdef LIN_INPUT
    (void)FIR_SetParFilt(LIN_SPEED_FILT,LIN_SPEED_INT_STEP_FREQ,LIN_SPEED_FREQ_CONST,LIN_SPEED_FREQ_CONST,LIN_SPEED_HYST_PU,0);
  #endif  /* LIN_INPUT */

  /* Inizializzazione variabili filtri. */

  rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();

  (void)FIR_InitVarFilt(SIGMA_FILT,(s16)((u32)((u32)rampGenerator_u16FreqRampPU_tmp>>(u32)SMOOTH_RAMP_PRESCALER_SHIFT)),(s16)((u32)((u32)rampGenerator_u16FreqRampPU_tmp>>(u32)SMOOTH_RAMP_PRESCALER_SHIFT)));
  (void)FIR_InitVarFilt(PBATT_FILT,0,0);
  (void)FIR_InitVarFilt(DELTA_FILT,0,0);
  (void)FIR_InitVarFilt(PHI_FILT,0,0);
  #ifdef OVERCURRENT_ROF
   (void)FIR_InitVarFilt(IPK_HIGH_FILT,0,0);   
  #endif  /* OVERCURRENT_ROF */
  #ifdef LIN_INPUT
    (void)FIR_InitVarFilt(LIN_SPEED_FILT,(s16)((u16)rampGenerator_u16FreqRampPU_tmp>>LIN_SPEED_PRESCALER_SHIFT),(s16)((u16)rampGenerator_u16FreqRampPU_tmp>>LIN_SPEED_PRESCALER_SHIFT));  
  #endif  /* LIN_INPUT */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void ACM_InitOnFly (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em>
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initializations for motor Run on Fly.
    \details This routine initializes variables and parameters for PI-regulators and software filters.
                            \par Used Variables
    \ref BOOLSteadyStateFlag This variable enables, in the PWM signal modulation, the delay of the narrowest pulses (some us.) to limit harmful power transients\n
    \return \b void No value returned by this function
    \note none
    \warning none
*/ 
void ACM_InitOnFly (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 rampGenerator_u16FreqRampPU_tmp;

  /* Inizializzazione strutture dati controllo. */
  (void)Power_InitPowerStruct();

  #ifdef RVP_SW_MANAGEMENT_ENABLE
    (void)RVP_SetRVPOn();
  #endif  /* RVP_SW_MANAGEMENT_ENABLE */

  #ifdef PULSE_ERASING
    BOOLSteadyStateFlag = FALSE;
  #endif  /* PULSE_ERASING */

  #ifdef GROUND_BALANCE  
    u16HwRefTimer = GROUND_BALANCE_TIME_BIT;
    BOOLHwRefFlag = FALSE;
  #endif  /* GROUND_BALANCE */

  s16PmaxDeltaFreq = 0;

  #ifdef BATTERY_DEGRADATION
    (void)MTC_InitBatteryDegragation(); /* Inizializzazione delle strutture-dati per il BATTERY_DEGRADATION. */
  #endif  /* BATTERY_DEGRADATION */

  #ifdef PULSE_ERASING
    BOOLSteadyStateFlag = FALSE;
  #endif  /* PULSE_ERASING */

  /* Inizializzazione parametri regolatori. */
  (void)REG_SetParPI(START_REG,INT_REG_GAIN_START_PU,PROP_REG_GAIN_START_PU,0,VoF_REG_LIM_HIGH_PU,FALSE,1);
  (void)REG_SetParPI(VoF_REG,INT_REG_GAIN_VoF_PU,PROP_REG_GAIN_VoF_PU,VoF_REG_LIM_LOW_PU,VoF_REG_LIM_HIGH_PU,FALSE,1); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/
  (void)REG_SetParPI(TOD_REG,0,0,TOD_REG_LIM_LOW_PU,TOD_REG_LIM_HIGH_PU,FALSE,1);
  #ifdef WM_MANAGEMENT
    (void)REG_SetParPI(WM_REG,INT_REG_GAIN_WM_PU,PROP_REG_GAIN_WM_PU,0,WM_REG_LIM_HIGH_PU,FALSE,1); /*PRQA S 0410 #Nesting of parentheses has been verified. The behaviour is due to defines definitions and is correct*/
  #endif  /* WM_MANAGEMENT */
  (void)REG_SetParPI(MAX_CUR_REG,MAX_CURR_REG_GAIN_PU,0,0,MAX_CURR_REG_LIM_HIGH_PU,FALSE,1);
  (void)REG_SetParPI(DELTA_MAX_REG,DELTA_REG_GAIN_PU,0,0,DELTA_REG_LIM_HIGH_PU,FALSE,1);
  (void)REG_SetParPI(PMAX_REG,PMAX_REG_GAIN_PU,0,PMAX_REG_LIM_LOW_PU,0,FALSE,1);
  (void)REG_SetParPI(TEMP_REG,TEMP_REG_INT_GAIN_PU,TEMP_REG_PROP_GAIN_PU,0,0,FALSE,1);

  /* Inizializzazione variabili regolatori. */
  (void)REG_InitVarPI(START_REG,0,0,0,0,0);
  (void)REG_InitVarPI(VoF_REG,0,0,0,0,0);
  (void)REG_InitVarPI(TOD_REG,0,0,0,0,0);
  #ifdef WM_MANAGEMENT
    (void)REG_InitVarPI(WM_REG,0,0,0,0,0);
  #endif  /* WM_MANAGEMENT */
  (void)REG_InitVarPI(MAX_CUR_REG,0,0,0,0,0);
  (void)REG_InitVarPI(DELTA_MAX_REG,0,0,0,0,0);
  (void)REG_InitVarPI(PMAX_REG,0,0,0,0,0);
  (void)REG_InitVarPI(TEMP_REG,0,0,0,0,0);

  /* Inizializzazione parametri filtri. */
  (void)FIR_SetParFilt(SIGMA_FILT,SMOOTH_RAMP_INT_STEP_FREQ,SMOOTH_RAMP_FILT_FREQ_CONST,SMOOTH_RAMP_FILT_FREQ_CONST,SMOOTH_RAMP_FILT_HYST_PU,0);
  (void)FIR_SetParFilt(PBATT_FILT,(u16)((u16)UPDATE_LOOP_FREQ*FILTER_RES_FACT),PBATT_FILT_POLE_FREQ_CONST,PBATT_FILT_ZERO_FREQ_CONST,PBATT_FILT_HYST_WATT_PU,1);
  (void)FIR_SetParFilt(DELTA_FILT,(u16)((u16)UPDATE_LOOP_FREQ*FILTER_RES_FACT),DELTA_FILT_FREQ_CONST,DELTA_FILT_FREQ_CONST,DELTA_FILT_HYST_ANGLE_PU,0);
  (void)FIR_SetParFilt(PHI_FILT,(u16)((u16)UPDATE_LOOP_FREQ*FILTER_RES_FACT),PHI_FILT_FREQ_CONST,PHI_FILT_FREQ_CONST,PHI_FILT_HYST_ANGLE_PU,0);    
  #ifdef DC_LIKE_BEHAVIOUR
   (void)FIR_SetParFilt(IAVG_FILT,IAVG_STEP_FREQ_CONST,IAVG_FILT_FREQ_CONST,IAVG_FILT_FREQ_CONST,IAVG_FILT_HYST_PU,0);
  #endif  /* DC_LIKE_BEHAVIOUR */
  #ifdef OVERCURRENT_ROF
   (void)FIR_SetParFilt(IPK_HIGH_FILT,IPK_HIGH_STEP_FREQ_CONST,IPK_HIGH_FILT_POLE_FREQ_CONST,IPK_HIGH_FILT_ZERO_FREQ_CONST,IPK_HIGH_FILT_HYST_PU,1);
  #endif  /* OVERCURRENT_ROF */
  #ifdef LIN_INPUT
    (void)FIR_SetParFilt(LIN_SPEED_FILT,LIN_SPEED_INT_STEP_FREQ,LIN_SPEED_FREQ_CONST,LIN_SPEED_FREQ_CONST,LIN_SPEED_HYST_PU,0);
  #endif  /* LIN_INPUT */

  /* Inizializzazione variabili filtri. */

  rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();

  (void)FIR_InitVarFilt(SIGMA_FILT,(s16)((u32)((u32)rampGenerator_u16FreqRampPU_tmp>>(u32)SMOOTH_RAMP_PRESCALER_SHIFT)),(s16)((u32)((u32)rampGenerator_u16FreqRampPU_tmp>>(u32)SMOOTH_RAMP_PRESCALER_SHIFT)));
  (void)FIR_InitVarFilt(PBATT_FILT,0,0);
  (void)FIR_InitVarFilt(DELTA_FILT,0,0);
  (void)FIR_InitVarFilt(PHI_FILT,0,0);
  #ifdef OVERCURRENT_ROF
   (void)FIR_InitVarFilt(IPK_HIGH_FILT,0,0);
  #endif  /* OVERCURRENT_ROF */
  #ifdef DC_LIKE_BEHAVIOUR
   (void)FIR_InitVarFilt(IAVG_FILT,0,0);
  #endif  /* DC_LIKE_BEHAVIOUR */
  #ifdef LIN_INPUT
    (void)FIR_InitVarFilt(LIN_SPEED_FILT,(s16)((u16)rampGenerator_u16FreqRampPU_tmp>>LIN_SPEED_PRESCALER_SHIFT),(s16)((u16)rampGenerator_u16FreqRampPU_tmp>>LIN_SPEED_PRESCALER_SHIFT));
  #endif  /* LIN_INPUT */

  /* Inizializzazione timer per step fisso di integrazione. */
  u16UpdateLoopTime = UPDATE_LOOP_TIME_PU;
  u16UpdateLoopTimer = u16UpdateLoopTime;
  BitSet((UPDATE_LOOP), C_D_ISR_Status);  
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void ACM_ManageParking (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em>
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Parking phase management.
                            \par Used Variables
    \ref u16UpdateLoopTime Timer/Counter preset of the delay of time to set the flag to update all the PI regulators and the V/f modulator inputs\n
    \ref u16UpdateLoopTimer Timer/Counter that counts the delay of time to set the flag to update all the PI regulators and the V/f modulator inputs\n
    \ref C_D_ISR_Status \n
    \return \b void No value returned by this function
    \note none
    \warning none
*/ 
void ACM_ManageParking (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  /*-------------------- Frenata ----------------------*/
    TLE_EnableOutputs();                /* Abilitazione del TLE (e del ponte). */
    
    #ifdef GROUND_BALANCE
      u16HwRefTimer = 0;
      BOOLHwRefFlag = TRUE;

      while( u16HwRefTimer < GROUND_BALANCE_TIME_BIT )
      {
        INT0_PWM_FailSafeMonitor();
      
        WTD_ResetWatchdog();
#ifdef _CANTATA_
        u16HwRefTimer = GROUND_BALANCE_TIME_BIT;
#endif
      }
      INT0_PWM_FailSafeMonitor_rst();
    #endif  /* GROUND_BALANCE */

    #ifdef RVP_SW_MANAGEMENT_ENABLE
      (void)RVP_SetRVPOn();
    #endif  /* RVP_SW_MANAGEMENT_ENABLE */

    BRAKING_ON_LOW;                     /* Chiusura MOSFET bassi del ponte per frenata. */

  /* In modalita' TEST si evita la frenata prima del parcheggio. */
    if ( (OperatingMode_t)InputSP_GetOperatingMode() == NORMAL_OPERATION )
    {
      /* Ciclo di durata della frenata. */
      RTI_SetMainTimeBase((u8)PRE_ALIGNMENT_BRAKING_TIME);
      while ((RTI_IsMainTimeElapsed()) == (BOOL)FALSE)
      {
        INT0_FailSafeMonitor();

        #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
          DIAG_ManageDiagnosticToECU(IDLE, NO_ERROR);         /* Chiamata generica per il check del corto-circuito sulla diagnostica. */
        #endif
        WTD_ResetWatchdog();
      }
      INT0_FailSafeMonitor_rst();

      #ifdef LIN_INPUT
        (void)LIN_ExecuteLINProcedure();
      #endif  /* LIN_INPUT */
    }
    else
    {
      /* Nop(); */
    }

  /*-------------------- Parcheggio -------------------*/
    (void)MTC_SettingForParkingPhase();

    Tacho_ResetTachoOverflowNumber();                       /* Reset del Tacho in fase di parcheggio. */
    INTERRUPT_TACHO_ENABLE;
    #ifdef DUAL_LINE_ZERO_CROSSING
      INTERRUPT_BEMF_DISABLE;
    #endif  /* DUAL_LINE_ZERO_CROSSING */

    (void)MTC_ParkAngleSweep();

  #ifdef DC_LIKE_BEHAVIOUR
    /* Inizializzazione filtro corrente di batteria per setpoint blower.*/
    (void)FIR_SetParFilt(IAVG_FILT,IAVG_STEP_FREQ_CONST,IAVG_FILT_FREQ_CONST,IAVG_FILT_FREQ_CONST,IAVG_FILT_HYST_PU,0);
    (void)FIR_InitVarFilt(IAVG_FILT,0,(s16)((s16)Power_Get_structPowerMeas_u16IbattPU()*IAVG_FILT_SCALER_FACT));
  #endif  /* DC_LIKE_BEHAVIOUR */

    /* Inizializzazione timer per step fisso di integrazione. */
    u16UpdateLoopTime = UPDATE_LOOP_TIME_PU;
    u16UpdateLoopTimer = u16UpdateLoopTime;
    BitClear((UPDATE_LOOP), C_D_ISR_Status);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_InitControlStruct (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This routine initializes the main control structures.
    \details This routine initializes \ref strMachine and \ref rampGenerator structures.
    
                                \par Used Variables
    \ref strMachine       Electrical machine \n
    \ref rampGenerator    Frequency-ramp generator \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_InitControlStruct (void)
{
  u16 rampGenerator_u16FreqRampPU_tmp;
  u16 rampGenerator_u16FreqRampSmoothPU_tmp;

  strMachine.u16VoFVoltagePU = 0;
  strMachine.u16VoltagePU    = 0;
  strMachine.u16CurrentPU    = 0;
  strMachine.s16AngleDeltaPU = 0;
  strMachine.s16AnglePhiPU   = 0;

  /* N.B.: u16FreqRampPU e' inizializzata con la frequenza elettrica raggiunta a fine parcheggio */
  /* (cio' per non avere discontinuita' all'avvio della fase di RUN).                            */
  ACM_Set_u16FreqRampPU((u16)((u32)((float32_t)FREQUENCY_RES_FOR_PU_BIT*UPDATE_LOOP_FREQ)/(u32)((u32)BASE_FREQUENCY_HZ*(u16)((u16)MTC_ComputeParkingTime()/POLE_PAIR_NUM))));
  rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();
  ACM_Set_u16FreqRampSmoothPU(rampGenerator_u16FreqRampPU_tmp);

  rampGenerator_u16FreqRampSmoothPU_tmp = ACM_Get_u16FreqRampSmoothPU();
  strMachine.u16FrequencyPU = rampGenerator_u16FreqRampSmoothPU_tmp;
  strMachine.u16FrequencyAppPU = rampGenerator_u16FreqRampSmoothPU_tmp;

  #ifdef BMW_LIN_ENCODING
    u16FrequencyMem = rampGenerator_u16FreqRampSmoothPU_tmp; 
  #endif  /* BMW_LIN_ENCODING */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void ACM_UpdateControlStruct (u16 u16ActualBemf, u16 u16ActualFreq)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This routine updates the main control structures.
    \details This routine updates \ref strMachine and \ref rampGenerator structures.
    
                                   \par Used Variables
    \ref strMachine                 Electrical machine \n
    \ref rampGenerator              Frequency-ramp generator \n
    \param[in] u16ActualFreq        Actual electrical frequency \n
    \param[in] u16ActualBemf        Bemf value @ u16ActualFreq \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void ACM_UpdateControlStruct (u16 u16ActualBemf, u16 u16ActualFreq) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 rampGenerator_u16FreqRampPU_tmp;
  u16 rampGenerator_u16FreqRampSmoothPU_tmp;

  strMachine.u16VoFVoltagePU = u16ActualBemf;

  ACM_Set_u16FreqRampPU(u16ActualFreq);
  rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();
  ACM_Set_u16FreqRampSmoothPU(rampGenerator_u16FreqRampPU_tmp);

  rampGenerator_u16FreqRampSmoothPU_tmp = ACM_Get_u16FreqRampSmoothPU();
  strMachine.u16FrequencyPU = rampGenerator_u16FreqRampSmoothPU_tmp;
  strMachine.u16FrequencyAppPU = rampGenerator_u16FreqRampSmoothPU_tmp;

  #ifdef BMW_LIN_ENCODING
    u16FrequencyMem = rampGenerator_u16FreqRampSmoothPU_tmp; 
  #endif  /* BMW_LIN_ENCODING */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void ACM_DoMotorControl (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Main motor control routine.
    \details This routine manages frequency ramps, tacho computations and PI-loop regulations.
    
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void ACM_DoMotorControl (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 u16InputSP;

  /*-------------------- Aggiornamento setpoint di velocita' ----------------------*/
  if ( (RTI_IsRegPeriodElapsed()) != 0u )                                             /* Base-tempi 8 ms. */
  {
    u16InputSP = (u16)ACM_SetFrequencySetPoint((u8)InputSP_GetInputSetPoint());       /* Setpoint di velocita' da centralina.              */
    (void)ACM_DoFrequencyRampPU(u16InputSP);                                          /* Scalatura interna setpoint e generatore di rampa. */

    /*(void)ACM_SetVoltageVoverF();*/

    (void)MaxPower_PowerSetPoint();

    (void)ACM_ManagePulseCanc();
  }
  else
  {
    /* Nop(); */
  }

  /*-------------------- Aggiornamento angoli elettrici ---------------------------*/
  if ( (BitTest((MANAGE_TACHO), C_D_ISR_Status)) != 0u )      /* Zero-crossing di corrente. */
  {
    BitClear((MANAGE_TACHO),C_D_ISR_Status);                  /* Reset flag ZC.     */
    (void)Tacho_ManageTachoCapture();                         /* Misura angolo Phi. */
    (void)ACM_ComputeDeltaAngle();
  }
  else
  {
    /* Nop(); */
  }

  /*-------------------- Aggiornamento anelli di controllo ------------------------*/

  if ( (BitTest((UPDATE_LOOP), C_D_ISR_Status)) != 0u )       /* Step fisso di integrazione. */
  {
      BitClear((UPDATE_LOOP), C_D_ISR_Status);                /* Reset flag di aggiornameto controllo. */

    /*-------------------- Aggancio al volo per over-current ----------------------*/

    #ifdef OVERCURRENT_ROF
      (void)ACM_ManageOvercurrentRoF();                       /* Gestione aggancio al volo per over-current (se attivo). */
    #endif  /* OVERCURRENT_ROF */

    /*-------------- Aggiornamento angolo Delta, Vbatt,Ibatt e Pbatt --------------*/
      (void)Power_UpdatePowerStruct();              /* Per Pmax e TOD. */

    /*-------------------- Aggiornamento frequenza applicata ----------------------*/

      (void)ACM_WindMillLoop();                     /* Anello di Wind-mill.                                                                                                            */
      (void)ACM_DeltaLimiterLoop();                 /* Anello di Delta-limiter.                                                                                                        */
      (void)ACM_MaxPbattLoop();                     /* Anello di massima potenza.                                                                                                      */
      (void)ACM_SetVoltageVoverF();                 /* N.B.: chiamato ora qui, affinche' il contributo dei vari anelli (escluso max Ipk) passi per la rampa di V/f. Cio' ha dimostrato */
                                                    /* essere una strategia piu' stabile (e concettualmente piu' corretta), rispetto alla mera regolazione sulla frequenza.            */
      (void)ACM_MaxIpkLoop();                       /* Anello di massima corrente Ipk.                                                                                                 */
                                                    /* N.B.: posto DOPO la rampa di V/f a causa del rotore bloccato. Tale anello, se agente anche sulla tensione, potrebbe             */
                                                    /* non consentire alla Ipk di salire e oltrepassare la soglia di SOA, nel caso il drive sia realmente bloccato.                    */
      (void)ACM_TODLoop();                          /* Anello di TOD.                                                                                                                  */

    /*-------------------- Aggiornamento tensione applicata -----------------------*/

      (void)ACM_PhaseVoltageLoop();                 /* Anello di tensione applicata. */

    /*-------------------- Aggiornamento modulatore -------------------------------*/
      (void)MTC_UpdateSine(strMachine.u16VoltagePU,strMachine.u16FrequencyAppPU);    /* Passaggio tensione e frequenza da applicare al modulatore. */
  }
  else
  {
    /* Nop(); */
  }
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_DoFrequencyRampPU (u16 u16NewFreqPU)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Frequency-ramp generator.
    \details This routine updates the \ref rampGenerator structure according to the speed setpoint.
    
                                \par Used Variables
    \ref rampGenerator        Frequency-ramp generator \n
    \param[in] u16NewFreqPU   New frequency setpoint \n    
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_DoFrequencyRampPU (u16 u16NewFreqPU)
{
  u16 rampGenerator_u16FreqRampPU_tmp;

  rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();

  if ( u16NewFreqPU > rampGenerator_u16FreqRampPU_tmp )         /* Cambio di setpoint (incremento di velocita'). */
  {
    /* Accelerazione se:                                                      */
    /*   1. nuovo setpoint > (attuale frequenza + singolo step di incremento) */
    /*   2. Derating termico NON attivo                                       */

  #ifdef BATTERY_DEGRADATION
    /* N.B.: se il regolatore PI sulla Ibatt sta lavorando, le rampe di accelerazione vanno inibite. Il caso tipico e' il seguente:           */
    /*                                                                                                                                        */
    /*  1. la tensione di batteria e' molto bassa (i.e. 8.5 - 9 V)                                                                            */
    /*  2. il drive e' inizialmente fermo                                                                                                     */
    /*  3. la ECU ha comandato max velocita'/potenza                                                                                          */
    /*  4. il generatore di rampa e' arrivato alla frequenza richiesta (i.e. la nominale/massima)                                             */
    /*  5. il regolatore sulla Ibatt si e' caricato fino ad un certo punto di lavoro                                                          */
    /*                                                                                                                                        */
    /* In questo contesto il suddetto regolatore ha operato piu' di quanto necessario, a causa della rampa di frequenza, la quale e' evoluta  */
    /* si' fino al setpoint richiesto, ma senza che il motore la seguisse.                                                                    */
    /* Si e' quindi creato un effetto wind-up, che il regolatore riversera' in uscita non appena si effettuera' una decelerazione (ad esempio */
    /* alla velocita' minima).                                                                                                                */
    /* Per questo e' fondamentale inibire le accelerazioni qualora ci si trovi in limitazione di Ibatt.                                       */
    if ( ( Temp_Get_BOOLSOATemperatureCorr() == (BOOL)FALSE ) && ( u16NewFreqPU >= (u16)( rampGenerator_u16FreqRampPU_tmp + FREQ_INC_PU ) ) && ( IbattReg.u16IbattRegOut == 0u ) )
  #else
    if ( ( Temp_Get_BOOLSOATemperatureCorr() == (BOOL)FALSE ) && ( u16NewFreqPU >= (u16)( rampGenerator_u16FreqRampPU_tmp + FREQ_INC_PU ) ) )
  #endif  /* BATTERY_DEGRADATION */
    {
      #ifndef BMW_LIN_ENCODING
        rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU() + FREQ_INC_PU;
        ACM_Set_u16FreqRampPU(rampGenerator_u16FreqRampPU_tmp);
      #else
        if ( (BOOL)LIN_CheckSoftStart() == TRUE )
        {
          rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU() + FREQ_INC_SLOW_PU;
          ACM_Set_u16FreqRampPU(rampGenerator_u16FreqRampPU_tmp);
        }
        else
        {
          rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU() + FREQ_INC_PU;
          ACM_Set_u16FreqRampPU(rampGenerator_u16FreqRampPU_tmp);
        }
      #endif  /* BMW_LIN_ENCODING */

      #ifdef BATTERY_DEGRADATION
        IbattStateMachine.BOOLForceGainFlag = FALSE;
      #endif  /* BATTERY_DEGRADATION */

      #ifdef PULSE_ERASING
        BOOLSteadyStateFlag = FALSE;
      #endif  /* PULSE_ERASING */
    }
    else
    {
      #ifdef BATTERY_DEGRADATION
        IbattStateMachine.BOOLForceGainFlag = FALSE;
      #endif  /* BATTERY_DEGRADATION */
      
      #ifdef PULSE_ERASING
        BOOLSteadyStateFlag = TRUE;
      #endif  /* PULSE_ERASING */
    }
  }
  else if ( u16NewFreqPU < rampGenerator_u16FreqRampPU_tmp )    /* Cambio di setpoint (decremento di velocita') */
  {
    /* Decelerazione se:                                                      */
    /*   1. nuovo setpoint < (attuale frequenza - singolo step di incremento) */
    if ( rampGenerator_u16FreqRampPU_tmp > (u16)( u16NewFreqPU + FREQ_DEC_PU ) )
    {
      rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU() - FREQ_DEC_PU;
      ACM_Set_u16FreqRampPU(rampGenerator_u16FreqRampPU_tmp); 

      #ifdef PULSE_ERASING
        BOOLSteadyStateFlag = FALSE;
      #endif  /* PULSE_ERASING */

      #ifdef BATTERY_DEGRADATION
        IbattStateMachine.BOOLForceGainFlag = TRUE;   /* Con una rampa di decelerazione, al regolatore di Ibatt si impone il    */
      #endif  /* BATTERY_DEGRADATION */               /* set di guadagni a banda larga. Cio' per far scaricare rapidamente e    */
    }                                                 /* totalmente il regolatore stesso, evitando cosi' che rampa di frequenza */
    else                                              /* e contributo sulla Ibatt evolvano con tempistiche talmente differenti  */
    {                                                 /* da far divergere la corrente di fase nel motore.                       */
      #ifdef BATTERY_DEGRADATION
        IbattStateMachine.BOOLForceGainFlag = FALSE;
      #endif  /* BATTERY_DEGRADATION */

      #ifdef PULSE_ERASING
        BOOLSteadyStateFlag = TRUE;
      #endif  /* PULSE_ERASING */
    }
  }
  else
  {
    #ifdef BATTERY_DEGRADATION
      IbattStateMachine.BOOLForceGainFlag = FALSE;
    #endif  /* BATTERY_DEGRADATION */

    #ifdef PULSE_ERASING
      BOOLSteadyStateFlag = TRUE;
    #endif  /* PULSE_ERASING */
  }

  #ifdef SIGMOIDAL_VoF
    /* Filtraggio passa-basso rampa di velocita' (sigmoide). */
    rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();
  	(void)FIR_UpdateFilt(SIGMA_FILT,(s16)((u32)((u32)rampGenerator_u16FreqRampPU_tmp>>(u32)SMOOTH_RAMP_PRESCALER_SHIFT)));
  	ACM_Set_u16FreqRampSmoothPU((u16)((u16)VectVarFilt[SIGMA_FILT].s16NewOutput<<SMOOTH_RAMP_PRESCALER_SHIFT));
  #else
    rampGenerator_u16FreqRampPU_tmp = ACM_Get_u16FreqRampPU();
    ACM_Set_u16FreqRampSmoothPU(rampGenerator_u16FreqRampPU_tmp);
  #endif  /* SIGMOIDAL_VoF */
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_GetMaximumVoltageApp(void)

    \author Ianni Fabrizio   \n <em>Developer</em> 

    \date 09/03/2017
                            \par Starting SW requirement
    "link al requirement"
    \brief Return the maximum phase voltage.
                            \par Used Variables
    \ref buffu16ADC_READING[V_BUS_CONV]     Vmon acquired by A/D converter        \n
    \ref u16VbattAvg                        Low-pass filtered Vmon                \n
    \ref BOOLDisableVdcComp                 Flag to disable Vbatt compensation    \n
    \return \b  u16 PU-maximum phase voltage.
    \note none
    \warning none
*/
static u16 ACM_GetMaximumVoltageApp(void)
{
  u16 u16VoltageAppMaxPU;

  /* Saturazione Vapp quando si e' "a pacco", in base alla Vbatt (o Vmon) attuale */
  u16VoltageAppMaxPU = (u16)((u32)((u32)MAX_PHASE_VOLTAGE_OVERMOD_PU*buffu16ADC_READING[V_BUS_CONV])/V_BUS_REFERENCE_BIT);
  
  return((u16)u16VoltageAppMaxPU);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_SetVoltageVoverF (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute the phase-voltage main ramp.
    \details This routine computes the feed-forward component of phase-voltage as the bemf actual value.
    
                                        \par Used Variables
    \ref strMachine.u16VoFVoltagePU      V/f voltage component \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_SetVoltageVoverF (void)
{
  /* Feed-forward V/f. */
  #ifdef BATTERY_DEGRADATION
    /* L'azione del regolatore sulla Vbatt deve tradursi anche in un contributo di tensione. Agendo solo sulla frequenza, il controllo    */
    /* complessivo potrebbe risultare instabile. Per questo il delta-frequenza del regolatore e' tramutato in un termine di feed-forrward */
    /* sulla rampa V/f.                                                                                                                   */
    strMachine.u16VoFVoltagePU = (u16)((u32)((u32)((u32)strMachine.u16FrequencyPU-IbattReg.u16IbattRegOut)*K_VoF_PU_SCALED)>>K_VoF_RES_FOR_PU_SHIFT);
  #else
    strMachine.u16VoFVoltagePU = (u16)((u32)((u32)strMachine.u16FrequencyPU*K_VoF_PU_SCALED)>>K_VoF_RES_FOR_PU_SHIFT);    
  #endif  /* BATTERY_DEGRADATION */

  /* Compensazione temperatura su bemf. */
  strMachine.u16VoFVoltagePU = (u16)((u32)((u32)strMachine.u16VoFVoltagePU*(u32)((s32)((s32)BEMF_TEMP_CORR_OFFSET_PU-(s32)INT0_Get_strTempDrive_s16TempAmbReadPU())))/(u32)BEMF_TEMP_CORR_DIVIDER_PU); /* Taking count of temperature. */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static u16 ACM_RsTempCorr (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Stator resistance vs temperature.
    \details Update the stator resistance according to the ambient temperature.
    
                                \par Used Variables
    \ref strTempDrive.s16TempAmbReadPU        (TLE) ambient temperature \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static u16 ACM_RsTempCorr (void)
{
  return((u16)((u32)((u32)(SERIES_RESISTANCE_PU*(float32_t)1000)*(u32)((s32)((s32)Rs_VS_TEMP_CU_OFFSET_PU + (s32)INT0_Get_strTempDrive_s16TempAmbReadPU())))/(u16)((s16)Rs_VS_TEMP_CU_OFFSET_PU + TEMP_REF_FOR_RS_COMPUTATION_PU)));
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static u16 ACM_ComputeDeltaMax (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Maximum-Delta angle computation.
    \details Estimate the Delta-limiter setpoint as atan(wLs/Rs).
    
                                \par Used Variables
    \ref strMachine.u16FrequencyAppPU        Electrical frequency \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static u16 ACM_ComputeDeltaMax (void)
{
  u32 u32TanDeltaPU;
  u16 u16DeltaMaxPU;

  u32TanDeltaPU = (u32)((u32)((u32)strMachine.u16FrequencyAppPU * (u16)(SERIES_INDUCTANCE_PU*(float32_t)1000))/(u16)ACM_RsTempCorr());

  if ( u32TanDeltaPU < TAN_PU_RES_1ST_SEG )
  {
    u16DeltaMaxPU = (u16)((u16)((u32)((u32)u32TanDeltaPU*ARCTAN_PU_1ST_SEG_COEFF)>>ARCTAN_PU_1ST_SEG_SHIFT)+ ARCTAN_PU_1ST_SEG_OFFSET);
  }
  else if ( u32TanDeltaPU < TAN_PU_RES_2ND_SEG )
  {
    u16DeltaMaxPU = (u16)((u16)((u32)((u32)u32TanDeltaPU*ARCTAN_PU_2ND_SEG_COEFF)>>ARCTAN_PU_2ND_SEG_SHIFT)+ ARCTAN_PU_2ND_SEG_OFFSET);
  }
  else if ( u32TanDeltaPU < TAN_PU_RES_3RD_SEG )
  {
    u16DeltaMaxPU = (u16)((u16)((u32)((u32)u32TanDeltaPU*ARCTAN_PU_3RD_SEG_COEFF)>>ARCTAN_PU_3RD_SEG_SHIFT)+ ARCTAN_PU_3RD_SEG_OFFSET);
  }
  else if ( u32TanDeltaPU < TAN_PU_RES_4TH_SEG )
  {
    u16DeltaMaxPU = (u16)((u16)((u32)((u32)u32TanDeltaPU*ARCTAN_PU_4TH_SEG_COEFF)>>ARCTAN_PU_4TH_SEG_SHIFT)+ ARCTAN_PU_4TH_SEG_OFFSET);
  }
  else
  {
    u16DeltaMaxPU = ARCTAN_PU_5TH_SEG_VALUE;
  }

  u16DeltaMaxPU = (u16)((u32)((u32)u16DeltaMaxPU*DELTA_LIMIT_NUM)/DELTA_LIMIT_DEN);

  return((u16)u16DeltaMaxPU);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_ComputeDeltaAngle (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Estimation of the Delta angle.
    \details Delta angle is estimated as (wL*Ipk/Vapp) every tacho event.
    
                                  \par Used Variables
    \ref strMachine                 Electrical machine \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_ComputeDeltaAngle (void)
{
  u16 u16VoltageMaxPU;      
  u32 u32SinGamma,u32LsVoltage;
  s16 s16GammaAngleNew,s16AnglePhiPU;
  s32 s32VfSenPhi;

  /* Lettura grandezze elettriche di riferimento. */
  u16VoltageMaxPU = (u16)ACM_GetMaximumVoltageApp();
  (void)Curr_GetPeakCurrent();  /* Per aggiornamento strMachine.u16CurrentPU. */

  /* Lettura (filtrata) angolo Phi e clamp a 90 °el, per prevenite eventuali errori di calcolo numerico (i.e. approssimazioni, etc..). */
  s16AnglePhiPU = strMachine.s16AnglePhiPU; /*VectVarFilt[PHI_FILT].s16NewOutput; */

  if ( s16AnglePhiPU >= MAXIMUM_ABSOLUTE_ANGLE_PU_RES )
  {
    s16AnglePhiPU = MAXIMUM_ABSOLUTE_ANGLE_PU_RES;
  }
  else
  {
    /* Nop(); */
  }

/* ******* NUOVO CALCOLO DEL DELTA ******* */
  /* Nel caso di angoli negativi, si sfrutta la simmetria dispari della funzione seno (sin(-x) = -sin(x)). */
  if ( s16AnglePhiPU > 0 )
  {
    s32VfSenPhi = (s32)(SINPU[(u8)((u16)((s16)((s16)s16AnglePhiPU/(s16)64)))]);
  }
  else
  {
    s32VfSenPhi = -((s32)(SINPU[(u8)((u16)((s16)((-(s16)s16AnglePhiPU)/(s16)64)))]));  
  }

  /* Limitazione di Vf*sin(Phi) al massimo valore di Vf. */
  if ( strMachine.u16VoltagePU < u16VoltageMaxPU )
  {
    s32VfSenPhi *= (s32)strMachine.u16VoltagePU;
  }
  else
  {
    s32VfSenPhi *= (s32)u16VoltageMaxPU;
  }

  s32VfSenPhi /= (s32)strMachine.u16VoFVoltagePU;

  /* Calcolo della caduta sulla reattanza induttiva. */
  /* u32LsVoltage = (u32)((u32)((u32)((u32)((u32)414*strMachine.u16CurrentPU)/1000)*u16FrequencyPU)/strMachine.u16VoFVoltagePU); */
  u32LsVoltage = (u32)((u32)((u32)((u32)((u32)(BASE_LS_VOLTAGE_COEFF*(float32_t)1000*SERIES_INDUCTANCE_PU)*strMachine.u16CurrentPU)/(u32)1000)*strMachine.u16FrequencyPU)/strMachine.u16VoFVoltagePU);

  /* sin(Gamma) = (Vf*sin(Phi) - w*Ls*Ipk) / Bemf.                                   */
  /* L'ordine dei termini al numeratore e' fatto in modo da operare con valori >= 0. */
  /* L'eventuale cambio di segno sul calcolo di Gamma lo si fa alla fine.            */

  if ( (s32)s32VfSenPhi > 0 )
  {
    if ( (s32)s32VfSenPhi > (s32)u32LsVoltage )
    {
      u32SinGamma = (u32)((u32)s32VfSenPhi - u32LsVoltage);
    }
    else
    {
      u32SinGamma = (u32)((s32)((s32)u32LsVoltage - s32VfSenPhi));
    }
  }
  else
  {
    u32SinGamma = (u32)((s32)((s32)((s32)s32VfSenPhi - (s32)u32LsVoltage)*(s32)(-1)));
  }

  /* Protezione da eventuali overflow numerici di u32SinGamma, oltre SIN_GAMMA_MAX_VALUE_BIT (i.e. Gamma oltre 90 gradi ?!) */
  if ( u32SinGamma > SIN_GAMMA_MAX_VALUE_BIT )
  {
    u32SinGamma = SIN_GAMMA_MAX_VALUE_BIT;
  }
  else
  {
    /* Nop(); */
  }

  /* Calcolo arcsin(Gamma) tramite interpolazione di spezzate approssimanti la funzione arcoseno. */
  if ( u32SinGamma <= SIN_LIMIT_VALUE_1ST_SEG )
  {
    s16GammaAngleNew = (s16)((s32)((s32)((s32)u32SinGamma * ARCSIN_1ST_SEG_COEFF)/ARCSIN_SEGMENT_DIVIDER) - ARCSIN_1ST_SEG_OFFSET);
  }
  else if ( u32SinGamma <= SIN_LIMIT_VALUE_2ND_SEG )
  {
    s16GammaAngleNew = (s16)((s32)((s32)((s32)u32SinGamma * ARCSIN_2ND_SEG_COEFF)/ARCSIN_SEGMENT_DIVIDER) - ARCSIN_2ND_SEG_OFFSET);
  }
  else if ( u32SinGamma <= SIN_LIMIT_VALUE_3RD_SEG )
  {
    s16GammaAngleNew = (s16)((s32)((s32)((s32)u32SinGamma * ARCSIN_3RD_SEG_COEFF)/ARCSIN_SEGMENT_DIVIDER) - ARCSIN_3RD_SEG_OFFSET);
  }
  else if ( u32SinGamma <= SIN_LIMIT_VALUE_4TH_SEG )
  {
    s16GammaAngleNew = (s16)((s32)((s32)((s32)u32SinGamma * ARCSIN_4TH_SEG_COEFF)/ARCSIN_SEGMENT_DIVIDER) - ARCSIN_4TH_SEG_OFFSET);
  }
  else if ( u32SinGamma <= SIN_LIMIT_VALUE_5TH_SEG )
  {
    s16GammaAngleNew = (s16)((s32)((s32)((s32)u32SinGamma * ARCSIN_5TH_SEG_COEFF)/ARCSIN_SEGMENT_DIVIDER) - ARCSIN_5TH_SEG_OFFSET);
  }
  #ifndef _CANTATA_
  else if ( u32SinGamma <= SIN_LIMIT_VALUE_6TH_SEG ) /*PRQA S 2991,2995 #This is due to the fact that the values of the two defines SIN_LIMIT_VALUE_6TH_SEG and SIN_GAMMA_MAX_VALUE_BIT are equal in this project */ /* ATTENZIONE !! */
  {
    s16GammaAngleNew = (s16)((s32)((s32)((s32)u32SinGamma * ARCSIN_6TH_SEG_COEFF)/ARCSIN_SEGMENT_DIVIDER) - ARCSIN_6TH_SEG_OFFSET);
  }
  else
  {
    /* Nop */
  }
  #else
  else
  {
    s16GammaAngleNew = (s16)((s32)((s32)((s32)u32SinGamma * ARCSIN_6TH_SEG_COEFF)/ARCSIN_SEGMENT_DIVIDER) - ARCSIN_6TH_SEG_OFFSET);
  }
  #endif

  /* Convenzione sui segni: il Gamma calcolato dal suddetto approccio e' di segno opposto a quello normalmente considerato nel fw SPAL. */
  /* Occorre quindi un semplice cambio di segno.                                                                                        */
  s16GammaAngleNew *= (s16)(-1);

  /* Avendo per comodita' numerica operato sul modulo di (Vf*sin(Phi) - w*Ls*Ipk), qui occorre considerare l'effettivo segno di tale quantita'. */
  if ( ( (s32)s32VfSenPhi <= 0) || ( /*( (s32)s32VfSenPhi > 0) &&*/ ( (s32)s32VfSenPhi < (s32)u32LsVoltage ) ) ) /* ATTENZIONE !! */
  {
    s16GammaAngleNew *= (s16)(-1);
  }
  else
  {
    /* Nop(); */
  }

  /* L'angolo delta e' determinato a partire dalla suddetta stima del Gamma e dalla misura di Phi. */
  strMachine.s16AngleDeltaPU = (s16)((s16)s16AnglePhiPU + s16GammaAngleNew);

/*
  u16 u16VoltageAppMax;
  u16VoltageAppMax = (u16)((u32)((u32)MAX_PHASE_VOLTAGE_PU*buffu16ADC_READING[V_BUS_CONV])/V_BUS_REFERENCE_BIT);
  (void)Curr_GetPeakCurrent();  // Per aggiornamento strMachine.u16CurrentPU.

  if ( strMachine.u16VoltagePU > u16VoltageAppMax )
  {
    strMachine.s16AngleDeltaPU = (u16)((u32)((u32)((u32)(SERIES_INDUCTANCE_PU*1000) * strMachine.u16FrequencyPU/u16VoltageAppMax)*strMachine.u16CurrentPU)/(u32)1000);
  }
  else
  {
    strMachine.s16AngleDeltaPU = (u16)((u32)((u32)((u32)(SERIES_INDUCTANCE_PU*1000) * strMachine.u16FrequencyPU/strMachine.u16VoltagePU)*strMachine.u16CurrentPU)/(u32)1000);
  }
*/
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_WindMillLoop (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Routine for Wind-mill managing.
    
                                \par Used Variables
    \ref strMachine             Electrical machine \n
    \ref VectVarFilt[PHI_FILT]  Phi-angle Low-pass filter \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_WindMillLoop (void)
{
  #ifdef WM_MANAGEMENT
    u16 u16IntGain,u16PropGain;
  #endif  /* WM_MANAGEMENT */

  (void)FIR_UpdateFilt(PHI_FILT,(s16)strMachine.s16AnglePhiPU);   /* Phi filtrato va usato nel calcolo del Gamma, quindi questa istruzione non va sotto #ifdef! */

  #ifdef WM_MANAGEMENT
    #ifdef BATTERY_DEGRADATION
      /* Il Wind-mill e' inibito quando il regolatore sulla Ibatt e' attivo OPPURE si rilevano delle variazioni a gradino sulla Vbatt. */
      /* Cio' per:                                                                                                                     */
      /*  1. evitare wind-up con entrambi i regolatori attivi (i.e. il regolatore sulla Ibatt integrerebbe "invano", se la procedura   */
      /*   di Wind-mill tendesse ad accelerare il drive).                                                                              */
      /*  2. garantire maggiore stabilita' con gradini di tensione sulla Vbatt (specie quando si filtra la compensazione di batteria e */
      /*   l'angolo Phi tende ad oscillare attorno al valore limite, se si e' ad esempio a minima velcota').                           */
      if ( ( IbattReg.u16IbattRegOut == 0u ) && ( IbattStateMachine.BOOLChangeVbattComp == (BOOL)FALSE ) )
      {
    #endif  /* BATTERY_DEGRADATION */
        /* N.B.: l'anello di Wind-mill e' inserito quando la rampa supera la velcita' minima (e quindi e' sicuro che la frequenza post-sigmoide abbia raggiunto il target). */
        /* Cio' e' utile per non innescare erroneamente tale routine durante la regolazione di corrente, a bassi giri. In tale condizione, infatti, si e' talmente          */
        /* svattati che il loop in questione potrebbe essere erroneamente attivato (specie in alta temperatura, con carichi minori) e far stallare il motore                */
        /* durante la partenza da fermo.                                                                                                                                    */
        if ( ACM_Get_u16FreqRampPU() >= EL_FREQ_MIN_PU_RES_BIT )
        {
          (void)REG_UpdateInputPI(WM_REG,(s16)VectVarFilt[PHI_FILT].s16NewOutput,(s16)WM_PHI_ANGLE_LIM_PU);

          u16IntGain = (u16)((u32)((u32)strMachine.u16VoltagePU*INT_REG_GAIN_WM_PU)/strMachine.u16FrequencyPU);
          u16PropGain = (u16)((u32)((u32)strMachine.u16VoltagePU*PROP_REG_GAIN_WM_PU)/strMachine.u16FrequencyPU);

          (void)REG_SetParPI(WM_REG,u16IntGain,u16PropGain,0,WM_REG_LIM_HIGH_PU,FALSE,1);
          (void)REG_UpdateRegPI(WM_REG);

          if ( (ACM_CheckWindmillRecovery()) != 0u )
          {
            Main_SetBit_u16ErrorType(WINDMILL_FLAG);
          }
          else
          {
            Main_ClrBit_u16ErrorType(WINDMILL_FLAG);
          }
        }
        else
        {
          /* Nop(); */
        }
    #ifdef BATTERY_DEGRADATION
      }
      else
      {
        /* Nop(); */
      }
    #endif  /* BATTERY_DEGRADATION */

    strMachine.u16FrequencyPU = (u16)((u16)ACM_Get_u16FreqRampSmoothPU() + (u16)((u32)VectVarPI[WM_REG].s32RegOutput>>WM_REG_COMPUTATION_RES_SHIFT));

  #else
    strMachine.u16FrequencyPU = ACM_Get_u16FreqRampSmoothPU();
  #endif  /* WM_MANAGEMENT */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_DeltaLimiterLoop (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Routine for Delta-limiter managing.
    
                                \par Used Variables
    \ref strMachine               Electrical machine \n
    \ref VectVarFilt[DELTA_FILT]  Delta-angle Low-pass filter \n    
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_DeltaLimiterLoop (void)
{
  u16 u16IntGain,u16VoltageMaxPU;
  u16 u16VbattAvg_tmp1; /* used for the volatile variable copy */

  u16VoltageMaxPU = ACM_GetMaximumVoltageApp();  
  u16IntGain = (u16)((u32)((u32)strMachine.u16VoltagePU*DELTA_REG_GAIN_PU)/strMachine.u16FrequencyPU);
  
  (void)REG_SetParPI(DELTA_MAX_REG,u16IntGain,0,0,DELTA_REG_LIM_HIGH_PU,FALSE,1);
     
  (void)FIR_UpdateFilt(DELTA_FILT,(s16)strMachine.s16AngleDeltaPU);

  #ifdef BATTERY_DEGRADATION    
    u16VbattAvg_tmp1 = u16VbattAvg;
    if ( ( IbattReg.u16IbattRegOut == 0u ) && ( u16VbattAvg_tmp1 > IBATT_VS_VMON_HIGH_LIMIT_BIT ) )
    {
  #endif  /* BATTERY_DEGRADATION */
      if ( ACM_Get_u16FreqRampPU() >= EL_FREQ_MIN_PU_RES_BIT )
      {                                                               /* N.B.: il Delta-limiter ha senso innescarlo solo al di sopra della velocita' minima e,in particolare, */
        if ( strMachine.u16VoltagePU >= u16VoltageMaxPU )             /* solo quando la tensione e' a pacco (condizione doppia, cosi' da essere sicuri di avere tale          */
        {                                                             /* contributo solo quando serve e non in fasi delicate, come ad esempio lo startup regolato in          */
                                                                      /* corrente, con misure d'angolo ancora non ben consolidate a bassissimi giri).                         */

          (void)REG_UpdateInputPI(DELTA_MAX_REG,(s16)VectVarFilt[DELTA_FILT].s16NewOutput,(s16)ACM_ComputeDeltaMax());
        }
        else
        {
          (void)REG_UpdateInputPI(DELTA_MAX_REG,(s16)VectVarFilt[DELTA_FILT].s16NewOutput,(s16)S16_MAX);
        }

        (void)REG_UpdateRegPI(DELTA_MAX_REG);
      }
      else
      {
        /* Nop(); */
      }
  #ifdef BATTERY_DEGRADATION
    }
    else
    {
      /* Nop(); */
    }
  #endif  /* BATTERY_DEGRADATION */

  strMachine.u16FrequencyPU = (u16)((u16)strMachine.u16FrequencyPU - (u16)((u32)VectVarPI[DELTA_MAX_REG].s32RegOutput>>DELTA_REG_COMPUTATION_RES_SHIFT));
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_MaxIpkLoop (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Routine for maximum-Ipk loop managing.
    
                                \par Used Variables
    \ref strMachine               Electrical machine \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_MaxIpkLoop (void)
{
  u16 u16MaxIpkLimit;

  if ( strMachine.u16FrequencyPU < EL_FREQ_MIN_PU_RES_BIT )
  {
    u16MaxIpkLimit = MAX_IPK_LIMIT_PU_LOW;
  }
  else if ( strMachine.u16FrequencyPU >= EL_FREQ_NOMINAL_PU_RES_BIT )
  {
    u16MaxIpkLimit = MAX_IPK_LIMIT_PU_HIGH;
  }
  else
  {
    u16MaxIpkLimit = (u16)((u16)MAX_IPK_LIMIT_PU_LOW + (u16)((u32)((u32)((u32)strMachine.u16FrequencyPU-EL_FREQ_MIN_PU_RES_BIT)*MAX_IPK_LIMIT_PU_COEFF)>>MAX_IPK_LIMIT_SHIFT));
  }

  (void)REG_UpdateInputPI(MAX_CUR_REG,(s16)strMachine.u16CurrentPU,(s16)u16MaxIpkLimit);
  (void)REG_UpdateRegPI(MAX_CUR_REG);

  strMachine.u16FrequencyPU = (u16)((u16)strMachine.u16FrequencyPU - (u16)((u32)VectVarPI[MAX_CUR_REG].s32RegOutput>>MAX_CURR_REG_COMPUTATION_RES_SHIFT));

  #ifdef BMW_LIN_ENCODING
    if ( (s32)VectVarPI[MAX_CUR_REG].s32RegOutput > 0 )
    {
      if ( (s16)((s16)u16FrequencyMem - strMachine.u16FrequencyPU) > EL_FREQ_CURRENT_LIM_FEEDBAKCK_ON_PU_RES_BIT )
      {
        BOOL_Current_Limitation_Active = TRUE;  /* FLAG per segnalare il warning di sovracorrente */
      }
      else
      {
        /* Nop(); */
      }
    }
    else
    {
      BOOL_Current_Limitation_Active = FALSE;
      u16FrequencyMem = strMachine.u16FrequencyPU; 
    }
  #endif  /* BMW_LIN_ENCODING */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_MaxPbattLoop (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Routine for moximum-power loop managing.
    
                                \par Used Variables
    \ref strMachine               Electrical machine \n
    \ref structPowerMeas          Power measures \n
    \ref VectParPI                PI-regulator parameters \n    
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_MaxPbattLoop (void)
{
  (void)REG_UpdateInputPI(PMAX_REG,(s16)MaxPower_GetPowerSetPoint(),(s16)((s32)((u32)(Power_Get_structPowerMeas_u32PbattPU()/(u32)VOLTAGE_RES_FOR_PU_BIT))));
  (void)REG_UpdateRegPI(PMAX_REG);

  s16PmaxDeltaFreq = (s16)((s32)((s32)((s32)VectVarPI[PMAX_REG].s32RegOutput/(s32)CURRENT_RES_FOR_PU_BIT)*(s32)((u32)FREQUENCY_RES_FOR_PU_BIT))/(s32)PMAX_REG_COMPUTATION_RES_BIT); 
  strMachine.u16FrequencyPU = (u16)((u32)((s32)(((s32)((u32)strMachine.u16FrequencyPU)) + (s32)s16PmaxDeltaFreq)));

  /* Se il derating e' attivo e la velocita' tende a scendere sotto il valor minimo, allora: */
  if ( Temp_Get_BOOLSOATemperatureCorr() != (BOOL)FALSE )
  {
    if ( strMachine.u16FrequencyPU <= EL_FREQ_MIN_PU_RES_BIT )
    {
      strMachine.u16FrequencyPU = EL_FREQ_MIN_PU_RES_BIT;   /* - si satura il setpoint di velocita' al valor minimo;                                */
      VectParPI[TEMP_REG].BOOLFreeze = TRUE;                /* - si congela l'integrativa di temperatura;                                           */
      VectParPI[PMAX_REG].BOOLFreeze = TRUE;                /* - si congela l'integrativa di potenza.                                               */
    }                                                       /* N.B.: e' inutile far evolvere TEMP_REG e PMAX_REG se di piu' non si puo' rallentare. */
    else                                                    /* Cio' produrrebbe solo wind-up!!                                                      */
    {
      /* Nop(); */
    }
  }
  else
  {
    /* Nop(); */
  }
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_TODLoop (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Routine for Torsional Oscillation Damping loop.
    
                                  \par Used Variables
    \ref strMachine                 Electrical machine \n
    \ref structPowerMeas            Power measures \n
    \ref VectVarFilt[PBATT_FILT]    High-pass Pbatt filter \n    
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_TODLoop (void)
{
  u16 u16PropGain;

  (void)FIR_UpdateFilt(PBATT_FILT,(s16)((s32)((u32)((u32)Power_Get_structPowerMeas_u32PbattPU()>>(u8)VOLTAGE_RES_FOR_PU_SHIFT))));  
  (void)REG_UpdateInputPI(TOD_REG,(s16)VectVarFilt[PBATT_FILT].s16NewOutput,0);

  if ( strMachine.u16FrequencyPU < EL_FREQ_NOMINAL_TOD_PU_RES_BIT )
  {
    u16PropGain = (u16)((u32)((u32)((u32)((u32)TOD_REG_GAIN_MIN_SPEED_PU*strMachine.u16FrequencyPU)/EL_FREQ_NOMINAL_TOD_PU_RES_BIT)*strMachine.u16FrequencyPU)/EL_FREQ_NOMINAL_TOD_PU_RES_BIT);
  }
  else if ( strMachine.u16FrequencyPU < EL_FREQ_MIN_PU_RES_BIT )
  {
    u16PropGain = TOD_REG_GAIN_MIN_SPEED_PU;
  }
  else if ( strMachine.u16FrequencyPU >= EL_FREQ_NOMINAL_PU_RES_BIT )
  {
    u16PropGain = TOD_REG_GAIN_MAX_SPEED_PU;
  }
  else
  {
    u16PropGain = (u16)((u32)((u32)TOD_REG_GAIN_MIN_SPEED_PU*EL_FREQ_MIN_PU_RES_BIT)/strMachine.u16FrequencyPU);
  }

  (void)REG_SetParPI(TOD_REG,0,u16PropGain,TOD_REG_LIM_LOW_PU,TOD_REG_LIM_HIGH_PU,FALSE,1);
  (void)REG_UpdateRegPI(TOD_REG);

  /* N.B.: l'utilizzo di strMachine.u16FrequencyAppPU, oltre a strMachine.u16FrequencyPU, permette di avere la frequenza elettrica */
  /* globale aggiornata solo alla fine dell'intero loop di regolazione. Cio' e' conveniente in diversi contesti.                   */
  /* Ad esempio, con la modalita' BATTERYLESS attiva, fattorizzare nell'interrupt di PWM u16FrequencyAppPU mentre altri anelli ci  */
  /* stanno lavorando sopra (i.e. Delta-limiter, Pmax, etc..), genererebbe instabilita' sul profilo della corrente di fase.        */
  /* Sfruttando u16FrequencyAppPU cio' non accade, poiche' si fattorizza l'ultima informazione di frequenza valida e non si va a   */
  /* sovrascrivere di continuo l'azione di uno o piu' anelli di regolazione.                                                       */
  strMachine.u16FrequencyAppPU = (u16)((u32)((s32)((s32)((u32)strMachine.u16FrequencyPU) - (s32)((s32)((s32)((s32)VectVarPI[TOD_REG].s32RegOutput/(s32)CURRENT_RES_FOR_PU_BIT)*(s32)FREQUENCY_RES_FOR_PU_BIT)/(s32)TOD_REG_COMPUTATION_RES_BIT))));
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn static void ACM_PhaseVoltageLoop (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Routine for phase-voltage loop managing.
    
                                  \par Used Variables
    \ref strMachine                 Electrical machine \n
    \ref VectVarFilt[DELTA_FILT]    Low-pass Delta-angle filter \n
    \ref VectVarFilt[PHI_FILT]      Low-pass Phi-angle filter \n
    \ref VectVarPI                  PI-regulator variables \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void ACM_PhaseVoltageLoop (void)
{
  u16 u16IntGain,u16PropGain,u16VoltageAppMax;
  s16 s16GammaAnglePU;

  if ( ACM_Get_u16FreqRampPU() < EL_FREQ_MIN_PU_RES_BIT )
  {
    (void)REG_SetParPI(START_REG,INT_REG_GAIN_START_PU,PROP_REG_GAIN_START_PU,0,VoF_REG_LIM_HIGH_PU,FALSE,1);
    (void)REG_UpdateInputPI(START_REG,(s16)CURRENT_REGULATION_REF_PU,(s16)((s32)((u32)((u32)Curr_GetCurrent()*(u32)BASE_CURRENT_CONV_FACT_RES))));
    (void)REG_UpdateRegPI(START_REG);
    strMachine.u16VoltagePU = (u16)((u32)((s32)((s32)((u32)strMachine.u16VoFVoltagePU) + (s32)((s32)VectVarPI[START_REG].s32RegOutput/(s32)START_REG_COMPUTATION_RES_BIT))));
  }
  else
  {
    (void)REG_SetParPI(START_REG,INT_REG_GAIN_START_PU_OFF,PROP_REG_GAIN_START_PU_OFF,0,VoF_REG_LIM_HIGH_PU,FALSE,1);

    if ( VectVarPI[WM_REG].s32RegOutput != 0 )
    {
      (void)REG_UpdateInputPI(START_REG,(s16)MIN_PEAK_CURR_WM_PU,(s16)strMachine.u16CurrentPU);
    }
    else
    {
      (void)REG_UpdateInputPI(START_REG,(s16)MIN_PEAK_CURR_PU,(s16)strMachine.u16CurrentPU);
    }

    (void)REG_UpdateRegPI(START_REG);
    strMachine.u16VoltagePU = (u16)((u32)((s32)((s32)((u32)strMachine.u16VoFVoltagePU) + (s32)((s32)VectVarPI[START_REG].s32RegOutput/(s32)START_REG_COMPUTATION_RES_BIT_OFF))));
  }

  #ifdef BATTERY_DEGRADATION
    /* Il regolatore di tensione e' inibito quando il regolatore sulla Ibatt e' attivo OPPURE si rilevano delle variazioni a gradino sulla Vbatt. */
    /* Cio' perche':                                                                                                                              */
    /*  1. il contributo di regolazione sulla Ibatt opera con tempistiche nettamente superiori a quelle del PI sulla tensione di fase.            */
    /*    La "reazione" di quest'ultimo al delta-frequenza, imposto per limitare Ibatt nell'interrupt di PWM, tenderebbe a far divergere          */
    /*    la corrente nel motore e quindi a far stallare il drive.                                                                                */
    /*    N.B.: il punto 1 + il riporto in feed-forward del contributo del regolatore su Ibatt, consentono di evitare il calcolo di un            */
    /*    V/f equivalente dentro l'interrupt di PWM, cosa particolarmente onerosa in termini di tempi di calcolo.                                 */
    /*  2. si riesce garantire maggiore stabilita' con gradini di tensione sulla Vbatt.                                                           */
    if ( ( IbattReg.u16IbattRegOut == 0u ) && ( IbattStateMachine.BOOLChangeVbattComp == (BOOL)FALSE ) )
    {
  #endif  /* BATTERY_DEGRADATION */

      if ( strMachine.u16FrequencyPU >= EL_FREQ_MIN_PU_RES_BIT )
      {
        if ( VectVarPI[START_REG].s32RegOutput == 0 )
        {
          u16IntGain = (u16)((u32)((u32)strMachine.u16VoltagePU*INT_REG_GAIN_VoF_PU)/strMachine.u16FrequencyPU);
          u16PropGain = (u16)((u32)((u32)strMachine.u16VoltagePU*PROP_REG_GAIN_VoF_PU)/strMachine.u16FrequencyPU);

          if( (u16)MTC_GetScaledVoltage() > VAPP_MAX_LINEAR_MOD_BIT )
          {
            u16IntGain = (u16)((u16)u16IntGain<<VAPP_OVERMOD_GAINSHIFT);   /* <=> *32 */
            u16PropGain = (u16)((u16)u16PropGain<<VAPP_OVERMOD_GAINSHIFT); /* <=> *32 */
          }
          else
          {
            /* Nop(); */
          }

          /* Calcolo angolo Gamma (con contributo anello minima corrente). */
          s16GammaAnglePU = (s16)((s16)strMachine.s16AngleDeltaPU - strMachine.s16AnglePhiPU);
          (void)REG_SetParPI(VoF_REG,u16IntGain,u16PropGain,VoF_REG_LIM_LOW_PU,VoF_REG_LIM_HIGH_PU,VectParPI[VoF_REG].BOOLFreeze,1);
          (void)REG_UpdateInputPI(VoF_REG,(s16)0,(s16)(-s16GammaAnglePU));  /* -(s16GammaAnglePU) per avere nel regolatore l'errore = angolo Gamma !! */
          (void)REG_UpdateRegPI(VoF_REG);
        }
        else
        {
          /* Nop(); */
        }
      }
      else
      {
        /* Nop(); */
      }

      /*(void)REG_UpdateRegPI(VoF_REG);*/

  #ifdef BATTERY_DEGRADATION
    }
    else
    {
      /* Nop(); */
    }
  #endif  /* BATTERY_DEGRADATION */

  /* Calcolo tensione applicata. */
  strMachine.u16VoltagePU = (u16)((u32)((s32)((s32)((u32)strMachine.u16VoltagePU) + (s32)((s32)VectVarPI[VoF_REG].s32RegOutput/(s32)VoF_REG_COMPUTATION_RES_BIT))));

  /* Controllo limiti di tensione applicata (complessiva, cioe' VoF + integrativa). */
  u16VoltageAppMax = (u16)ACM_GetMaximumVoltageApp();    /* Tensione massima applicabile, tenendo conto dell'effetiva Vdc!! */

  if ( strMachine.u16VoltagePU < strMachine.u16VoFVoltagePU )
  {
    strMachine.u16VoltagePU = strMachine.u16VoFVoltagePU;
    VectParPI[VoF_REG].BOOLFreeze = TRUE;
  }
  else if ( strMachine.u16VoltagePU > u16VoltageAppMax )
  {
    VectParPI[VoF_REG].BOOLFreeze = TRUE;   /* Da qui il regolatore VoF non deve piu' integrare, ma u16VoltagePU deve poter evolvere grazie al contributo */
  }                                         /* di u16VoFVoltagePU, cosi' da poter eventualmente avere u16SineMagScaled == SINEMAGSCALED_MAX in piena      */
  else                                      /* sovramodulazione.                                                                                          */
  {
    VectParPI[VoF_REG].BOOLFreeze = FALSE;
  }
}


#ifdef WM_MANAGEMENT
/*-----------------------------------------------------------------------------*/
/** 
    \fn static BOOL ACM_CheckWindmillRecovery (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check if the drive can work under Wind-mill.
    \details This function checks if the Wind-mill loop is working fine and if the drive still acts
    as a motor (not as a generator). If it is not, the system will be stopped. 
    
                                  \par Used Variables
    \ref VectVarFilt[PHI_FILT]     Low-pass Phi-angle filter \n
    \return \b BOOL boolean returned from this function
    \note none
    \warning none
*/
#if  defined(_CANTATA_) && !defined(_CANTATA_INTEGR_)
static u8 u8WMerrCounts = 0;
#endif

static BOOL ACM_CheckWindmillRecovery (void)
{
  BOOL ret;
#ifndef _CANTATA_
  static u16 tmp_MTC_GetScaledVoltage;
  static u8 u8WMerrCounts = 0;
#else
  #ifdef _CANTATA_INTEGR_
  #undef static
  static u8 u8WMerrCounts = 0;
  static u16 tmp_MTC_GetScaledVoltage;
  #define static
  #else
  static u16 tmp_MTC_GetScaledVoltage;
  #endif
#endif

  if ( (s16)/*strMachine.s16AnglePhiPU*/VectVarFilt[PHI_FILT].s16NewOutput > (s16)WM_PHI_ANGLE_LIM_PU )
  {
    u8WMerrCounts++;
  }
  else 
  {
    u8WMerrCounts = 0; 
  }

  tmp_MTC_GetScaledVoltage = MTC_GetScaledVoltage();

  if ( ( u8WMerrCounts >= UNDER_MIN_CURR_ERR_CHECK_NUMBER ) && ( (u16)tmp_MTC_GetScaledVoltage >= SINEMAGSCALED_MAX ) )
  {                                                                               /* N.B.                                                                                                                                                               */
    EMR_EmergencyDisablePowerStage();                                             /* During strong bus voltage decreasing, current can have strong transient and can get close to zero since motor can operate into generative zone.                    */
    ret = (BOOL)TRUE;                                                             /* Since current will be under minimum windmill level, windmill loop will turn on and integral windmill correction will be greater then zero.                         */
  }                                                                               /* During transient, voltage is still to maximum value and since windmill output is different from zero,ACM_CheckWindmillRecoverywindmill() function will check error */
  else
  {
    ret = (BOOL)FALSE;
  }
  return ret;
}

#ifdef BMW_LIN_ENCODING
  void ACM_CheckWindmillShutdown (void)
  {
    if ( strMachine.u16FrequencyPU > (u16)((u16)ACM_Get_u16FreqRampSmoothPU() + DELTA_FREQ_FOR_WM_SHUTDOWN_PU_RES_BIT) )
    {
      if ( strMachine.u16FrequencyPU <= MAX_FREQ_FOR_WM_SHUTDOWN_PU_RES_BIT )
      {
        BOOLWmShutDown = TRUE;
      }
      else
      {
        BOOLWmShutDown = FALSE;
      }
    }
    else
    {
      BOOLWmShutDown = FALSE;
    }
  }
#endif  /* BMW_LIN_ENCODING */
#endif  /* WM_MANAGEMENT */


#ifndef DC_LIKE_BEHAVIOUR

static u16 SetFrequencySetPoint (u8 u8InRef)
{
 u16 u16OutRef;

 u16OutRef = (u16)((K_SETPOINT_PU)*(u8InRef));

 return((u16)u16OutRef);
}
/*-----------------------------------------------------------------------------*/
/** 
    \fn static u16 ACM_SetFrequencySetPointFixed (u8 InRef)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute frequency setpoint for axial drive.
    \details This function computes the new target electrical frequency setpoint deriving from
    the customer Transfer Function execution. 
    
                        \par Used Variables
    \param[in]u8InRef    8-bit speed setpoint \n
    \return \b u16 integer returned from this function
    \note none
    \warning none
*/
static u16 ACM_SetFrequencySetPointFixed (u8 u8InRef)
{
  u16 u16FreqSetPU;

  u16FreqSetPU = SetFrequencySetPoint(u8InRef);

  if ( u16FreqSetPU < EL_FREQ_MIN_PU_RES_BIT )
  {
    u16FreqSetPU = EL_FREQ_MIN_PU_RES_BIT;
  }
  else if ( u16FreqSetPU > EL_FREQ_MAX_PU_RES_BIT )
  {
    u16FreqSetPU = EL_FREQ_MAX_PU_RES_BIT;
  }
  else
  {
    /* Nop(); */
  }

  return((u16)u16FreqSetPU);
}

#else /* DC_LIKE_BEHAVIOUR defined */

#ifdef BLOWER_FDT
/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 ACM_BlowerFdT (u8 u8InRef)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute Transfer Function for blower drive.
    \details This function allows to change the original Transfer Function of a blower application,
    according to the airflow requested on the main input duty-cycles. 
    
                        \par Used Variables
    \param[in] u8InRef   8-bit speed setpoint \n
    \return \b u16 integer returned from this function
    \note none
    \warning none
*/
u16 ACM_BlowerFdT (u8 u8InRef)
{
  u16 u16K0;

  if( u8InRef < (u8)(PWM_IN_0) )
  {
    u16K0 = K0_0;
  }
  else if( u8InRef < (u8)(PWM_IN_1) )
  {
    u16K0 = (u16)(((u16)K0_SLOPE_1*(u8InRef-PWM_IN_0))>>8)+K0_0;
  }
  else if( u8InRef < (u8)(PWM_IN_2) )
  {
    u16K0 = (u16)(((u16)K0_SLOPE_2*(u8InRef-PWM_IN_1))>>8)+K0_1;
  }
  else if( u8InRef < (u8)(PWM_IN_3) )
  {
    u16K0 = (u16)(((u16)K0_SLOPE_3*(u8InRef-PWM_IN_2))>>8)+K0_2;
  }
  else if( u8InRef < (u8)(PWM_IN_4) )
  {
    u16K0 = (u16)(((u16)K0_SLOPE_4*(u8InRef-PWM_IN_3))>>8)+K0_3;
  }
  else if( u8InRef < (u8)(PWM_IN_5) )
  {
    u16K0 = (u16)(((u16)K0_SLOPE_5*(u8InRef-PWM_IN_4))>>8)+K0_4;
  }
  else
  {
    u16K0 = K0_5;
  }

  return ( (u16)u16K0);
}

#endif /* BLOWER_FDT */

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 ACM_SetFrequencySetPointDClike (u8 u8InRef)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute frequency setpoint for blower drive.
    \details This function computes the frequency setpoint of a blower drive according to the
    relation setpoint = freq - K*Iavg, i.e. considering the battery current as a feddback on
    the ECU setpoint.
        
                                   \par Used Variables
    \param[in] u8InRef              8-bit speed setpoint \n
    \ref structPowerMeas            Power measures \n
    \ref VectVarFilt[IAVG_FILT]     Ibatt low-pass filter \n    
    \return \b u16 integer returned from this function
    \note none
    \warning none
*/
u16 ACM_SetFrequencySetPointDClike (u8 u8InRef)
{
  u16 u16Iavg;
  u32 u32SetFreq;

  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    (void)FIR_UpdateFilt(IAVG_FILT,(s16)((s16)Power_Get_structPowerMeas_u16IbattPU()*IAVG_FILT_SCALER_FACT));
    u16Iavg = (u16)((s16)VectVarFilt[IAVG_FILT].s16NewOutput/IAVG_FILT_SCALER_FACT);

    #ifdef BLOWER_FDT
      u32SetFreq = (u32)((u32)K0_BLOWER_SET_POINT*ACM_BlowerFdT(u8InRef));
    #else
      u32SetFreq = (u32)((u32)K0_BLOWER_SET_POINT*u8InRef);
    #endif  /* BLOWER_FDT */

    if ( u32SetFreq > (u32)((u32)u16Iavg*K1_BLOWER_SET_POINT_PU) )
    {
      u32SetFreq -= (u32)((u32)u16Iavg*K1_BLOWER_SET_POINT_PU);
    }
    else
    {
      u32SetFreq = K0_MINIMUM_BLOWER_SET_POINT;
    }

    if ( u32SetFreq > EL_FREQ_MAX_PU_RES_BIT )
    {
      u32SetFreq = EL_FREQ_MAX_PU_RES_BIT;
    }
    else if ( u32SetFreq < EL_FREQ_MIN_PU_RES_BIT )
    {
      u32SetFreq = EL_FREQ_MIN_PU_RES_BIT;
    }
  }
  else  /*Balance test and torque test*/
  {
    if ( u8InRef >= SP_MIN_SPEED_VALUE )
    {
      u32SetFreq = (u32)((u32)K1_BALANCE_BLOWER_SET_POINT*(u8)((u8)u8InRef-SP_MIN_SPEED_VALUE));
      u32SetFreq += K2_BALANCE_BLOWER_SET_POINT;
    }
    else
    {
      u32SetFreq = K2_BALANCE_BLOWER_SET_POINT;
    }
  }

  return((u16)u32SetFreq);
}
#endif  /* DC_LIKE_BEHAVIOUR */

#ifdef OVERCURRENT_ROF
/** 
    \fn void ACM_ManageOvercurrentRoF (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 07/08/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Manage Run on Fly for over-current.
        
                           \par Used Variables
    \ref rampGenerator      Frequency-ramp generator \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void ACM_ManageOvercurrentRoF (void)
{
  if( ACM_Get_u16FreqRampSmoothPU() > IPK_TRANSIENT_CONTROL_PU_RES_BIT ) /* ACM_CheckCurrentTransient() computed if electrical frequency is greater then BLOWER_FREE_AIR_MAX_FREQ_PU */
  {
    if( (BOOL)ACM_CheckCurrentTransient() == TRUE )
    {
      Main_SetBit_u16ErrorType(I_TRANSIENT_FLAG);
    }
    else
    {
      /* Nop(); */
    }
  }
  else
  {
    /* Nop(); */
  }
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL ACM_CheckCurrentTransient( void )
    \author Ianni Fabrizio   \n <em>Developer</em> 
    \date 06/03/2017
                            \par Starting SW requirement
    "link al requirement"
    \brief Check peak-current dynamics during load transients.
    \details This function signalizes if peak-phase current derivative gets over \ref IPK_MAX_DELTA_PU, or peak-phase current
    itself gets over \ref IPK_MAX_OVERCURRENT_LIMIT_PU, due to a strong load variation. Then, an over-current RoF is required.
                                   \par Used Variables
    \ref strMachine.u16CurrentPU      Motor-phase current \n
    \ref VectVarFilt[IPK_HIGH_FILT]   Ipk high-pass filter \n    
    \ref u8OverCurrentCount           Number of over-current events                 \n
    \return \b  BOOL boolean value.
    \note none
    \warning none
*/
BOOL ACM_CheckCurrentTransient( void )
{
  static u8 u8OverCurrentCount = 0;
  s16 s16DeltaIpk;

  (void)FIR_UpdateFilt(IPK_HIGH_FILT,(s16)((s16)strMachine.u16CurrentPU*IPK_HIGH_FILT_SCALER_FACT));    /* Aggiornamento filtro passa-alto corrente di picco. */
  s16DeltaIpk = (s16)((s16)VectVarFilt[IPK_HIGH_FILT].s16NewOutput/IPK_HIGH_FILT_SCALER_FACT);          /* Campionamento del filtro passa-alto sulla Ipk.     */

  /* Se la Ipk (assoluta) supera la soglia massima IPK_MAX_OVERCURRENT_LIMIT_BIT, o se la derivata della Ipk supera il delta massimo IPK_MAX_DELTA_BIT... */
  if ( ( strMachine.u16CurrentPU >= IPK_MAX_OVERCURRENT_LIMIT_PU ) || ( s16DeltaIpk >= (s16)IPK_MAX_DELTA_PU ) )
  {
    u8OverCurrentCount++;
    if( u8OverCurrentCount >= IPK_MAX_OVERCURRENT_NUM )   /* ...e cio' accade per IPK_MAX_OVERCURRENT_NUM volte consecutive... */
    {
      u8OverCurrentCount = 0;
      return(TRUE);                                       /* ...allora viene comandato un aggancio al volo.                    */
    }
    else
    {
      /* Nop(); */
    }
  }
  else
  {
    u8OverCurrentCount = 0;                               /* Se non ci sono transitori violenti sulla Ipk (i.e. prese di carico), il contatore di tali eventi rimane azzerato. */
  }

  return(FALSE);
}
#endif /* OVERCURRENT_ROF */

/*-----------------------------------------------------------------------------*/
/**
    \fn static void ACM_Set_u16FreqRampPU(u16 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static rampGenerator.u16FreqRampPU variable
                            \par Used Variables
    \ref rampGenerator.u16FreqRampPU \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void ACM_Set_u16FreqRampPU(u16 value) 
{
    rampGenerator.u16FreqRampPU = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u16 ACM_Get_u16FreqRampPU(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static rampGenerator.u16FreqRampPU variable
                            \par Used Variables
    \ref rampGenerator.u16FreqRampPU \n
    \return \b The value of static rampGenerator.u16FreqRampPU variable.
    \note None.
    \warning None.
*/
#ifndef RVS
static u16 ACM_Get_u16FreqRampPU(void)
#else
 u16 ACM_Get_u16FreqRampPU(void)
#endif
{
    return (rampGenerator.u16FreqRampPU);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void ACM_Set_u16FreqRampSmoothPU(u16 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static rampGenerator.u16FreqRampSmoothPU variable
                            \par Used Variables
    \ref rampGenerator.u16FreqRampSmoothPU \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void ACM_Set_u16FreqRampSmoothPU(u16 value) 
{
    rampGenerator.u16FreqRampSmoothPU = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u16 ACM_Get_u16FreqRampSmoothPU(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static rampGenerator.u16FreqRampSmoothPU variable
                            \par Used Variables
    \ref rampGenerator.u16FreqRampSmoothPU \n
    \return \b The value of static rampGenerator.u16FreqRampSmoothPU variable.
    \note None.
    \warning None.
*/
u16 ACM_Get_u16FreqRampSmoothPU(void)
{
    return (rampGenerator.u16FreqRampSmoothPU);
}

#ifdef PULSE_ERASING
/*-----------------------------------------------------------------------------*/
/**
    \fn static void ACM_ManagePulseCanc(void)
    \author Ianni Fabrizio \n <em>Main Developer</em>

    \date 23/07/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Manage the PWM-pulse cancellation
                            \par Used Variables
    \ref BOOLSteadyStateFlag    \n
    \ref u16VbattAvg            \n
    \ref s16PmaxDeltaFreq       \n
    \ref u8MTCStatus            \n
    \return \b void No value returned from this function
    \note None.
    \warning None.
*/
static void ACM_ManagePulseCanc (void)
{
   if ( BOOLSteadyStateFlag == (BOOL)TRUE )
   {
     if ( u16VbattAvg >= VBATT_PULSE_CANC_ENABLE_BIT )
     {
       if ( s16PmaxDeltaFreq != 0 )
       {
         if ( INT0_Get_strTempDrive_s16TempAmbReadPU() >= PULSE_CANC_MOD_TEMP_HIGH_PU )
         {
           BitSet((PULSE_ERASE), u8MTCStatus);
         }
         else
         {
           if ( (BitTest((PULSE_ERASE), u8MTCStatus)) != (BOOL)FALSE )
           {
             if ( INT0_Get_strTempDrive_s16TempAmbReadPU() <= PULSE_CANC_MOD_TEMP_LOW_PU )
             {
               BitClear((PULSE_ERASE), u8MTCStatus);
             }
             else
             {
               /* Nop(); */
             }
           }
           else
           {
             /* Nop(); */
           }
         }
       }
       else
       {
         BitClear((PULSE_ERASE), u8MTCStatus);
       }
     }
     else
     {
       BitClear((PULSE_ERASE), u8MTCStatus);
     }
   }
   else
   {
     BitClear((PULSE_ERASE), u8MTCStatus);
   }
}
#endif  /* PULSE_ERASING */

/*** (c) 2017 SPAL Automotive ****************** END OF FILE **************/
