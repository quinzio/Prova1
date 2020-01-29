/* -----------------------------------------------------------------------------
  Filename:    v_par.c
  Description: Toolversion: 00.00.00.01.40.03.60.01.00.00
               
               Serial Number: CBD1400360
               Customer Info: SPAL Automotive srl
                              Package: LIN_SLP2
                              Micro: S9S12GN32F1MLC
                              Compiler: Metrowerks 5.1
               
               
               Generator Fwk   : GENy 
               Generator Module: GenTool_GenyVcfgNameDecorator
               
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

/* -----------------------------------------------------------------------------
    &&&~ BaseEnv_PSF_Includes
 ----------------------------------------------------------------------------- */

#include "v_inc.h"


/* -----------------------------------------------------------------------------
    &&&~ GENy Version Information
 ----------------------------------------------------------------------------- */

/* ROM CATEGORY 4 START */
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 kGENyVersion[kGENyVersionNumberOfBytes] = 
{
  VGEN_DELIVERY_VERSION_BYTE_0, 
  VGEN_DELIVERY_VERSION_BYTE_1, 
  VGEN_DELIVERY_VERSION_BYTE_2, 
  VGEN_DELIVERY_VERSION_BYTE_3, 
  VGEN_DELIVERY_VERSION_BYTE_4, 
  VGEN_DELIVERY_VERSION_BYTE_5, 
  VGEN_DELIVERY_VERSION_BYTE_6, 
  VGEN_DELIVERY_VERSION_BYTE_7, 
  VGEN_DELIVERY_VERSION_BYTE_8, 
  VGEN_DELIVERY_VERSION_BYTE_9
};
/* ROM CATEGORY 4 END */




/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 259943842
      #error "The magic number of the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\v_par.c> is different. Please check time and date of generated files!"
  #endif
#else
  #error "The magic number is not defined in the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\v_par.c> "

#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

