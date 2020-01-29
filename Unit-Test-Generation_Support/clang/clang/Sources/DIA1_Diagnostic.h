/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  DIA1_Diagnostic.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file DIA1_Diagnostic.h
    \version see \ref FMW_VERSION 
    \brief Basic Diagnostic functions prototypes.
    \details Routines for diagnostic operation and variables related initialization.\n
    \note none
    \warning none
*/

#ifndef DIAG_H
#define DIAG_H  


/* Public Constants ---------------------------------------------------------------------- --------------*/
/* Diagnostic monitor & ECONOMY input */
#if ( ( PCB == PCB220A )  || ( PCB == PCB220B ) || ( PCB == PCB220C ) || ( PCB == PCB224C ) || ( PCB == PCB224D ) || ( PCB == PCB225A ) || ( PCB == PCB225B ) || ( PCB == PCB225C ) || ( PCB == PCB233C ) || ( PCB == PCB230A ) || ( PCB == PCB240A ) || ( PCB == PCB236B ) )
  #define DIAG_ECON_RESISTOR_UP_PER_255       (u16)11985       /* R88*255 FOR PCB 220A  -> 47*255 */
  #define DIAG_ECON_RESISTOR_DOWN_PER_255     (u16)1912        /* R75*255 FOR PCB 220A  -> 7.5*2550 */

  #define DIAG_ALPHA_MON                      (u16)((u32)((u32)1024*V_MON_RESISTOR_DOWN_PER_255)/(u32)((u32)(V_MON_RESISTOR_UP_PER_255+V_MON_RESISTOR_DOWN_PER_255)*5))
  #define DIAG_ALPHA_SHORT_ECON               (u16)((u32)((u32)1024*DIAG_ECON_RESISTOR_DOWN_PER_255)/((u32)(DIAG_ECON_RESISTOR_UP_PER_255+DIAG_ECON_RESISTOR_DOWN_PER_255)*5))  
 
  #define DIAG_DELTA_VOLTAGE                  (u8)2   /* Maximum drop ([V]) between V_MON and ECON_ANA_IN (Used as short circuit monitor) */
  #define  DIAG_LOW_VOLTAGE_LIMIT             (u8)2   /* Absolute voltage drop [V] */
#endif /*PCB */

#if defined(DIAGNOSTIC_SHORT_ANALOG_PROTECTION)
  #define DIAG_ECON_RESISTOR_UP       (u16)(V_MON_RESISTOR_UP)
  #define DIAG_ECON_RESISTOR_DOWN     (u16)(V_MON_RESISTOR_DOWN)

  #define DIAG_ALPHA_MON              (u16)((u32)((u32)1024*V_MON_RESISTOR_DOWN)/(u32)((u32)(V_MON_RESISTOR_UP+V_MON_RESISTOR_DOWN)*5u))
  #define DIAG_ALPHA_SHORT_ECON       (u16)((u32)((u32)1024*DIAG_ECON_RESISTOR_DOWN)/((u32)(DIAG_ECON_RESISTOR_UP+DIAG_ECON_RESISTOR_DOWN)*5u))

  #define DIAG_DELTA_VOLTAGE          (u8)2   /* Maximum drop ([V]) between V_MON and ECON_ANA_IN (Used as short circuit monitor) */
  #define DIAG_LOW_VOLTAGE_LIMIT      (u8)2   /* Absolute voltage drop [V] */
#endif

/* Diagnostic parameters */
/*#define DIAGNOSTIC_ERROR_FREQ                  0.1 */     /* Periodo = 10 s */
#define DIAGNOSTIC_ISR_FREQ_CONST               (u16)1024   /* Scaler (potenza di 2) nella conversione del periodo in bit. */
/*#define DIAGNOSTIC_NEXT_ISR                   (u16)((u32)5000000/(u32)(DIAGNOSTIC_ERROR_FREQ*DIAGNOSTIC_ISR_FREQ_CONST)) */   /* Periodo del segnale di diagnostica in bit. */
#define DIAGNOSTIC_END_PERIOD                   (u16)DIAGNOSTIC_ISR_FREQ_CONST
/*#define DIAGNOSTIC_DUTY_BASE                  (u16)(DIAGNOSTIC_END_PERIOD/10) */  /* Duty base = periodo diagnostica/10 */

#define DIAG_RESTART_MASK_ATTEMPTS              (u8)3

#define DIAG_PARTIAL_STALL_DUTY_SEC             (u16)4 /* [s] */
#define DIAG_FULL_STALL_DUTY_SEC                (u16)3 /* [s] */
#define DIAG_OVER_VOLTAGE_DUTY_SEC              (u16)2 /* [s] */
#define DIAG_UNDER_VOLTAGE_DUTY_SEC             (u16)8 /* [s] */

#define DIAG_RELEASE_TIME_SEC                   (u16)5 /* [s] */

#define DIAG_PARTIAL_STALL_PERIOD_SEC           (u8)((u8)DIAG_PARTIAL_STALL_DUTY_SEC+(u8)DIAG_RELEASE_TIME_SEC)
#define DIAG_FULL_STALL_PERIOD_SEC              (u8)((u8)DIAG_FULL_STALL_DUTY_SEC+(u8)DIAG_RELEASE_TIME_SEC)
#define DIAG_OVER_VOLTAGE_PERIOD_SEC            (u8)((u8)DIAG_OVER_VOLTAGE_DUTY_SEC+(u8)DIAG_RELEASE_TIME_SEC)
#define DIAG_UNDER_VOLTAGE_PERIOD_SEC           (u8)((u8)DIAG_UNDER_VOLTAGE_DUTY_SEC+(u8)DIAG_RELEASE_TIME_SEC)

#define SHORT_CIRCUIT_DIAG_TIMEOUT_SEC    1 /* [s] */
#define SHORT_CIRCUIT_DIAG_TIMEOUT        (u8)((float32_t)((float32_t)SHORT_CIRCUIT_DIAG_TIMEOUT_SEC*(float32_t)REG_LOOP_TIME)/(float32_t)REG_LOOP_TIME_SEC)
#define DIAGNOSTIC_MAX_SHORT_NUMBER       (u8)4

/* Public type definition -------------------------------------------------- */
typedef enum
{
  DIAG_VOLTAGE_NO_ERR = 0,
  DIAG_UNDER_VOLTAGE  = 1,
  DIAG_OVER_VOLTAGE   = 2
} Diag_Voltage_Error;

#ifdef _CANTATA_

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
  BOOL boolDiagnosticShort;        /* !< True if a short at diagnostic output is found.                                */
  #endif  /* DIAGNOSTIC_SHORT_SW_PROTECTION */
} Diagnostic_s;

#endif /* _CANTATA_ */

/* Public Variables -------------------------------------------------------- */
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

/* Public  macros ---------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
void DIAG_InitDiagnostic(void);
/*void DIAG_SetFault(u16);*/
void DIAG_RemoveFault(void);
void DIAG_ManageDiagnosticToECU(SystStatus_t enLocState, u16 u16DiagErrorType);
BOOL DIAG_IsDiagnosticEnable (void);
void DIAG_DiagnosticShutDown(void);
void DIAG_Set_enumDiagVoltage(Diag_Voltage_Error Value);
#endif /* DIAGNOSTIC_STATE */

#endif /* DIAG_H */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
