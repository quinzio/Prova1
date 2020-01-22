/* -----------------------------------------------------------------------------
  Filename:    lin_par.c
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

#include "v_inc.h"
#include "lin_cfg.h"
#include "drv_par.h"
#include "lin_par_i.h"
#include "lin_par.h"
#include "sio_cfg.h"
#include "sio_drv.h"
/* -----------------------------------------------------------------------------
    &&&~ MISRA definitions
 ----------------------------------------------------------------------------- */

/* PRQA S 759 EOF *//* MD_CBD_18.4 */
/* PRQA S 3453 EOF *//* MD_CBD_19.7 */


/* -----------------------------------------------------------------------------
    &&&~ Configurable NAD
 ----------------------------------------------------------------------------- */

V_MEMRAM0 V_MEMRAM1 vuint8 V_MEMRAM2 bLinConfiguredNAD;


/* -----------------------------------------------------------------------------
    &&&~ Response max times for each data length in tics of LinfastTimerTask()
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_T_FRAME_MAX)
V_MEMROM0 V_MEMROM1 tLinFpTimerType V_MEMROM2 cabFpResponseMaxTimes[9] = { 2, 2, 2, 2, 2, 2, 2, 2, 2 };

#endif



/* -----------------------------------------------------------------------------
    &&&~ List of Ids for all messages
 ----------------------------------------------------------------------------- */

V_MEMRAM0 V_MEMRAM1 vuint8 V_MEMRAM2 cabLinSlaveProtectedIdTbl[kLinNumberOfConfigurablePIDs];
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 cabLinSlaveProtectedIdInitTbl[kLinNumberOfConfigurablePIDs] = 
{
  0x03 /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */, 
  0xC4 /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */
};
V_MEMROM0 V_MEMROM1 tMessageInfoType V_MEMROM2 cabLinRxTxMsgInfo[kLinNumberOfMsgInfoObjects] = 
{
  { kChecksumEnhanced, 0, kMessageDirectionRx, 8 } /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */, 
  { kChecksumEnhanced, 0, kMessageDirectionTx, 8 } /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */, 
  { kChecksumClassic, 0, kMessageDirectionRx, 8 } /* ID: 0x3c, Handle: 2, Name: MasterReq__MLIN_4 */, 
  { kChecksumClassic, 0, kMessageDirectionTx, 8 } /* ID: 0x3d, Handle: 3, Name: SlaveResp__MLIN_4 */
};




/* -----------------------------------------------------------------------------
    &&&~ Receive Structures
 ----------------------------------------------------------------------------- */

V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 cabLinRxMsgBufferLenTbl[kLinNumberOfRxObjects] = 
{
  3 /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */
};



/* -----------------------------------------------------------------------------
    &&&~ Tables for send and receive messages
 ----------------------------------------------------------------------------- */

V_MEMROM0 V_MEMROM1 tLinMsgDataPtrType V_MEMROM2 capbLinRxTxMsgDataPtr[kLinNumberOfRxTxObjects] = 
{
  (tLinMsgDataPtrType) CTR_FAN_LIN__MLIN_4._c /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */, 
  (tLinMsgDataPtrType) ST_FAN_1_LIN__MLIN_4._c /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */
};



/* -----------------------------------------------------------------------------
    &&&~ Precopy Functions of Rx Messages
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_MSG_PRECOPY_FCT) || defined(LIN_ENABLE_DIAGMSG_PRECOPY_FCT)
#endif

#ifdef LIN_ENABLE_MSG_PRECOPY_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgPrecopyFct V_MEMROM2 capfLinRxApplMsgPrecopyFctTbl[kLinNumberOfRxObjects] = 
{
  V_NULL /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */
};

#endif

#ifdef LIN_ENABLE_DIAGMSG_PRECOPY_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgPrecopyFct V_MEMROM2 capfLinRxDiagMsgPrecopyFctTbl[kLinNumberOfDiagRxObjects] = 
{
  V_NULL /* ID: 0x3c, Handle: 2, Name: MasterReq__MLIN_4 */
};

#endif



/* -----------------------------------------------------------------------------
    &&&~ Pretransmit Functions of Tx messages
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_MSG_PRETRANSMIT_FCT) || defined(LIN_ENABLE_DIAGMSG_PRETRANSMIT_FCT)
#endif

#ifdef LIN_ENABLE_MSG_PRETRANSMIT_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgPretransmitFct V_MEMROM2 capfLinTxApplMsgPretransmitFctTbl[kLinNumberOfTxObjects] = 
{
  V_NULL /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */
};

#endif

#ifdef LIN_ENABLE_DIAGMSG_PRETRANSMIT_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgPretransmitFct V_MEMROM2 capfLinTxDiagMsgPretransmitFctTbl[kLinNumberOfDiagTxObjects] = 
{
  V_NULL /* ID: 0x3d, Handle: 3, Name: SlaveResp__MLIN_4 */
};

#endif



/* -----------------------------------------------------------------------------
    &&&~ Indication Flags of Rx messages
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_MSG_INDICATION_FLAG) || defined(LIN_ENABLE_DIAGMSG_INDICATION_FLAG)
V_MEMRAM0 V_MEMRAM1_NEAR tLinMsgIndicationBits V_MEMRAM2_NEAR uLinMsgIndicationFlags;

#endif

#if defined(LIN_ENABLE_MSG_INDICATION_FLAG) || defined(LIN_ENABLE_DIAGMSG_INDICATION_FLAG) || defined(LIN_ENABLE_INTMSG_INDICATION_FLAG)
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 cabLinMsgIndicationOffset[kLinNumberOfRxObjects+kLinNumberOfDiagRxObjects] = 
{
  0 /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */, 
  0 /* ID: 0x3c, Handle: 2, Name: MasterReq__MLIN_4 */
};


V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 cabLinMsgIndicationMask[kLinNumberOfRxObjects+kLinNumberOfDiagRxObjects] = 
{
  0x01 /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */, 
  0x02 /* ID: 0x3c, Handle: 2, Name: MasterReq__MLIN_4 */
};


#endif



/* -----------------------------------------------------------------------------
    &&&~ Confirmation Flags of Tx messages
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_MSG_CONFIRMATION_FLAG) || defined(LIN_ENABLE_DIAGMSG_CONFIRMATION_FLAG)
V_MEMRAM0 V_MEMRAM1_NEAR tLinMsgConfirmationBits V_MEMRAM2_NEAR uLinMsgConfirmationFlags;

V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 cabLinMsgConfirmationOffset[kLinNumberOfTxObjects+kLinNumberOfDiagTxObjects] = 
{
  0 /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */, 
  0 /* ID: 0x3d, Handle: 3, Name: SlaveResp__MLIN_4 */
};


V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 cabLinMsgConfirmationMask[kLinNumberOfTxObjects+kLinNumberOfDiagTxObjects] = 
{
  0x01 /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */, 
  0x02 /* ID: 0x3d, Handle: 3, Name: SlaveResp__MLIN_4 */
};


#endif



/* -----------------------------------------------------------------------------
    &&&~ Indication Functions of Rx messages
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_MSG_INDICATION_FCT) || defined(LIN_ENABLE_DIAGMSG_INDICATION_FCT)
#endif

#ifdef LIN_ENABLE_MSG_INDICATION_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgIndicationFct V_MEMROM2 capfLinRxApplMsgIndicationFctTbl[kLinNumberOfRxObjects] = 
{
  V_NULL /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */
};

#endif

#ifdef LIN_ENABLE_DIAGMSG_INDICATION_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgIndicationFct V_MEMROM2 capfLinRxDiagMsgIndicationFctTbl[kLinNumberOfDiagRxObjects] = 
{
  V_NULL /* ID: 0x3c, Handle: 2, Name: MasterReq__MLIN_4 */
};

#endif



/* -----------------------------------------------------------------------------
    &&&~ Confirmation Functions of Tx messages
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_MSG_CONFIRMATION_FCT) || defined(LIN_ENABLE_DIAGMSG_CONFIRMATION_FCT)
#endif

#ifdef LIN_ENABLE_MSG_CONFIRMATION_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgConfirmationFct V_MEMROM2 capfLinTxApplMsgConfirmationFctTbl[kLinNumberOfTxObjects] = 
{
  V_NULL /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */
};

#endif

#ifdef LIN_ENABLE_DIAGMSG_CONFIRMATION_FCT
V_MEMROM0 V_MEMROM1 tApplLinMsgConfirmationFct V_MEMROM2 capfLinTxDiagMsgConfirmationFctTbl[kLinNumberOfDiagTxObjects] = 
{
  V_NULL /* ID: 0x3d, Handle: 3, Name: SlaveResp__MLIN_4 */
};

#endif



/* -----------------------------------------------------------------------------
    &&&~ Default values for Frames
 ----------------------------------------------------------------------------- */

#if defined(LIN_ENABLE_INIT_TX_DEFAULT_DATA) || defined(LIN_ENABLE_START_TX_DEFAULT_DATA) || defined(LIN_ENABLE_STOP_TX_DEFAULT_DATA) || defined(LIN_ENABLE_INIT_RX_DEFAULT_DATA) || defined(LIN_ENABLE_START_RX_DEFAULT_DATA) || defined(LIN_ENABLE_STOP_RX_DEFAULT_DATA)
V_MEMROM0 V_MEMROM1 static vuint8 V_MEMROM2 LinMsgDefault_CTR_FAN_LIN__MLIN_4[3] = { 0xFF, 0xFF, 0xFF };

V_MEMROM0 V_MEMROM1 static vuint8 V_MEMROM2 LinMsgDefault_ST_FAN_1_LIN__MLIN_4[8] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xEF, 0xFF };

#endif

#if defined(LIN_ENABLE_INIT_TX_DEFAULT_DATA) || defined(LIN_ENABLE_START_TX_DEFAULT_DATA) || defined(LIN_ENABLE_STOP_TX_DEFAULT_DATA) || defined(LIN_ENABLE_INIT_RX_DEFAULT_DATA) || defined(LIN_ENABLE_START_RX_DEFAULT_DATA) || defined(LIN_ENABLE_STOP_RX_DEFAULT_DATA)
V_MEMROM0 V_MEMROM1 vuint8 V_MEMROM2 V_MEMROM3* V_MEMROM1 V_MEMROM2 capbLinRxTxDefaultDataPtrTbl[kLinNumberOfRxTxObjects] = 
{
  LinMsgDefault_CTR_FAN_LIN__MLIN_4 /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */, 
  LinMsgDefault_ST_FAN_1_LIN__MLIN_4 /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */
};

#endif



/* -----------------------------------------------------------------------------
    &&&~ Pointer to Diagnostic Message Buffers
 ----------------------------------------------------------------------------- */



/* -----------------------------------------------------------------------------
    &&&~ ConfigurablePID Handle Table
 ----------------------------------------------------------------------------- */

V_MEMROM0 V_MEMROM1 tLinFrameHandleType V_MEMROM2 cabConfigurablePIDHandleTbl[kLinNumberOfConfigurablePIDs] = 
{
  0 /* ID: 0x03, Handle: 0, Name: CTR_FAN_LIN__MLIN_4 */, 
  1 /* ID: 0x04, Handle: 1, Name: ST_FAN_1_LIN__MLIN_4 */
};



/* begin Fileversion check */
#ifndef SKIP_MAGIC_NUMBER
#ifdef MAGIC_NUMBER
  #if MAGIC_NUMBER != 259943842
      #error "The magic number of the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\lin_par.c> is different. Please check time and date of generated files!"
  #endif
#else
  #error "The magic number is not defined in the generated file <C:\Users\boschesir\Desktop\BMW 35uP\KBL201P fw 12_5_207_RC119.2\Sources\GEN_data\lin_par.c> "

#endif  /* MAGIC_NUMBER */
#endif  /* SKIP_MAGIC_NUMBER */

/* end Fileversion check */

