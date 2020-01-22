/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  CUR1_Current.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file CUR1_Current.c
    \version see \ref FMW_VERSION 
    \brief Basic current functions.
    \details Routines for current operation and variables related initialization.\n
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
#include "MCN1_acmotor.h"     /* It has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"
#include "PBATT1_Power.h"     /* Per structPowerMeas.u16IbattPU */
#include MAC0_Register        /* Needed for macro definitions */
#include ADM0_ADC
#include "VBA1_Vbatt.h"
#include "CUR1_Current.h"     /* Needed for void Curr_SetDefaultParamI(void) */
#include "main.h"             /* Per strDebug */
#include "WTD1_Watchdog.h"
#include "TMP1_Temperature.h"
#include "AUTO1_AutoTest.h"   /* Per verifica parametri di taratura */
#include "PARAM1_Param.h"     /* Per indirizzi parametri taratura */
#include "INT0_PWMIsrS12.h"
#ifdef BMW_LIN_ENCODING
  #include "COM1_LINUpdate_BMW.h"   /* Per BOOL_Current_Autotest_Error */
#endif  /* BMW_LIN_ENCODING */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* ---------------------- Private Constants ----------------------------------*/
/** 
    \var I_PARAM
    \brief Tuning parameter for current measuring 
    \details This constant has to be written at tuning (test) time and used to compensate
    hw tolerances.
    \note none
    \warning none.
*/
static const volatile u16 I_PARAM I_PARAM_ADDRESS = I_PARAM_REF; /*PRQA S 1019 #mandatory for linker behavior*/ /*PRQA S 3218 #Gives better visibility here*/

#pragma DATA_SEG DEFAULT


/* ----------------------------- Private Const ------------------------------ */
/* ---------------------------- Private Variables --------------------------- */
/** 
    \var u16IOffset
    \brief used for storing offset current value.\n
    \note This varaible is placed in \b ZEROPAGE segment.
    \warning This variable is \b static.
    \details None.
*/
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
static u16 u16IOffset;
#pragma DATA_SEG DEFAULT


static u16 u16ParamI; 

/* ---------------------------- Public Variables --------------------------- */


/* ------------------------------------------------------------------------- */
/** 
    \fn BOOL Curr_AutoTestIBus(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns TRUE when Bus current reading is corrupted.  
    \details This function performs:\n
    - 10 bit current channel convertion.\n
    - Filling in \ref buffu16ADC_READING with current value just read.\n
    - Returns TRUE if current < \ref AUTOTEST_LOBUS_THRESHOLD\n
    or\n
    if current > \ref AUTOTEST_HIBUS_THRESHOLD\n

\f[ \left\{\begin{matrix} \begin{vmatrix}
FLAG^{SET}_{InverterStatus}\ UNDER\_CURRENT \\ return\ TRUE \end{vmatrix}
 & \Leftrightarrow  & (I_{READ} \geq  LOBUS_{AUTOTEST}^{THRESHOLD}) \\ \\ \begin{vmatrix}
FLAG^{RESET}_{InverterStatus}\ UNDER\_CURRENT \\ FLAG^{RESET}_{InverterStatus}\ OVER\_CURRENT
\\ return\ FALSE \end{vmatrix} & \Leftrightarrow  & \begin{matrix}
(I_{READ} >  LOBUS_{AUTOTEST}^{THRESHOLD}) \\ \wedge \\ (I_{READ} <  HIBUS_{AUTOTEST}^{THRESHOLD})
\end{matrix} \\ \\ \begin{vmatrix} FLAG^{SET}_{InverterStatus}\ OVER\_CURRENT \\ return\ TRUE 
\end{vmatrix} & \Leftrightarrow  & (I_{READ} \geq  HIBUS_{AUTOTEST}^{THRESHOLD}) \end{matrix}\right. \f]
                            \par Used Variables
    \return \b bool Returns TRUE when Bus current reading is out of the ranges.
    \note None.
    \warning None
*/
BOOL Curr_AutoTestIBus(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16IRead;

  u16IRead = buffu16ADC_READING[I_PEAK_CONV];                       /* PWM peripherals turns on but TLE still in error so NO pwm on MOS appears!    */
                                                                    /* Disassemble:                                                                 */
                                                                    /* LDD   buffu16ADC_READING                                                     */
                                                                    /* ...                                                                          */
                                                                    /* This instruction is good since u16PeakCurr reading in done in single time    */
                                                                    /* so if interrupt occours during this reading -> no error!                     */
                                                                    /* Even if a writing interrupt occours between the two instruction: no problem. */
  u16IOffset = u16IRead;

  if (u16IRead >= AUTOTEST_HIBUS_THRESHOLD)
  {
    #ifdef BMW_LIN_ENCODING
      BOOL_Current_Autotest_Error = TRUE;
    #endif  /* BMW_LIN_ENCODING */

    u8InverterStatus |= OVER_CURRENT;
  }
  else if (u16IRead <= AUTOTEST_LOBUS_THRESHOLD)
  {
    #ifdef BMW_LIN_ENCODING
      BOOL_Current_Autotest_Error = TRUE;
    #endif  /* BMW_LIN_ENCODING */

    u8InverterStatus |= UNDER_CURRENT;
  }
  else
  {
    #ifdef BMW_LIN_ENCODING
      BOOL_Current_Autotest_Error = FALSE;
    #endif  /* BMW_LIN_ENCODING */
  
    u8InverterStatus &= (u8)(~OVER_CURRENT);
    u8InverterStatus &= (u8)(~UNDER_CURRENT);
  }

  if((u8InverterStatus&(OVER_CURRENT|UNDER_CURRENT)) != 0u)
  {
    ret = TRUE;
  }
  else
  {
    ret = FALSE;
  }

  return (ret);
} 


BOOL Curr_AutoTestIBatt(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  BOOL ret;
  u16 u16IRead;

  u16IRead = buffu16ADC_READING[I_BATT_CONV_257HZ];            /* PWM peripherals turns on but TLE still in error so NO pwm on MOS appears!        */
                                                               /* Disassemble:                                                                     */
                                                               /* LDD   buffu16ADC_READING                                                         */
                                                               /* ...                                                                              */
                                                               /* This instruction is good since u16PeakCurr reading in done in single time so if  */
                                                               /* interrupt occours during this reading -> no error!                               */
                                                               /* Even if a writing interrupt occours between the two instruction: no problem.     */

  INT0_Set_u16IBattOffset(u16IRead);

  if (u16IRead >= AUTOTEST_HIBATT_THRESHOLD)
  {
    #ifdef BMW_LIN_ENCODING
      BOOL_Current_Autotest_Error = TRUE;
    #endif  /* BMW_LIN_ENCODING */

    u8InverterStatus |= OVER_CURRENT;
  }
  else if (u16IRead <= AUTOTEST_LOBATT_THRESHOLD)
  {
    #ifdef BMW_LIN_ENCODING
      BOOL_Current_Autotest_Error = TRUE;
    #endif  /* BMW_LIN_ENCODING */

    u8InverterStatus |= UNDER_CURRENT;
  }
  else
  {
    #ifdef BMW_LIN_ENCODING
      BOOL_Current_Autotest_Error = FALSE;
    #endif  /* BMW_LIN_ENCODING */

    u8InverterStatus &= (u8)(~OVER_CURRENT);
    u8InverterStatus &= (u8)(~UNDER_CURRENT);
  }

  if((u8InverterStatus&(OVER_CURRENT|UNDER_CURRENT)) != 0u)
  {
    ret = TRUE;
  }
  else
  {
    ret = FALSE;
  }

  return (ret);
}

/*-----------------------------------------------------------------------------*/
#ifdef ENABLE_RS232_DEBUG
/** 
    \fn u16 Curr_GetOffsetCurrent(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Return the convertion of offset current.  
    \details Return the convertion of offset current.\n
                            \par Used Variables
    \return \b u16 Current converted.
    \note None.
    \warning None
*/
u16 Curr_GetOffsetCurrent(void)
{
  return(u16IOffset);
}
#endif

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Curr_GetPeakCurrent(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns peak current taking care of current offset and tuning values
    \details This function returns peak current found at tacho capture event
    (\ref INT0_TachoCapture) minus current offset value. This difference is
    compensated by current tuning parameters i.e. \ref I_PARAM
    It takes into account possible tacho happening at function execution time so first assegnation to
    local variable is done: u32Ipeak = (u16)\ref u16PeakCurr;   
                            \par Used Variables
    \return \b u16 peak current minus offset current found at hardware turning on.
    \note None.
    \warning None
*/
u16 Curr_GetPeakCurrent(void)
{
  u32 u32Ipeak;

  u32Ipeak = (u16)u16PeakCurr;                   /* Disassemble:                                                                                                                       */
                                                 /* LDHX  u16PeakCurr                                                                                                                  */
  if ( u32Ipeak > u16IOffset )                   /* STHX  1,SP                                                                                                                         */
  {                                              /* This instruction is good since u16PeakCurr reading in done in single time so if interrupt occours during this reading -> no error! */
    u32Ipeak -= (u16)u16IOffset;                 /* Even if a writing interrupt occours between the two instruction: no problem.                                                       */
  }
  else
  {
    u32Ipeak = 0;
  }

  u32Ipeak = (u16)((u32)((u32)u32Ipeak*u16ParamI)>>OFFSET_SHIFT);	
  strMachine.u16CurrentPU = (u16)((u16)u32Ipeak * BASE_CURRENT_CONV_FACT_RES);

  return((u16)u32Ipeak);
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Curr_GetCurrent(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns current basing on correct factor found at test bench.  
    \details This function returns peak current minus current offset value. This difference
    is compensated by current tuning parameters i.e. \ref I_PARAM
    It takes into account possible tacho happening at function execution time so first assegnation to
    local variable is done: u32Ipeak = (u16)\ref u16PeakCurr;   
                            \par Used Variables
    \return \b u16 peak current corrects by correct factor found at test bench.
    \note None.
    \warning None
*/
u16 Curr_GetCurrent(void)
{
  u32 u32Result;

  u32Result = (u16)buffu16ADC_READING[I_PEAK_CONV];     /* Disassemble:                                                                                                                       */
                                                        /* LDHX  buffu16ADC_READING                                                                                                           */
  if ( u32Result > u16IOffset )                         /* STHX  3,SP                                                                                                                         */
  {                                                     /* This instruction is good since u16PeakCurr reading in done in single time so if interrupt occours during this reading -> no error! */
    u32Result -= u16IOffset;                            /* Even if a writing interrupt occours between the two instruction: no problem.                                                       */
  }
  else
  {
    u32Result=	0;
  }

  u32Result = (u16)((u32)((u32)u32Result*u16ParamI)>>OFFSET_SHIFT);  
  /*strMachine.u16CurrentPU = (u16)((u16)u32Result * BASE_CURRENT_CONV_FACT_RES);*/

  return((u16)u32Result);
}


u16 Curr_GetBattCurrent(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u32 u32Result;
  u16 structPowerMeas_u16IbattPU_tmp;

  u32Result = (u16)buffu16ADC_READING[I_BATT_CONV_257HZ];   /* Disassemble:                                                                                                                       */
                                                              /* LDHX  buffu16ADC_READING                                                                                                           */
  if ( u32Result > INT0_Get_u16IBattOffset() )                /* STHX  3,SP                                                                                                                         */
  {                                                           /* This instruction is good since u16PeakCurr reading in done in single time so if interrupt occours during this reading -> no error! */
    u32Result -= INT0_Get_u16IBattOffset();                   /* Even if a writing interrupt occours between the two instruction: no problem.                                                       */
  }
  else
  {
    u32Result=	0;
  }

  structPowerMeas_u16IbattPU_tmp = (u16)((u32)((u32)u32Result*u16ParamI)>>OFFSET_SHIFT); 
  Power_Set_structPowerMeas_u16IbattPU((u16)(structPowerMeas_u16IbattPU_tmp));
  structPowerMeas_u16IbattPU_tmp = (u16)((u16)Power_Get_structPowerMeas_u16IbattPU() * BASE_CURRENT_CONV_FACT_RES);
  Power_Set_structPowerMeas_u16IbattPU(structPowerMeas_u16IbattPU_tmp);

  return((u16)((u32)((u32)u32Result*u16ParamI)>>OFFSET_SHIFT));
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 Curr_GetIparam(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Returns temperature reading from TLE7184F.  
    \details This function return current compensating value.\n
                            \par Used Variables
    \return \b u16 I_PARAM current compensating value.\n
    \note None.
    \warning None
*/
u16 Curr_GetIparam(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16ParamI = I_PARAM;
  return ( (u16)u16ParamI );
}

void Curr_SetDefaultParamI (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  u16ParamI = I_PARAM_REF;
}
/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
