/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  DIA1_Diagnostic.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file DIA1_Diagnostic.c
    \version see \ref FMW_VERSION 
    \brief Basic Diagnostic functions.
    \details Routines for diagnostic operation and variables related initialization.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif

#include <string.h>           /* for EnableInterrupts macro */
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include "main.h"             /* Per SystStatus_t def, enumerativa errori drive e per strDebug         */
#include "TIM1_RTI.h"         /* Needed for RTI_SetDiagShortDuration                                   */
#include "MCN1_acmotor.h"     /* it has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"         /* Public motor control peripheral function prototypes                   */
#include MAC0_Register        /* Needed for macro definitions                                          */
#include "DIA1_Diagnostic.h"
#include INT0_DIAGIsr         /* Needed for errors definitions      */
#include ADM0_ADC             /* Needed for short circuit detection */
#include "TMP1_Temperature.h" /* Needed for u8TambErrCount          */

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
/* ------------------------- Private def --------------------------- */

#ifndef _CANTATA_

/** 
    \typedef Diag_Error_Out_t
    \brief Diagnostic internal error type from error got from main 
    \details This enumerator describes all kind of error that can happen
*/
typedef enum
{
  DIAG_ERR_NO_ERR,                 /* !< No error found                  */
  DIAG_ERR_RPI_PROTECTION,         /* !< \ref RPI_PROTECTION error.      */
  DIAG_ERR_V_BUS_UNDER,            /* !< \ref V_BUS under_voltage error. */
  DIAG_ERR_V_BUS_OVER,             /* !< \ref V_BUS over_voltage error.  */
  DIAG_ERR_T_AMB,                  /* !< \ref T_AMB error.               */
  DIAG_ERR_WINDMILL,               /* !< \ref WINDMILL error.            */
  DIAG_ERR_I_LIM,                  /* !< \ref I_LIM error.               */
  DIAG_ERR_INVALID_PWM_IN,         /* !< \ref INVALID_PWM_IN error.      */
  DIAG_ERR_GD_ERROR,               /* !< \ref GD_ERROR error.            */
  DIAG_ERR_FAULT,                  /* !< \ref FAULT error.               */
  DIAG_ERR_PARTIAL_STALL           /* !< Partial stall error.            */
} Diag_Error_Out_t;

/** 
    \struct Diagnostic_s
    \brief Diagnostic internal error managing. 
    \details This structure gets diagnostic managing
*/
typedef struct 
{
  Diag_Error_Out_t enumDiagError;  /* !< Internal diagnostic error. It's used to define error signal duty. */
  #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
  u8 u8ShortNumber;                /* !< Number of diagnostic shorts count. Used to reset diagnostic output behavoiur. */
  BOOL boolDiagnosticShort;        /* !< True if a short at diagnostic output is found. */
  #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */
} Diagnostic_s;

#endif /* _CANTATA_ */

/* -------------------------- Public variables ----------------------------- */
#pragma DATA_SEG DEFAULT

/* ------------------------- Private variables --------------------------- */
/** 
    \var strErrorStatus
    \brief It stores diagnostic behaviour.
    \details This variable stores Internal diagnostic error and, if \ref DIAGNOSTIC_SHORT_SW_PROTECTION is defined
    Number of diagnostic shorts count and short at diagnostic output state.
    \note This variable is placed in \b DEFAULT segment.
    \warning This variable is \b STATIC.
*/
static Diagnostic_s strErrorStatus;  /*STATIC*/

/** 
    \var enumDiagErOld
    \brief It stores the previous (main cycle) diagnostic error.
    \details None.
    \note This variable is placed in \b DEFAULT segment.
    \warning This variable is \b STATIC.
*/
static Diag_Error_Out_t enumDiagErOld;

static Diag_Voltage_Error enumDiagVoltage;
static u16 u16DiagnosticDutyBase;


/* --------------------------- Private functions --------------------------- */
static void DIAG_SetDiagnosticError(u16 u16Error);
static u16 DIAG_SetDiagnosticDuty(Diag_Error_Out_t StrError);
static u16 DIAG_SetDiagnosticFrequency(Diag_Error_Out_t StrError);
static void DIAG_SetFault(u16 u16Error);
/*void DIAG_UpdateDutyError(u16 u16Error);*/
static BOOL DIAG_IsDiagnosticShorted(void);
static Diag_Voltage_Error DIAG_Get_enumDiagVoltage(void);
/*-----------------------------------------------------------------------------*/
/** 
    \fn void DIAG_InitDiagnostic(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function inits diagostic variables and state machine.  
    \details This function performs:\n
    - Diagnostic internal variables resetting ( \ref enumDiagError = \ref DIAG_ERR_NO_ERR).\n
    - Diagnostic output in off ( \ref DIAGNOSTIC_OFF and \ref DIAGNOSTIC_SET_OUTPUT).\n
                            \par Used Variables
    \ref strErrorStatus Structure with errors data.\n
    \return \b void no value return
    \note None.
    \warning None
*/
void DIAG_InitDiagnostic(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{ 
  #if defined(DIAGNOSTIC_SHORT_ANALOG_PROTECTION)
    INPUT_FEEDBACK_ANA_INIT;
  #endif /* DIAGNOSTIC_SHORT_ANALOG_PROTECTION */

  strErrorStatus.enumDiagError = DIAG_ERR_NO_ERR;
  enumDiagErOld = DIAG_ERR_NO_ERR;
  INT0_Set_BOOLDiagPeriodFlag(FALSE);
  DIAG_Set_enumDiagVoltage(DIAG_VOLTAGE_NO_ERR);

  #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
  strErrorStatus.u8ShortNumber = 0;
  strErrorStatus.boolDiagnosticShort = FALSE; 
  #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */

  /*DIAGNOSTIC_REMOVE_SIGNAL;*/
  DIAGNOSTIC_OFF;
  DIAGNOSTIC_SET_OUTPUT;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void DIAG_SetDiagnosticError(u16 u16Error)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function sets diagnostic internal error got from main.  
    \details This function sets diagnostic internal error got from main.\n
    In order to do that it switches through \ref u16Error values (i.e. error passing from calling function) and associates any error to its relative diagnostic internal error.\n
    \param[in] u16Error error got from main.\n
                                \par Used Variables
    \return \b void no value return
    \note None.
    \warning None
*/
static void DIAG_SetDiagnosticError(u16 u16Error)
{
	switch ( u16Error )
	{
		case RPI_PROTECTION:
			strErrorStatus.enumDiagError = DIAG_ERR_RPI_PROTECTION;
		break;

		case V_BUS:
			if ( DIAG_Get_enumDiagVoltage() == DIAG_UNDER_VOLTAGE )
			{
				strErrorStatus.enumDiagError = DIAG_ERR_V_BUS_UNDER;
			}
			else
			{
				strErrorStatus.enumDiagError = DIAG_ERR_V_BUS_OVER;
			}
		break;

		case T_AMB:
			strErrorStatus.enumDiagError = DIAG_ERR_T_AMB;
		break;

		case WINDMILL:
			strErrorStatus.enumDiagError = DIAG_ERR_WINDMILL;
		break;

		case I_LIM:
			strErrorStatus.enumDiagError = DIAG_ERR_I_LIM;
		break;

		case INVALID_PWM_IN:
			strErrorStatus.enumDiagError = DIAG_ERR_INVALID_PWM_IN;
		break;

		case GD_ERROR:
			strErrorStatus.enumDiagError = DIAG_ERR_GD_ERROR;
		break;

		case FAULT_ERROR:
			strErrorStatus.enumDiagError = DIAG_ERR_FAULT;
		break;

		case PARTIAL_STALL:
			strErrorStatus.enumDiagError = DIAG_ERR_PARTIAL_STALL;
		break;

		default:
			strErrorStatus.enumDiagError = DIAG_ERR_NO_ERR;
		break;

	} /* switch */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 DIAG_SetDiagnosticDuty(Diag_Error_Out_t StrError)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 

    \date 20/09/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set the duty cycle of diagnostic signal.  
                                \par Used Variables
    \param[in] StrError           Error to be used to compute duty cycle.\n
    \ref u16DiagnosticDutyBase    Base-duty cycle of diagnostic signal.\n
    \ref enumDiagVoltage          Type of voltage error.\n                               
    \return \b u16 returned value
    \note None.
    \warning None
*/
static u16 DIAG_SetDiagnosticDuty(Diag_Error_Out_t StrError) /*PRQA S 2889 #multiple return due to error management*/
{
	switch ( StrError )
	{		
		case DIAG_ERR_PARTIAL_STALL:
			return ((u16)((u16)u16DiagnosticDutyBase*DIAG_PARTIAL_STALL_DUTY_SEC));
		/*break;*/

		case DIAG_ERR_RPI_PROTECTION:
		case DIAG_ERR_T_AMB:
		case DIAG_ERR_WINDMILL:
		case DIAG_ERR_I_LIM:
		case DIAG_ERR_GD_ERROR:
		case DIAG_ERR_FAULT:
			return ((u16)((u16)u16DiagnosticDutyBase*DIAG_FULL_STALL_DUTY_SEC));
		/*break;*/

		case DIAG_ERR_V_BUS_UNDER:
			return ((u16)((u16)u16DiagnosticDutyBase*DIAG_UNDER_VOLTAGE_DUTY_SEC));
		/*break;*/

		case DIAG_ERR_V_BUS_OVER:
			return ((u16)((u16)u16DiagnosticDutyBase*DIAG_OVER_VOLTAGE_DUTY_SEC));
		/*break;*/

		case DIAG_ERR_INVALID_PWM_IN:
		case DIAG_ERR_NO_ERR:
		default:
			return (0);
		/*break;*/
	} /* switch */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn u16 DIAG_SetDiagnosticFrequency(Diag_Error_Out_t StrError)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 

    \date 20/09/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Set the frequency of diagnostic signal.  
                                \par Used Variables
    \param[in] StrError           Error to be used to compute frequency.\n
    \ref u16DiagnosticDutyBase    Base-duty cycle of diagnostic signal.\n                                
    \return \b u16 returned value
    \note None.
    \warning None
*/
static u16 DIAG_SetDiagnosticFrequency(Diag_Error_Out_t StrError)
{
	u16 u16DiagFreq;

	switch ( StrError )
	{
		case DIAG_ERR_PARTIAL_STALL:
			u16DiagFreq = (u16)((u16)DIAGNOSTIC_ISR_FREQ_CONST/(u16)DIAG_PARTIAL_STALL_PERIOD_SEC);
		break;

		case DIAG_ERR_RPI_PROTECTION:
		case DIAG_ERR_T_AMB:
		case DIAG_ERR_WINDMILL:
		case DIAG_ERR_I_LIM:
		case DIAG_ERR_GD_ERROR:
		case DIAG_ERR_FAULT:
			u16DiagFreq = (u16)((u16)DIAGNOSTIC_ISR_FREQ_CONST/DIAG_FULL_STALL_PERIOD_SEC);
		break;

		case DIAG_ERR_V_BUS_UNDER:
			u16DiagFreq = (u16)((u16)DIAGNOSTIC_ISR_FREQ_CONST/DIAG_UNDER_VOLTAGE_PERIOD_SEC);
		break;

		case DIAG_ERR_V_BUS_OVER:
			u16DiagFreq = (u16)((u16)DIAGNOSTIC_ISR_FREQ_CONST/DIAG_OVER_VOLTAGE_PERIOD_SEC);
		break;

		case DIAG_ERR_INVALID_PWM_IN:
		case DIAG_ERR_NO_ERR:
		default:
			u16DiagFreq = 0;
		break;
	} /* switch */
	
	/* N.B.: il duty-base coincide, di fatto, con l'intero periodo di diagnosi di un errore.                                                                           */
	/* Nella funzione DIAG_SetDiagnosticDuty, il calcolo dell'esatto duty relativo ad uno specifico errore, si ottiene                                                 */
	/* scalando il duty base con la latenza di attivazione del feedback per tale errore.                                                                               */
	/* (Esempio: PARTIAL_STALL => Periodo complessivo in bit della diagnostica: Pdiag = 1024 => Duty_PARTIAL_STALL_bit = Pdiag * Duty_PARTIAL_STALL_sec/Duty_BASE_sec) */
	if ( u16DiagFreq == (u16)0 )
	{
		u16DiagnosticDutyBase = 0;
	}
	else
	{
		u16DiagnosticDutyBase = u16DiagFreq;
	}
	
	return((u16)u16DiagFreq);
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void DIAG_SetFault(u16 u16Error)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function sets diagnostic internal error.  
    \details This function sets diagnostic internal error and resets diagnostic short count.\n
    In order to do that it calls:\n
    - \ref DIAG_SetDiagnosticError; as argument it uses \ref DIAG_UpdateDutyError function argument.\n 
    - \ref boolDiagnosticShort = FALSE.\n 
    \param[in] u16Error error that will be saved into diagnostic internal error.\n
                            \par Used Variables
    \ref strErrorStatus Structure with errors data.\n
    \return \b void no value return
    \note None.
    \warning None
*/
static void DIAG_SetFault(u16 u16Error)
{  
  DIAG_SetDiagnosticError(u16Error);

  /*#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION*/
    /*strErrorStatus.boolDiagnosticShort = FALSE;*/    /* N.B: da commentare poiche', dopo un corto, con la diagnostica attiva, questa inizializzazione impedisce di rientrare */
  /*#endif*/  /* DIAGNOSTIC_SHORT_SW_PROTECTION */     /* nell'if ( strErrorStatus.boolDiagnosticShort == TRUE) e riabilitare la funzionalita' di feedback                     */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void DIAG_RemoveFault(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function remove diagnostic internal error to \ref DIAG_ERR_NO_ERR.  
    \details This function performs:
    - remove diagnostic internal error to \ref DIAG_ERR_NO_ERR.\n
    - \ref boolDiagnosticShort = FALSE if \ref DIAGNOSTIC_SHORT_SW_PROTECTION.\n
    - Reset \ref DIAG_SPEED_FEEDBACK flag into \ref C_D_ISR_Status \ref DIAGNOSTIC_STAND_ALONE.\n
    - Calls \ref DIAGNOSTIC_REMOVE_SIGNAL.\n
                            \par Used Variables
    \ref strErrorStatus Structure with errors data.\n
    \return \b void no value return
    \note None.
    \warning None
*/
void DIAG_RemoveFault(void)
{
  DIAG_SetDiagnosticError(NO_ERROR);
  
  INT0_Set_BOOLDiagPeriodFlag(FALSE);
  u16DiagnosticDutyBase = 0;
  INT0_Set_u16DiagnosticNextIsr((u16)0);
  enumDiagErOld = DIAG_ERR_NO_ERR;

  #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
    strErrorStatus.boolDiagnosticShort = FALSE; 
  #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */

  #ifdef DIAGNOSTIC_STAND_ALONE
    BitClear((DIAG_SPEED_FEEDBACK), C_D_ISR_Status); /* Tell tacho capture interrupt not to toggle diagnostic output. */
  #endif  /* DIAGNOSTIC_STAND_ALONE */

  DIAGNOSTIC_REMOVE_SIGNAL;
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void DIAG_DiagnosticShutDown(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function remove diagnostic managment before turn off the TLE
                            \par Used Variables
    \ref strErrorStatus Structure with errors data.\n
    \return \b void no value return
    \note None.
    \warning None
*/
void DIAG_DiagnosticShutDown(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  DIAGNOSTIC_OFF;
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL DIAG_IsDiagnosticShorted(void)
    \author	Pasquale Rubini  \n <em>Main Developper</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    
    \date 05/12/2011
                                \par Starting SW requirement
    "link al requirement"
    \brief check short circuit condition on diagnostic output hardware.  
    \details This function checks short circuit condition ondiagnostic output
    Due to different hardware is possible to check short circuit condition on digital pin
    (211X & 213X PCB with negative output) or with ADC input(220X with positive output).
    When it is using ADC is possible to check the dropout between Voltage monitor and 
    Diagnostic Output voltage through ECO_ANA_IN net.(see 220A electrical scheme).
    Due to different voltage divider it is necessary to multiply both DC values for different 
    ALPHA coefficients.
    DIAG_DELTA_VOLTAGE is maximum voltage dropout ([V]) above which function returns TRUE
    DIAG_ALPHA_SHORT_ECON & DIAG_ALPHA_MON take into account of diffent voltage dividers.   
    Function check if DIAG_CONV voltage > V_BUS_CONV voltage - DELTA voltage. 
      TRUE  -> Short circuit condition appened
      FALSE -> No short circuit on diagnostic output      
    \ref buffu16ADC_READING ADC readings.\n
*/    
    
static BOOL DIAG_IsDiagnosticShorted(void) /*PRQA S 2889 #multiple return due to error management*/
{
#if defined(DIAGNOSTIC_SHORT_ANALOG_PROTECTION)
  #ifdef NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC
  if( buffu16ADC_READING[DIAG_CONV] < ((u16)((u32)DIAG_LOW_VOLTAGE_LIMIT * (u32)DIAG_ALPHA_SHORT_ECON)) )
  {
    return ( FALSE );
  } else
  {
    return ( TRUE );
  }
  #else
  u32 TempDiagDrop;
  if (((u32)buffu16ADC_READING[V_BUS_CONV] * DIAG_ALPHA_SHORT_ECON)>((u32)DIAG_DELTA_VOLTAGE * DIAG_ALPHA_SHORT_ECON* DIAG_ALPHA_MON))
  {
    TempDiagDrop = (((u32)buffu16ADC_READING[V_BUS_CONV] * DIAG_ALPHA_SHORT_ECON) - ((u32)DIAG_DELTA_VOLTAGE* DIAG_ALPHA_SHORT_ECON* DIAG_ALPHA_MON)); 
  }
  else
  {
   TempDiagDrop = 0;
  }
  if (((u32)buffu16ADC_READING[DIAG_CONV] * DIAG_ALPHA_MON) > (u32)TempDiagDrop) 
    return (FALSE);
  else
    return (TRUE);
  #endif	/* NEGATIVE_DIAGNOSTIC_LOGIC */

#elif defined(DIAGNOSTIC_SHORT_DIGITAL_PROTECTION)
  return(IS_DIAGNOSTIC_SHORTED);
  
#else
  #error("Diagnostic short circuit protection not specified")
#endif
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void DIAG_ManageDiagnosticToECU(SystStatus_t enLocState, u16 u16DiagErrorType)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Managed error through frequency and duty cycle diagnostic output.  
    \details This function managed error through frequency and duty cycle diagnostic output.
    It takes care of diagnostic output short cut as well.\n
    In Run main state machine, diagnostic output is toggled every tacho period and so the 
    diagnostic output freqeuncy will be a half of electrical frequency.
    If any error is detected, diagnostic output will supply 10 hz frequency output with
    duty cycle depending on error type according to the following table:\n
    DutyCycle = 7%*strErrorStatus.enumDiagError @  strErrorStatus.enumDiagError = 0...10 set into 
    \ref DIAG_SetDiagnosticError function.\n
    
     \dot
  digraph G {
	subgraph cluster_0 {
		style=filled;
		color=lightgrey;
		node [style=filled,color=white];
		start -> RUN -> end_main_state;
		start -> "FAULT WAIT" -> end_main_state;
		start -> "PARK RUN_ON_FLY" -> end_main_state;
		start -> "IDLE STOP" -> end_main_state;
		label = "Check Main State";
	}
	subgraph cluster_1 {
		node [style=filled];
		label = "Check short circuit";
            end_main_state -> check_short_state;
            check_short_state -> Check_timeout[label="TRUE"];
            check_short_state -> Check_diag_on[label="FALSE"];

            Check_timeout -> Turn_on_diag [label="TRUE"] ;
            Turn_on_diag -> end;
 		Check_timeout -> end [label="FALSE"];
		Check_diag_on -> Check_diag_short[label="TRUE"] ;
		Check_diag_on -> end [label="FALSE"] ;
   		Check_diag_short -> Turn_off_diag [label="TRUE"] ;
		Turn_off_diag -> end;
            Check_diag_short -> end[label="FALSE"] ;
		color=blue
	}
	start [shape=Mdiamond];
	end [shape=Mdiamond];
}

    
    \param[in] enLocState State of Main Machine.\n
    \param[in] u16DiagErrorType Error causing motor stops.\n
                            \par Used Variables
    \ref strErrorStatus Structure with errors data.\n
    \ref C_D_ISR_StatusVariable with ISR flags.\n
    \return \b void no value return
    \note None.
    \warning None
 \enddot
*/
void DIAG_ManageDiagnosticToECU(SystStatus_t enLocState, u16 u16DiagErrorType)	
{  
  u16 u16DiagnosticNextIsr_tmp;
  u16 u16ErrorTypeMem_tmp;
  u8  u8StartNumber_tmp;

  if ( u16DiagErrorType != NO_ERROR )
  {
    if ( u16DiagErrorType == T_AMB )
    {
      /* T_AMB error must occur at least TAMB_ERR_COUNT_MAX times to be signalized. */
      /* This is a filter on wrong sporadic signalizations, which may occur after   */
      /* a RoF commanded, for istance, during the thermal derating.                 */
      if ( Temp_Get_u8TambErrCount() >= TAMB_ERR_COUNT_MAX )
      {
        DIAG_SetFault(u16DiagErrorType);
      }
      else
      {
        /* Nop(); */
      }
    }
    else
    {
      /* All the other errors are signalized at the first occurrece. */
      DIAG_SetFault(u16DiagErrorType);
    }
  }
  else
  {
    /* Nop(); */
  }

  switch ( enLocState )
  {
    case RUN:
    #if (defined(DIAGNOSTIC_STAND_ALONE) && defined(SPEED_FEEDBACK_ENABLE))
      #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
        if ( !strErrorStatus.boolDiagnosticShort )                      /* If diagnostic isn't shorted: */
        {
          DIAGNOSTIC_SET_OUTPUT;                                        /* Set Diagnostic output */
          BitSet(DIAG_SPEED_FEEDBACK, C_D_ISR_Status);                  /* And tells tacho capture interrupt to toggle diagnostic output. */
        }
        else
        {
          BitClear(DIAG_SPEED_FEEDBACK, C_D_ISR_Status);                /* And tells tacho capture interrupt NOT to toggle diagnostic output. */
        }
      #else
        DIAGNOSTIC_SET_OUTPUT;                                          /* Set Diagnostic output */
        BitSet(DIAG_SPEED_FEEDBACK, C_D_ISR_Status);                    /* And tells tacho capture interrupt to toggle diagnostic output. */
      #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */
    #endif  /* DIAGNOSTIC_STAND_ALONE */

    if ( u16DiagErrorType == PARTIAL_STALL )                                                                         /* Se durante il RUN si verifica un partial stall (i.e. sovraccarico, derating termico, etc)... */
    {
      u16DiagnosticNextIsr_tmp = (u16)((u32)5000000/(u32)DIAG_SetDiagnosticFrequency(strErrorStatus.enumDiagError)); /* ...allora si abilita la diagnostica con messaggio M4. */
      INT0_Set_u16DiagnosticNextIsr(u16DiagnosticNextIsr_tmp);
      u16DiagnosticNextIsr_tmp = INT0_Get_u16DiagnosticNextIsr();
      DIAGNOSTIC_SET_SIGNAL((u16DiagnosticNextIsr_tmp), (DIAG_SetDiagnosticDuty(strErrorStatus.enumDiagError)) );
    }
    else
    {
      /* Nop(); */
    }
    break;
          
    case WAIT:
    #if (defined(DIAGNOSTIC_STAND_ALONE) && defined(SPEED_FEEDBACK_ENABLE))
      BitClear(DIAG_SPEED_FEEDBACK, C_D_ISR_Status);
    #endif  /* DIAGNOSTIC_STAND_ALONE */

    u8StartNumber_tmp = Main_Get_u8StartNumber();
    u16ErrorTypeMem_tmp = Main_Get_u16ErrorTypeMem();

    /* N.B.: la condizione su enumDiagErOld (nell'if seguente) serve a far partire immediatamente la diagnostica, qualora si passi dallo stand-by al run, con un errore in corso (senza attendere lo scadere del periodo dell'interrupt di feedback). */
    if ( ( INT0_Get_BOOLDiagPeriodFlag() == (BOOL)TRUE ) || ( enumDiagErOld == DIAG_ERR_INVALID_PWM_IN ) )   /* Se l'ultimo errore registrato e' INVALID_PWM, oppure se e' trascorso un intero periodo del segnale di diagnostica... */
    {
      INT0_Set_BOOLDiagPeriodFlag(FALSE);                                                                                         /* ...si resetta il relativo flag... */

      if ( ( u8StartNumber_tmp >= DIAG_RESTART_MASK_ATTEMPTS ) || (( u16ErrorTypeMem_tmp & RESTART_MASK ) == (BOOL)FALSE) )       /* ...si verifica che l'errore attuale o non sia della RESTART_MASK, o, in caso contratio, che esso sia alla terza occorrenza... */
      {
        if ( enumDiagErOld != strErrorStatus.enumDiagError)                                                                       /* ...e, se vi e' da notificare un errore diverso dal precedente,... */
        {
          /* N.B.: DIAGNOSTIC_REMOVE_SIGNAL e non DIAG_RemoveFault()       */
          /* poiche' non si vuole disabilitare l'interrupt di diagnostica, */
          /* ma solo resettarlo!!                                          */
          DIAGNOSTIC_REMOVE_SIGNAL;                                                           /* ...si resetta provvisoriamente l'interrupt di diagnostica...                    */
          enumDiagErOld = strErrorStatus.enumDiagError;                                       /* ...e si fa ripartire il relativo counter con il duty associato al nuovo errore. */
        }
        else                                                                                  /* N.B.: in quest'ultimo "if" si entra immediatamente se l'ultimo errore registrato e' INVALID_PWM. */
        {                                                                                     /* Cio' perche' INVALID_PWM non comporta nessuna segnalazione sulla linea di diagnostica, quindi    */
          /* Nop(); */                                                                        /* e' inutile aspettare la conclusione di un periodo di tale segnale, quando in realta' si puo'     */
        }                                                                                     /* partire subito con la segnalazione del nuovo errore.                                             */
      }
      else
      {
        /* Nop(); */                                                                          /* N.B.: se l'errore in esame e' della RESTART_MASK, ma non alla terza occorrenza, NON va fatto il reset della diagnostica. */
      }                                                                                       /* Se lo si facesse, il duty subito precedente potrebbe essere interrotto a meta' (erroneamente), prima di rilasciare la    */
    }                                                                                         /* linea di feedback fino alla terza occorrenza dell'errore attuale (in pratica si creerebbe un duty anomalo sporadico).    */
    else
    {
      /* Nop(); */
    }

    if ( strErrorStatus.enumDiagError > DIAG_ERR_NO_ERR )                                     /* Se vi e' un errore da segnalare sulla diagnostica... */
    {
      if (( u16ErrorTypeMem_tmp & RESTART_MASK ) != (BOOL)FALSE)                              /* ...ed esso fa capo alla RESTART_MASK (i.e. rotore bloccato, etc)... */
      {
        if ( u8StartNumber_tmp >= DIAG_RESTART_MASK_ATTEMPTS )                                /* ...e si e' alla terza occorrenza... */
        {
          if((DIAG_SetDiagnosticFrequency(strErrorStatus.enumDiagError)) != (u16)0)
          {
            u16DiagnosticNextIsr_tmp = (u16)((u32)5000000/(u32)DIAG_SetDiagnosticFrequency(strErrorStatus.enumDiagError));    /* ...allora si abilita' la linea di diagnostica. */
            INT0_Set_u16DiagnosticNextIsr(u16DiagnosticNextIsr_tmp);
          }
          u16DiagnosticNextIsr_tmp = INT0_Get_u16DiagnosticNextIsr();
          DIAGNOSTIC_SET_SIGNAL((u16DiagnosticNextIsr_tmp), (DIAG_SetDiagnosticDuty(strErrorStatus.enumDiagError)) );
        }
        else                                                                                /* Se si deve passare dalla segnalazione di un errore della RESTART_MASK ad uno generico (i.e. over/under-voltage, etc)... */
        {
          if ( DIAG_IsDiagnosticEnable() ==(BOOL) FALSE )                                   /* ...si verifica prima che la linea di diagnostica venga rilasciata... */
          {
            DIAG_RemoveFault();                                                             /* ...e dopo si disattiva il feedback stesso (cosi' da non iniziare la nuova segnalazione con la */
          }                                                                                 /* diagnostica ancora attiva, cosa che genererebbe un duty transitorio falsato). */
          else
          {
            /* Nop(); */
          }
        }
      }
      else                                                                                  /* Se l'errore da segnalare non e' nella RESTART_MASK (i.e. over-voltage,etc)... */
      {
        if ( DIAG_IsDiagnosticEnable() == (BOOL)FALSE )                                     /* ...e sulla linea non si sta segnalando nulla... */
        {
          if((DIAG_SetDiagnosticFrequency(strErrorStatus.enumDiagError)) != (u16)0)
          {
            u16DiagnosticNextIsr_tmp = (u16)((u32)5000000/(u32)DIAG_SetDiagnosticFrequency(strErrorStatus.enumDiagError));
            INT0_Set_u16DiagnosticNextIsr(u16DiagnosticNextIsr_tmp);
          }
          u16DiagnosticNextIsr_tmp = INT0_Get_u16DiagnosticNextIsr();
          DIAGNOSTIC_SET_SIGNAL((u16DiagnosticNextIsr_tmp), (DIAG_SetDiagnosticDuty(strErrorStatus.enumDiagError)) );           /* ...allora si attiva il feedback relativo al nuovo errore. */
        }
        else
        {
          /* Nop(); */
        }
      }
    }
    else
    {
      /* Nop(); */
    }
    break;

    case PARK:
    case RUN_ON_FLY:
    case IDLE:
    case FAULT:
    default:
      /*Nop();*/
    break;
  }

  #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
    if ( strErrorStatus.boolDiagnosticShort == (BOOL)TRUE)
    {
      if (RTI_IsDiagShortElapsed() != (BOOL)FALSE)
      {
        strErrorStatus.boolDiagnosticShort = FALSE;
        DIAGNOSTIC_SET_OUTPUT;
        strErrorStatus.u8ShortNumber=0;
      }
    } 
    else
    {
      if ( (BOOL)DIAG_IsDiagnosticEnable() == (BOOL)TRUE )
      {
        if ( (BOOL)DIAG_IsDiagnosticShorted() == (BOOL)TRUE )
        {
          strErrorStatus.u8ShortNumber++;
          if ( strErrorStatus.u8ShortNumber > DIAGNOSTIC_MAX_SHORT_NUMBER )
          {
            strErrorStatus.boolDiagnosticShort = TRUE;
            RTI_SetDiagShortDuration(SHORT_CIRCUIT_DIAG_TIMEOUT);
            DIAGNOSTIC_REMOVE_OUTPUT;
          }
        }
        else
        {
          strErrorStatus.u8ShortNumber = 0;
        }
      }
    }
  #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn BOOL DIAG_IsDiagnosticEnable (void)
    \author	Ianni Fabrizio  \n <em>Main Developer</em> 

    \date 13/07/2018
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function returns the diagnostic port state.
    \return \b BOOL boolean value returned
    \note None.
    \warning None
*/
BOOL DIAG_IsDiagnosticEnable (void)
{
   return((BOOL)DIAGNOSTIC_PORT_BIT);
}


/*-----------------------------------------------------------------------------*/
/**
    \fn void DIAG_Set_enumDiagVoltage(u8 u8Value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static enumDiagVoltage variable
                            \par Used Variables
    \ref enumDiagVoltage \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void DIAG_Set_enumDiagVoltage(Diag_Voltage_Error Value)
{
   enumDiagVoltage = Value;
}


/*-----------------------------------------------------------------------------*/
/**
    \fn static Diag_Voltage_Error DIAG_Get_enumDiagVoltage(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Gets the value of static enumDiagVoltage variable
                            \par Used Variables
    \ref enumDiagVoltage \n
    \return \b The value of static enumDiagVoltage variable.
    \note None.
    \warning None.
*/
static Diag_Voltage_Error DIAG_Get_enumDiagVoltage(void)
{
   return (enumDiagVoltage);
}

#endif  /* DIAGNOSTIC_STATE */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
