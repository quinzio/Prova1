/* -----------------------------------------------------------------------------
  Filename:    drv_par.h
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

#if !defined(__DRV_PAR_H__)
#define __DRV_PAR_H__

/* -----------------------------------------------------------------------------
    &&&~ Signal Structures
 ----------------------------------------------------------------------------- */

typedef struct _c_CTR_FAN_LIN__MLIN_4_msgTypeTag
{
  vbittype TARVL_V_FAN_1_LIN__MLIN_4 : 8;
  vbittype TARVL_T_FAN_1_LIN__MLIN_4 : 8;
  vbittype TAR_OPMO_FAN_1_LIN__MLIN_4 : 3;
  vbittype unused0 : 5;
  vbittype unused1 : 8;
} _c_CTR_FAN_LIN__MLIN_4_msgType;
typedef struct _c_ST_FAN_1_LIN__MLIN_4_msgTypeTag
{
  vbittype AVL_RPM_FAN_1_LIN__MLIN_4 : 8;
  vbittype AVL_TEMP_FAN_1_LIN__MLIN_4 : 8;
  vbittype AVL_I_FAN_1_LIN__MLIN_4 : 8;
  vbittype AVL_U_FAN_1_LIN__MLIN_4 : 8;
  vbittype ST_ERR_BRI_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_TEMP_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_I_SWO_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_BLK_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_SLU_FAN_1_LIN__MLIN_4 : 2;
  vbittype COMM_ERR_FAN_1_LIN__MLIN_4 : 1;
  vbittype AVL_OPMO_FAN_1_LIN__MLIN_4 : 3;
  vbittype ST_ERR_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_I_FAN_1_LIN__MLIN_4 : 2;
  vbittype ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4 : 2;
  vbittype unused0 : 2;
} _c_ST_FAN_1_LIN__MLIN_4_msgType;
typedef struct _c_MasterReq__MLIN_4_msgTypeTag
{
  vbittype MasterReqB0__MLIN_4 : 8;
  vbittype MasterReqB1__MLIN_4 : 8;
  vbittype MasterReqB2__MLIN_4 : 8;
  vbittype MasterReqB3__MLIN_4 : 8;
  vbittype MasterReqB4__MLIN_4 : 8;
  vbittype MasterReqB5__MLIN_4 : 8;
  vbittype MasterReqB6__MLIN_4 : 8;
  vbittype MasterReqB7__MLIN_4 : 8;
} _c_MasterReq__MLIN_4_msgType;
typedef struct _c_SlaveResp__MLIN_4_msgTypeTag
{
  vbittype SlaveRespB0__MLIN_4 : 8;
  vbittype SlaveRespB1__MLIN_4 : 8;
  vbittype SlaveRespB2__MLIN_4 : 8;
  vbittype SlaveRespB3__MLIN_4 : 8;
  vbittype SlaveRespB4__MLIN_4 : 8;
  vbittype SlaveRespB5__MLIN_4 : 8;
  vbittype SlaveRespB6__MLIN_4 : 8;
  vbittype SlaveRespB7__MLIN_4 : 8;
} _c_SlaveResp__MLIN_4_msgType;


/* -----------------------------------------------------------------------------
    &&&~ Signal value names
 ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    &&&~ Message Unions
 ----------------------------------------------------------------------------- */

typedef union _c_CTR_FAN_LIN__MLIN_4_bufTag
{
  vuint16 _w[2];
  vuint8 _c[4];
  _c_CTR_FAN_LIN__MLIN_4_msgType CTR_FAN_LIN__MLIN_4;
} _c_CTR_FAN_LIN__MLIN_4_buf;
typedef union _c_ST_FAN_1_LIN__MLIN_4_bufTag
{
  vuint16 _w[4];
  vuint8 _c[8];
  _c_ST_FAN_1_LIN__MLIN_4_msgType ST_FAN_1_LIN__MLIN_4;
} _c_ST_FAN_1_LIN__MLIN_4_buf;
typedef union _c_MasterReq__MLIN_4_bufTag
{
  vuint16 _w[4];
  vuint8 _c[8];
  _c_MasterReq__MLIN_4_msgType MasterReq__MLIN_4;
} _c_MasterReq__MLIN_4_buf;
typedef union _c_SlaveResp__MLIN_4_bufTag
{
  vuint16 _w[4];
  vuint8 _c[8];
  _c_SlaveResp__MLIN_4_msgType SlaveResp__MLIN_4;
} _c_SlaveResp__MLIN_4_buf;


/* -----------------------------------------------------------------------------
    &&&~ Message Buffers
 ----------------------------------------------------------------------------- */

/* RAM CATEGORY 2 START */
V_MEMRAM0 extern  V_MEMRAM1 _c_CTR_FAN_LIN__MLIN_4_buf V_MEMRAM2 CTR_FAN_LIN__MLIN_4;
/* RAM CATEGORY 2 END */

/* RAM CATEGORY 2 START */
V_MEMRAM0 extern  V_MEMRAM1 _c_ST_FAN_1_LIN__MLIN_4_buf V_MEMRAM2 ST_FAN_1_LIN__MLIN_4;
/* RAM CATEGORY 2 END */

/* RAM CATEGORY 2 START */
V_MEMRAM0 extern  V_MEMRAM1 _c_MasterReq__MLIN_4_buf V_MEMRAM2 MasterReq__MLIN_4;
/* RAM CATEGORY 2 END */

/* RAM CATEGORY 2 START */
V_MEMRAM0 extern  V_MEMRAM1 _c_SlaveResp__MLIN_4_buf V_MEMRAM2 SlaveResp__MLIN_4;
/* RAM CATEGORY 2 END */






/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 259943842
      #error "The magic number of the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\drv_par.h> is different. Please check time and date of generated files!"
  #endif
#else
  #define MAGIC_NUMBER 259943842
#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

#endif /* __DRV_PAR_H__ */
