/* Kernbauer Version: 1.16 Konfiguration: VStdLib Erzeugungsgangnummer: 1 */


/**********************************************************************************************************************
| Project Name: vstdlib library
|    File Name: vstdlib_ll.h
|
|  Description        : lowlevel part of the implementation of standard 
|                       Vector functions.
|                       Mcs12, Mcs12x specific implementation
|                               
|----------------------------------------------------------------------------------------------------------------------
|               C O P Y R I G H T
|----------------------------------------------------------------------------------------------------------------------
|   Copyright (c) by Vector Informatik GmbH.     All rights reserved.
|
|   This software is copyright protected and proprietary to 
|   Vector Informatik GmbH. Vector Informatik GmbH grants to you 
|   only those rights as set out in the license conditions. All 
|   other rights remain with Vector Informatik GmbH.
|
|----------------------------------------------------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|----------------------------------------------------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     ------------------------------------------------------------------------------
| Ml           Patrick Markl             Vector Informatik GmbH
| Hs           Hannes Haas               Vector Informatik GmbH
| Ces          Senol Cendere             Vector Informatik GmbH
| Ou           Mihai Olariu              Vector Informatik GmbH
| Was          Andreas Weinrauch         Vector Informatik GmbH
|----------------------------------------------------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|----------------------------------------------------------------------------------------------------------------------
| Date       Version   Author  Description
|----------  --------  ------  ----------------------------------------------------------------------------------------
|2006-02-01  01.00.00    Ml     - Creation
|2006-02-14  01.01.00    Ml     - Add copy routines for gconst segment
|2006-03-23  01.02.00    Hs     - HL part updated
|2006-03-27  01.03.00    Hs     - ESCAN00015838: Added support for Hiware compiler
|2006-03-29  01.04.00    Hs     - ESCAN00015881: added void cast of return value
|2006-06-19  01.05.00    Hs     - ESCAN00016656: changed Hiware MemCpy implementation
|2006-07-13  01.06.00    Ml     - Using build-in copy functions for Metrowerks
|2006-08-04  01.07.00    Ml     - Added Mcs12 and compatibility for older VStdLib functions
|2006-08-12  01.08.00    Ml     - Added compatibility for memcpy
|2006-10-26  01.09.00    Ml     - Added encapsulation for different platforms
|2006-12-12  01.10.00    Ces    - Added support for MCS12 derivative
|                        Ces    - Added support for IAR compiler
|2007-02-21  01.11.00    Ml     - New HL template - changed handling of interrupt lock functions
|2007-02-28  01.12.00    Ml     - ESCAN00019768: Added support for IAR compiler for HC12Idx target
|2007-05-10  01.13.00    Ml     - HL 2.17.00
|2007-08-24  01.14.00    Ces    - removed VSTD_ENABLE_DEFAULT_INTCTRL from token VStdDefine_Functionality
|2008-01-24  01.14.01    Ml     - ESCAN00022655: Added casts to avoid warnings
|                        Ml     - Removed VStdIntDisable_Disable, VStdIntDisable_Disable, VStdGetIntState
|2008-05-05  01.14.02    Ml     - ESCAN00026681: Encapsulate S12X specific assembler code
|2008-05-28  01.14.03    Ml     - ESCAN00024192: Added possibility to use standard implementation
|2008-08-20  01.15.00    Ou     - ESCAN00029356: VStdRomMemCpy() fails with Cosmic compiler versions >= 4.07
|2009-01-19  01.16.00    Ou     - ESCAN00032082: ECU shows undefined behavior when copying GPAGE ROM data to RAM
|                        Ou     - ESCAN00032448: CANbedded only: Extend the API in order to support global variables copy routines
|2010-07-05  01.17.00    Ou     - No change, just merge with HL 3.05.01
|2010-08-20  01.18.00    Ou     - ESCAN00044789: Add support for compiler abstraction in order to be AUTOSAR compliant
|2011-03-08  01.19.00    Ou     - ESCAN00049251: Support IAR compiler for MCS12X derivative
|2011-07-11  01.19.01    Ou     - No change, just add a user config-file in the implementation package
|2013-06-05  01.20.00    Was    - ESCAN00067884:Support IAR compiler version 3.x
|********************************************************************************************************************/

/**********************************************************************************************************************
|
| Project Name : vstdlib library
|    File Name : vstdlib_HL.h
|
|  Description : Highlevel part of the implementation of standard Vector functions
|                               
|----------------------------------------------------------------------------------------------------------------------
|               C O P Y R I G H T
|----------------------------------------------------------------------------------------------------------------------
|   Copyright (c) by Vector Informatik GmbH.     All rights reserved.
|
|   This software is copyright protected and proprietary to 
|   Vector Informatik GmbH. Vector Informatik GmbH grants to you 
|   only those rights as set out in the license conditions. All 
|   other rights remain with Vector Informatik GmbH.
|
|----------------------------------------------------------------------------------------------------------------------
|               A U T H O R   I D E N T I T Y
|----------------------------------------------------------------------------------------------------------------------
| Initials     Name                      Company
| --------     ---------------------     ------------------------------------------------------------------------------
| Ml           Patrick Markl             Vector Informatik GmbH
| HH           Hartmut Hoerner           Vector Informatik GmbH
| Ms           Gunnar Meiss              Vector Informatik GmbH
| Et           Thomas Ebert              Vector Informatik GmbH
| Bus          Sabine Buecherl           Vector Informatik GmbH
| Hs           Hannes Haas               Vector Informatik GmbH
| BWR          Brandon Root              Vector CANtech Inc.
| Ard          Thomas Arnold             Vector Informatik GmbH
| Tvi          Timo Vanoni               Vector Informatik GmbH
|----------------------------------------------------------------------------------------------------------------------
|               R E V I S I O N   H I S T O R Y
|----------------------------------------------------------------------------------------------------------------------
| Date       Version   Author  Description
|----------  --------  ------  ----------------------------------------------------------------------------------------
|2003-09-06  00.01.00    Ml     - Creation
|                        Ml     - Added HC12 Cosmic
|2003-10-07  00.02.00    Ml/HH  - v_lib input merged
|2004-04-10  00.03.00    Ml     - Added headerfile
|                        Ml     - Added HC08 Cosmic
|                        Ml     - removed channel dependence for assertions
|                        Ml     - Added function to retrieve the IRQ state
|                        Ml     - Added VSTD_FULL_LIB_SUPPORT switch
|2004-04-27  01.00.00    Ml     - resolved issues found in code inspection
|2004-05-17  01.01.00    Ml     - no changes
|                        Ml     - changed bugfix into releaseversion
|2004-05-18  01.02.00    Ms/HH  - changed memory qualifier concept
|2004-06-01  01.03.00    Ml     - no changes
|2004-08-06  01.04.00    Ml     - Added macros for local IRQ disable
|                               - Split of library into HL and LL
|2004-08-18  01.05.00    Ml     - Added function qualifier
|2004-09-06  01.06.00    Ml     - ESCAN00009316: fixed parameter type mismatch
|2004-09-27  01.07.00    Ml     - changed memcpy macros
|                        Ml     - changed namingconvention
|2004-09-29  01.08.00    Et     - ESCAN00009692: split VStdMemCpy in VStdRamMemCpy and VStdRomMemCpy   
|2004-10-01  01.09.00    Ml     - ESCAN00009731: usage of standard type qualifiers
|2004-10-26  01.10.00    Ml     - ESCAN00010000: LL parts are split into HW specific files
|2004-11-09  01.11.00    Ml     - no changes
|2004-11-15  01.12.00    Ml     - ESCAN00010533: Added optimized memory functions for HC08
|                        Ml     - ESCAN00010672: Added memory qualifier
|2005-01-24  01.13.00    Ml     - ESCAN00010670: Added memset
|2005-02-25  01.14.00    Ml     - ESCAN00011215  Added VStdInitPowerOn
|2005-04-26  01.15.00    Ml     - ESCAN00012047: Encapsulate VStdInitPowerOn
|            01.16.00    Ml     - ESCAN00012105: Change definition of VStdMemCpy
|2005-05-30  02.00.00    Ml     - ESCAN00013446: Development concept changed
|2005-09-12  02.01.00    Ml     - ESCAN00013512: Fixed Misra warnings
|2005-11-06  02.02.00    Ml     - ESCAN00014183: no changes
|2005-11-28  02.03.00    Ml     - Changed version defines
|                        Ml     - ESCAN00014428: VStdMemClr/Set are now internal functions
|                        Ml     - ESCAN00014409: Added casts for mem function defines
|2005-12-12  02.04.00    Ml/Bus - ESCAN00012774: insert comments for different resource categories
|2006-01-12  02.05.00    Ml     - ESCAN00014897: Changed interrupt handling
|2006-01-14  02.06.00    Ml     - ESCAN00014908: Adapted interrupt control handling for RI1.5
|2006-02-07  02.07.00    Ml     - Added VStdGetInterruptState
|                        Hs     - ESCAN00015802: compatibility defines added
|2006-05-09  02.08.00    Ces    - ESCAN00016289: Insert memory qualifier V_MEMROM3 to VStdRomMemCpy()
|2006-05-24  02.09.00    BWR    - ESCAN00016488: Compiler error in VStdSuspendAllInterrupts and VStdResumeAllInterrupts
|2006-07-16  02.10.00    Ml     - Changed return type of VStdGetInterruptState from uint8 to tVStdIrqStateType
|2006-07-31  02.11.00    Ml     - ESCAN00017133: Changed assertion handling
|2006-08-04  02.12.00    Ml     - ESCAN00017188: Added compatibility for older copy functions
|2006-11-20  02.13.00    Ml     - Removed VStdGetInterruptState
|2006-12-07  02.14.00    Ml     - Changed switch names
|                        Ml     - ESCAN00018889: Fixed code inspection report findings
|                               - ESCAN00018880: added casts to memXXX macros
|2007-01-27  02.15.00    Ml,Ard - ESCAN00019251: VStdLib HL usable for RI1.4 and RI1.5
|2007-02-19  02.16.00    Ml     - ESCAN00019672: no changes
|                        Ml     - ESCAN00019674: Changed names of interrupt lock macros
|2007-05-10  02.17.00    Ml     - ESCAN00020705: no changes
|                        Ml     - ESCAN00020241: Cast of length parameter
|2007-11-26  03.00.00    Ml     - ESCAN00024126: Support word and dword copy routines
|                        Ml     - ESCAN00024192: Default and specific implementations supported at the same time
|2008-06-25  03.01.00    Ml     - ESCAN00023334: Added u suffix to the version numbers
|                        Ml     - ESCAN00018659: No changes
|                        Ml     - ESCAN00015501: Added compatibility to older version defines   
|2008-07-17  03.01.01    Ml     - ESCAN00027617: Added library version check support
|2008-07-23  03.02.00    Ml     - ESCAN00028605: old interrupt status is initialized by low level token
|2008-07-27  03.02.01    Ml     - ESCAN00028752: Include of MSR header according to naming convention
|2008-08-13  03.03.00    Ml     - ESCAN00029196: Encapsulated AUTOSAR library version check
|2008-12-08  03.03.01    Ml     - ESCAN00031855: Additional memcpy interfaces
|2009-06-30  03.04.00    Ml     - ESCAN00036089: Application must care about nested calls in case of user handling of locks
|                        Ml     - ESCAN00035049: Resolved extended version check include
|2009-10-15  03.05.00    Ml     - ESCAN00038482: Added additional function qualifier (VSTD_API_2)
|                        Ml     - ESCAN00038485: Added general OS header include.
|2009-10-19  03.05.01    Ml     - ESCAN00038485: Renamed the OS header include according to osCAN
|2010-01-21  03.06.00    Tvi    - ESCAN00040103: [AUTOSAR] Add support for compiler abstraction
|2011-09-01  03.06.01    Tvi    - ESCAN00052664: [MSR only] "upper/lower case" issue @ #include compiler/memory abstraction
|2012-06-20  03.07.00    Tvi    - ESCAN00058623: AR4-170: support VStdLib with CFG5 MSR4 R5
|*********************************************************************************************************************/

#if !defined(_VSTDLIB_H_)
# define _VSTDLIB_H_

/* Include the configuration for vstdlib */
# include "v_cfg.h"

#if defined(VSTD_ENABLE_LIBRARY_FUNCTIONS)
# if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
#  include <String.h>
# endif
#endif

/* Encapsulation of AUTOSAR library check */
#if defined(VGEN_ENABLE_GW_ASRPDUR)
# define VSTD_ENABLE_MSR_LIB_VERSION_CHECK
# define VSTD_ENABLE_API_AUTOSAR
#endif

#if defined (VSTD_ENABLE_API_AUTOSAR)
/* If MICROSAR is used, several headers are needed */
# include "Std_Types.h"
# include "Compiler_Cfg.h"
# include "Compiler.h"
#else
# include "v_def.h"
#endif

/* Includes required in the header file */

/*********************************************************************************************************************/
/* Defines                                                                                                           */
/*********************************************************************************************************************/

/* ##V_CFG_MANAGEMENT ##CQProject : VStdLib_Mcs12x CQComponent : Implementation */
#define VSTDLIB_MCS12X_VERSION         0x0120u
#define VSTDLIB_MCS12X_RELEASE_VERSION 0x00u

/* ##V_CFG_MANAGEMENT ##CQProject : VStdLib__coreHll CQComponent : Implementation */
# define VSTDLIB__COREHLL_VERSION         0x0307u
# define VSTDLIB__COREHLL_RELEASE_VERSION 0x00u

/* Introduced for comparibility with older VStdLib versions */
# define COMMON_VSTDLIB_VERSION             VSTDLIB__COREHLL_VERSION
# define COMMON_VSTDLIB_RELEASE_VERSION     VSTDLIB__COREHLL_RELEASE_VERSION

#if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12)  
# if !(defined(VSTD_ENABLE_DEFAULT_INTCTRL) || \
       defined(VSTD_ENABLE_OSEK_INTCTRL)    || \
       defined(VSTD_ENABLE_APPL_INTCTRL) )
#  define VSTD_ENABLE_DEFAULT_INTCTRL
# endif
#endif

#if !defined(VSTD_ENABLE_API_AUTOSAR)
# define STATIC static
# define VSTD_DEF_P2FARVAR_PARA(storage, ptrtype, memclass, ptrclass)   storage V_MEMRAM1_FAR ptrtype V_MEMRAM2_FAR V_MEMRAM3_FAR *
#endif

/* Compatibility defines */
# if !defined(VSTD_API_0)
#  define VSTD_API_0
# endif

# if !defined(VSTD_API_1)
#  define VSTD_API_1
# endif

# if !defined(VSTD_API_2)
#  define VSTD_API_2
# endif

# if !defined(VSTD_CALLBACK_0)
#  define VSTD_CALLBACK_0
# endif

# if !defined(VSTD_CALLBACK_1)
#  define VSTD_CALLBACK_1
# endif

# if !defined(VSTD_NONE)
#  define VSTD_NONE
# endif

/* MICROSAR compatibility */
# if defined(VSTD_ENABLE_API_AUTOSAR)

#  if !defined(VSTD_USE_MEMORY_MACROS)
#   define VSTD_USE_MEMORY_MACROS STD_OFF
#  endif

# endif


/* Memory abtraction layer */
# if defined(VSTD_ENABLE_API_AUTOSAR)
#  if !defined(VSTDLIB_AUTOMATIC)
#    define VSTDLIB_AUTOMATIC AUTOMATIC
#  endif
#  if !defined(V_NULL)
#   define V_NULL NULL_PTR
#  endif

#  define VSTD_DEF_VAR(storage, vartype, memclass)                    storage VAR(vartype, VSTDLIB_##memclass)
#  define VSTD_DEF_VAR_TYPE(storage, vartype)                         typedef storage VAR(vartype, TYPEDEF)
#  define VSTD_DEF_P2VAR(storage, ptrtype, memclass, ptrclass)        storage P2VAR(ptrtype, VSTDLIB_##memclass, VSTDLIB_##ptrclass)
#  define VSTD_DEF_P2VAR_TYPE(storage, ptrtype, ptrclass)             typedef storage P2VAR(ptrtype, TYPEDEF, VSTDLIB_##ptrclass)
#  define VSTD_DEF_P2VAR_PARA(storage, ptrtype, memclass, ptrclass)   storage P2VAR(ptrtype, VSTDLIB_##memclass, VSTDLIB_##ptrclass)
#  define VSTD_DEF_CONSTP2VAR(storage, ptrtype, memclass, ptrclass)   storage CONSTP2VAR(ptrtype, VSTDLIB_##memclass, VSTDLIB_##ptrclass)
#  define VSTD_DEF_CONST(storage, vartype, memclass)                  storage CONST(vartype, VSTDLIB_##memclass)
#  define VSTD_DEF_CONST_TYPE(storage, vartype)                       typedef storage CONST(vartype, TYPEDEF)
#  define VSTD_DEF_P2CONST(storage, ptrtype, memclass, ptrclass)      storage P2CONST(ptrtype, VSTDLIB_##memclass, VSTDLIB_##ptrclass)
#  define VSTD_DEF_P2CONST_TYPE(storage, ptrtype, ptrclass)           typedef storage P2CONST(ptrtype, TYPEDEF, VSTDLIB_##ptrclass)
#  define VSTD_DEF_P2CONST_PARA(storage, ptrtype, memclass, ptrclass) storage P2CONST(ptrtype, VSTDLIB_##memclass, VSTDLIB_##ptrclass)
#  define VSTD_DEF_CONSTP2CONST(storage, ptrtype, memclass, ptrclass) storage CONSTP2CONST(ptrtype, VSTDLIB_##memclass, VSTDLIB_##ptrclass)
#  define VSTD_DEF_FUNC(storage, rettype, memclass)                   storage FUNC(rettype, VSTDLIB_##memclass)
#  define VSTD_DEF_FUNC_API(storage, rettype, memclass)               storage FUNC(rettype, VSTDLIB_##memclass)
#  define VSTD_DEF_FUNC_CBK(storage, rettype, memclass)               storage FUNC(rettype, VSTDLIB_##memclass)

# else /* VSTD_ENABLE_API_AUTOSAR */

#  define VSTD_DEF_VAR(storage, vartype, memclass)                    V_MEMRAM0 storage V_MEMRAM1 vartype V_MEMRAM2
#  define VSTD_DEF_VAR_TYPE(storage, vartype)                         typedef storage V_MEMRAM1 vartype V_MEMRAM2
#  define VSTD_DEF_P2VAR(storage, ptrtype, memclass, ptrclass)        V_MEMRAM0 storage V_MEMRAM1 ptrtype V_MEMRAM2 *
#  define VSTD_DEF_P2VAR_TYPE(storage, ptrtype, ptrclass)             typedef storage V_MEMRAM1 ptrtype V_MEMRAM2 *
#  define VSTD_DEF_P2VAR_PARA(storage, ptrtype, memclass, ptrclass)   storage V_MEMRAM1 ptrtype V_MEMRAM2 *
#  define VSTD_DEF_CONSTP2VAR(storage, ptrtype, memclass, ptrclass)   V_MEMROM0 storage V_MEMRAM1 ptrtype V_MEMRAM2 V_MEMRAM3 * V_MEMROM1 V_MEMROM2
#  define VSTD_DEF_CONST(storage, vartype, memclass)                  V_MEMROM0 storage V_MEMROM1 vartype V_MEMROM2
#  define VSTD_DEF_CONST_TYPE(storage, vartype)                       typedef storage V_MEMROM1 vartype V_MEMROM2
#  define VSTD_DEF_P2CONST(storage, ptrtype, memclass, ptrclass)      V_MEMRAM0 storage V_MEMROM1 ptrtype V_MEMROM2 V_MEMROM3 * V_MEMRAM1 V_MEMRAM2
#  define VSTD_DEF_P2CONST_TYPE(storage, ptrtype, ptrclass)           typedef storage V_MEMROM1 ptrtype V_MEMROM2 V_MEMROM3 *
#  define VSTD_DEF_P2CONST_PARA(storage, ptrtype, memclass, ptrclass) storage V_MEMROM1 ptrtype V_MEMROM2 V_MEMROM3 * V_MEMRAM1 V_MEMRAM2
#  define VSTD_DEF_CONSTP2CONST(storage, ptrtype, memclass, ptrclass) V_MEMROM0 storage V_MEMROM1 ptrtype V_MEMROM2 V_MEMROM3 * V_MEMROM1 V_MEMROM2
#  define VSTD_DEF_FUNC(storage, rettype, memclass)                   storage rettype
#  define VSTD_DEF_FUNC_API(storage, rettype, memclass)               VSTD_API_0 storage VSTD_API_1 rettype
#  define VSTD_DEF_FUNC_CBK(storage, rettype, memclass)               storage VSTD_CALLBACK_0 rettype VSTD_CALLBACK_1

# endif


/* Types abstraction layer */
# if defined(VSTD_ENABLE_API_AUTOSAR) 
#  if !defined(vuint8)
#   define vuint8 uint8
#  endif
#  if !defined(vuint16)
#   define vuint16 uint16
#  endif
#  if !defined(vuint32)
#   define vuint32 uint32
#  endif
#  if !defined(vsint8)
#   define vsint8 sint8
#  endif
#  if !defined(vsint16)
#   define vsint16 sint16
#  endif
#  if !defined(vsint32)
#   define vsint32 sint32
#  endif
# endif



#  if defined(VSTD_ENABLE_DEFAULT_INTCTRL) || \
      defined(VSTD_ENABLE_OSEK_INTCTRL)    || \
      defined(VSTD_ENABLE_APPL_INTCTRL)
#   define VSTD_ENABLE_INTCTRL_HANDLING
#   define VSTD_HL_ENABLE_SUPPORT_IRQ_FCT  /* compatibility for Autosar */
#   define VSTD_ENABLE_SUPPORT_IRQ_FCT
#  else
#   define VSTD_DISABLE_INTCTRL_HANDLING
#   define VSTD_HL_DISABLE_SUPPORT_IRQ_FCT  /* compatibility for Autosar */
#   define VSTD_DISABLE_SUPPORT_IRQ_FCT
#  endif

# if !defined(VSTD_ENABLE_LIBRARY_FUNCTIONS) && !defined(VSTD_DISABLE_LIBRARY_FUNCTIONS)
#  define VSTD_ENABLE_LIBRARY_FUNCTIONS
# endif


/*********************************************************************************************************************/
/* Error codes for assertions                                                                                        */
/*********************************************************************************************************************/
# define kVStdErrorIntDisableTooOften        ((vuint8)0x01)
# define kVStdErrorIntRestoreTooOften        ((vuint8)0x02)
# define kVStdErrorMemClrInvalidParameter    ((vuint8)0x03)
# define kVStdErrorMemCpyInvalidParameter    ((vuint8)0x04)
# define kVStdErrorFunctionNotSupportedByHw  ((vuint8)0x05)
# define kVStdErrorMemSetInvalidParameter    ((vuint8)0x06)
# define kVStdErrorUnexpectedLockState       ((vuint8)0x07)


/*********************************************************************************************************************/
/* Plattforms with optimized VStdLib support                                                                         */
/*********************************************************************************************************************/
#define VSTD_ENABLE_SUPPORT_MEM_FCT
#if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
# define VSTD_ENABLE_SUPPORT_IRQ_FCT
#else
# define VSTD_DISABLE_SUPPORT_IRQ_FCT
#endif

/* in case of the standard implementation enable all functions */
#if defined(VSTD_ENABLE_LIBRARY_FUNCTIONS)
# define VSTD_HL_ENABLE_MEM_FCT_MAPPING
# define VSTD_HL_USE_VSTD_ROMMEMCPY
#else
# define VSTD_HL_USE_VSTD_MEMCLR
# define VSTD_HL_USE_VSTD_MEMSET
# define VSTD_HL_USE_VSTD_RAMMEMCPY
# define VSTD_HL_USE_VSTD_ROMMEMCPY
#endif

# define VSTD_HL_USE_VSTD_WORD_MEMCPY
# define VSTD_HL_USE_VSTD_DWORD_MEMCPY


/*********************************************************************************************************************/
/* Version check                                                                                                     */
/*********************************************************************************************************************/
# if defined(VSTD_ENABLE_API_AUTOSAR)
#  define VSTDLIB_START_SEC_CONST_UNSPECIFIED
#  include "MemMap.h"
# endif

/* ROM CATEGORY 4 START*/
VSTD_DEF_CONST(extern, vuint8, CONST) kVStdMainVersion;
VSTD_DEF_CONST(extern, vuint8, CONST) kVStdSubVersion;
VSTD_DEF_CONST(extern, vuint8, CONST) kVStdReleaseVersion;
/* ROM CATEGORY 4 END*/

# if defined(VSTD_ENABLE_API_AUTOSAR)
#  define VSTDLIB_STOP_SEC_CONST_UNSPECIFIED
#  include "MemMap.h"
# endif


/*********************************************************************************************************************/
/* Function like macros                                                                                              */
/*********************************************************************************************************************/

/*********************************************************************************************************************/
/* Types                                                                                                             */
/*********************************************************************************************************************/
typedef vuint8 tVStdIrqStateType;

/* this type exists because of compatibility */
typedef tVStdIrqStateType vIrqStateType;

/*********************************************************************************************************************/
/* Defines                                                                                                           */
/*********************************************************************************************************************/
#  if defined(VSTD_ENABLE_INTCTRL_HANDLING)
#   if defined(VSTD_ENABLE_DEFAULT_INTCTRL)
#    define VStdDeclareGlobalInterruptOldStatus     tVStdIrqStateType localInterruptOldStatus;
#    define VStdPutGlobalInterruptOldStatus(x)      (localInterruptOldStatus = (x))
#    define VStdGetGlobalInterruptOldStatus(x)      ((x) = localInterruptOldStatus)
#    define VStdGlobalInterruptDisable()            VStdLL_GlobalInterruptDisable(&localInterruptOldStatus)
#    define VStdGlobalInterruptRestore()            VStdLL_GlobalInterruptRestore(localInterruptOldStatus)
#   elif defined(VSTD_ENABLE_OSEK_INTCTRL)
#    define VStdDeclareGlobalInterruptOldStatus
#    define VStdPutGlobalInterruptOldStatus(x)
#    define VStdGetGlobalInterruptOldStatus(x)
#    if defined(osdOSEK2_1)
#     define VStdGlobalInterruptDisable()           OsSaveDisableGlobalNested()
#     define VStdGlobalInterruptRestore()           OsRestoreEnableGlobalNested()
#     define VStdSuspendAllInterrupts()             OsSaveDisableGlobalNested()
#     define VStdResumeAllInterrupts()              OsRestoreEnableGlobalNested()
#    else
#     define VStdGlobalInterruptDisable()           SuspendAllInterrupts()
#     define VStdGlobalInterruptRestore()           ResumeAllInterrupts()
#     define VStdSuspendAllInterrupts()             SuspendAllInterrupts()
#     define VStdResumeAllInterrupts()              ResumeAllInterrupts()
#    endif
#   elif defined(VSTD_ENABLE_APPL_INTCTRL)
#    define VStdDeclareGlobalInterruptOldStatus
#    define VStdPutGlobalInterruptOldStatus(x)
#    define VStdGetGlobalInterruptOldStatus(x)
#    define VStdGlobalInterruptDisable()           VStdUserNestedDisable()
#    define VStdGlobalInterruptRestore()           VStdUserNestedRestore()
#    define VStdSuspendAllInterrupts()             VStdUserNestedDisable()
#    define VStdResumeAllInterrupts()              VStdUserNestedRestore()
#   else
#    error "Please check VStdLib interrupt configuration!"
#   endif
#   define VStdNestedGlobalInterruptDisable() VStdGlobalInterruptDisable()
#   define VStdNestedGlobalInterruptRestore() VStdGlobalInterruptRestore()
#  endif

# if defined(VSTD_ENABLE_API_AUTOSAR)
/* For AUTOSAR, mem mapping has to be disabled */

/* Default implementaion of macros */
#  if !defined(VStdMemCpy_Macro)
#   define VStdMemCpy_Macro(dst, src, len)  { uint16_least __i; for ( __i = 0x00u; __i < (len); ++__i) { (dst)[__i] = (src)[__i]; } }
#  endif

#  if !defined(VStdMemSet_Macro)
#   define VStdMemSet_Macro(dst, val, len)  { uint16_least __i; for ( __i = 0x00u; __i < (len); ++__i) { (dst)[__i] = (val); } }
#  endif

#  if !defined(VStdMemClr_Macro)
#   define VStdMemClr_Macro(dst, len)       { uint16_least __i; for ( __i = 0x00u; __i < (len); ++__i) { (dst)[__i] = 0x00u; } }
#  endif

#  if !defined(VStdMemCpyRom_Macro)
#   define VStdMemCpyRom_Macro VStdMemCpy_Macro
#  endif

/* Set the default copy macros either to macros or to functions */
#  if (VSTD_USE_MEMORY_MACROS == STD_ON)
#   define VStdMemCpy(dst, src, len)            VStdMemCpy_Macro((dst), (src), (len))
#   define VStdMemCpy16(dst, src, len)          VStdMemCpy_Macro((dst), (src), (len))
#   define VStdMemCpy32(dst, src, len)          VStdMemCpy_Macro((dst), (src), (len))
#   define VStdMemSet(dst, val, len)            VStdMemSet_Macro((dst), (val), (len))
#   define VStdMemClr(dst, len)                 VStdMemClr_Macro((dst), (len))
#   define VStdMemCpyRom(dst, src, len)         VStdMemCpyRom_Macro((dst), (src), (len))
#  else
#   define VStdMemCpy(dst, src, len)            VStdMemCpy_Func(  (VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))(dst), (VSTD_DEF_P2CONST_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))(src), (len))
#   define VStdMemCpy16(dst, src, len)          VStdMemCpy16_Func((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint16, AUTOMATIC, VAR_FAR))(dst), (VSTD_DEF_P2CONST_PARA(VSTD_NONE, vuint16, AUTOMATIC, VAR_FAR))(src), (len))
#   define VStdMemCpy32(dst, src, len)          VStdMemCpy32_Func((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint32, AUTOMATIC, VAR_FAR))(dst), (VSTD_DEF_P2CONST_PARA(VSTD_NONE, vuint32, AUTOMATIC, VAR_FAR))(src), (len))
#   define VStdMemSet(dst, val, len)            VStdMemSet_Func(  (VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))(dst), (val), (len))
#   define VStdMemClr(dst, len)                 VStdMemClr_Func(  (VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))(dst), (len))   
#     define VStdMemCpyRom(dst, src, len)       VStdMemCpy_Func(  (VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))(dst), (VSTD_DEF_P2CONST_PARA(VSTD_NONE, vuint8, AUTOMATIC, CONST_FAR))(src), (len))
#  endif /* VSTD_USE_MEMORY_MACROS */


/* Mapping for compatibility reasons */
/* memset abstraction */
#   define VStdMemNearSet(d, p, l)              VStdMemSet((d), (p), (l))
#   define VStdMemFarSet(d, p, l)               VStdMemSet((d), (p), (l))

/* memclr abstraction */
#   define VStdMemNearClr(d, l)                 VStdMemClr((d), (l))
#   define VStdMemFarClr(d, l)                  VStdMemClr((d), (l))

/* memcpy abstraction */
#   define VStdMemCpyRamToRam(d, s, l)          VStdMemCpy((d), (s), (l))
#   define VStdMemCpyRomToRam(d, s, l)          VStdMemCpyRom((d), (s), (l))
#   define VStdMemCpyRamToNearRam(d, s, l)      VStdMemCpy((d), (s), (l))
#   define VStdMemCpyRomToNearRam(d, s, l)      VStdMemCpyRom((d), (s), (l))
#   define VStdMemCpyRomToFarRam(d, s, l)       VStdMemCpyRom((d), (s), (l))
#   define VStdMemCpyRamToFarRam(d, s, l)       VStdMemCpy((d), (s), (l))
#   define VStdMemCpyFarRamToRam(d, s, l)       VStdMemCpy((d), (s), (l))
#   define VStdMemCpyFarRamToFarRam(d, s, l)    VStdMemCpy((d), (s), (l))
#   define VStdMemCpyFarRomToFarRam(d, s, l)    VStdMemCpyRom((d), (s), (l))

#    define VStdMemCpy16RamToRam(d, s, l)       VStdMemCpy16((d), (s), (l))
#    define VStdMemCpy16RamToFarRam(d, s, l)    VStdMemCpy16((d), (s), (l))
#    define VStdMemCpy16FarRamToRam(d, s, l)    VStdMemCpy16((d), (s), (l))

#    define VStdMemCpy32RamToRam(d, s, l)       VStdMemCpy32((d), (s), (l))
#    define VStdMemCpy32RamToFarRam(d, s, l)    VStdMemCpy32((d), (s), (l))
#    define VStdMemCpy32FarRamToRam(d, s, l)    VStdMemCpy32((d), (s), (l))




# else /* VSTD_ENABLE_API_AUTOSAR */
#  if defined(VSTD_HL_ENABLE_MEM_FCT_MAPPING)
/* define hardware specific mappings of memcpy etc. */




# if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
/* memset abstraction */
#  define VStdMemSet(d, p, l)                 (void)memset((void *)(d), (p), (vuint16)(l))
#  define VStdMemNearSet(d, p, l)             (void)memset((void *)(d), (p), (vuint16)(l))
#  define VStdMemFarSet(d, p, l)              (void)memset((void *)(d), (p), (vuint16)(l))

/* memclr abstraction */
#  define VStdMemClr(d, l)                    (void)memset((void *)(d), 0, (vuint16)(l))
#  define VStdMemNearClr(d, l)                (void)memset((void *)(d), 0, (vuint16)(l))
#  define VStdMemFarClr(d, l)                 (void)memset((void *)(d), 0, (vuint16)(l))

/* memcpy abstraction */
#  define VStdMemCpyRamToRam(d, s, l)         (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyRomToRam(d, s, l)         (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyRamToNearRam(d, s, l)     (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyRomToNearRam(d, s, l)     (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyRomToFarRam(d, s, l)      (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyRamToFarRam(d, s, l)      (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyFarRamToRam(d, s, l)      (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyFarRamToFarRam(d, s, l)   (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))
#  define VStdMemCpyFarRomToFarRam(d, s, l)   (void)memcpy((void *)(d), (void *)(s), (vuint16)(l))

/* 16-bit memcpy abstraction */
#  if defined(VSTD_HL_USE_VSTD_WORD_MEMCPY)
#   define VStdMemCpy16RamToRam(d, s, l)      VStdRamMemCpy16((void *)(d), (void *)(s), (vuint16)(l))
#   define VStdMemCpy16RamToFarRam(d, s, l)   VStdRamMemCpy16((void *)(d), (void *)(s), (vuint16)(l))
#   define VStdMemCpy16FarRamToRam(d, s, l)   VStdRamMemCpy16((void *)(d), (void *)(s), (vuint16)(l))
#  endif

/* 32-bit memcpy abstraction */
#  if defined(VSTD_HL_USE_VSTD_DWORD_MEMCPY)
#   define VStdMemCpy32RamToRam(d, s, l)      VStdRamMemCpy32((void *)(d), (void *)(s), (vuint16)(l))
#   define VStdMemCpy32RamToFarRam(d, s, l)   VStdRamMemCpy32((void *)(d), (void *)(s), (vuint16)(l))
#   define VStdMemCpy32FarRamToRam(d, s, l)   VStdRamMemCpy32((void *)(d), (void *)(s), (vuint16)(l))
#  endif
# endif

#  else /* VSTD_HL_ENABLE_MEM_FCT_MAPPING */

/* per default all memclr and memcpy functions are mapped on one routine which
   does not support different memory models */
/* memset abstraction */
#   define VStdMemSet(d, p, l)                 VStdMemSetInternal((void*)(d), (p), (vuint16)(l))
#   define VStdMemNearSet(d, p, l)             VStdMemSetInternal((void*)(d), (p), (vuint16)(l))
#   define VStdMemFarSet(d, p, l)              VStdMemSetInternal((void*)(d), (p), (vuint16)(l))

/* memclr abstraction */
#   define VStdMemClr(d, l)                    VStdMemClrInternal((void*)(d), (vuint16)(l))
#   define VStdMemNearClr(d, l)                VStdMemClrInternal((void*)(d), (vuint16)(l))
#   define VStdMemFarClr(d, l)                 VStdMemClrInternal((void*)(d), (vuint16)(l))

/* memcpy abstraction */
#   define VStdMemCpyRamToRam(d, s, l)         VStdRamMemCpy((void*)(d), (void*)(s), (vuint16)(l))
#   define VStdMemCpyRomToRam(d, s, l)         VStdRomMemCpy((void*)(d), (V_MEMROM1 void V_MEMROM2 V_MEMROM3 *)(s), (vuint16)(l))

#   define VStdMemCpyRamToNearRam(d, s, l)     VStdRamMemCpy((d), (void*)(s), (l))
#   define VStdMemCpyRomToNearRam(d, s, l)     VStdRomMemCpy((d), (V_MEMROM1 void V_MEMROM2 V_MEMROM3 *)(s), (vuint16)(l))

#   define VStdMemCpyRomToFarRam(d, s, l)      VStdRomMemCpy((d), (V_MEMROM1 void V_MEMROM2 V_MEMROM3 *)(s), (vuint16)(l))

#   define VStdMemCpyRamToFarRam(d, s, l)      VStdRamMemCpy((d), (s), (vuint16)(l))
#   define VStdMemCpyFarRamToRam(d, s, l)      VStdRamMemCpy((d), (s), (vuint16)(l))

#   define VStdMemCpyFarRamToFarRam(d, s, l)   VStdRamMemCpy((d), (s), (vuint16)(l))
#   define VStdMemCpyFarRomToFarRam(d, s, l)   VStdRomMemCpy((d), (s), (vuint16)(l))

#    define VStdMemCpy16RamToRam(d, s, l)      VStdRamMemCpy16((d), (s), (vuint16)(l))
#    define VStdMemCpy16RamToFarRam(d, s, l)   VStdRamMemCpy16((d), (s), (vuint16)(l))
#    define VStdMemCpy16FarRamToRam(d, s, l)   VStdRamMemCpy16((d), (s), (vuint16)(l))

#    define VStdMemCpy32RamToRam(d, s, l)      VStdRamMemCpy32((d), (s), (vuint16)(l))
#    define VStdMemCpy32RamToFarRam(d, s, l)   VStdRamMemCpy32((d), (s), (vuint16)(l))
#    define VStdMemCpy32FarRamToRam(d, s, l)   VStdRamMemCpy32((d), (s), (vuint16)(l))

#  endif /* VSTD_HL_ENABLE_MEM_FCT_MAPPING */
# endif
/*********************************************************************************************************************/
/* Compatibility                                                                                                     */
/*********************************************************************************************************************/
# if defined (VSTD_ENABLE_API_AUTOSAR)
# else
#  define VStdMemCpy(d,s,l)     VStdMemCpyRamToRam((d),(s),(l))
# endif

# if defined(VSTD_HL_USE_VSTD_RAMMEMCPY) && !defined(VSTD_ENABLE_API_AUTOSAR)
# else
#  define VStdRamMemCpy(d,s,l) VStdMemCpyRamToRam((d),(s),(l))
# endif

# if defined(VSTD_HL_USE_VSTD_ROMMEMCPY) && !defined(VSTD_ENABLE_API_AUTOSAR)
# else
#  define VStdRomMemCpy(d,s,l) VStdMemCpyRomToRam((d),(s),(l))
# endif

/*********************************************************************************************************************/
/* Prototypes of functions                                                                                           */
/*********************************************************************************************************************/

# if defined(VSTD_ENABLE_API_AUTOSAR)
#  define VSTDLIB_START_SEC_CODE
#  include "MemMap.h"
# endif

/* initialization */
/* CODE CATEGORY 4 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdInitPowerOn(void);
/* CODE CATEGORY 4 END*/


/* function for moving data */
# if defined(VSTD_ENABLE_API_AUTOSAR)
/* CODE CATEGORY 1 START */
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemClr_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pDest, uint16_least nCnt);
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemSet_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pDest, uint8 nPatter, uint16_least nCnt);
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemCpy_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pDest, VSTD_DEF_P2CONST_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pSrc, uint16_least nCnt);
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemCpy16_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint16, AUTOMATIC, VAR_FAR) pDest, VSTD_DEF_P2CONST_PARA(VSTD_NONE, uint16, AUTOMATIC, VAR_FAR) pSrc, uint16_least nCnt);
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemCpy32_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint32, AUTOMATIC, VAR_FAR) pDest, VSTD_DEF_P2CONST_PARA(VSTD_NONE, uint32, AUTOMATIC, VAR_FAR) pSrc, uint16_least nCnt);
/* CODE CATEGORY 1 END */
# else /* VSTD_ENABLE_API_AUTOSAR */
/* CODE CATEGORY 1 START*/
#  if defined(VSTD_HL_USE_VSTD_MEMCLR)
VSTD_API_0 void VSTD_API_1 VStdMemClrInternal(void *pDest, vuint16 nCnt) VSTD_API_2;
#  endif

#  if defined(VSTD_HL_USE_VSTD_MEMSET)
VSTD_API_0 void VSTD_API_1 VStdMemSetInternal(void *pDest, vuint8 nPattern, vuint16 nCnt) VSTD_API_2;
#  endif

#  if defined(VSTD_HL_USE_VSTD_RAMMEMCPY)
VSTD_API_0 void VSTD_API_1 VStdRamMemCpy(void *pDest, void *pSrc, vuint16 nCnt) VSTD_API_2;
#  endif

#  if defined(VSTD_HL_USE_VSTD_ROMMEMCPY)
VSTD_API_0 void VSTD_API_1 VStdRomMemCpy(void *pDest, V_MEMROM1 void V_MEMROM2 V_MEMROM3 *pSrc, vuint16 nCnt) VSTD_API_2;
# endif

#  if defined(VSTD_HL_USE_VSTD_WORD_MEMCPY)
VSTD_API_0 void VSTD_API_1 VStdRamMemCpy16(void *pDest, void *pSrc, vuint16 nWordCnt) VSTD_API_2;
# endif

#  if defined(VSTD_HL_USE_VSTD_DWORD_MEMCPY)
VSTD_API_0 void VSTD_API_1 VStdRamMemCpy32(void *pDest, void *pSrc, vuint16 nDWordCnt) VSTD_API_2;
#  endif
/* CODE CATEGORY 1 END*/

/* Declaration of additional memory manipulation functions */
# endif /* VSTD_ENABLE_API_AUTOSAR */

#  if defined(VSTD_ENABLE_INTCTRL_HANDLING)

/* interrupt control functions */
#   if defined(VSTD_ENABLE_DEFAULT_INTCTRL)

/* CODE CATEGORY 1 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdSuspendAllInterrupts(void) VSTD_API_2;
/* CODE CATEGORY 1 END*/

/* CODE CATEGORY 1 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdResumeAllInterrupts(void) VSTD_API_2;
/* CODE CATEGORY 1 END*/
#   endif

/* Declaration of additional Irq Support functions */
#if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
# if defined(VSTD_ENABLE_SUPPORT_IRQ_FCT)
/* CODE CATEGORY 1 START */
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdLL_GlobalInterruptDisable(VSTD_DEF_P2VAR_PARA(VSTD_NONE, tVStdIrqStateType, AUTOMATIC, VAR_NEAR) pOldState) VSTD_API_2;
/* CODE CATEGORY 1 END */
/* CODE CATEGORY 1 START */
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdLL_GlobalInterruptRestore(tVStdIrqStateType nOldState) VSTD_API_2;
/* CODE CATEGORY 1 END */
#  define VStdDisableInterrupt VStdLL_GlobalInterruptDisable
#  define VStdRestoreInterrupt VStdLL_GlobalInterruptRestore
# endif
#else
#endif
#  endif


/*********************************************************************************************************************/
/* Callback functions                                                                                                */
/*********************************************************************************************************************/
# if defined(VSTD_ENABLE_DEBUG_SUPPORT)
/* CODE CATEGORY 4 START*/
VSTD_DEF_FUNC_CBK(VSTD_NONE, void, CODE) ApplVStdFatalError(vuint8 nErrorNumber);
/* CODE CATEGORY 4 END*/
# endif

# if defined(VSTD_ENABLE_API_AUTOSAR)
#  define VSTDLIB_STOP_SEC_CODE
#  include "MemMap.h"
# endif


#endif
/************   Organi, Version 3.9.1 Vector-Informatik GmbH  ************/
