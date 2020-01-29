/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  main.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file main.c
    \version see \ref FMW_VERSION
    \brief Main file with high level routines
    \details This file contains the state machine that rules the motor behaviour.
    This is a level 2 files; this implies that all functions have to be Hw independant and
    belongs to an high abstracton level.
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include <hidef.h>
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include "main.h"
#include "TIM1_RTI.h"
#include "MCN1_acmotor.h"              /* it has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_RegPI.h"
#include "MCN1_mtc.h"
#include "SCI.h"
#include "MCN1_Tacho.h"
#include "TLE7184F.h"
#include "COM1_COM.h"
#include MAC0_Register
#include INI0_INIT                     /* Per MTC_CLOCK */
#include "TMP1_Temperature.h"
#include "VBA1_Vbatt.h"
#include "CUR1_Current.h"
#include "PBATT1_Power.h"              /* Per structPowerMeas */
#include ADM0_ADC
#include "EMR0_Emergency.h"
#include "SOA1_SOA.h"
#include "DIA1_Diagnostic.h"
#include "INP1_InputSP.h"
#include "INP1_InputPWM.h"
#include "INP1_InputAN.h"
#include "INP1_Key.h"
#include "CKS1_SpalChecksum.h"
#include "FIR1_FIR.h"
#include "AUTO1_AutoTest.h"
#include "WTD1_Watchdog.h"
#include "TIM1_API.h"
#include "INT0_PWMIsrS12.h"
#include "RVP1_RVP.h"
#include "LOG0_LogDataS12.h"
#include "POWER1_MaxPower.h"           /* Per BOOLPartStallDerFlag (con diagnostica attiva) */
#include "INT0_RTIIsrS12.h"

#ifdef LIN_INPUT
  #include "lin_api.h"
  #include "INT0_PWMIsrS12.h"

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

/* ---------------------------Private functions-------------------------------*/
static void Main_FreePinsInit(void);
static BOOL	Main_CheckPowerStage(void);
static void Main_SetStart(void);
static void Main_Set_u16ErrorType(u16 errTyp);
#if (!defined(ENABLE_RS232_DEBUG) && !defined(BMW_LIN_ENCODING))
static u16  Main_Get_u16ErrorType(void);
#endif
static void Main_Set_u8StartNumber(u8 u8Value);
static void Main_Inc_u8StartNumber(void);

#if ( !defined(RVS) || (defined(RVS) && defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE)) )
static SystStatus_t Main_CheckStartOnFly(void);
#endif  /* !RVS || RVS && REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

static void Main_SetupVariables(void);

#ifdef KEY_SHORT_LOGIC
  static void Main_ManageKeyShort (void);
#endif  /* KEY_SHORT_LOGIC */

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  static void Main_CheckPartialStall (void);
#endif  /* DIAGNOSTIC_STATE */

#if defined(LIN_INPUT)
  void Main_UpdateLinSpeedFeedback (void);
#endif  /* LIN_INPUT */

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  static void Main_Set_u16ErrorTypeMem(u16 u16Value);
#endif  /* DIAGNOSTIC_STATE */

/*----------------------------Private variables ------------------------------*/
/** 
    \var enState
    \brief Current State Machine
    \details This is a defined enumerator that represent the current state of the 
    software machine.
    \note none
    \warning none
    
    \var u16NewFreq
    \brief New Frequency To Achive
    \details This variable contains the value of the new frequency to achieve.
    \note none
    \warning none
    
    \var u16Target_Curr
    \brief Current Target for Parking State
    \details It's the current target needed to be compliant with the StartUp current requirement.
    To assure a standard start up it's needed a parking before start up with a designed current that is calculated
    and stored in this variable.
    \note none
    \warning none
     
    \var u8StartNumber
    \brief Variable used for current start number.
    \note This variable is placed in \b DEFAULT segment.
    \warning None.
    \details None.
     
    \var u16ErrorType
    \brief Type of error got from main.
    \note This variable is placed in \b DEFAULT segment.
    \warning None.
    \details None.        
*/

static SystStatus_t enState; 
#ifdef LIN_INPUT
  u16 u16OldFreq;     /* Vecchio campione di bemf rilevata.                                                                  */
  u16 u16NewFreqTol;  /* Tolleranza calcolata on-line sulla bemf rilevata.                                                   */
  u8 u8BemfMinCount;  /* Contatore del numero di volte in cui la bemf rilevata scende sotto la minima velocita' segnalabile. */
#endif  /* LIN_INPUT */
static u16 u16NewFreq;         /*PRQA S 3218 #Gives better visibility here*/
static u16 u16Target_Curr; /*PRQA S 3218 #Gives better visibility here*/
static u8 u8StartNumber;
static u16 u16ErrorType;

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  static u16 u16ErrorTypeMem;  /* Variabile-memoria del generico errore, per consentire la corretta gestione della linea di diagnostica. */
#endif  /* DIAGNOSTIC_STATE */

#if ( defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG) )
  structDebug strDebug; /* PRQA S 1504 #rcma-1.5.0 warning message: this is a structure of debug variables that could be placed anywhere in the code if necessary */
#endif  /* defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG) */

/* -------------------------- Public variables --------------------------- */

/* -------------------------- Private functions -------------------------- */
#ifndef LIN_INPUT 
  static void (*CheckTestPhase)(void) = /*PRQA S 3218 #Gives better visibility here*/
  #if defined(PWM_INPUT)
    InputPWM_FillBufferAndSetOperating;
  #elif defined(AN_INPUT)
    #if defined(PWM_TO_ANALOG_IN)
      InputPWM_FillBufferAndSetOperating;     /* Call this function to detect the xxx_TEST mode */
    #else
      InputAN_CheckTestPhase;
    #endif  /* PWM_TO_ANALOG_IN */
  #else
    Main_SetupVariables;           /* Just call a dummy function */
  #endif  /* PWM_TO_ANALOG_IN */
#endif  /* LIN_INPUT */

/** 
    \fn void Main_FreePinsInit( void )

    \author	Ruosi Gabriele  \n <em>Reviewer</em> 

    \date 21/02/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initialization of unused pins

    \par No variables used
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void Main_FreePinsInit(void)
{
  #if (defined(NO_EXTERNAL_OSC) && (PCB == PCB262A))

  /* Pin 4  -> PE0/EXTAL            -> Schematic pin name: EXTAL  */
  EXTAL_SET;

  /* Pin 6  -> PE1/XTAL             -> Schematic pin name: XTAL   */
  XTAL_SET;

  #endif

  #if (PCB == PCB258A)

  /* Pin 11 -> PWM2/ETRIG2/KWP2/PP2 -> Schematic pin name: DEBUG1 */
  SET_DEBUG1_OUT;
  DEBUG1_OFF;

  /* Pin 16 -> XIRQ/IOC0/PT0        -> Schematic pin name: N/A    */
  PORT_T0_SET;

  #endif

  /* Pin 21 -> PAD4/KWAD4/AN4       -> Schematic pin name: N/A    */
  INPUTECON_INIT;

  /* Pin 24 -> PAD7/KWAD7/AN7/ACMPM -> Schematic pin name: DEBUG2 */
  SET_DEBUG2_OUT;
  DEBUG2_OFF;

  #ifndef LIN_INPUT

  /* Pin 29 -> PS6/IOC5/SCK0        -> Schematic pin name: N/A    */
  LIN_TRANSCEIVER_OFF;

  #endif
}

/**
    \fn void main(void)

    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 22/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Main function with startup, initializations and the state machine. 
    \details This function resume the whole motor control. It starts with user initialisations
    when the micro boot is complete. As first step all micro devices are initialized to the correct
    state. When all register are correctly written and the needed waiting time is passed, the function
    start the never ending loop [for(;;)] where the state machine is implemented.\n
    Each state define a different motor behaviour:\n
    - \ref IDLE : In idle mode the motor is waiting for running condition and have already performed a initial
    hardware check in order to avoid PCB failures. The micro is in listening mode because it performs continuous
    check to define starting/locking conditions.\n
    - \ref FAULT : If the micro reach this state an important failure has been detected and the programs has
    exhausted all attempts to restart. The system can not exit form this state but it's confined here unless the
    car ECU don't restart all. If a diagnostic output is implemented, a failure signal is provided.\n
    - \ref WAIT : If a minor failure is occurred the micro disable the three phase bridge and set his status in a
    low consumption mode where it wait for a new restart. In this state no operation are done before the time to wait is over;
    the only check performed in polling mode tests the Input PWM signal.\n
    -\ref PARK : This state has been defined to identify a prestart phase done to stop the rotor and place it in a known position.
    This is done to comply to a specific requiement and to ensure a repetitive starting condition.\n
    -\ref RUN_ON_FLY : In some cases, if a residual speed is detected, it's appreciated to avoid rotor blocking and to start driving
    rotor without restarting from zero. If the machine state reachs this state, the control has already detected the Run_On_Fly 
    conditions and this state prepare the software constants to perform the running lock.\n
    -\ref RUN : This state summarize all the running conditions. If some corrections are applied (SOA derating), these are all computed in this state.
    All the control loops are closed and computed after reaching this state.\n
    
     \dot
     digraph StatiMacchina {
          	node [shape=box, style=filled, fontname=Arial, fontsize=10];  
          	RUN_ON_FLY[ label="RUN_ON_FLY", color="yellow"];
          	RUN[ label="RUN", color="green"]; 
          	WAIT[ label="WAIT", color="cyan"];     
          	PARK[ label="PARK", color="yellow"];
          	FAULT[ label="FAULT", color="red",shape=doubleoctagon];       
          	IDLE [ label="IDLE", color="cyan"]; 
          	STOP [ label="STOP", color="orange"];
          	Reset[label="Reset"];
          	Init[label="Initializations"];
          	Low[label="Low Consumption"];

          	Reset->Init[];
          	Init->FAULT  [  label=" Main_AutoTest=TRUE ", fontsize=8, color="red"];
          	Init->IDLE  [  ];
          	
            FAULT->Low [  label=" PwmReadyToSleep", fontsize=8];
          	WAIT->Low [  label=" PwmReadyToSleep", fontsize=8];
          	IDLE->PARK->RUN[ ];
          	IDLE->RUN_ON_FLY->RUN[];
          	RUN->STOP[];
          	STOP->WAIT[];
          	IDLE->WAIT[label="Main_CheckPowerStage = ERROR", fontsize=8];
          	IDLE->FAULT[label=" Main_CheckStartOnFly Failed", fontsize=8, color="red"];
          	WAIT->IDLE[label=" Error Managed", fontsize=8];
          	WAIT->FAULT[label=" Fault Recognized", fontsize=8, color="red"];
          	RUN->WAIT[label=" Main_CheckPowerStage = ERROR", fontsize=8];
          	IDLE->IDLE[label=" Main_CheckStartOnFly evaluating", fontsize=8];
          	RUN_ON_FLY -> RUN_ON_FLY[label=" Waiting Running Concitions", fontsize=8] ;
          	RUN -> RUN[label=" Normal Running", fontsize=8] ;

          	PARK -> PARK [label=" Parking", fontsize=8] ;
          	WAIT -> WAIT[label=" Waiting", fontsize=8] ;
          	STOP -> STOP[label=" Braking", fontsize=8] ;
      }
    \enddot
                            \par Used Variables
    \ref enState Current Machine State \n
    \ref u16NewFreq New Frequency to achive \n
    \ref u16Target_Curr Target current to park the fan \n
    \ref C_D_ISR_Status description \n
    \ref u8StartNumber description \n
    \ref u16ErrorType description \n
    
    \return \b Void No value returned
    \note none
    \warning none                                      
*/
#ifdef BMW_LIN_ENCODING
  V_MEMROM0   V_MEMROM1 vuint8 V_MEMROM2 kLinApplVariantId = 0x01;
#endif  /* BMW_LIN_ENCODING */

/*PRQA S 0601 ++ #The main function doesn't return anything so it has been declared in a non-standard form */
#ifdef _CANTATA_
  void _main(void)
#else
void main(void)
#endif
{
  u16  u16ErrorType_tmp;
  #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
    u16  u16ErrorTypeMem_tmp;
  #endif  /* DIAGNOSTIC_STATE */
  BOOL ret_InputSP_ReadyToSleep;
  BOOL ret_Auto_AutoTestChecks;
  BOOL faultcase = (BOOL)FALSE;
  u8   u8StartNumber_tmp;

  DisableInterrupts; /*PRQA S 1006 #Necessary assembly code*/        /* To disable interrupts enabled by Bootloader */

  TLE7184F_ResetTLEAndForceHiZ();                                    /* Resets TLE7184F and gets it in error mode */

  Init_InitMCU();                                                    /* Initializes micro core by setting and trimming bus clock */

  #ifndef WINDOWED_WATCHDOG
  API_InitAPI();                                                     /* Start API Timer if WINDOWED_WATCHDOG is defined */
  WTD_InitWatchdog();                                                /* Start Watchdog (From here on it must be refreshed !!!!!!!!!!!) */
  #endif

  RTI_START;
  RTI_ENABLE_INTERRUPT;

  Init_InitADC();

  Init_InitTimer();

  Init_InitPWMGenerator();                                           /* PWM peripherals turns on but TLE still in error so NO pwm on MOS appears! */

  ENABLE_PWM_TIMER;                                                  /* N.B. */
  INTERRUPT_PWM_ENABLE;                                              /* Since Load dump is managed in sw way, PWM interrupt has to be keep alway on. */

  TIM_OF_ISR_ENABLE;                                                 /* N.B. */
  TIM_CLK_ENABLE;                                                    /* First of all checking testing phase is required. */
                                                                     /* To detect testing phase, selected pwm input signal could be plugged at analog or pwm input */
                                                                     /* In order to detect pwm input signal following this has to happen: */
                                                                     /* - Init timer (5MHz) as base time to evaluate pwm input duty and frequency. Enable timer (5MHz) overflow and clock as well. */
                                                                     /* - Init and enable pwm bridge isr in order to allow adc multiple convertion. */
  #ifdef RVP_SW_MANAGEMENT_ENABLE
    (void)RVP_InitRVP();
  #endif  /* RVP_SW_MANAGEMENT_ENABLE */

  Vbatt_InitVbattOn();

  #ifdef LIN_INPUT
    (void)INT0_DisablePWMISRComputation();                           /* Chiamata PRIMA di accendere gli interrupt, cosi' da non eseguire subito dopo le istruzioni di pilotaggio del motore nell'ISR di PWM, */
  #endif  /* LIN_INPUT */                                            /* e quindi in modo da permettere al LIN di rispondere alla ECU entro i primi 100 ms di accensione del drive. */

  Main_FreePinsInit();

  Main_SetupVariables();

  #ifndef LIN_INPUT
    #ifdef CHECKSUM_SPAL_ENABLED
      while (CKS_IsROMCheckSumOK()==(BOOL)FALSE){}
      ;
    #endif  /* CHECKSUM_SPAL_ENABLED */
  #endif  /* LIN_INPUT */

  Tacho_InitTachoVariables();                                        /* In order to initializer pstrElectricFreqBuff_Index before INT0_TachoOverflow interrupt                          */
                                                                     /* Interrupts are enabled by Bootloader, so call this function before TIM_OF_ISR_ENABLE                            */
                                                                     /* BUG DESCRIPTION:pstrElectricFreqBuff_Index = 0 before INT0_TachoOverflow                                        */
                                                                     /* (pstrElectricFreqBuff_Index -> ZCVfOvfTemp)++; into INT0_TachoOverflow interrupt causes DIRECT register changes */

  #ifdef WINDOWED_WATCHDOG
  API_InitAPI();                                                     /* Start API Timer if WINDOWED_WATCHDOG is defined                */
  WTD_InitWatchdog();                                                /* Start Watchdog (From here on it must be refreshed !!!!!!!!!!!) */
  #endif

  RTI_Init();
  INT0_FailSafeMonitor_rst();

  MTC_InitSineGen();                                                 /* PWM isr ON with u32SineFreqBuf and amplitude above computed. N.B. PWM OFF! */
  INT0_PWM_FailSafeMonitor_rst();

  MTC_ParkFailSafeMonitor_rst();

  InputSP_InitInput();

  #ifdef  LOG_DATI
    LOG0_InitLogdati();
  #endif  /* LOG_DATI */

  #if ( defined(LIN_INPUT) || defined(LIN_TRANSCEIVER) )
    LIN_TRANSCEIVER_ON;
  #endif  /* defined(LIN_INPUT) || defined(LIN_TRANSCEIVER) */

  #ifdef LIN_INPUT  
    (void)LIN_LoadEEPROMParameters();
    (void)LIN_InitLinVariables();
    (void)LinInitPowerOn();
    (vuint8)LinStart(kLinWakeUpExternal);                            /* Inform LIN driver on wake up event of LIN tranceiver */
  #endif  /* LIN_INPUT */
                                                                     /* Enable interrupt mask. */
  EnableInterrupts; /*PRQA S 1006 #Necessary assembly code*/         /* After these enabling, pwm input (into analog pin as weel) can be checked. */

  #ifndef LIN_INPUT
    CheckTestPhase();                                                /* Checking testing phase: pwm signal is checked at analog or pwm input depending on interface type. */
  #endif  /* LIN_INPUT */

  #ifdef LIN_INPUT
    #ifdef CHECKSUM_SPAL_ENABLED
      while (CKS_IsROMCheckSumOK()==(BOOL)FALSE);
    #endif  /* CHECKSUM_SPAL_ENABLED */

    (void)INT0_EnablePWMISRComputation();                            /* Checksum performed --> enable PWM computation */
  #endif  /* LIN_INPUT */

  Init_InitTacho();
  Tacho_ResetTachoOverflowNumber();

  #ifdef KEY_INPUT
    INP1_InitKey();
  #endif

  #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
    DIAG_InitDiagnostic();
  #endif

  #ifdef ENABLE_RS232_DEBUG
    SCI_Init();
  #endif /* ENABLE_RS232_DEBUG */

  Auto_InitProtections();

  (void)Temp_InitTemp(); /* N.B.: messo qui per avere le temperature TLE/MOS gia' inizializzate (eventualmente) per il LOG_DATI!! */

  ret_InputSP_ReadyToSleep = InputSP_ReadyToSleep();

  while ((Auto_AutoTestChecks() == (BOOL)TRUE) && (ret_InputSP_ReadyToSleep == (BOOL)FALSE ))
  {
    /* N.B.: configurazione di default per u16ErrorType e enState durante il FAULT_ERROR*/
    u16ErrorType = FAULT_ERROR;
    enState = WAIT;

    #ifdef LOG_DATI

      (void)Vbatt_GetBusVoltage();    /* Per aggiornare structPowerMeas.u16VbattPU. */
      (void)Curr_GetPeakCurrent();    /* Per aggiornare strMachine.u16CurrentPU.    */

      #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
        LOG0_LogError(FAULT_ERROR, Power_Get_structPowerMeas_u16VbattPU(), \
           Temp_GetTMos(), strMachine.u16FrequencyAppPU, strMachine.u16CurrentPU);
        LOG0_DoLogData(WAIT);
      #else
        LOG0_LogError(FAULT_ERROR, Power_Get_structPowerMeas_u16VbattPU(), \
           Temp_GetTamb(), strMachine.u16FrequencyAppPU, strMachine.u16CurrentPU);
        LOG0_DoLogData(WAIT);        
      #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
    #endif  /* LOG_DATI */

    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      if ( (u8)InputSP_GetInputSetPoint() == (u8)0 )        /* Se il drive deve rimanere fermo... */
      {
        Main_Set_u16ErrorType(INVALID_PWM_IN);              /* ...in tal caso non occorre segnalare nulla sulla diagnostica... */
        Main_Set_u16ErrorTypeMem(INVALID_PWM_IN);
        Main_Set_u8StartNumber(0);
      }
      else
      {
        Main_Set_u16ErrorTypeMem(FAULT_ERROR);              /* ...altrimenti si segnala il FAULT_ERROR... */
        Main_Set_u8StartNumber(DIAG_RESTART_MASK_ATTEMPTS); /* ...generandolo immediatamente (i.e. saturando u8StartNumber). */
      }

      u16ErrorType_tmp = Main_Get_u16ErrorType();
      DIAG_ManageDiagnosticToECU(enState, u16ErrorType_tmp);
    #endif  /* !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) */

    #ifdef GM_LIN_ENCODING
      SetBit(u8GM_StatusByte, INTERNAL_OPERATIONAL_FAULT_PRESENT);
     (void)LIN_ExecuteLINProcedure();
    #endif  /* GM_LIN_ENCODING */
  }

  Main_SetStart();

  #ifdef LIN_INPUT
   (void)LIN_ExecuteLINProcedure();
  #endif  /* LIN_INPUT */
  /*(void)Temp_InitTemp();*/  /* Spostata prima del while dell'auto-test!!*/
  
  #if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
   u16VbattAvg = (u16)Vbatt_GetBusVoltage();  /* Lettura della Vmon.*/
   (void)FIR_SetParFilt(VBATT_FILT,VBATT_STEP_FREQ_CONST,VBATT_RC_FREQ_CONST,VBATT_RC_FREQ_CONST,(s16)VBATT_FILT_HYST_SCALED,0);  /* Inizializzazione dei parametri del filtro sulla Vmon. */
   (void)FIR_InitVarFilt(VBATT_FILT,0,(s16)((s16)u16VbattAvg*(s16)VBATT_FILT_SCALER_FACT));  /* Inizializzazione dello stato del filtro sulla Vmon.*/
  #endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )*/

#ifdef _CANTATA_INTEGR_
  num_of_iter = getForLoopNumOfIteration();
  for(index_instr_for=0;index_instr_for<num_of_iter;index_instr_for++)
#else
#ifdef _CANTATA_
#else
  for(;;)
#endif /* _CANTATA_ */
#endif /* _CANTATA_INTEGR_ */
  {
    WTD_ResetWatchdog();

    /*#ifdef RVP_SW_MANAGEMENT_ENABLE*/
      /*(void)RVP_ManageRVPStatus();*/
    /*#endif*/  /* RVP_SW_MANAGEMENT_ENABLE*/

    #ifdef LOG_DATI
      LOG0_DoLogData(enState);
    #endif /*LOG_DATI*/

    (void)Temp_UpdateValues();

#ifdef WTD_TEST
    if(strDebug.u16DebugVar1 == (u16)1)
    {
      DEBUG2_ON;
      for(;;){} /* Loop for reset Watchdog test */
    }
#endif

    switch ( enState )
    {
       #if ( !defined(RVS) || (defined(RVS) && defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE)) )
       case IDLE:
       if ( Main_CheckPowerStage() == (BOOL)FALSE ) 
       {
         /* To avoid demagnetization if (T > RVS_TEMPERATURE_TRESHOLD_PU)--> Brake immediately           */
         /* else if (T <= RVS_TEMPERATURE_TRESHOLD_PU)--> Brake only if the speed is lower that 500 rpm, */
         /* otherwise wait that the speed decreases                                                      */

         #ifdef REDUCED_SPEED_BRAKE_VS_TEMPERATURE
         if ( Temp_GetTamb() > RVS_TEMPERATURE_TRESHOLD_PU )
         {
           enState = PARK;
         }
         else
         {
           enState = Main_CheckStartOnFly();
         }
         #else
           enState = Main_CheckStartOnFly();
         #endif  /* REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

         if ( enState == RUN_ON_FLY )
         {
           #if (!defined(RVS))  /*Avoid ROF behaviour if RVS is defined */
             Temp_InitSoaTemperature();
             RTI_SetSpeedRegPeriod(REG_LOOP_TIME); 
             SOA_InitSOAMachine();
             u16NewFreq = Tacho_GetActualRotorSpeed();                                           /* Update u32SineFreqBuf with new freqency!                                                                                    */
             u16Target_Curr = MTC_GetRunOnFlySinAmplitude(u16NewFreq);                           /* and new amplitude.                                                                                                          */
             ACM_UpdateControlStruct(u16Target_Curr,u16NewFreq);                                 /* Update VoF structure with new freq and amplitude.                                                                           */
             ACM_InitOnFly();                                                                    /* N.B.: spostato qui per inizializzare il sigmoide con rampGenerator.u16FreqRampPU gia' calcolato in ACM_UpdateControlStruct!!*/
             MTC_ForceSineGen(u16NewFreq);                                                       /* This is mandatory in order to enter u32SineFreqBuf computation.                                                             */
             (void)MTC_UpdateSine(strMachine.u16VoFVoltagePU, ACM_Get_u16FreqRampSmoothPU());    /* From next tacho masking and pwm pulse will be set                                                                           */
             MTC_ComputeStartOnFlyPhase();                                                       /* Compute phasing for tacho happening during start on fly                                                                     */
           #else
               enState = PARK;
           #endif  /* RVS */
          }
          else
          {
            /* Nop(); */
          }
        }
       break;
       #endif  /* !RVS || RVS && REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

       case PARK:
       if ( Main_CheckPowerStage() == (BOOL)FALSE ) 
       {
        MTC_InitSineGen();
        Temp_InitSoaTemperature();
        SOA_InitSOAMachine();
        RTI_SetSpeedRegPeriod(REG_LOOP_TIME);
        ACM_Init();
        ACM_ManageParking();
        enState = RUN;

        if ( Main_Get_u8StartNumber() > (u8)0 )
        {
          (void)RTI_SetTimeOutDuration(START_NUMBER_RESET_TIME);
        }
        else
        {
          /* Nop(); */
        }
       }
       else
       {
          EMR_EmergencyDisablePowerStage();  
       }
       break;

       case RUN_ON_FLY:
       if ( BitTest((MANAGE_TACHO), C_D_ISR_Status) != 0u )
       {
          s32 s32DeltaRoFVoltage;
          u16NewFreq = Tacho_GetActualRotorSpeed();
          u16Target_Curr = MTC_GetRunOnFlySinAmplitude(u16NewFreq);
          s32DeltaRoFVoltage = (s32)((s32)((s32)strMachine.u16VoFVoltagePU - (s32)u16Target_Curr)*(s32)VoF_REG_COMPUTATION_RES_BIT);
          (void)REG_InitVarPI(VoF_REG,0,0,s32DeltaRoFVoltage,0,s32DeltaRoFVoltage);

          #ifdef DUAL_LINE_ZERO_CROSSING
            INTERRUPT_BEMF_DISABLE;
          #endif  /* DUAL_LINE_ZERO_CROSSING */

          MTC_ResetZCECntrl(); /* Reset ZCCECntrl */

          #ifdef DUAL_LINE_ZERO_CROSSING
            INTERRUPT_TACHO_ENABLE;
          #endif  /* DUAL_LINE_ZERO_CROSSING */

          Tacho_ManageTachoCapture();
          Tacho_SetFirstRofMaskingTime();                                 /* Tacho_ManageTachoCapture() routine computes masking basing on current ZC event but till now, tacho sensing is BEMF.*/
                                                                          /* That's the reason of correcting first tacho masking time before enabling.                                          */
                                                                          /* Tacho_SetFirstRofMaskingTime() calling has to stand after Tacho_ManageTachoCapture() in order to overwrite         */
                                                                          /* u16PwmTicksToUnmaskTacho value and before enabling output.                                                         */
        TLE_EnableOutputs();                                              /* PWM ON!                                                                                                            */
        /*BitClear(HALL_CHECKED, C_D_ISR_Status);*/
        BitClear((MANAGE_TACHO), C_D_ISR_Status);

        /*#ifdef RVP_SW_MANAGEMENT_ENABLE*/
          /*(void)RVP_SetRVPForROF();*/
        /*#endif*/ /*RVP_SW_MANAGEMENT_ENABLE*/

        enState = RUN;
       }
       break;

       case RUN:
       if ( Main_CheckPowerStage() == (BOOL)TRUE ) 
       {
         EMR_EmergencyDisablePowerStage();
       }
       else
       {
         #ifdef BMW_LIN_ENCODING
          BOOLDriveShutdown = FALSE;
         #endif  /* BMW_LIN_ENCODING */

         #ifdef GM_LIN_ENCODING
          u8ClrBit(u8GM_StatusByte, POWER_OVERCURRENT_FAULT_PRESENT);
          u8ClrBit(u8GM_StatusByte, INTERNAL_OPERATIONAL_FAULT_PRESENT);
          u8ClrBit(u8GM_StatusByte, MALFUNCTION_SPEED_SENSING);
         #endif  /* GM_LIN_ENCODING */

         ACM_DoMotorControl();

         if ( ACM_Get_u16FreqRampSmoothPU() > EL_FREQ_RESET_START_NUM_PU_RES_BIT )
         {
           #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
            (void)Main_CheckPartialStall();
           #endif  /* DIAGNOSTIC_STATE */

           if (RTI_IsTimeOutElapsed() != (BOOL)FALSE)
           {
             Main_Set_u8StartNumber(0);

             #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
              u16ErrorType_tmp = Main_Get_u16ErrorType();
              if ( ( DIAG_IsDiagnosticEnable() == (BOOL)FALSE ) && ( u16ErrorType_tmp == NO_ERROR ) )   /* Se la velocita' minima e' stata raggiunta e la linea di diagnostica e' spenta... */
              {
                DIAG_RemoveFault();                                                                     /* ...allora non ci sono errori da segnalare e la suddetta linea puo' essere disabilitata totalmente (compreso relativo interrupt). */
                Main_Set_u16ErrorTypeMem(NO_ERROR);
              }
              else
              {
                /* Nop(); */
              }
             #endif  /* DIAGNOSTIC_STATE */
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
       break;

       case WAIT:
       if(InputSP_ReadyToSleep() == (BOOL)TRUE)
       {
         EMR_SetStopMode();
       }
       else
       {
         /* Nop(); */
       }

       (void)InputSP_GetInputSetPoint(); /* During WAIT state keep checking InputSP_GetInputSetPoint() function keep plausibility check alive. */
        
       if (RTI_IsTimeOutElapsed() == (BOOL)TRUE)
       {
          Auto_InitProtections();
          ret_Auto_AutoTestChecks = Auto_AutoTestChecks();
          if ( (Main_CheckPowerStage()== (BOOL)FALSE) && (ret_Auto_AutoTestChecks == (BOOL)FALSE) )   /* If PWM input get higher than PWM_IN_MAX_TO_EMERGENCY                    */
          {                                                                                           /* enumPwmInState gets EMERGENCY and in order to keep enumPwmInState value */
                                                                                                      /* and so hysteresis we check GetInputSetPoint before calling              */
                                                                                                      /* MTC_InitPWMInputVariable() that will reset pwm input state machine      */
            #ifdef KEY_SHORT_LOGIC
              if ( INT0_Get_enKeyShort() == CHECK )   /* If the drive has to run and the key-short logic state is CHECK (i.e.: key input has been analized and no short-circuit has been detected)... */
              {
                (void)Vbatt_EnableVbattOn();          /* ...re-enable V_BATT_ON line... */
                INT0_Set_enKeyShort(REFRESH);         /* ...and set REFRESH state for the key-short logic (i.e. key-short control will be re-executed after TIME_TO_REFRESH_KEY_SHORT). */
              }
              else
              {
                /* Nop(); */
              }
            #endif /* KEY_SHORT_LOGIC */

            #if ( defined(RVS) && !defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE) )  /* Even if u16SineMagScaled gets equal to SINEMAGSCALED_MAX and so ACM_CheckWindmillRecovery() function returns true, */
             enState = PARK;                                                      /* since ACM_CheckWindmillRecovery() function is called into Main_CheckPowerStage() funtion only when enState == RUN, motor will always turns on if reverse speed is defined even if it got off because of windmill effect. */
            #else
             enState = IDLE;
             MTC_SetupCheckStartingSpeed();                                       /* Setup system for checking initial speed condition */
            #endif  /* RVS && !REDUCED_SPEED_BRAKE_VS_TEMPERATURE */
          }
       }
       break;

       case FAULT:
       faultcase = (BOOL)TRUE;
       break;

       default:
       EMR_EmergencyDisablePowerStage();
       RESET_MICRO;
    
       faultcase = (BOOL)TRUE;
       break;
    } /* switch */

    if(faultcase == (BOOL)TRUE)
    {
#ifndef _CANTATA_
      for (;;)
#endif
      {
        WTD_ResetWatchdog();
        #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
          u16ErrorType_tmp = Main_Get_u16ErrorType();
          DIAG_ManageDiagnosticToECU(enState, u16ErrorType_tmp);
        #endif
        Main_Set_u16ErrorType(NO_ERROR); /* Avoid latter error updating. */

        if( InputSP_GetOperatingMode() == NORMAL_OPERATION ) /* Go to sleep mode when u16NumNoDuty >= PWM_IN_DISAPPEAR_TEST_OP_NUM */
        {
          if(InputSP_ReadyToSleep() == (BOOL)TRUE)
          {
            EMR_SetStopMode();
          }
        }
        else /* Test mode  --> send byte when ( u16NumNoDuty >= PWM_IN_DISAPPEAR_TEST_OP_NUM ) */
        {
          if(InputSP_PwmDisappeared() == (BOOL)TRUE)
          {
            EMR_SetStopMode();
          }
        }
      }
    }

    #ifdef LIN_INPUT
      (void)LIN_ExecuteLINProcedure();
      WTD_ResetWatchdog();
      (void)Main_UpdateLinSpeedFeedback();
    #endif  /* LIN_INPUT */

    #ifdef KEY_SHORT_LOGIC
      (void)Main_ManageKeyShort();
    #endif  /* KEY_SHORT_LOGIC */
    
    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      #ifndef DIAGNOSTIC_ONLY_IN_FAULT
        u16ErrorType_tmp = Main_Get_u16ErrorType();
        DIAG_ManageDiagnosticToECU(enState, u16ErrorType_tmp);
      #endif  /* DIAGNOSTIC_ONLY_IN_FAULT */
    #endif  /* DIAGNOSTIC_STATE */

    #if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
      if ( INT0_Get_UpdateVbattFilt() == (BOOL)TRUE )  /* Ogni 8 ms... */
      {
        INT0_Set_UpdateVbattFilt(FALSE);
        (void)FIR_UpdateFilt(VBATT_FILT,(s16)((s16)Vbatt_GetBusVoltage()*(s16)VBATT_FILT_SCALER_FACT));
        u16VbattAvg = (u16)((s16)((s16)VectVarFilt[VBATT_FILT].s16NewOutput/(s16)VBATT_FILT_SCALER_FACT));  /* ..il filtro sulla Vmon viene aggiornato. */
      }
      else
      {
        /* Nop(); */
      } 
    #endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */

    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      u16ErrorType_tmp = Main_Get_u16ErrorType();
      if ( ( u16ErrorType_tmp == NO_ERROR ) || ( u16ErrorType_tmp == PARTIAL_STALL ) )
    #else
        u16ErrorType_tmp = Main_Get_u16ErrorType();
       if ( u16ErrorType_tmp == NO_ERROR )
    #endif  /* DIAGNOSTIC_STATE */
       {
         #ifdef ENABLE_RS232_DEBUG
         Com_SendSystemDataRS232(); /* This Routine requires 1mS more or less */
         #endif /*ENABLE_RS232_DEBUG*/
       }
       else
       {
          #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
            u16ErrorTypeMem_tmp = Main_Get_u16ErrorType();
            Main_Set_u16ErrorTypeMem(u16ErrorTypeMem_tmp);
          #endif  /* DIAGNOSTIC_STATE */

          #ifndef DUAL_LINE_ZERO_CROSSING
           #ifdef LIN_INPUT
            INTERRUPT_TACHO_DISABLE;
            ZCE_CNTRL_OFF;
            ZCE_CNTRL_SET_OUT;
            C_D_ISR_Status = PHASE_INITIAL_SPEED;       /* Necessario per evitare lo spegnimento del sensore di ZC nell'interrupt INT0_TachoCapture() */
            INTERRUPT_TACHO_ENABLE;
           #endif  /* LIN_INPUT */
          #else
           INTERRUPT_TACHO_DISABLE;
           INTERRUPT_BEMF_ENABLE;
          #endif  /* SINGLE_LINE_ZERO_CROSSING */

          #ifdef LOG_DATI
            (void)Vbatt_GetBusVoltage();
            (void)Curr_GetPeakCurrent();

            #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
              if ( Main_Get_u16ErrorType() == I_LIM )
              {
                LOG0_LogError(I_LIM, Power_Get_structPowerMeas_u16VbattPU(), \
                   Temp_GetTMos(), strMachine.u16FrequencyAppPU, u16LogPeakCurrentPU);
              }
              else
              {
                u16ErrorType_tmp = Main_Get_u16ErrorType();
                LOG0_LogError(u16ErrorType_tmp, Power_Get_structPowerMeas_u16VbattPU(), \
                   Temp_GetTMos(), strMachine.u16FrequencyAppPU, strMachine.u16CurrentPU);          
              }
            #else
              if ( Main_Get_u16ErrorType() == I_LIM )
              {
                LOG0_LogError(I_LIM, Power_Get_structPowerMeas_u16VbattPU(), \
                   Temp_GetTamb(), strMachine.u16FrequencyAppPU, u16LogPeakCurrentPU);
              }
              else
              {
                u16ErrorType_tmp = Main_Get_u16ErrorType();
                LOG0_LogError(u16ErrorType_tmp, Power_Get_structPowerMeas_u16VbattPU(), \
                   Temp_GetTamb(), strMachine.u16FrequencyAppPU, strMachine.u16CurrentPU);          
              }
            #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
          #endif  /*LOG_DATI */

          u16ErrorType_tmp = Main_Get_u16ErrorType();
          if( ValBit((u16ErrorType_tmp),INVALID_PWM_IN_FLAG) != 0u )
          {
            EMR_EmergencyDisablePowerStage();

            RTI_SetTimeOutDuration(IDLE_TIME_WAIT_PWM_IN);

            Main_Set_u8StartNumber(0);

            #ifdef BMW_LIN_ENCODING
              BOOLWmShutDown = FALSE;               /* Reset del flag di spegnimento per Windmill. Al termine di IDLE_TIME_WAIT_PWM_IN, si ripassa dallo stato di IDLE*/
            #endif  /* BMW_LIN_ENCODING */          /* e si decide se partire da fermo/agganciarsi al volo/restare in attesa. */

            if( InputSP_GetOperatingMode() == NORMAL_OPERATION )
            {
              enState = WAIT;
            }
            else
            {
              #ifdef LIN_INPUT
                EMR_SetStopMode();
                RTI_SetTimeOutDuration(IDLE_TIME_WAIT_PWM_IN);  
                enState = WAIT;
              #else      
                enState = FAULT;     /* If motor is in test mode and pwm disappears -> FAULT and stop. */
              #endif  /* LIN_INPUT */
            } 
          }
          else if ( (u16ErrorType_tmp & RESTART_MASK) != 0u )
          {
            Main_Inc_u8StartNumber();

            if ( Main_Get_u8StartNumber() >= RESTART_MASK_MAX_VALUE )
            {
              Main_Set_u8StartNumber(RESTART_MASK_MAX_VALUE);
            }
            else
            {
              #if defined(DBG_WATCHDOG)
              if ( Main_Get_u8StartNumber() >= 3)
              {
               for(;;){} /* Loop for reset Watchdog test */
              }
              #else
              /* Nop(); */
              #endif
            }

            u8StartNumber_tmp = Main_Get_u8StartNumber();
            RTI_SetTimeOutDuration((u16)((u16)RESTART_MASK_TIME*u8StartNumber_tmp));
            enState = WAIT;
          }
          #ifdef OVERCURRENT_ROF
          else if ( ValBit(u16ErrorType_tmp,I_TRANSIENT_FLAG) )
          {
            EMR_EmergencyDisablePowerStage();
            enState = IDLE;
            MTC_SetupCheckStartingSpeed(); /* Set on RoF */
          }
          #endif  /* OVERCURRENT_ROF */
          else
          {
            Main_Set_u8StartNumber(0);
            RTI_SetTimeOutDuration(IDLE_TIME);
            enState = WAIT;
          }
          Main_Set_u16ErrorType(NO_ERROR); /* To avoid latter checking! */
    }

  } /* for(;;) */
} /* main */

/*PRQA S 0601 -- */

/** 
    \fn BOOL Main_CheckPowerStage(void)

    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check main signals during running to intercept errors
    \details This function is designed to check all the possible errors during the motor running.\n
    This has to be executed at least once during the main loop to ensure the consistency of the whole system.
    This function has also the aim to recognize normal events that request the motor stop (Input Signal in Stop Mode).\n
    \dot
         digraph Main_CheckPowerStage {
          	node [shape=box, style=filled, fontname=Arial, fontsize=10];  
		Init[label="Init",color="yellow"];
         	FALSE[ label="return FALSE", color="darkgreen",shape=doubleoctagon];
          	TRUE[ label="return TRUE", color="red",shape=doubleoctagon];       
   	compound=true;
    	subgraph cluster0{
         	Test1[label="Check InputSetPoint?", color=".7 .3 1.0"];
		Test2[label="No InputSetPoint?", color=".7 .3 1.0"];
		Test3[label="Vbatt OverRanges?", color=".7 .3 1.0"];
		Test4[label="Windmill Error?", color=".7 .3 1.0"];
		label="General Check"; fontsize=10;
	}
		Err1[label="SetBit(u16ErrorType,INVALID_PWM_IN_FLAG)"];
		Err2[label="SetStopMode", shape=ellipse, color="orangered"];
		Err3[label="SetBit(u16ErrorType,V_BUS_FLAG)"];
		Err4[label="SetBit(u16ErrorType,WINDMILL_FLAG)"];
		Err5[label="SetBit(u16ErrorType,T_AMB_FLAG)"];
		Err6[label="SetBit(u16ErrorType,RPI_PROTECTION_FLAG)"];
		Err7[label="SetBit(u16ErrorType,I_LIM_FLAG)"];


    	subgraph cluster1{
		Test5R[label="TAmbAtRun \n outside valid ranges?", color=".7 .3 1.0"];
		Test6R[label="Tacho Overflow \n outside valid ranges?", color=".7 .3 1.0"];
		Test7R[label="SOA protection exceeded?", color=".7 .3 1.0"];
		label="enState==RUN"; fontsize=10;
	}
    	subgraph cluster2{
		Test5[label="TAmbPreRun \n outside valid ranges?", color=".7 .3 1.0"];
		label="enState!=RUN"; fontsize=10;
	}
		Init->Test1[];
          	Test1->Test2->Test3->Test4->Test5->FALSE[label=" NO", fontsize=8, color="darkgreen"];	
		Test4->Test5R->Test6R->Test7R->FALSE[label=" NO", fontsize=8, color="darkgreen"];					
		Test1->Err1->TRUE[label=" YES", fontsize=8];
		Test2->Err2[label=" YES", fontsize=8];
		Err2->TRUE[style =dotted, fontsize=8];
		Test3->Err3->TRUE[label=" YES", fontsize=8];
		Test4->Err4->TRUE[label=" YES", fontsize=8];
		Test5->Err5->TRUE[label=" YES", fontsize=8];
		Test5R->Err5[label=" YES", fontsize=8];
		Test6R->Err6->TRUE[label=" YES", fontsize=8];
		Test7R->Err7->TRUE[label=" YES", fontsize=8];
       }
    \enddot
                            \par Used Variables
    ref var_name description \n
    \return \b BOOL Boolean Value: TRUE if an Error is occurred FALSE if everything is OK
    \note This funtion is called at least once during the main loop to ensure system consistency
    \warning none
*/

static BOOL Main_CheckPowerStage(void) /*PRQA S 2889 #multiple return due to error management*/
{
  RTI_CheckCPSTimeBase();    /* Verify if CheckPowerStage timebase is expired (8msec) */

  #ifdef KEY_INPUT
  if( InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    if ((INP1_KeyIsOff()) != 0u)
    {
      Main_SetBit_u16ErrorType(KEY_INPUT_OFF_FLAG);
      return (TRUE);
    }
    else
    {
      Main_ClrBit_u16ErrorType(KEY_INPUT_OFF_FLAG);
    }
  }
  else
  {
    Main_ClrBit_u16ErrorType(KEY_INPUT_OFF_FLAG); /* Don't pay attention at key/Enable if motor is running in test mode. */
  }
  #endif  /* KEY_INPUT */

  if (InputSP_PwmDisappeared() == (BOOL)TRUE)
  {
    Main_SetBit_u16ErrorType(INVALID_PWM_IN_FLAG);
    return (TRUE);
  }
  else
  {
    Main_ClrBit_u16ErrorType(INVALID_PWM_IN_FLAG);
  }

  if (InputSP_GetInputSetPoint() == (u8)0)
  {
    Main_SetBit_u16ErrorType(INVALID_PWM_IN_FLAG);
    return (TRUE);
  }
  else
  {
    Main_ClrBit_u16ErrorType(INVALID_PWM_IN_FLAG);
  }

  if ( Vbatt_CheckOverVoltage() == (BOOL)TRUE )
  {
    Main_SetBit_u16ErrorType(V_BUS_FLAG);

    #ifdef BMW_LIN_ENCODING
      BOOLDriveShutdown = TRUE;
    #endif  /* BMW_LIN_ENCODING */

    #ifdef GM_LIN_ENCODING
      SetBit(u8GM_StatusByte, POWER_OVERVOLTAGE_FAULT_PRESENT);
    #endif  /* GM_LIN_ENCODING */

    return (TRUE);
  }
  else
  {
    Main_ClrBit_u16ErrorType(V_BUS_FLAG);

    #ifdef GM_LIN_ENCODING
  	  u8ClrBit(u8GM_StatusByte, POWER_OVERVOLTAGE_FAULT_PRESENT);
    #endif  /* GM_LIN_ENCODING */
  }

  if ( ( enState == PARK ) || ( enState == RUN ) )
  {
    if ( SOA_CheckSOA() == (BOOL)TRUE )
    {
      Main_SetBit_u16ErrorType(I_LIM_FLAG);
      MTC_InitMTCVariables(); /* Per avere u16PeakCurr = 0 ad ogni partenza (reset lettura del picco). In questo modo la SOA puo' essere attiva sin dal PARK !! */

      #ifdef BMW_LIN_ENCODING
        BOOLDriveShutdown = TRUE;
      #endif  /* BMW_LIN_ENCODING */
 
      #ifdef GM_LIN_ENCODING
        SetBit(u8GM_StatusByte, POWER_OVERCURRENT_FAULT_PRESENT);
      #endif  /* GM_LIN_ENCODING */

      return (TRUE);
    }
    else
    {
      Main_ClrBit_u16ErrorType(I_LIM_FLAG);
    }
  }
  else
  {
    Main_ClrBit_u16ErrorType(I_LIM_FLAG);
  }

  if (enState == RUN) /* Before RUN, TLE_EnableOutputs() is called into RUN_ON_FLY or PARK state -> u8GDErrorCount = 0. */
  {
    if ( TLE_CheckError() == (BOOL)TRUE )
    {
      Main_SetBit_u16ErrorType(GD_ERROR_FLAG);

      #ifdef BMW_LIN_ENCODING
       BOOLDriveShutdown = TRUE;
      #endif  /* BMW_LIN_ENCODING */

      #ifdef GM_LIN_ENCODING
       SetBit(u8GM_StatusByte, INTERNAL_OPERATIONAL_FAULT_PRESENT);
      #endif  /* GM_LIN_ENCODING   */

      return (TRUE);
    }
    else
    {
      Main_ClrBit_u16ErrorType(GD_ERROR_FLAG);
    }
  }

  if (enState == RUN)
  {
    if ( Temp_CheckTempAmbAtRun() == (BOOL)TRUE )
    {
      Main_SetBit_u16ErrorType(T_AMB_FLAG);

      #ifdef BMW_LIN_ENCODING
       BOOLDriveShutdown = TRUE;
      #endif  /* BMW_LIN_ENCODING */

      return (TRUE);
    }
    else
    {
      Main_ClrBit_u16ErrorType(T_AMB_FLAG);
    }

    if ( Tacho_CheckTachoOverflow() == (BOOL)TRUE )
    {
      Main_SetBit_u16ErrorType(RPI_PROTECTION_FLAG);

      #ifdef BMW_LIN_ENCODING
       BOOLDriveShutdown = TRUE;
      #endif  /* BMW_LIN_ENCODING */

      #ifdef GM_LIN_ENCODING
       SetBit(u8GM_StatusByte, MALFUNCTION_SPEED_SENSING);
      #endif  /* GM_LIN_ENCODING */

      return (TRUE);
    }
    else
    {
      Main_ClrBit_u16ErrorType(RPI_PROTECTION_FLAG);
    }
  }
  else
  {
    if ( Temp_CheckTempAmbPreRun() == (BOOL)TRUE )
    {
      Main_SetBit_u16ErrorType(T_AMB_FLAG);

      #ifdef BMW_LIN_ENCODING
       BOOLDriveShutdown = TRUE;
      #endif  /* BMW_LIN_ENCODING */

      return (TRUE);
    }
    else
    {
      Main_ClrBit_u16ErrorType(T_AMB_FLAG);
    }
  }

  RTI_ClearCPSTimeBase(); /* Clear CheckPowerStage timebase (8msec) */

  return (FALSE);
}

/** 
    \fn void Main_SetStart(void)

    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Start Up management
    \details This function is designed to set initial start condition.\n
    This function set \ref enState variable basing on:\n
    - \ref enState = \ref PARK if \ref AXIAL and reverse speed are defined in \ref NORMAL_OPERATION mode or.\n 
      in test mode i.e. \ref enumOperatingMode different from NORMAL_OPERATION.\n
    - \ref enState = \ref IDLE if \ref AXIAL is defined and reverse speed isn't defined
                            \par Used Variables
    \return \b void
    \note none
    \warning none
*/
static void Main_SetStart(void)
{
  if ( InputSP_GetOperatingMode() == NORMAL_OPERATION) 
  {
    #if ( defined (RVS) && !defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE) )
      enState = PARK;
    #else
      enState = IDLE;
      MTC_SetupCheckStartingSpeed();
    #endif  /* RVS && !REDUCED_SPEED_BRAKE_VS_TEMPERATURE */
  }
  else
  {
    enState = PARK;
  } 
}


#if ( !defined(RVS) || (defined(RVS) && defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE)) )

/** 
    \fn SystStatus_t Main_CheckStartOnFly( void )

    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check conditions to allow Start On Fly
    \details This routins have to check if there are conditions to start motor motion with a residual speed on the rotor.\n
    At the begining it monitors the status of the input set point, to define the starting speed to look, and the rotor speed
    to decide if the software can start in Start_On_Fly mode.\n
         \dot
     digraph StartOnFly {
          	node [shape=box, style=filled, fontname=Arial, fontsize=10];  
            WAIT[ label="START_WAIT_FOR_RIGHT_W", fillcolor="green"];
          	FLY[ label="START_FROM_RUN", fillcolor="darkgreen"];
          	FAULT[ label="START_FAIL", fillcolor="red",shape=doubleoctagon];       
          	Start[shape=ellipse, label="Reset"];
          	Test[label="Is Sequence Complete?"];
          	Start->Test[];
          	Test->FAULT[label=" Error", fontsize=8];
          	Test->WAIT[label=" W Too High", fontsize=8  ];
          	Test->FLY[label=" W Acceptable", fontsize=8  ];
       }
    \enddot
                            \par Used Variables
    ref var_name description \n
    \return \b SystStatus_t Enum Type: Return the Start-up Status
    \note none
    \warning none
*/
static SystStatus_t Main_CheckStartOnFly( void )
{
  SystStatus_t ret;
  StartStatus_t enumStartStatus;
  u8 u8RefIn;
  u16 u16SpeedSetPoint;

  u8RefIn = InputSP_GetInputSetPoint();

  #ifdef DUAL_LINE_ZERO_CROSSING
    (void)MTC_ResetZCECntrl();    /* Per mantenere la linea di zero-crossing non differenziale in modalita' "corrente". */
  #endif  /* DUAL_LINE_ZERO_CROSSING */

  #if (FAN_TYPE == AXIAL)

    #ifdef REDUCED_SPEED_BRAKE_VS_TEMPERATURE
      u16SpeedSetPoint = RVS_BRAKE_FREQ_MIN_PU;   /*To force IDLE state if speed is > RVS_BRAKE_FREQ_MIN_PU*/
    #else
      u16SpeedSetPoint = ACM_SetFrequencySetPoint(u8RefIn);
    #endif /*REDUCED_SPEED_BRAKE_VS_TEMPERATURE*/
    
  #elif ( ( FAN_TYPE == BLOWER ) || defined(OVERCURRENT_ROF) )
    u16SpeedSetPoint = IPK_TRANSIENT_CONTROL_PU_RES_BIT;
  #endif /*FAN_TYPE*/

  enumStartStatus = MTC_CheckStartingSpeed(u16SpeedSetPoint);

  if (enumStartStatus == START_UNDER_W_MIN)
  {
    ret = (PARK);
  }
  else if (enumStartStatus == START_FROM_RUN)
  {
    ret = (RUN_ON_FLY);
  }
  else  /* START_WAIT_FOR_RIGHT_W */
  {
    ret = (IDLE);
  }
  return ret;
}
#endif  /* !RVS || RVS && REDUCED_SPEED_BRAKE_VS_TEMPERATURE */

/** 
    \fn void Main_SetupVariables( void )

    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initialisation of main Variables
    \details Here there are initializations for main variables

                            \par Used Variables
    \ref enState Current Machine State \n
    \ref strDebug External structure for debug \n
    \ref u8StartNumber Number of starts \n
    \ref u16ErrorType Error main flags \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/

static void Main_SetupVariables( void )
{
  #if ( defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG) )
    strDebug.u16DebugVar1 = 0;
    strDebug.u32DebugVar2 = 0;
    strDebug.s16DebugVar3 = 0;
    strDebug.s32DebugVar4 = 0;
  #endif  /* defined(DEBUG_MODE) || defined(ENABLE_RS232_DEBUG)*/

  Main_Set_u8StartNumber(0);
  
  #ifdef KEY_SHORT_LOGIC
    INT0_Set_u16KeyRefreshCount((u16)0);
    INT0_Set_enKeyShort(DISABLE);
  #endif  /* KEY_SHORT_LOGIC*/

  #if ( defined (RVS) && !defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE) )
    enState = PARK;
  #else
    enState = IDLE;
  #endif  /* RVS */

  Main_Set_u16ErrorType(NO_ERROR);

  #ifdef LOG_DATI
    u16LogPeakCurrentPU = 0;
  #endif  /* LOG_DATI */

  #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
    Main_Set_u16ErrorTypeMem(NO_ERROR);
  #endif  /* DIAGNOSTIC_STATE */   /* To avoid latter checking! */
}


#ifdef KEY_SHORT_LOGIC
/** 
    \fn void Main_ManageKeyShort ( void )

    \author	Ianni Fabrizio  \n <em>Main Developer</em> 

    \date 28/03/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Logic to detect short-circuit on key input

                            \par Used Variables
    \ref enState              state of the main logical machine       \n
    \ref enKeyShort           state of the key-short logic            \n
    \ref u16KeyRefreshCount   counter to refresh key-short detection  \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void Main_ManageKeyShort (void)
{
  if ( INT0_Get_enKeyShort() == INIT )                         /* If key-short logic is intialized... */
  {
    if ( ( enState == PARK ) || ( enState == RUN ) )           /* ...and the drive is running... */
    {
      (void)EMR_EmergencyDisablePowerStage();                  /* ...disable the power stage. */
    }
    else
    {
      /* Nop(); */
    }

    enState = WAIT;                                            /* Set WAIT state in the main loop... */
    RTI_SetTimeOutDuration(CHECK_KEY_SHORT_TIME_WAIT);         /* ...for a CHECK_KEY_SHORT_TIME_WAIT delaty time. */

    (void)Vbatt_DisableVbattOn();                              /* Disable V_BATT line. */

    INT0_Set_u16KeyRefreshCount(TIME_TO_REFRESH_KEY_SHORT);    /* Set the key-short refresh counter to the refresh timeout. */

    INT0_Set_enKeyShort(CHECK);                                /* Set CHECK state in the key-short logic. */
  }
  else
  {
    /* Nop(); */
  }
}
#endif  /* KEY_SHORT_LOGIC */


#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
/** 
    \fn void Main_CheckPartialStall ( void )

    \author	Ianni Fabrizio  \n <em>Main Developer</em> 

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check partial stall condition

                            \par Used Variables
    \ref VectVarPI[DELTA_MAX_REG].s32RegOutput    Delta-max regulator output \n
    \ref VectVarPI[MAX_CUR_REG].s32RegOutput      Delta-max regulator output \n
    \ref rampGenerator.u16FreqRampSmoothPU        Smooth-frequency ramp \n
    \ref strMachine.u16FrequencyPU                Drive frequency \n
    \ref BOOLSOATemperatureCorr                   Derating flag \n
    \ref u16ErrorType                             Drive-error variable \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void Main_CheckPartialStall (void)
{
  static u16 u16PartialStallErr;

  u16PartialStallErr = NO_ERROR;

  if ( (u32)VectVarPI[DELTA_MAX_REG].s32RegOutput > DELTA_LIM_PARTIAL_STALL_PU_RES_BIT )
  {
    u16PartialStallErr = PARTIAL_STALL;
  }
  else
  {
    /* Nop(); */
  }

  if ( (u32)VectVarPI[MAX_CUR_REG].s32RegOutput > MAX_IPK_LIM_PARTIAL_STALL_PU_RES_BIT )
  {
    u16PartialStallErr = PARTIAL_STALL;
  }
  else
  {
    /* Nop(); */
  }

  if ( MaxPower_Get_BOOLPartStallDerFlag() == (BOOL)TRUE )
  {
    u16PartialStallErr = PARTIAL_STALL;
  }
  else
  {
    /* Nop(); */
  }

  Main_Set_u16ErrorType(u16PartialStallErr);
}
#endif  /* DIAGNOSTIC_STATE */

#ifdef LIN_INPUT
/** 
    \fn void Main_UpdateLinSpeedFeedback( void )

    \author	Ianni Fabrizio  \n <em>Developer</em>

    \date 16/03/2016
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Manage and update the (LIN) speed feedback output, based on bemf detection, during the free running or Wind-mill phase (i.e. power stage halted).

                            \par Used Variables
    \ref enState                            Current Machine State         \n
    \ref strVoFSStruct.FrequencySetPoint    Non-filtered frequency output \n
    \ref u16NewFreq                         New bemf sample               \n
    \ref u16OldFreq                         Old bemf sample               \n
    \ref u16NewFreqTol                      Tolerance on new bemf sample  \n
    \ref u8BemfMinCount                     Counter of bemf-under-freq_min events \n
    \ref u8StartNumber                      Number of startup attempts  \n
    \ref u8TachoOverflowNumber              Number of overflowes on tacho signal      \n
    \ref C_D_ISR_Status                     State-byte for Interrupt Service Routine  \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void Main_UpdateLinSpeedFeedback (void)
{
  /* Questa gestione del feedback di velocita' e' relativa solo agli stati IDLE e WAIT, in cui c'e' il sensing della bemf. */
  if ( ( enState == IDLE ) || ( enState == WAIT ) ) 
  {
    /* Se vi e' un errore della RESTART_MASK oppure gli overflows dello ZC indicano che il drive e' fermo, si resetta subito il feddback di velocita'. */
    if ( u8TachoOverflowNumber > TACHO_OVF_FOR_SPEED_RESET  )
    {
      strMachine.u16FrequencyAppPU = 0;
      u16OldFreq                   = 0;
      u16NewFreqTol                = 0;
    }
    /* Se viene percepito un segnale di ZC sulla bemf si esegue questo ramo. */
    else if ( BitTest(MANAGE_TACHO, C_D_ISR_Status) )
    {
      u16NewFreq = Tacho_GetActualRotorSpeed();    /* Si considera la frequenza elettrica calcolata. */

      /* Se la frequenza elettrica e' al di sotto di EL_FREQ_BEMF_SPEED_FEEDBACK_MIN, si resetta il feedback di velocita'... */
      if ( u16NewFreq < EL_FREQ_MIN_SPEED_FEEDBACK_PU_RES_BIT )
      {
        u8BemfMinCount++;
        if ( u8BemfMinCount > BEMF_MIN_COUNT_MAX )
        {
          strMachine.u16FrequencyAppPU = 0;
          u16OldFreq                   = 0;
          u16NewFreqTol                = 0;
          u8BemfMinCount               = 0;
        }
        else
        {
          /* Nop(); */
        }
       }
       else            /*...altrimenti segnalo la velocita' percepita. */
       {
         u8BemfMinCount = 0;

         if ( u16OldFreq == 0  )    /* Se il feedback di velocita' (in IDLE o WAIT) non e' inizializzato allora: */
         {
           if ( strMachine.u16FrequencyAppPU > 0 )        /* 1) se il drive viene dallo stato di RUN..... */
           {
             u16OldFreq = strMachine.u16FrequencyAppPU;   /*...allora l'isteresi di velocita' viene centrata sull'ultimo valore (aggiornato appunto durante il RUN) di strVoFSStruct.FrequencySetPoint, cosi' da avere continuita' sul feedback. */
             u16NewFreqTol = (u16)((u32)((u32)u16OldFreq*BEMF_SPEED_TOL_NUM)>>BEMF_SPEED_TOL_SHIFT);
           }
           else                                           /* 2) se il drive viene da uno stato di reset... */
           {
             /* Inizializzazione del viltro del feedback-LIN (di velocita'), con drive in WAIT/IDLE (ed eventualmente sotto Windmill). */
             (void)FIR_SetParFilt(LIN_SPEED_FILT,LIN_SPEED_INT_STEP_FREQ,LIN_SPEED_FREQ_CONST,LIN_SPEED_FREQ_CONST,LIN_SPEED_HYST_PU,0);
             (void)FIR_InitVarFilt(LIN_SPEED_FILT,0,(s16)((u16)u16NewFreq>>LIN_SPEED_PRESCALER_SHIFT));

             u16OldFreq = u16NewFreq;                     /* ...allora l'isteresi viene centrata sulla bemf appena rilevata. */
             u16NewFreqTol = (u16)((u32)((u32)u16NewFreq*BEMF_SPEED_TOL_NUM_WM)>>BEMF_SPEED_TOL_SHIFT);           
           }
         }
         else                                             /* Se il feedback di velocita' (in IDLE o WAIT) e' gia' inizializzato.... */
         {
          if  ( ( u16NewFreq >= (u16)((u16)u16OldFreq-u16NewFreqTol)) && ( u16NewFreq <= (u16)((u16)u16OldFreq+u16NewFreqTol)))      /* ... si controlla che il nuovo campione di frequenza sia entro l'isteresi stabilita... */
          {
            u16OldFreq = u16NewFreq; 
            u16NewFreqTol = (u16)((u32)((u32)u16OldFreq*BEMF_SPEED_TOL_NUM)>>BEMF_SPEED_TOL_SHIFT);    /* ...in tal caso si aggiorna l'isteresi centrandola sul nuovo campione (per il prossimo confronto)... */
            strMachine.u16FrequencyAppPU = u16NewFreq;                                                 /* ...e si aggiorna il feedback di velocita'. */
          }
          else
          {
            /* Nop(); */
          }
        }
       }
       BitClear(MANAGE_TACHO, C_D_ISR_Status);
    }
    else
    {
      /* Nop(); */
    }
  }
  else    /* Le variabili di gestione del feedback di velocita' in IDLE o WAIT vengono resettate se il drive non e' in tali stati. */
  {
    u16OldFreq     = 0;
    u16NewFreqTol  = 0;
    u8BemfMinCount = 0; 
  }
}
#endif  /* LIN_INPUT */

/*-----------------------------------------------------------------------------*/
/**
    \fn void Main_SetBit_u16ErrorType(u16 errTyp)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the error type bit
                            \par Used Variables
    \ref u16ErrorType \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void Main_SetBit_u16ErrorType(u16 errTyp)
{
   u16ErrorType |= (u16)((u16)1<<(u8)(errTyp));
}

/*-----------------------------------------------------------------------------*/
/**
    \fn static void Main_Set_u16ErrorType(u16 errTyp)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the error type bit
                            \par Used Variables
    \ref u16ErrorType \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void Main_Set_u16ErrorType(u16 errTyp)
{
   u16ErrorType = errTyp;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void Main_ClrBit_u16ErrorType(u16 errTyp)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Clears the error type bit
                            \par Used Variables
    \ref u16ErrorType \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void Main_ClrBit_u16ErrorType(u16 errTyp)
{
   u16ErrorType = (u16)(u16ErrorType)&(~((u16)1<<(u8)(errTyp)));
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u32 Main_Get_u16ErrorType(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the error type
                            \par Used Variables
    \ref u16ErrorType \n
    \return \b The value of static u16ErrorType variable.
    \note None.
    \warning None.
*/
#if (defined(ENABLE_RS232_DEBUG) || defined(BMW_LIN_ENCODING))
u16 Main_Get_u16ErrorType(void)
#else
static u16 Main_Get_u16ErrorType(void)
#endif
{
   return (u16ErrorType);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn static void Main_Set_u8StartNumber(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the number of static u8StartNumber variable
                            \par Used Variables
    \ref u8StartNumber \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void Main_Set_u8StartNumber(u8 u8Value)
{
   u8StartNumber = u8Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 Main_Get_u8StartNumber(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Gets the value of static u8StartNumber variable
                            \par Used Variables
    \ref u8StartNumber \n
    \return \b The value of static u8StartNumber variable.
    \note None.
    \warning None.
*/
u8 Main_Get_u8StartNumber(void) /*PRQA S 1505 #Could be used in diagnostic file when enabled*/
{
   return (u8StartNumber);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn static void Main_Inc_u8StartNumber(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Increments the value of static u8StartNumber variable
                            \par Used Variables
    \ref u8StartNumber \n
    \return \b None
    \note None.
    \warning None.
*/
static void Main_Inc_u8StartNumber(void)
{
   u8StartNumber++;
}

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
/*-----------------------------------------------------------------------------*/
/**
    \fn static void Main_Set_u16ErrorTypeMem(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16ErrorTypeMem
                            \par Used Variables
    \ref u16ErrorTypeMem \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void Main_Set_u16ErrorTypeMem(u16 u16Value)
{
   u16ErrorTypeMem = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u16 Main_Get_u16ErrorTypeMem(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of the static variable u16ErrorTypeMem
                            \par Used Variables
    \ref u16ErrorTypeMem \n
    \return \b The value of the static variable u16ErrorTypeMem.
    \note None.
    \warning None.
*/
u16 Main_Get_u16ErrorTypeMem(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   return(u16ErrorTypeMem);
}
#endif  /* DIAGNOSTIC_STATE */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
