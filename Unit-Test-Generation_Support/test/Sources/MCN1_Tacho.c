/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MCN1_Tacho.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MCN1_Tacho.c
    \version see \ref FMW_VERSION
    \brief Tacho high level management and calculations.
    \details Here are regrouped all functions related to Tacho managing and calculation
    In line of principle all functions that are contained in this files have to drive Level 0 routines
    and will use and provides variables for routines in other Levels. In particular in file 
    \ref INT0_TachoIsrS08.c \ref INT0_TachoIsrS12.c are regrouped the fast Interrupt Service Routines that provide the basic low level
    measurements for speed and current zero crossing calculations. 
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif /* _CANTATA_ */

#include "SPAL_Setup.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include INI0_INIT          /* Per MTC_CLOCK */
#include "main.h"           /* Per strDebug */
#include "MCN1_acmotor.h"   /* it has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"       /* Public motor control peripheral function prototypes*/ 
#include MAC0_Register      /* Needed for macro definitions */
#include "MCN1_Tacho.h"     /* Needed for macro definitions */
#include "EMR0_Emergency.h"
#include "SOA1_SOA.h"
#include "main.h"
#include "TMP1_Temperature.h"
#include "VBA1_Vbatt.h"
#include "INT0_PWMIsrS12.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* -------------------------- Private Constants ----------------------------*/
#define OVF_INIT_VALUE (u16)(1000)

/* -------------------------- Public variables -----------------------------*/
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
/** 
    \var u16PwmTicksToUnmaskTacho
    \brief Number of pwm pulses before re-enabling tacho isr. 
    \details Once tacho happens, tacho period is measured (@ 5Mhz) and tacho capture is disabled for
    5/8 of measured period.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var u8TachoOverflowNumber
    \brief Flags for motor ISR status 
    \details In this u8 variable are regrouped some flag for PWM ISR use. This flags are useful to 
    control the PWM ISR by the main loop.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
    
    \var bufstrElectricFreqBuff[SPEED_FIFO_SIZE]
    \brief Main variables for PWM ISR to calc the Phi value
    \details This variable is a structure that regroups the main variables used and managed to link PWM ISR
    and main loop alghoritm. The struct declaration \ref DeltaStruct_s (for s8) or \ref pstrElectricFreqBuff_Index (for s12) is a mixture of variables used only in the ISR
    and others updated also in the main loop. The main purpose of this structure is to contain all the needed value to 
    calc precisely the phi value. Dimension of this array is defined by \ref SPEED_FIFO_SIZE
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var pstrElectricFreqBuff_Index
    \brief Pointer to the Tacho Data buffer
    \details Pointer to \ref bufstrElectricFreqBuff array. It has used to computed electrical frequency and delta and gamma angles.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable

    \var boolOVFVsTacho
    \brief Variable used to mask Tacho 
    \details This variable is used to mask Tacho for a designed period fraction. It's incremented and tested in the
    PWM ISR.
    \note This variable is allocated in the \b ZERO Page
    \warning This is a \b GLOBAL variable
*/
volatile u16 u16PwmTicksToUnmaskTacho;
u8 u8TachoOverflowNumber;
ElectricFreq_s bufstrElectricFreqBuff[SPEED_FIFO_SIZE];
volatile PElectricFreq_s pstrElectricFreqBuff_Index;
BOOL boolOVFVsTacho;

/* -------------------------- Private variables --------------------------- */
/**     
    \var u32TachoToStart
    \brief Tacho period calculated starting from Input Setpoint
    \details This is a buffer to store the value for the minimum tacho period required for starting
    in Run_On_Fly mode. This is equivalent to impose a speed max for the Tacho checking speed.
    \note This variable is allocated in the \b DEFAULT Page
    \warning This is a \b STATIC variable
    
*/
static u32 u32TachoToStart;

#pragma DATA_SEG DEFAULT

/**
    \var u8UnderWSet
    \brief Number of Tacho measurements under the speed required during Run_On_Fly
    \details This variable keeps trace of how many times the measured speed was under the
    required speed, imposed by the Input Setpoint.
    \note This variable is allocated in the \b DEFAULT Page
    \warning This is a \b STATIC variable
    
    \var u8OverWMin
    \brief Number of Tacho measurements over the minumum speed allowed for Run_On_Fly
    \details This variable keeps trace of how many times the measured speed was over the
    minimum speed allowed to consider a start in Run_On_FLy modality.
    \note This variable is allocated in the \b DEFAULT Page
    \warning This is a \b STATIC variable
*/

static u8 u8UnderWSet;
static u8 u8OverWMin;

/* -------------------------- Private functions --------------------------- */
static u32 Tacho_GetLastTachoPeriod(void);

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Tacho_InitTachoVariables(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Initialization of software Tacho variables
    \details This routine initialize all the software variables relied to Tacho measurement and
    management. Whith a dummy pointer every field of \ref bufstrElectricFreqBuff is crushed to 
    \ref OVF_INIT_VALUE value. At the end the pstrElectricFreqBuff_Index pointer is initialized to the
    first element.
                           \par Used Variables
    \ref u8UnderWSet Number of Tacho measurements under the speed required during Run_On_Fly \n
    \ref u8OverWMin Number of Tacho measurements over the minumum speed allowed for Run_On_Fly \n
    \ref u8TachoOverflowNumber Variables needed to calculate Delta angle \n
    \ref boolOVFVsTacho Buffer to check maximal current \n
    \ref u16PWMCount Peak current measured during past electrical period \n
    \ref bufstrElectricFreqBuff Global Buffer for time events calculation purpose \n
    \ref pstrElectricFreqBuff_Index Global Buffer Pointer for time events calculation purpose \n
    \return \b void No value returned from this function
    \note The dummy pointer is allocated only for this function and at the end de-allocated
    \warning none
*/
void Tacho_InitTachoVariables(void)
{
  u8UnderWSet           = 0;
  u8OverWMin            = 0;
  u8TachoOverflowNumber = 0;          /* Clear overflow counter */
  boolOVFVsTacho        = FALSE;
  u16PWMCount           = 0;

  {                                   /* Clear tacho buffer */
    u8 i;

    for(i = (u8)0; i< SPEED_FIFO_SIZE; i++)
    {
       bufstrElectricFreqBuff[i].u16ActualCapture = OVF_INIT_VALUE;
       bufstrElectricFreqBuff[i].u16LastCapture = OVF_INIT_VALUE;
       bufstrElectricFreqBuff[i].u16NumOverflow = OVF_INIT_VALUE;
    }

    pstrElectricFreqBuff_Index = bufstrElectricFreqBuff;
  }
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Tacho_ResetTachoOverflowNumber(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Crush u8TachoOverflowNumber to 0
    \details Crush u8TachoOverflowNumber to 0
                           \par Used Variables
    \ref u8TachoOverflowNumber Number of Overflows occurred between two Tacho IC events \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void Tacho_ResetTachoOverflowNumber(void)
{
  u8TachoOverflowNumber = 0;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Tacho_SetFirstRofMaskingTime(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set the first tacho masking after rof.
    \details Tacho_ManageTachoCapture() routine computes masking basing on current ZC event but till now tacho sensiing is BEMF
    That's the reason if correcting first tacho masking time before enabling.
    Tacho_SetFirstRofMaskingTime() calling has to stand after Tacho_ManageTachoCapture() in order to overwrite
    u16PwmTicksToUnmaskTacho value and before enabling output. 
  
                           \par Used Variables
    \ref u16PwmTicksToUnmaskTacho Number of pwm ticks in which keeping tacho capture off.\n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void Tacho_SetFirstRofMaskingTime(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16PwmTicksToUnmaskTacho >>= 1;             /* Take into account, when enState = RUN_ON_FLY serving Tacho_ManageTachoCapture() routine will write u16PwmTicksToUnmaskTacho     */
                                              /* basing on current tacho position as starting masking. At this time however TACHO POSITION IS STILL BASED ON BEMF and so         */
                                              /* is 120 electrical degrees in advance. That's the reason of reducing mask from near a half electrical period to a quarter of it. */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Tacho_SetTachoCount(u16 u16SetPoint)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Compute the Tacho period to Start using the Input Setpoint
    \details Here the Tacho period to Start is calculated taking as Input the incoming setpoint.\n
    The function implemented is a simple division since it's required the period for the Run_On_Fly
    and it's known the frequency to achieve.
    \f[
    T_{MIN}=
                \frac{K_{NORMALIZATION}}{SpeedRequired}
    \f]    \f[
    u32TachoToStart=
                \frac{TACHO128FREQCOUNT}{u16SetPoint}
    \f]
  
                           \par Used Variables
    \ref u32TachoToStart Number of Overflows occurred between two Tacho IC events \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void Tacho_SetTachoCount(u16 u16SetPoint) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u32TachoToStart = (u32)((u32)((u32)((u32)TACHOFREQCOUNT/u16SetPoint)*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u8 Tacho_GetOverWminTachoPeriod(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns u8OverWMin value
    \details Returns u8OverWMin value
                           \par Used Variables
    \ref u8OverWMin Static variable confined in \ref MCN1_Tacho.c \n
    \return \b u8OverWMin This function return this static variable to allow its value use.
    \note none
    \warning none
*/
u8 Tacho_GetOverWminTachoPeriod(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  return(u8OverWMin);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Tacho_ClearOverUnderWminTachoCount(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Clear Tacho counter for Run_On_Fly
    \details Clear the two Tacho counter for Run_On_Fly.
                           \par Used Variables
    \ref u8OverWMin Static variable confined in \ref MCN1_Tacho.c \n
    \ref u8UnderWSet Static variable confined in \ref MCN1_Tacho.c \n
    \return \b void No value returned from this function
    \note none
    \warning none
*/
void Tacho_ClearOverUnderWminTachoCount(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u8OverWMin  = 0;
  u8UnderWSet = 0;
}

/******************************************************************************
                          CALCOLO VELOCITA'
                          
Il calcolo della velocita' avviene utilizzando il canale 0 del timer 2.
Il timer 2 e' impostato con un clock a 5MHz ed e' attivo l'input capture del 
canale 0 con interrupt sul fronte di salita.
Dato l'esteso range di frequenza del segnale in ingresso [1Hz..200Hz] e la 
risoluzione richiesta [decimo di Hz] e' necessario un clock elevato con 
conseguenti overflow del timer durante il conteggio di un periodo del segnale in
ingresso. E' attivo quindi l'interrupt di overflow del canale 2 che tiene 
traccia del numero di overflow avvenuti.
Tale interrupt ha priorita' inferiore rispetto all'interrupt dell'input capture.
Per questo motivo, nel caso di arrivo quasi contemporaneo dei due interrupt e'                          
stato necessario inserire una logica di gestione del conteggio degli overflow per
evitare conteggi errati.
Se l'interrupt di input capture verifica che il valore del timer catturato e' 
piccolo, verifica anche che l'arrivo di un precedente interrupt di overflow sia 
gia' stato gestito. Se cosi' non e', la routine di gestione dell'interrupt di 
input capture incrementa la variabile che tiene traccia degli overflow e abbassa 
il flag di segnalazione di overflow. La routine di gestione dell'overflow per 
prima cosa testa il proprio flag di overflow per verificare se la condizione non 
sia gia' stata gestita dalla routine di input capture a priorita' piu' alta. Se non
e' ancora stata gestita allora avviene l'incremento della variabile che tiene 
traccia del numero di overflow.
                              
******************************************************************************/

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Tacho_CheckTachoOverflow(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Checks if the rotor speed is too low
    \details This function check if the timer Overflow number overcomes a limits defined by
    \ref NUM_OVF_AT_F_MIN. If it's so, the three phase bridge is stopped and placed in an emergency state
    and an error is returned. Otherwise it returns zero.
                           \par Used Variables
    \ref u8TachoOverflowNumber Number of Timer Overflows occurred between two Tacho events\n
    \return \b Bool This function return a boolean value that is TRUE if the Tacho Period is too long.
    \note none
    \warning none
*/
BOOL Tacho_CheckTachoOverflow(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  if ( u8TachoOverflowNumber > (u8)NUM_OVF_AT_F_MIN )
  {
    EMR_EmergencyDisablePowerStage();
    ret = (BOOL)TRUE;
  }
  else
  {
    ret = (BOOL)FALSE;
  }
  return (ret);
}

/** 
    \fn void Tacho_ManageTachoCapture(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Tacho Measurement Management
    \details Main purpose of this function is to take all the measurements done on Tacho inputs and 
    to calculate the electrical period using this data. At first, the routine checks if there is to take care
    about some Timer Overflows.  \n
    If there aren't Overflows to compute, the electrical period is:\n
    \f[ |u32CountBuffer|= TIMER_{N}-TIMER_{N-1} \f]
    Otherwise is:\n
    \f[ |u32CountBuffer|= TIMER_{N}+TIMER_{N-1}-N_{OVERFLOW}*T_{TIMER PERIOD} \f]
    After having calculated the electrical period, if the \ref PHASE_INITIAL_SPEED flag is low, the routine starts to 
    evaluate information carried by Tacho data. If \ref PHASE_NOT_LOCKED is high the routine checks if the inertial rotor
    speed is enought to start driving motor in Run_On_Fly; otherwise the motor is in RUN mode so the routine check if rotor
    speed stay between two limits.
          \dot  digraph ManageTacho {
          node [shape=box, style=filled, fontname=Arial, fontsize=10]; edge [fontsize=8];
          subgraph cluster2{ FLY[shape=record, label="{ Test if Speed \< Wset | Test if Speed \> Wmin | Test if ready to lock }"];
          PREP[label="Init Run_On_Fly lock"]; FLY->PREP[label="Ready to lock",color=green];
          label= "case: IDLE/RUN ON FLY"; fontsize=8; }
          subgraph cluster1{ RUN[shape=record, label="{Check W protection | Check Voltage Zero Crossing | Compute Voltage Zero Crossing | Compute Delta Angle |Compute Delta Phase}"];
          label= "case: RUN"; fontsize=8; }
          Start[shape=ellipse, label="Reset"];
          Test1[label="NumOverFlow=0?", fillcolor=yellow];
          Test2[label="PHASE_INITIAL_SPEED?", fillcolor=yellow];
          Test3[label="PHASE_NOT_LOCKED?", fillcolor=yellow];
          End[shape=ellipse, label="End"];
          subgraph cluster5{ CalcNo[label="Period=Timer[n]-Timer[n-1]"];
          CalcYes[label="Period=Timer[n]+(TimerPeriod-Timer[n-1])+TimerPeriod*(nOVF-1)"];
          label= "Compute Electrical Period"; fontsize=8; }
          Start->Test1[]; PREP->End[]; FLY->End[];RUN->End[];
          Test1->CalcNo[label="NO",color=red]; Test1->CalcYes[label="YES",color=green];
          CalcNo->Test2[]; CalcYes->Test2[];
          Test3->RUN[label="NO",color=red]; Test2->FLY[label="NO",color=red];
          }  \enddot    
    To reject the Tacho ringing a masking of the measurement is previewed. This is managed with the PWM interrupt
    that provide the time base and the reenabling routines.\n
    If the motor is in running mode, this routine compute the delta angle needed to keep current phased with the Back-EMF.
    The conceptual formula that rules this behaviour is \n
    \f[ \varphi =\frac{T_{ZERO\ CROS}^{CURRENT}-T_{ZERO\ CROS}^{CURRENT}}{T_{PERIOD}^{TACHO}}  \f]
    where the angle phi is shown in the picture below.\n    
    \image rtf AngleName.PNG "Convention used to call angles between vectors" height=10cm   
    \image html AngleName.PNG "Convention used to call angles between vectors" height=10cm   
    The Tacho zero crossing event measurement is associated with an \a Input \a Capture event; so the measure is taken with a 
    great precision because the capture event is triggered with an hardware device. instead the voltage zero crossing event is
    taken by reaading the Timer value at the beginning of the PWM ISR; in addition it's not mandatory that the PWM array cell with
    the zero value event is directly imposed during the electrical period. So it's needed an additional value that have to be added 
    to increase precision. So, the final formula implemented is
    \f[\Delta_{ZeroCros}=\varphi =ZcI_{sampled}+(T_{PERIOD}^{TIMER}-ZcV_{sampled})+ZcV_{offset}+T_{PERIOD}^{TIMER}*N_{OVFs}\f]
    Value of the voltage zero crossing offset is described in notes.\n    

                           \par Used Variables
    \ref pstrElectricFreqBuff_Index_Buff Pointer to the Tacho Data buffer \n
    \ref C_D_ISR_Status Variable with all the ISR sincronization flags \n
    \ref u8UnderWSet Number of Tacho measurements under the Input speed required \n 
    \ref u8OverWMin Number of Tacho measurements over the minimum speed required \n
    \ref u16ErrorType Error flags \n
    \ref s32GammaAngle Output of the Delta Phase Loop for current \n
    \ref u8MTCStatus General motor control Flags \n
    \ref ARRAY_PWM_MOD Const PWM value array \n
    \ref pstrElectricFreqBuff_Index Data for delta calulation \n
    \ref u16DeltaAngle Phase Error Required for the current setpoint \n
    \return \b void No value returned from this function
    \note The implemented alghoritm needs a fine mesurement of the delta between voltage and current
    zero crossing. The timer reserved for time measurements is the timer module (TIM); in \ref Init_InitTacho it is 
    configured to count with a period of 200 ns (frequency of 5 MHz). If the PWM frequency is 20 kHz as imposed 
    by system requirements, the Tck counted between two PWM ISR is:
    \f[ n_{timer module (TIM)}=\frac{5\ MHz}{20\ kHz}=\frac{5e6}{20e3}=250\ T_{CK} \f]
    So, some manipulation on the theoretical equation are done to eval the offset value and, at the same time, to optimize the code.
        \f[ \begin{align*} ZcV_{offset} &= T_{offset}=\frac{T_{offset}}{T_{PWM}}*T_{PWM}=\\ \xrightarrow[a\ triangle]{because\ it's} &= \frac{A}{B}\ *\ T_{PWM}=
        \\ &= \frac{PWM[n]-PWM[=0]} {PWM[n]-PWM[n-1]}*T_{PWM}= \\ &= \frac{PWM[n]-PWM[=0]} {PWM[n]-PWM[n-1]}*250*T_{CK}= \\ 
        \xrightarrow[250\ \simeq \ 256]{approximation} &\simeq \frac{PWM[n]-PWM[=0]} {PWM[n]-PWM[n-1]}*256=  \\
        &=\frac{(PWM[n]-PWM[=0])*2^8} {PWM[n]-PWM[n-1]}  \end{align*}\f]
    This simplify computation because all values of the last passage are known and don't require additional passages or hardware devices.\n
    Step from the first to the second line is possible because the line that interpolates the two points
    build a triangle so the two ratio are equals.
    \f[ \frac{T_{offset}}{T_{PWM}}=\frac{A}{B} \f]
            
    \image rtf TachoOffset.PNG "Variables used for Tacho Offset Computation" width=10cm   
    \image html TachoOffset.PNG "Variables used for Tacho Offset Computation" width=10cm 
                                              
    \warning none
*/
void Tacho_ManageTachoCapture(void)
{ 
  u32 u32ElPeriod;
  u16 u16OVFs;
  PElectricFreq_s pstrElectricFreqBuff_Index_Buff;

  pstrElectricFreqBuff_Index_Buff = pstrElectricFreqBuff_Index;   /* Buffer pstrElectricFreqBuff_Index pointer to avoid bad computation if current zc occours. */

  if ( pstrElectricFreqBuff_Index_Buff != (PElectricFreq_s)bufstrElectricFreqBuff )
  {
    pstrElectricFreqBuff_Index_Buff--;
  }
  else
  {
    pstrElectricFreqBuff_Index_Buff = &bufstrElectricFreqBuff[SPEED_FIFO_SIZE-1u];
  }

  if ( (BitTest((PHASE_INITIAL_SPEED), C_D_ISR_Status)) == 0u )             /* Take care of brackets between ! and BitTest. */
  {
    u32ElPeriod = (u32)((u32)((u32)((u32)TACHOFREQCOUNT/strMachine.u16FrequencyAppPU)*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ);

    u16PwmTicksToUnmaskTacho = (u16)((u32)((u32)TACHO_MASK_NUM*FREQUENCY_RES_FOR_PU_BIT)/(u32)((u32)strMachine.u16FrequencyAppPU*BASE_FREQUENCY_HZ*TACHO_MASK_DEN));

                                                                            /* Disassemble:                                                                                                                                    */
                                                                            /* ...                                                                                                                                             */
                                                                            /* STHX  u16PwmTicksToUnmaskTacho                                                                                                                  */
                                                                            /* This instruction is good since u16PwmTicksToUnmaskTacho writing in done in single time so if interrupt occours during this reading -> no error! */
                                                                            /* Even if a writing interrupt occours between the two instruction: no problem.                                                                    */

    if( pstrElectricFreqBuff_Index_Buff -> SineFreqStored == 0u )           /* N.B.                                                                                           */
    {                                                                       /* pstrElectricFreqBuff36_Index_Buff -> ZCVfOffset is computed always basing on SineFreqStored    */
      pstrElectricFreqBuff_Index_Buff -> ZCVfOffset = (u16)0;               /* at vf zc happening. Have a look at doxygen routine comment file into doc.                      */
    }                                                                       /* pstrElectricFreqBuff_Index_Buff -> ZCVfOffset is computed with following formula:              */
    else                                                                    /* pstrElectricFreqBuff_Index_Buff -> ZCVfOffset = (u32Phase - LUT_VF_ZC_REF_HEX)*256/u32SineFreq */
    {                                                                       /* where *256 takes into account 5Mhz(tacho freq count) and PWM freq (19607Hz) ratio.             */
      pstrElectricFreqBuff_Index_Buff -> ZCVfOffset = (u16)((u32)((u32)(pstrElectricFreqBuff_Index_Buff -> DeltaVfOffset)<<8)/((u16)(pstrElectricFreqBuff_Index_Buff -> SineFreqStored)) );
    }

    if( (pstrElectricFreqBuff_Index_Buff -> ZCVfNumOverflow) == 0u )
    {
      pstrElectricFreqBuff_Index_Buff -> DeltaZCVfCurr = (u32)(pstrElectricFreqBuff_Index_Buff -> ZCCurrSampled) - (u16)(pstrElectricFreqBuff_Index_Buff -> ZCVfSampled) + (u16)(pstrElectricFreqBuff_Index_Buff -> ZCVfOffset);
    }
    else
    {
      pstrElectricFreqBuff_Index_Buff -> DeltaZCVfCurr = (u32)(pstrElectricFreqBuff_Index_Buff -> ZCCurrSampled) + (u32)(~(u16)(pstrElectricFreqBuff_Index_Buff -> ZCVfSampled)) + (u16)(pstrElectricFreqBuff_Index_Buff -> ZCVfOffset);
      u16OVFs = (u16)((u16)(pstrElectricFreqBuff_Index_Buff -> ZCVfNumOverflow)-1u);
      pstrElectricFreqBuff_Index_Buff -> DeltaZCVfCurr += (u32)((u32)u16OVFs<<16);
    }

    u16OVFs = (u16)((u32)((u32)(pstrElectricFreqBuff_Index_Buff -> DeltaZCVfCurr)<<(u8)(ANGLE_RES_FOR_PU_SHIFT-8u))/(u32)((u32)u32ElPeriod>>(u8)(ANGLE_RES_FOR_PU_SHIFT-7u)));
    strMachine.s16AnglePhiPU = (s16)((s32)u16OVFs - (s16)BASE_OFFSET_ANGLE_PU_RES);

  }  /*!(C_D_ISR_Status & PHASE_INITIAL_SPEED)*/
  else
  { /*C_D_ISR_Status & PHASE_INITIAL_SPEED*/

    if( (pstrElectricFreqBuff_Index_Buff -> u16NumOverflow) == 0u )
    {
      u32ElPeriod = (u32)((u32)((u16)pstrElectricFreqBuff_Index_Buff -> u16ActualCapture ) - (u32)((u16)pstrElectricFreqBuff_Index_Buff -> u16LastCapture));
    }
    else
    {
      u32ElPeriod = (u32)pstrElectricFreqBuff_Index_Buff -> u16ActualCapture + (u32)(~(u16)(pstrElectricFreqBuff_Index_Buff -> u16LastCapture));

      if ((pstrElectricFreqBuff_Index_Buff -> u16NumOverflow) <= (u8)RUN_ON_FLY_LUT_SIZE)
      {
        u16OVFs = (u16)((pstrElectricFreqBuff_Index_Buff -> u16NumOverflow)-1u);
      }
      else
      {
        u16OVFs = (u16)((u8)RUN_ON_FLY_LUT_SIZE-1u);
      }
      u32ElPeriod += (u32)((u32)u16OVFs<<16);
    }

    if ( (u32)(u32ElPeriod) > (u32)u32TachoToStart )                            /* Check if speed is less than set point speed */
    {
      u8UnderWSet++;
    }
    else
    {
      u8UnderWSet = 0;
    }

    if ( (u32)u32ElPeriod < (u32)COUNT_TO_RUN_MAX)                              /* Check if speed is greater than minimum speed */
    {
      if ( u8OverWMin <= RUN_ON_FLY_VALID_TACHO_NUMBER )                        /* If OverWMin were incremented any time could happen unfortunatly when */
      {
        u8OverWMin++;                                                           /* OverWMin reset from 255 to 0 and RTI_IsMainTimeElapsed is over -> startup from stop */
      }
    }
    else
    {
       u8OverWMin = 0;
    }

    if ( ( u8UnderWSet > RUN_ON_FLY_VALID_TACHO_NUMBER ) && ( u8OverWMin > RUN_ON_FLY_VALID_TACHO_NUMBER ) )
    {
      BitClear((PHASE_INITIAL_SPEED), C_D_ISR_Status);
      BitSet((PHASE_LOCKED), C_D_ISR_Status);
      BitSet((PHASE_RESET), u8MTCStatus);
      u16PwmTicksToUnmaskTacho = (u16)(((u32)u32ElPeriod*5u)>>11);            /* Shift 11 since shift 8 for tacho to pwm frequency ratio and shift 3 for 8 division.                                                             */
                                                                              /* Disassemble:                                                                                                                                    */
                                                                              /* ...                                                                                                                                             */
                                                                              /* STHX  u16PwmTicksToUnmaskTacho                                                                                                                  */
                                                                              /* This instruction is good since u16PwmTicksToUnmaskTacho writing in done in single time so if interrupt occours during this reading -> no error! */
                                                                              /* Even if a writing interrupt occours between the two instruction: no problem.                                                                    */
    }
  }  /*C_D_ISR_Status == PHASE_INITIAL_SPEED*/
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn static u32 Tacho_GetLastTachoPeriod(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns the last Tacho period
    \details In order to calculate frequency this routine return the last Tacho period value.
    This is done by scrolling the buffer to recall the actual and the last timer Value.
    The function is the same implemented in the \ref Tacho_ManageTachoCapture.
    \f[
    |u32CountBuffer|= TIMER_{N}-TIMER_{N-1}+N_{OVERFLOW}*T_{TIMER PERIOD}
    \f]

                           \par Used Variables
    \return \b u32Frequency This is the actual frequency
    \note The very last tacho period acquired may not be considered for the 
    calculation if a capture occurs during averaging.
    \warning none
*/
static u32 Tacho_GetLastTachoPeriod(void)
{
  #pragma DATA_SEG SHORT _DATA_ZEROPAGE
  static PElectricFreq_s LastSpeedFIFO_Index;

  #pragma DATA_SEG DEFAULT
  u32 u32CountBuffer;
  u16 u16LastValue;
  u16 u16ThisValue;
  u16 u16Overflow;

  /* Store pointer to prevent errors due to Tacho Capture during processing */
  LastSpeedFIFO_Index = pstrElectricFreqBuff_Index;
  /*Ricerca l'ultimo elemento salvato*/
  if (LastSpeedFIFO_Index != (PElectricFreq_s)bufstrElectricFreqBuff)
  {
    LastSpeedFIFO_Index -= 1;
  }
  else
  {
    LastSpeedFIFO_Index += SPEED_FIFO_SIZE - 1u;  /*PRQA S 0488 #mandatory memory management*/
  }

  /*Acquisisci i valori dell'ultimo elemento salvato*/
  u16ThisValue   = (u16)(LastSpeedFIFO_Index->u16ActualCapture);
  u16LastValue   = (u16)(LastSpeedFIFO_Index->u16LastCapture);
  u16Overflow    = (u16)(LastSpeedFIFO_Index->u16NumOverflow);
  u32CountBuffer = (u32)(((u32)u16Overflow)<<16) + (u16)u16ThisValue - (u16)u16LastValue;

  return(u32CountBuffer);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Tacho_GetActualRotorSpeed(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>
    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns the actual rotor frequency
    \details This function recalls the last Tacho Period and compute the rotor electrical frequency 
                           \par Used Variables
    \return \b u32Frequency This is the actual frequency
    \note none
    \warning none
*/
u16 Tacho_GetActualRotorSpeed(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  u16 ret;
  u32 u32Frequency;
  u32 u32TachoPeriod;

  u32TachoPeriod = Tacho_GetLastTachoPeriod();

  if(u32TachoPeriod != 0u) 
  {
    u32Frequency = (u32)((u32)((u32)((u32)TACHOFREQCOUNT/u32TachoPeriod)*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ);

    if((u32)u32Frequency>U16_MAX) 
    {
      ret = ((u16)U16_MAX);
    }
    else
    {
      ret = ((u16)u32Frequency);
    }
  }
  else
  {
    ret = ((u16)0);
  }
  return (ret);
}

/* END */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
