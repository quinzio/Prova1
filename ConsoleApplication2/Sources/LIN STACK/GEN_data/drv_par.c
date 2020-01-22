/* -----------------------------------------------------------------------------
  Filename:    drv_par.c
  Description: Toolversion: 00.00.00.01.40.03.60.01.00.00
               
               Serial Number: CBD1400360
               Customer Info: SPAL Automotive srl
                              Package: LIN_SLP2
                              Micro: S9S12GN32F1MLC
                              Compiler: Metrowerks 5.1
               
               
               Generator Fwk   : GENy 
               Generator Module: GenTool_GenyDriverBase
               
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

#include "v_inc.h"
#include "drv_par.h"
/* -----------------------------------------------------------------------------
    &&&~ Message Buffers
 ----------------------------------------------------------------------------- */

/* RAM CATEGORY 2 START */
/* PRQA  S 0759 QAC_Can_0759 */ /* MD_Can_0759 */
V_MEMRAM0 V_MEMRAM1 _c_CTR_FAN_LIN__MLIN_4_buf V_MEMRAM2 CTR_FAN_LIN__MLIN_4;
V_MEMRAM0 V_MEMRAM1 _c_ST_FAN_1_LIN__MLIN_4_buf V_MEMRAM2 ST_FAN_1_LIN__MLIN_4;
V_MEMRAM0 V_MEMRAM1 _c_MasterReq__MLIN_4_buf V_MEMRAM2 MasterReq__MLIN_4;
V_MEMRAM0 V_MEMRAM1 _c_SlaveResp__MLIN_4_buf V_MEMRAM2 SlaveResp__MLIN_4;
/* PRQA  L:QAC_Can_0759 */

/* RAM CATEGORY 2 END */





/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 259943842
      #error "The magic number of the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\drv_par.c> is different. Please check time and date of generated files!"
  #endif
#else
  #error "The magic number is not defined in the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\drv_par.c> "

#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

