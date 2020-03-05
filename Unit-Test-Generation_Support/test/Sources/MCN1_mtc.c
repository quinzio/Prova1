/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MCN1_mtc.c

VERSION  :  1.3

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MCN1_mtc.c
    \version see \ref FMW_VERSION
    \brief Motion Control Routines
    \details In this files all the intermidiate motion control routines are regrouped.
    In line of principle all functions that are contained in this files have to drive Level 0 routines
    and will use and provides variables for routines in \ref MCN1_acmotor.c 
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif

#include "SPAL_Setup.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include INI0_INIT              /* Per MTC_CLOCK */
#include "main.h"               /* Per strDebug */
#include "MCN1_acmotor.h"       /* It has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"           /* Public motor control peripheral function prototypes */
#include "MCN1_RegPI.h"
#include "TIM1_RTI.h"           /* Public general purpose Timebase function prototypes */
#include "MCN1_Tacho.h"
#include MAC0_Register          /* Needed for macro definitions */
#include ADM0_ADC
#include "TMP1_Temperature.h"
#include "CUR1_Current.h"
#include "WTD1_Watchdog.h"
#include "INP1_InputSP.h"
#include "SOA1_SOA.h"
#include "VBA1_Vbatt.h"
#include "INT0_PWMIsrS12.h"
#include "DIA1_Diagnostic.h"    /* Per DIAG_ManageDiagnosticToECU(IDLE, NO_ERROR) */
#ifdef LIN_INPUT

  #if defined(BMW_LIN_ENCODING)
    #include "COM1_LINUpdate_BMW.h"
  #elif defined(GM_LIN_ENCODING)
    #include "COM1_LINUpdate_GM.h"
  #else
    #error("Define LIN stack!!");
  #endif  /* BMW_LIN_ENCODING */

  #include "FIR1_FIR.h"         /* Per FIR_InitVarFilt(LIN_SPEED_FILT,0,0); */
#endif  /* LIN_INPUT */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* --------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables ---------------------------- */
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE

  static BOOL MTC_parkfailsafe_flg;
  static volatile u16 MTC_parkfailsafe_cnt;

  #define MTC_PARKFAILSAFE_CNT_MAX (u16)1000  /* Max counts between two consecutive INT0_dpwmmin_modulation isr */

/** 
    \var u16StatorFreq
    \brief Buffer for the new frequency value
    \details This variable is allocated to store the old value of this parameter. If the old
    value is different from the new one, the motion control parameters are updated.
    \note This variable is allocated in the \b DEFAULT Page
    \warning This is a \b STATIC variable
    
    \var u16NewVoltageBuf
    \brief Buffer for the new voltage value
    \details This variable is allocated to store the old value of this parameter. If the old
    value is different from the new one, the motion control parameters are updated.
    \note This variable is allocated in the \b DEFAULT Page
    \warning This is a \b STATIC variable
*/
  static u16 u16StatorFreq;
  static u16 u16NewVoltageBuf;

/* -------------------------- Public variables ----------------------------- */
/* Some variables accessed in assembly located in page zero to decrease CPU load */
/* WARNING: if SensorPeriod structure is put outside Zero page, the declaration  */
/* of the pointer PSpeedMeas_s has to be modified (remove near/@tiny)            */

/** 
    \var C_D_ISR_Status
    \brief Flags for motor ISR status 
    \details In this u8 variable are regrouped some flag for PWM ISR use. This flags are useful to 
    control the PWM ISR by the main loop.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var ZCStatusBitfield
    \brief Flags for zero crossing storing
    \details This variable stores flags for current zero crossing computation (interrupts managing) and reverse speed. 
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var u8MTCStatus
    \brief Flags for General motor status
    \details In this u8 variable are regrouped some flag for sincronize motor control routines. This flags are used 
    to manage code parts in the same routines in different time. The bitfield is mapped using two series of MACRO, the first
    used to direct operations and the second, linked in the list below, used to define the bit number.
    - \ref FREQ_CHANGE_BIT	
    - \ref SINEMAG_CHANGE_BIT 
    - \ref VBUS_SAMPLED_BIT	  
    - \ref PLATEAU_ON_BIT     
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var u16PWMCount
    \brief Variable used to mask Tacho 
    \details This variable is used to mask Tacho for a designed period fraction. It's incremented and tested in the
    PWM ISR.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var u16UpdateLoopTimer;
    \brief Value of the delay time
    \details This variable count the delay of time to set the flag to update all the PI regulators and the V/f modulator inputs.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u16UpdateLoopTime;
    \brief Value of the delay time preset
    \details This variable is the preset of the delay of time to set the flag to update all the PI regulators and the V/f modulator inputs.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var u16SineMag
    \brief Voltage amplitude imposed on motor phases
    \details The PWM ISR use this variable as a reference for the Voltage needed by the V/f alghoritm.
    This is updated only if the \ref u16SineMagBuf is updated and a flag is set.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u16SineMagBuf
    \brief Value of the PWM amplitude calculated by V/f Alghoritm
    \details To ensure variable consistency all the V/f alghoritm, executed in the main loop, can access only to the 
    SineMagBuff variable that represent the voltage amplitude imposed on motor phases. This value will be read by the 
    PWM ISR only after its update.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u16SineMagScaled
    \brief Voltage amplitude imposed on motor phases, compensated by battery voltage
    \details This variable represents the voltage amplitude imposed on motor phases. This variable differs from 
    the \ref u16SineMag for compensation done by taking into account the battery voltage.\n
    This variable is the result of calculation done on PWM ISR to compensate battery voltage fluctuations.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u32SineFreq
    \brief Actual Frequency imposed by the PWM ISR
    \details This is the frequency value imposed by the V/f alghoritm. The PWM Interrupt Service Routine 
    add to the rotor angle this value every time it's executed. So SineFreq can be considered like the rotor 
    frequency multiplied by a coefficent to consider integration time.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u32SineFreqBuf
    \brief Buffer for the sine frequency calculated
    \details This variable is used in Level 1 to store results for frequency alghoritms. When the frequency
    is calculated, the value of this variable is updated and can be read by the PWM ISR.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u16Phase
    \brief Value of the istant rotor Phase
    \details This is the Value of the rotor Angle. \ref U16_MAX represent 360 electrical degrees.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u16PhaseBuff
    \brief Buffer for the Phase Value before the Run_On_Fly Locking
    \details This Variable is used during the Run_On_Fly locking to match the correct initial phase
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u16MaxCurr
    \brief Maximal current calculated during an electric period
    \details During the PWM ISR the MaxCurr is continuously updated to find the Peak Current during an electric
    period. Each times a Tacho event occur this variable is resetted to zero.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u16PeakCurr
    \brief Peak current during the previous period
    \details During the PWM ISR the MaxCurr is continuously updated to find the Peak Current during an electric
    period. Each times a Tacho event occur teh MaxCurr value is copied in PeakCurr  before the reset to zero.
    So PeakCurr is the peak value measured the previous electric period.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var u16HwRefTimer
    \brief Counter variable
    \details This variable is used to create the necessary time delay, after closing the RVP, to balance the mass of the battery with the mass of the electrolytics.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var BOOLHwRefFlag
    \brief Boolean variable
    \details Variable used to allow counting the time delay to balance the mass of the battery with the mass of the electrolytics.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var u16VbattAvg
    \brief 
    \details Variable Low-pass filtered Vmon.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
*/
  volatile C_D_ISR_Status_t C_D_ISR_Status;
  u8 ZCStatusBitfield;
  u8  u8MTCStatus;
  volatile u16 u16PWMCount;
  volatile u16 u16UpdateLoopTimer;
  volatile u16 u16UpdateLoopTime;
  volatile u16 u16SineMag;
  volatile u16 u16SineMagBuf;
  volatile u16 u16SineMagScaled;
  volatile u16 u16SineFreq;
  volatile u32 u32SineFreqBuf;
  volatile u16 u16Phase;
  volatile u16 u16PhaseBuff;
  volatile u16 u16MaxCurr;
  volatile u16 u16PeakCurr;

  #ifdef GROUND_BALANCE  
    volatile u16 u16HwRefTimer;
    BOOL BOOLHwRefFlag;
  #endif  /* GROUND_BALANCE */

  #if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
    volatile u16 u16VbattAvg;                /* Tensione di batteria (Vmon) filtrata RC. */
  #endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */

  #pragma DATA_SEG DEFAULT

  static u32 u32DeltaAngleSweep;

/* --------------------------- Private functions --------------------------- */
static void  MTC_SetZCECntrl(void);
static void  MTC_RegParkAmplitude(u16 u16ParkingFreq);
static void  MTC_ParkFailSafeMonitor(void);

/* --------------------------- Private macros ------------------------------ */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_ResetZCECntrl( void )
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Switch the Current HW Sensor in Back EMF Sensor
    \details This Digital Output force the implemented hardware to switch from different functions.\n
    In low state it's in Current Sensing Mode and allow the micro to mesure the current zero crossing;
    in high state the hw is configured as Back-EMF detector.
    \ref enState Current Machine State \n
                            \par Used Variables
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void MTC_ResetZCECntrl(void)
{
  ZCE_CNTRL_ON;
  ZCE_CNTRL_SET_OUT;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void MTC_SetZCECntrl( void )
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Switch the Current HW Sensor in Back EMF Sensor
    \details This Digital Output force the implemented hardware to switch from different functions.\n
    In low state it's in Current Sensing Mode and allow the micro to mesure the current zero crossing;
    in high state the hw is configured as Back-EMF detector.
    \ref enState Current Machine State \n
                            \par Used Variables
    \return \b void No value returned from this function
    \note none
    \warning none
*/
static void MTC_SetZCECntrl(void)
{
  ZCE_CNTRL_OFF;
  ZCE_CNTRL_SET_OUT;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_InitMTCVariables( void )
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initialization of motor control variables
    \details This routine initializes all motor control variables to a consistent value for
    motor starting 
                               \par Used Variables
    \ref strMachine Structure that is made up of all the variables representing the electrical machine \n
    \ref ZCStatusBitfield This variable stores flags for current zero crossing computation (interrupts managing) and reverse speed \n
    \ref u16MaxCurr Buffer to check maximal current \n
    \ref u16PeakCurr Peak current measured during past electrical period \n
    \ref u16HwRefTimer This variable is used to create the necessary time delay, after closing the RVP, to balance the mass of the battery with the mass of the electrolytics
    \ref BOOLHwRefFlag Variable used to allow counting the time delay to balance the mass of the battery with the mass of the electrolytics
    \ref u16SineMagScaled Voltage amplitude compensated by battery voltage \n
    \ref u16PwmTicksToUnmaskTacho Number of pwm pulses before re-enabling tacho isr \n
    \ref u16UpdateLoopTime Timer/Counter preset of the delay of time to set the flag to update all the PI regulators and the V/f modulator inputs\n
    \ref u16UpdateLoopTimer Timer/Counter that counts the delay of time to set the flag to update all the PI regulators and the V/f modulator inputs\n
    \ref C_D_ISR_Status State-byte for Interrupt Service Routine \n
    \ref u8MTCStatus Flags used to update or not sinus generation \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void MTC_InitMTCVariables(void)
{
  strMachine.s16AngleDeltaPU = 0;
  strMachine.s16AnglePhiPU = 0;	

  ZCStatusBitfield = 0; /* Reset DeltaZCStatus flags. MANDATORY!! */

  u16MaxCurr = 0;
  u16PeakCurr = 0;
  strMachine.u16CurrentPU = 0;

  #ifdef GROUND_BALANCE  
    u16HwRefTimer = GROUND_BALANCE_TIME_BIT;
    BOOLHwRefFlag = FALSE;
  #endif  /* GROUND_BALANCE */

  u16SineMagScaled = 0;
  u16PwmTicksToUnmaskTacho = STARTUP_ANTI_RINGING;
  u16UpdateLoopTime = UPDATE_LOOP_TIME_PU;
  u16UpdateLoopTimer = u16UpdateLoopTime;
  BitClear((UPDATE_LOOP), C_D_ISR_Status);

  #ifdef RVS
    if ( InputSP_GetOperatingMode() != NORMAL_OPERATION )
    {
      SetBit(u8MTCStatus,FWD_BWD_DIR_BIT);
    }
    else
    {
      /* Nop(); */
    }
  #endif  /* RVS*/
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 MTC_GetRunOnFlySinAmplitude(u16 )
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Calc the Run_On_Fly Amplitude
    \details This routines has to return the amplitude for a correct locking during Run_On_Fly mode. To calulate this
    value it need the locking frequency measured by reading the Tacho signal. \n
    The function needed to calculate the correct V/f voltage amplitude is:
       \f[ V(T[^\circ C],f[Hz]= \left\{ 
       (\frac{f}{K_{SCALE}}-K_{OFFSET})*[1-(T_{AMB}-25)*K_{PROP}] \right\} \f]
    According to material parameters the Back-EMF variation is +/- 1% each 10 degrees vith a negative slope; 
    this is the K_PROP value.\n
    This function can not be directly implemented on the micro but it have to be scaled to consider quantization error
    and to increase resolution. \n
    So the micro alghoritm becomes:
    
          \f[ \begin{align*}
          V(T,f) &= \left ( \frac{u16ElectricalFreq * K_{VoF ON FLY NUM}}{2^{K _{VoF ON FLY SHIFT}}}-K_{VoF ON FLY OFFSET} \right )\left [1-(Tamb-25)*0,001 \right ] =
          \\  &= \left ( \frac{u16ElectricalFreq * K_{VoF ON FLY NUM}}{2^{K _{VoF ON FLY SHIFT}}}-K_{VoF ON FLY OFFSET} \right ) \left [ \frac{8192-(Tamb-25)*8}{8192} \right ]
          \end{align*} \f]
    
    \param[in] u16ElectricalFreq Frequency value needed to match rotor frequency at locking time. \n
    \return \b u16 An integer value that quantifies the voltage amplitude to apply for correct Run_On_Fly locking
    \note none
    \warning none
*/  
u16 MTC_GetRunOnFlySinAmplitude(u16 u16ElectricalFreq) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 u16ActualBemf;

  u16ActualBemf = (u16)((u32)((u32)u16ElectricalFreq*K_VoF_PU_SCALED)>>K_VoF_RES_FOR_PU_SHIFT);
  u16ActualBemf = (u16)((u32)((u32)u16ActualBemf*(u32)((u32)BEMF_TEMP_CORR_OFFSET_PU-(u32)INT0_Get_strTempDrive_s16TempAmbReadPU()))/(u32)BEMF_TEMP_CORR_DIVIDER_PU); /* Taking count of temperature. */
                                                                                                                                                                      /* Casting is needed to avoid MISRA errors but all of the */
                                                                                                                                                                      /* signed operations are properly managed */

  return((u16)u16ActualBemf);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_ComputeStartOnFlyPhase(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute the Initial locking phase for Run_On_Fly
    \details This subroutine calculates the phase value . \n
    The function implemented is: 

    \f[
      u16PhaseBuff = K_{SoF OFFSET FIXED} + (Freq - Freq_{CHANGE\_SLOPE})/8
    \f]
    Firmware costant are:
    \f[
    K_{SoF OFFSET FIXED} 
    \f]
    = \ref K_SOF_OFFSET_FIXED
    \f[
    Freq 
    \f]
    = u16NewFrequency
    \f[
    Freq_{CHANGE\_SLOPE} 
    \f]
    = \ref FREQ_CHANGE_VoF_SLOPE_SCALED

                            \par Used Variables
    \ref u16PhaseBuff This is the calculated value for initial locking Phase during Run_On_Fly \n
    \ref strMachine Structure that is made up of all the variables representing the electrical machine \n
    \return \b void No value returned by this function
    \note none
    \warning none
*/  
void MTC_ComputeStartOnFlyPhase(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16PhaseBuff = K_SOF_OFFSET_FIXED;
  strMachine.s16AngleDeltaPU = 0;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_InitSineGen(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Basic V/f variables initialization
    \details Here all the V/f variables (amplitude, frequency, flags...) are initialized before a start up
    or a micro reset.
    At the end of \ref WAIT state, pwm input is checked again and a new \ref PARK and so \ref MTC_InitSineGen(void) function call is done.
    This last call has to keep \ref FWD_BWD_DIR_BIT bit into u8MTCStatus.
                            \par Used Variables
    \ref u8MTCStatus This is an u8 variable that contain all the control flag \n
    \ref u16SineFreq This is the frequency applied to the rotor by the V/f alghoritm  \n
    \ref u16SineMag This is the amplitude that is applied to the rotor by the V/f alghoritm without the battery compensation \n
    \ref u16SineMagBuf This is the amplitude needed by the V/f alghoritm to drive rotor at the desired frequency stored in u32SineFreq \n
    \ref u16SineMagScaled This is the amplitude that is applied to the rotor by the V/f alghoritm with applied the battery compensation \n
    \ref u16StatorFreq This is a buffer for the frequency needed to the rotor by the V/f alghoritm \n        
    \ref u16NewVoltageBuf This is a buffer for the amplitude needed to the rotor by the V/f alghoritm \n            
    \return \b void No value returned by this function
    \note The u16StatorFreq value <b>has to be</b> initialized to a different value from FREQ_INIT thet is the start value for the V/f
    \warning none
*/  
void MTC_InitSineGen(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  #ifdef RVS 
    u8MTCStatus &= SINE_GEN_u8MTCStatus_FLAGS;      /* N.B.                                                                                                    */
                                                    /* If RVS is used, FWD_BWD_DIR_BIT bit into u8MTCStatus has to keep its own value from different starting. */
  #else
    u8MTCStatus = 0;                /* Reset all flags */
  #endif  /* RVS */

  u16SineFreq      = 0;             /* Ora si segue lo Zc di tensione: meglio partire da zero (tanto poi segue l'allineamento) !! */
  u16SineMag       = 0;
  u16SineMagBuf    = 0;
  u16SineMagScaled = 0;
  u16StatorFreq    = FREQ_INIT-1u;  /* This ensure that Sinewave will be updated */
  u16NewVoltageBuf = 0;
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_ForceSineGen(u16)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Used to force a new sine computation
    \details This function takes as parameter the new frequncy requested and updates the u16StatorFreq to a different value; 
    in addition, to force generation the MTCStatus Flag \ref FREQ_CHANGE is setted.\n
    This forces a new computation of the V/f alghoritm.
                            \par Used Variables
    \ref u8MTCStatus This is an u8 variable that contain all the control flag \n
    \ref u16StatorFreq This is a buffer for the frequency needed to the rotor by the V/f alghoritm \n        
    \param[in] u16NewFrequency New Frequency value that will be applied to the next electrical period. \n
    \return \b void No value returned by this function
    \note none
    \warning none
*/
void MTC_ForceSineGen(u16 u16NewFrequency) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16StatorFreq = u16NewFrequency-1u;
  u8MTCStatus &= ~((u8)FREQ_CHANGE);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn static void MTC_RegParkAmplitude (u16 u16ParkingFreq)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Used to force a new sine computation
    \details This function takes as parameter the new frequncy requested and updates the u16SineMagBuf to a different value; 
    in addition, to force generation the MTCStatus Flag \ref SINEMAG_CHANGE is setted.\n
    This forces a new computation of the V/f alghoritm.
                            \par Used Variables
    \ref u8MTCStatus This is an u8 variable that contain all the control flag \n
    \ref u16PeakCurr Current peak value over electrical period \n  
    \ref strMachine Structure that is made up of all the variables representing the electrical machine \n
    \ref u16SineMagBuf Value of the PWM amplitude calculated by V/f Alghoritm        
    \param[in] u16ParkingFreq New Parking Frequency value that will be applied to the next electrical period. \n
    \return \b void No value returned by this function
    \note none
    \warning none
*/
static void MTC_RegParkAmplitude (u16 u16ParkingFreq)
{
  u32 u32VofParkDelta;
  u16 u16CurrSp;

  u16PeakCurr = Curr_GetCurrent();
  strMachine.u16CurrentPU = (u16)((u16)u16PeakCurr * BASE_CURRENT_CONV_FACT_RES);

  u16CurrSp = (u16)((u32)((u32)CURRENT_REGULATION_REF_PU*u16ParkingFreq)/(u16)((u32)((u32)FREQUENCY_RES_FOR_PU_BIT*(u32)UPDATE_LOOP_FREQ)/(u32)((u32)BASE_FREQUENCY_HZ*(u16)((u16)MTC_ComputeParkingTime()/POLE_PAIR_NUM))));
  
  if ( u16CurrSp >= CURRENT_REGULATION_REF_PU )
  {
    u16CurrSp = CURRENT_REGULATION_REF_PU;
  }
  else
  {
    /* Nop(); */
  }

  (void)REG_UpdateInputPI(START_REG,(s16)u16CurrSp,(s16)strMachine.u16CurrentPU);
  (void)REG_UpdateRegPI(START_REG);

  /* Contributo V/f della frequenza di parcheggio, che si incrementa ad ogni ciclo elettrico */
  /* (utile per avere continuita' di tensione Vf quando si passa alla fase di RUN).          */
  u32VofParkDelta = (u32)((u32)((u32)u16ParkingFreq*K_VoF_PU_SCALED)>>K_VoF_RES_FOR_PU_SHIFT);
  u32VofParkDelta = (u32)((u32)((u32)u32VofParkDelta*(u32)((u32)BEMF_TEMP_CORR_OFFSET_PU-(u32)INT0_Get_strTempDrive_s16TempAmbReadPU()))/(u32)BEMF_TEMP_CORR_DIVIDER_PU); /* Taking count of temperature. */
  u32VofParkDelta *= START_REG_COMPUTATION_RES_BIT;

  /* Contributo del regolatore di parcheggio + V/f di cui sopra. */
  u16SineMagBuf = (u16)((u32)((u32)((u32)((u32)((u32)VectVarPI[START_REG].s32RegOutput + u32VofParkDelta) * (u16)(BASE_VOLTAGE_VOLT*(float32_t)10))/START_REG_COMPUTATION_RES_BIT) * 512u) / (u32)((u32)((u32)((u32)V_BUS_REFERENCE_VOLT*10000u)/(u16)1732) * VOLTAGE_RES_FOR_PU_BIT));

  u8MTCStatus |= SINEMAG_CHANGE;
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL MTC_UpdateSine (u16, u16)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Voltage and Frequency update for next electrical period
    \details In this function both voltage and frequency are updated to drive the PWM ISR to a new V/f parameters set-up.
    If this function is called, a new parameters set is passed as arguments. These value are written in the correct buffers
    and the relative flags are setted to communicate that an update has been done. Passed values are squared to impose limitations
    and they are multiplied for some coefficients, stored in macros, to give the correct scale.
              
                            \par Used Variables
    \ref u8MTCStatus This is an u8 variable that contain all the control flag \n
    \ref u32SineFreqBuf This is a buffer for the frequency applied to the rotor by the V/f alghoritm  \n
    \ref u16SineMagBuf This is the amplitude needed by the V/f alghoritm to drive rotor at the desired frequency stored in u32SineFreq \n
    \ref u16StatorFreq This is a buffer for the frequency needed to the rotor by the V/f alghoritm \n        
    \ref u16NewVoltageBuf This is a buffer for the amplitude needed to the rotor by the V/f alghoritm \n            
    \param[in] u16NewVoltage new Voltage value that will be applied to the next electrical period. \n
    \param[in] u16NewFrequency new Frequency value that will be applied to the next electrical period. \n
    \return \b bool boolUpdateStatus is a boolean value that is TRUE if a something is changedin frequency variables; 
    it is FALSE if u16NewFrequency = u16StatorFreq.
    \note The U Update interrupts are masked during the updation of the variables defining the magnitude and the frequency of the 
		sinewave, u32SineFreq and u16SineMag; This ensure that both LSB and MSB	modification will be taken into account within the same U interrupt
    \warning none
*/

BOOL MTC_UpdateSine (u16 u16NewVoltage, u16 u16NewFrequency)
{
  BOOL boolUpdateStatus;

  boolUpdateStatus = FALSE;

  /* ========================================================================= */
  /*                          FREQUENCY UPDATION PART                          */
  /* ========================================================================= */

  /* Recompute frequency only if value is different from the current one... */
  if ((u8MTCStatus & FREQ_CHANGE) == 0u)  /* ... unless an updation is on-going */
  {
    if (u16NewFrequency != u16StatorFreq)
    {
      /* Store previous value, valid untill PWM Update event occurs */
      u16StatorFreq = u16NewFrequency;

      if ( u16NewFrequency >= EL_FREQ_MAX_PU_RES_BIT )
      {
        u32SineFreqBuf = (u32)((u32)SINEFREQ_FREQ_RESOL_PU * EL_FREQ_MAX_PU_RES_BIT);
      }
      else
      {
        if ( u16NewFrequency <= VoF_INIT_FREQ_PU_RES_BIT )
        {
          u32SineFreqBuf = (u32)((u32)SINEFREQ_FREQ_RESOL_PU * VoF_INIT_FREQ_PU_RES_BIT);
        }
        else
        {
          /* Extend to decrease quantization effects when dividing */
          u32SineFreqBuf = (u32)((u32)SINEFREQ_FREQ_RESOL_PU * u16NewFrequency);
        }
      }

      u32SineFreqBuf >>= 16; /* Passaggio a u16 dopo la moltiplicazione con u16NewFrequency. */

      /* New Freq value can be used in Update interrupt */
      u8MTCStatus |= FREQ_CHANGE;
      boolUpdateStatus = TRUE;
    }
    else  /* No frequency updation untill the previous one is completed */
    {
      boolUpdateStatus = FALSE;
    }  /* End of Frequency updation part */
  }

  /* ========================================================================= */
  /*                          VOLTAGE UPDATION PART                            */
  /* ========================================================================= */
  if ( u16NewVoltage != u16NewVoltageBuf )
  {
    u16NewVoltageBuf = u16NewVoltage;

    /*u16SineMagBuf = (u16)((u32)((u32)u16NewVoltage * (u16)(BASE_VOLTAGE_VOLT*10) * 512) / PHASE_VOLTAGE_CONVERSION_DEN);*/
    u16SineMagBuf = (u16)((u32)((u32)u16NewVoltage * (u16)(BASE_VOLTAGE_VOLT*(float32_t)10) * 512u) / (u32)((u32)((u32)((u32)V_BUS_REFERENCE_VOLT*10000u)/(u16)1732) * VOLTAGE_RES_FOR_PU_BIT));
    u8MTCStatus |= SINEMAG_CHANGE;
  }

  return (boolUpdateStatus);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 MTC_GetScaledVoltage(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns to another environment the u16SineMagScaled value
    \details Returns to another environment the u16SineMagScaled value.
              
                            \par Used Variables
    \ref u16SineMagScaled This is the amplitude applied by the V/f alghoritm with the battery voltage compensation \n
    \return \b u16  This function returns u16SineMagScaled
    \note none
    \warning none
*/
u16 MTC_GetScaledVoltage(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  return (u16SineMagScaled);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_SettingForParkingPhase(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Setting for V/f variables during the parking phase
    \details Here the frequency and the amplitude variables are setted to perform the parking phase
                            \par Used Variables
    \ref u32SineFreqBuf This is a buffer for the frequency applied to the rotor by the V/f alghoritm  \n
    \ref u16SineMagScaled This is the amplitude that is applied to the rotor by the V/f alghoritm with applied the battery compensation \n
    \ref u16SineMagBuf This is the amplitude needed by the V/f alghoritm to drive rotor at the desired frequency stored in u32SineFreq \n
    \ref u16SineMag This is the amplitude that is applied to the rotor by the V/f alghoritm without the battery compensation \n
    \ref strMachine This structure is made up of all the variables representing the electrical machine.
    \ref u8MTCStatus This is an u8 variable that contain all the control flag \n
    \ref u16Phase This is the current electrical angle imposed to rotor\n
    \ref u32DeltaAngleSweep This is the angle increment to be made at each adjustment interval
    \return \b void No value returned by this function
    \note none
    \warning none
*/
void MTC_SettingForParkingPhase( void ) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u32SineFreqBuf             = 0;
  u16SineMagScaled           = 0;     /* Mandatory since at parking phase in ACM_ManageParking routine             */
  u16SineMagBuf              = 0;     /* u16SineMag (and so u16SineMagScaled computed after vbus sampling) will be */
  u16SineMag                 = 0;     /* at right value of INITIAL_PARK_VOLTAGE                                    */
  strMachine.u16VoFVoltagePU = 0;
  u8MTCStatus |= FREQ_CHANGE;

  u16Phase = (u16)START_SWEEP_ANGLE;
  u32DeltaAngleSweep = (u32)((u32)(TARGET_SWEEP_ANGLE-START_SWEEP_ANGLE)/(u16)((u16)MTC_ComputeParkingTime()));
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 MTC_ComputeParkingTime(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return parking time.
    \details This function return a u16 value telling parking time (base time RTI
    is 8ms).\n
    
    \f[
      TIME_{POSTALIGNMENT}=f(T_{AMB})= \left\{\begin{matrix}
          TIME_{NORMAL}^{MIN40DEG} &\Leftrightarrow &(s16Tamb  \leq T_{-40^\circ})
      \\
          TIME_{NORMAL}^{MIN40DEG} +  * \frac {K_{COEFF}^{TEMP}}{128}*(s16Tamb - T_{-40^\circ}) & \Leftrightarrow &(T_{-40^\circ}< s16Tamb  \leq T_{120^\circ}) 
      \\ 
          TIME_{NORMAL}^{120DEG} &\Leftrightarrow &(s16Tamb  > T_{120^\circ})
      \end{matrix}\right.
    \f]
                            \par Used Variables
    \return \b u16 parking time (base time RTI is 8ms).
    \note none
    \warning none
*/
u16 MTC_ComputeParkingTime(void)
{
  u16 u16ParkingTime;

  /*
  s16 s16Tamb;
  s16Tamb = Temp_GetTamb();

  if ( s16Tamb <= PARKING_TIME_25DEG_TEMP_PU )
  {
    u16ParkingTime = PARKING_TIME_25DEG;
  } 
  else if ( s16Tamb <= PARKING_TIME_160DEG_TEMP_PU )
  {
    u16ParkingTime = (u16)(PARKING_TIME_25DEG + (u16)((u32)((u32)((u32)((u32)PARKING_TIME_25DEG * KT_NUM * (u16)(s16Tamb - PARKING_TIME_25DEG_TEMP_PU))>>TEMPERATURE_RES_FOR_PU_SHIFT)*TLE_BASE_TEMPERATURE_DEG)>>KT_SHIFT));
  }
  else
  {
    u16ParkingTime = PARKING_TIME_160DEG;
  }
  */

  u16ParkingTime = PARKING_TIME_25DEG;

  return((u16)u16ParkingTime);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void MTC_ParkFailSafeMonitor( void )

    \author	Ruosi Gabriele  \n <em>Reviewer</em> 

    \date 21/02/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief

    \par No variables used
    \return \b void No value returned from this function
    \note none
    \warning none
*/

static void MTC_ParkFailSafeMonitor(void)
{
  if(MTC_parkfailsafe_flg == (BOOL)FALSE)
  {
    MTC_parkfailsafe_cnt++;
  }
  else
  {
    MTC_parkfailsafe_flg = (BOOL)FALSE;
    MTC_parkfailsafe_cnt = (u16)0;
  }

  if(MTC_parkfailsafe_cnt >= MTC_PARKFAILSAFE_CNT_MAX)
  {
    #ifndef _CANTATA_
    for(;;){}                           /* Never ending loop that causes WTD reset */
    #endif /* _CANTATA_ */
  }
  else
  {
    /*Nop();*/
  }
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void MTC_ParkFailSafeMonitor_rst( void )

    \author	Ruosi Gabriele  \n <em>Reviewer</em> 

    \date 21/02/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief

    \par No variables used
    \return \b void No value returned from this function
    \note none
    \warning none
*/

void MTC_ParkFailSafeMonitor_rst(void)
{
  MTC_parkfailsafe_flg = (BOOL)FALSE;
  MTC_parkfailsafe_cnt = (u16)0;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_ParkAngleSweep(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Angle rotation after aligment phase
    \details This fanction has the purpose to increase slowly the angle after the aligment phase
    to avoid some incorrect rotor position
                            \par Used Variables
    \ref u16Phase This is the current electrical angle imposed to rotor\n
    \ref C_D_ISR_Status Flags for motor ISR status\n
    \ref u16ErrorType Error main flags \n
    \return \b bool ret is a boolean value that is TRUE if set point is no more present, FALSE otherwise
    \note none
    \warning none
*/
BOOL MTC_ParkAngleSweep(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16ParkCycleFreq;
  u8 u8PolePairCountMax;
  u8 u8PolePairCount = 1;  /* Contatore del numero di cicli di parcheggio (<=> coppie polari), cosi' da percorrere 1 giro meccanico. */

  if ( (OperatingMode_t)InputSP_GetOperatingMode() == NORMAL_OPERATION )
  {
    #ifndef _CANTATA_
    u8PolePairCountMax = POLE_PAIR_NUM;
    #else
    u8PolePairCountMax = 1;
    #endif
  }
  else
  {
    u8PolePairCountMax = 1;
  }

  while ( u8PolePairCount <= u8PolePairCountMax )
  {
    /* Ad ogni ciclo, la frequenza elettrica e' incrementata proporzionalmente con il conteggio delle paia-poli. */
    u16ParkCycleFreq = (u16)((u32)((float32_t)FREQUENCY_RES_FOR_PU_BIT*UPDATE_LOOP_FREQ)/(u32)((u32)BASE_FREQUENCY_HZ*(u16)((u16)MTC_ComputeParkingTime()/u8PolePairCount)));

    while( u16Phase < TARGET_SWEEP_ANGLE )
    {
      if (( BitTest((UPDATE_LOOP), C_D_ISR_Status)) != 0u )
      {
        MTC_parkfailsafe_flg = (BOOL)TRUE;

        BitClear((UPDATE_LOOP), C_D_ISR_Status);  /* Il regolatore di parcheggio e' fatto ogni UPDATE_LOOP_TIME, cosi' come il resto del controllo. */
        (void)MTC_RegParkAmplitude((u16)u16ParkCycleFreq);

        WTD_ResetWatchdog();

        /* N.B.: per avere una frequenza di parcheggio ad ogni ciclo piu' breve, occorre incrementare lo sweep */
        /* proporzionalmente con le paia-poli.                                                                 */
        if ( u16Phase < (u32)( TARGET_SWEEP_ANGLE - (u32)((u32)u32DeltaAngleSweep*u8PolePairCount) ) )
        {
          u16Phase += (u16)((u16)u32DeltaAngleSweep*u8PolePairCount);
        }
        else
        {
          u16Phase = (u16)TARGET_SWEEP_ANGLE;
        }
      }
      else
      {
        /* Nop(); */
        MTC_ParkFailSafeMonitor();
#if defined(_CANTATA_) && defined(_CANTATA_INTEGR_)
        u16Phase = TARGET_SWEEP_ANGLE;
#endif
      }

      #ifdef LIN_INPUT  
        (void)LIN_ExecuteLINProcedure();
      #endif  /* LIN_INPUT */

      #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
        DIAG_ManageDiagnosticToECU(IDLE, NO_ERROR);         /* Chiamata generica per il check del corto-circuito sulla diagnostica. */
      #endif

      WTD_ResetWatchdog();
    }
    MTC_ParkFailSafeMonitor_rst();

    /*(void)Tacho_ManageTachoCapture(); */ /* Misura angolo Phi. */
    /*(void)ACM_ComputeDeltaAngle();    */

    u16Phase = (u16)START_SWEEP_ANGLE; /* Per ripercorrere ancora l'angolo giro (elettrico). */
    u8PolePairCount++;
    WTD_ResetWatchdog();

    if ((InputSP_GetInputSetPoint()) == 0u)
    {
      Main_SetBit_u16ErrorType(INVALID_PWM_IN_FLAG);
      ret = (BOOL)TRUE;
    }
    else
    {
      ret = (BOOL)FALSE;
    }
  }
  return (ret);
}

#if ( !defined(RVS) || (defined(RVS) && defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE)) )
/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_SetupCheckStartingSpeed(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Setup the Run_On_Fly initial check 
    \details This function initializes all variables and outputs for checking the
    initial rotor speed. This is done to allow the Run_On_Fly.
                            \par Used Variables
    \ref C_D_ISR_Status Flags for motor ISR status\n
    \return \b void No value returned by this function
    \note none
    \warning It's \b mandatory to keep C_D_ISR_Status set to PHASE_INITIAL_SPEED 
    before interrupt enabling since if tacho interrupt happens between interrupt 
    enabling and PHASE_INITIAL_SPEED setting motor doesn't start.
*/
void MTC_SetupCheckStartingSpeed(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  MTC_SetZCECntrl();                    /* N.B.                                                                                            */
                                        /* IT'S MANDATORY BEFORE DOING MTC_SetZCECntrl, HAVING SET ZCE_CNTRL AS OUTPUT AND SET IT TO ZERO. */
                                        /* WITHOUT DOING THIS, HW CRASHES HERE BY TURNING OFF STAND_BY (PTA1) PIN.                         */
                                        /* THE PRE-SET IS DONE JUST TURNED ON MICRO AT TLE7184F_ResetTLEAndForceHiZ(void) FUNCTION!!!      */

  C_D_ISR_Status = PHASE_INITIAL_SPEED; /* MANDATORY!!                                                */
                                        /* C_D_ISR_Status set to PHASE_INITIAL_SPEED has to           */
                                        /* stand before interrupt enabling since if tacho interrupt   */
                                        /* happens between interrupt enabling and PHASE_INITIAL_SPEED */
                                        /* setting motor doesn't start.                               */
  Tacho_ResetTachoOverflowNumber();
  Tacho_InitTachoVariables();
  MTC_InitMTCVariables();
  RTI_SetMainTimeBase(READ_TACHO_TIME); /* 300mS timebase */

  #ifndef DUAL_LINE_ZERO_CROSSING
    INTERRUPT_TACHO_ENABLE;           /* Enabling interrupt is done after all previous settings. */
  #else
    INTERRUPT_TACHO_DISABLE;
    INTERRUPT_BEMF_ENABLE;
  #endif  /* DUAL_LINE_ZERO_CROSSING */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn StartStatus_t MTC_CheckStartingSpeed(u16)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Check the Run_On_Fly initial speed
    \details This function check the initial rotor speed by reading Tacho resuts. If the speed is more than a minimum speed defined by
    \ref MZ_STUCK_MAX_0 and, at the same time, is less than the requested speed by the SetPoint, the function return \ref START_FROM_RUN.
    Otherwise, the function returns \ref START_WAIT_FOR_RIGHT_W if the rotor speed exceeds the requested speed, or it returns \ref START_UNDER_W_MIN
    if the rotor speed is insufficent.
          \dot digraph CheckStartingSpeed{
          node [shape=box, style=filled, fontname=Arial, fontsize=10];  
          Start[label="Start"];
          subgraph cluster1{ GetSetPoint[label="Wmax=f(SetPoint)"];
          label="Tacho_SetTachoCount"; fontsize=8; }
          GetWMin[label="Speed < Wmin?", fillcolor="yellow"];
          ICTacho[label="Tacho IC Occurred?", fillcolor="yellow"];
          subgraph cluster3{ Tacho[label="Compute Speed"];
          label="Tacho_ManageTachoCapture"; fontsize=8; }
          Lock[label="Phase Locked?", fillcolor="yellow"];
          Time[label="MainTime is over?", fillcolor="yellow"];
          GO[label="return \n START_FROM_RUN",shape=ellipse,fillcolor="darkgreen"];
          WAIT[label="return \n START_WAIT_FOR_RIGHT_W",shape=ellipse,fillcolor="deepskyblue"];
          subgraph cluster2{ Reset[label="u8OverWMin = 0;\n u8UnderWSet = 0;"];
          label="Tacho_ClearOverUnderWminTachoCount"; fontsize=8; }
          Start->GetSetPoint->ICTacho[];Tacho->Lock[];
          ICTacho->Tacho[label="YES", fontsize=8,color=green];
          GetWMin->Reset[label="OverWmin=0", fontsize=8];
          Lock->GO[label="YES", fontsize=8,color=green];
          ICTacho->Lock->Time->WAIT[label="NO", fontsize=8,color=red];
          Time->GetWMin[label="YES", fontsize=8,color=green];
          Reset->WAIT[]; } \enddot
                            \par Used Variables
    \ref C_D_ISR_Status Flags for motor ISR status\n
    \param[in] u16SetPoint The Input set point value is passed to evalue if the rotor speed is between a minimal value and the passed value \n
    \return \b StartStatus Starting status described by \ref StartStatus_t enumerator
    \note none
    \warning none
*/
StartStatus_t MTC_CheckStartingSpeed(u16 u16SetPoint) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  StartStatus_t ret;
  Tacho_SetTachoCount(u16SetPoint);

  if ( (BitTest((MANAGE_TACHO), C_D_ISR_Status)) != 0u ) /* Manage Tacho computing during starting phase */
  {
    Tacho_ManageTachoCapture();
    BitClear((MANAGE_TACHO), C_D_ISR_Status);
  }

  if ( (BitTest((PHASE_LOCKED), C_D_ISR_Status)) != 0u )
  {
    ret = START_FROM_RUN;
  }
  else if ( (RTI_IsMainTimeElapsed()) != 0u)
  {
    if ( ((BOOL)Tacho_GetOverWminTachoPeriod()) == 0u )
    {
      #ifdef LIN_INPUT
        /* Nel caso di partenza da fermo, si resettano l'input e lo stato del filtro di velocita' restiuita via LIN.                   */
        /* In particolare, u16FrequencyAppPU e' azzerata qui e non in EMR_EmergencyDisablePowerStage(), proprio perche', dallo         */
        /* spegnimento del ponte in poi, vi e' tutta la fase di evoluzione libera, in cui tale variabile e' assegnata con la frequenza */
        /* del segnale di bemf. Solo quando si decide di frenare e ripartire, u16FrequencyAppPU = 0 per l'appunto.                     */
        strMachine.u16FrequencyAppPU = 0;
        (void)FIR_InitVarFilt(LIN_SPEED_FILT,0,0);
      #endif  /* LIN_INPUT */

      ret = START_UNDER_W_MIN;
    }
    else
    {
      Tacho_ClearOverUnderWminTachoCount();
      ret = START_WAIT_FOR_RIGHT_W;   /* Windmill --> Wait right Speed */
    }
  }
  else
  {
    ret = START_WAIT_FOR_RIGHT_W;
  } 
  return (ret);
}
#endif  /* !RVS || RVS && REDUCED_SPEED_BRAKE_VS_TEMPERATURE */


#ifdef BATTERY_DEGRADATION
/*-----------------------------------------------------------------------------*/
/** 
    \fn void MTC_InitBatteryDegragation (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 29/11/2017
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initialization of Ibatt-PI regulator and Ibatt-state machine.
    
                            \par Used Variables
    \ref IbattReg           Ibatt-regulator \n
    \ref IbattStateMachine  Ibatt-state machine \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void MTC_InitBatteryDegragation (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  /* Inizializzazione regolatore Ibatt. */
  IbattReg.u16IbattSp     = IBATT_VS_VMON_SETPOINT_HIGH_BIT; 
  IbattReg.s16IbattProp   = 0;
  IbattReg.u16IbattInt    = 0; 
  IbattReg.u16IbattRegOut = 0;
  IbattReg.u8KpBatt       = IBATT_PROP_GAIN_ON_BIT;
  IbattReg.u8KiBatt       = IBATT_INT_GAIN_ON_BIT;
  
  /* Inizializzazione macchina a stati. */
  IbattStateMachine.u8IbattRegCount     = 0;
  IbattStateMachine.u8IbattRegCountMax  = 0;
  IbattStateMachine.enumGain            = GAIN_HIGH;
  IbattStateMachine.BOOLComputeSetpoint = TRUE;
  IbattStateMachine.BOOLChangeVbattComp = FALSE;
  IbattStateMachine.BOOLForceGainFlag   = FALSE;
}
#endif  /* #ifdef BATTERY_DEGRADATION */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
