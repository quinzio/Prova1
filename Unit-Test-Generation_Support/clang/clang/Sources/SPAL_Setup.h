/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SPAL_Setup.h

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file SPAL_Setup.h
    \version see \ref FMW_VERSION 
    \brief HL Definition for define
    \details This file is like a box for all HL definitions.\n
    \note none
    \warning none
*/

#ifndef SPALSETUP_H
#define SPALSETUP_H


/*----------------------------------------------------------------------------*/
/* Define here files to use basing on micro choosen */

#define FREESCALE_MC9S12GN32

#ifdef __HCS12__
  #ifdef FREESCALE_MC9S12GN16
    #define MICRO_REGISTERS   <MC9S12GN16.h>
  #endif /* FREESCALE_MC9S12GN16 */

  #ifdef FREESCALE_MC9S12GN32
  #define MICRO_REGISTERS   <MC9S12GN32.h>
  #endif /* FREESCALE_MC9S12GN32 */

  #define INI0_INIT         "INI0_InitS12.h"
  #define ADM0_ADC          "ADM0_ADCS12.h"
  #define INT0_PWMIsr       "INT0_PWMIsrS12.h"
  #define INT0_DIAGIsr      "INT0_DIAGIsrS12.h"
  #define INT0_TachoIsr     "INT0_TachoIsrS12.h"
  #define MAC0_Register     "MAC0_RegisterS12.h"
#else
  #error "Specify micro type"
#endif  /* __HCS12__ */


/*----------------------------------------------------------------------------*/
/* Define here files to use basing battery voltage application */
#define V_BATT_12V 1
#define V_BATT_24V 2

#define BATTERY_VOLTAGE V_BATT_12V

#define SPAL_DEF "SPAL_def.h"

/*----------------------------------------------------------------------------*/
/* Watchdog */
/* Define here if watchdog is windowed or not */
/* If WINDOWED_WATCHDOG is defined window watchdog is enabled                                                       */
/* If WINDOWED_WATCHDOG is undefined watchdog is enabled in normal mode (must be refreshed anytime before timeout)  */

/* #define WINDOWED_WATCHDOG */

/* Define here window watchdog prescaler and its refresh period */
/*  WTD_NUM       Watchdog Timeout                              */
/*      0           disabled                                    */
/*      1           2^7/10KHz   -> 12,8 msec                    */
/*      2           2^9/10KHz   -> 51,2 msec                    */
/*      3           2^11/10KHz  -> 204,8 msec                   */
/*      4           2^13/10KHz  -> 819,2 msec                   */
/*      5           2^15/10KHz  -> 3,27 sec                     */
/*      6           2^16/10KHz  -> 6,55 sec                     */
/*      7           2^17/10KHz  -> 13,1 sec                     */
/*      >7          error                                       */

#ifdef WINDOWED_WATCHDOG
#define WTD_NUM 1u
#endif  /* WINDOWED_WATCHDOG */

/* Remove WTD_TEST for the production fw release */
/* #define WTD_TEST */      /* For watchdog test */
/* #define DBG_WATCHDOG */

/* Remove RST_MON for the production fw release  */
/* #define RST_MON */       /* For reset monitor */

/*----------------------------------------------------------------------------*/
/* BOOTLOADER VERSION*/
#define BOOTLOADER_LESS_14  1
#define BOOTLOADER_LATER_15 2
#define BOOTLOADER_LATER_20 3

#define BOOTLOAD_VERS BOOTLOADER_LATER_20   /* L3 */

/*--------------------------------------------------------------------------------------*/
/*Enable DEBUG mode in order to use the microcontroller without Bootloader              */
/* The debug mode insert the reset routine address into the location FFFA,FFFC and FFFE */
/* ...to manage reset condition                                                         */

/* Remove DEBUG_MODE definition when you nedd to load s19 with bootloader */

#if (BOOTLOAD_VERS == BOOTLOADER_LATER_20)

/* #define DEBUG_MODE */ /* Scommentare per eseguire codice senza bootloader (debug tramite BDM) */

#define CHECKSUM_SPAL_ENABLED   /* Nel Bootloader 20 (32K flash) il checksum deve essere eseguito dall'applicativo */

#endif

/*----------------------------------------------------------------------------*/
/*Define here Bootloader reprogramming protection */
/* BOOTLOADER_LOCK   --> Bootloader cannot reprogram FLASH MEMORY */
/* BOOTLOADER_UNLOCK --> Bootloader can reprogram FLASH MEMORY    */
#if ((BOOTLOAD_VERS == BOOTLOADER_LATER_15) || (BOOTLOAD_VERS == BOOTLOADER_LATER_20))

  /*#define BOOTLOADER_LOCK*/
  #define BOOTLOADER_UNLOCK

#endif /*BOOTLOAD_VERS*/

/*----------------------------------------------------------------------------*/
/*Define here used PCB*/
#define  PCB211A      1 
#define  PCB211A_HY   2 
#define  PCB211B      3 
#define  PCB213A      4 
#define  PCB213B      5 
#define  PCB213C      6
#define  PCB213D      7 
#define  PCB220A      8
#define  PCB220B      9
#define  PCB220C      10
#define  PCB221A      11  /*BL10-25*/
#define  PCB221B      12  /*BL10-25*/
#define  PCB224A      13  /*BL10-25*/
#define  PCB224B      14  /*BL10-25*/
#define  PCB224C      15  /*BL10-25*/
#define  PCB224D      16  /*BL10-25*/
#define  PCB225A      17
#define  PCB225B      18
#define  PCB225C      19
#define  PCB228A      20
#define  PCB228B      21
#define  PCB230A      22  /*SBL40*/
#define  PCB233C      23
#define  PCB237E      24  /*1000HT*/
#define  PCB240A      25  /*SBL40 (24 V)*/
#define  PCB236B      26
#define  PCB250A      27
#define  PCB262A      28
#define  PCB258A      29

#define PCB PCB262A

#ifdef __HCS12__
  #if ( ( PCB == PCB211A ) || ( PCB == PCB211B ) )
    #error "PCB211A and PCB211B hardware don't fit S12 micro"
  #endif  /* PCB */
#endif  /* __HCS12__ */

/*-----------------------------------------------------------------------------*/
/*Define here used Fan*/
#define  BLOWER 0
#define  AXIAL 1

#define FAN_TYPE AXIAL

/*-----------------------------------------------------------------------------*/
/* Define DC like behaviour*/
#if ( FAN_TYPE == BLOWER )
  #define DC_LIKE_BEHAVIOUR
#endif

/*-----------------------------------------------------------------------------*/
/* Define Transfer Function re-scaling for blower application                  */
/*#define BLOWER_FDT                                                           */

/*-----------------------------------------------------------------------------*/
/* Run on Fly because of overcurrent                                           */
/*#define OVERCURRENT_ROF                                                      */

/*-----------------------------------------------------------------------------*/
/* Define RVP protection diode on VDH line                                     */
/*#define VDH_RVP_DIODE                                                        */

/*-----------------------------------------------------------------------------*/
/* Define for management of RVP MOS                                            */
#define RVP_SW_MANAGEMENT_ENABLE

/*-----------------------------------------------------------------------------*/
/* Define for low-pass filtering on Vmon checks                                */
/*#define VBATT_FILTERING                                                      */

/*-----------------------------------------------------------------------------*/
/* Define ground-balancing procedure                                           */
#define GROUND_BALANCE

/*-----------------------------------------------------------------------------*/
/*Use this define to enable battery degradation feature                        */
#define BATTERY_DEGRADATION

/*-----------------------------------------------------------------------------*/
/*Use this define to enable battery degradation feature                        */
#define PULSE_ERASING

/*-----------------------------------------------------------------------------*/
/*Use this define to enable key-short circuit detection feature                */
#define KEY_SHORT_LOGIC

/*-----------------------------------------------------------------------------*/
/* Define reverse-speed feature                                                */
/*#define RVS                                                                  */

/*-----------------------------------------------------------------------------*/
/* Define ZC detection on differential line                                    */
#define DUAL_LINE_ZERO_CROSSING

/*-----------------------------------------------------------------------------*/
/*Use this define to avoid the demagnetization at low temperature              */
/*The brake occurs at RPM_BRAKE_MIN under RVS_TEMPERATURE_TRESHOLD             */
#ifdef RVS
  #define REDUCED_SPEED_BRAKE_VS_TEMPERATURE
#endif  /* RVS */

/*-----------------------------------------------------------------------------*/
/*Define here rotation direction                                               */
#define CW 1
#define CCW 2

#define ROTATION CW

/*-----------------------------------------------------------------------------*/
/* Define over modulation behaviour                                            */
#define OVER_MODULATION

/*-----------------------------------------------------------------------------*/
/* Define VoF function type                                                    */
#define SIGMOIDAL_VoF

/*-----------------------------------------------------------------------------*/
/* Define Wind-mill management procedure                                       */
#if ( FAN_TYPE == AXIAL )
  #define WM_MANAGEMENT
#endif  /* FAN_TYPE */

/*-----------------------------------------------------------------------------*/
/*Define here diagnostic                                                       */
#define DIAGNOSTIC_DISABLE  0
#define DIAGNOSTIC_ENABLE   1

/*-----------------------------------------------------------------------------*/
/*Use this define to enable speed feedback during RUN state                    */
/*#define SPEED_FEEDBACK_ENABLE                                                */

/*-----------------------------------------------------------------------------*/
/*Use this define to enable mosfet overtemperature protection                  */
#define MOSFET_OVERTEMPERATURE_PROTECTION
#ifdef MOSFET_OVERTEMPERATURE_PROTECTION  /*define sensor*/
/*#define MOSFET_OVERTEMP_BAV99*/         /*Diode*/
#define MOSFET_OVERTEMP_NCP18XH103F       /*Murata NTC*/
#endif

/*-----------------------------------------------------------------------------*/
/* Define for applications with LIN transceiver onboard                        */
/*#define LIN_TRANSCEIVER                                                      */

/*-------------------------------------------------------------------------------*/
/* Define here memory segments (RAM, D-Flash, non-paged FLASHs) used by .prm file*/
#ifdef _CANTATA_
#define RAM1_START          &dummymem[0x3800]
#else
#define RAM1_START          0x3800
#endif /* _CANTATA_ */
#define RAM1_END            0x3BFF
#define RAM1_FILL           0x00

#define Z_RAM_START         0x3C00
#define Z_RAM_END           0x3DFF
#define Z_RAM_FILL          0x00

#define RAM2_START          0x3E00
#define RAM2_END            0x3E01
#define RAM2_FILL           0x00

#define PI_RAM_START        0x3E02
#define PI_RAM_END          0x3FA0
#define PI_RAM_FILL         0x00

#define FILT_RAM_START      0x3FA1
#define FILT_RAM_END        0x3FFF
#define FILT_RAM_FILL       0x00

#define DFLASH_START        0x000400
#define DFLASH_END          0x0007FF

#define ROM_A000_START      0xA000
#define ROM_A000_END        0xBFFF
#define ROM_A000_FILL       0x3F    /* 0x3F is a SWI instruction */

#define ROM_C000_START      0xC000
#define ROM_C000_END        0xEEFF
#define ROM_C000_FILL       0x3F    /* 0x3F is a SWI instruction */

#define PROD_PAR_ROM_START  0xEF00
#define PROD_PAR_ROM_END    0xEF7F

#define UNUSED_1_START      0xEF0B
#define UNUSED_1_END        0xEF7F
#define UNUSED_1_FILL       0x3F    /* 0x3F is a SWI instruction */

#define UNUSED_2_START      0xF000
#define UNUSED_2_END        0xFEFF
#define UNUSED_2_FILL       0x3F    /* 0x3F is a SWI instruction */

/* Beware that from 0xFF00 to 0xFF0F there are microcontroller security options */

#define UNUSED_3_START      0xFF10
#define UNUSED_3_END        0xFF9E
#define UNUSED_3_FILL       0x3F    /* 0x3F is a SWI instruction */

#if ((BOOTLOAD_VERS == BOOTLOADER_LATER_20) && defined(DEBUG_MODE))
#define UNUSED_4_START      0xFFA0
#define UNUSED_4_END        0xFFF9
#define UNUSED_4_FILL       0x3F    /* 0x3F is a SWI instruction */
#else
#define UNUSED_4_START      0xFFA0
#define UNUSED_4_END        0xFFFF
#define UNUSED_4_FILL       0x3F    /* 0x3F is a SWI instruction */
#endif

#define STACK_SIZE          0x0100
#define STACK_START         RAM1_START
#define STACK_END           STACK_START+STACK_SIZE

/*----------------------------------------------------------------------------*/
/* Define here if external oscillator is not present */
#define NO_EXTERNAL_OSC

/*----------------------------------------------------------------------------*/
/* Redirection Table per parametri in EEPROM */
#define REDIR_TABLE

/*----------------------------------------------------------------------------*/
/* Abilita log dati In Eeprom */
#define LOG_DATI

/*----------------------------------------------------------------------------*/
/* Abilita salvataggio periodico in Eeprom (periodo di salvataggio di default: 30') */
#define LOG_PERIODICSAVE_ENABLE

/*----------------------------------------------------------------------------*/
/* Define here if you want to use the SCI interface to monitor some internal  */
/* variables during run time                                                  */
/* IMPORTANT NOTE: As communication is done by polling, this will decrease    */
/* the sampling rate of the PI Speed controller                               */
/*  #define ENABLE_RS232_DEBUG                                                */
/*  #define RS232_PHASE                                                       */
/*  #define RS232_SPEED                                                       */
/*  #define RS232_VOLTAGE                                                     */
/*  #define RS232_BUS_VOLTAGE                                                 */
/*  #define RS232_ERROR_TYPE                                                  */
/*  #define RS232_EXTVAR                                                      */
/*  #define RS232_CURRENT                                                     */
/*  #define RS232_OFFSET_CURRENT                                              */
/*  #define RS232_TAMB                                                        */
/*  #define RS232_TMOS                                                        */

/*----------------------------------------------------------------------------*/
/* Select the mode of communication for the SCI*/
#define SCI_POLLING_TX		/*Polling mode transmission*/
#define SCI_POLLING_RX		/*polling mode reception*/
/*#define SCI_ITDRV_WITHOUTBUF_TX */	/*Interrupt driven without buffer transmission mode*/
/*#define SCI_ITDRV_WITHOUTBUF_RX */	/*Interrupt driven without buffer reception mode*/

/*----------------------------------------------------------------------------*/
/*Define here used Fan*/
#define  INTERFACE_TYPE_0  0
#define  INTERFACE_TYPE_1  1
#define  INTERFACE_TYPE_2  2
#define  INTERFACE_TYPE_3  3
#define  INTERFACE_TYPE_4  4
#define  INTERFACE_TYPE_5  5
#define  INTERFACE_TYPE_6  6
#define  INTERFACE_TYPE_7  7
#define  INTERFACE_TYPE_8  8
#define  INTERFACE_TYPE_9  9
#define  INTERFACE_TYPE_10 10
#define  INTERFACE_TYPE_11 11
#define  INTERFACE_TYPE_12 12
#define  INTERFACE_TYPE_13 13
#define  INTERFACE_TYPE_14 14
#define  INTERFACE_TYPE_15 15
#define  INTERFACE_TYPE_16 16
#define  INTERFACE_TYPE_17 17
#define  INTERFACE_TYPE_18 18
#define  INTERFACE_TYPE_19 19
#define  INTERFACE_TYPE_20 20
#define  INTERFACE_TYPE_21 21
#define  INTERFACE_TYPE_22 22
#define  INTERFACE_TYPE_30 30

#define INTERFACE_TYPE INTERFACE_TYPE_1

#if ( INTERFACE_TYPE == INTERFACE_TYPE_1 )
  #define KEY_INPUT
  #define PWM_INPUT
  #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
  #define DIAGNOSTIC_SHORT_SW_PROTECTION
  #define DIAGNOSTIC_SHORT_DIGITAL_PROTECTION         /*Short circuit detected by digital pin uP (NEGATIVE LOGIC)*/
  #define PWM_IN_POSITIVE_LOGIC
  #define SLEEP_HIGH_LEVEL
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_2 )
  #define KEY_INPUT
  #define PWM_INPUT
  #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
  #define DIAGNOSTIC_SHORT_SW_PROTECTION
  #define DIAGNOSTIC_SHORT_DIGITAL_PROTECTION         /*Short circuit detected by digital pin uP (NEGATIVE LOGIC)*/
  #define PWM_IN_NEGATIVE_LOGIC
  #define SLEEP_HIGH_LEVEL
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_3 )
  #define PWM_INPUT
  #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
  #define DIAGNOSTIC_STAND_ALONE
  #define DIAGNOSTIC_SHORT_SW_PROTECTION
  #define DIAGNOSTIC_SHORT_DIGITAL_PROTECTION         /*Short circuit detected by digital pin uP (NEGATIVE LOGIC)*/
  #define PWM_IN_NEGATIVE_LOGIC
  #define SLEEP_HIGH_LEVEL
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_4 )
  /*#define KEY_INPUT*/
  #define PWM_INPUT
  #define AN_INPUT
  #define POTENTIOMETER
  #define DIAGNOSTIC_STATE DIAGNOSTIC_DISABLE
  #define PWM_IN_POSITIVE_LOGIC
  #define SLEEP_HIGH_LEVEL
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_5 )
  #define KEY_INPUT
  #define AN_INPUT
  #define NTC
  #define DIAGNOSTIC_STATE DIAGNOSTIC_DISABLE
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_6 )
  #define NO_INPUT
  #define DIAGNOSTIC_STATE DIAGNOSTIC_DISABLE
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_7 )
  #define AN_INPUT
  #define NTC
  #define DIAGNOSTIC_STATE DIAGNOSTIC_DISABLE
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_8 )
  #define AN_INPUT
  #define POTENTIOMETER
  #define DIAGNOSTIC_STATE DIAGNOSTIC_DISABLE
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_9 )
   #define AN_INPUT
   #define PWM_TO_ANALOG_IN
   #define PWM_IN_POSITIVE_LOGIC
   #define DIAGNOSTIC_STATE DIAGNOSTIC_DISABLE
   #define SLEEP_HIGH_LEVEL
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_10 )       /* INTERFACCIA 4g */
   #define AN_INPUT
   #define PWM_TO_ANALOG_IN
   #define PWM_IN_POSITIVE_LOGIC
   #define SLEEP_LOW_LEVEL
   #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
   #define DIAGNOSTIC_STAND_ALONE
   #define DIAGNOSTIC_SHORT_SW_PROTECTION
   #define DIAGNOSTIC_SHORT_ANALOG_PROTECTION
   #define POSITIVE_ANALOG_DIAGNOSTIC_LOGIC         /* Short circuit detected by analog difference between Vbatt and Diag output (POSITIVE LOGIC) */
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_11 )       /* INTERFACCIA 5m */
   #define AN_INPUT
   #define PWM_TO_ANALOG_IN
   #define PWM_IN_POSITIVE_LOGIC                    /* For test mode with digital PWM 12Hz */
   #define ECONOMY_INPUT 
   #define SLEEP_LOW_LEVEL                          /* For test mode with digital PWM 12Hz */
   #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
   #define DIAGNOSTIC_STAND_ALONE
   #define DIAGNOSTIC_SHORT_SW_PROTECTION
   #define DIAGNOSTIC_SHORT_ANALOG_PROTECTION
   #define POSITIVE_ANALOG_DIAGNOSTIC_LOGIC         /* Short circuit detected by analog difference between Vbatt and Diag output (POSITIVE LOGIC) */
   #define DIAGNOSTIC_ONLY_IN_FAULT  
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_12 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_13 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_14 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_15 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_16 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_17 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_18 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_19 )
  error("Interface not defined ! ")
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_20 )
   #define PWM_INPUT
   #define PWM_IN_POSITIVE_LOGIC
   #define SLEEP_LOW_LEVEL
   #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
   #define DIAGNOSTIC_STAND_ALONE
   #define DIAGNOSTIC_SHORT_SW_PROTECTION
   #define DIAGNOSTIC_SHORT_ANALOG_PROTECTION
   #define POSITIVE_ANALOG_DIAGNOSTIC_LOGIC
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_21 )
   #define PWM_INPUT
   #define AN_INPUT
   #define PWM_TO_ANALOG_IN
   #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
   #define DIAGNOSTIC_STAND_ALONE
   #define DIAGNOSTIC_SHORT_SW_PROTECTION
   #define DIAGNOSTIC_SHORT_ANALOG_PROTECTION
   #define POSITIVE_ANALOG_DIAGNOSTIC_LOGIC         /* Short circuit detected by analog difference between Vbatt and Diag output (POSITIVE LOGIC) */
   #define PWM_IN_POSITIVE_LOGIC
   #define SLEEP_HIGH_LEVEL
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_22 )
  #define KEY_INPUT
  #define PWM_INPUT
  #define DIAGNOSTIC_STATE DIAGNOSTIC_ENABLE
  #define DIAGNOSTIC_STAND_ALONE
  #define DIAGNOSTIC_SHORT_SW_PROTECTION
  #define DIAGNOSTIC_SHORT_ANALOG_PROTECTION
  #define POSITIVE_ANALOG_DIAGNOSTIC_LOGIC
  #define DIAGNOSTIC_ONLY_IN_FAULT 
  #define PWM_IN_POSITIVE_LOGIC
  #define SLEEP_HIGH_LEVEL   
#elif ( INTERFACE_TYPE == INTERFACE_TYPE_30 )
  #define LIN_INPUT
  #define BMW_LIN_ENCODING
  /*#define GM_LIN_ENCODING*/
  #define LIN_BAL_MODE_PASSWORD   
  #define DIAGNOSTIC_STATE DIAGNOSTIC_DISABLE
  #define PWM_IN_NEGATIVE_LOGIC
  #define SLEEP_HIGH_LEVEL   
#endif  /* INTERFACE_TYPE */


/*-----------------------------------------------------------------------------*/
/* Set SW_VERSION according to the rule:                                       */
/* - 0x0X --> Production version                                               */
/* - 0x8X --> Sample version                                                   */

#define FMW_DATE      "15/11/19"      /* DD/MM/YYYY */
#define FMW_VERSION   "30.02.81"      /* MAX 255.255.255 */

#define CUSTOMER         0x30
#define PROJECT          0x02
#define SW_VERSION       0x81
#define SW_SUBVERSION    0x16

#define SBL1000HT                 (u8)18
#define APPLICATION               SBL1000HT

#define FAN_131A                  (u8)18
#define FAN_MODEL                 FAN_131A

#define SHUNT_RESISTOR_PER_10000_FLOAT    (float32_t)((float32_t)SHUNT_RESISTOR*(float32_t)10000)
#define SHUNT_RESISTOR_PER_10000_INT      (u16)((float32_t)SHUNT_RESISTOR*(float32_t)10000)
#define SHUNT_RESISTOR_PER_10000          ( ( SHUNT_RESISTOR_PER_10000_FLOAT > (float32_t)SHUNT_RESISTOR_PER_10000_INT ) ? (u16)(SHUNT_RESISTOR_PER_10000_INT+(u16)1) : SHUNT_RESISTOR_PER_10000_INT )

#define PEAK_DETECTOR_GAIN_PER_10 (u16)(OP_AMP_GAIN*(float32_t)10)

#ifdef BMW_LIN_ENCODING
  #define BMW_SW_VERSION_LSB     0x08
  #define BMW_SW_VERSION_MSB     0x00

  #define DESIGN_VOLTAGE         0x12   /* 12 V                       */
  #define DESING_POWER           0x20   /* 1000 W                     */
  #define MANUFACTURER           0x00   /* 0 = SPAL                   */
  #define FABRICATION_SITE       0x00   /* 0 = Correggio              */
  #define PRODUCT_FAMILY         0x02   /* 0 = BL; 1 = SBL; 2 = SBLHT */
#endif /* BMW_LIN_ENCODING */

#endif /* SPALSETUP_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
