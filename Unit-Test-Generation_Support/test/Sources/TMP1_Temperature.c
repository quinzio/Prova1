/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  TMP1_Temperature.c

VERSION  :  1.3

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file TMP1_Temperature.c
    \version see \ref FMW_VERSION 
    \brief Basic temperature functions.
    \details Routines for temperature operation and variables related initialization.\n
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
#include SPAL_DEF
#include "TIM1_RTI.h"
#include MAC0_Register        /* Needed for macro definitions */
#include ADM0_ADC
#include "TMP1_Temperature.h"
#include "MCN1_RegPI.h"
#include "FIR1_FIR.h"
#include "main.h"             /* Per strDebug                                      */
#include "AUTO1_AutoTest.h"   /* Per verifica parametri di taratura                */
#include "PARAM1_Param.h"     /* Per indirizzi parametri taratura                  */
#include "MCN1_acmotor.h"     /* Per strMachine.u16FrequencyAppPU                  */
#include "POWER1_MaxPower.h"  /* Per BOOLPartStallDerFlag (con diagnostica attiva) */
#include "INT0_RTIIsrS12.h"
#include "INT0_PWMIsrS12.h"
#ifdef _CANTATA_
#undef Nop
#define Nop()
#endif

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* ---------------------- Private typedefs ----------------------------------*/

/* ---------------------- Private Constants ----------------------------------*/
/** 
    
    \brief Tuning parameter for temperature measuring 
    \details This constant has to be written at tuning (test) time and used to compensate
    hw tolerances.
    \note none
    \warning none.
*/

/*#pragma DATA_SEG ROM_C000*/
  static const volatile u16 T_PARAM T_PARAM_ADDRESS         = T_PARAM_REF;      /*PRQA S 1019 #mandatory for linker behavior*/ /*PRQA S 3218 #Gives better visibility here*/
  static const volatile u16 T_MOS_PARAM T_MOS_PARAM_ADDRESS = T_MOS_PARAM_REF;  /*PRQA S 1019 #mandatory for linker behavior*/ /*PRQA S 3218 #Gives better visibility here*/

  #if defined(MOSFET_OVERTEMP_NCP18XH103F)     /* Murata NTC */

    /* MURATA NTC NCP18XH103F segment linear interpolation - 2k7 resistor */
    /* temp = ( INTERCEPT - NEGSLOPE * ADCvalue ) >> DOWNSHIFT            */
    /* ADC value thresholds (>= comparison), obtained by simulation       */

    #define NCP18XH103F_ADTHS_01                993 /* -25 °C */
    #define NCP18XH103F_ADTHS_02                962 /* -10 °C */
    #define NCP18XH103F_ADTHS_03                912 /*   5 °C */
    #define NCP18XH103F_ADTHS_04                836 /*  20 °C */
    #define NCP18XH103F_ADTHS_05                737 /*  35 °C */
    #define NCP18XH103F_ADTHS_06                391 /*  80 °C */
    #define NCP18XH103F_ADTHS_07                327 /*  90 °C */
    #define NCP18XH103F_ADTHS_08                271 /* 100 °C */
    #define NCP18XH103F_ADTHS_09                224 /* 110 °C */
    #define NCP18XH103F_ADTHS_10                185 /* 120 °C */
    #define NCP18XH103F_ADTHS_11                152 /* 130 °C */
    #define NCP18XH103F_ADTHS_12                126 /* 140 °C */
    /*#define NCP18XH103F_ADTHS_13              105 */ /* 150 °C */
    /* NEGSLOPE multiplied by 2^NCP18XH103F_DOWNSHIFT */
    #define NCP18XH103F_NEGSLOPE_01             56
    #define NCP18XH103F_NEGSLOPE_02             31
    #define NCP18XH103F_NEGSLOPE_03             19
    #define NCP18XH103F_NEGSLOPE_04             13
    #define NCP18XH103F_NEGSLOPE_05             10
    #define NCP18XH103F_NEGSLOPE_06             8
    #define NCP18XH103F_NEGSLOPE_07             10
    #define NCP18XH103F_NEGSLOPE_08             11
    #define NCP18XH103F_NEGSLOPE_09             14
    #define NCP18XH103F_NEGSLOPE_10             16
    #define NCP18XH103F_NEGSLOPE_11             19
    #define NCP18XH103F_NEGSLOPE_12             25
    #define NCP18XH103F_NEGSLOPE_13             30
    /* INTERCEPT multiplied by 2^NCP18XH103F_DOWNSHIFT */
    #define NCP18XH103F_INTERCEPT_01            54060
    #define NCP18XH103F_INTERCEPT_02            29270
    #define NCP18XH103F_INTERCEPT_03            17710
    #define NCP18XH103F_INTERCEPT_04            12210 
    #define NCP18XH103F_INTERCEPT_05            9670
    #define NCP18XH103F_INTERCEPT_06            8210
    #define NCP18XH103F_INTERCEPT_07            9030
    #define NCP18XH103F_INTERCEPT_08            9390
    #define NCP18XH103F_INTERCEPT_09            10215
    #define NCP18XH103F_INTERCEPT_10            10670
    #define NCP18XH103F_INTERCEPT_11            11220
    #define NCP18XH103F_INTERCEPT_12            12135
    #define NCP18XH103F_INTERCEPT_13            12775
    /* DOWNSHIFT - scale factor */
    #define NCP18XH103F_DOWNSHIFT               6 /* 2^6 = 64 scale factor */
    /* array size */
    #define NCP18XH103F_SEGMENTS_NUM            13
  #endif  /* MOSFET_OVERTEMP_NCP18XH103F */

#pragma DATA_SEG DEFAULT


/* ---------------------- Private variables ----------------------------------*/
/**     
    \var enumSOATempState
      \brief Enumerator for the temperature state machine.\n
      \details Enumerator for indexing the \ref SOATemperatureFunction array. this allow to call teh right 
      subroutine and define the working status.
      \note This variable is placed in \b DEFAULT segment.
      \warning This variable is \b STATIC.

    \var buffu16tmp1_TempRead
      \brief Buffer for temperature reading.\n
      \details Every time \ref Temp_GetTamb  function is called, new value is inserted into \ref buffu16tmp1_TempRead
      buffer and mean value over it is returned.
      \note This variable is placed in \b DEFAULT segment.
      \warning This variable is \b STATIC.

    \var u8tmp1_TempReadIndex
      \brief Buffer temperature index.\n
      \details Every time \ref Temp_GetTamb  function is called, new value is inserted into \ref buffu16tmp1_TempRead
      buffer and \ref u8tmp1_TempReadIndex is incremented (\ref TMP_TEMP_READ_LENGHT modulo).
      \note This variable is placed in \b DEFAULT segment.
      \warning This variable is \b STATIC.
      
    \var buffu16_TempMosRead
      \brief Buffer for MOS FET temperature reading.\n
      \details Every time \ref Temp_GetTMos  function is called, new value is inserted into \ref buffu16_TempMosRead
      buffer and mean value over it is returned.
      \note This variable is placed in \b DEFAULT segment.
      \warning This variable is \b STATIC.

    \var u8TempMosReadIndex
      \brief Buffer MOS FET temperature index.\n
      \details Every time \ref Temp_GetTMos  function is called, new value is inserted into \ref buffu16_TempMosRead
      buffer and \ref u8TempMosReadIndex is incremented (\ref TMP_TEMP_MOS_READ_LENGHT modulo).
      \note This variable is placed in \b DEFAULT segment.
      \warning This variable is \b STATIC.
      
    \var BOOLSOATemperatureCorr
      \brief used for SOA correction factor.\n
      \note This variable is placed in \b DEFAULT segment.
      \warning None.
      \details None.  
*/
static StateSOATemp_T enumSOATempState = NORMAL_TEMP;
static BOOL BOOLSOATemperatureCorr;
static DeratingMode enumDeratingMode;
static u16 u16ParamT;
static u16 u16TempStepInt;

#ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  /*static s16 s16TempMOSRead;*/
  static u16 u16TempMOSReadOld;
  static u16 u16ParamTMOS;
#endif /*MOSFET_OVERTEMPERATURE_PROTECTION*/

#if defined(MOSFET_OVERTEMP_NCP18XH103F)     /*Murata NTC*/
static const u16 NTCADCThresholds[NCP18XH103F_SEGMENTS_NUM-1] = /*PRQA S 3218 #Gives better visibility here*/ 
{
  NCP18XH103F_ADTHS_01,
  NCP18XH103F_ADTHS_02,
  NCP18XH103F_ADTHS_03,
  NCP18XH103F_ADTHS_04,
  NCP18XH103F_ADTHS_05,
  NCP18XH103F_ADTHS_06,
  NCP18XH103F_ADTHS_07,
  NCP18XH103F_ADTHS_08,
  NCP18XH103F_ADTHS_09,
  NCP18XH103F_ADTHS_10,
  NCP18XH103F_ADTHS_11,
  NCP18XH103F_ADTHS_12
};

static const u16 NTCSlopeIntercept[NCP18XH103F_SEGMENTS_NUM][2] = /*PRQA S 3218 #Gives better visibility here*/
{
  { NCP18XH103F_NEGSLOPE_01, NCP18XH103F_INTERCEPT_01 },
  { NCP18XH103F_NEGSLOPE_02, NCP18XH103F_INTERCEPT_02 },
  { NCP18XH103F_NEGSLOPE_03, NCP18XH103F_INTERCEPT_03 },
  { NCP18XH103F_NEGSLOPE_04, NCP18XH103F_INTERCEPT_04 },
  { NCP18XH103F_NEGSLOPE_05, NCP18XH103F_INTERCEPT_05 },
  { NCP18XH103F_NEGSLOPE_06, NCP18XH103F_INTERCEPT_06 },
  { NCP18XH103F_NEGSLOPE_07, NCP18XH103F_INTERCEPT_07 },
  { NCP18XH103F_NEGSLOPE_08, NCP18XH103F_INTERCEPT_08 },
  { NCP18XH103F_NEGSLOPE_09, NCP18XH103F_INTERCEPT_09 },
  { NCP18XH103F_NEGSLOPE_10, NCP18XH103F_INTERCEPT_10 },
  { NCP18XH103F_NEGSLOPE_11, NCP18XH103F_INTERCEPT_11 },
  { NCP18XH103F_NEGSLOPE_12, NCP18XH103F_INTERCEPT_12 },
  { NCP18XH103F_NEGSLOPE_13, NCP18XH103F_INTERCEPT_13 },
};
#endif  /* MOSFET_OVERTEMP_NCP18XH103F */

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  static u8 u8TambErrCount;    /* Counter of over-temperature events */
#endif  /* DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE */

/* ---------------------- Public variables ----------------------------------*/

/* ---------------------- Private functions ----------------------------------*/
static void SOATemperatureNormal(void);
static void SOATemperatureDerating(void);
static void SOATemperatureOverMax(void);    /* Empty function. Defined just to make sure that *SOATemperatureFunction pointer does not refer to a wrong address. */
static void Temp_Set_BOOLSOATemperatureCorr(BOOL boolValue);

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  static void Temp_Set_u8TambErrCount(u8 value);
  static void Temp_Inc_u8TambErrCount(void);
#endif  /* DIAGNOSTIC_STATE */

/**
    \var SOATemperatureFunction
      \brief Array of pointers to functions.\n
      \details This is an array filled with pointer to function. The index is an enumerator that have the same buffer length.
      This software solution is done to increase optimization and avoid flash unnecessary occupation.
      \note This variable is placed in \b DEFAULT segment.
      \warning This variable is \b STATIC.
*/
static void (*SOATemperatureFunction[])(void)= /*PRQA S 3218 #Gives better visibility here*/
{
    SOATemperatureNormal,
    SOATemperatureDerating,
    SOATemperatureOverMax
};

static void Temp_ReadTamb(u16 ADReading);
static void Temp_ReadTMos(u16 ADReading);

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Temp_CheckTempAmbPreRun(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns TRUE when temperature reading is out of range.  
    \details Return TRUE if:
		-the temperature converted at the right ADC channel has reached 
		 the high threshold level (\ref TLE7184F_HIGH_TEMP) or the temperature
		 previously reached this value and still over \ref DRIVER_OVER_DERATING_TEMPERATURE.
		-the temperature converted at the right ADC channel is below 
		 the low threshold level (\ref TLE7184F_LOW_TEMP) or the temperature
		 previously reached this value and still under \ref TLE7184F_LOW_TEMP + \ref TLE7184F_HYSTERESIS.\n
                            \par Used Variables
    \return \b bool Returns TRUE when temperature reading is out of the ranges.
    \note None.
    \warning None
*/
BOOL Temp_CheckTempAmbPreRun(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  s16 strTempDrive_s16TempAmbReadPU_tmp;
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION 
  s16 strTempDrive_s16TempMOSReadPU_tmp;
  #endif

  strTempDrive_s16TempAmbReadPU_tmp = INT0_Get_strTempDrive_s16TempAmbReadPU();
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION 
  strTempDrive_s16TempMOSReadPU_tmp = INT0_Get_strTempDrive_s16TempMOSReadPU();
  #endif

  /* If ambient or MOS temperature overcomes the maximum threshold, set the OVER_MAX_TEMP state */
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION 
  if ( ( strTempDrive_s16TempAmbReadPU_tmp >= T_DRIVER_SHUTDOWN_THRESHOLD_PU ) || ( strTempDrive_s16TempMOSReadPU_tmp >= T_MOSFET_SHUTDOWN_THRESHOLD_PU ) )
  #else
  if ( strTempDrive_s16TempAmbReadPU_tmp >= T_DRIVER_SHUTDOWN_THRESHOLD_PU )
  #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
  {
    /* During pre-run phase (i.e. WAIT main state), the over-temperature condition must be identified */
    /* at least TAMB_ERR_COUNT_MAX times, before to be signalized on feedback output.                 */
    /* This avoid wrong sporadic signalizations when thermal derating is active and a RoF is          */
    /* commanded (for istance). In such condition, the thermal transitory (due to RoF) could increase */
    /* the drive temperature for few moments and, without the filter below, T_AMB error could be      */
    /* signalized for a single diagnostic period.                                                     */
    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
    Temp_Inc_u8TambErrCount();
    if ( Temp_Get_u8TambErrCount() >= TAMB_ERR_COUNT_MAX )
    {
      Temp_Set_u8TambErrCount(TAMB_ERR_COUNT_MAX);
    }
    else
    {
      /* Nop(); */
    }
    #endif  /* DIAGNOSTIC_STATE */

    u8InverterStatus |= TAMB_OVER_TEMPERATURE;
    ret = (BOOL)TRUE;
  }
  else
  {
    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      Temp_Set_u8TambErrCount(0);   /* Reset the over-temperature event counter with no over-temperature. */
    #endif  /* DIAGNOSTIC_STATE */

    #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
      if ( ( ( strTempDrive_s16TempAmbReadPU_tmp >= T_DRIVER_DERATING_RESUME_THR_PU ) && ( (u8InverterStatus & TAMB_OVER_TEMPERATURE) != 0u ) ) || ( ( strTempDrive_s16TempMOSReadPU_tmp >= T_MOSFET_DERATING_RESUME_THR_PU ) && ( (u8InverterStatus & TAMB_OVER_TEMPERATURE) != 0u ) ) )
    #else
      if ( ( strTempDrive_s16TempAmbReadPU_tmp >= T_DRIVER_DERATING_RESUME_THR_PU ) && ( (u8InverterStatus & TAMB_OVER_TEMPERATURE) != 0u ) )
    #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
      {
        ret = (BOOL)TRUE; /* Wait BOTH ambient and MOS temperature to get under the DERATING threshold. */
      }
      else
      {
        u8InverterStatus &= (u8)(~TAMB_OVER_TEMPERATURE);
        ret = (BOOL)FALSE;;	/* Normal operation */
      }
    }
    return (ret);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Temp_InitTemp(void)
    \author Lorenzo Fornari  \n <em>Main Developer</em> 

    \date 13/11/2015
    \brief Initialize temperature reading, replaces Temp_InitGetTMosFilter\n
    \details None
    \return None
    \note None.
    \warning None
*/
void Temp_InitTemp(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 u16BuffTemp;

  /*T_AMB*/
  u16BuffTemp = buffu16ADC_READING[T_AMB_TLE];
  /*init value*/
  Temp_ReadTamb(u16BuffTemp);
  /*init filter*/
  FIR_SetParFilt(TEMP_TLE_FILT,TEMP_STEP_FREQ_CONST,TEMP_RC_FREQ_CONST,TEMP_RC_FREQ_CONST,TEMP_FILT_HYST_SCALED,0);
  FIR_InitVarFilt(TEMP_TLE_FILT,0,(s16)((u16)(u16BuffTemp<<TEMP_ADC_SHIFT)));

  /*T_MOS*/
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  /*u8TmosCounter = TMOS_MAX_TMOS_OVERT_EVENTS;*/     /* After reset, when Temp_CheckMosOvertemperature is called for the first time, return true if                       */
                                                      /* temperature is above maximum rating. If u8TmosCounter == 0 , Temp_CheckMosOvertemperature returns TRUE only after */
                                                      /* ..TMOS_MAX_TMOS_OVERT_EVENTS consecutive events.                                                                  */
  u16BuffTemp = (u16)buffu16ADC_READING[T_MOS_CONV];
  /*init value*/
  Temp_ReadTMos(u16BuffTemp);
  /*init filter*/
  FIR_SetParFilt(TEMP_MOS_FILT,TEMP_STEP_FREQ_CONST,TEMP_RC_FREQ_CONST,TEMP_RC_FREQ_CONST,TEMP_FILT_HYST_SCALED,0);
  FIR_InitVarFilt(TEMP_MOS_FILT,0,(s16)((u16)(u16BuffTemp<<TEMP_ADC_SHIFT)));
  u16TempMOSReadOld=u16BuffTemp;
  #endif

  INT0_Set_u8TempFiltTimer(TEMP_SAMPLING_BIT);
} 

/*-----------------------------------------------------------------------------*/
/** 
    \fn void  Temp_InitSoaTemperature(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Reset temeprature related variables and state machine.  
    \details Reset temeprature related variables and state machine.
                            \par Used Variables
    \return \b void no value return
    \note None.
    \warning None
*/
void  Temp_InitSoaTemperature(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  Temp_Set_BOOLSOATemperatureCorr(FALSE);
  enumSOATempState = NORMAL_TEMP;
  u8InverterStatus &=(u8)~(TAMB_OVER_TEMPERATURE);
  enumDeratingMode = DISABLED;

  #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
    Temp_Set_u8TambErrCount(0);
  #endif  /* DIAGNOSTIC_STATE */

  #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
    MaxPower_Set_BOOLPartStallDerFlag(FALSE);
  #endif  /* DIAGNOSTIC_STATE */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Temp_CheckTempAmbAtRun(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Manages temperature derating
    \details This function requires the temperature measured by Level0 functions and computes a correction
    to limit the overheating of the whole motor. This function calls indexed functions to implement a 
    temperature state machine:
    - \b SOATemperatureUnderMin This function identify a fault condition because the ambient temperature is too low
    - \b SOATemperatureNormal This function correspond to a normal state; in this case the motor can run at full speed 
    without limitations. If a precedent correction was applied and temperature is again behind normal ranges, this function reduce
    the correction integral memory with an integral regulator only.
    - \b SOATemperatureDerating This function correspond to a warning state; in this case the motor can run but it's needed a 
    limitation of power dissipated to avoid dangerous overheating. This function implements a PI regulator to compute a correction
    in power injection. The proportional gain is added to increase the feedforward and to increase dinamics.
    - \b SOATemperatureOverMax This function identify a fault condition because the ambient temperature is too high
    \dot digraph Temperature{
        node [shape=box, style=filled, fontname=Arial, fontsize=10]; compound=true;
        subgraph cluster0{
          MIN[ label="UNDER_MIN_TEMP", color="red",shape=doubleoctagon];
          SetMIN[label=" Set \n TAMB_UNDER_TEMPERATURE", fontsize=8];
          ResetMIN[label=" Reset \n TAMB_UNDER_TEMPERATURE", fontsize=8];
          MIN->SetMIN[label=" s16TAmb < \n (TLE7184F_LOW_TEMP+TLE7184F_HYSTERESIS)", fontsize=8];
          SetMIN->MIN[];
          MIN->ResetMIN[label=" s16TAmb >= \n (TLE7184F_LOW_TEMP + \n + TLE7184F_HYSTERESIS)", fontsize=8];
          label="SOATemperatureUnderMin";fontsize=10;
        }                  
        subgraph cluster1{
          DER[ label="DERATING_TEMP", color="yellow",shape=doubleoctagon];  
          AddCOR[label=" INCREASE \n Derating Correction \n PI Regulator"];
          SetDERMax[label=" Set \n TAMB_OVER_TEMPERATURE", fontsize=8];
          PIreg[label="PI regulator",style="rounded,filled", fontsize=8,fillcolor=violet]
          DER->AddCOR->PIreg->DER;
          DER->SetDERMax[label=" s16TAmb >= \n TLE7184F_HIGH_TEMP", fontsize=8];
          label="SOATemperatureDerating";fontsize=10;
        }
        subgraph cluster2{
          NORM[ label="NORMAL_TEMP", color="darkgreen",shape=doubleoctagon];  
          NORM->SetNORMMin[label=" s16TAmb >= \n DRIVER_OVER_DERATING_TEMPERATURE", fontsize=8]; 
          SetNORMMin[label=" Set \n TAMB_UNDER_TEMPERATURE", fontsize=8]; 
          RemCOR[label=" DECREASE \n Derating Correction", fontsize=8];
          Ireg[label="I regulator \n ONLY",style="rounded,filled", fontsize=8,fillcolor=violet]
          NORM->RemCOR->Ireg->NORM;
          label="SOATemperatureNormal";fontsize=10;
        }
        subgraph cluster3{
          MAX[ label="OVER_MAX_TEMP", color="red",shape=doubleoctagon];  
          MAX->SetMAX[label=" s16TAmb < \n DRIVER_OVER_DERATING_TEMPERATURE", fontsize=8];
          SetMAX[label=" Set \n TAMB_OVER_TEMPERATURE", fontsize=8];
          SetMAX->MAX[ ];
          ResetMAX[label=" Reset < \n TAMB_OVER_TEMPERATURE", fontsize=8];
          MAX->ResetMAX[label=" s16TAmb < \n DRIVER_OVER_DERATING_TEMPERATURE", fontsize=8];
          label="SOATemperatureOverMax";fontsize=10;
        }

        ResetMIN->NORM[color="blue"];
        SetNORMMin->MIN[color="blue"];         	
        NORM->DER[label=" s16TAmb >= \n DRIVER_OVER_DERATING_TEMPERATURE", fontsize=8,color="blue"];
        DER->NORM[label=" s16TAmb >= \n DRIVER_OVER_DERATING_TEMPERATURE", fontsize=8,color="blue"];
        ResetMAX->NORM[label=" ", fontsize=8,color="blue"];
        SetDERMax->MAX[color="blue"];
    }      \enddot
                            \par Used Variables
    \ref SOATemperatureFunction Buffer with pointers to functions  \n
    \ref enumSOATempState used to identify the Temperature machine state and to index the functions pointers buffer  \n
    \ref u8InverterStatus Flag for inverter management \n
    \return \b bool Returns TRUE when temperature reading is out of the ranges.
    \note None.
    \warning None
*/ 

BOOL Temp_CheckTempAmbAtRun(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  SOATemperatureFunction[enumSOATempState]();

  if(enumSOATempState == OVER_MAX_TEMP)
  {
     ret = TRUE;
  }
  else
  {
     ret = FALSE;
  }

  return (ret);
}


 /*-----------*/ 
static void SOATemperatureNormal (void)
{
  s16 strTempDrive_s16TempAmbReadPU_tmp;
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  s16 strTempDrive_s16TempMOSReadPU_tmp;
  #endif

  strTempDrive_s16TempAmbReadPU_tmp = INT0_Get_strTempDrive_s16TempAmbReadPU();
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  strTempDrive_s16TempMOSReadPU_tmp = INT0_Get_strTempDrive_s16TempMOSReadPU();
  #endif

  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION  
    if ( strTempDrive_s16TempAmbReadPU_tmp > (s16)(T_DRIVER_HIGH_DERATING_THR_PU+T_DERATING_HYSTERESIS_TLE_PU) ) 
    {
      enumDeratingMode = TLE;                                        /* Il TLE e' la causa del derating. */
      enumSOATempState = DERATING_TEMP;
      Temp_Set_BOOLSOATemperatureCorr(TRUE);
      /*RTI_SetTimeOutDuration((u16)SOA_TEMP_INT_STEP_TIME_BIT);*/   /* Step di integrazione del regolatore = 1 s.                     */
      u16TempStepInt = SOA_TEMP_INT_STEP_TIME_BIT;
      VectParPI[TEMP_REG].u16PropGain = TEMP_REG_PROP_GAIN_PU;       /* Setting del guadagno proporzionale.                            */
      VectParPI[PMAX_REG].BOOLFreeze = FALSE;                        /* Il regolatore di Pmax viene sbloccato per il derating, qualora */
                                                                     /* sia stato inibito dall'anello di Delta massimo o Ipk massima.  */
      /*return;*/
    }
    else if ( strTempDrive_s16TempMOSReadPU_tmp > (s16)(T_MOSFET_HIGH_DERATING_THR_PU+T_DERATING_HYSTERESIS_MOS_PU) )
    {
      enumDeratingMode = MOSFET;                                     /* D70 e' la causa del derating. */
      enumSOATempState = DERATING_TEMP;
      Temp_Set_BOOLSOATemperatureCorr(TRUE);
      /*RTI_SetTimeOutDuration((u16)SOA_TEMP_INT_STEP_TIME_BIT);*/   /* Step di integrazione del regolatore = 1 s.                     */
      u16TempStepInt = SOA_TEMP_INT_STEP_TIME_BIT;
      VectParPI[TEMP_REG].u16PropGain = TEMP_REG_PROP_GAIN_PU;       /* Setting del guadagno proporzionale.                            */
      VectParPI[PMAX_REG].BOOLFreeze = FALSE;                        /* Il regolatore di Pmax viene sbloccato per il derating, qualora */
                                                                     /* sia stato inibito dall'anello di Delta massimo o Ipk massima.  */
      /*return;*/
    }
    else
    {
      /* Nop(); */
    }
  #else
    if ( strTempDrive_s16TempAmbReadPU_tmp > (s16)(T_DRIVER_HIGH_DERATING_THR_PU+T_DERATING_HYSTERESIS_TLE_PU) ) 
    {
      enumDeratingMode = TLE;                                        /* Il TLE e' la causa del derating. */
      enumSOATempState = DERATING_TEMP;
      Temp_Set_BOOLSOATemperatureCorr(TRUE);
      /*RTI_SetTimeOutDuration((u16)SOA_TEMP_INT_STEP_TIME_BIT);*/   /* Step di integrazione del regolatore = 1 s.                     */
      u16TempStepInt = SOA_TEMP_INT_STEP_TIME_BIT;
      VectParPI[TEMP_REG].u16PropGain = TEMP_REG_PROP_GAIN_PU;       /* Setting del guadagno proporzionale.                            */
      VectParPI[PMAX_REG].BOOLFreeze = FALSE;                        /* Il regolatore di Pmax viene sbloccato per il derating, qualora */
                                                                     /* sia stato inibito dall'anello di Delta massimo o Ipk massima.  */
      return;
    }
    else
    {
      /* Nop(); */
    }
  #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */

  u8InverterStatus &=(u8)~(TAMB_OVER_TEMPERATURE);  
}

static void SOATemperatureDerating (void) /*PRQA S 2889 #multiple return due to error management*/
{
  s16 strTempDrive_s16TempAmbReadPU_tmp;
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  s16 strTempDrive_s16TempMOSReadPU_tmp;
  #endif

  strTempDrive_s16TempAmbReadPU_tmp = INT0_Get_strTempDrive_s16TempAmbReadPU();
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
  strTempDrive_s16TempMOSReadPU_tmp = INT0_Get_strTempDrive_s16TempMOSReadPU();
  #endif

  /* Il drive viene spento se la temperatura supera T_xxx_SHUTDOWN_THRESHOLD. */
  #ifdef MOSFET_OVERTEMPERATURE_PROTECTION 
    if ( ( strMachine.u16FrequencyAppPU <= EL_FREQ_DERATING_SHUTDOWN_PU_RES_BIT ) || ( strTempDrive_s16TempAmbReadPU_tmp >= T_DRIVER_SHUTDOWN_THRESHOLD_PU ) || ( strTempDrive_s16TempMOSReadPU_tmp >= T_MOSFET_SHUTDOWN_THRESHOLD_PU ) )
  #else
    if ( ( strMachine.u16FrequencyAppPU <= EL_FREQ_DERATING_SHUTDOWN_PU_RES_BIT ) || ( strTempDrive_s16TempAmbReadPU_tmp >= T_DRIVER_SHUTDOWN_THRESHOLD_PU ) )
  #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
  {
    #if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
      /* If the drive is halted at the end of thermal derating, u8TambErrCount is saturated to TAMB_ERR_COUNT_MAX.         */
      /* This is correct because, after derating, it is for sure that T_AMB error must be signalized on the feedback line. */
      Temp_Set_u8TambErrCount(TAMB_ERR_COUNT_MAX);
    #endif  /* DIAGNOSTIC_STATE */
        
    u8InverterStatus |= TAMB_OVER_TEMPERATURE;
    enumSOATempState = OVER_MAX_TEMP;
    return;
  }
  else
  {
    /* Nop(); */
  }

  /*if ( RTI_IsTimeOutElapsed() == (BOOL)TRUE )*/   /* Step di integrazione */
  if ( u16TempStepInt == (u16)0 )
  {
    u16TempStepInt = SOA_TEMP_INT_STEP_TIME_BIT;

    if ( enumDeratingMode == TLE )
    {
      /* Aggiornamento ingressi del PI di temperatura. */
      (void)REG_UpdateInputPI(TEMP_REG,(s16)T_DRIVER_HIGH_DERATING_THR_PU,(s16)strTempDrive_s16TempAmbReadPU_tmp);
      
      /* Se la temperatura del TLE torna al di sotto di T_DRIVER_HIGH_DERATING_THR_PU,    */
      /* le integrative dei regolatori di temperatura e potenza vengono tenute abilitate. */
      /* Cio' consente di uscire dal derating.                                            */
      if ( strTempDrive_s16TempAmbReadPU_tmp < T_DRIVER_HIGH_DERATING_THR_PU)
      {
        VectParPI[TEMP_REG].BOOLFreeze  = FALSE;
        VectParPI[PMAX_REG].BOOLFreeze  = FALSE;
        VectParPI[TEMP_REG].u16PropGain = 0;        /* N.B.: non avendo la limitazione sul proporzionale del regolatore di temperatura, il relativo guadagno va messo a 0. */
                                                    /* Se cosi' non fosse, l'uscita del regolatore di temperatura verrebbe saturata anch'essa a 0 PRIMA dello svuotamento  */
                                                    /* della componente integrale ed il drive non tornerebbe alla velocita' da cui e' partito il derating.                 */
      }
      else
      {
        VectParPI[TEMP_REG].u16PropGain = TEMP_REG_PROP_GAIN_PU;   /* Con sovratemperatura, la componenete proporzionale deve essere attiva. */
      }
    }
    #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
    else if ( enumDeratingMode == MOSFET )
    {
      /* Aggiornamento ingressi del PI di temperatura. */
      (void)REG_UpdateInputPI(TEMP_REG,(s16)T_MOSFET_HIGH_DERATING_THR_PU,(s16)strTempDrive_s16TempMOSReadPU_tmp);

      /* Se la temperatura di D70 torna al di sotto di T_MOSFET_HIGH_DERATING_THR_PU,     */
      /* le integrative dei regolatori di temperatura e potenza vengono tenute abilitate. */
      /* Cio' consente di uscire dal derating.                                            */
      if ( strTempDrive_s16TempMOSReadPU_tmp < T_MOSFET_HIGH_DERATING_THR_PU)
      {
        VectParPI[TEMP_REG].BOOLFreeze = FALSE;
        VectParPI[PMAX_REG].BOOLFreeze = FALSE;
        VectParPI[TEMP_REG].u16PropGain = 0;          /* N.B.: non avendo la limitazione sul proporzionale del regolatore di temperatura, il relativo guadagno va messo a 0. */
      }                                               /* Se cosi' non fosse, l'uscita del regolatore di temperatura verrebbe saturata anch'essa a 0 PRIMA dello svuotamento  */
      else                                            /* della componente integrale ed il drive non tornerebbe alla velocita' da cui e' partito il derating.                 */
      {
        VectParPI[TEMP_REG].u16PropGain = TEMP_REG_PROP_GAIN_PU;   /* Con sovratemperatura, la componenete proporzionale deve essere attiva. */
      }
    }
    #endif  /* MOSFET_OVERTEMPERATURE_PROTECTION */
    else
    {
      /* Nop(); */
    }

    /* Aggiornamento del PI di temperatura. */
    (void)REG_UpdateRegPI(TEMP_REG);

    /* Quando il regolatore si e' resettato e non ci sono piu' sovratemperature, si torna in NORMAL_TEMP. */
    if ( VectVarPI[TEMP_REG].s32RegOutput == 0 )
    {
      enumSOATempState = NORMAL_TEMP;
      enumDeratingMode = DISABLED;
      Temp_Set_BOOLSOATemperatureCorr(FALSE);
      (void)REG_UpdateInputPI(TEMP_REG,0,0);    /* Reset degli ingressi di TEMP_REG. */
      return;
    }
    else
    {
      /* Nop(); */
    }
  }
}

static void SOATemperatureOverMax (void) /*PRQA S 3006 #Due to the necessary use of Nop function written in assembly code*/ /*Created to define address for SOATemperatureFunction variable */
{
  Nop(); /*PRQA S 1006 #Necessary assembly code*/
}

s32 Temp_GetTempCorr (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  return((s32)VectVarPI[TEMP_REG].s32RegOutput);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Temp_AutoTestTLETempAmb(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns TRUE when temperature reading is corrupted.  
    \details This function performs:\n
    - 10 bit temperature channel convertion.\n
    - Filling in \ref buffu16ADC_READING with voltage value just read.\n
    - Returns TRUE if temperature < AUTOTEST_TLE_TAMB_LOW_THRESHOLD\n
    or\n
    if temperature > \ref AUTOTEST_TLE_TAMB_HIGH_THRESHOLD\n
    
    This is a fault condition because the hardware device that perform the temperature measure, has been projected to ensure a minimum voltage different to zero
    when the PCB is powered. If the AD converter return a very low value, less than \ref AUTOTEST_TLE_TAMB_LOW_THRESHOLD, or a bigger one,
    more than \ref AUTOTEST_TLE_TAMB_HIGH_THRESHOLD, the sensing hardware chain is corrupted or broken and it's impossible to perform a safe motor drive with this hardware failure
    because the temperature measure is not reliable.\n
    
                            \par Used Variables
    \ref buffu16ADC_READING Buffer with all the readded value from AD channels \n
    \ref u8InverterStatus Variable with flags for Inverter management \n
    \return \b bool Returns TRUE when temperature reading is out of the ranges.
    \note None.
    \warning None
*/
BOOL Temp_AutoTestTLETempAmb(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16TAmb;

  /*buffu16ADC_READING[T_AMB_TLE] = u16TAmb; */                                     /* Fill in the buffu16ADC_READING array */

  u16TAmb = buffu16ADC_READING[T_AMB_TLE];                                          /* PWM peripherals turns on but TLE still in error so NO pwm on MOS appears!                                                        */
                                                                                    /* Disassemble:                                                                                                                       */
                                                                                    /* LDD   buffu16ADC_READING:6                                                                                                         */
                                                                                    /* ...                                                                                                                                */
                                                                                    /* This instruction is good since u16PeakCurr reading in done in single time so if interrupt occours during this reading -> no error! */
                                                                                    /* Even if a writing interrupt occours between the two instruction: no problem.                                                       */

  if (u16TAmb >= AUTOTEST_TLE_TAMB_HIGH_THRESHOLD)
  {
    ret = (BOOL)TRUE;
  }
  else if (u16TAmb <= AUTOTEST_TLE_TAMB_LOW_THRESHOLD)
  {
    ret = (BOOL)TRUE;
  }
  else
  {
    ret = (BOOL)FALSE; /* Normal operation */
  }
  return ret;
}

#ifdef MOSFET_OVERTEMPERATURE_PROTECTION
/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Temp_AutoTestMosTemp(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Ianni Fabrizio  \n <em>Reviewer</em>

    \date 13/01/2013
     \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns TRUE when Mosfet temperature reading is corrupted.  
    \details This function performs:\n
    - 10 bit temperature channel convertion.\n
    - Returns TRUE if temperature < AUTOTEST_MOS_TEMPERATURE_LOW_THRESHOLD\n
    or\n
    if temperature > \ref AUTOTEST_MOS_TEMPERATURE_HIGH_THRESHOLD\n
    
    This is a fault condition because the hardware device that perform the temperature measure, has been projected to ensure a minimum voltage different to zero
    when the PCB is powered. If the AD converter return a very low value, less than \ref AUTOTEST_MOS_TEMPERATURE_LOW_THRESHOLD, or a bigger one,
    more than \ref AUTOTEST_MOS_TEMPERATURE_HIGH_THRESHOLD, the sensing hardware chain is corrupted or broken and it's impossible to perform a safe motor drive with this hardware failure
    because the temperature measure is not reliable.\n
    
                            \par Used Variables
    \ref buffu16ADC_READING Buffer with all the readded value from AD channels \n
    \ref u8InverterStatus Variable with flags for Inverter management \n
    \return \b bool Returns TRUE when temperature reading is out of the ranges.
    \note None.
    \warning None
*/

BOOL Temp_AutoTestMosTemp(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16TMos;

  u16TMos = buffu16ADC_READING[T_MOS_CONV];                                         /* PWM peripherals turns on but  TLE still in error so NO pwm on MOS appears!                                                         */
                                                                                    /* Disassemble:                                                                                                                       */
                                                                                    /* LDD   buffu16ADC_READING:6                                                                                                         */
                                                                                    /* ...                                                                                                                                */
                                                                                    /* This instruction is good since u16PeakCurr reading in done in single time so if interrupt occours during this reading -> no error! */
                                                                                    /* Even if a writing interrupt occours between the two instruction: no problem.                                                       */

  u16TempMOSReadOld = u16TMos;                                                      /* Reset EMI filter */

  if (u16TMos >= AUTOTEST_MOS_TEMPERATURE_HIGH_THRESHOLD)
  {
    ret = (BOOL)TRUE;
  }
  else if (u16TMos <= AUTOTEST_MOS_TEMPERATURE_LOW_THRESHOLD)
  {
    ret = (BOOL)TRUE;
  }
  else
  {
    ret = (BOOL)FALSE;	/* Normal operation */
  }

  return ret;
}
#endif /* MOSFET_OVERTEMPERATURE_PROTECTION */

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Temp_CalcValues(void)
    \author Lorenzo Fornari  \n <em>Main Developer</em> 

    \date 13/11/2015
    \par None.
    \brief Read ADC calling proper functions and filter values.
    Filtered values are passed to convertion method that returns °C value.
    \details This function has to be polled in main loop.
    \return \b None.
    \note None.
    \warning None
*/
void Temp_UpdateValues(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16 u16BuffTemp;

  if( INT0_Get_u8TempFiltTimer() == 0u )
  {
    INT0_Set_u8TempFiltTimer(TEMP_SAMPLING_BIT);
    /*T_AMB from TLE */
    u16BuffTemp = (u16)buffu16ADC_READING[T_AMB_TLE];
    FIR_UpdateFilt(TEMP_TLE_FILT,(s16)((u16)(u16BuffTemp<<TEMP_ADC_SHIFT)));
    u16BuffTemp = (u16)((u16)VectVarFilt[TEMP_TLE_FILT].s16NewOutput>>TEMP_ADC_SHIFT);
    Temp_ReadTamb(u16BuffTemp);
    /*T_MOS from NTC */
    #ifdef MOSFET_OVERTEMPERATURE_PROTECTION
      u16BuffTemp = (u16)buffu16ADC_READING[T_MOS_CONV];
      FIR_UpdateFilt(TEMP_MOS_FILT,(s16)((u16)(u16BuffTemp<<TEMP_ADC_SHIFT)));
      u16BuffTemp = (u16)((u16)VectVarFilt[TEMP_MOS_FILT].s16NewOutput>>TEMP_ADC_SHIFT);
      Temp_ReadTMos(u16BuffTemp);
    #endif
  }
}

/*-----------------------------------------------------------------------------*/
#if (defined(REDUCED_SPEED_BRAKE_VS_TEMPERATURE) || defined (ENABLE_RS232_DEBUG) || defined(BMW_LIN_ENCODING) || defined(LOG_DATI))
/** 
    \fn s16 Temp_GetTamb(void)
    \author Lorenzo Fornari  \n <em>Main Developer</em>
     
    \date 13/11/2015
    \par None.
    \brief Temperature computation and filtering is done elsewhere. \n
    This method called by other modules simply returns filterd °C value.
    \details None.
    \return \b u16 Returns mos temperature in °C
    \note None.
    \warning None
*/
s16 Temp_GetTamb(void) /* PRQA S 1503 #rcma-1.5.0 warning message: this function could be used for log data management */
{
  s16 strTempDrive_s16TempAmbReadPU_tmp;

  strTempDrive_s16TempAmbReadPU_tmp = INT0_Get_strTempDrive_s16TempAmbReadPU(); 
  return((s16)strTempDrive_s16TempAmbReadPU_tmp);
}
#endif

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Temp_ReadTamb(void)
    \author	Lorenzo Fornari  \n <em>Main Developer</em> 

    \date 13/11/2015
    \brief Converts ADC to °C value. Final is written in local variable  
    \details Uses interal temperature sensor of TLE7184F.\n
    Value[°C] = buffu16ADC_READING[T_AMB_TLE] - 300) + TAMB_TLE.\n
    \return None.
    \note None.
    \warning None
*/
static void Temp_ReadTamb(u16 ADReading)
{
  s16 s16TempAmb; 
  s16TempAmb = ( (s16)((s16)((s16)((u32)ADReading) - (s16)u16ParamT) + TAMB_TLE));
  INT0_Set_strTempDrive_s16TempAmbReadPU((s16)((s32)((s32)s16TempAmb*(s32)TEMPERATURE_RES_FOR_PU_BIT)/TLE_BASE_TEMPERATURE_DEG));
}

#ifdef MOSFET_OVERTEMPERATURE_PROTECTION
/*-----------------------------------------------------------------------------*/
#if (defined(ENABLE_RS232_DEBUG) || defined(LOG_DATI))
/** 
    \fn s16 Temp_GetTmos(void)
    \author Lorenzo Fornari  \n <em>Main Developer</em>
     
    \date 13/11/2015
    \par None.
    \brief Temperature computation and filtering is done elsewhere. \n
    This method called by other modules simply returns filterd °C value.
    \details None.
    \return \b u16 Returns mos temperature in °C
    \note None.
    \warning None
*/
s16 Temp_GetTMos(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  s16 strTempDrive_s16TempMOSReadPU_tmp;

  strTempDrive_s16TempMOSReadPU_tmp = INT0_Get_strTempDrive_s16TempMOSReadPU();
  return((s16)strTempDrive_s16TempMOSReadPU_tmp);
}
#endif

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Temp_ReadTMos(void)
    \author	Lorenzo Fornari  \n <em>Main Developer</em> 

    \date 13/11/2015
    \brief Converts ADC to °C value. Final is written in local variable  
    \details Sensor: Murata NTC NCP18XH103F with 2k7 Ohm pullup.
    (near MOSFETS POWER STAGE).\n
    Temperature vs ADC reading trend is approx with segment linear interpolation\n
    !!!No trimming implemented till available in production line!!!\n \n
    \return None.
    \note None.
    \warning None
*/
static void Temp_ReadTMos(u16 ADReading)
{
  u16 ADReading_tmp;
  s16 s16BuffTemp;

  ADReading_tmp = ADReading;

  #if defined(MOSFET_OVERTEMP_BAV99)           /*Diode*/
  /* FILTER PREVIOUS VALUE TO AVOID INTERFERENCE CAUSED BY RADIATED IMMUNITY */

  if(ADReading_tmp < T_MOS_TOL)
  {
    ADReading_tmp = T_MOS_TOL;    /*To avoid underflow in the next operations */
  }

  if( ADReading_tmp < ( u16TempMOSReadOld - T_MOS_TOL ) )     /* Avoid only samples below the previous one */
  {                                                           /* (Samples below a certain treshold turns off the motor) */
    ADReading_tmp = u16TempMOSReadOld;
  }
  else
  {
    u16TempMOSReadOld = ADReading_tmp;  
  }

  /* VOLTAGE TO CELSIUS CONVERSION */

  /*Calculate T_MOS_PARAM - Tread */
  s16BuffTemp = (s16)u16ParamTMOS - (s16)ADReading_tmp;

  /* The diode and its resistor convert temperature with a ratio 4.04 mV/°C */
  /* Multiply by 1.207  (1 bit [4.88 mV] = 1,207 °C) */
  s16BuffTemp *= ((s16)76);
  s16BuffTemp /= ((s16)63);

  s16BuffTemp = ((s16)((s16)((s16)s16BuffTemp) + TAMB_MOS));
  INT0_Set_strTempDrive_s16TempMOSReadPU((s16)((s32)((s32)s16BuffTemp*TEMPERATURE_RES_FOR_PU_BIT)/MOS_BASE_TEMPERATURE_DEG));

  #elif defined(MOSFET_OVERTEMP_NCP18XH103F)     /*Murata NTC */
  /*segment linear interpolation*/
  /*ADReading is unsigned value because multiplication by SLOPE overflows signed range*/
  for(s16BuffTemp=0; s16BuffTemp<(NCP18XH103F_SEGMENTS_NUM-1); s16BuffTemp++)
  {
    if (ADReading_tmp >= NTCADCThresholds[s16BuffTemp])
    {
      break;
    }
  }

  ADReading_tmp *= NTCSlopeIntercept[s16BuffTemp][0];
  s16BuffTemp = (s16)((s32)((u32)((u32)NTCSlopeIntercept[s16BuffTemp][1] - (u32)ADReading_tmp)));

  s16BuffTemp = (s16BuffTemp >> NCP18XH103F_DOWNSHIFT);/*PRQA S 4533 #Signed shift ok: compiler manages it properly*/
  INT0_Set_strTempDrive_s16TempMOSReadPU((s16)((s32)((s32)s16BuffTemp*(s32)TEMPERATURE_RES_FOR_PU_BIT)/MOS_BASE_TEMPERATURE_DEG));
  #endif
}

#endif /* MOSFET_OVERTEMPERATURE_PROTECTION */

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Temp_GetTparam(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns temperature reading from TLE7184F.  
    \details This function return temperature compensating value.\n
    It has been computed as following:\n
    \f[
        T_{param} = ( 25 - T_{amb} ) + T_{ad}
    \f]
    where:\n
    Tparam = stored parameter at \ref T_PARAM location.\n
    Tamb = Ambient temeprature measured by external tool.\n
    Tad = Temperature converted by A/D.\n
                            \par Used Variables
    \return \b u16 T_PARAM temperature compensating value.\n
    \note None.
    \warning None
*/
u16 Temp_GetTparam(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16ParamT = T_PARAM;
  return ( (u16)u16ParamT );
}

void Temp_SetDefaultParamT(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16ParamT = T_PARAM_REF;
}

#ifdef LIN_INPUT

#ifdef GM_LIN_ENCODING
/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL Temp_GetOvertemperature (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 09/01/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return TRUE if the over-temperature condition is on.    
                                \par Used Variables
    \ref u8InverterStatus       Status of the drive \n
    \return \b BOOL return boolean value
    \note none
    \warning none
*/
BOOL Temp_GetOvertemperature(void)
{ 
  return ( (BOOL)(u8InverterStatus & TAMB_OVER_TEMPERATURE) );
}
#endif  /* GM_LIN_ENCODING */


#ifdef BMW_LIN_ENCODING
/*-----------------------------------------------------------------------------*/
/** 
    \fn StateSOATemp_T Temp_GetTemperatureState(void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 
    \date 09/01/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return the Temperature State Machine actual condition.    
                                \par Used Variables
    \ref u8InverterStatus       Status of the drive \n
    \return \b StateSOATemp_T return enumerative value
    \note none
    \warning none
*/
StateSOATemp_T Temp_GetTemperatureState(void)
{
  return((StateSOATemp_T)enumSOATempState);
}
#endif  /* BMW_LIN_ENCODING */

#endif  /* LIN_INPUT */


#ifdef MOSFET_OVERTEMPERATURE_PROTECTION
/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Temp_GetTMosParam(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns correction factor for temperature reading from Sensor near MOSFETs.  
    \details This function return temperature compensating value.\n
    It has been computed as following:\n
    
    \return \b u16 T_MOS_PARAM temperature compensating value.\n
    \note None.
    \warning None
*/
u16 Temp_GetTMosParam(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  u16ParamTMOS = T_MOS_PARAM; 
  return ( (u16)u16ParamTMOS );
}

void Temp_SetDefaultParamTMOS(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16ParamTMOS = T_MOS_PARAM_REF;
}

#endif /* MOSFET_OVERTEMPERATURE_PROTECTION */

/*-----------------------------------------------------------------------------*/
/**
    \fn static void Temp_Set_BOOLSOATemperatureCorr(BOOL boolValue)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the bool value of static BOOLSOATemperatureCorr variable
                            \par Used Variables
    \ref BOOLSOATemperatureCorr \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void Temp_Set_BOOLSOATemperatureCorr(BOOL boolValue) 
{
    BOOLSOATemperatureCorr = boolValue;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn BOOL Temp_Get_BOOLSOATemperatureCorr(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the bool value of static BOOLSOATemperatureCorr variable
                            \par Used Variables
    \ref BOOLSOATemperatureCorr \n
    \return \b Gets the value of static BOOLSOATemperatureCorr variable.
    \note None.
    \warning None.
*/
BOOL Temp_Get_BOOLSOATemperatureCorr(void)
{
    return (BOOLSOATemperatureCorr);
}

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
/*-----------------------------------------------------------------------------*/
/**
    \fn static void Temp_Set_u8TambErrCount(u8 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static u8TambErrCount variable
                            \par Used Variables
    \ref u8TambErrCount \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void Temp_Set_u8TambErrCount(u8 value) 
{
    u8TambErrCount = value;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u8 Temp_Get_u8TambErrCount(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static u8TambErrCount variable
                            \par Used Variables
    \ref u8TambErrCount \n
    \return \b The value of static u8TambErrCount variable.
    \note None.
    \warning None.
*/
u8 Temp_Get_u8TambErrCount(void)
{
    return (u8TambErrCount);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn static void Temp_Inc_u8TambErrCount(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Increments the value of static u8TambErrCount variable
                            \par Used Variables
    \ref u8TambErrCount \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void Temp_Inc_u8TambErrCount(void)
{
    u8TambErrCount++;
}

/*-----------------------------------------------------------------------------*/
/**
    \fn u16 Temp_Get_u16TempStepInt(void)
    \author Ianni Fabrizio \n <em>Main developer</em>

    \date 14/06/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static u16TempStepInt variable
                            \par Used Variables
    \ref u16TempStepInt \n
    \return \b The value of static u16TempStepInt variable.
    \note None.
    \warning None.
*/
u16 Temp_Get_u16TempStepInt(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    return (u16TempStepInt);
}

/*-----------------------------------------------------------------------------*/
/**
    \fn void Temp_Dec_u16TempStepInt(void)
    \author Ianni Fabrizio \n <em>Main developer</em>

    \date 14/06/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Decrements the value of static u16TempStepInt variable
                            \par Used Variables
    \ref u16TempStepInt \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void Temp_Dec_u16TempStepInt(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
    u16TempStepInt--;
}
#endif  /* DIAGNOSTIC_STATE */
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
