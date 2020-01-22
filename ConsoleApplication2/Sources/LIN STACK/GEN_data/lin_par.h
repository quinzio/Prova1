/* -----------------------------------------------------------------------------
  Filename:    lin_par.h
  Description: Toolversion: 00.00.00.01.40.03.60.01.00.00
               
               Serial Number: CBD1400360
               Customer Info: SPAL Automotive srl
                              Package: LIN_SLP2
                              Micro: S9S12GN32F1MLC
                              Compiler: Metrowerks 5.1
               
               
               Generator Fwk   : GENy 
               Generator Module: DrvLin__base
               
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

#if !defined(__LIN_PAR_H__)
#define __LIN_PAR_H__

/* -----------------------------------------------------------------------------
    &&&~ MISRA definitions
 ----------------------------------------------------------------------------- */

/* PRQA S 3458 EOF *//* MD_CBD_19.4 */


/* -----------------------------------------------------------------------------
    &&&~ Timeout handling
 ----------------------------------------------------------------------------- */

#define kLinFastTimeoutTaskCycle             10000
#define kLinSlowTimeoutTaskCycle             10
#define kLinTimeBaseHighRes                  10000


/* -----------------------------------------------------------------------------
    &&&~ Message Callback functions
 ----------------------------------------------------------------------------- */

/* return values used by precopy and pretransmit functions */
#define kLinNoCopyData                       0x00U
#define kLinCopyData                         0x01U


/* -----------------------------------------------------------------------------
    &&&~ Access to confirmation flags
 ----------------------------------------------------------------------------- */

#define ST_FAN_1_LIN__MLIN_4_conf_b          (uLinMsgConfirmationFlags.w[0].b0)
#define l_flg_tst_ST_FAN_1_LIN__MLIN_4()     (ST_FAN_1_LIN__MLIN_4_conf_b != 0)
#define l_flg_clr_ST_FAN_1_LIN__MLIN_4()     (ST_FAN_1_LIN__MLIN_4_conf_b = 0)
#define SlaveResp__MLIN_4_conf_b             (uLinMsgConfirmationFlags.w[0].b1)
#define l_flg_tst_SlaveResp__MLIN_4()        (SlaveResp__MLIN_4_conf_b != 0)
#define l_flg_clr_SlaveResp__MLIN_4()        (SlaveResp__MLIN_4_conf_b = 0)


/* -----------------------------------------------------------------------------
    &&&~ Access to indication flags
 ----------------------------------------------------------------------------- */

#define CTR_FAN_LIN__MLIN_4_ind_b            (uLinMsgIndicationFlags.w[0].b0)
#define l_flg_tst_CTR_FAN_LIN__MLIN_4()      (CTR_FAN_LIN__MLIN_4_ind_b != 0)
#define l_flg_clr_CTR_FAN_LIN__MLIN_4()      (CTR_FAN_LIN__MLIN_4_ind_b = 0)
#define MasterReq__MLIN_4_ind_b              (uLinMsgIndicationFlags.w[0].b1)
#define l_flg_tst_MasterReq__MLIN_4()        (MasterReq__MLIN_4_ind_b != 0)
#define l_flg_clr_MasterReq__MLIN_4()        (MasterReq__MLIN_4_ind_b = 0)


/* -----------------------------------------------------------------------------
    &&&~ Macro Access to signals
 ----------------------------------------------------------------------------- */

#define _s_TARVL_V_FAN_1_LIN__MLIN_4         (CTR_FAN_LIN__MLIN_4.CTR_FAN_LIN__MLIN_4.TARVL_V_FAN_1_LIN__MLIN_4)
#define _s_TARVL_T_FAN_1_LIN__MLIN_4         (CTR_FAN_LIN__MLIN_4.CTR_FAN_LIN__MLIN_4.TARVL_T_FAN_1_LIN__MLIN_4)
#define _s_TAR_OPMO_FAN_1_LIN__MLIN_4        (CTR_FAN_LIN__MLIN_4.CTR_FAN_LIN__MLIN_4.TAR_OPMO_FAN_1_LIN__MLIN_4)
#define _s_AVL_RPM_FAN_1_LIN__MLIN_4         (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.AVL_RPM_FAN_1_LIN__MLIN_4)
#define _s_AVL_TEMP_FAN_1_LIN__MLIN_4        (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.AVL_TEMP_FAN_1_LIN__MLIN_4)
#define _s_AVL_I_FAN_1_LIN__MLIN_4           (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.AVL_I_FAN_1_LIN__MLIN_4)
#define _s_AVL_U_FAN_1_LIN__MLIN_4           (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.AVL_U_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_BRI_FAN_1_LIN__MLIN_4      (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_BRI_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4   (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_TEMP_FAN_1_LIN__MLIN_4     (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_TEMP_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4 (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_I_SWO_FAN_1_LIN__MLIN_4    (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_I_SWO_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4   (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4   (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4  (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_BLK_FAN_1_LIN__MLIN_4      (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_BLK_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_SLU_FAN_1_LIN__MLIN_4      (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_SLU_FAN_1_LIN__MLIN_4)
#define _s_AVL_OPMO_FAN_1_LIN__MLIN_4        (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.AVL_OPMO_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_FAN_1_LIN__MLIN_4          (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_I_FAN_1_LIN__MLIN_4        (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_I_FAN_1_LIN__MLIN_4)
#define _s_ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4 (ST_FAN_1_LIN__MLIN_4.ST_FAN_1_LIN__MLIN_4.ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4)
#define _s_MasterReqB0__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB0__MLIN_4)
#define _s_MasterReqB1__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB1__MLIN_4)
#define _s_MasterReqB2__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB2__MLIN_4)
#define _s_MasterReqB3__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB3__MLIN_4)
#define _s_MasterReqB4__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB4__MLIN_4)
#define _s_MasterReqB5__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB5__MLIN_4)
#define _s_MasterReqB6__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB6__MLIN_4)
#define _s_MasterReqB7__MLIN_4               (MasterReq__MLIN_4.MasterReq__MLIN_4.MasterReqB7__MLIN_4)
#define _s_SlaveRespB0__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB0__MLIN_4)
#define _s_SlaveRespB1__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB1__MLIN_4)
#define _s_SlaveRespB2__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB2__MLIN_4)
#define _s_SlaveRespB3__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB3__MLIN_4)
#define _s_SlaveRespB4__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB4__MLIN_4)
#define _s_SlaveRespB5__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB5__MLIN_4)
#define _s_SlaveRespB6__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB6__MLIN_4)
#define _s_SlaveRespB7__MLIN_4               (SlaveResp__MLIN_4.SlaveResp__MLIN_4.SlaveRespB7__MLIN_4)


/* -----------------------------------------------------------------------------
    &&&~ Access to signals
 ----------------------------------------------------------------------------- */

#define l_u8_rd_TARVL_V_FAN_1_LIN__MLIN_4()  _s_TARVL_V_FAN_1_LIN__MLIN_4
#define l_u8_wr_TARVL_V_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_TARVL_V_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_TARVL_T_FAN_1_LIN__MLIN_4()  _s_TARVL_T_FAN_1_LIN__MLIN_4
#define l_u8_wr_TARVL_T_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_TARVL_T_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_TAR_OPMO_FAN_1_LIN__MLIN_4() _s_TAR_OPMO_FAN_1_LIN__MLIN_4
#define l_u8_wr_TAR_OPMO_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_TAR_OPMO_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x07)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_AVL_RPM_FAN_1_LIN__MLIN_4()  _s_AVL_RPM_FAN_1_LIN__MLIN_4
#define l_u8_wr_AVL_RPM_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_AVL_RPM_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_AVL_TEMP_FAN_1_LIN__MLIN_4() _s_AVL_TEMP_FAN_1_LIN__MLIN_4
#define l_u8_wr_AVL_TEMP_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_AVL_TEMP_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_AVL_I_FAN_1_LIN__MLIN_4()    _s_AVL_I_FAN_1_LIN__MLIN_4
#define l_u8_wr_AVL_I_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_AVL_I_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_AVL_U_FAN_1_LIN__MLIN_4()    _s_AVL_U_FAN_1_LIN__MLIN_4
#define l_u8_wr_AVL_U_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_AVL_U_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_BRI_FAN_1_LIN__MLIN_4() _s_ST_ERR_BRI_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_BRI_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_BRI_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4() _s_ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_BRI_SC_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_TEMP_FAN_1_LIN__MLIN_4() _s_ST_ERR_TEMP_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_TEMP_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_TEMP_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4() _s_ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_TEMP_SWO_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_I_SWO_FAN_1_LIN__MLIN_4() _s_ST_ERR_I_SWO_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_I_SWO_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_I_SWO_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4() _s_ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_I_OFFS_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4() _s_ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_U_IPLB_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4() _s_ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_GND_LSS_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_BLK_FAN_1_LIN__MLIN_4() _s_ST_ERR_BLK_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_BLK_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_BLK_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_SLU_FAN_1_LIN__MLIN_4() _s_ST_ERR_SLU_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_SLU_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_SLU_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_AVL_OPMO_FAN_1_LIN__MLIN_4() _s_AVL_OPMO_FAN_1_LIN__MLIN_4
#define l_u8_wr_AVL_OPMO_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_AVL_OPMO_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x07)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_FAN_1_LIN__MLIN_4()   _s_ST_ERR_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_I_FAN_1_LIN__MLIN_4() _s_ST_ERR_I_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_I_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_I_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4() _s_ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4
#define l_u8_wr_ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_ST_ERR_LD_SUPY_IPLB_FAN_1_LIN__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0x03)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB0__MLIN_4()        _s_MasterReqB0__MLIN_4
#define l_u8_wr_MasterReqB0__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB0__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB1__MLIN_4()        _s_MasterReqB1__MLIN_4
#define l_u8_wr_MasterReqB1__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB1__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB2__MLIN_4()        _s_MasterReqB2__MLIN_4
#define l_u8_wr_MasterReqB2__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB2__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB3__MLIN_4()        _s_MasterReqB3__MLIN_4
#define l_u8_wr_MasterReqB3__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB3__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB4__MLIN_4()        _s_MasterReqB4__MLIN_4
#define l_u8_wr_MasterReqB4__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB4__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB5__MLIN_4()        _s_MasterReqB5__MLIN_4
#define l_u8_wr_MasterReqB5__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB5__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB6__MLIN_4()        _s_MasterReqB6__MLIN_4
#define l_u8_wr_MasterReqB6__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB6__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_MasterReqB7__MLIN_4()        _s_MasterReqB7__MLIN_4
#define l_u8_wr_MasterReqB7__MLIN_4(a) \
{ \
  LinStartRxBitSignalReadSync(); \
  _s_MasterReqB7__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndRxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB0__MLIN_4()        _s_SlaveRespB0__MLIN_4
#define l_u8_wr_SlaveRespB0__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB0__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB1__MLIN_4()        _s_SlaveRespB1__MLIN_4
#define l_u8_wr_SlaveRespB1__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB1__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB2__MLIN_4()        _s_SlaveRespB2__MLIN_4
#define l_u8_wr_SlaveRespB2__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB2__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB3__MLIN_4()        _s_SlaveRespB3__MLIN_4
#define l_u8_wr_SlaveRespB3__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB3__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB4__MLIN_4()        _s_SlaveRespB4__MLIN_4
#define l_u8_wr_SlaveRespB4__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB4__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB5__MLIN_4()        _s_SlaveRespB5__MLIN_4
#define l_u8_wr_SlaveRespB5__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB5__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB6__MLIN_4()        _s_SlaveRespB6__MLIN_4
#define l_u8_wr_SlaveRespB6__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB6__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}
#define l_u8_rd_SlaveRespB7__MLIN_4()        _s_SlaveRespB7__MLIN_4
#define l_u8_wr_SlaveRespB7__MLIN_4(a) \
{ \
  LinStartTxBitSignalReadSync(); \
  _s_SlaveRespB7__MLIN_4 = ((vuint8) (((vuint8) (a)) & 0xFF)); \
  LinEndTxBitSignalReadSync(); \
}


/* -----------------------------------------------------------------------------
    &&&~ Access to data bytes of Rx messages
 ----------------------------------------------------------------------------- */

/* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */
#define c1_CTR_FAN_LIN__MLIN_4_c             (CTR_FAN_LIN__MLIN_4._c[0])
#define c2_CTR_FAN_LIN__MLIN_4_c             (CTR_FAN_LIN__MLIN_4._c[1])
#define c3_CTR_FAN_LIN__MLIN_4_c             (CTR_FAN_LIN__MLIN_4._c[2])



/* -----------------------------------------------------------------------------
    &&&~ Access to data bytes of Tx messages
 ----------------------------------------------------------------------------- */

/* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */
#define c1_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[0])
#define c2_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[1])
#define c3_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[2])
#define c4_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[3])
#define c5_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[4])
#define c6_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[5])
#define c7_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[6])
#define c8_ST_FAN_1_LIN__MLIN_4_c            (ST_FAN_1_LIN__MLIN_4._c[7])



/* -----------------------------------------------------------------------------
    &&&~ Prototypes of Precopy Functions
 ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    &&&~ Prototypes of Indication Functions
 ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    &&&~ Prototypes of Confirmation Functions
 ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    &&&~ Prototypes of Pretransmit Functions
 ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    &&&~ Message Handles
 ----------------------------------------------------------------------------- */

#define LinRxCTR_FAN_LIN__MLIN_4             0
#define LinTxST_FAN_1_LIN__MLIN_4            1
#define LinRxMasterReq__MLIN_4               2
#define LinTxSlaveResp__MLIN_4               3


/* -----------------------------------------------------------------------------
    &&&~ Declaration of objects provided by lin_par.c
 ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    &&&~ Name defines for LIN channel
 ----------------------------------------------------------------------------- */

#ifndef SPAL_LIN
#define SPAL_LIN                             0
#endif

#ifndef _MLIN_4
#define _MLIN_4                              0
#endif




/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 259943842
      #error "The magic number of the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\lin_par.h> is different. Please check time and date of generated files!"
  #endif
#else
  #define MAGIC_NUMBER 259943842
#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

#endif /* __LIN_PAR_H__ */
