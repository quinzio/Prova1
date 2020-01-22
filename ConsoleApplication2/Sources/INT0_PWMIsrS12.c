/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_PWMIsrS12.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_PWMIsrS12.c
    \version see \ref FMW_VERSION 
    \brief PWM Interrupt service routine.
    \details Routines and definition for pwm interrupt service routine.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif

#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF               /* needed for const def                                                  */
#include INI0_INIT              /* Per MTC_CLOCK nella define SINEFREQ_FREQ_RESOL_PU                     */
#include "MCN1_acmotor.h"       /* it has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"           /* Needed for u8MTCStatus bitfield                                       */
#include "MCN1_RegPI.h"
#include "FIR1_FIR.h"           /* Per costanti di fattorizzazione */
#include "MOD2_DPWM.h"          /* DPWM LUT                        */
#include "MOD2_Sine3Th.h"       /* sine3Th LUT                     */
#include MAC0_Register          /* Needed for macro definitions    */
#include ADM0_ADC
#include "MCN1_Tacho.h"
#include "WTD1_Watchdog.h"
#include "main.h"               /* Per strDebug */
#include "VBA1_Vbatt.h"
#include "INT0_PWMIsrS12.h"
#include "PBATT1_Power.h"
#include "CUR1_Current.h"       /* Per u16IBattOffset                                             */
#include "TIM1_RTI.h"           /* Per UPDATE_LOOP_TIME nella define di IBATT_DERIVATIVE_GAIN_DEN */
#include "TMP1_Temperature.h"   /* strTempDrive.s16TempAmbReadPU                                  */

#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* -------------------------- Private variables ---------------------------- */
/* Some variables accessed in assembly located in page zero to decrease CPU load */
/* WARNING: if SensorPeriod structure is put outside Zero page, the declaration  */
/* of the pointer PSpeedMeas_s has to be modified (remove near/@tiny)            */
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE

    static BOOL PWM_failsafe_flg;
    static volatile u16 PWM_failsafe_cnt;

    #ifdef BATTERY_DEGRADATION
    volatile structIbattPIreg IbattReg;              /* Regolatore sulla Ibatt.                                     */
    volatile structIbattLogic IbattStateMachine;     /* Macchina a stati per la gestione del regolatore sulla Ibatt.*/
    #endif  /* BATTERY_DEGRADATION */

#ifdef LIN_INPUT
  static BOOL BoolPWMInterruptEnabled = FALSE;     /* Disable PWM interrupt computation in order to reduce CPU overload */
#endif  /* LIN_INPUT */

#define PWM_FAILSAFE_CNT_MAX   (u16)200  /* Max counts between two consecutive INT0_dpwmmin_modulation isr */

#pragma DATA_SEG DEFAULT

static u16 u16IBattOffset;
static strTemperature strTempDrive;


/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_strTempDrive_s16TempAmbReadPU(s16 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static variable strTempDrive.s16TempAmbReadPU
                            \par Used Variables
    \ref strTempDrive.s16TempAmbReadPU \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_strTempDrive_s16TempAmbReadPU(s16 value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   strTempDrive.s16TempAmbReadPU = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn s16 INT0_Get_strTempDrive_s16TempAmbReadPU(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static variable strTempDrive.s16TempAmbReadPU
                            \par Used Variables
    \ref strTempDrive.s16TempAmbReadPU \n
    \return \b The value of static variable strTempDrive.s16TempAmbReadPU.
    \note None.
    \warning None.
*/
s16 INT0_Get_strTempDrive_s16TempAmbReadPU(void)
{
   return(strTempDrive.s16TempAmbReadPU);
}

#ifdef MOSFET_OVERTEMPERATURE_PROTECTION
/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_strTempDrive_s16TempMOSReadPU(s16 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static variable strTempDrive.s16TempMOSReadPU
                            \par Used Variables
    \ref strTempDrive.s16TempMOSReadPU \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_strTempDrive_s16TempMOSReadPU(s16 value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   strTempDrive.s16TempMOSReadPU = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn s16 INT0_Get_strTempDrive_s16TempMOSReadPU(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static variable strTempDrive.s16TempMOSReadPU
                            \par Used Variables
    \ref strTempDrive.s16TempMOSReadPU \n
    \return \b The value of static variable strTempDrive.s16TempMOSReadPU.
    \note None.
    \warning None.
*/
s16 INT0_Get_strTempDrive_s16TempMOSReadPU(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   return(strTempDrive.s16TempMOSReadPU);
}
#endif

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_Set_u16IBattOffset(u16 u16Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of the static variable u16IBattOffset
                            \par Used Variables
    \ref u16IBattOffset \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void INT0_Set_u16IBattOffset(u16 u16Value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   u16IBattOffset = u16Value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u16 INT0_Get_u16IBattOffset(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of the static variable u16IBattOffset
                            \par Used Variables
    \ref u16IBattOffset \n
    \return \b The value of the static variable u16IBattOffset.
    \note None.
    \warning None.
*/
u16 INT0_Get_u16IBattOffset(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   return(u16IBattOffset);
}

#ifdef LIN_INPUT
  /*-----------------------------------------------------------------------------*/
  /** 
      \fn void INT0_DisablePWMISRComputation(void)
      \author	 Boschesi Roberto  \n <em>Main Developer</em> 
      \date 01/02/2016
      \brief This function disable pwm interrupt service routine computation in order to 
           reduce CPU overload when PWM isr is not necessary.  
     
  */
  void INT0_DisablePWMISRComputation(void) 
  {
    BoolPWMInterruptEnabled = FALSE;
  }

  /*-----------------------------------------------------------------------------*/
  /** 
      \fn void INT0_EnablePWMISRComputation(void)
      \author	 Boschesi Roberto  \n <em>Main Developer</em> 
      \date 01/02/2016
      \brief This function Enable pwm interrupt service routine computation 
  */
  void INT0_EnablePWMISRComputation(void) 
  {
    BoolPWMInterruptEnabled = TRUE;
  }
#endif  /* LIN_INPUT */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_PWM_FailSafeMonitor( void )

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

void INT0_PWM_FailSafeMonitor(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  /*DEBUG2_ON;*/

  if(PWM_failsafe_flg == (BOOL)FALSE)
  {
    PWM_failsafe_cnt++;
  }
  else
  {
    PWM_failsafe_flg = (BOOL)FALSE;
    PWM_failsafe_cnt = (u16)0;
  }

  if(PWM_failsafe_cnt >= PWM_FAILSAFE_CNT_MAX)
  {
    #ifndef _CANTATA_
    for(;;){}                      /* Never ending loop that causes WTD reset */
    #endif /* _CANTATA_ */
  }
  else
  {
    /*Nop();*/
  }

  /*DEBUG2_OFF;*/
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void INT0_PWM_FailSafeMonitor_rst( void )

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

void INT0_PWM_FailSafeMonitor_rst(void)
{
   PWM_failsafe_flg = (BOOL)FALSE;
   PWM_failsafe_cnt = (u16)0;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void INT0_dpwmmin_modulation(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function manages timer1 i.e. pwm interrupt service routine.  
    \details This function manages pwm inteerupt service routine by managing timer1
    interrupt overflow. It performs different issues.\n
    1. <b>Sequential successive convertion of ADC channel </b> by \ref u8ADC_Struct_Index
    Results of these convertions will be placed into \ref buffu16ADC_READING array.\n
    This feature is implemented using two buffer and only a pointer. The pointer (\ref u8ADC_Struct_Index)
    is the index for the two array \ref buffu8ADC_Converting, used to store to AD channel settings, and
    \ref buffu16ADC_READING, used to store the last AD aquisition value.
    The whole data system is so represented
      \dot
      digraph Buffer {
      node [shape=box, style=filled, fontname=Arial, fontsize=10];  rankdir=LR; edge [fontsize=8];
      subgraph cluster3{
      	buff1[shape=record, label="<f0>0: u16 AD readed value |<f1>1: u16 AD readed value |<f2>2: u16 AD readed value |<f3>3: u16 AD readed value"];
      label= "buffu16ADC_READING[ADC_READING_LENGHT]"; fontsize=8;
      }
      subgraph cluster1{
      buff2[shape=record, label="<f0>CURRENT_INPUT_CHANNEL| <f1> BUS_VOLTAGE_CHANNEL|<f2> TEMP_SENSOR_INT_CHANNEL|<f3> TLE_TEMP_SENSOR_CHANNEL"];
      label= "buffu8ADC_Converting[ADC_READING_LENGHT]"; fontsize=8;
      }
      ptr[label="u8ADC_Struct_Index", shape=ellipse];

      ptr-> buff1:f0->buff2:f0[];
      buff1:f1->buff2:f1[style=dotted];
      buff1:f2->buff2:f2[style=dotted];
      buff1:f3->buff2:f3[style=dotted];
      }
      \enddot
    3. <b>Electrical angle computation</b>(i.e. \ref u32Phase updating). Each isr \ref u32Phase is incremented
    by \ref u32SineFreq.\n 
    4. <b>PWM duty cycle updating</b> basing on overmodulation or not (\ref OVER_MODULATION).\n 
    The PWM array indexing is done by shifting the Phase Value. Because of 8bit micro core, the maximum array index
    value is 256. To extend this range to 512 and cover the full dPWM table the carry bit value is used to extend range from 8 to 9 bit.\n
    \dot
      digraph ManageTacho {
      node [shape=record, style=filled, fontname=Arial, fontsize=10];  
      edge [fontsize=8]; subgraph cluster3{	subgraph cluster31{
      u81[shape=record, label="<f7> 7|<f6> 6|<f5> 5|<f4> 4|<f3> 3|<f2> 2|<f1> 1|<f0> 0"];
      label= "u32Phase:1"; fontsize=10; style=filled; color=cyan;
      } subgraph cluster32{
      u82[shape=record, label="<f7> 7|<f6> 6|<f5> 5|<f4> 4|<f3> 3|<f2> 2|<f1> 1|<f0> 0"];
      label= "u32Phase:0"; fontsize=10; style=filled; color=deepskyblue;
      } label= "u32Phase"; fontsize=10; 	}
      subgraph cluster1{ 	subgraph cluster4{
      u84[shape=record, label="<f7> 7|<f6> 6|<f5> 5|<f4> 4|<f3> 3|<f2> 2|<f1> 1|<f0> 0"];
      label= "Trashed"; fontsize=10; style=filled; color=orange; }
      subgraph cluster2{
      u83[shape=record, label="<f7> 7|<f6> 6|<f5> 5|<f4> 4|<f3> 3|<f2> 2|<f1> 1|<f0> 0"];
      C[label="Carry"]; label= "PWM Indexing"; fontsize=10; style=filled; color=green; }
      label= "bit+u8+u8"; fontsize=10; 	}
      u82:f7->C;	u82:f6->u83:f7;	u82:f5->u83:f6;	u82:f4->u83:f5;	u82:f3->u83:f4;	u82:f2->u83:f3;	u82:f1->u83:f2;	u82:f0->u83:f1;
      u81:f7->u83:f0;	u81:f6->u84:f7;	u81:f5->u84:f6;	u81:f4->u84:f5;	u81:f3->u84:f4;	u81:f2->u84:f3;	u81:f1->u84:f2;	u81:f0->u84:f1; }
    \enddot

    5. <b>Update \ref pstrElectricFreqBuff_Index </b> in order to compute \ref s32GammaAngle\n 
    6. <b>Tacho masking</b>. At tacho capture isr ( \ref INT0_TachoCapture) isr has turned off and
    re-enabled after 5/8 of previous computed electrical period. This time has expressed as pwm cycles number. Masking is done with different time if the motor is
    in start up or in running phase. This feature is implemented to avoid troubles due to tacho jitter.\n
    7. <b>New AD current convertion </b> to get current peak value. After any current convertion \ref u16MaxCurr
    will be updated and at tacho capture isr ( \ref INT0_TachoCapture) \ref u16MaxCurr will be
    copied into \ref u16PeakCurr to freeze current peak value over electrical period.\n
                            \par Used Variables
    \ref pstrElectricFreqBuff_Index -> . Used to find phi value and manage motor control\n
    \ref u8ADC_Struct_Index. Used for continuous ADC convertions.\n
    \ref buffu16ADC_READING. Used to stored ADC continuous convertions.\n
    \ref u8MTCStatus. Flags used to update or not sinus generation.\n
    \ref u32SineFreqBuf. New sinus frequency related value.\n
    \ref u32SineFreq. Actual sinus frequency related value.\n
    \ref u16SineMagBuf. New desired sinus amplitude related value.\n
    \ref u16SineMag. Actual desired sinus amplitude related value.\n
    \ref u16SineMagScaled. Actual sinus amplitude related value compensated from Vbus.\n
    \ref u32Phase. Actual electrical degree.\n
    \ref u16MaxCurr. After any current convertion \ref u16MaxCurr will be updated.\n
    \return \b void no value return
    \note Before computing sine waveform a scale operation has to be done in order to match
    \ref u16SineMag with bus voltage value.\n
    \f[ u16SineMag = \left\{\begin{matrix}
    52 &\Rightarrow& V_{\Delta }=\ 2.55\ V \\ 22 &\Rightarrow& V_{\Delta }=\ 1.07\ V
    \end{matrix}\right. \quad V_{PEAK}\ measured\ with\ FFT\ analysis \f]
    It has to be noticed that \n
    \b if u16SineMag = 52 \b so u16SineMag*PWM_MOD(max)= 52*255 = 13260 = 0x33CC
    \b and _TPM1CxV(max) = 0x0066 = 102 (for the alghorithm used).\n
    consequentely \f[ DC(max) = \frac{102}{510} = 20\% \Rightarrow V_{DPWM}(max) = 20\%\ *13\ V = 2,6\f]\n
    the fundamental has phase aplitude equal to: \n  \f[ V_{PHASE}=[\frac {V_{DPWM}(max)}{2}]*1,15\f]\n
    \f[ [\frac{Vdpwm(max)}{2}]*1,15 = 1,49\f] \n
    The delta fundamental has amplitude equal to the square root phase fundamental\n
    \f[ V_{\Delta FUNDAM}=\sqrt{3}*1,49 = 2,59 \f] as shown in FFT analysis\n
      \f[ Estimated\ execution\ time:\ \left\{\begin{matrix}
      t_{MIN} &=& 19 & \mu s \\ t_{MEAN} &=& 21.4 & \mu s \\ t_{MAX} &=& 28.4 & \mu s
      \end{matrix}\right. \f]
    \warning ASSEMBLER ROUTINE
*/
__interrupt void INT0_dpwmmin_modulation(void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/ /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8 u8LUTElement;
  s16 s16DeltaPhaseAfterZc;
  
  #ifdef BATTERY_DEGRADATION
    s16 s16IbattErr;            /* Errore sulla Ibatt.                                     */
    s16 s16IbattNextInt;        /* Risultato parziale della componente integrale su Ibatt. */
    s16 s16IbattRegResult;      /* Risultato totale del regolatore su Ibatt.               */
    s16 s16IbattRegHighLimit;   /* Limite superiore del regolatore su Ibatt.               */
  #endif  /* BATTERY_DEGRADATION */

  #if ( ( ROTATION == CCW ) || defined(RVS) )
    u16 u16PWMDtyTemp;
  #endif  /* ( ( ROTATION == CCW ) || defined(RVS) ) */

  u16 u16PwmTicksToUnmaskTacho_tmp; /* used for the volatile variable copy */
  u16 IbattReg_u16IbattInt_tmp;     /* used for the volatile variable copy */

  BitClear((TSCR1_TFFCA_MASK), TSCR1);                    /* N.B.                                                                                     */
                                                          /* TFFCA has to be clear in order to avoid:                                                 */
                                                          /* For TFLG2 (0x000F), any access to the TCNT register (0x0004, 0x0005) clears the TOF flag */

  pstrElectricFreqBuff_Index -> TPM2CNTBuff = (u16)TCNT;  /* First of all saving TCNT to check Vf ZC latter in order to avoid jitter; */

  BitSet((TSCR1_TFFCA_MASK), TSCR1);                      /* N.B.                                                                                                                                        */
                                                          /* TFFCA has to be set in order to avoid bad PWM isr behaviour during PWM input capture isr.                                                   */
                                                          /* It can happen during PWM input capture isr, WITHOUT USING TFFCA, clearing input capture channel -> clear output compare channel as well!!!! */
                                                          /* For TFLG1(0x000E), a read from an input capture or a write to the output compare channel (0x0010–0x001F)                                    */
                                                          /* causes the corresponding channel flag, CnF, to be cleared. For TFLG2 (0x000F), any access to the TCNT                                       */
                                                          /* register (0x0004, 0x0005) clears the TOF flag                                                                                               */

  #if ( ROTATION == CW )
    #ifdef RVS
      if ( !(BitTest(FWD_BWD_DIR,u8MTCStatus)) )
      {
        s16DeltaPhaseAfterZc = (s16)((u16)u16Phase - LUT_VF_ZC_REF_HEX_BWD);
      }
      else
      {
        s16DeltaPhaseAfterZc = (s16)((u16)u16Phase - LUT_VF_ZC_REF_HEX);
      }
    #else
      s16DeltaPhaseAfterZc = (s16)((s32)u16Phase - (s32)LUT_VF_ZC_REF_HEX);
    #endif  /* RVS */
  #elif ( ROTATION == CCW )
    #ifndef RVS
      s16DeltaPhaseAfterZc = (s16)((u16)u16Phase - LUT_VF_ZC_REF_HEX_BWD);
    #else
      if ( BitTest(FWD_BWD_DIR,u8MTCStatus) )
      {
        s16DeltaPhaseAfterZc = (s16)((u16)u16Phase - LUT_VF_ZC_REF_HEX_BWD);
      }
      else
      {
        s16DeltaPhaseAfterZc = (s16)((u16)u16Phase - LUT_VF_ZC_REF_HEX);
      }
    #endif  /* RVS */
  #endif  /* ROTATION */

  if ( s16DeltaPhaseAfterZc > 0 )
  {
    if ( (u16)s16DeltaPhaseAfterZc <= u16SineFreq ) /* Vf ZC. */
    {
      if ( (TFLG2_TOF != 0u) && (((u8)((u16)(pstrElectricFreqBuff_Index -> TPM2CNTBuff)>>8) == 0u)) ) /* If tacho overflow flag is set AND if TCNT is closed to zero from positive values:               */
      {                                                                                               /* 1. set flag ZCVF_OVFinPWMisrBit in ZCStatusBitfield                                             */
        BitSet((ZCVF_OVFinPWMisr), ZCStatusBitfield);                                                 /* 2. and manage pstrElectricFreqBuff_Index -> ZCVfOvfTemp in latter MTC_TachoOverflow isr routine */
      }
      else
      {
        /* Nop(); */
      }

      pstrElectricFreqBuff_Index -> ZCVfOvfTemp = 0;
      pstrElectricFreqBuff_Index -> ZCVfSampled = pstrElectricFreqBuff_Index -> TPM2CNTBuff;
      pstrElectricFreqBuff_Index -> DeltaVfOffset = (u16)s16DeltaPhaseAfterZc; 
      pstrElectricFreqBuff_Index -> SineFreqStored = u16SineFreq;
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

  if ((ATDSTAT0_SCF) != 0u)
  {
    buffu16ADC_READING[I_PEAK_CONV]       = ATDDR_ARR[I_PEAK_CONV];
    buffu16ADC_READING[V_BUS_CONV]        = ATDDR_ARR[V_BUS_CONV];
    buffu16ADC_READING[I_BATT_CONV_257HZ] = ATDDR_ARR[I_BATT_CONV_257HZ];
    buffu16ADC_READING[T_AMB_TLE]         = ATDDR_ARR[T_AMB_TLE];
    buffu16ADC_READING[T_MOS_CONV]        = ATDDR_ARR[T_MOS_CONV];
    /*buffu16ADC_READING[I_BATT_CONV]       = ATDDR_ARR[I_BATT_CONV];*/
    ATDCTL5 = 0x10;                                   /* Multiple ch conv and start from ch0. Convertion will run after writing ATDCTL5. */
  }
  else
  {
    ATDCTL5 = 0x10;                                   /* Multiple ch conv and start from ch0. Convertion will run after writing ATDCTL5. */
  }

#ifdef LIN_INPUT
  /* Inhibit Main PWM Computation in order to reduce CPU overload (during init procedure in order to accelerate Checksum procedure) */
  if( BoolPWMInterruptEnabled == TRUE ) 
  {
#endif  /* LIN_INPUT */

    if ( (BitTest((SINEMAG_CHANGE), u8MTCStatus)) != 0u )
    {
      BitClear((SINEMAG_CHANGE), u8MTCStatus);
      u16SineMag = u16SineMagBuf;
    }
    else
    {
      /* Nop(); */
    }

    if ( (BitTest((FREQ_CHANGE), u8MTCStatus)) != 0u )
    {
      BitClear((FREQ_CHANGE), u8MTCStatus);
      u16SineFreq = (u16)u32SineFreqBuf;
    }
    else
    {
      /* Nop(); */
    }


    #ifdef BATTERY_DEGRADATION
      if ( strMachine.u16FrequencyAppPU > EL_FREQ_FOR_DEGRADATION_PU_RES_BIT )      /* Il regolatore sulla Ibatt e' eseguito solo al di sopra di EL_FREQ_FOR_DEGRADATION_PU_RES_BIT. */
      {
        if ( IbattStateMachine.BOOLComputeSetpoint == (BOOL)TRUE )                  /* Ogni 50 us si alterna l'esecuzione di:                                                      */
        {                                                                           /*   - calcolo del setpoint di Ibatt + macchina a stati (per gestione del regolatore su Ibatt) */
            /* Calcolo del setpoint sulla corrente di batteria massima. */          /*   - esecuzione del regolatore su Ibatt                                                      */
            if ( buffu16ADC_READING[V_BUS_CONV] <= IBATT_VS_VMON_LOW_LIMIT_BIT )    /*                                                                                             */
            {                                                                       /* (Di fatto ciascun ramo e' eseguito ogni 100 us).                                            */
              IbattReg.u16IbattSp = IBATT_VS_VMON_SETPOINT_LOW_BIT;
            }
            else if ( buffu16ADC_READING[V_BUS_CONV] >= IBATT_VS_VMON_HIGH_LIMIT_BIT )
            {
              IbattReg.u16IbattSp = IBATT_VS_VMON_SETPOINT_HIGH_BIT;
            }
            else
            {
              IbattReg.u16IbattSp = (u16)(IBATT_VS_VMON_SETPOINT_LOW_BIT + (u16)((u16)((u16)((u16)buffu16ADC_READING[V_BUS_CONV]-IBATT_VS_VMON_LOW_LIMIT_BIT)*IBATT_VS_VMON_COEFF)>>IBATT_VS_VMON_COEFF_SHIFT));
            }

            IbattReg.u16IbattSp += u16IBattOffset;

            /* Macchina a stati per la logica del cambio di guadagni e tempo di integrazione. */
            if ( IbattStateMachine.BOOLForceGainFlag == (BOOL)FALSE )     /* In questo caso la logica seguente non e' inibita dal controllo motore a monte. */
            {
              s16IbattErr = (s16)((s16)buffu16ADC_READING[I_BATT_CONV_257HZ] - (s16)IbattReg.u16IbattSp);    /* Calcolo dell'errore sulla Ibatt. */

              if ( s16IbattErr <= IBATT_LEVEL_GAIN_CHANGE_BIT )           /* Se l'errore e' al piu' pari a IBATT_LEVEL_GAIN_CHANGE_BIT... */
              {
                if ( IbattStateMachine.enumGain == GAIN_HIGH )            /* ...ed il set di guadagni selezionati e' quello a banda superiore... */
                {
                  IbattReg.u8KpBatt = IBATT_PROP_GAIN_ON_BIT;             /* ...si continuano a tenere gli stessi guadagni... */
                  IbattReg.u8KiBatt = IBATT_INT_GAIN_ON_BIT;
                  IbattStateMachine.u8IbattRegCountMax = 0;               /* ...e ad eseguire il ramo di regolazione PI ogni 100 us... */
                }
                else
                {
                  IbattReg.u8KpBatt = IBATT_PROP_GAIN_OFF_BIT;                                /* ...altrimenti si impone il set di guadagni a banda inferiore... */
                  IbattReg.u8KiBatt = IBATT_INT_GAIN_OFF_BIT;
                  IbattStateMachine.u8IbattRegCountMax = IBATT_REG_OFF_INTEGRATION_TIME_BIT;  /* ...e si esegue il regolatore su Ibatt ogni IBATT_REG_OFF_INTEGRATION_TIME_BIT.             */
                                                                                              /* N.B.: IBATT_REG_OFF_INTEGRATION_TIME_BIT <=> 1 ms ed e' questo il tempo di integrazione    */
                  if ( IbattReg.u16IbattRegOut == 0u )                                        /* del regolatore sulla Ibatt. Questo cambiamento da 100 us ad 1 ms e' fatto per acquisire    */
                  {                                                                           /* risoluzione sul set di guadagni a banda inferiore, che altrimenti sarebbero troppo piccoli */
                    IbattStateMachine.enumGain = GAIN_HIGH;                                   /* per poter essere rappresentati con degli interi.                                           */
                    IbattStateMachine.u8IbattRegCount = 0;
                    IbattStateMachine.u8IbattRegCountMax = 0;       /* Nel caso in cui il regolatore sulla Ibatt si sia scaricato, si torna alle condizioni di default */
                  }                                                 /* (i.e. guadagni a banda larga + step di integrazione pari a 100 us). */
                  else
                  {
                    /* Nop(); */
                  }
                }
              }
              else                                                  /* Se l'errore e' superiore a IBATT_LEVEL_GAIN_CHANGE_BIT...                                  */
              {                                                     /* ...vuol dire che il regolatore sta lavorando per limitare la Ibatt al relativo setpoint... */
                IbattReg.u8KpBatt = IBATT_PROP_GAIN_ON_BIT;         /* ...per cui si tiene il set di guadagni a banda larga...                                    */
                IbattReg.u8KiBatt = IBATT_INT_GAIN_ON_BIT;          
                IbattStateMachine.enumGain = GAIN_LOW;              /* ... ma ci si predispone per il successivo cambio di guadagni, che avverra quando l'errore tornera' al di sotto di IBATT_LEVEL_GAIN_CHANGE_BIT. */
                IbattStateMachine.u8IbattRegCountMax = 0;           /* Lo step di integrazione resta 100 us.                                                                                                          */
              }
            }
            else                                                    /* Qualora il controllo motore a monte lo comandi...                                */
            {                                                       /* ...vengono forzate le condizioni di default.                                     */
              IbattStateMachine.enumGain = GAIN_HIGH;               /* N.B.: questa condizione si verifica quando vi e' una rampa di decelerazione.     */
              IbattStateMachine.u8IbattRegCountMax = 0;             /* In tal caso il regolatore PI deve scaricarsi il piu' rapidamente possibile,      */
              IbattReg.u8KpBatt = IBATT_PROP_GAIN_ON_BIT;           /* altrimenti si avra' un effetto wind-up sulla corrente nel motore, dovuto proprio */
              IbattReg.u8KiBatt = IBATT_INT_GAIN_ON_BIT;            /* al fatto che il delta-frequenza dato dal regolatore si estingue in tempi molto   */
            }                                                       /* piu' lunghi di quelli della rampa di velocita' comandata dalla ECU.              */

            IbattStateMachine.BOOLComputeSetpoint = FALSE;          /* Al prossimo interrupt si eseguira' il regolatore PI sulla Ibatt. */
        }
        else
        {
          if ( IbattStateMachine.u8IbattRegCount == 0u )            /* Ad ogni step di integrazione... */
          {
              /* Regolatore PI sulla Ibatt. */
              s16IbattErr = (s16)((s16)buffu16ADC_READING[I_BATT_CONV_257HZ] - (s16)IbattReg.u16IbattSp);      /* ...si aggiorna l'errore sulla Ibatt... */
              s16IbattRegHighLimit = (s16)((s16)strMachine.u16FrequencyAppPU - (s16)EL_FREQ_MIN_PU_RES_BIT);   /* ...e il limite superiore del regolatore PI, sulla base del massimo delta-frequenza imponibile (rispetto alla velocita' minima). */

              IbattReg.s16IbattProp = (s16)((s16)s16IbattErr*(s16)IbattReg.u8KpBatt);                          /* Si calcola la parte propoprzionale del regolatore... */

              IbattReg_u16IbattInt_tmp = IbattReg.u16IbattInt;                                                 /* ...e la parte integrale. */
              s16IbattNextInt = (s16)((s16)IbattReg_u16IbattInt_tmp + (s16)((s16)((s16)s16IbattErr*(s16)IbattReg.u8KiBatt)>>IBATT_INT_GAIN_SHIFT)); /*PRQA S 4533 #Signed shift ok: compiler manages it properly*/

              if ( ( s16IbattNextInt > 0 ) && ( s16IbattNextInt <= s16IbattRegHighLimit ) )
              {
                IbattReg.u16IbattInt = (u16)s16IbattNextInt;    /* La memoria integrale evolve solo se si e' nei limiti del regolatore PI. */
              }
              else
              {
                if ( s16IbattNextInt <= 0 )    /* Al di sotto del limite inferiore (i.e. 0)... */
                {
                  IbattReg.u16IbattInt = 0;    /* ...la parte integrale viene azzerata. */
                }
                else
                {
                  /* Nop(); */
                }
              }

              IbattReg_u16IbattInt_tmp = IbattReg.u16IbattInt;
              s16IbattRegResult = (s16)((s16)IbattReg.s16IbattProp + (s16)IbattReg_u16IbattInt_tmp);   /* Si calcola il risultato del regolatore... */
              if ( (s16)s16IbattRegResult <= 0 )
              {
                IbattReg.u16IbattRegOut = 0;
              }
              else
              {
                if ( s16IbattRegResult > s16IbattRegHighLimit )
                {
                  IbattReg.u16IbattRegOut = (u16)s16IbattRegHighLimit;
                }
                else
                {
                  IbattReg.u16IbattRegOut = (u16)s16IbattRegResult;                           /* ...e lo si assegna solo se esso risulta entro i limiti del regolatore stesso */
                }                                                                             /* (i.e. gli stessi della parte integrale, ovviamente, cosi' da non avere wind-up). */
              }
              
            IbattStateMachine.u8IbattRegCount = IbattStateMachine.u8IbattRegCountMax;         /* Il prossimo step di integrazione sara' il tempo deciso nella macchina a stati. */
          }
          else
          {
            IbattStateMachine.u8IbattRegCount--;                                              /* Timer per eseguire il regolatore PI ogni IbattStateMachine.u8IbattRegCountMax. */
          }
          
          IbattStateMachine.BOOLComputeSetpoint = TRUE;                                       /* Al prossimo interrupt si eseguira' la macchina a stati per gestire il regolatore sulla Ibatt. */
        }

        u16SineFreq = (u16)((u32)((u32)SINEFREQ_FREQ_RESOL_PU * (u16)((u16)strMachine.u16FrequencyAppPU - IbattReg.u16IbattRegOut))>>16);    /* Aggiornamento della frequenza del modulatore col contributo del regolatore sulla Ibatt. */
      }
      else
      {
        /* Nop(); */
      }
    #endif  /* BATTERY_DEGRADATION */

    
    u16SineMagScaled = (u16)((u32)((u32)u16SineMag*(u16)(V_BUS_REFERENCE_BIT))/(u16)buffu16ADC_READING[V_BUS_CONV]);
    
    if ( u16SineMagScaled > (u16)(SINEMAGSCALED_MAX) )
    {
      u16SineMagScaled = (u16)(SINEMAGSCALED_MAX);
    }
    else
    {
      /* Nop(); */
    }

    u16Phase += u16SineFreq;               /* Aggiornamento angolo di fase.  */
    /* u16PhaseAux = u16Phase; */          /* Angolo di fase da 32 a 16 bit. */

    /******************** Modulazione PWM di controllo motore ********************/
      /******* (Nuova) Sin-3a armonica *******/
    #ifdef PULSE_ERASING    
      if ( ( u16SineMagScaled >= PWM_MODULO ) || (( BitTest((PULSE_ERASE), u8MTCStatus) ) != (BOOL)FALSE) )  /* >= PWM_MODULO, e non >, anche per poter lavorare con operazioni in u16 con la modulazione discontinua */
    #else                                                                                                    /* (caso particolare [u8LUTElement = 128 ; u16SineMagScaled = 512] => 128*512 = 65536).                  */
      if  ( u16SineMagScaled >= PWM_MODULO )
    #endif  /* PULSE_ERASING */
      {
        u8LUTElement = PWM_MOD_3TH[(u8)((u16)u16Phase>>MODULATION_PRESCALER_SHIFT)];
        if ( u8LUTElement >= SIN_3TH_ARM_OFFSET_VALUE_BIT )
        {
          PWMDTY45 = (u16)((u16)SIN_3TH_ARM_CENTER_VALUE_BIT + (u16)((u16)((u16)((u16)((u16)u8LUTElement - SIN_3TH_ARM_OFFSET_VALUE_BIT)*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT));
                                                                                      /* N.B.: gli shift >>8 e <<1 equivalgono complessivamente a dividere per 128. */
        }                                                                             /* Organizzarli in questa maniera permette di associare, in Assembler, */
        else                                                                          /* a >>8 l'istruzione TFR, il cui costo computazionale e' minore di una */
        {                                                                             /* sequenza di shift molteplici. */
          PWMDTY45 = (u16)((u16)((u16)((u16)((u16)SIN_3TH_ARM_OFFSET_VALUE_BIT - u8LUTElement)*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);
                                                                                     /* Lo stesso vale per tutti gli shift con un indice multiplo di 8 (i.e. >>24). */
          if ( PWMDTY45 < SIN_3TH_ARM_CENTER_VALUE_BIT )
          {
            PWMDTY45 = (u16)((u16)SIN_3TH_ARM_CENTER_VALUE_BIT - PWMDTY45);
          }
          else
          {
            PWMDTY45 = 0;
          }
        }

        u8LUTElement = PWM_MOD_3TH[(u8)((u16)((u16)u16Phase+PHASE_OFFSET_120_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
        if ( u8LUTElement >= SIN_3TH_ARM_OFFSET_VALUE_BIT )
        {
          PWMDTY23 = (u16)((u16)SIN_3TH_ARM_CENTER_VALUE_BIT + (u16)((u16)((u16)((u16)((u16)u8LUTElement - SIN_3TH_ARM_OFFSET_VALUE_BIT)*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT));
        }
        else
        {
          PWMDTY23 = (u16)((u16)((u16)((u16)((u16)SIN_3TH_ARM_OFFSET_VALUE_BIT - u8LUTElement)*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);
          if ( PWMDTY23 < SIN_3TH_ARM_CENTER_VALUE_BIT )
          {
            PWMDTY23 = (u16)((u16)SIN_3TH_ARM_CENTER_VALUE_BIT - PWMDTY23);
          }
          else
          {
            PWMDTY23 = 0;
          }
        }

        u8LUTElement = PWM_MOD_3TH[(u8)((u16)((u16)u16Phase+PHASE_OFFSET_240_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
        if ( u8LUTElement >= SIN_3TH_ARM_OFFSET_VALUE_BIT )
        {
          PWMDTY01 = (u16)((u16)SIN_3TH_ARM_CENTER_VALUE_BIT + (u16)((u16)((u16)((u16)((u16)u8LUTElement - SIN_3TH_ARM_OFFSET_VALUE_BIT)*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT));
        }
        else
        {
          PWMDTY01 = (u16)((u16)((u16)((u16)((u16)SIN_3TH_ARM_OFFSET_VALUE_BIT - u8LUTElement)*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);
          if ( PWMDTY01 < SIN_3TH_ARM_CENTER_VALUE_BIT )
          {
            PWMDTY01 = (u16)((u16)SIN_3TH_ARM_CENTER_VALUE_BIT - PWMDTY01);
          }
          else
          {
            PWMDTY01 = 0;
          }
        }
      }
      else
      {
        /******* (Nuova) Discontinua *******/
        #ifdef GROUND_BALANCE
          if ( BOOLHwRefFlag == (BOOL)TRUE )
          {
            u8LUTElement = PWM_MOD[(u8)0];
            PWMDTY45 = (u16)((u16)((u32)((u32)u8LUTElement*GROUND_BALANCE_DUTY_STEP_SCALED*u16HwRefTimer)>>(u8)((u16)(GROUND_BALANCE_DUTY_SHIFT+(u16)MODULATION_PRESCALER_SHIFT)))<<MODULATION_SCALER_SHIFT);

            u8LUTElement = PWM_MOD[(u8)((u16)((u16)PHASE_OFFSET_120_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
            PWMDTY23 = (u16)((u16)((u32)((u32)u8LUTElement*GROUND_BALANCE_DUTY_STEP_SCALED*u16HwRefTimer)>>(u8)((u16)(GROUND_BALANCE_DUTY_SHIFT+(u16)MODULATION_PRESCALER_SHIFT)))<<MODULATION_SCALER_SHIFT);

            u8LUTElement = PWM_MOD[(u8)((u16)((u16)PHASE_OFFSET_240_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
            PWMDTY01 = (u16)((u16)((u32)((u32)u8LUTElement*GROUND_BALANCE_DUTY_STEP_SCALED*u16HwRefTimer)>>(u8)((u16)(GROUND_BALANCE_DUTY_SHIFT+(u16)MODULATION_PRESCALER_SHIFT)))<<MODULATION_SCALER_SHIFT);
          }
          else
          {
            u8LUTElement = PWM_MOD[(u8)((u16)u16Phase>>MODULATION_PRESCALER_SHIFT)];
            PWMDTY45 = (u16)((u16)((u16)((u16)u8LUTElement*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);

            u8LUTElement = PWM_MOD[(u8)((u16)((u16)u16Phase+PHASE_OFFSET_120_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
            PWMDTY23 = (u16)((u16)((u16)((u16)u8LUTElement*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);

            u8LUTElement = PWM_MOD[(u8)((u16)((u16)u16Phase+PHASE_OFFSET_240_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
            PWMDTY01 = (u16)((u16)((u16)((u16)u8LUTElement*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);
          }
        #else
          u8LUTElement = PWM_MOD[(u8)((u16)u16Phase>>MODULATION_PRESCALER_SHIFT)];
          PWMDTY45 = (u16)((u16)((u16)((u16)u8LUTElement*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);

          u8LUTElement = PWM_MOD[(u8)((u16)((u16)u16Phase+PHASE_OFFSET_120_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
          PWMDTY23 = (u16)((u16)((u16)((u16)u8LUTElement*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);

          u8LUTElement = PWM_MOD[(u8)((u16)((u16)u16Phase+PHASE_OFFSET_240_DEG_16BIT)>>MODULATION_PRESCALER_SHIFT)];
          PWMDTY01 = (u16)((u16)((u16)((u16)u8LUTElement*u16SineMagScaled)>>MODULATION_PRESCALER_SHIFT)<<MODULATION_SCALER_SHIFT);
        #endif  /* GROUND_BALANCE */
      }

      #ifdef PULSE_ERASING
        if ( (BitTest((PULSE_ERASE), u8MTCStatus)) != (BOOL)FALSE )
        {
          if ( PWMDTY45 >= PULSE_CANC_DUTY_HIGH_BIT )
          {
            PWMDTY45 = PWM_DUTY_MAX;
          }
          else if ( PWMDTY45 <= PULSE_CANC_DUTY_LOW_BIT )
          {
            PWMDTY45 = PWM_DUTY_MIN;
          }
          else
          {
            /* Nop(); */
          }

          if ( PWMDTY23 >= PULSE_CANC_DUTY_HIGH_BIT )
          {
            PWMDTY23 = PWM_DUTY_MAX;
          }
          else if ( PWMDTY23 <= PULSE_CANC_DUTY_LOW_BIT )
          {
            PWMDTY23 = PWM_DUTY_MIN;
          }
          else
          {
            /* Nop(); */
          }

          if ( PWMDTY01 >= PULSE_CANC_DUTY_HIGH_BIT )
          {
            PWMDTY01 = PWM_DUTY_MAX;
          }
          else if ( PWMDTY01 <= PULSE_CANC_DUTY_LOW_BIT )
          {
            PWMDTY01 = PWM_DUTY_MIN;
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
      #endif  /* PULSE_ERASING */

      #if ( ROTATION == CW )
        #ifdef RVS
          if ( !(BitTest(FWD_BWD_DIR,u8MTCStatus)) )
          {
            u16PWMDtyTemp = PWMDTY45;
            PWMDTY45 = PWMDTY01;
            PWMDTY01 = u16PWMDtyTemp;
          }
          else
          {
            /* Nop(); */
          }
        #endif  /* RVS */
      #elif ( ROTATION == CCW )
        #ifndef RVS
          u16PWMDtyTemp = PWMDTY45;
          PWMDTY45 = PWMDTY01;
          PWMDTY01 = u16PWMDtyTemp;
        #else
          if ( BitTest(FWD_BWD_DIR,u8MTCStatus) )
          {
            u16PWMDtyTemp = PWMDTY45;
            PWMDTY45 = PWMDTY01;
            PWMDTY01 = u16PWMDtyTemp;
          }
          else
          {
            /* Nop(); */
          }
        #endif  /* RVS */
      #endif  /* ROTATION */
    /******************** Modulazione PWM di controllo motore ********************/

    if ( (BitTest((ZCCurrFilter), ZCStatusBitfield)) != 0u )                      /* If current zero crossing filtering is off jump */
    {
      u16PWMCount++;
      u16PwmTicksToUnmaskTacho_tmp = u16PwmTicksToUnmaskTacho;

      if ( u16PWMCount > u16PwmTicksToUnmaskTacho_tmp )                           /* Depending on C_D_ISR_Status status can happen:                                                                                    */
      {                                                                           /* - if BLIND PHASE -> u16PwmTicksToUnmaskTacho = STARTUP_ANTI_RINGING ( set into MTC_InitMTCVariables() routine ) -> Masking = 21mS */
        BitClear((ZCCurrFilter), ZCStatusBitfield);                               /* - if RUN PHASE   -> u16PwmTicksToUnmaskTacho = 5/8 of previous electrical period.                                                 */
        INTERRUPT_TACHO_ENABLE;
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
#ifdef LIN_INPUT 
  } /* Bool BoolPWMInterruptEnabled == TRUE */
  else
  {
    /* Bool BoolPWMInterruptEnabled == FALSE */
    /* Nop(); */
  }
#endif  /* LIN_INPUT */

  if ( u16MaxCurr < (u16)buffu16ADC_READING[I_PEAK_CONV] )
  {                                          
    u16MaxCurr = (u16)buffu16ADC_READING[I_PEAK_CONV];                          /* New local ( into electrical period) peak current. */
  }
  else
  {
    /* Nop(); */
  }

  if ( (u16UpdateLoopTimer) != 0u )
  {
    u16UpdateLoopTimer--;
  }
  else
  {
    /* Nop(); */
  }

  if ( (u16UpdateLoopTimer) == 0u )
  {
    BitSet((UPDATE_LOOP), C_D_ISR_Status);
    u16UpdateLoopTimer = u16UpdateLoopTime;
  }
  else
  {
    /* Nop(); */
  }

  #ifdef GROUND_BALANCE
    if ( u16HwRefTimer < GROUND_BALANCE_TIME_BIT )
    {
      PWM_failsafe_flg = (BOOL)TRUE; /* OK -> INT0_dpwmmin_modulation is running */
      u16HwRefTimer++;
    }
    else
    {
      BOOLHwRefFlag = FALSE;
    }
  #endif  /* GROUND_BALANCE */

  INTERRUPT_PWM_NEW_CAPTURE;  /* N.B.                                                                                                    */
                              /* Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare */
                              /* channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared                     */
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
