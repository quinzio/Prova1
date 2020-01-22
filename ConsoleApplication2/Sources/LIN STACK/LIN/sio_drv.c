/*****************************************************************************
|      Project Name: S I O - D R I V E R
|         File Name: sio_drv.c
|
|       Description: Implementation of the SIO driver
|
|    Target systems: Motorola XC68HC912D60
|                    Motorola MC9S12DP256
|                    Freescale MC9S12XDP512
|
|          Compiler: Metrowerks ANSI-C/cC++ Compiler for HC12
|         Assembler: Metrowerks Assembler for HC12
|            Linker: Metrowerks SmartLinker
|
|-----------------------------------------------------------------------------
|               C O P Y R I G H T
|-----------------------------------------------------------------------------
|
|   Copyright (c) 2014 Vector Informatik GmbH          All rights reserved.
|
|   This software is copyright protected and proprietary to Vector Informatik GmbH.
|   Vector Informatik GmbH grants to you only those rights as set out in the
|   license conditions. All other rights remain with Vector Informatik GmbH.
|
|-----------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|-----------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     ------------------------------------
| Et           Thomas Ebert              Vector Informatik GmbH
| dh           Gunnar de Haan            Vector Informatik GmbH
| Wr           Hans Waaser               Vector Informatik GmbH
| Svh          Sven Hesselmann           Vector Informatik GmbH
| Ap           Andreas Pick              Vector Informatik GmbH
| Eta          Edgar Tongoona            Vector Informatik GmbH
| Bmo          Bastian Molkenthin        Vector Informatik GmbH
| Lpr          Lutz Pflueger             Vector Informatik GmbH
|-----------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|-----------------------------------------------------------------------------
| Date         Ver    Author  Description
| ----------   ----   ------  ------------------------------------------------
| 10-Oct-00    1.00   dH      first version
| 13-Nov-00    1.01   Et      deleted enabling and disabling SIO interrupts within
|                             the SIO driver RX interrupt
|                             adapted SIO driver to changed prototype of ApplSioErrorNotification()
|                             delete wakeup frame functionality
|                     dH      only call ApplSioByteIndication() if databyte received
| 16-Apr-01    1.02   Et      used type description as part of variable names
|                             added volatile keyword for read only dummy variables
|                             moved SioInitPowerOn() to SioInit()
| 14-May-01    1.03   Et      supported both derivative DXX and DPXX
|                             supported both compiler COSMIC and HIWARE
| 11-Jun-01    1.04   Et      ESCAN00000775: support OSEK category 2 interrupts
|                             ESCAN00000825: support different derivatives for HW platform HC12
|                             ESCAN00000826: support both UART interface 0 and UART interface 1
| 12-Dec-01    1.05   Et      ESCAN00001940: shortening of stop bit length
| 03-Jul-02    1.06   Wr      ESCAN00003148: Added defines for derivative C_PROCESSOR_HC12XX and C_PROCESSOR_S12XX
| 05-Aug-02    1.07   Wr      ESCAN00004496: add support for K-line driver
|                             ESCAN00003901: Added @near to ISR SioInterrupt()
| 18-Dec-02    1.08   Wr      ESCAN00004497: Extend API for multichannel support
| 24-Apr-03    1.09   Wr      ESCAN00000773: Support for IAR compiler
|                             ESCAN00005547: Reorganisation of function calls
| 03-Jun-04    2.00   Wr      Adapted to LIN driver 2.x implementation
| 09-Jul-04    2.01   Wr      Removed return value from ApplSioErrorNotification
| 11-Apr-05    2.02   Svh     Removed a ) which was not deleted
|                     Wr      Corrected defines for indexed API
|                     Wr      ESCAN00011931: Removed initialization of DDRS
|                     Wr      ESCAN00011933: Added functionality SIO_ENABLE_NO_ISR_FUNCTION
| 24-Oct-05    2.03   Svh     ESCAN00013751: UART may not transmit further bytes
|                             ESCAN00013820: Use new memory qualifier for variables
|                             ESCAN00014035: Change SIO driver from Hc12 to Mcs12x
| 2006-03-01   2.04   Ap      ESCAN00015519: Interrupt status variable type qualifier "static" removed for inline assembly
| 2006-03-08   2.05   Svh     ESCAN00015588: Add support for C_COMP_COSMIC_MCS12X_MSCAN12 and C_COMP_COSMIC_MCS12_MSCAN12 switches
| 2006-06-12   2.06   Ap      ESCAN00016594: Banked memory model support
| 2006-08-15   2.07   dH      ESCAN00017286: increase number of supported UARTs from 2 up to 8
| 2007-03-07   2.08   Ap      ESCAN00017961: K-LIN Implementation: For channel SIO_ENABLE_SIO_UART1 runtime check is implemented for kChannel_UART0
|                     Ap      ESCAN00019856: Cast added for strict compiler check settings
| 2007-06-05   2.09   Ap      ESCAN00020640: Interface for LIN API: l_ifc_tx and l_ifc_rx not used
| 2009-01-27   2.10   Ap      ESCAN00032598: Interrupt declaration for banked memory model adapted
| 2009-02-19   2.11   Ap      ESCAN00032798: LIN ISRs do not secure the GPAGE register when the global constants are used
| 2009-02-23   2.12   Ap      ESCAN00033308: Allow configuration of banked or non banked interrupt handling
| 2010-03-04   2.13   Eta     ESCAN00039978: Support for baudrate synchronisation and detection
| 2010-03-17   2.14   Eta     ESCAN00041659: Correct the calculation of baudrate during baudrate synchronisation
| 2010-05-19   2.15   Eta     ESCAN00043023: Interrupt flags of timer module not cleared correctly during baudrate synchronisation
| 2010-09-06   2.16   Eta     ESCAN00045158: Abort baudrate detection if measurement was not started during a valid synch break
| 2011-03-09   2.17   Bmo     ESCAN00049174: Add support for new IAR Compiler defines
| 2011-05-17   2.18   Bmo     ESCAN00050941: Compile error with IAR compiler when UART > 1 is used
| 2011-07-12   2.19   Ap      ESCAN00052097: User defined SynchBreak length
| 2011-08-31   2.20   Eta     ESCAN00053125: Add support for derivative S12G
| 2011-11-17   2.21   Bmo     ESCAN00054911: Disable SCI receiver during bus idle
| 2012-03-15   2.22   Bmo     ESCAN00057602: First frame after wakeup is not correctly handled (Only Baudrate Detection)
| 2012-05-12   2.23   Eta/Ap  ESCAN00058953: Support stop mode when entering LIN sleep
|                     Bmo     ESCAN00059008: Incorrect channel reported in framing error notification for channel > 0
| 2012-05-30   2.24   Eta     ESCAN00059169: Wrong parameter used when STOP mode is supported
| 2012-08-23   2.25   Ap      ESCAN00061087: J2602 framing error reported as data error
| 2012-11-21   2.26   Bmo     ESCAN00063182: Add support for MCS12XS
| 2013-04-23   2.27   Ap      ESCAN00065609: Support for S12ZVL
| 2013-04-26          Lpr     ESCAN00065977: Support for polling
| 2013-07-10   2.28   Lpr     ESCAN00068867: Add memory sections for FBL
| 2013-09-09   2.29   Eta     ESCAN00070309: Response error bit not set for framing error in first data byte
| 2013-09-18   2.29.01 Eta    ESCAN00070473: No LIN communication after reset with STOP mode active
| 2014-02-27   2.29.02 Eta    ESCAN00073563: Measurement for baudrate detection or synchronisation is started during an invalid sync break
| 2014-05-20   2.30.00 Bmo    ESCAN00075780: Support for polling mode for master nodes
| 2014-08-01   2.30.01 Ap     ESCAN00077568: Type casts corrected
| 2014-08-12   2.31.00 Ap     ESCAN00077755: Support interrupt nesting
|                      Ap     ESCAN00077818: Support for S12VR
|                      Ap     ESCAN00077502: Support for S12ZVM
****************************************************************************/


/*****************************************************************************
 * Include files
 *****************************************************************************/
#include "sio_inc.h"

#if defined( SIO_ENABLE_OSEK_OS )
# include "osek.h"
#endif

/*****************************************************************************
 * version check
 *****************************************************************************/
# if ( DRVLIN_MCS12XSCISTD_VERSION != 0x0231u )
#  error "Source and Header file are inconsistent!"
# endif
# if ( DRVLIN_MCS12XSCISTD_RELEASE_VERSION != 0x00u )
#  error "Source and Header file are inconsistent (release version)!"
# endif


/*****************************************************************************
 * Defines
 *****************************************************************************/

/* checks for valid static configuration of SIO driver ************************/

# define SIO_CHANNEL_PARA_UART0
# define SIO_CHANNEL_PARA_UART1
# define SIO_CHANNEL_PARA_UART2
# define SIO_CHANNEL_PARA_UART3
# define SIO_CHANNEL_PARA_UART4
# define SIO_CHANNEL_PARA_UART5
# define SIO_CHANNEL_PARA_UART6
# define SIO_CHANNEL_PARA_UART7

# if defined( SIO_ENABLE_SIO_UART0 )
#  define kLinRegHc12ScL_Baudrate_0        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_0        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_0      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_0      kLinRegHc12ScH_SynchBreak
# elif defined( SIO_ENABLE_SIO_UART1 )
#  define kLinRegHc12ScL_Baudrate_1        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_1        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_1      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_1      kLinRegHc12ScH_SynchBreak
# elif defined( SIO_ENABLE_SIO_UART2 )
#  define kLinRegHc12ScL_Baudrate_2        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_2        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_2      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_2      kLinRegHc12ScH_SynchBreak
# elif defined( SIO_ENABLE_SIO_UART3 )
#  define kLinRegHc12ScL_Baudrate_3        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_3        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_3      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_3      kLinRegHc12ScH_SynchBreak
# elif defined( SIO_ENABLE_SIO_UART4 )
#  define kLinRegHc12ScL_Baudrate_4        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_4        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_4      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_4      kLinRegHc12ScH_SynchBreak
# elif defined( SIO_ENABLE_SIO_UART5 )
#  define kLinRegHc12ScL_Baudrate_5        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_5        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_5      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_5      kLinRegHc12ScH_SynchBreak
# elif defined( SIO_ENABLE_SIO_UART6 )
#  define kLinRegHc12ScL_Baudrate_6        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_6        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_6      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_6      kLinRegHc12ScH_SynchBreak
# elif defined( SIO_ENABLE_SIO_UART7 )
#  define kLinRegHc12ScL_Baudrate_7        kLinRegHc12ScL_Baudrate
#  define kLinRegHc12ScH_Baudrate_7        kLinRegHc12ScH_Baudrate
#  define kLinRegHc12ScL_SynchBreak_7      kLinRegHc12ScL_SynchBreak
#  define kLinRegHc12ScH_SynchBreak_7      kLinRegHc12ScH_SynchBreak
# endif

#if (!defined( SIO_ENABLE_SIO_UART0 ) && \
     !defined( SIO_ENABLE_SIO_UART1 ) && \
     !defined( SIO_ENABLE_SIO_UART2 ) && \
     !defined( SIO_ENABLE_SIO_UART3 ) && \
     !defined( SIO_ENABLE_SIO_UART4 ) && \
     !defined( SIO_ENABLE_SIO_UART5 ) && \
     !defined( SIO_ENABLE_SIO_UART6 ) && \
     !defined( SIO_ENABLE_SIO_UART7 )     )
# error "At least one UART interface must be enabled!"
#endif

# if defined( kSioNumberOfChannels )
#  if ( kSioNumberOfChannels > 1 )
#   error "More than one UART interfaces are enabled!"
#  endif
# endif

#if ((!defined( SIO_ENABLE_OSEK_OS )) && (!defined( SIO_DISABLE_OSEK_OS )))
# error "OSEK support is neither enabled nor disabled!"
#else
# if (defined( SIO_ENABLE_OSEK_OS ) && defined( SIO_DISABLE_OSEK_OS ))
#  error "OSEK support is both enabled and disabled!"
# endif
#endif

#if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
# if defined ( C_PROCESSOR_MCS12X ) || defined ( C_PROCESSOR_MCS12XS ) || defined ( C_PROCESSOR_MCS12ZVL ) || defined ( C_PROCESSOR_MCS12ZVM ) || defined (C_PROCESSOR_MCS12G ) || defined (C_PROCESSOR_MCS12VR )
# else
#  error "Baudrate detection/synchronisation is not supported on this derivative"
# endif
# if defined( SIO_ENABLE_POLLINGMODE )
#  error "Baudrate detection/synchronisation not supported in polling mode."
# endif
#endif

#if defined ( SIO_ENABLE_BAUDRATE_SYNC )
# if ( kSioTIMPrescalerShiftValue > 5 )
#  error "TIM Prescaler setting in the configuration tool should be between 1 and 32"
# endif
#endif


/*============================================================================*/
/* Bitmasks of SCI Control Register 1 (SCCR1):                                */
/*============================================================================*/
/* #define  SCI_LOOPS    0x80   Loop mode select bit                          */
/* #define  SCI_WOMS     0x40   Wired-Or Mode for Serial Pins                 */
/* #define  SCI_RSRC     0x20   Receiver Source                               */
/* #define  SCI_M        0x10   Mode character length bit                     */
/* #define  SCI_WAKE     0x08   Wakeup condition bit                          */
/* #define  SCI_ILT      0x04   Idle line type bit                            */
/* #define  SCI_PEN      0x02   Parity enable bit                             */
/* #define  SCI_PT       0x01   Parity type bit                               */

/*=============================================================================*/
/* Bitmasks of SCI Control Register 2 (SCCR2):                                 */
/*=============================================================================*/
/* #define  SCI_TIE      0x80u   SCI transmit interrupt enable bit             */
/* #define  SCI_TCIE     0x40u   Transmission complete interrupt enable bit    */
#define  SCI_RIE         ((vuint8)0x20u)  /* SCI receive interrupt enable bit  */
/* #define  SCI_ILIE     0x10u   Idle line interrupt enable bit                */
#define  SCI_TE          ((vuint8)0x08u)  /* Transmitter enable bit            */
#define  SCI_RE          ((vuint8)0x04u)  /* Receiver enable bit               */
/* #define  SCI_RWU      0x02u   Receiver wakeup bit                           */
/* #define  SCI_SBK      0x01u   Send break bit                                */

/*=============================================================================*/
/* Bitmasks of SCI Status Register 1 (SCSR1):                                  */
/*=============================================================================*/
/* #define  SCI_TDRE     0x80u   Transmit Data Register Empty                  */
/* #define  SCI_TC       0x40u   Transmission complete bit                     */
#define  SCI_RDRF        ((vuint8)0x20u)  /* Receiver Data Register Empty      */
/* #define  SCI_IDLE     0x10u   IDLE line detected                            */
#define  SCI_OR          ((vuint8)0x08u)  /* Receiver overrun bit              */
#define  SCI_NF          ((vuint8)0x04u)  /* Receiver noise flag bit           */
#define  SCI_FE          ((vuint8)0x02u)  /* Receiver noise flag bit           */
#define  SCI_PE          ((vuint8)0x01u)  /* Receiver parity error bit         */

/*=============================================================================*/
/* Bitmasks of SCI Status Register 2 (SCSR2):                                  */
/*=============================================================================*/
/* #define  SCI_SCSWAI   0x80u   Transmit Data Register Empty                  */
/* #define  SCI_MIE      0x40u   Transmission complete bit                     */
/* #define  SCI_MDL1     0x20u   Receiver Data Register Empty                  */
/* #define  SCI_MDL0     0x10u   IDLE line detected                            */
/* #define  SCI_RAF      0x01u   Receiver parity error bit                     */

#define SIO_FRAMING_ERROR    SCI_FE
#define SIO_OVERRUN_ERROR    SCI_OR
#define SIO_NOISE_FLAG       SCI_NF
#define SIO_ERROR            ( SCI_PE | SCI_FE | SCI_OR | SCI_NF )
#define SIO_RDR_FLAG         SCI_RDRF
#define SIO_ALL_EVENTS       ( SCI_RDRF | SIO_ERROR )

/*============================================================================*/
/* Data struct for Timer Module (TIM)                                         */
/*============================================================================*/
#if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
# if defined ( C_PROCESSOR_MCS12ZVL ) || defined ( C_PROCESSOR_MCS12ZVM )
#  define SIO_TIOS          (*(volatile vuint8*) 0x5C0u)  /* Timer Input Capture/Output Compare Select */
#  define SIO_TCTL3         (*(volatile vuint8*) 0x5CAu)  /* Timer Control Register 3 */
#  define SIO_TCTL4         (*(volatile vuint8*) 0x5CBu)  /* Timer Control Register 4 */
#  define SIO_TIE           (*(volatile vuint8*) 0x5CCu)  /* Timer Interrupt Enable Register */
#  define SIO_TFLG1         (*(volatile vuint8*) 0x5CEu)  /* Main Timer Interrupt Flag 1 */
/* Timer Input Capture Compare Register High */
#  define SIO_TCxH          (*(volatile vuint8*)(0x5D0u+(kTimerChannel << 1)))
/* Timer Input Capture Compare Register Low */
#  define SIO_TCxL          (*(volatile vuint8*)(0x5D1u+(kTimerChannel << 1)))
#  define SIO_MODRR2        (*(volatile vuint8*) 0x202u)  /* Module Routing Register 2 */
#  define SIO_MODRR4        (*(volatile vuint8*) 0x204u)  /* Module Routing Register 4 */
# else
#  define SIO_TIOS          (*(volatile vuint8*) 0x40u)  /* Timer Input Capture/Output Compare Select */
#  define SIO_TCTL3         (*(volatile vuint8*) 0x4Au)  /* Timer Control Register 3 */
#  define SIO_TCTL4         (*(volatile vuint8*) 0x4Bu)  /* Timer Control Register 4 */
#  define SIO_TIE           (*(volatile vuint8*) 0x4Cu)  /* Timer Interrupt Enable Register */
#  define SIO_TFLG1         (*(volatile vuint8*) 0x4Eu)  /* Main Timer Interrupt Flag 1 */
/* Timer Input Capture Compare Register High */
#  define SIO_TCxH          (*(volatile vuint8*)(0x50u+(kTimerChannel << 1)))
/* Timer Input Capture Compare Register Low */
#  define SIO_TCxL          (*(volatile vuint8*)(0x51u+(kTimerChannel << 1)))
#  define SIO_MODRR2        (*(volatile vuint8*) 0x24Fu)  /* Module Routing Register */
# endif

# define kSioSyncOff                        ((vuint8)0x00u)
# define kSioSyncWaitFirstEdge              ((vuint8)0x01u)
# define kSioSyncWaitSecondEdge             ((vuint8)0x02u)
# define kSioSyncWaitThirdEdge              ((vuint8)0x03u)
#  if !defined kSioPulseMaxDeviation
#   define kSioPulseMaxDeviation            2
#  endif

# if defined ( SIO_ENABLE_BAUDRATE_DETECT )
#  if defined( SIO_ENABLE_SIO_UART0 )
#   undef kLinRegHc12ScL_Baudrate_0
#   define kLinRegHc12ScL_Baudrate_0        kLinRegHc12ScL_Baudrate_00
#   undef kLinRegHc12ScH_Baudrate_0
#   define kLinRegHc12ScH_Baudrate_0        kLinRegHc12ScH_Baudrate_00
#  elif defined( SIO_ENABLE_SIO_UART1 )
#   undef kLinRegHc12ScL_Baudrate_1
#   define kLinRegHc12ScL_Baudrate_1        kLinRegHc12ScL_Baudrate_10
#   undef kLinRegHc12ScH_Baudrate_1
#   define kLinRegHc12ScH_Baudrate_1        kLinRegHc12ScH_Baudrate_10
#  elif defined( SIO_ENABLE_SIO_UART2 )
#   undef kLinRegHc12ScL_Baudrate_2
#   define kLinRegHc12ScL_Baudrate_2        kLinRegHc12ScL_Baudrate_20
#   undef kLinRegHc12ScH_Baudrate_2
#   define kLinRegHc12ScH_Baudrate_2        kLinRegHc12ScH_Baudrate_20
#  elif defined( SIO_ENABLE_SIO_UART3 )
#   undef kLinRegHc12ScL_Baudrate_3
#   define kLinRegHc12ScL_Baudrate_3        kLinRegHc12ScL_Baudrate_30
#   undef kLinRegHc12ScH_Baudrate_3
#   define kLinRegHc12ScH_Baudrate_3        kLinRegHc12ScH_Baudrate_30
#  elif defined( SIO_ENABLE_SIO_UART4 )
#   undef kLinRegHc12ScL_Baudrate_4
#   define kLinRegHc12ScL_Baudrate_4        kLinRegHc12ScL_Baudrate_40
#   undef kLinRegHc12ScH_Baudrate_4
#   define kLinRegHc12ScH_Baudrate_4        kLinRegHc12ScH_Baudrate_40
#  elif defined( SIO_ENABLE_SIO_UART5 )
#   undef kLinRegHc12ScL_Baudrate_5
#   define kLinRegHc12ScL_Baudrate_5        kLinRegHc12ScL_Baudrate_50
#   undef kLinRegHc12ScH_Baudrate_5
#   define kLinRegHc12ScH_Baudrate_5        kLinRegHc12ScH_Baudrate_50
#  elif defined( SIO_ENABLE_SIO_UART6 )
#   undef kLinRegHc12ScL_Baudrate_6
#   define kLinRegHc12ScL_Baudrate_6        kLinRegHc12ScL_Baudrate_60
#   undef kLinRegHc12ScH_Baudrate_6
#   define kLinRegHc12ScH_Baudrate_6        kLinRegHc12ScH_Baudrate_60
#  elif defined( SIO_ENABLE_SIO_UART7 )
#   undef kLinRegHc12ScL_Baudrate_7
#   define kLinRegHc12ScL_Baudrate_7        kLinRegHc12ScL_Baudrate_70
#   undef kLinRegHc12ScH_Baudrate_7
#   define kLinRegHc12ScH_Baudrate_7        kLinRegHc12ScH_Baudrate_70
#  endif
# endif
#endif


/*****************************************************************************
 *   macros used to check chip hardware
 *****************************************************************************/

/* assertions */
/* not yet available */

/*****************************************************************************
 *   defines / data types / structs / unions
 *****************************************************************************/

/*============================================================================*/
/* Data struct for Serial Communication Interface Module (SCI)                */
/*============================================================================*/
typedef volatile struct
{
  vuint8   SCBDH;      /* SCI Baudrate Control Register */
  vuint8   SCBDL;      /* SCI Baudrate Control Register */
  vuint8   SCCR1;      /* SCI Control Register 1 */
  vuint8   SCCR2;      /* SCI Control Register 2 */
  vuint8   SCSR1;      /* SCI Status Register 1 */
  vuint8   SCSR2;      /* SCI Status Register 2 */
  vuint8   SCDRH;      /* SCI Data Register High */
  vuint8   SCDRL;      /* SCI Data Register Low */
} tSciModule;

/*============================================================================*/
/* Data definition for Serial Communication Interface Module (SCI)            */
/*============================================================================*/
#if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( C_PROCESSOR_MCS12ZVR )
# if defined( SIO_ENABLE_SIO_UART0 )
#  define sSciModule_0   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x00C8u)) )
# endif
# if defined( SIO_ENABLE_SIO_UART1 )
#  define sSciModule_1   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x00D0u)) )
# endif
# if defined ( C_PROCESSOR_MCS12XS ) || defined( C_PROCESSOR_MCS12ZVR )
#  if (defined ( SIO_ENABLE_SIO_UART2 ) || \
       defined ( SIO_ENABLE_SIO_UART3 ) || \
       defined ( SIO_ENABLE_SIO_UART4 ) || \
       defined ( SIO_ENABLE_SIO_UART5 ) || \
       defined ( SIO_ENABLE_SIO_UART6 ) || \
       defined ( SIO_ENABLE_SIO_UART7 ) )
#   error "Selected UART not supported on MCS12XS. Only UART 0 and 1 are supported"
#  endif
# endif
# if defined( SIO_ENABLE_SIO_UART2 )
#  if defined ( C_PROCESSOR_MCS12G )
#   define sSciModule_2   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x00E8u)) )
#  else
#   define sSciModule_2   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x00B8u)) )
#  endif
# endif
# if defined( C_PROCESSOR_MCS12G )
#  if (defined ( SIO_ENABLE_SIO_UART3 ) || \
       defined ( SIO_ENABLE_SIO_UART4 ) || \
       defined ( SIO_ENABLE_SIO_UART5 ) || \
       defined ( SIO_ENABLE_SIO_UART6 ) || \
       defined ( SIO_ENABLE_SIO_UART7 ) )
#   error "Selected UART not supported on MCS12G only UART 0, 1 and 2 are supported"
#  endif
# endif
# if defined( SIO_ENABLE_SIO_UART3 )
#  define sSciModule_3   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x00C0u)) )
# endif
# if defined( SIO_ENABLE_SIO_UART4 )
#  define sSciModule_4   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x0130u)) )
# endif
# if defined( SIO_ENABLE_SIO_UART5 )
#  define sSciModule_5   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x0138u)) )
# endif
# if defined( SIO_ENABLE_SIO_UART6 )
#  define sSciModule_6   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x0330u)) )
# endif
# if defined( SIO_ENABLE_SIO_UART7 )
#  define sSciModule_7   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x0338u)) )
# endif
#elif defined ( V_CPU_MCS12Z )
# if defined( SIO_ENABLE_SIO_UART0 )
#  define sSciModule_0   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x0700u)) )
# endif
# if defined( SIO_ENABLE_SIO_UART1 )
#  define sSciModule_1   (* ((tSciModule*)(V_REG_BLOCK_ADR+0x0710u)) )
# endif
#else
# error "Derivative is not supported by SIO/LIN driver!"
#endif

/*****************************************************************************
 * constants
 *****************************************************************************/
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 kSioMainVersion    = (vuint8)((DRVLIN_MCS12XSCISTD_VERSION >> 8) & 0xFFu);
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 kSioSubVersion     = (vuint8)(DRVLIN_MCS12XSCISTD_VERSION & 0x00FFu);
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 kSioReleaseVersion = (vuint8)(DRVLIN_MCS12XSCISTD_RELEASE_VERSION);

/*****************************************************************************
 * external declarations
 *****************************************************************************/

/*****************************************************************************
 * global data definitions
 *****************************************************************************/

/*****************************************************************************
 * local data definitions
 *****************************************************************************/

/* 'bIflag' is used in assembler for some controller / compiler and therfor can make a pclint warning */
V_MEMRAM0 V_MEMRAM1 vuint8 V_MEMRAM2 bIflag;


#if !defined ( SIO_ENABLE_POLLINGMODE )
# if defined( SIO_ENABLE_SIO_UART0 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_0;
# endif
# if defined( SIO_ENABLE_SIO_UART1 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_1;
# endif
# if defined( SIO_ENABLE_SIO_UART2 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_2;
# endif
# if defined( SIO_ENABLE_SIO_UART3 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_3;
# endif
# if defined( SIO_ENABLE_SIO_UART4 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_4;
# endif
# if defined( SIO_ENABLE_SIO_UART5 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_5;
# endif
# if defined( SIO_ENABLE_SIO_UART6 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_6;
# endif
# if defined( SIO_ENABLE_SIO_UART7 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bSioInterruptDisableCount_7;
#endif
#endif

#if defined ( SIO_ECUTYPE_SLAVE )
# if defined( SIO_ENABLE_SIO_UART0 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_0;
# endif
# if defined( SIO_ENABLE_SIO_UART1 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_1;
# endif
# if defined( SIO_ENABLE_SIO_UART2 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_2;
# endif
# if defined( SIO_ENABLE_SIO_UART3 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_3;
# endif
# if defined( SIO_ENABLE_SIO_UART4 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_4;
# endif
# if defined( SIO_ENABLE_SIO_UART5 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_5;
# endif
# if defined( SIO_ENABLE_SIO_UART6 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_6;
# endif
# if defined( SIO_ENABLE_SIO_UART7 )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bByteReceived_7;
# endif
#endif


#if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
V_MEMRAM0 static V_MEMRAM1 vuint8  V_MEMRAM2 bMeasurementStep;
V_MEMRAM0 static V_MEMRAM1 vuint16 V_MEMRAM2 wInitialTimerValue;
V_MEMRAM0 static V_MEMRAM1 vuint16 V_MEMRAM2 wSioTmpPulseLengthSum;
#endif

#if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_SUPPORT_STOP_MODE )
V_MEMRAM0 static V_MEMRAM1 vuint8  V_MEMRAM2 bSioResetMuteMode;
#  define   get_SioResetMuteMode(c)            (bSioResetMuteMode)
#  define   set_SioResetMuteMode(c, val)       (bSioResetMuteMode = val)
#endif

#if defined( V_CPU_MCS12Z ) && !defined( SIO_ENABLE_POLLINGMODE )
V_MEMRAM0 static V_MEMRAM1 vuint8 V_MEMRAM2 bInterruptOldState;
#endif



/*****************************************************************************
 * local prototypes
 *****************************************************************************/
#if defined ( V_COMP_IAR )
extern non_banked void disableint(vuint8 *);
extern non_banked void restoreint(vuint8);
#endif


/*****************************************************************************
 * Functions
 *****************************************************************************/

/************************************************************************
* NAME:            SioInit
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     initialize SCI (UART interface)
*************************************************************************/
/* disable PC-lint warning 550: "Symbol 'bDummy' (line YYY) not accessed" */
/*lint -e550*/
void SIO_API_CALL_TYPE SioInit( SIO_CHANNEL_TYPE_ONLY )
{
  /* volatile variable bDummy used for register readout must not be optimized away */
  volatile vuint8 bDummy;

#if defined( SIO_ENABLE_SIO_UART0 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_0.SCBDH = kLinRegHc12ScH_Baudrate_0;
    sSciModule_0.SCBDL = kLinRegHc12ScL_Baudrate_0;
    /* configure SCI */
    sSciModule_0.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_0.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_0.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_0 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_0.SCSR1;
    bDummy = sSciModule_0.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_0             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART0 */

#if defined( SIO_ENABLE_SIO_UART1 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_1.SCBDH = kLinRegHc12ScH_Baudrate_1;
    sSciModule_1.SCBDL = kLinRegHc12ScL_Baudrate_1;
    /* configure SCI */
    sSciModule_1.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_1.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_1.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_1 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_1.SCSR1;
    bDummy = sSciModule_1.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_1             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART1 */

#if defined( SIO_ENABLE_SIO_UART2 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_2.SCBDH = kLinRegHc12ScH_Baudrate_2;
    sSciModule_2.SCBDL = kLinRegHc12ScL_Baudrate_2;
    /* configure SCI */
    sSciModule_2.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_2.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_2.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_2 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_2.SCSR1;
    bDummy = sSciModule_2.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_2             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART2 */

#if defined( SIO_ENABLE_SIO_UART3 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_3.SCBDH = kLinRegHc12ScH_Baudrate_3;
    sSciModule_3.SCBDL = kLinRegHc12ScL_Baudrate_3;
    /* configure SCI */
    sSciModule_3.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_3.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_3.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_3 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_3.SCSR1;
    bDummy = sSciModule_3.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_3             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART3 */

#if defined( SIO_ENABLE_SIO_UART4 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_4.SCBDH = kLinRegHc12ScH_Baudrate_4;
    sSciModule_4.SCBDL = kLinRegHc12ScL_Baudrate_4;
    /* configure SCI */
    sSciModule_4.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_4.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_4.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_4 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_4.SCSR1;
    bDummy = sSciModule_4.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_4             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART4 */

#if defined( SIO_ENABLE_SIO_UART5 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_5.SCBDH = kLinRegHc12ScH_Baudrate_5;
    sSciModule_5.SCBDL = kLinRegHc12ScL_Baudrate_5;
    /* configure SCI */
    sSciModule_5.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_5.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_5.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_5 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_5.SCSR1;
    bDummy = sSciModule_5.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_5             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART5 */

#if defined( SIO_ENABLE_SIO_UART6 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_6.SCBDH = kLinRegHc12ScH_Baudrate_6;
    sSciModule_6.SCBDL = kLinRegHc12ScL_Baudrate_6;
    /* configure SCI */
    sSciModule_6.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_6.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_6.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_6 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_6.SCSR1;
    bDummy = sSciModule_6.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_6             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART6 */

#if defined( SIO_ENABLE_SIO_UART7 )
  {
    /* set normal baudrate for SYNCH, IDENTIFIER, DATA and CHECKSUM FIELD */
    sSciModule_7.SCBDH = kLinRegHc12ScH_Baudrate_7;
    sSciModule_7.SCBDL = kLinRegHc12ScL_Baudrate_7;
    /* configure SCI */
    sSciModule_7.SCCR1 = 0x00u;

# if defined( SIO_ENABLE_POLLINGMODE )
    /* enable receiver/transmitter interface */
    sSciModule_7.SCCR2 = ( SCI_TE | SCI_RE );
# else
    /* enable rx interrupt and receiver/transmitter interface */
    sSciModule_7.SCCR2 = (SCI_RIE | SCI_TE | SCI_RE);
    bSioInterruptDisableCount_7 = 0;
# endif
    /* clear pending SCI interrupts */
    bDummy = sSciModule_7.SCSR1;
    bDummy = sSciModule_7.SCDRL;


# if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_7             = 0;
# endif
  }
#endif  /* SIO_ENABLE_SIO_UART7 */

#if defined( V_CPU_MCS12Z ) && !defined( SIO_ENABLE_POLLINGMODE )
  bInterruptOldState = 0;
#endif

#if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
# if defined ( C_PROCESSOR_MCS12ZVM )
#  if defined  ( SIO_ENABLE_SIO_UART0 )
  /*  TIM0 input capture channel 3 is connected to RXD0 */
  SIO_MODRR2 &= ~(vuint8)0x08u;
  SIO_MODRR2 |= 0x04u;
#  elif defined( SIO_ENABLE_SIO_UART1 )
  /*  TIM0 input capture channel 3 is connected to RXD1 */
  SIO_MODRR2 &= ~(vuint8)0x04u;
  SIO_MODRR2 |= 0x08u;
#  endif
# elif defined ( C_PROCESSOR_MCS12ZVL )
#  if defined ( SIO_ENABLE_SIO_UART0 )
  SIO_MODRR4 = 0x01u; /* connect rx pin of sci0 to timer 0 channel 3 */
#  elif defined ( SIO_ENABLE_SIO_UART1 )
  SIO_MODRR4 = 0x02u; /* connect rx pin of sci1 to timer 0 channel 3 */
#  endif
# elif defined ( C_PROCESSOR_MCS12ZVR )
#  if defined ( SIO_ENABLE_SIO_UART0 )
  SIO_MODRR2 |= 0x80u; /* TIM input capture channel 3 is connected to LPRXD */
# elif defined ( SIO_ENABLE_SIO_UART1 )
  /* not possible to connect rx pin with timer channel internally, however could be possible if considered by custom hardware layout */
# endif
# endif

  SIO_TIOS &= ~(vuint8)(0x01u << kTimerChannel); /* selected channel acts as input capture */
  if ((kTimerChannel >= 0 ) && (kTimerChannel < 4)) /* configure falling edge */
  {
    SIO_TCTL4 &= ~(vuint8)(0x01u << (kTimerChannel << 1) );
    SIO_TCTL4 |=          (0x02u << (kTimerChannel << 1) );
  }
  else
  {
    SIO_TCTL3 &= ~(vuint8)(0x01u << ((kTimerChannel - 4) << 1) );
    SIO_TCTL3 |=          (0x02u << ((kTimerChannel - 4) << 1) );
  }
  SIO_TFLG1  =          (0x01u << kTimerChannel);  /* clear flag */
  SIO_TIE   &= ~(vuint8)(0x01u << kTimerChannel);  /* disable timer interrupt */
  bMeasurementStep = kSioSyncOff;
#endif
#if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_SUPPORT_STOP_MODE )
  set_SioResetMuteMode(siochannel, 1);  /* start in sleep mode */
#endif
}
/*lint +e550*/
/* enable PC-lint warning 550: "Symbol 'bDummy' (line YYY) not accessed" */






/************************************************************************
* NAME:            SioSendByte
* PARAMETER:       uint8 value
* RETURN VALUE:    void
* DESCRIPTION:     transmit SYNCH, IDENTIFIER, DATA or CHECKSUM FIELD
*                  with normal baudrate setting
*************************************************************************/
/* disable PC-lint warning 550: "Symbol 'bScsr1Reg' (line YYY) not accessed" */
/*lint -e550*/
void SIO_API_CALL_TYPE SioSendByte( SIO_CHANNEL_TYPE_FIRST vuint8 bValue )
{
  volatile vuint8 bScsr1Reg;

#if defined( SIO_ENABLE_SIO_UART0 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_0.SCSR1;
    /* send byte immediately */
    sSciModule_0.SCDRL = bValue;
  }
#endif

#if defined( SIO_ENABLE_SIO_UART1 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_1.SCSR1;
    /* send byte immediately */
    sSciModule_1.SCDRL = bValue;
  }
#endif

#if defined( SIO_ENABLE_SIO_UART2 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_2.SCSR1;
    /* send byte immediately */
    sSciModule_2.SCDRL = bValue;
  }
#endif

#if defined( SIO_ENABLE_SIO_UART3 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_3.SCSR1;
    /* send byte immediately */
    sSciModule_3.SCDRL = bValue;
  }
#endif

#if defined( SIO_ENABLE_SIO_UART4 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_4.SCSR1;
    /* send byte immediately */
    sSciModule_4.SCDRL = bValue;
  }
#endif

#if defined( SIO_ENABLE_SIO_UART5 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_5.SCSR1;
    /* send byte immediately */
    sSciModule_5.SCDRL = bValue;
  }
#endif

#if defined( SIO_ENABLE_SIO_UART6 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_6.SCSR1;
    /* send byte immediately */
    sSciModule_6.SCDRL = bValue;
  }
#endif

#if defined( SIO_ENABLE_SIO_UART7 )
  {
    /* clear TDRE bit if it is set */
    bScsr1Reg = sSciModule_7.SCSR1;
    /* send byte immediately */
    sSciModule_7.SCDRL = bValue;
  }
#endif
}
/*lint +e550*/
/* enable PC-lint warning 550: "Symbol 'bScsr1Reg' (line YYY) not accessed" */


#if defined( SIO_ENABLE_SIO_UART0 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART0
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_0 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_0 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_0 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_0();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_0 ( void )
# else
void SioInterruptTask_0 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_0, bScdrlReg_0;

  /* clear pending SCI interrupts */
  bScsr1Reg_0 = sSciModule_0.SCSR1;
  bScdrlReg_0 = sSciModule_0.SCDRL;
    asm(CLI);  //intterrupt nesting for SPAL

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_0 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_0 = (vuint8)(bByteReceived_0 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART0) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_0.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_0.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_0.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_0.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_0.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART0 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
      {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_0 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_0 != 0) && ( bByteReceived_0 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART0 kSioNoiseError );
          }
          else
# endif
#endif
        {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART0 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART0 kSioFrameError );
              if (bScdrlReg_0 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART0 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_0 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART0 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_0 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART0 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_0 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_0 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART0 bScdrlReg_0 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART0 */


#if defined( SIO_ENABLE_SIO_UART1 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART1
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_1 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_1 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_1 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_1();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_1 ( void )
# else
void SioInterruptTask_1 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_1, bScdrlReg_1;

  /* clear pending SCI interrupts */
  bScsr1Reg_1 = sSciModule_1.SCSR1;
  bScdrlReg_1 = sSciModule_1.SCDRL;

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_1 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_1 = (vuint8)(bByteReceived_1 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART1) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_1.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_1.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_1.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_1.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_1.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART1 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
    {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_1 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_1 != 0) && ( bByteReceived_1 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART1 kSioNoiseError );
          }
          else
# endif
#endif
          {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART1 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART1 kSioFrameError );
              if (bScdrlReg_1 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART1 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_1 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART1 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_1 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART1 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_1 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_1 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART1 bScdrlReg_1 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART1 */


#if defined( SIO_ENABLE_SIO_UART2 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART2
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_2 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_2 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_2 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_2();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_2 ( void )
# else
void SioInterruptTask_2 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_2, bScdrlReg_2;

  /* clear pending SCI interrupts */
  bScsr1Reg_2 = sSciModule_2.SCSR1;
  bScdrlReg_2 = sSciModule_2.SCDRL;

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_2 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_2 = (vuint8)(bByteReceived_2 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART2) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_2.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_2.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_2.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_2.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_2.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART2 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
      {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_2 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_2 != 0) && ( bByteReceived_2 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART2 kSioNoiseError );
          }
          else
# endif
#endif
          {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART2 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART2 kSioFrameError );
              if (bScdrlReg_2 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART2 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_2 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART2 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_2 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART2 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_2 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_2 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART2 bScdrlReg_2 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART2 */


#if defined( SIO_ENABLE_SIO_UART3 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART3
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_3 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_3 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_3 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_3();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_3 ( void )
# else
void SioInterruptTask_3 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_3, bScdrlReg_3;

  /* clear pending SCI interrupts */
  bScsr1Reg_3 = sSciModule_3.SCSR1;
  bScdrlReg_3 = sSciModule_3.SCDRL;

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_3 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_3 = (vuint8)(bByteReceived_3 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART3) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_3.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_3.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_3.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_3.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_3.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART3 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
      {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_3 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_3 != 0) && ( bByteReceived_3 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART3 kSioNoiseError );
          }
          else
# endif
#endif
          {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART3 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART3 kSioFrameError );
              if (bScdrlReg_3 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART3 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_3 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART3 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_3 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART3 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_3 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_3 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART3 bScdrlReg_3 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART3 */


#if defined( SIO_ENABLE_SIO_UART4 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART4
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_4 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_4 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_4 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_4();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_4 ( void )
# else
void SioInterruptTask_4 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_4, bScdrlReg_4;

  /* clear pending SCI interrupts */
  bScsr1Reg_4 = sSciModule_4.SCSR1;
  bScdrlReg_4 = sSciModule_4.SCDRL;

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_4 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_4 = (vuint8)(bByteReceived_4 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART4) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_4.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_4.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_4.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_4.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_4.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART4 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
      {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_4 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_4 != 0) && ( bByteReceived_4 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART4 kSioNoiseError );
          }
          else
# endif
#endif
          {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART4 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART4 kSioFrameError );
              if (bScdrlReg_4 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART4 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_4 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART4 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_4 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART4 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_4 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_4 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART4 bScdrlReg_4 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART4 */


#if defined( SIO_ENABLE_SIO_UART5 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART5
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_5 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_5 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_5 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_5();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_5 ( void )
# else
void SioInterruptTask_5 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_5, bScdrlReg_5;

  /* clear pending SCI interrupts */
  bScsr1Reg_5 = sSciModule_5.SCSR1;
  bScdrlReg_5 = sSciModule_5.SCDRL;

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_5 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_5 = (vuint8)(bByteReceived_5 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART5) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_5.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_5.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_5.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_5.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_5.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART5 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
      {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_5 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_5 != 0) && ( bByteReceived_5 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART5 kSioNoiseError );
          }
          else
# endif
#endif
          {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART5 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART5 kSioFrameError );
              if (bScdrlReg_5 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART5 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_5 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART5 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_5 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART5 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_5 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_5 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART5 bScdrlReg_5 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART5 */


#if defined( SIO_ENABLE_SIO_UART6 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART6
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_6 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_6 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_6 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_6();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_6 ( void )
# else
void SioInterruptTask_6 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_6, bScdrlReg_6;

  /* clear pending SCI interrupts */
  bScsr1Reg_6 = sSciModule_6.SCSR1;
  bScdrlReg_6 = sSciModule_6.SCDRL;

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_6 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_6 = (vuint8)(bByteReceived_6 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART6) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_6.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_6.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_6.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_6.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_6.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART6 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
      {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_6 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_6 != 0) && ( bByteReceived_6 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART6 kSioNoiseError );
          }
          else
# endif
#endif
          {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART6 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART6 kSioFrameError );
              if (bScdrlReg_6 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART6 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_6 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART6 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_6 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART6 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_6 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_6 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART6 bScdrlReg_6 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART6 */


#if defined( SIO_ENABLE_SIO_UART7 )
/************************************************************************
* NAME:            SioInterrupt /  SioPollingFunction of UART7
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI receive, transmit and error handling
*************************************************************************/
#if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  void l_ifc_rx_7 ( void )
#elif defined( SIO_ENABLE_OSEK_OS )
  ISR( l_ifc_rx_7 )
#else
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG __NEAR_SEG NON_BANKED
# endif
SIO_ISR_CALL_TYPE void l_ifc_rx_7 ( void )  /* non banked ISR routine */
{
  SioInterruptTask_7();        /* call banked routine to service UART IRQ */
}
/* switch back to the default code segment */
# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
     defined( V_COMP_FREESCALE_MCS12Z )
#  pragma CODE_SEG DEFAULT
# endif
# if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioInterruptTask_7 ( void )
# else
void SioInterruptTask_7 ( void )
# endif
#endif
{
  volatile vuint8 bScsr1Reg_7, bScdrlReg_7;

  /* clear pending SCI interrupts */
  bScsr1Reg_7 = sSciModule_7.SCSR1;
  bScdrlReg_7 = sSciModule_7.SCDRL;

#if defined( SIO_ENABLE_POLLINGMODE )
  if ( (bScsr1Reg_7 & SIO_ALL_EVENTS) != 0)
#else
# if defined ( SIO_ENABLE_IRQ_NESTING )
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
     cli
  }
#  endif
#  if defined( V_CPU_MCS12Z )
  _asm("cli");
#  endif
# endif
#endif
  {
#if defined ( SIO_ECUTYPE_SLAVE )
    bByteReceived_7 = (vuint8)(bByteReceived_7 + 1);  /* increment byte count */
#endif

#if defined ( SIO_SUPPORT_STOP_MODE )
    if (get_SioResetMuteMode(kChannel_UART7) == 1)
    { /* wake up from sleep mode - clear RXEDGIF */
      sSciModule_7.SCSR2 |= 0x80u;  /* AMAP = 1 */
      if ( (sSciModule_7.SCBDH & 0x80u) != 0)
      {
        /* RXEDGIF is set */
        sSciModule_7.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
        sSciModule_7.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
      }
      sSciModule_7.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
      /* report wake up event */
      ApplSioErrorNotification( SIO_CHANNEL_PARA_UART7 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
    }
    else
#endif
    {

#if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ((bMeasurementStep >= kSioSyncWaitFirstEdge) && (bMeasurementStep <= kSioSyncWaitThirdEdge))
      {
        /* while measuring a SynchField character no UART interrupt is reported to the protocol driver to prevent stop of measurement */
      }
      else
#endif
      {
        /* SCI (framing) error interrupt */
        if ( (bScsr1Reg_7 & SIO_FRAMING_ERROR) != 0 ) /* FE : framing error flag */
        {
#if defined ( SIO_ENABLE_J2602_EXTENSION )
#else
# if defined ( SIO_ECUTYPE_SLAVE )
          if (( bScdrlReg_7 != 0) && ( bByteReceived_7 == 4) )
          {/* if receive buffer is not 0 in case of framing error this is not a new synch break and start of a new frame */
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART7 kSioNoiseError );
          }
          else
# endif
#endif
          {
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
            if (get_SioResetMuteMode(siochannel) == 1)
            { /* do not start sync field measurement for baudrate detection on a wakeup frame in order to correctly handle next frame */
              set_SioResetMuteMode(siochannel, 0);
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART7 kSioFrameError );  /* must be called after check of bSioResetMuteMode */
            }
            else
#  endif
            {
              ApplSioErrorNotification( SIO_CHANNEL_PARA_UART7 kSioFrameError );
              if (bScdrlReg_7 == 0)
              { /* Start of SF measurement */
                SIO_TFLG1  = (0x01u << kTimerChannel);      /* clear flag */
                SIO_TIE   |= (0x01u << kTimerChannel);      /* enable timer interrupt */
                bMeasurementStep = kSioSyncWaitFirstEdge;
              }
            }
# else
            ApplSioErrorNotification( SIO_CHANNEL_PARA_UART7 kSioFrameError );
# endif /* SIO_ENABLE_BAUDRATE_SYNC */
          }
        }

        /* SCI (overrun) error interrupt */
        if ( (bScsr1Reg_7 & SIO_OVERRUN_ERROR) != 0 ) /* OVE : overrun error flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART7 kSioOverrunError );
        }

        /* SCI (noise) error interrupt */
        if ( (bScsr1Reg_7 & SIO_NOISE_FLAG) != 0 ) /* NF: noise flag */
        {
          ApplSioErrorNotification( SIO_CHANNEL_PARA_UART7 kSioNoiseError );
        }

        /* SCI receive interrupt without any occured errors */
        if ( ((bScsr1Reg_7 & SIO_RDR_FLAG) != 0) && ((bScsr1Reg_7 & SIO_ERROR) == 0) ) /* RDRF : received data ready flag */
        {
          ApplSioByteIndication( SIO_CHANNEL_PARA_UART7 bScdrlReg_7 );
        }
      }
    }
  }
}
#endif /* SIO_ENABLE_SIO_UART7 */

#if defined ( SIO_ECUTYPE_SLAVE )
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
/******************************************************************************
* NAME:            l_ifc_measure
* PARAMETER:       void
* RETURN VALUE:    void
* DESCRIPTION:     SCI Interrupt handling in one function
*                  Include this function into Rx and Err Interrupt Vector
*******************************************************************************/
#  if defined ( SIO_ENABLE_NO_ISR_FUNCTION )
void l_ifc_measure( void )
#  elif defined ( SIO_ENABLE_OSEK_OS )
ISR( l_ifc_measure )
#  else
#   if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
       defined( V_COMP_FREESCALE_MCS12Z )
#    pragma CODE_SEG __NEAR_SEG NON_BANKED
#   endif
SIO_ISR_CALL_TYPE void l_ifc_measure ( void ) /* non banked ISR routine */
{
  SioTimerInterrupt(); /* call banked routine to service Timer IRQ */
}
/* switch back to the default code segment */
#   if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
       defined( V_COMP_FREESCALE_MCS12Z )
#    pragma CODE_SEG DEFAULT
#   endif

#   if defined( V_COMP_IAR )
void SIO_API_CALL_TYPE SioTimerInterrupt ( void )
#   else
void SioTimerInterrupt ( void )
#   endif
#  endif
{
  vuint16 wSioPulseLength;
  vuint16 wCompareTimerValue;

  SIO_TFLG1  = (0x01u << kTimerChannel);  /* clear flag */
  wCompareTimerValue = (vuint16)((SIO_TCxH << 8) | SIO_TCxL);

  if ( bMeasurementStep == kSioSyncWaitFirstEdge )
  { /* first falling edge of SF in start bit */
    wInitialTimerValue = wCompareTimerValue; /* store initial timer value */
    /* wait for next falling edge - step 2 */
    bMeasurementStep = kSioSyncWaitSecondEdge;
  }
  else if ( bMeasurementStep == kSioSyncWaitSecondEdge )
  { /* next falling edge of SF after start bit */
    if (wCompareTimerValue < wInitialTimerValue)
    { /* timer overflow occured */
      wSioPulseLength = (vuint16)((0xFFFFu - wInitialTimerValue) + wCompareTimerValue);
    }
    else
    {
      wSioPulseLength = (vuint16)(wCompareTimerValue - wInitialTimerValue);
    }
    wSioTmpPulseLengthSum = wSioPulseLength;
    /* take last capture value as initial value for next measurement step */

#  if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ( (wSioTmpPulseLengthSum <= (kSioPulseMinLength_0)) || wSioTmpPulseLengthSum >= ((kSioPulseMaxLength_1)) ||
          ((wSioTmpPulseLengthSum >= (kSioPulseMaxLength_0)) && wSioTmpPulseLengthSum <= ((kSioPulseMinLength_1)))  )
#  else
      if ( ( wSioTmpPulseLengthSum <= kSioPulseMinLength ) || (  wSioTmpPulseLengthSum >= kSioPulseMaxLength  ) )
#  endif
      {
        /* abort synchronization */
        SIO_TIE &= ~(vuint8)(0x01u << kTimerChannel);
        bMeasurementStep =  kSioSyncOff;
      }
      else
      {
        /* take last capture value as initial value for next measurement step */
        wInitialTimerValue = wCompareTimerValue;
        bMeasurementStep = kSioSyncWaitThirdEdge;
      }
  }
  else if ( bMeasurementStep == kSioSyncWaitThirdEdge )
  { /* next falling edge of SF */
    if (wCompareTimerValue < wInitialTimerValue)
    { /* timer overflow occured */
      wSioPulseLength = (vuint16)((0xFFFFu - wInitialTimerValue) + wCompareTimerValue);
    }
    else
    {
      wSioPulseLength = (vuint16)(wCompareTimerValue - wInitialTimerValue);
    }

#  if defined ( SIO_ENABLE_BAUDRATE_DETECT ) && !defined ( SIO_ENABLE_BAUDRATE_SYNC )
    /* plausibility check - new pulse length must be same as previous plus measurement tolerance */
    if (  (wSioPulseLength >=          (wSioTmpPulseLengthSum - kSioPulseMaxDeviation))
       && (wSioPulseLength <= (vuint16)(wSioTmpPulseLengthSum + kSioPulseMaxDeviation)) )
    {
      /* Check range of counter value to be within +/- 15 % of nominal baudrate */
      if ( ( wSioPulseLength >= kSioPulseMinLength_0 ) && (  wSioPulseLength <= kSioPulseMaxLength_0  ) )
      {
#   if defined( SIO_ENABLE_SIO_UART0 )
        sSciModule_0.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_00);
        sSciModule_0.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_00);
#   endif  /* SIO_ENABLE_SIO_UART0 */

#   if defined( SIO_ENABLE_SIO_UART1 )
        sSciModule_1.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_10);
        sSciModule_1.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_10);
#   endif  /* SIO_ENABLE_SIO_UART1 */

#   if defined( SIO_ENABLE_SIO_UART2 )
        sSciModule_2.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_20);
        sSciModule_2.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_20);
#   endif  /* SIO_ENABLE_SIO_UART2 */

#   if defined( SIO_ENABLE_SIO_UART3 )
        sSciModule_3.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_30);
        sSciModule_3.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_30);
#   endif  /* SIO_ENABLE_SIO_UART3 */

#   if defined( SIO_ENABLE_SIO_UART4 )
        sSciModule_4.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_40);
        sSciModule_4.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_40);
#   endif  /* SIO_ENABLE_SIO_UART4 */

#   if defined( SIO_ENABLE_SIO_UART5 )
        sSciModule_5.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_50);
        sSciModule_5.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_50);
#   endif  /* SIO_ENABLE_SIO_UART5 */

#   if defined( SIO_ENABLE_SIO_UART6 )
        sSciModule_6.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_60);
        sSciModule_6.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_60);
#   endif  /* SIO_ENABLE_SIO_UART6 */

#   if defined( SIO_ENABLE_SIO_UART7 )
        sSciModule_7.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_70);
        sSciModule_7.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_70);
#   endif  /* SIO_ENABLE_SIO_UART7 */

#   if defined(SIO_LL_ENABLE_BAUDRATE_INFORMATION)
        ApplSioBaudrateInformation ( 0 );
#   endif
#   if defined ( SIO_ENABLE_CLOCK_DEV_INDICATION )
        /* indicate clock deviation to application - negative value indicates too slow positive too high system clock  */
        ApplSioClockDeviation( kSioPulseNominalLength_0 - wSioPulseLength );
#   endif

      }
      else if ( ( wSioPulseLength >= kSioPulseMinLength_1 ) && (  wSioPulseLength <= kSioPulseMaxLength_1  ) )
      {
#   if defined( SIO_ENABLE_SIO_UART0 )
        sSciModule_0.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_01);
        sSciModule_0.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_01);
#   endif  /* SIO_ENABLE_SIO_UART0 */

#   if defined( SIO_ENABLE_SIO_UART1 )
        sSciModule_1.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_11);
        sSciModule_1.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_11);
#   endif  /* SIO_ENABLE_SIO_UART1 */

#   if defined( SIO_ENABLE_SIO_UART2 )
        sSciModule_2.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_21);
        sSciModule_2.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_21);
#   endif  /* SIO_ENABLE_SIO_UART2 */

#   if defined( SIO_ENABLE_SIO_UART3 )
        sSciModule_3.SCBDH   = (vuint8)(kLinRegHc12ScH_Baudrate_31);
        sSciModule_3.SCBDL   = (vuint8)(kLinRegHc12ScL_Baudrate_31);
#   endif  /* SIO_ENABLE_SIO_UART3 */

#   if defined( SIO_ENABLE_SIO_UART4 )
        sSciModule_4.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_41);
        sSciModule_4.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_41);
#   endif  /* SIO_ENABLE_SIO_UART4 */

#   if defined( SIO_ENABLE_SIO_UART5 )
        sSciModule_5.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_51);
        sSciModule_5.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_51);
#   endif  /* SIO_ENABLE_SIO_UART5 */

#   if defined( SIO_ENABLE_SIO_UART6 )
        sSciModule_6.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_61);
        sSciModule_6.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_61);
#   endif  /* SIO_ENABLE_SIO_UART6 */

#   if defined( SIO_ENABLE_SIO_UART7 )
        sSciModule_7.SCBDH    = (vuint8)(kLinRegHc12ScH_Baudrate_71);
        sSciModule_7.SCBDL    = (vuint8)(kLinRegHc12ScL_Baudrate_71);
#   endif  /* SIO_ENABLE_SIO_UART7 */

#   if defined(SIO_LL_ENABLE_BAUDRATE_INFORMATION)
        ApplSioBaudrateInformation ( 1 );
#   endif
#   if defined ( SIO_ENABLE_CLOCK_DEV_INDICATION )
        /* indicate clock deviation to application - negative value indicates too slow positive too high system clock  */
        ApplSioClockDeviation( kSioPulseNominalLength_1 - wSioPulseLength );
#   endif

      }
      else
      { /* detected pulse length does not match configured baudrates */
      }
    }
    /* clear CHnF and stop active edge detection by disable appropriate IRQ */
    SIO_TIE &= ~(vuint8)(0x01u << kTimerChannel);
    bMeasurementStep = kSioSyncOff;

#  endif /* defined ( SIO_ENABLE_BAUDRATE_DETECT ) && !defined(SIO_ENABLE_BAUDRATE_SYNC) */

#  if defined ( SIO_ENABLE_BAUDRATE_SYNC )
    /* plausibility check - new pulse length must be same as previous plus measurement tolerance */
    if (  (wSioPulseLength >=          (wSioTmpPulseLengthSum - kSioPulseMaxDeviation))
       && (wSioPulseLength <= (vuint16)(wSioTmpPulseLengthSum + kSioPulseMaxDeviation)) )
    {

#   if defined ( SIO_ENABLE_BAUDRATE_DETECT )
      if ( ( ( wSioTmpPulseLengthSum >= kSioPulseMinLength_0 ) && (  wSioTmpPulseLengthSum <= kSioPulseMaxLength_0 ) )
       || ( ( wSioTmpPulseLengthSum >= kSioPulseMinLength_1 ) && (  wSioTmpPulseLengthSum <= kSioPulseMaxLength_1 ) ) )
#   else
      if ( ( wSioTmpPulseLengthSum >= kSioPulseMinLength ) && (  wSioTmpPulseLengthSum <= kSioPulseMaxLength  ) )
#   endif
      {
#   if defined ( SIO_ENABLE_BAUDRATE_DETECT )
#    if defined(SIO_LL_ENABLE_BAUDRATE_INFORMATION)
        if ( ( wSioTmpPulseLengthSum >= kSioPulseMinLength_0 ) && (  wSioTmpPulseLengthSum <= kSioPulseMaxLength_0 ) )
        {
          ApplSioBaudrateInformation ( 0 );
        }
        else
        {
          ApplSioBaudrateInformation ( 1 );
        }
#    endif
#   endif
        /* add current measurment value to existing value */
        wSioTmpPulseLengthSum = (vuint16)(wSioTmpPulseLengthSum + wSioPulseLength );
        /* (1 << (5-kSioTIMPrescalerShiftValue)) is added to round up the resulting BR value */
        wSioTmpPulseLengthSum = (vuint16)(wSioTmpPulseLengthSum + (vuint8)(1 << (5-kSioTIMPrescalerShiftValue)) );

        /* TIM_Prescaler can be 1, 2, 4, 8, 16, 32, 64, 128                                     */
        /* kSioTIMPrescalerShiftValue (binary power of TIM_prescaler) 0, 1, 2, 3, 4, 5, 6, 7    */

#   if defined ( V_CPU_MCS12Z )
#    if (kSioTIMPrescalerShiftValue < 3)
        wSioTmpPulseLengthSum = wSioTmpPulseLengthSum >> (2-kSioTIMPrescalerShiftValue);
#    endif
#    if (kSioTIMPrescalerShiftValue >= 3)
        wSioTmpPulseLengthSum = wSioTmpPulseLengthSum << (kSioTIMPrescalerShiftValue-2);
#    endif
#   else
#    if (kSioTIMPrescalerShiftValue < 6)
        wSioTmpPulseLengthSum = wSioTmpPulseLengthSum >> (6-kSioTIMPrescalerShiftValue);
#    endif
#   endif

#   if defined( SIO_ENABLE_SIO_UART0 )
        sSciModule_0.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_0.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART0 */

#   if defined( SIO_ENABLE_SIO_UART1 )
        sSciModule_1.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_1.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART1 */

#   if defined( SIO_ENABLE_SIO_UART2 )
        sSciModule_2.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_2.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART2 */

#   if defined( SIO_ENABLE_SIO_UART3 )
        sSciModule_3.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_3.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART3 */

#   if defined( SIO_ENABLE_SIO_UART4 )
        sSciModule_4.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_4.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART4 */

#   if defined( SIO_ENABLE_SIO_UART5 )
        sSciModule_5.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_5.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART5 */

#   if defined( SIO_ENABLE_SIO_UART6 )
        sSciModule_6.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_6.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART6 */

#   if defined( SIO_ENABLE_SIO_UART7 )
        sSciModule_7.SCBDH   = (vuint8)((wSioTmpPulseLengthSum >> 8) & 0xFFu);
        sSciModule_7.SCBDL   = (vuint8)( wSioTmpPulseLengthSum       & 0xFFu);
#   endif  /* SIO_ENABLE_SIO_UART7 */

#   if defined ( SIO_ENABLE_CLOCK_DEV_INDICATION )
        /* indicate clock deviation to application - negative value indicates too slow positive too high system clock  */
        ApplSioClockDeviation( kSioPulseNominalLength - wSioPulseLength );
#   endif

      }
    }
    /* clear CHnF and stop active edge detection by disable appropriate IRQ */
    SIO_TIE &= ~(vuint8)(0x01u << kTimerChannel);
    bMeasurementStep = kSioSyncOff;
#  endif  /* SIO_ENABLE_BAUDRATE_SYNC */

  }
  else
  { /* unexpected step */
    /* clear CHnF and stop active edge detection by disable appropriate IRQ */
    SIO_TIE &= ~(vuint8)(0x01u << kTimerChannel);
    bMeasurementStep = kSioSyncOff;
  }
}
# endif /* SIO_ENABLE_BAUDRATE_DETECT || SIO_ENABLE_BAUDRATE_SYNC */

/************************************************************************
* NAME:            SioSetMuteMode
* PARAMETER:       siochannel parameter
* RETURN VALUE:    void
* DESCRIPTION:     Set hardware to mute mode
*************************************************************************/
void SioSetMuteMode(SIO_CHANNEL_TYPE_ONLY)
{
# if defined ( SIO_ENABLE_BAUDRATE_SYNC ) || defined ( SIO_ENABLE_BAUDRATE_DETECT )
 /* stop active edge detection by disabling appropriate IRQ */
  SIO_TIE &= ~(vuint8)(0x01u << kTimerChannel);  /* disable timer interrupt */
  bMeasurementStep = kSioSyncOff;
# endif /* SIO_ENABLE_BAUDRATE_SYNC || SIO_ENABLE_BAUDRATE_DETECT */
# if defined( SIO_ENABLE_SIO_UART0 )
   bByteReceived_0             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_0.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_0.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_0.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_0.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART0 */

# if defined( SIO_ENABLE_SIO_UART1 )
   bByteReceived_1             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_1.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_1.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_1.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_1.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART1 */

# if defined( SIO_ENABLE_SIO_UART2 )
   bByteReceived_2             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_2.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_2.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_2.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_2.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART2 */

# if defined( SIO_ENABLE_SIO_UART3 )
   bByteReceived_3             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_3.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_3.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_3.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_3.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART3 */

# if defined( SIO_ENABLE_SIO_UART4 )
   bByteReceived_4             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_4.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_4.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_4.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_4.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART4 */

# if defined( SIO_ENABLE_SIO_UART5 )
   bByteReceived_5             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_5.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_5.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_5.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_5.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART5 */

# if defined( SIO_ENABLE_SIO_UART6 )
   bByteReceived_6             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_6.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_6.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_6.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_6.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART6 */

# if defined( SIO_ENABLE_SIO_UART7 )
   bByteReceived_7             = 0;
#  if defined ( SIO_SUPPORT_STOP_MODE )
   if (get_SioResetMuteMode(siochannel) == 1)
   {
     /* wake up from sleep mode - clear RXEDGIF */
     sSciModule_7.SCSR2 |= 0x80u;  /* AMAP = 1 */
     sSciModule_7.SCBDH |= 0x80u; /* clear RXEDGIF by writing 1 */
     sSciModule_7.SCBDL &= ~(vuint8)0x80u; /* RXEDGIE = 0 */
     sSciModule_7.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
   }
#  endif /* SIO_SUPPORT_STOP_MODE */
# endif  /* SIO_ENABLE_SIO_UART7 */

# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_SUPPORT_STOP_MODE )
   set_SioResetMuteMode(siochannel, 0);
# endif /* SIO_ENABLE_BAUDRATE_DETECT */

}

/************************************************************************
* NAME:            SioResetMuteMode
* PARAMETER:       siochannel parameter
* RETURN VALUE:    void
* DESCRIPTION:     Called by protocol driver in transition to sleep mode
*************************************************************************/
void SioResetMuteMode(SIO_CHANNEL_TYPE_ONLY)
{
# if defined ( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_SUPPORT_STOP_MODE )
  set_SioResetMuteMode(siochannel, 1);
# endif /* SIO_ENABLE_BAUDRATE_DETECT */

# if defined ( SIO_SUPPORT_STOP_MODE )
  /* enable RXEDGIE to be able to detect a falling edge on rx pin in stop mode */
#  if defined( SIO_ENABLE_SIO_UART0 )
  {
    sSciModule_0.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_0.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_0.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_0.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART0 */

#  if defined( SIO_ENABLE_SIO_UART1 )
  {
    sSciModule_1.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_1.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_1.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_1.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART1 */

#  if defined( SIO_ENABLE_SIO_UART2 )
  {
    sSciModule_2.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_2.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_2.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_2.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART2 */

#  if defined( SIO_ENABLE_SIO_UART3 )
  {
    sSciModule_3.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_3.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_3.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_3.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART3 */

#  if defined( SIO_ENABLE_SIO_UART4 )
  {
    sSciModule_4.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_4.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_4.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_4.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART4 */

#  if defined( SIO_ENABLE_SIO_UART5 )
  {
    sSciModule_5.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_5.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_5.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_5.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART5 */

#  if defined( SIO_ENABLE_SIO_UART6 )
  {
    sSciModule_6.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_6.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_6.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_6.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART6 */

#  if defined( SIO_ENABLE_SIO_UART7 )
  {
    sSciModule_7.SCSR2 |= 0x80u;  /* AMAP = 1 */
    sSciModule_7.SCBDH |= 0x80u;  /* clear RXEDGIF by writing 1 */
    sSciModule_7.SCBDL |= 0x80u;  /* SCIACR1: RXEDGIE = 1 */
    sSciModule_7.SCSR2 &= ~(vuint8)0x80u; /* AMAP = 0 */
  }
#  endif  /* SIO_ENABLE_SIO_UART7 */
# endif
}

#endif /* SIO_ECUTYPE_MASTER */


#if !defined( SIO_ENABLE_POLLINGMODE )
/************************************************************************
* NAME:            SioInterruptDisable
* PARAMETER:       siochannel channel
* RETURN VALUE:    void
* DESCRIPTION:     disable receive (and error) interrupt of SCI
*************************************************************************/
void SIO_API_CALL_TYPE SioInterruptDisable( SIO_CHANNEL_TYPE_ONLY )
{
/* Disable global interrupts before SIO interrupt is disabled.           */
/* When bIflag is used, global interrupts are disabled                   */

# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
    tpa
    sei
    staa  bIflag ; disable global interrupts
  }
# endif
# if defined( V_COMP_FREESCALE_MCS12Z )
  __asm 
  {
    tfr ccl, d0
    sei
    st d0, bIflag ; disable global interrupts
  }
# endif
# if defined( V_COMP_COSMIC_MCS12Z )
  bInterruptOldState = (vuint8) _asm("tfr ccl,d0");
  _asm("sei");
# endif

# if defined( V_COMP_IAR )
  disableint(&bIflag);
# endif

# if defined( SIO_ENABLE_SIO_UART0 )
  {
    bSioInterruptDisableCount_0++;
    /* disable rx interrupt */
    sSciModule_0.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif
# if defined( SIO_ENABLE_SIO_UART1 )
  {
    bSioInterruptDisableCount_1++;
    /* disable rx interrupt */
    sSciModule_1.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif
# if defined( SIO_ENABLE_SIO_UART2 )
  {
    bSioInterruptDisableCount_2++;
    /* disable rx interrupt */
    sSciModule_2.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif
# if defined( SIO_ENABLE_SIO_UART3 )
  {
    bSioInterruptDisableCount_3++;
    /* disable rx interrupt */
    sSciModule_3.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif
# if defined( SIO_ENABLE_SIO_UART4 )
  {
    bSioInterruptDisableCount_4++;
    /* disable rx interrupt */
    sSciModule_4.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif
# if defined( SIO_ENABLE_SIO_UART5 )
  {
    bSioInterruptDisableCount_5++;
    /* disable rx interrupt */
    sSciModule_5.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif
# if defined( SIO_ENABLE_SIO_UART6 )
  {
    bSioInterruptDisableCount_6++;
    /* disable rx interrupt */
    sSciModule_6.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif
# if defined( SIO_ENABLE_SIO_UART7 )
  {
    bSioInterruptDisableCount_7++;
    /* disable rx interrupt */
    sSciModule_7.SCCR2 &= (vuint8)(~ SCI_RIE);
  }
# endif

# if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
  asm {
    ldaa  bIflag ; restore global interrupts
    tap
  }
# endif
# if defined( V_COMP_FREESCALE_MCS12Z )
  if ((bIflag & 0x10U) != 0x10U)
  {
    __asm { cli }
  }
# endif
# if defined( V_COMP_COSMIC_MCS12Z )
  _asm("tfr d0,ccl", bInterruptOldState);
# endif
# if defined( V_COMP_IAR )
  restoreint(bIflag);
# endif
}


/************************************************************************
* NAME:            SioInterruptRestore
* PARAMETER:       siochannel channel
* RETURN VALUE:    void
* DESCRIPTION:     restore receive (and error) interrupt of SCI
*************************************************************************/
void SIO_API_CALL_TYPE SioInterruptRestore( SIO_CHANNEL_TYPE_ONLY )
{
# if defined( SIO_ENABLE_SIO_UART0 )
  {
    if ( bSioInterruptDisableCount_0 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_0--;
    }
    if ( bSioInterruptDisableCount_0 == 0 )
    {
      sSciModule_0.SCCR2 |= SCI_RIE;
    }
  }
# endif
# if defined( SIO_ENABLE_SIO_UART1 )
  {
    if ( bSioInterruptDisableCount_1 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_1--;
    }
    if ( bSioInterruptDisableCount_1 == 0 )
    {
      sSciModule_1.SCCR2 |= SCI_RIE;
    }
  }
# endif
# if defined( SIO_ENABLE_SIO_UART2 )
  {
    if ( bSioInterruptDisableCount_2 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_2--;
    }
    if ( bSioInterruptDisableCount_2 == 0 )
    {
      sSciModule_2.SCCR2 |= SCI_RIE;
    }
  }
# endif
# if defined( SIO_ENABLE_SIO_UART3 )
  {
    if ( bSioInterruptDisableCount_3 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_3--;
    }
    if ( bSioInterruptDisableCount_3 == 0 )
    {
      sSciModule_3.SCCR2 |= SCI_RIE;
    }
  }
# endif
# if defined( SIO_ENABLE_SIO_UART4 )
  {
    if ( bSioInterruptDisableCount_4 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_4--;
    }
    if ( bSioInterruptDisableCount_4 == 0 )
    {
      sSciModule_4.SCCR2 |= SCI_RIE;
    }
  }
# endif
# if defined( SIO_ENABLE_SIO_UART5 )
  {
    if ( bSioInterruptDisableCount_5 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_5--;
    }
    if ( bSioInterruptDisableCount_5 == 0 )
    {
      sSciModule_5.SCCR2 |= SCI_RIE;
    }
  }
# endif
# if defined( SIO_ENABLE_SIO_UART6 )
  {
    if ( bSioInterruptDisableCount_6 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_6--;
    }
    if ( bSioInterruptDisableCount_6 == 0 )
    {
      sSciModule_6.SCCR2 |= SCI_RIE;
    }
  }
# endif
# if defined( SIO_ENABLE_SIO_UART7 )
  {
    if ( bSioInterruptDisableCount_7 != 0 )
    {
      /* decrement counter when it has been incremented before */
      bSioInterruptDisableCount_7--;
    }
    if ( bSioInterruptDisableCount_7 == 0 )
    {
      sSciModule_7.SCCR2 |= SCI_RIE;
    }
  }
# endif
}
#endif /* SIO_ENABLE_POLLINGMODE */


/* END OF SIO_DRV.C */

