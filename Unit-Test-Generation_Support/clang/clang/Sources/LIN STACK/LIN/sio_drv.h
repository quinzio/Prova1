/*****************************************************************************
| Project Name: S I O - D R V I V E R
|    File Name: SIO_DRV.H
|
|  Description: Application-Interface of the SIO-Driver
|               Declaration of functions, variables and constants
|
|     Compiler: see module file
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
| ----------   ----   ------  -------------------------------------------------
| 10-Oct-00    1.00   dH      first version
| 08-Nov-00    1.01   Et      adapted SIO driver to changed prototype of ApplSioErrorNotification()
|                             delete wakeup frame functionality
| 16-Apr-01    1.02   Et      used type description as part of variable names
|                             deleted incorrect compatibility defines
|                             moved SioInitPowerOn() to SioInit()
| 14-May-01    1.03   Et      supported both derivative DXX and DPXX
|                             supported both compiler COSMIC and HIWARE
| 11-Jun-01    1.04   Et      EXCAN00000726: extern declaration of SIO driver module version
|                             ESCAN00000775: support OSEK category 2 interrupts
|                             ESCAN00000825: support different derivatives for HW platform HC12
|                             ESCAN00000826: support both UART interface 0 and UART interface 1
| 12-Dec-01    1.05   Et      ESCAN00001940: shortening of stop bit length
| 03-Jul-02    1.06   Wr      changed version number (see sio_drv.c)
| 31-Jul-02    1.07   Wr      ESCAN00004496: add support for K-line driver
| 18-Dec-02    1.08   Wr      ESCAN00004497: Extend API for multichannel support
| 24-Apr-03    1.09   Wr      ESCAN00000773: Support for IAR compiler
|                             ESCAN00005547: Reorganisation of function calls
| 03-Jun-04    2.00   Wr      Adapted to LIN driver 2.x implementation
| 09-Jul-04    2.01   Wr      Removed return value from ApplSioErrorNotification
| 11-Apr-05    2.02   Wr      ESCAN00011933: Added functionality SIO_ENABLE_NO_ISR_FUNCTION
| 24-Oct-05    2.03   Svh     ESCAN00013820: Use new memory qualifier for variables
|                             ESCAN00014035: Change SIO driver from Hc12 to Mcs12x
| 2006-03-01   2.04   Ap      ESCAN00015519: Interrupt status variable type qualifier "static" removed for inline assembly
| 2006-03-08   2.05   Svh     ESCAN00015588: Add support for C_COMP_COSMIC_MCS12X_MSCAN12 and C_COMP_COSMIC_MCS12_MSCAN12 switches
| 2006-06-12   2.06   Ap      ESCAN00016594: Banked memory model support
| 2006-08-15   2.07   dH      ESCAN00017286: increase number of supported UARTs from 2 up to 8 (for C_COMP_COSMIC_MCS12X_MSCAN12)
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
#ifndef  __SIO_DRV_H__
#define  __SIO_DRV_H__

/*****************************************************************************
 * Version: (abcd: main version ab, sub version cd
 *****************************************************************************/

/* ##V_CFG_MANAGEMENT ##CQProject : DrvLin_Mcs12xSciStd CQComponent : Implementation */
# define DRVLIN_MCS12XSCISTD_VERSION            0x0231u
# define DRVLIN_MCS12XSCISTD_RELEASE_VERSION    0x00u

/*****************************************************************************
 * Defines
 *****************************************************************************/


 /* SCI errors signalized by ApplSioErrorNotification() */
#define kSioParityError   0x01u
#define kSioFrameError    0x02u
#define kSioOverrunError  0x03u
#define kSioNoiseError    0x04u


# define SIO_CHANNEL_TYPE_ONLY     void
# define SIO_CHANNEL_TYPE_FIRST
# define SIO_CHANNEL_TYPE_LOCAL
# define SIO_CHANNEL_PARA_ONLY
# define SIO_CHANNEL_PARA_FIRST
# define siochannel                0x00u

/* Type definition for API Functions and ISR to support banked memory model */
#if defined ( V_COMP_FREESCALE_MCS12Z )
# define SIO_API_CALL_TYPE
# define SIO_ISR_CALL_TYPE         interrupt
#endif
#if defined ( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) 
# define SIO_API_CALL_TYPE
# define SIO_ISR_CALL_TYPE         interrupt
#endif
#if defined ( V_COMP_IAR )
# define SIO_API_CALL_TYPE non_banked
# define SIO_ISR_CALL_TYPE interrupt
#endif

#if defined ( V_COMP_IAR )
/* specify interrupt vectors for interrupt service routines */
# if !defined (SIO_INTVECT_UART_0) && defined ( SIO_ENABLE_SIO_UART0 )
#  define SIO_INTVECT_UART_0
# endif
# if !defined (SIO_INTVECT_UART_1) && defined ( SIO_ENABLE_SIO_UART1 )
#  define SIO_INTVECT_UART_1
# endif
# if !defined (SIO_INTVECT_UART_2) && defined ( SIO_ENABLE_SIO_UART2 )
#  define SIO_INTVECT_UART_2
# endif
# if !defined (SIO_INTVECT_UART_3) && defined ( SIO_ENABLE_SIO_UART3 )
#  define SIO_INTVECT_UART_3
# endif
# if !defined (SIO_INTVECT_UART_4) && defined ( SIO_ENABLE_SIO_UART4 )
#  define SIO_INTVECT_UART_4
# endif
# if !defined (SIO_INTVECT_UART_5) && defined ( SIO_ENABLE_SIO_UART5 )
#  define SIO_INTVECT_UART_5
# endif
# if !defined (SIO_INTVECT_UART_6) && defined ( SIO_ENABLE_SIO_UART6 )
#  define SIO_INTVECT_UART_6
# endif
# if !defined (SIO_INTVECT_UART_7) && defined ( SIO_ENABLE_SIO_UART7 )
#  define SIO_INTVECT_UART_7
# endif
#endif


/*****************************************************************************
 * macros
 *****************************************************************************/


/*****************************************************************************
 * error codes
 *****************************************************************************/

/*****************************************************************************
 * compatibility defines
 *****************************************************************************/
extern void SioSetMuteMode(SIO_CHANNEL_TYPE_ONLY);
extern void SioResetMuteMode(SIO_CHANNEL_TYPE_ONLY);

/*****************************************************************************
 * end of compatibility defines
 *****************************************************************************/

/*****************************************************************************
 * data type definitions
 *****************************************************************************/

/*****************************************************************************
 * external declarations
 *****************************************************************************/

/* SIO driver version */
V_MEMROM0 extern V_MEMROM1 vuint8 V_MEMROM2 kSioMainVersion;
V_MEMROM0 extern V_MEMROM1 vuint8 V_MEMROM2 kSioSubVersion;
V_MEMROM0 extern V_MEMROM1 vuint8 V_MEMROM2 kSioReleaseVersion;


/*****************************************************************************
 * callback function prototypes
 *****************************************************************************/
extern void     ApplSioByteIndication       (SIO_CHANNEL_TYPE_FIRST vuint8 value );
extern void     ApplSioErrorNotification    (SIO_CHANNEL_TYPE_FIRST vuint8 error );
#if defined(SIO_LL_ENABLE_BAUDRATE_INFORMATION)
extern void     ApplSioBaudrateInformation  ( vuint8 bBaudrate );
#endif
#if defined ( SIO_ENABLE_CLOCK_DEV_INDICATION )
extern void     ApplSioClockDeviation       (SIO_CHANNEL_TYPE_FIRST vsint16 wDeviation);
#endif

/*****************************************************************************
 * function prototypes
 *****************************************************************************/
extern void SIO_API_CALL_TYPE   SioInit                  ( SIO_CHANNEL_TYPE_ONLY );
extern void SIO_API_CALL_TYPE   SioSendSynchBreakField   ( SIO_CHANNEL_TYPE_ONLY );
extern void SIO_API_CALL_TYPE   SioSendByte              ( SIO_CHANNEL_TYPE_FIRST vuint8 value );
                                
#if defined ( SIO_ENABLE_POLLINGMODE )
# define SioInterruptDisable(c)
# define SioInterruptRestore(c)
#else
extern void SIO_API_CALL_TYPE   SioInterruptDisable      ( SIO_CHANNEL_TYPE_ONLY );
extern void SIO_API_CALL_TYPE   SioInterruptRestore      ( SIO_CHANNEL_TYPE_ONLY );
#endif


#if defined( SIO_ENABLE_BAUDRATE_DETECT ) || defined ( SIO_ENABLE_BAUDRATE_SYNC )
# if defined( SIO_ENABLE_OSEK_OS )
# elif defined( SIO_ENABLE_POLLINGMODE )
  extern void l_ifc_measure ( void );
# elif defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_measure ( void );
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_measure( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_TIM l_ifc_measure( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioTimerInterrupt ( void ); /* non_banked ISR */
#  else
  void SioTimerInterrupt ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART0 )
# define l_ifc_tx_0() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_0 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_0( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_0 l_ifc_rx_0( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_0 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_0 ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART1 )
# define l_ifc_tx_1() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_1 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_1( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_1 l_ifc_rx_1( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_1 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_1 ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART2 )
# define l_ifc_tx_2() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_2 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_2( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_2 l_ifc_rx_2( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_2 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_2 ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART3 )
# define l_ifc_tx_3() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_3 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_3( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_3 l_ifc_rx_3( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_3 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_3 ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART4 )
# define l_ifc_tx_4() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_4 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_4( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_4 l_ifc_rx_4( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_4 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_4 ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART5 )
# define l_ifc_tx_5() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_5 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_5( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_5 l_ifc_rx_5( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_5 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_5 ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART6 )
# define l_ifc_tx_6() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_6 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_6( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_6 l_ifc_rx_6( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_6 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_6 ( void ); /* banked ISR */
#  endif
# endif
#endif

#if defined( SIO_ENABLE_SIO_UART7 )
# define l_ifc_tx_7() /* not used */
# if defined( SIO_ENABLE_POLLINGMODE ) || defined( SIO_ENABLE_NO_ISR_FUNCTION )
  extern void l_ifc_rx_7 ( void );
# elif defined( SIO_ENABLE_OSEK_OS )
# else
#  if defined( C_COMP_MTRWRKS_MCS12X_MSCAN12 ) || \
      defined( V_COMP_FREESCALE_MCS12Z )
/*  non banked section */
#   pragma CODE_SEG __NEAR_SEG NON_BANKED
  extern SIO_ISR_CALL_TYPE void l_ifc_rx_7( void );
#  pragma CODE_SEG DEFAULT
#  endif
#  if defined ( V_COMP_IAR )
  void SIO_ISR_CALL_TYPE SIO_INTVECT_UART_7 l_ifc_rx_7( void );
#  endif
#  if defined( V_COMP_IAR )
  void SIO_API_CALL_TYPE SioInterruptTask_7 ( void ); /* non_banked ISR */
#  else
  void SioInterruptTask_7 ( void ); /* banked ISR */
#  endif
# endif
#endif


#endif /* __SIO_DRV_H__ */
