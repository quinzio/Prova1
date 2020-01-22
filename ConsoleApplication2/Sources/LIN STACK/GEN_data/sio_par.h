/* -----------------------------------------------------------------------------
  Filename:    sio_par.h
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

#if !defined(__SIO_PAR_H__)
#define __SIO_PAR_H__

#define kSioBaudrate                         19200
#define kSioCalcBaudrate                     19230

#define kSioSynchBreakByte                   0x00

#define kLinFrequency                        20000
#define kChannel_UART0                       0
#define kSioTIMPrescalerShiftValue           2
#define kSioSBPulseMinLength                 0x0B31
#define kSioPulseNominalLength               0x0209
#define kSioPulseMinLength                   0x01BB
#define kSioPulseMaxLength                   0x0257
#define kLinRegHc12ScH_Baudrate              0x00
#define kLinRegHc12ScL_Baudrate              0x41
#define kTimerChannel                        3


/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 259943842
      #error "The magic number of the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\sio_par.h> is different. Please check time and date of generated files!"
  #endif
#else
  #define MAGIC_NUMBER 259943842
#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

#endif /* __SIO_PAR_H__ */
