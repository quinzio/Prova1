/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_IsrTableS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_IsrTableS12.c
    \version see \ref FMW_VERSION 
    \brief Interrupt table.
    \details Routines and definition for interrupt table.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif
#include "hidef.h"
#include "start12.h"
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include MAC0_Register
#include SPAL_DEF
#include "INT0_RTIIsrS12.h"        /* Needed for INT0_isrVrti function */
#include "INT0_InputPWMIsrS12.h"	 /* Needed for INT0_InputPWM_PWMInputISR function */
#include "INT0_InputANPWMIsrS12.h" /* Needed for INT0_InputAN_PWMInputISR function */
#include "SCI.h"                   /* Needed for Vsci1txISR and Vsci1rxISR functions */
#include "INP1_InputPWM.h"         /* Needed for INT0_InputPWM_PWMInputISR function */
#include "INP1_InputAN.h"          /* Needed for INT0_InputAN_PWMInputISR function */
#include INT0_PWMIsr               /* Needed for INT0_dpwmmin_modulation function */
#include INT0_TachoIsr             /* Needed for INT0_TachoOverflow and INT0_TachoCapture functions */
#include INT0_DIAGIsr              /* Needed for INT0_DiagnosticIsr functions */
#include "MCN1_acmotor.h"	         /* It has to stand before MCN1_mtc.h because of StartStatus_t definition */
#include "MCN1_mtc.h"	  	         /* Needed for MTC_LVDDetection function */
#include "EMR0_Emergency.h"
#include "INT0_APIIsrS12.h" 
#include "main.h"                  /* Per strDebug */
#include "PARAM1_Param.h"          /* Per indirizzi RAM */
#ifdef LIN_INPUT
  #include "v_cfg.h"               /* Per non avere errore su direttiva V_DEF_SUPPORTED_PLATFORM */
  #include "v_def.h"               /* Per non avere errore su define V_MEMROM0 */
  #include "sio_cfg.h"             /* Per non avere warning sulle chiamate l_ifc_rx_0 e l_ifc_measure */
  #include "sio_drv.h"             /* Per non avere warning sulle chiamate l_ifc_rx_0 e l_ifc_measure */
  
  #if defined(BMW_LIN_ENCODING)
    #include "COM1_LINUpdate_BMW.h"
  #elif defined(GM_LIN_ENCODING)
    #include "COM1_LINUpdate_GM.h"
  #else
    #error("Define LIN stack!!");
  #endif  /* BMW_LIN_ENCODING */
      
#endif  /* LIN_INPUT */
#include "WTD1_Watchdog.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* -------------------------- Private typedefs ----------------------------- */

/* -------------------------- Private variables ---------------------------- */
/** 
    \fn void _Startup(void)
    \author \b Freescale \b Semiconductor \n <em>Standard Library</em>  
    \author	Pasquale Rubini  \n <em>Reviewer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Standard library for micro boot
    \details Standard extern library used for micro boot. This is a library from Freescale.\n
                            \par Used Variables
    \return \b void no value return
    \note None.
    \warning None
*/
/*extern void near _Startup(void);*/ /* Removed and replaced with the inclusion of start12.h file */


/* --------------------------- Private variables ----------------------------- */

/* --------------------------- Private functions ----------------------------- */

/* --------------------------- Private macros -------------------------------- */

static void INT0_MCU_init_reset(void);

/*-----------------------------------------------------------------------------*/
/** 
    \fn static void INT0_MCU_init_reset(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief Device initialization code for after reset initialization.  
    \details Device initialization code for after reset initialization.\n
                            \par Used Variables
    \return \b void no value return
    \note None.
    \warning None
*/

static void INT0_MCU_init_reset(void) /*PRQA S 3006 #Due to the necessary use of DisableInterrupts function written in assembly code*/
{
  u8 *stack_ptr;

#if(defined(WTD_TEST) || defined(RST_MON))
  SET_DEBUG2_OUT;
  DEBUG2_ON;
#endif

  DisableInterrupts; /*PRQA S 1006 #Necessary assembly code*/

  /*PRQA S 0306,0488 ++ #mandatory stack management*/
  stack_ptr = ((u8*)STACK_START);

  while(stack_ptr != ((u8*)STACK_END))
  {
    *(stack_ptr) = 0;
    stack_ptr++;
  }
  /*PRQA S 0306,0488 --*/

  /*  Initialization of memory configuration */
  /* MMCCTL1: NVMRES=0 */
  MMCCTL1 = 0U;                                      
  /* PKGCR: APICLKS7=0,PKGCR2=0,PKGCR1=0,PKGCR0=1 */
  PKGCR = 1U;                                      
  /* DIRECT: DP15=0,DP14=0,DP13=0,DP12=0,DP11=0,DP10=0,DP9=0,DP8=0 */
  DIRECT = 0x3C;                                      
  /* IVBR: IVB_ADDR=255 */
  #if (BOOTLOAD_VERS == BOOTLOADER_LATER_20) 
    IVBR = 0xEF;    /* Value for new interrupt vector table. */
  #endif                                      
  /* ECLKCTL: NECLK=1,NCLKX2=1,DIV16=0,EDIV4=0,EDIV3=0,EDIV2=0,EDIV1=0,EDIV0=0 */
  ECLKCTL = 192U;                                      
  /* Jump to the default entry point */
  /*lint -save  -e950 Disable MISRA rule (1.1) checking. */

#if(defined(WTD_TEST) || defined(RST_MON))
  DEBUG2_OFF;
#endif

#ifndef _CANTATA_
  asm jmp _Startup; /*PRQA S 1006 #Necessary assembly code*/
#endif
  /*lint -restore Enable MISRA rule (1.1) checking. */
} /*MCU_init*/

  
/*lint -restore Enable MISRA rule (8.10) checking. */

/*lint -save  -e950 Disable MISRA rule (1.1) checking. */
/* Initialization of the CPU registers in FLASH */

/* #define F_B_PROTECTED   0x00 */
/* #define F_B_UNPROTECTED 0x02 */

/** 
    \var NVFSEC_INIT
    \brief Non Volatile Memory Protection Initialization 
    \details Initialization of one of two micro register for non volatile memory protection
    \note none
    \warning none.
*/
/* NVFSEC: KEYEN1=0,KEYEN0=1,RNV5=1,RNV4=1,RNV3=1,RNV2=1,SEC1=0,SEC0=0 */
#ifdef _CANTATA_
static const u8 NVFSEC_INIT = 126U;
#else
static const u8 NVFSEC_INIT @0x0000FF0FU = 126U; /*PRQA S 0289, 1019 #mandatory for linker behavior*/
#endif

/*lint -restore Enable MISRA rule (1.1) checking. */


#pragma MESSAGE DEFAULT C12056

/*lint -save  -e765 Disable MISRA rule (8.10) checking. */
#pragma CODE_SEG __NEAR_SEG NON_BANKED

#if(defined(LIN_INPUT))
  __interrupt void INT0_isr_DispatcherSerialCom(void)
  {
    l_ifc_rx_0();
  }

  __interrupt void INT0_isr_DispatcherLinMeas(void)
  {
    l_ifc_measure();
    u16NumLinBusIdle=0;
  }
#endif /* LIN_INPUT */


/*
** ===================================================================
**     Interrupt handler : INT0_isr_default
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
static __interrupt void INT0_isr_default(void)
{

  EMR_EmergencyDisablePowerStage();
  RESET_MICRO;

  /* Write your interrupt code here ... */

}
/* end of INT0_isr_default */


/*
** ===================================================================
**     Interrupt handler : INT0_isr_unimplemented_instruction
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
static __interrupt void INT0_isr_unimplemented_instruction(void) /* IP EAE77 - PC AE77*/
{

  EMR_EmergencyDisablePowerStage();
  RESET_MICRO;

}
/* end of INT0_isr_unimplemented_instruction */

/*
** ===================================================================
**     Interrupt handler : INT0_isr_swi_instruction
**
**     Description :
**         User interrupt service routine. 
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
static __interrupt void INT0_isr_swi_instruction(void)
{

  EMR_EmergencyDisablePowerStage();
  RESET_MICRO;

}
/* end of INT0_isr_swi_instruction */


/* Interrupt vector table */
/** 
    \var _InterruptVectorTable
    \brief Interrupt vector Table
    \details Interrupt vector Table used to redirect interrupt service request to respective function.
    This is an array of function pointer, placed in the location FFC6 to place pointers in the correct 
    location.
    \note none
    \warning none.
*/

/* ISR prototype */
typedef void (*near tIsrFunc)(void);

#ifndef UNASSIGNED_ISR
  #define UNASSIGNED_ISR INT0_isr_default   /* unassigned interrupt service routine */
#endif
#ifndef UNIMPLEMENTED_INSTRUCTION_ISR
  #define UNIMPLEMENTED_INSTRUCTION_ISR INT0_isr_unimplemented_instruction   /* unimplemented instruction interrupt service routine */
#endif
#ifndef SWI_INSTRUCTION_ISR
  #define SWI_INSTRUCTION_ISR INT0_isr_swi_instruction                       /* swi instruction interrupt service routine */
#endif
/*lint -save  -e950 Disable MISRA rule (1.1) checking. */

/* Interrupt vector table */
static const tIsrFunc InterruptVectorTable[] INTERRUPT_VECTOR_TABLE_ADDRESS = { /*PRQA S 1019 #mandatory for linker behavior*/
/*lint -restore Enable MISRA rule (1.1) checking. */
  /* ISR name                               No.  Address  Name          Description */
  &UNASSIGNED_ISR,                      /* 0x40  0xEF80   ivVsi         unused by PE */ /* 00 */
  &UNASSIGNED_ISR,                      /* 0x41  0xEF82   ivVportad     unused by PE */ /* 01 */
  #ifdef AN_INPUT
    #ifdef PWM_TO_ANALOG_IN 
      &UNASSIGNED_ISR,                  /* 0x42  0xEF84   ivVatdcompare unused by PE */
    #else
      &INT0_InputAN_PWMInputISR,        /* 0x42  0xEF84   ivVatdcompare unused by PE */
    #endif  /* PWM_TO_ANALOG_IN */
  #else
    &UNASSIGNED_ISR,                                                                    /* 02 */
  #endif  /* AN_INPUT */
  &UNASSIGNED_ISR,                      /* 0x43  0xEF86   ivVReserved60 unused by PE */ /* 03 */
  #ifdef WINDOWED_WATCHDOG
  &INT0_APIIsr,                         /* 0x44  0xEF88   ivVapi        unused by PE */ /* 04 */
  #else
  &UNASSIGNED_ISR,                      /* 0x44  0xEF88   ivVapi        unused by PE */ /* 04 */
  #endif
  &UNASSIGNED_ISR,                      /* 0x45  0xEF8A   ivVReserved58 used by PE   */ /* 05 */
  &UNASSIGNED_ISR,                      /* 0x46  0xEF8C   ivVReserved57 unused by PE */ /* 06 */
  &UNASSIGNED_ISR,                      /* 0x47  0xEF8E   ivVReserved56 unused by PE */ /* 07 */
  &UNASSIGNED_ISR,                      /* 0x48  0xEF90   ivVReserved55 unused by PE */ /* 08 */
  &UNASSIGNED_ISR,                      /* 0x49  0xEF92   ivVReserved54 unused by PE */ /* 09 */
  &UNASSIGNED_ISR,                      /* 0x4A  0xEF94   ivVReserved53 unused by PE */ /* 10 */
  &UNASSIGNED_ISR,                      /* 0x4B  0xEF96   ivVReserved52 unused by PE */ /* 11 */
  &UNASSIGNED_ISR,                      /* 0x4C  0xEF98   ivVReserved51 unused by PE */ /* 12 */
  &UNASSIGNED_ISR,                      /* 0x4D  0xEF9A   ivVReserved50 unused by PE */ /* 13 */
  &UNASSIGNED_ISR,                      /* 0x4E  0xEF9C   ivVReserved49 unused by PE */ /* 14 */
  &UNASSIGNED_ISR,                      /* 0x4F  0xEF9E   ivVReserved48 unused by PE */ /* 15 */
  &UNASSIGNED_ISR,                      /* 0x50  0xEFA0   ivVReserved47 unused by PE */ /* 16 */
  &UNASSIGNED_ISR,                      /* 0x51  0xEFA2   ivVReserved46 unused by PE */ /* 17 */
  &UNASSIGNED_ISR,                      /* 0x52  0xEFA4   ivVReserved45 unused by PE */ /* 18 */
  &UNASSIGNED_ISR,                      /* 0x53  0xEFA6   ivVReserved44 unused by PE */ /* 19 */
  &UNASSIGNED_ISR,                      /* 0x54  0xEFA8   ivVReserved43 unused by PE */ /* 20 */
  &UNASSIGNED_ISR,                      /* 0x55  0xEFAA   ivVReserved42 unused by PE */ /* 21 */
  &UNASSIGNED_ISR,                      /* 0x56  0xEFAC   ivVReserved41 unused by PE */ /* 22 */
  &UNASSIGNED_ISR,                      /* 0x57  0xEFAE   ivVReserved40 unused by PE */ /* 23 */
  &UNASSIGNED_ISR,                      /* 0x58  0xEFB0   ivVcantx      unused by PE */ /* 24 */
  &UNASSIGNED_ISR,                      /* 0x59  0xEFB2   ivVcanrx      unused by PE */ /* 25 */
  &UNASSIGNED_ISR,                      /* 0x5A  0xEFB4   ivVcanerr     unused by PE */ /* 26 */
  &UNASSIGNED_ISR,                      /* 0x5B  0xEFB6   ivVcanwkup    unused by PE */ /* 27 */
  &UNASSIGNED_ISR,                      /* 0x5C  0xEFB8   ivVflash      unused by PE */ /* 28 */
  &UNASSIGNED_ISR,                      /* 0x5D  0xEFBA   ivVflashfd    unused by PE */ /* 29 */
  &UNASSIGNED_ISR,                      /* 0x5E  0xEFBC   ivVspi2       unused by PE */ /* 30 */
  &UNASSIGNED_ISR,                      /* 0x5F  0xEFBE   ivVspi1       unused by PE */ /* 31 */
  &UNASSIGNED_ISR,                      /* 0x60  0xEFC0   ivVReserved31 unused by PE */ /* 32 */
  &UNASSIGNED_ISR,                      /* 0x61  0xEFC2   ivVsci2       unused by PE */ /* 33 */
  &UNASSIGNED_ISR,                      /* 0x62  0xEFC4   ivVReserved29 unused by PE */ /* 34 */
  &UNASSIGNED_ISR,                      /* 0x63  0xEFC6   ivVcpmuplllck unused by PE */ /* 35 */
  &UNASSIGNED_ISR,                      /* 0x64  0xEFC8   ivVcpmuocsns  unused by PE */ /* 36 */
  &UNASSIGNED_ISR,                      /* 0x65  0xEFCA   ivVReserved26 unused by PE */ /* 37 */
  &UNASSIGNED_ISR,                      /* 0x66  0xEFCC   ivVReserved25 unused by PE */ /* 38 */
  &UNASSIGNED_ISR,                      /* 0x67  0xEFCE   ivVportj      unused by PE */ /* 39 */
  &UNASSIGNED_ISR,                      /* 0x68  0xEFD0   ivVReserved23 unused by PE */ /* 40 */
  &UNASSIGNED_ISR,                      /* 0x69  0xEFD2   ivVatd        unused by PE */ /* 41 */
  &UNASSIGNED_ISR,                      /* 0x6A  0xEFD4   ivVsci1       unused by PE */ /* 42 */
#if defined (ENABLE_RS232_DEBUG)
  &Vsci0ISR,                            /* 0x6B  0xEFD6   ivVsci0       unused by PE */ /* 43 */
#elif(defined(LIN_INPUT))
  &INT0_isr_DispatcherSerialCom,        /* 0x6B  0xEFD6   ivVsci0       unused by PE */ /* 43 */ 
#else
  &UNASSIGNED_ISR,                      /* 0x6B  0xEFD6   ivVsci0       unused by PE */ /* 43 */
#endif  /* ENABLE_RS232_DEBUG */
  &UNASSIGNED_ISR,                      /* 0x6C  0xEFD8   ivVspi0       unused by PE */ /* 44 */
  &UNASSIGNED_ISR,                      /* 0x6D  0xEFDA   ivVtimpaie    unused by PE */ /* 45 */
  &UNASSIGNED_ISR,                      /* 0x6E  0xEFDC   ivVtimpaaovf  unused by PE */ /* 46 */
  &INT0_TachoOverflow,                  /* 0x6F  0xEFDE   ivVtimovf     unused by PE */ /* 47 */
  &UNASSIGNED_ISR,                      /* 0x70  0xEFE0   ivVtimch7     unused by PE */ /* 48 */
  &UNASSIGNED_ISR,                      /* 0x71  0xEFE2   ivVtimch6     unused by PE */ /* 49 */
  &UNASSIGNED_ISR,                      /* 0x72  0xEFE4   ivVtimch5     unused by PE */ /* 50 */
	#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
  &INT0_DiagnosticIsr,                  /* 0x73  0xEFE6   ivVtimch4     unused by PE */ /* 51 */
  #else
  &UNASSIGNED_ISR,                      /* 0x73  0xEFE6   ivVtimch4     unused by PE */ /* 51 */
  #endif  /* DIAGNOSTIC_STATE */
  #if ( defined(PWM_INPUT) || defined(PWM_TO_ANALOG_IN) )
    #if(!defined(LIN_INPUT))
      &INT0_InputPWM_PWMInputISR,       /* 0x74  0xEFE8   ivVtimch3     unused by PE */ /* 52 */
    #else
      &INT0_isr_DispatcherLinMeas,      /* 0x74  0xEFE8   ivVtimch3     unused by PE */ /* 52 */
    #endif
  #elif(defined(LIN_INPUT))
    &INT0_isr_DispatcherLinMeas,        /* 0x74  0xEFE8   ivVtimch3     unused by PE */ /* 52 */ 
  #else
  &UNASSIGNED_ISR,                      /* 0x74  0xEFE8   ivVtimch3     unused by PE */ /* 52 */
  #endif  /* PWM_INPUT */
  &INT0_TachoCapture,                   /* 0x75  0xEFEA   ivVtimch2     unused by PE */ /* 53 */
  #ifdef DUAL_LINE_ZERO_CROSSING
    &INT0_TachoBemfCapture,             /* 0x76  0xEFEC   ivVtimch1     unused by PE */ /* 54 */
  #else
    &UNASSIGNED_ISR,                    /* 0x76  0xEFEC   ivVtimch1     unused by PE */ /* 54 */  
  #endif  /* DUAL_LINE_ZERO_CROSSING */
  &INT0_dpwmmin_modulation,             /* 0x77  0xEFEE   ivVtimch0     unused by PE */ /* 55 */
  &INT0_isrVrti,                        /* 0x78  0xEFF0   ivVrti        used   by PE */ /* 56 */
  &UNASSIGNED_ISR,                      /* 0x79  0xEFF2   ivVirq        unused by PE */ /* 57 */
  &UNASSIGNED_ISR,                      /* 0x7A  0xEFF4   ivVxirq       unused by PE */ /* 58 */
  &SWI_INSTRUCTION_ISR,                 /* 0x7B  0xEFF6   ivVswi        unused by PE */ /* 59 */
  &UNIMPLEMENTED_INSTRUCTION_ISR        /* 0x7C  0xEFF8   ivVtrap       unused by PE */ /* 60 */
};


/*lint -save  -e950 Disable MISRA rule (1.1) checking. */
static const tIsrFunc ResetVectorTable[] INTERRUPT_VECTOR_RESET_ADDRESS = { /*PRQA S 1019 #mandatory for linker behavior*/ /* Reset vector table */
/*lint -restore Enable MISRA rule (1.1) checking. */
  /* Reset handler name                          Address  Name          Description  */
  &INT0_MCU_init_reset,                 /*       0xEFFA   ivVcop        unused by PE */
  &INT0_MCU_init_reset,                 /*       0xEFFC   ivVclkmon     unused by PE */
  &INT0_MCU_init_reset                  /*       0xEFFE   ivVreset      used by PE   */
};

#if ((BOOTLOAD_VERS == BOOTLOADER_LATER_20) && defined(DEBUG_MODE)) /* Necessario per far DEBUG quando non c'e' Bootloader */

/*lint -save  -e950 Disable MISRA rule (1.1) checking. */
static const tIsrFunc _DebugResetVectorTable[] @0xFFFAU = { /* Reset vector table */
/*lint -restore Enable MISRA rule (1.1) checking. */
  /* Reset handler name                          Address  Name          Description  */
  &INT0_MCU_init_reset,                 /*       0xFFFA   ivVcop        unused by PE */
  &INT0_MCU_init_reset,                 /*       0xFFFC   ivVclkmon     unused by PE */
  &INT0_MCU_init_reset                  /*       0xFFFE   ivVreset      used by PE   */
};

#endif /*(BOOTLOAD_VERS == BOOTLOADER_LATER_20) */

/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/
          
          
          
          
          
          
          
          
          
          
          
          
          
          
