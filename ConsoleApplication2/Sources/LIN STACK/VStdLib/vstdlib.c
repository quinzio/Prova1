/* Kernbauer Version: 1.16 Konfiguration: VStdLib Erzeugungsgangnummer: 1 */


/**********************************************************************************************************************
| Project Name: vstdlib library
|    File Name: vstdlib_ll.c
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
|2006-03-27  01.03.00    Hs     - ESCAN00015838: no changes
|2006-03-29  01.04.00    Hs     - ESCAN00015881: no changes
|2006-06-19  01.05.00    Hs     - ESCAN00016656: no changes
|2006-07-13  01.06.00    Ml     - Using build-in copy functions for Metrowerks
|2006-08-04  01.07.00    Ml     - Added Mcs12 and compatibility for older VStdLib functions
|2006-08-12  01.08.00    Ml     - no changes
|2006-10-26  01.09.00    Ml     - Added encapsulation for different platforms
|2006-12-12  01.10.00    Ces    - Added support for MCS12 derivative
|                        Ces    - Added support for IAR compiler
|2007-02-21  01.11.00    Ml     - New HL template - changed handling of interrupt lock functions
|2007-02-28  01.12.00    Ml     - ESCAN00019768: Added support for IAR compiler for HC12Idx target
|2007-05-10  01.13.00    Ml     - HL 2.17.00
|                        Ml     - Added default implementation for VStdRomMemCpy
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
|    File Name : vstdlib_HL.c
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
|2004-05-17  01.01.00    Ml     - added assertion
|                        Ml     - added define section
|                        Ml     - changed memoryqualifier
|                        Ml     - changed bugfix into releaseversion
|                        Ml     - added check for correct v_def.h version
|2004-05-18  01.02.00    Ms/HH  - changed memory qualifier concept
|2004-06-01  01.03.00    Ml     - corrected define for IRQ functions
|2004-08-06  01.04.00    Ml     - Split of library into HL and LL
|2004-08-18  01.05.00    Ml     - Added function qualifier
|2004-09-06  01.06.00    Ml     - ESCAN00009316: no changes
|2004-09-27  01.07.00    Ml     - no changes
|                        Ml     - changed namingconvention     
|2004-09-29  01.08.00    Et     - ESCAN00009692: split VStdMemCpy in VStdRamMemCpy and VStdRomMemCpy   
|2004-10-01  01.09.00    Ml     - ESCAN00009731: usage of standard type qualifiers
|2004-10-26  01.10.00    Ml     - ESCAN00010000: LL parts are split into HW specific files
|2004-11-09  01.11.00    Ml     - added VStd_LLPostInclude
|2004-11-15  01.12.00    Ml     - ESCAN00010533: Added optimized memory functions for HC08
|                        Ml     - ESCAN00010672: Added memory qualifier
|2005-01-24  01.13.00    Ml     - ESCAN00010670: Added memset
|2005-02-25  01.14.00    Ml     - ESCAN00011215: Added VStdInitPowerOn
|2005-04-26  01.15.00    Ml     - ESCAN00012047: Encapsulate VStdInitPowerOn
|            01.16.00    Ml     - ESCAN00012105: no changes
|2005-05-30  02.00.00    Ml     - ESCAN00013446: Development concept changed
|2005-09-12  02.01.00    Ml     - ESCAN00013512: Fixed Misra warnings
|2005-11-06  02.02.00    Ml     - ESCAN00014183: Added token for defines and types
|2005-11-28  02.03.00    Ml     - Changed version defines
|                        Ml     - ESCAN00014428: VStdMemClr/Set are now internal functions
|                        Ml     - ESCAN00014409: No changes       
|2005-12-12  02.04.00    Ml/Bus - ESCAN00012774: insert comments for different resource categories
|2006-01-12  02.05.00    Ml     - ESCAN00014897: Changed interrupt handling
|2006-01-14  02.06.00    Ml     - ESCAN00014908: Adapted interrupt control handling for RI1.5
|2006-02-07  02.07.00    Ml     - Added VStdGetInterruptState
|                        Hs     - ESCAN00015802: no changes
|2006-05-09  02.08.00    Ces    - ESCAN00016289: Insert memory qualifier V_MEMROM3 to VStdRomMemCpy()
|2006-05-24  02.09.00    BWR    - ESCAN00016488: Compiler error in VStdSuspendAllInterrupts and VStdResumeAllInterrupts
|2006-07-16  02.10.00    Ml     - Changed return type of VStdGetInterruptState from uint8 to tVStdIrqStateType
|2006-07-31  02.11.00    Ml     - ESCAN00017133: Changed assertion handling
|2006-08-04  02.12.00    Ml     - ESCAN00017188: no changes
|2006-11-20  02.13.00    Ml     - Removed VStdGetInterruptState
|2006-12-07  02.14.00    Ml     - Changed switch names
|                        Ml     - ESCAN00018889: Fixed code inspection report findings
|                               - ESCAN00018880: No changes
|2007-01-27  02.15.00    Ml,Ard - ESCAN00019251: VStdLib HL usable for RI1.4 and RI1.5
|2007-02-19  02.16.00    Ml     - ESCAN00019672: include os.h for AUTOSAR
|                        Ml     - ESCAN00019674: Changed names of interrupt lock macros
|2007-05-10  02.17.00    Ml     - ESCAN00020705: Added _COMMENT switches for single source filtering
|                        Ml     - ESCAN00020241: No changes
|2007-11-26  03.00.00    Ml     - ESCAN00024126: Support word and dword copy routines
|                        Ml     - ESCAN00024192: Default and specific implementations supported at the same time
|2008-06-25  03.01.00    Ml     - ESCAN00023334: Added u suffix to the version numbers
|                        Ml     - ESCAN00018659: Removed warnings if interrupt counter not used
|                        Ml     - ESCAN00015501: No changes
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

#define _VSTD_LIB_C_

#include "vstdlib.h"

#if defined(V_OSTYPE_AUTOSAR)
# include "Os.h"
#elif defined(V_OSTYPE_OSEK)
# include "osek.h"
#endif

#if defined(VSTD_ENABLE_API_AUTOSAR)
# if !defined(vsintx)
#  define vsintx sint8_least
# endif
#endif

/* MICROSAR library version checks */
#if defined(VSTD_ENABLE_MSR_LIB_VERSION_CHECK)
# include "Std_Types.h"
# if !defined(V_SUPPRESS_EXTENDED_VERSION_CHECK)
#  include "v_ver.h"
# endif
#endif

/*********************************************************************************************************************/
/* Version check                                                                                                     */
/*********************************************************************************************************************/


#if(VSTDLIB_MCS12X_VERSION != 0x0120u)
# error "Lowlevel Source and Header files are inconsistent!"
#endif

#if(VSTDLIB_MCS12X_RELEASE_VERSION != 0x00u)
# error "Lowlevel Source and Header file are inconsistent!"
#endif

/* MICROSAR library version check */
#define VSTDLIB_LIB_VERSION  VSTDLIB_MCS12X_LIB_VERSION


/* Highlevel versioncheck */
#if(VSTDLIB__COREHLL_VERSION != 0x0307u)
# error "HighLevel Source and Header files are inconsistent!"
#endif

#if(VSTDLIB__COREHLL_RELEASE_VERSION != 0x00u)
# error "HighLevel Source and Header file are inconsistent!"
#endif

/* V_def.h Compatibility Check */
#if !defined(VSTD_ENABLE_API_AUTOSAR)
# if(COMMON_VDEF_VERSION < 0x0220u)
#  error "VstdLib requires at least v_def.h in version 2.20.00!"
# endif
#endif


/* Checks relevant for MICROSAR stack */
#if defined(VSTD_ENABLE_API_AUTOSAR)

/* MICROSAR library version checks */
# if defined(VSTD_ENABLE_MSR_LIB_VERSION_CHECK)
#  if !defined(V_SUPPRESS_EXTENDED_VERSION_CHECK)
#   if !defined(VSTDLIB_LIB_VERSION)
#    error "Make sure VSTDLIB_LIB_VERSION is defined in VStd_LLVersionCheck label!"
#   endif
CONST(uint32, VSTDLIB_CONST) VSTDLIB_LIB_SYMBOL = VSTDLIB_LIB_VERSION;
#  endif /* !V_SUPPRESS_EXTENDED_VERSION_CHECK */
# endif /* VSTD_ENABLE_MSR_LIB_VERSION_CHECK */

/* Additional checks for microsar */
# if !defined(VSTD_USE_MEMORY_MACROS)
#  error "VSTD_USE_MEMROY_MACROS STD_[ON|OFF] must be defined in the Compiler_Cfg.h"
# endif

#endif /* VSTD_ENABLE_API_AUTOSAR */


/* Additional checks */
#if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
#else
# error "Platform / Compiler not supported!"
#endif

/*********************************************************************************************************************/
/* Constants                                                                                                         */
/*********************************************************************************************************************/
#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_START_SEC_CONST_UNSPECIFIED
# include "MemMap.h"
#endif
/* ROM CATEGORY 4 START*/
VSTD_DEF_CONST(VSTD_NONE, vuint8, CONST) kVStdMainVersion    = (vuint8)(((vuint16)VSTDLIB_MCS12X_VERSION) >> 8);
VSTD_DEF_CONST(VSTD_NONE, vuint8, CONST) kVStdSubVersion     = (vuint8)(((vuint16)VSTDLIB_MCS12X_VERSION) & 0x00FF);
VSTD_DEF_CONST(VSTD_NONE, vuint8, CONST) kVStdReleaseVersion = (vuint8)( (vuint16)VSTDLIB_MCS12X_RELEASE_VERSION);
/* ROM CATEGORY 4 END*/
#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_STOP_SEC_CONST_UNSPECIFIED
# include "MemMap.h"
#endif

/*********************************************************************************************************************/
/* Local Types                                                                                                       */
/*********************************************************************************************************************/
/* RAM CATEGORY 1 START*/
/* RAM CATEGORY 1 END*/

/*********************************************************************************************************************/
/* Defines                                                                                                           */
/*********************************************************************************************************************/
/* assembler does not accept static variables */
#define VSTD_STATIC_DECL

#if !defined(VSTD_STATIC_DECL)
# define VSTD_STATIC_DECL static
#endif 

/*********************************************************************************************************************/
/* Local data definitions                                                                                            */
/*********************************************************************************************************************/
#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_START_SEC_VAR_NOINIT_UNSPECIFIED
# include "MemMap.h"
#endif

# if defined(VSTD_ENABLE_INTCTRL_HANDLING)
#  if defined(VSTD_ENABLE_DEFAULT_INTCTRL)
/* RAM CATEGORY 1 START*/
VSTD_DEF_VAR(VSTD_STATIC_DECL, tVStdIrqStateType, VAR_NOINIT) vstdInterruptOldStatus; /* old IRQ state, variable not static because of some inline asm!! */
/* RAM CATEGORY 1 END*/
#  endif
#  if defined(VSTD_ENABLE_DEFAULT_INTCTRL)
/* RAM CATEGORY 1 START*/
VSTD_DEF_VAR(STATIC, vsintx, VAR_NOINIT) vstdInterruptCounter;   /* interrupt counter */
/* RAM CATEGORY 1 END*/
#  endif
# endif


#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_STOP_SEC_VAR_NOINIT_UNSPECIFIED
# include "MemMap.h"
#endif

/*********************************************************************************************************************/
/* Local function declarations                                                                                       */
/*********************************************************************************************************************/
#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_START_SEC_CODE
# include "MemMap.h"
#endif


#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_STOP_SEC_CODE
# include "MemMap.h"
#endif

/*********************************************************************************************************************/
/* Macros                                                                                                            */
/*********************************************************************************************************************/

#if defined(VSTD_ENABLE_DEBUG_SUPPORT)
# define VStdAssert(p,e)       if(!(p)){ ApplVStdFatalError(e); }
#else
# define VStdAssert(p,e)
#endif

/*********************************************************************************************************************/
/* Functions                                                                                                         */
/*********************************************************************************************************************/
#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_START_SEC_CODE
# include "MemMap.h"
#endif

/*********************************************************************************************************************
Name          : VStdInitPowerOn
Called by     : init routine
Preconditions : none
Parameters    : none
Return values : none
Description   : initializes the global data of the VStdLib
*********************************************************************************************************************/
/* CODE CATEGORY 4 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdInitPowerOn(void)
{

# if defined(VSTD_ENABLE_INTCTRL_HANDLING)
#  if defined(VSTD_ENABLE_DEFAULT_INTCTRL)
  vstdInterruptOldStatus = (tVStdIrqStateType)0;
#  endif
#  if defined(VSTD_ENABLE_DEFAULT_INTCTRL)
  vstdInterruptCounter = (vsintx)0;
#  endif
# endif
  
}
/* CODE CATEGORY 4 END*/


#if defined(VSTD_ENABLE_API_AUTOSAR)
/* CODE CATEGORY 1 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemClr_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pDest, uint16_least nCnt)
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemClrInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {  
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pDest)[nCnt] = (vuint8)0x00;
    } 
}

VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemSet_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pDest, uint8 nPattern, uint16_least nCnt)
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemSetInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {  
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pDest)[nCnt] = nPattern;
    }
}

VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemCpy_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pDest, VSTD_DEF_P2CONST_PARA(VSTD_NONE, uint8, AUTOMATIC, VAR_FAR) pSrc, uint16_least nCnt)
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    VStdAssert(pSrc!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {  
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pDest)[nCnt] = ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pSrc)[nCnt];
    } 
}

VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemCpy16_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint16, AUTOMATIC, VAR_FAR) pDest, VSTD_DEF_P2CONST_PARA(VSTD_NONE, uint16, AUTOMATIC, VAR_FAR) pSrc, uint16_least nCnt)
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    VStdAssert(pSrc!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint16, AUTOMATIC, VAR_FAR))pDest)[nCnt] = ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint16, AUTOMATIC, VAR_FAR))pSrc)[nCnt];
    } 
}

VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdMemCpy32_Func(VSTD_DEF_P2VAR_PARA(VSTD_NONE, uint32, AUTOMATIC, VAR_FAR) pDest, VSTD_DEF_P2CONST_PARA(VSTD_NONE, uint32, AUTOMATIC, VAR_FAR) pSrc, uint16_least nCnt)
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    VStdAssert(pSrc!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint32, AUTOMATIC, VAR_FAR))pDest)[nCnt] = ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint32, AUTOMATIC, VAR_FAR))pSrc)[nCnt];
    } 
}


/* CODE CATEGORY 1 END*/
#else /* VSTD_ENABLE_API_AUTOSAR */
# if defined(VSTD_HL_USE_VSTD_MEMSET)
/*********************************************************************************************************************
Name          : VStdMemSetInternal
Called by     : -
Preconditions : -
Parameters    : void*   pDest    : destination pointer          
                vuint8  nPattern : the value which is used for filling
                vuint16 nCnt     : number of bytes to be filled 
Return values : none
Description   : fills nCnt bytes from address pDest on with the nPattern value
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_API_0 void VSTD_API_1 VStdMemSetInternal(void *pDest, vuint8 nPattern, vuint16 nCnt) VSTD_API_2
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemSetInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {  
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pDest)[nCnt] = nPattern;
    }
}
/* CODE CATEGORY 1 END*/
# endif

# if defined(VSTD_HL_USE_VSTD_MEMCLR)
/*********************************************************************************************************************
Name          : VStdMemClr
Called by     : -
Preconditions : -
Parameters    : void*   pDest : destination pointer          
                vuint16 nCnt  : number of bytes to be filled 
Return values : none
Description   : fills nCnt bytes from address pDest on with 0x00 pattern
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_API_0 void VSTD_API_1 VStdMemClrInternal(void *pDest, vuint16 nCnt) VSTD_API_2
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemClrInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {  
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pDest)[nCnt] = (vuint8)0x00;
    } 
}
/* CODE CATEGORY 1 END*/
# endif
 
# if defined(VSTD_HL_USE_VSTD_RAMMEMCPY)
/*********************************************************************************************************************
Name          : VStdRamMemCpy
Called by     : -
Preconditions : -
Parameters    : void*   pDest : RAM destination pointer          
                void*   pSrc  : RAM source pointer               
                vuint16 nCnt  : number of bytes to be copied 
Return values : none
Description   : moves nCnt bytes from RAM pSrc to RAM pDest
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_API_0 void VSTD_API_1 VStdRamMemCpy(void *pDest, void* pSrc, vuint16 nCnt) VSTD_API_2
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    VStdAssert(pSrc!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {  
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pDest)[nCnt] = ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pSrc)[nCnt];
    } 
}
/* CODE CATEGORY 1 END*/
# endif

# if defined(VSTD_HL_USE_VSTD_ROMMEMCPY)
/*********************************************************************************************************************
Name          : VStdRomMemCpy
Called by     : -
Preconditions : -
Parameters    : void*   pDest : ROM destination pointer          
                void*   pSrc  : ROM source pointer               
                vuint16 nCnt  : number of bytes to be copied 
Return values : none
Description   : moves nCnt bytes from ROM pSrc to RAM pDest
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_API_0 void VSTD_API_1 VStdRomMemCpy(void *pDest, V_MEMROM1 void V_MEMROM2 V_MEMROM3 *pSrc, vuint16 nCnt) VSTD_API_2
{  
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    VStdAssert(pSrc!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    while(nCnt > (vuint16)0x0000)
    {
      nCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pDest)[nCnt] = ((VSTD_DEF_P2CONST_PARA(VSTD_NONE, vuint8, AUTOMATIC, VAR_FAR))pSrc)[nCnt];
    } 
}
/* CODE CATEGORY 1 END*/
# endif

# if defined(VSTD_HL_USE_VSTD_WORD_MEMCPY)
/*********************************************************************************************************************
Name          : VStdRamMemCpy16
Called by     : -
Preconditions : -
Parameters    : void*   pDest    : RAM destination pointer
                void*   pSrc     : RAM source pointer
                vuint16 nWordCnt : Number of words to be copied
Return values : none
Description   : moves nWordCnt words from the source RAM address to the destination RAM address
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_API_0 void VSTD_API_1 VStdRamMemCpy16(void *pDest, void *pSrc, vuint16 nWordCnt) VSTD_API_2
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    VStdAssert(pSrc!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    while(nWordCnt > (vuint16)0x0000)
    {
      nWordCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint16, AUTOMATIC, VAR_FAR))pDest)[nWordCnt] = ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint16, AUTOMATIC, VAR_FAR))pSrc)[nWordCnt];
    } 
}
/* CODE CATEGORY 1 END*/
# endif

# if defined(VSTD_HL_USE_VSTD_DWORD_MEMCPY)
/*********************************************************************************************************************
Name          : VStdRamMemCpy32
Called by     : -
Preconditions : -
Parameters    : void*   pDest    : RAM destination pointer
                void*   pSrc     : RAM source pointer
                vuint16 nDWordCnt: Number of dwords to be copied
Return values : none
Description   : moves nDWordCnt dwords from the source RAM address to the destination RAM address
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_API_0 void VSTD_API_1 VStdRamMemCpy32(void *pDest, void *pSrc, vuint16 nDWordCnt) VSTD_API_2
{
    /* Defaultimplementation */
    VStdAssert(pDest!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    VStdAssert(pSrc!=V_NULL, kVStdErrorMemCpyInvalidParameter);
    while(nDWordCnt > (vuint16)0x0000)
    {
      nDWordCnt--;
      ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint32, AUTOMATIC, VAR_FAR))pDest)[nDWordCnt] = ((VSTD_DEF_P2VAR_PARA(VSTD_NONE, vuint32, AUTOMATIC, VAR_FAR))pSrc)[nDWordCnt];
    } 
}
/* CODE CATEGORY 1 END*/
# endif

#endif /* VSTD_ENABLE_API_AUTOSAR */


#if defined(VSTD_ENABLE_API_AUTOSAR)
#else
/*********************************************************************************************************************/
/* Additional functions for memory manipulation                                                                      */
/*********************************************************************************************************************/

#endif

/*********************************************************************************************************************
The functions below require a plattform specific implementation, therefore 
they are not available for the standard implementation of the library!
*********************************************************************************************************************/
# if defined(VSTD_ENABLE_INTCTRL_HANDLING)
#  if defined(VSTD_ENABLE_DEFAULT_INTCTRL)
/*********************************************************************************************************************
Name          : VStdSuspendAllInterrupts
Called by     : -
Preconditions : -
Parameters    : none
Return values : none
Description   : disables the global IRQ and saves the current IRQ state
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdSuspendAllInterrupts(void) VSTD_API_2
{
  VStdDeclareGlobalInterruptOldStatus
#    if defined(VSTD_ENABLE_OSEK_INTCTRL)
  VStdGlobalInterruptDisable();
  VStdGetGlobalInterruptOldStatus(vstdInterruptOldStatus);
#    else
  VStdAssert(vstdInterruptCounter < 127, kVStdErrorIntDisableTooOften);
  if(vstdInterruptCounter == 0) 
  {
    VStdGlobalInterruptDisable();
    VStdGetGlobalInterruptOldStatus(vstdInterruptOldStatus);
  }
  vstdInterruptCounter++;
#    endif
}
/* CODE CATEGORY 1 END*/

/*********************************************************************************************************************
Name          : VStdResumeAllInterrupts
Called by     : -
Preconditions : -
Parameters    : none
Return values : none
Description   : restores the previous IRQ state
*********************************************************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE)  VStdResumeAllInterrupts(void) VSTD_API_2
{
  VStdDeclareGlobalInterruptOldStatus
#    if defined(VSTD_ENABLE_OSEK_INTCTRL)
  VStdPutGlobalInterruptOldStatus(vstdInterruptOldStatus);
  VStdGlobalInterruptRestore();
#    else
  VStdAssert(vstdInterruptCounter > 0, kVStdErrorIntRestoreTooOften);
  vstdInterruptCounter--;
  if(vstdInterruptCounter == 0)
  {
    VStdPutGlobalInterruptOldStatus(vstdInterruptOldStatus);
    VStdGlobalInterruptRestore();
  }
#    endif
}
/* CODE CATEGORY 1 END*/
#  endif

/*********************************************************************************************************************/
/* Additional functions for interrupt manipulation                                                                   */
/*********************************************************************************************************************/

#if defined(VSTD_ENABLE_SUPPORT_IRQ_FCT)
/***************************************************************************/
/* Additional functions for interrupt manipulation                         */
/***************************************************************************/
# if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
/****************************************************************************
Name          : VStdLL_GlobalInterruptDisable
Called by     : -
Preconditions : -
Parameters    : tVStdIrqStateType* pOldState: pointer to the local irq state
Return values : none
Description   : saves the current CCR value to *pOldState and disables the
                global interrupt
****************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdLL_GlobalInterruptDisable(VSTD_DEF_P2VAR_PARA(VSTD_NONE, tVStdIrqStateType, AUTOMATIC, VAR_NEAR) pOldState) VSTD_API_2
{

#  if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
  asm{
#  endif

  pshy      ; save index register
  tfr d,y   ; transfer pointer to index register
  tpa       ; load ccr to accumulator
  sei       ; disable global interrupt
  staa 0,y  ; store the value of ccr into loction pointed by pOldState
  puly      ; restore index register
  
#  if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
  }
#  endif
}
/* CODE CATEGORY 1 END*/

/****************************************************************************
Name          : VStdLL_GlobalInterruptRestore
Called by     : -
Preconditions : -
Parameters    : tVStdIrqStateType nOldState: value of the local irq state
Return values : none
Description   : restores the CCR value stored in nOldState 
****************************************************************************/
/* CODE CATEGORY 1 START*/
VSTD_DEF_FUNC_API(VSTD_NONE, void, CODE) VStdLL_GlobalInterruptRestore(tVStdIrqStateType nOldState) VSTD_API_2
{
#  if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
  asm{
#  endif
  tba       ; b to a
  tap       ; load accumulator to ccr
#  if defined(C_COMP_MTRWRKS_MCS12X_MSCAN12) 
  }
#  endif
}
/* CODE CATEGORY 1 END*/

# endif
#endif /* VSTD_SUPPORT_IRQ_FCT */

# endif

#if defined(VSTD_ENABLE_API_AUTOSAR)
# define VSTDLIB_STOP_SEC_CODE
# include "MemMap.h"
#endif

/************   Organi, Version 3.9.1 Vector-Informatik GmbH  ************/
