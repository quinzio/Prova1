/* -----------------------------------------------------------------------------
  Filename:    sio_cfg.h
  Description: Toolversion: 00.00.00.01.40.03.60.01.00.00
               
               Serial Number: CBD1400360
               Customer Info: SPAL Automotive srl
                              Package: LIN_SLP2
                              Micro: S9S12GN32F1MLC
                              Compiler: Metrowerks 5.1
               
               
               Generator Fwk   : GENy 
               Generator Module: DrvLin__baseSci
               
               Configuration   : C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Geny\SPAL_FAN_6_Pick_Ammar.gny
               
               ECU: 
                       TargetSystem: Hw_Mcs12xCpu
                       Compiler:     Metrowerks
                       Derivates:    MCS12G
               
               Channel "SPAL_LIN":
                       Databasefile: C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Database\ldf.ldf
                       Bussystem:    LIN
                       Node:         FAN_1

 ----------------------------------------------------------------------------- */
/* -----------------------------------------------------------------------------
  C O P Y R I G H T
 -------------------------------------------------------------------------------
  Copyright (c) 2001-2014 by Vector Informatik GmbH. All rights reserved.
 
  This software is copyright protected and proprietary to Vector Informatik 
  GmbH.
  
  Vector Informatik GmbH grants to you only those rights as set out in the 
  license conditions.
  
  All other rights remain with Vector Informatik GmbH.
 -------------------------------------------------------------------------------
 ----------------------------------------------------------------------------- */

#if !defined(__SIO_CFG_H__)
#define __SIO_CFG_H__

/* Version defines */
#define DRVLIN__BASESCIDLL_VERSION           0x0210u
#define DRVLIN__BASESCIDLL_RELEASE_VERSION   0x03u


/* Hardware dependent configuration */
#define SIO_APITYPE_SINGLE
#define SIO_LL_APISTYLE_BYTE
#define SIO_LL_APITYPE_FE_IND
#define SIO_LL_DISABLE_BAUDRATE_INFORMATION
#define SIO_ECUTYPE_SLAVE
#define SIO_DISABLE_OSEK_OS
#define SIO_PROTOCOLVERSION_VALUE            0x21
#define SIO_DISABLE_CLOCK_DEV_INDICATION
#define SIO_LL_DISABLE_COMPATIBILITY_MODE
#define SIO_ENABLE_SIO_UART0
#define SIO_DISABLE_SIO_UART1
#define SIO_DISABLE_SIO_UART2
#define SIO_DISABLE_SIO_UART3
#define SIO_DISABLE_SIO_UART4
#define SIO_DISABLE_SIO_UART5
#define SIO_DISABLE_SIO_UART6
#define SIO_DISABLE_SIO_UART7
#define SIO_INTERFACE_SCISTD
#define SIO_ENABLE_NO_ISR_FUNCTION
#define SIO_ENABLE_BAUDRATE_SYNC
#define SIO_DISABLE_BAUDRATE_DETECT
#define SIO_DISABLE_POLLINGMODE
#define SIO_DISABLE_IRQ_NESTING


/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 259943842
      #error "The magic number of the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\sio_cfg.h> is different. Please check time and date of generated files!"
  #endif
#else
  #define MAGIC_NUMBER 259943842
#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

#endif /* __SIO_CFG_H__ */
