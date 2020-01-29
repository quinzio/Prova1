/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INI0_InitS12.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INI0_InitS12.c
    \version see \ref FMW_VERSION 
    \brief Functions providing micro peripherals initialization.
    \details These routines are called as first steps when entering into main
    and provide basic hw initialization for micro peripherals.\n
    \note none
    \warning none
*/

/*PRQA S 0380, 0857 ++  #Compiler supports more than 4096 defines and more than 1024 macro definitions */

#ifdef _CANTATA_
#include "cantata.h"
#endif

#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MICRO_REGISTERS
#include SPAL_DEF
#include MAC0_Register
#include "main.h"           /* Per strDebug */
#include "MCN1_Tacho.h"
#include "INT0_PWMIsrS12.h"
#include "INI0_InitS12.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* --------------------------- Private typedefs ---------------------------- */

/* -------------------------- Private variables ---------------------------- */

/* -------------------------- Public variables ----------------------------- */
  
/* --------------------------- Private functions --------------------------- */

/* --------------------------- Private macros ------------------------------ */



#pragma CODE_SEG __NEAR_SEG NON_BANKED

#pragma CODE_SEG DEFAULT
#pragma MESSAGE DISABLE C12056

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Init_InitMCU(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function initializes micro core by setting and trimming bus clock. 
    \details This function initializes micro core by setting and trimming bus clock.\n
                            \par Used Variables
    None
    
    \return \b void no value return
    \note Before coming out loading trimming value from flash memory is done.
    \warning None
*/
void Init_InitMCU(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  CPMUCOP |= 0x60U;                    /* Suspend watchdog in BDM mode (Not count for write once CPMUCOP)*/
  /* CPMUPROT: PROT=0 */
  CPMUPROT = 38U;                      /* Disable protection of clock configuration registers */
  /* CPMUCLKS: PSTP=0 */
  CPMUCLKS &= (u8)~(u8)64U;
  /* CPMUCLKS: PLLSEL=1 */
  CPMUCLKS |= (u8)128U;     /* Enable the PLL to allow write to divider registers */
  /* CPMUSYNR: VCOFRQ1=0,VCOFRQ0=0,SYNDIV5=0,SYNDIV4=1,SYNDIV3=0,SYNDIV2=0,SYNDIV1=1,SYNDIV0=1 */
  CPMUSYNR = 19U;                      /* Set the multiplier register */
  /* CPMUPOSTDIV: POSTDIV4=0,POSTDIV3=0,POSTDIV2=0,POSTDIV1=0,POSTDIV0=0 */
  CPMUPOSTDIV = 0U;                    /* Set the post divider register */
  /* CPMUPLL: FM1=0,FM0=0 */
  CPMUPLL = 0U;                        /* Set the PLL frequency modulation */
  while(CPMUFLG_LOCK == 0U) {          /* Wait until the PLL is within the desired tolerance of the target frequency */
    #ifdef _CANTATA_
      CPMUFLG_LOCK = 1U;
    #endif
  }
  /* CPMUPROT: PROT=0 */
  CPMUPROT = 0U;                       /* Enable protection of clock configuration registers */
  /* Common initialization of the CPU registers */
  /* ACMPC: ACDIEN=1 */
  ACMPC |= (u8)16U;
  /* WOMS: WOMS7=0 */
  WOMS &= (u8)~(u8)128U;
  /* CPMUINT: LOCKIE=0,OSCIE=0 */
  CPMUINT &= (u8)~(u8)18U;
  /* CPMULVCTL: LVIE=0 */
  CPMULVCTL |= 0U;
  /* IRQCR: IRQEN=0 */
  IRQCR &= (u8)~(u8)64U;
  /* ATDDIEN: IEN7=1,IEN6=1,IEN5=1,IEN4=1,IEN3=1,IEN2=1,IEN1=1,IEN0=1 */
  ATDDIEN |= 255U;                    /* N.B.                                                                                                    */
                                      /* MC9S12G Family Reference Manual, Rev. 0.41 pag.95                                                       */
                                      /* Pins AD15-0.                                                                                            */
                                      /* To activate the digital input function the related bit in the ADC Digital Input                         */
                                      /* Enable Register (ATDDIEN) must be set to 1.                                                             */
                                      /* MC9S12G Family Reference Manual, Rev. 0.41 pag.350                                                      */
                                      /* The pad of the ATD input pin is always connected to the analog input channel of the analog mulitplexer. */
                                      /* Each pad input signal is buffered to the digital port register.                                         */
                                      /* This buffer can be turned on or off with the ATDDIEN register for each ATD input pin.                   */
}


/*-----------------------------------------------------------------------------*/
/** 
    \fn void Init_InitTimer(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function initializes micro 5Mhz timer.
    \details This timer is used to detect and generate:\n
    - PWM over bridge.\n
    - Tacho ZC.\n
    - PWM input\n.
                            \par Used Variables
    None
    
    \return \b void no value return
    \note None.
    \warning None
*/
void Init_InitTimer(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  TSCR1 &= (u8)~(u8)128U;       /* Disables the main timer, including the counter */
  BitSet((TSCR1_TFFCA_MASK), TSCR1);                  /* N.B.                                                                                                                                        */
                                                      /* TFFCA has to be set in order to avoid bad PWM isr behaviour during PWM input capture isr.                                                   */
                                                      /* It can happen during PWM input capture isr, WITHOUT USING TFFCA, clearing input capture channel -> clear output compare channel as well!!!! */
                                                      /* For TFLG1(0x000E), a read from an input capture or a write to the output compare channel (0x0010–0x001F)                                    */
                                                      /* causes the corresponding channel flag, CnF, to be cleared. For TFLG2 (0x000F), any access to the TCNT                                       */
                                                      /* register (0x0004, 0x0005) clears the TOF flag                                                                                               */
  TIE = 0U;                                           /* Input Capture/Output Compare “x” Interrupt disable                                                                                          */

  #ifdef DUAL_LINE_ZERO_CROSSING
    TIOS = 17U;                                       /* IOC0 acts as an output compare for PWM isr.    */
                                                      /* IOC1 acts as an input compare for bemf zc.     */
                                                      /* IOC4 acts as an output compare for diagnostic. */
  #else
    TIOS = 19U;                                       /* IOC0 acts as an output compare for PWM isr.    */
                                                      /* IOC1 acts as an output compare for debug.      */
                                                      /* IOC4 acts as an output compare for diagnostic. */
  #endif  /* DUAL_LINE_ZERO_CROSSING */

  /* TC0: BIT15=0,BIT14=0,BIT13=0,BIT12=0,BIT11=0,BIT10=0,BIT9=0,BIT8=0,BIT7=1,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
  TC0 = 128U;                            
  /* OCPD: OCPD5=0,OCPD4=0,OCPD3=1,OCPD2=1,OCPD1=1,OCPD0=1 */
  OCPD  = 0xFF;
  TFLG1 = 63U;                                        /* Used in the clearing mechanism (set bits cause corresponding bits to be cleared).                                                                      */
  TFLG2 = 128U;                                       /* Timer Overflow Flag. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one */
  /* PTPSR: PTPS7=0,PTPS6=0,PTPS5=0,PTPS4=0,PTPS3=0,PTPS2=0,PTPS1=1,PTPS0=1 */
  TSCR2 = TACHOPRESC;                                 /* Timer Overflow Interrupt Enable: Interrupt inhibited. Timer Clock Selection: Bus Clock / 4 */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Init_InitPWMGenerator(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function initializes micro PWM timer used as pwm generator.
    \details This function initializes micro micro PWM timer registers in order to 
    generate pwm.\n
                            \par Used Variables
    None
    
    \return \b void no value return
    \note None.
    \warning None
*/
void Init_InitPWMGenerator(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  /* PWME: PWME5=0,PWME4=0,PWME3=0,PWME2=0,PWME1=0,PWME0=0 */
  PWME = 0U;                           /* Disable all PWM channels */
  /* PWMPOL: PPOL5=0,PPOL4=0,PPOL3=0,PPOL2=0,PPOL1=0,PPOL0=0 */
  PWMPOL = 0U;
  /* PWMCLK: PCLK5=0,PCLK4=0,PCLK3=0,PCLK2=0,PCLK1=0,PCLK0=0 */
  PWMCLK = 0U;
  /* PWMCAE: CAE5=1,CAE4=0,CAE3=1,CAE2=0,CAE1=1,CAE0=0 */
  PWMCAE = 42U;
  /* PWMCTL: CON45=1,CON23=1,CON01=1,PSWAI=1,PFRZ=1 */
  PWMCTL = 124U;
  /* PWMDTY23: PWMDTY23=0 */
  PWMDTY23 = 0U;
  /* PWMPER23: PWMPER23=510 */
  PWMPER23 = PWM_MODULO;
  /* PWMDTY45: PWMDTY45=0 */
  PWMDTY45 = 0U;
  /* PWMPER45: PWMPER45=510 */
  PWMPER45 = PWM_MODULO;
  /* PWMDTY01: PWMDTY01=0 */
  PWMDTY01 = 0U;
  /* PWMPER01: PWMPER01=510 */
  PWMPER01 = PWM_MODULO;
  /* PWMSCLA: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
  PWMSCLA = 0U;
  /* PWMSCLB: BIT7=0,BIT6=0,BIT5=0,BIT4=0,BIT3=0,BIT2=0,BIT1=0,BIT0=0 */
  PWMSCLB = 0U;
  /* PWMPRCLK: PCKB2=0,PCKB1=0,PCKB0=0,PCKA2=0,PCKA1=0,PCKA0=0 */
  PWMPRCLK = 0U;
  /* PWME: PWME5=1,PWME4=0,PWME3=1,PWME2=0,PWME1=1,PWME0=0 */
  PWME = 42U;                          /* Enable only configured PWM channels */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Init_InitADC(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function initializes micro ADC peripheral.
    \details This function initializes micro ADC registers in order to 
    get ADC peripheral ready for convertion.\n
    ADC has used as below:\n
    Sample time: 8A/D conversion clock period\n 
    10 bit conversion +\n 
    Asynchronous clock\n
    Conversion time (Single or first continous 10-bit): 5uS\n
                            \par Used Variables
    None
    
    \return \b void no value return
    \note None.
    \warning None
*/
void Init_InitADC(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  /* ATDDIEN: IEN7=0,IEN6=0,IEN5=0,IEN4=0,IEN3=0,IEN2=0,IEN1=0,IEN0=0 */
  /*ATDDIEN = 65280U;*/                                                               /* N.B.                                                                                                    */
                                                                                      /* MC9S12G Family Reference Manual, Rev. 0.41 pag.95                                                       */
                                                                                      /* Pins AD15-0.                                                                                            */
                                                                                      /* To activate the digital input function the related bit in the ADC Digital Input                         */
                                                                                      /* Enable Register (ATDDIEN) must be set to 1.                                                             */
                                                                                      /* MC9S12G Family Reference Manual, Rev. 0.41 pag.350                                                      */
                                                                                      /* The pad of the ATD input pin is always connected to the analog input channel of the analog mulitplexer. */
                                                                                      /* Each pad input signal is buffered to the digital port register.                                         */
                                                                                      /* This buffer can be turned on or off with the ATDDIEN register for each ATD input pin.                   */
  
  /* ATDCTL0: WRAP3=1,WRAP2=1,WRAP1=1,WRAP0=1 */
  /* ATDCTL0 = 15U;*/
  /* ATDCTL1: ETRIGSEL=0,SRES1=0,SRES0=1,SMP_DIS=1,ETRIGCH3=1,ETRIGCH2=1,ETRIGCH1=1,ETRIGCH0=1 */
  /* ATDCTL1 = 63U;*/
  /* ATDCTL3: DJM=1,S8C=0,S4C=1,S2C=0,S1C=0,FIFO=0,FRZ1=0,FRZ0=0 */
  /* ATDCTL3 = 160U;*/
  /* ATDCTL4: SMP2=0,SMP1=1,SMP0=0,PRS4=0,PRS3=0,PRS2=0,PRS1=0,PRS0=1 */
  /* ATDCTL4 = 65U; */
  /* ATDCTL5: SC=0,SCAN=0,MULT=0,CD=0,CC=0,CB=0,CA=0 */
  /* ATDCTL5 = 0U; */
  /* ATDCTL2: AFFC=1,ICLKSTP=0,ETRIGLE=0,ETRIGP=0,ETRIGE=0,ASCIE=0,ACMPIE=0 */
  /* ATDCTL2 = 64U; */

  ATDCTL0 = ((u8)ATDCTL0_WRAP2_MASK|ATDCTL0_WRAP1_MASK|ATDCTL0_WRAP0_MASK );          /* Multiple Channel Conversions (MULT = 1). Wraparound to AN0 after Converting AN7.                         */
  ATDCTL1 = 63U;                                                                      /* 10-bit data. External trigger source is reserved.                                                        */
  ATDCTL2 = (u8)((u8)ATDCTL2_AFFC_MASK);                                              /* Automatic compare is disabled (CMPE[n]=0 in ATDCMPE)                                                     */
  ATDCTL3 = (u8)((u8)ATDCTL3_DJM_MASK | ATDCTL3_S8C_MASK);                            /* Convert 8 channels in sequence and right align                                                           */
  ATDCTL4 = 65U;                                                                      /* Sample Time in Number of ATD Clock Cycles: 10. fATDCLK=fBUS/4.                                           */
  ATDCTL5 = ATDCTL5_MULT_MASK;                                                        /* Multi-Channel Sample Mode. AN0 is the first channel to be examined in the conversion sequence.           */
                                                                                      /* Subsequent channels are determined by incrementing the channel selection code or wrapping around         */
                                                                                      /* to AN0 (after converting the channel defined by the Wrap Around Channel Select Bits WRAP3-0 in ATDCTL0). */
  ATDCMPE = 0x00;                                                                     /* Automatic compare is enabled for conversion n                                                            */
}

/*-----------------------------------------------------------------------------*/
/** 
    \fn void Init_InitTacho(void)
    \author	Pasquale Rubini  \n <em>Main Developer</em> 
    \author Boschesi Roberto \n <em>Reviewer</em>
    \author Stocchi Filippo  \n <em>Reviewer</em>

    \date 23/11/2010
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief This function initializes micro timer module (TIM) peripheral.
    \details This function initializes micro timer module (TIM) registers used to evaluate tacho timing.
    timer module (TIM) has used in input capture on FALLING edge.\n
                            \par Used Variables
    None
    
    \return \b void no value return
    \note None.
    \warning None
*/
void Init_InitTacho(void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
  TCTL4_EDG2x = 2U;                                   /* ZC current (IOC2) -> Capture on falling edges only.                                                     */
  TFLG1_C2F = 1U;                                     /* These flags are set when an input capture or output compare event occurs.                               */
                                                      /* Clearing requires writing a one to the corresponding flag bit while TEN or PAEN is set to one.          */
  INTERRUPT_TACHO_CLEAR;                              /* N.B.                                                                                                    */
                                                      /* Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare */
                                                      /* channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared                     */

  #ifdef DUAL_LINE_ZERO_CROSSING
    TCTL4_EDG1x = 2U;
    TFLG1_C1F = 1U;

    INTERRUPT_BEMF_CLEAR;
  #endif  /* DUAL_LINE_ZERO_CROSSING */
}

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
