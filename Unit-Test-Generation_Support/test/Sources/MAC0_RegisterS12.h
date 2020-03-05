/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  MAC0_RegisterS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file MAC0_RegisterS12.h
    \version see \ref FMW_VERSION 
    \brief MACROs relied to micro register and pinout
    \details Main purpose of this header file is to make all other files hardware independent.\n
    This can be dome by transforming all micro references in MACROs 
    \note none
    \warning none
*/
#ifndef REGISTER_H12
#define REGISTER_H12

//---------------------------    WATCHDOG   -------------------------

//------------------------WINDOWED WACHDOG --------------------
// Windowed watchdog must be refreshed after 75% and before 100% of its timeout period.
// Timeout is defined by number of ACLK period(1/10kHz)
// Watchdog is refreshed with API interrupt called every 87% of Timeout Watchdog period.
// CPMUAPIR define interrupt period.
// WTD_COUNTS define period timeout.
// See MC9S12GRMV1.pdf §10.3.2.9 S12CPMU COP Control Register (CPMUCOP) and 
// ..§10.3.2.16 Autonomous Periodical Interrupt Rate High and Low Register
// (CPMUAPIRH / CPMUAPIRL) for further details
//
//  WTD_NUM       Watchdog Timeout
//      0           disabled
//      1           2^7/10KHz   -> 12,8 msec
//      2           2^9/10KHz   -> 51,2 msec
//      3           2^11/10KHz  -> 204,8 msec
//      4           2^13/10KHz  -> 819,2 msec
//      5           2^15/10KHz  -> 3,27 sec
//      6           2^16/10KHz  -> 6,55 sec
//      7           2^17/10KHz  -> 13,1 sec
//      >7          error


#ifdef WINDOWED_WATCHDOG

#if (WTD_NUM == 0)
  #define WTD_COUNTS 0u
#elif (WTD_NUM < 6)
  #define WTD_COUNTS (((u32)1u<<(5u+(2u*WTD_NUM))))
#elif (WTD_NUM == 6)
  #define WTD_COUNTS (((u32)1u<<(4u+(2u*WTD_NUM))))
#elif (WTD_NUM == 7)
  #define WTD_COUNTS (((u32)1u<<(3u+(2u*WTD_NUM))))
#else
  #error("wrong watchdog num") 
#endif  //WTD

#endif // WINDOWED_WATCHDOG

//CPMUAPIR --> minimum value is 56 with WTD_NUM=1  --> 11,2 msec
//CPMUAPIR --> maximum value is 57016 with WTD_NUM=7  --> 11,4 sec
// 11,4 seconds

#ifdef WTD_TEST

#define RESET_COP_WATCHDOG         {\
                                    CPMUARMCOP = 0x55;\
                                    CPMUARMCOP = 0xAA;\
                                    TOGGLE_DEBUG2;\
                                   }

#else

#define RESET_COP_WATCHDOG         {CPMUARMCOP = 0x55; CPMUARMCOP = 0xAA;}

#endif

// Set COP windowed and timeout to 2^7 clock period => timeout after 12.8msec  
// ..(CPMUCOP register is write once register if WRTMASK = 0 in normal  mode)
// Set alternate clock (ACLK -> Timebase 10KHz) for windowed watchdog 

#define WATCHDOG_INIT_WINDOWED_WTD {\
/* Enable write to clock register */   CPMUPROT = 38U;\
/* Select ACLK for COP (10Khz) */      CPMUCLKS_COPOSCSEL1 = 1;\
/* Set windowed and period*/           CPMUCOP =(u8)(CPMUCOP_WCOP_MASK|CPMUCOP_RSBCK_MASK|WTD_NUM);\
/* Disable write to clock register */  CPMUPROT = 0U;\
                                   }

// Set COP timeout to 2^7 clock period => timeout after 12.8msec
// ..(CPMUCOP register is write once register if WRTMASK = 0 in normal  mode)

#define WATCHDOG_INIT_NORMAL_WTD {\
/* Enable write to clock register */     CPMUPROT = 38U;\
/* Select ACLK for COP (10Khz) */        CPMUCLKS_COPOSCSEL1 = 1;\
/* Set period*/                          CPMUCOP = (CPMUCOP_RSBCK_MASK|CPMUCOP_CR0_MASK);\
/* Enable write to clock register */     CPMUPROT = 0U;\
                                 }
     
//--------------------------- API ------------------------------
//API rate connected to autonomous clock is ApiPeriod = (2*CPMUAPIR+1)/fACLK
// fACLK=10kHz
// Api period is (2*56+1)/fACLK=11.3msec
// Like 87.5% of WindowWatchdogPeriod=0.875*12.8msec
#define API_CLEAR_PENDING_INTERRUPT   { CPMUAPICTL_APIF = 1;}

#define API_ENABLE_API      { \
   /*Disable api*/             CPMUAPICTL_APIFE = 0u;\
   /*Wait write APIFE*/        Nop();\
   /*Disable API_EXTCLK*/      CPMUAPICTL_APIEA = 0u;\
   /*Set Api rate */           CPMUAPIR = ((u16)(((u32)WTD_COUNTS*88u)/200u));\
   /*API connected to ACLK*/   CPMUAPICTL_APICLK = 0u;\
   /*Clear pending interrupt*/ CPMUAPICTL_APIF = 1u;\
   /*Enable API Interrupt */   CPMUAPICTL_APIE = 1u;\
   /*Ensable api*/             CPMUAPICTL_APIFE = 1u;\
   }

#define API_DISABLE_API   { CPMUAPICTL_APIFE = 0;}  //Disable api



///BOOTLOADER SETTING 

#define RESET_BOOTLOADER_SETTINGS   { SCI0CR2=0;}       //Disable SCI interrupts leaved enabled by bootloader

//When LOG_DATI is enabled, EEPROM parameters must be protected
#define PROTECT_EEPROM_TRIMMING_PARAMETERS  (DFPROT = 0x01)   //Protect EEPROM parameters DFPROT = 0x01 --> 0x0_0400 ... 0x0_043F (64 bytes)


//	Possibles inverter status
#define UNDER_VOLTAGE                                     ((u8)0x01)
#define OVER_VOLTAGE                                      ((u8)0x02)
#define TAMB_OVER_TEMPERATURE                             ((u8)0x08)                                      // Private flag to store inverter state
#define TMOS_OVER_TEMPERATURE                             ((u8)0x10)                                      // Private flag to store inverter state
#define UNDER_CURRENT                                     ((u8)0x40)                                      // Private flag to store inverter state
#define OVER_CURRENT                                      ((u8)0x80)                                      // Private flag to store inverter state

// ADC peripheral bits and bitfields definitions
#define CONVERT_AIN0	((u8)	0)
#define CONVERT_AIN1	((u8) ATDCTL5_CA_MASK )                                                             
#define CONVERT_AIN2	((u8) ATDCTL5_CB_MASK )                                                             
#define CONVERT_AIN3	((u8) ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN4	((u8) ATDCTL5_CC_MASK )                                                             
#define CONVERT_AIN5	((u8) ATDCTL5_CC_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN6	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK )                                           
#define CONVERT_AIN7	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                         
#define CONVERT_AIN8	((u8) ATDCTL5_CD_MASK )                                                             
#define CONVERT_AIN9	((u8) ATDCTL5_CD_MASK + ATDCTL5_CA_MASK	)                                           
#define CONVERT_AIN10	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	)                                           
#define CONVERT_AIN11	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN12	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	)                                           
#define CONVERT_AIN13	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN14	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK )                         
#define CONVERT_AIN15	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK + ATDCTL5_CA_MASK ) 

#if ( PCB == PCB211A_HY )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRM_DDRM1 = 1  
#define DEBUG1_ON                                         PTM_PTM1 = 1
#define DEBUG1_OFF                                        PTM_PTM1 = 0
#define TOGGLE_DEBUG1                                     PTM_PTM1 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRM_DDRM0 = 1  
#define DEBUG2_ON                                         PTM_PTM0 = 1
#define DEBUG2_OFF                                        PTM_PTM0 = 0
#define TOGGLE_DEBUG2                                     PTM_PTM0 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRS_DDRS6 = 0                                  
#define STAND_BY_SET_OUT                                  DDRS_DDRS6 = 1                                  
#define STAND_BY_ON                                       PTS_PTS6 = 1                                    
#define STAND_BY_OFF                                      PTS_PTS6 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0                                    

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                     

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR_ARR[0] = ANALOG_PWM_HIGH_BIT;;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 1;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR_ARR[0] = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          } 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)    {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }
#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


//#define	INIT_START_WATCHDOG                               {CPMUCOP = 1; CPMUCLKS_COPOSCSEL1 = 1;}        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr enabling */                                TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB213A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
//I_GAIN
#define SET_I_GAIN_OUT                                    DDRT_DDRT1 = 1  
#define I_GAIN_ON                                         PTT_PTT1 = 1
#define I_GAIN_OFF                                        PTT_PTT1 = 0
#define TOGGLE_I_GAIN                                     PTT_PTT1 ^= 1

//VDH_CTRL
#define SET_VDH_CTRL_OUT                                  DDRP_DDRP0 = 1  
#define VDH_CTRL_ON                                       PTP_PTP0 = 1
#define VDH_CTRL_OFF                                      PTP_PTP0 = 0
#define TOGGLE_VDH_CTRL                                   PTP_PTP0 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)    {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }
#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


       

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB213B )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
//I_GAIN
#define SET_I_GAIN_OUT                                    DDRT_DDRT1 = 1  
#define I_GAIN_ON                                         PTT_PTT1 = 1
#define I_GAIN_OFF                                        PTT_PTT1 = 0
#define TOGGLE_I_GAIN                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


       

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB213C )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
//I_GAIN
#define SET_I_GAIN_OUT                                    DDRT_DDRT1 = 1  
#define I_GAIN_ON                                         PTT_PTT1 = 1
#define I_GAIN_OFF                                        PTT_PTT1 = 0
#define TOGGLE_I_GAIN                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB213D )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
//I_GAIN
#define SET_I_GAIN_OUT                                    DDRT_DDRT1 = 1  
#define I_GAIN_ON                                         PTT_PTT1 = 1
#define I_GAIN_OFF                                        PTT_PTT1 = 0
#define TOGGLE_I_GAIN                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                  

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB220A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY
#define INPUTECON_INIT                                    { PERS_PERS0=0;}  
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
/* Disable pullup on diag. short circuit monitor  */        DDRP_DDRP0 = 0;\
                                                            PERS_PERS0 = 0;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB220A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE

#else
 #error ("diagnostic not standalone is not possible on PCB220A")                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


       

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)5)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define DIAG_CONV                                         ((u8)4)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF



/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB220B )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY
#define INPUTECON_INIT                                    { PERS_PERS0=0;}  
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
/* Disable pullup on diag. short circuit monitor  */        DDRP_DDRP0 = 0;\
                                                            PERS_PERS0 = 0;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB220A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE

#else
 #error ("diagnostic not standalone is not possible on PCB220A")                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


       

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)5)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define DIAG_CONV                                         ((u8)4)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF



/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB220C )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY
#define INPUTECON_INIT                                    { PERS_PERS0=0;}  
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          

#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))
                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
/* Disable pullup on diag. short circuit monitor  */        DDRP_DDRP0 = 0;\
                                                            PERS_PERS0 = 0;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB220A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE

#else
 #error ("diagnostic not standalone is not possible on PCB220A")                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!

       

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)5)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define DIAG_CONV                                         ((u8)4)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF



/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB221A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
//I_GAIN
#define SET_I_GAIN_OUT                                    DDRT_DDRT1 = 1  
#define I_GAIN_ON                                         PTT_PTT1 = 1
#define I_GAIN_OFF                                        PTT_PTT1 = 0
#define TOGGLE_I_GAIN                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }
#define ZCE_CNTRL_SET_OUT                                 DDR1AD_DDR1AD7 = 1                              
#define ZCE_CNTRL_ON                                      PT1AD_PT1AD7 = 1                                
#define ZCE_CNTRL_OFF                                     PT1AD_PT1AD7 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */
#elif ( PCB == PCB221B )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
//I_GAIN
#define SET_I_GAIN_OUT                                    DDRT_DDRT1 = 1  
#define I_GAIN_ON                                         PTT_PTT1 = 1
#define I_GAIN_OFF                                        PTT_PTT1 = 0
#define TOGGLE_I_GAIN                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB224A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
#define SET_DEBUG_OUT                                    DDRT_DDRT1 = 1  
#define DEBUG_ON                                         PTT_PTT1 = 1
#define DEBUG_OFF                                        PTT_PTT1 = 0
#define TOGGLE_DEBUG                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)4)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF




/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB224B )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
#define SET_DEBUG_OUT                                    DDRT_DDRT1 = 1  
#define DEBUG_ON                                         PTT_PTT1 = 1
#define DEBUG_OFF                                        PTT_PTT1 = 0
#define TOGGLE_DEBUG                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)5)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define DIAG_CONV                                         ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define AN_CONV                                           ((u8)4)


#define ANALOG_INPUT_CHANNEL      CONVERT_AIN4
#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF



/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB224C )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
#define SET_DEBUG_OUT                                    DDRT_DDRT1 = 1  
#define DEBUG_ON                                         PTT_PTT1 = 1
#define DEBUG_OFF                                        PTT_PTT1 = 0
#define TOGGLE_DEBUG                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                                

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)5)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define DIAG_CONV                                         ((u8)4)
                                       
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4
#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF



/* Public Variables -------------------------------------------------------- */

/* Public Functions prototypes --------------------------------------------- */

#elif ( PCB == PCB224D )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////
#define SET_DEBUG_OUT                                    DDRT_DDRT1 = 1  
#define DEBUG_ON                                         PTT_PTT1 = 1
#define DEBUG_OFF                                        PTT_PTT1 = 0
#define TOGGLE_DEBUG                                     PTT_PTT1 ^= 1

//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0
#define IS_ZCE_CNTRL_OFF                                  PTM_PTM1 == 0

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          

#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )
//DIAGNOSTIC						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                        
#define DIAGNOSTIC_TOGGLE_PORT                            DIAGNOSTIC_PORT_BIT^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
                                                          } 
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                      
                                                          
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      

#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0                                    

#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#define IS_DIAGNOSTIC_SHORTED                             PT1AD_PT1AD6                                    // R32 not placed. R10 placed. Short on key wire i.e. PAD6.
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                                        // R32 placed. R10 not placed. Short on key wire i.e. PTS0.
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }


#endif  // DIAGNOSTIC_STATE

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define I_MEAN_CONV                                       ((u8)6)
#define DIAG_CONV                                         ((u8)7)                                                                             
                                       
#define CURRENT_INPUT_CHANNEL   CONVERT_AIN0
#define BUS_VOLTAGE_CHANNEL     CONVERT_AIN1
#define ANALOG_INPUT_CHANNEL    CONVERT_AIN2
#define TLE_TEMP_SENSOR_CHANNEL CONVERT_AIN3
#define MEAN_CURRENT_CHANNEL    CONVERT_AIN6
#define DIAG_SHORT_MON_CHANNEL  CONVERT_AIN7       //Monitor for diag short circuit condition                                       

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF



#elif ( PCB == PCB225A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           DDR1AD_DDR1AD5 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN5 = 1;\
                                                          }
#define GD_CHECK_ERROR_LINE                               PT1AD_PT1AD5                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//SCDL_HIGH_LEVEL
#define SCDL_HI_LEV_REMOVE_PULL_RESISTOR                  PUCR_PDPEE = 0                                  
#define SCDL_HI_LEV_SET_INPUT                             DDRE_DDRE1 = 0

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* set data direction register */                           DDR1AD_DDR1AD6 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN6 = 1;\
                                                          }
#define TEST_KEY_ON_OFF                                   PT1AD_PT1AD6 == 0                                    

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = 135U; }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }


//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                    


//******************** DIAGNOSTIC SETTINGS ********************
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC LOGIC ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
/* Disable pullup on diag. short circuit monitor  */        DDRP_DDRP0 = 0;\
                                                            PERS_PERS0 = 0;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB225A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC LOGIC ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
/* Disable pullup on diag. short circuit monitor  */        DDRP_DDRP0 = 0;\
                                                            PERS_PERS0 = 0;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB225A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
 #error ("diagnostic not standalone is not possible on PCB225A")                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define DIAG_CONV                                         ((u8)7)  

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF



#elif ( PCB == PCB225B )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = 135U; }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB225B") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTS_PTS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define DIAG_CONV                                         ((u8)7)  


#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF

#elif ( PCB == PCB225C )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))   

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_DISABLE_DIGITAL_INPUT                     {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
                                                          
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = 135U; }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB225C") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define AN_CONV                                           ((u8)4)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define DIAG_CONV                                         ((u8)7)  


#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF

#elif ( PCB == PCB233C )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))   

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_DISABLE_DIGITAL_INPUT                     {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
                                                          
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = 135U; }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB225C") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define AN_CONV                                           ((u8)4)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6)
#define DIAG_CONV                                         ((u8)7)  


#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition
#define TMOS_TEMP_SENSOR_CHANNEL  CONVERT_AIN5
#define IDC_INPUT_CHANNEL         CONVERT_AIN6
#define DIAG_INPUT_CHANNEL        CONVERT_AIN7

//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF

#elif ( PCB == PCB228A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB228A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define DIAG_CONV                                         ((u8)7)  

//	Possibles inverter status
#define UNDER_VOLTAGE                                     ((u8)0x01)
#define OVER_VOLTAGE                                      ((u8)0x02)
#define TAMB_OVER_TEMPERATURE                             ((u8)0x08)                                      // Private flag to store inverter state
#define UNDER_CURRENT                                     ((u8)0x40)                                      // Private flag to store inverter state
#define OVER_CURRENT                                      ((u8)0x80)                                      // Private flag to store inverter state

// ADC peripheral bits and bitfields definitions
#define CONVERT_AIN0	((u8)	0)
#define CONVERT_AIN1	((u8) ATDCTL5_CA_MASK )                                                             
#define CONVERT_AIN2	((u8) ATDCTL5_CB_MASK )                                                             
#define CONVERT_AIN3	((u8) ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN4	((u8) ATDCTL5_CC_MASK )                                                             
#define CONVERT_AIN5	((u8) ATDCTL5_CC_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN6	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK )                                           
#define CONVERT_AIN7	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                         
#define CONVERT_AIN8	((u8) ATDCTL5_CD_MASK )                                                             
#define CONVERT_AIN9	((u8) ATDCTL5_CD_MASK + ATDCTL5_CA_MASK	)                                           
#define CONVERT_AIN10	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	)                                           
#define CONVERT_AIN11	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN12	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	)                                           
#define CONVERT_AIN13	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN14	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK )                         
#define CONVERT_AIN15	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )       

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


#elif ( PCB == PCB228B )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  (u16)(K_SETPOINT_PU*u8InRef)  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB228B") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6)
#define DIAG_CONV                                         ((u8)7)  

//	Possibles inverter status
#define UNDER_VOLTAGE                                     ((u8)0x01)
#define OVER_VOLTAGE                                      ((u8)0x02)
#define TAMB_OVER_TEMPERATURE                             ((u8)0x08)                                      // Private flag to store inverter state
#define UNDER_CURRENT                                     ((u8)0x40)                                      // Private flag to store inverter state
#define OVER_CURRENT                                      ((u8)0x80)                                      // Private flag to store inverter state

// ADC peripheral bits and bitfields definitions
#define CONVERT_AIN0	((u8)	0)
#define CONVERT_AIN1	((u8) ATDCTL5_CA_MASK )                                                             
#define CONVERT_AIN2	((u8) ATDCTL5_CB_MASK )                                                             
#define CONVERT_AIN3	((u8) ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN4	((u8) ATDCTL5_CC_MASK )                                                             
#define CONVERT_AIN5	((u8) ATDCTL5_CC_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN6	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK )                                           
#define CONVERT_AIN7	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                         
#define CONVERT_AIN8	((u8) ATDCTL5_CD_MASK )                                                             
#define CONVERT_AIN9	((u8) ATDCTL5_CD_MASK + ATDCTL5_CA_MASK	)                                           
#define CONVERT_AIN10	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	)                                           
#define CONVERT_AIN11	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN12	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	)                                           
#define CONVERT_AIN13	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN14	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK )                         
#define CONVERT_AIN15	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )       

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF

#elif ( PCB == PCB230A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  ((u16)((u16)u8InRef*K_SETPOINT))  

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB230A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6)
#define DIAG_CONV                                         ((u8)7)  


#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


#elif ( PCB == PCB237E )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  (u16)(K_SETPOINT_PU*u8InRef)  

//LIN Interface
#define LIN_TRANSCEIVER_ON                                {\
/* set PS6 direction */                                    DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                        PTS_PTS6 = 1 ;\
                                                          }

#define LIN_TRANSCEIVER_OFF                               {\
/* set PS6 direction */                                    DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                        PTS_PTS6 = 0 ;\
                                                          }
//RVP
#define SET_RVP_OUT                                       DDRS_DDRS4 = 1  
#define RVP_ON                                            PTS_PTS4 = 1
#define RVP_OFF                                           PTS_PTS4 = 0

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB228B") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define I_BATT_CONV_257HZ                                 ((u8)2)   // CANALE 2 = filtro a 257 Hz / CANALE 7 = filtro a 27 Hz
#define T_AMB_TLE                                         ((u8)3)
#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)7)   // CANALE 2 = filtro a 257 Hz / CANALE 7 = filtro a 27 Hz

//	Possibles inverter status
#define UNDER_VOLTAGE                                     ((u8)0x01)
#define OVER_VOLTAGE                                      ((u8)0x02)
#define TAMB_OVER_TEMPERATURE                             ((u8)0x08)                                      // Private flag to store inverter state
#define UNDER_CURRENT                                     ((u8)0x40)                                      // Private flag to store inverter state
#define OVER_CURRENT                                      ((u8)0x80)                                      // Private flag to store inverter state

// ADC peripheral bits and bitfields definitions
#define CONVERT_AIN0	((u8)	0)
#define CONVERT_AIN1	((u8) ATDCTL5_CA_MASK )                                                             
#define CONVERT_AIN2	((u8) ATDCTL5_CB_MASK )                                                             
#define CONVERT_AIN3	((u8) ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN4	((u8) ATDCTL5_CC_MASK )                                                             
#define CONVERT_AIN5	((u8) ATDCTL5_CC_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN6	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK )                                           
#define CONVERT_AIN7	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                         
#define CONVERT_AIN8	((u8) ATDCTL5_CD_MASK )                                                             
#define CONVERT_AIN9	((u8) ATDCTL5_CD_MASK + ATDCTL5_CA_MASK	)                                           
#define CONVERT_AIN10	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	)                                           
#define CONVERT_AIN11	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN12	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	)                                           
#define CONVERT_AIN13	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN14	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK )                         
#define CONVERT_AIN15	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )       

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF

#elif ( PCB == PCB240A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  (u16)(K_SETPOINT_PU*u8InRef)

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRS_DDRS4 = 1  
#define DEBUG1_ON                                         PTS_PTS4 = 1
#define DEBUG1_OFF                                        PTS_PTS4 = 0
#define TOGGLE_DEBUG1                                     PTS_PTS4 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB230A") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define AN_CONV                                           ((u8)2)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6)
#define DIAG_CONV                                         ((u8)7)  


#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


#elif ( PCB == PCB236B )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////


//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  (u16)(K_SETPOINT_PU*u8InRef)   

//RVP
#define SET_RVP_OUT                                       DDRS_DDRS4 = 1  
#define RVP_ON                                            PTS_PTS4 = 1
#define RVP_OFF                                           PTS_PTS4 = 0

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS6 = 1
#define DEBUG2_ON                                         PTS_PTS6 = 1
#define DEBUG2_OFF                                        PTS_PTS6 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS6 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

                                                          
//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                      

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 


//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_DISABLE_DIGITAL_INPUT                     {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
                                                          
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)


#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)


#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB225C") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define PWM_TO_AN_CONV                                    ((u8)2)
#define T_AMB_TLE                                         ((u8)3)
#define AN_CONV                                           ((u8)4)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6)
#define DIAG_CONV                                         ((u8)7)  


#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


#elif ( PCB == PCB250A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////

//LIN Interface
#define LIN_TRANSCEIVER_ON                                {\
/* set PS6 direction */                                     DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                         PTS_PTS6 = 1 ;\
                                                           }

#define LIN_TRANSCEIVER_OFF                                {\
/* set PS6 direction */                                     DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                         PTS_PTS6 = 0 ;\
                                                           }  
                                                           
//RVP
#define SET_RVP_OUT                                       DDRS_DDRS4 = 1  
#define RVP_ON                                            PTS_PTS4 = 1
#define RVP_OFF                                           PTS_PTS4 = 0
                                                                                                                    
//SET POINT
#define SET_FREQUENCY_SET_POINT(u8InRef)                  (u16)((K_SETPOINT_PU)*(u8InRef)) /*PRQA S 3453 #This function-like macro has been tested and consolidated so it is not necessary to modify it */

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRP_DDRP2 = 1  
#define DEBUG1_ON                                         PTP_PTP2 = 1
#define DEBUG1_OFF                                        PTP_PTP2 = 0
#define TOGGLE_DEBUG1                                     PTP_PTP2 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDR1AD_DDR1AD7 = 1
#define DEBUG2_ON                                         PT1AD_PT1AD7 = 1
#define DEBUG2_OFF                                        PT1AD_PT1AD7 = 0
#define TOGGLE_DEBUG2                                     PT1AD_PT1AD7 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    ((BOOL)(PTIT_PTIT3==1))
#define PWM_IN_IS_LOW                                     ((BOOL)(PTIT_PTIT3==0))                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
}
/* Disable SCI control (BOOTL) of diagnostic pin */         /* SCI0CR2_TE = 0;\ */

#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB228B") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE                  TC4 = (u16)((u16)TC4+DIAGNOSTIC_NEXT_ISR)       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

#define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                          if ( !TIE_C4I )\
                                                          {\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
/* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+DIAGNOSTIC_NEXT_ISR);\
/* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                          }\
                                                          }

#define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                            DIAGNOSTIC_OFF;\
                                                            u16DiagPeriodCount = 0;\
                                                            u16DiagPeriodDuty = 0;\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
/* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                          }

#define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                            u16DiagPeriodDuty = u16TPM3Duty;\
                                                          }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!

        
//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//BEMF TACHO						
#define	BEMF_CAPTURE_REGISTER                             (TC1)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_BEMF_ENABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC1;\
/* Isr channel 2 is enabled */                              TIE_C1I = 1;\
                                                          }

#define INTERRUPT_BEMF_DISABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC1;\
/* Isr channel 2 is disabled */                             TIE_C1I = 0;\
                                                          }

#define	INTERRUPT_BEMF_CLEAR                              (void)TC1                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define I_BATT_CONV_257HZ                                 ((u8)2) // AN2 -> fbw = 257 Hz, AN6 -> fbw = 27 Hz
#define T_AMB_TLE                                         ((u8)3)
#define ECONOMY_CONV                                      ((u8)4)
#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6) // AN2 -> fbw = 257 Hz, AN6 -> fbw = 27 Hz
#define DIAG_CONV                                         ((u8)7)  

//	Possibles inverter status
#define UNDER_VOLTAGE                                     ((u8)0x01)
#define OVER_VOLTAGE                                      ((u8)0x02)
#define TAMB_OVER_TEMPERATURE                             ((u8)0x08)                                      // Private flag to store inverter state
#define UNDER_CURRENT                                     ((u8)0x40)                                      // Private flag to store inverter state
#define OVER_CURRENT                                      ((u8)0x80)                                      // Private flag to store inverter state

// ADC peripheral bits and bitfields definitions
#define CONVERT_AIN0	((u8)	0)
#define CONVERT_AIN1	((u8) ATDCTL5_CA_MASK )                                                             
#define CONVERT_AIN2	((u8) ATDCTL5_CB_MASK )                                                             
#define CONVERT_AIN3	((u8) ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN4	((u8) ATDCTL5_CC_MASK )                                                             
#define CONVERT_AIN5	((u8) ATDCTL5_CC_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN6	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK )                                           
#define CONVERT_AIN7	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                         
#define CONVERT_AIN8	((u8) ATDCTL5_CD_MASK )                                                             
#define CONVERT_AIN9	((u8) ATDCTL5_CD_MASK + ATDCTL5_CA_MASK	)                                           
#define CONVERT_AIN10	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	)                                           
#define CONVERT_AIN11	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN12	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	)                                           
#define CONVERT_AIN13	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN14	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK )                         
#define CONVERT_AIN15	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )       

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF


#elif ( PCB == PCB262A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////

//LIN Interface
#define LIN_TRANSCEIVER_ON                                {\
/* set PS6 direction */                                     DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                         PTS_PTS6 = 1 ;\
                                                           }

#define LIN_TRANSCEIVER_OFF                                {\
/* set PS6 direction */                                     DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                         PTS_PTS6 = 0 ;\
                                                           }                                                             
//RVP
#define SET_RVP_OUT                                       DDRS_DDRS4 = 1  
#define RVP_ON                                            PTS_PTS4 = 1
#define RVP_OFF                                           PTS_PTS4 = 0

//V_BATT_ON
#define SET_V_BATT_ON_OUT                                 DDRP_DDRP0 = 1  
#define V_BATT_ON_ENABLE                                  PTP_PTP0 = 1
#define V_BATT_ON_DISABLE                                 PTP_PTP0 = 0
                                                                                                                    
//SET POINT
//#define SET_FREQUENCY_SET_POINT(u8InRef)                  (u16)((K_SETPOINT_PU)*(u8InRef)) /*PRQA S 3453 #This function-like macro has been tested and consolidated so it is not necessary to modify it */

//DEBUG2
#define SET_DEBUG2_OUT                                    DDR1AD_DDR1AD7 = 1
#define DEBUG2_ON                                         PT1AD_PT1AD7 = 1
#define DEBUG2_OFF                                        PT1AD_PT1AD7 = 0
#define TOGGLE_DEBUG2                                     PT1AD_PT1AD7 ^= (u8)1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PERP_PERP2 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRP_DDRP2 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PTP_PTP2                                    

//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                    

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 

//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERT_PERT0 = 0;\
/* set data direction register to input */                  DDRT_DDRT0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTT_PTT0 == 0u

#endif // INTERFACE_TYPE

//EXTERNAL OSCILLATOR (If not present)
#if defined NO_EXTERNAL_OSC
#define EXTAL_SET                                         {\
/* Set data direction register */                           DDRE_DDRE0 = 1;\
/* Pull down device enabled    */                           PUCR_PDPEE = 1;\
/* Set low output */                                        PORTE_PE0  = 0;\
                                                          }

#define XTAL_SET                                          {\
/* Set data direction register */                           DDRE_DDRE1 = 1;\
/* Pull down device enabled    */                           PUCR_PDPEE = 1;\
/* Set low output */                                        PORTE_PE1  = 0;\
                                                          }
#endif

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#if defined ECONOMY_INPUT
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
#else
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 1;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      PT1AD_PT1AD4 = 0;\
                                                          }
#endif
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  
//INPUT ANALOG
#if defined(AN_INPUT)
#define INPUTAN_DISABLE_DIGITAL_INPUT                     {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#endif


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    (PTIT_PTIT3==1u)
#define PWM_IN_IS_LOW                                     (PTIT_PTIT3==0u)                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)

#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
}
/* Disable SCI control (BOOTL) of diagnostic pin */         /* SCI0CR2_TE = 0;\ */

#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

  //******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
  #if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
    #define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
    #define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

    #ifdef DIAGNOSTIC_STAND_ALONE
      #define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
      /* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                                  PERS_PERS1 = 0;\
      /* Transmitter disabled */                                  SCI0CR2_TE = 0;\
      /* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\                                                          } 
    #else
      #error ("diagnostic not standalone is not possible on PCB228B") 
    #endif  // DIAGNOSTIC_STAND_ALONE
                                                          
    #define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
    #define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
    #define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

  //******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
  #elif ( defined(NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined(DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
    #define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
    #define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1u

    #ifdef DIAGNOSTIC_STAND_ALONE
      #define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
      /* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                                  PERS_PERS1 = 0;\
      /* Transmitter disabled */                                  SCI0CR2_TE = 0;\
      /* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                                }
    #else
      #define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
      /* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
      /* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                                }
    #endif  // DIAGNOSTIC_STAND_ALONE
                                                          
    #define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
    #define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
    #define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

  //******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
  #else
    #error ("diagnostic logic not selected")
  #endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


  //******************** DIAGNOSTIC SHORT SW PROTECTION ********************
  #ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
    #ifdef DIAGNOSTIC_STAND_ALONE
    #else
      #define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
    #endif  // DIAGNOSTIC_STAND_ALONE
  #endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

  #define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE(u16TPM3Freq)     {TC4 = (u16)((u16)TC4+u16TPM3Freq);} /*PRQA S 3453 #This function-like macro has been tested and consolidated so it is not necessary to modify it */       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

  #define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                            if ( TIE_C4I == (u16)0 )\
                                                            {\
                                                              INT0_Set_u16DiagPeriodCount(0);\
                                                              INT0_Set_u16DiagPeriodDuty(u16TPM3Duty);\
  /* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+u16TPM3Freq);\
  /* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                            }\
                                                            }

  #define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                              DIAGNOSTIC_OFF;\
                                                              INT0_Set_u16DiagPeriodCount(0);\
                                                              INT0_Set_u16DiagPeriodDuty(0);\
  /* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
  /* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                            }

  #define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                              INT0_Set_u16DiagPeriodDuty(u16TPM3Duty);\
                                                            }
                                                            
  //******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!

        
//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//BEMF TACHO						
#define	BEMF_CAPTURE_REGISTER                             (TC1)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_BEMF_ENABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC1;\
/* Isr channel 2 is enabled */                              TIE_C1I = 1;\
                                                          }

#define INTERRUPT_BEMF_DISABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC1;\
/* Isr channel 2 is disabled */                             TIE_C1I = 0;\
                                                          }

#define	INTERRUPT_BEMF_CLEAR                              (void)TC1                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define I_BATT_CONV_257HZ                                 ((u8)2) // AN2 -> fbw = 257 Hz, AN6 -> fbw = 27 Hz
#define T_AMB_TLE                                         ((u8)3)

#ifndef AN_INPUT
#define ECONOMY_CONV                                      ((u8)4)
#else
#define AN_CONV                                           ((u8)4)
#define PWM_TO_AN_CONV                                    ((u8)7)
#endif

#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6) // AN2 -> fbw = 257 Hz, AN6 -> fbw = 27 Hz
#define DIAG_CONV                                         ((u8)7)  

//	Possibles inverter status
#define UNDER_VOLTAGE                                     ((u8)0x01)
#define OVER_VOLTAGE                                      ((u8)0x02)
#define TAMB_OVER_TEMPERATURE                             ((u8)0x08)                                      // Private flag to store inverter state
#define UNDER_CURRENT                                     ((u8)0x40)                                      // Private flag to store inverter state
#define OVER_CURRENT                                      ((u8)0x80)                                      // Private flag to store inverter state

// ADC peripheral bits and bitfields definitions
#define CONVERT_AIN0	((u8)	0)
#define CONVERT_AIN1	((u8) ATDCTL5_CA_MASK )                                                             
#define CONVERT_AIN2	((u8) ATDCTL5_CB_MASK )                                                             
#define CONVERT_AIN3	((u8) ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN4	((u8) ATDCTL5_CC_MASK )                                                             
#define CONVERT_AIN5	((u8) ATDCTL5_CC_MASK + ATDCTL5_CA_MASK )                                           
#define CONVERT_AIN6	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK )                                           
#define CONVERT_AIN7	((u8) ATDCTL5_CC_MASK + ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )                         
#define CONVERT_AIN8	((u8) ATDCTL5_CD_MASK )                                                             
#define CONVERT_AIN9	((u8) ATDCTL5_CD_MASK + ATDCTL5_CA_MASK	)                                           
#define CONVERT_AIN10	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	)                                           
#define CONVERT_AIN11	((u8) ATDCTL5_CD_MASK + ATDCTL5_CB_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN12	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	)                                           
#define CONVERT_AIN13	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CA_MASK )                         
#define CONVERT_AIN14	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK )                         
#define CONVERT_AIN15	((u8) ATDCTL5_CD_MASK + ATDCTL5_CC_MASK	+ ATDCTL5_CB_MASK + ATDCTL5_CA_MASK )       

#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF

#elif ( PCB == PCB258A )

/* Public Constants -------------------------------------------------------- */

/* Public type definition -------------------------------------------------- */

//////////////////////////// Parameters ADC //////////////////////////////////

//PORT_T0
#define PORT_T0_SET                                       {\
/* Set data direction register */                           DDRT_DDRT0 = 1;\
/* Pull down device enabled    */                           PERT_PERT0 = 1;\
/* Set low output */                                        PTT_PTT0   = 0;\
                                                          }

//SET POINT
//#define SET_FREQUENCY_SET_POINT(u8InRef)                  (u16)((K_SETPOINT_PU)*(u8InRef)) /*PRQA S 3453 #This function-like macro has been tested and consolidated so it is not necessary to modify it */

//LIN Interface
#define LIN_TRANSCEIVER_ON                                {\
/* set PS6 direction */                                     DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                         PTS_PTS6 = 1 ;\
                                                           }

#define LIN_TRANSCEIVER_OFF                                {\
/* set PS6 direction */                                     DDRS_DDRS6 = 1 ;\
/* set PS6 value */                                         PTS_PTS6 = 0 ;\
                                                           }
//RVP
#define SET_RVP_OUT                                       DDRS_DDRS4 = 1  
#define RVP_ON                                            PTS_PTS4 = 1
#define RVP_OFF                                           PTS_PTS4 = 0

//V_BATT_ON
#define SET_V_BATT_ON_OUT                                 DDRP_DDRP0 = 1  
#define V_BATT_ON_ENABLE                                  PTP_PTP0 = 1
#define V_BATT_ON_DISABLE                                 PTP_PTP0 = 0

//DEBUG1
#define SET_DEBUG1_OUT                                    DDRP_DDRP2 = 1
#define DEBUG1_ON                                         PTP_PTP2 = 1
#define DEBUG1_OFF                                        PTP_PTP2 = 0
#define TOGGLE_DEBUG1                                     PTP_PTP2 ^= 1

//DEBUG2
#define SET_DEBUG2_OUT                                    DDRS_DDRS7 = 1
#define DEBUG2_ON                                         PTS_PTS7 = 1
#define DEBUG2_OFF                                        PTS_PTS7 = 0
#define TOGGLE_DEBUG2                                     PTS_PTS7 ^= 1

//TIMER MODULE (TIM)
#define TIM_TIMER_COUNT                                   TCNT
#define	TIM_TIMER_OVF  			                              (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define TIM_CLK_ENABLE                                    TSCR1_TEN = 1                                   // Allows the timer to function normally
#define TIM_CLK_DISABLE                                   TSCR1_TEN = 0                                   // Disables the main timer, including the counter
//#define TIM_CLEAR_PENDING_OF_INTERRUPT                    TFLG2_TOF = 1                                 // Timer Overflow Flag. Set when 16-bit free-running timer overflows from 0xFFFF to 0x0000. Clearing this bit requires writing a one to bit 7 of TFLG2 register while the TEN bit of TSCR1 or PAEN bit of PACTL is set to one.
#define TIM_CLEAR_PENDING_OF_INTERRUPT                    (void)TCNT                                      // Any access to TCNT will clear TFLG2 register if the TFFCA bit in TSCR register is set.

#define TIM_OF_ISR_ENABLE                                 {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is Enabled */                         TSCR2_TOI = 1;\
                                                          }

#define TIM_OF_ISR_DISABLE                                {\
/* reset Timer Overflow isr flag */                         TFLG2_TOF = 1;\
/* Timer Overflow irs is disabled */                        TSCR2_TOI = 0;\
                                                          }

//GATE DRIVER
#define GD_SET_ERROR_LINE_INPUT                           {\
/* set data direction register */                           PUCR_PDPEE = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      DDRE_DDRE0 = 0;\
                                                          }                                                         
#define GD_CHECK_ERROR_LINE                               PORTE_PE0                                    

                                                          
//ZCE CNTRL
#define ZCE_CNTRL_RESET_OUT                               {DDRM_DDRM1 = 0}
#define ZCE_CNTRL_SET_OUT                                 DDRM_DDRM1 = 1                              
#define ZCE_CNTRL_ON                                      PTM_PTM1 = 1                                
#define ZCE_CNTRL_OFF                                     PTM_PTM1 = 0                               

//STAND-BY
#define STAND_BY_RESET_OUT                                DDRM_DDRM0 = 0                                  
#define STAND_BY_SET_OUT                                  DDRM_DDRM0 = 1                                  
#define STAND_BY_ON                                       PTM_PTM0 = 1                                    
#define STAND_BY_OFF                                      PTM_PTM0 = 0                                      

//HI_Z_BRIDGE
#define HI_Z_BRIDGE_RESET_OUT                             DDRS_DDRS5 = 0                                  
#define HI_Z_BRIDGE_SET_OUT                               DDRS_DDRS5 = 1                                  
#define HI_Z_BRIDGE_REMOVE_PULL_RESISTOR                  PERS_PERS5 = 0                                  
#define HI_Z_BRIDGE_ON                                    PTS_PTS5 = 1                                    
#define HI_Z_BRIDGE_OFF                                   PTS_PTS5 = 0 


//INPUT KEY
#if ( ( INTERFACE_TYPE == INTERFACE_TYPE_4 ) || ( INTERFACE_TYPE == INTERFACE_TYPE_5 ) )

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PTS_PTS0 == 1                                    

#else

#define SET_KEY_ON_OFF_INPUT                              {\
/* Pull device disabled */                                  PUCR_PDPEE = 0;\
/* set data direction register to input */                  DDRE_DDRE1 = 0;\
                                                          }
#define TEST_KEY_ON_OFF                                   PORTE_PE1 == 0u

#endif // INTERFACE_TYPE

//INPUT NTC
#define INPUTNTC_SET_PULLUP                               {\
                                                            PERS_PERS0 =0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* Set high output */                                       PTP_PTP2 = 1;\
/* Associated pin configured as output */                   DDRP_DDRP2 = 1;\
                                                          } 

//INPUT ANALOG
#define INPUTAN_DISABLE_DIGITAL_INPUT                     {\
/* Pull device disabled */                                  PERS_PERS0 = 0;\
/* set data direction register to input */                  DDRS_DDRS0 = 0;\
                                                          }
                                                          
#define INPUTAN_CLEAR_PENDING_INTERRUPT                   ATDSTAT2_CCF2 = 1
#define INPUTAN_SET_RISING_EDGE_SENS                      {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_HIGH_BIT;\
/* result of conversion n is higher than ATDDRn */          ATDCMPHT = 4;\
                                                          }

//INPUT_ECONOMY   
#define INPUTECON_INIT                                    {\
/* set data direction register */                           DDR1AD_DDR1AD4 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN4 = 1;\
                                                          }
                                                          
//INPUT_ECONOMY  
#define INPUT_FEEDBACK_ANA_INIT                           {\
/* set data direction register */                           DDR1AD_DDR1AD7 = 0;\
/* MC9S12G Family Reference Manual, Rev. 0.41 pg 95 */      ATDDIEN_IEN7 = 1;\
                                                          }                                                          
 
  


#define INPUTAN_SET_FALLING_EDGE_SENS                     {\
/* Write compare value to ATDDRn result register */         ATDDR2 = ANALOG_PWM_LOW_BIT;\
/* result of conversion n is lower or same than ATDDRn */   ATDCMPHT = 0;\
                                                          }

#define ANALOG_RESET_PWM_FEATURE                          {\
/* No automatic compare */                                  ATDCMPE = 0;\
/* ATD Compare interrupt requests are disabled */           ATDCTL2_ACMPIE = 0;\
                                                          }

//INPUT PWM
#define INPUTPWM_TIMER                                    TCNT
#define INPUTPWM_PIO_READING                              PTT_PTT3
#define INPUTPWM_DETECTING_EDGE_FLAG                      TCTL4_EDG3A
#define INPUTPWM_CLEAR_PENDING_INTERRUPT                  {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* If TFFCA is set, CH reading causes flag CxF clearing */  (void)(TC3);\
                                                          }
#define INPUTPWM_SET_FALLING_EDGE_SENS                    TCTL4_EDG3x = 2U;                               // PWM input (IOC3) -> Capture on falling edges only
#define INPUTPWM_SET_RISING_EDGE_SENS                     TCTL4_EDG3x = 1U;                               // PWM input (IOC3) -> Capture on rising edges only
#define	INPUTPWM_CAPTURE_REGISTER			                    (TC3)
#define	INPUTPWM_COUNT_OVF  			                        (TFLG2_TOF)                                     // reset Timer Overflow isr flag
#define INPUTPWM_INTERRUPT_ENABLE                         {\
                           /* 0225-A Debug*/                 PERS_PERS0 = 0;\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is enabled */                      TIE_C3I = 1;\
                                                          }

#define INPUTPWM_INTERRUPT_DISABLE                        {\
/* reset pwm input isr flag */                              TFLG1_C3F = 1;\
/* pwm input isr channel is disabled */                     TIE_C3I = 0;\
                                                          }
                                                          
#define PWM_IN_IS_HIGH                                    (PTIT_PTIT3==1u)
#define PWM_IN_IS_LOW                                     (PTIT_PTIT3==0u)                                                          

//RTI
#define RTI_START                                         { CPMUCLKS_RTIOSCSEL = 0; CPMURTI = (u8)(128u + (RTI_PERIOD_MSEC-1u)); }     // After writing CPMURTI register RTI starts.
#define RTI_ACK                                           { CPMUFLG_RTIF = 1; }                           // Used in the clearing mechanism (set bits cause corresponding bits to be cleared).
#define RTI_ENABLE_INTERRUPT                              {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 1;\
                                                          }

#define RTI_DISABLE_INTERRUPT                             {\
/* reset isr flag */                                        CPMUFLG_RTIF = 1;\
/* isr channel is enabled */                                CPMUINT_RTIE = 0;\
                                                          }
                                                          
//******************** DIAGNOSTIC ********************

//******************** DIAGNOSTIC RESET ********************
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC)


#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=1;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 0;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          } 

#elif defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC)


#define DIAGNOSTIC_RESET_PORT                             { PTP_PTP2=0;\
/* Output operates as push-pull output */                   DDRP_DDRP2=0;\
/* Pull device disabled */                                  PERP_PERP2 = 0;\
/* DISABLE BOOTLOADER */                                    DDRS_DDRS1 = 1;\
/* Output buffer operates as input */                       WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#else

#define DIAGNOSTIC_RESET_PORT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Set low output */                                        PTS_PTS1 = 0;\
/* Pull device disabled */                                  PERS_PERS1 = 0;\
/* Disable SCI control (BOOTL) of diagnostic pin */         SCI0CR2_TE = 0;\
                                                          }
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC                                                                                                                   


//******************** DIAGNOSTIC SETTINGS ******************** 
#if ( !( DIAGNOSTIC_STATE == DIAGNOSTIC_DISABLE ) )

//******************** POSITIVE DIAGNOSTIC SETTINGS ********************                                                          
#if defined (POSITIVE_ANALOG_DIAGNOSTIC_LOGIC) 
						
#define DIAGNOSTIC_PORT_BIT                               PTIP_PTIP2                                        
#define DIAGNOSTIC_TOGGLE_PORT                            PTP_PTP2^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 0;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 1;\
                                                          } 
#else
 #error ("diagnostic not standalone is not possible on PCB225C") 
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTP_PTP2=1                                                                                                
#define DIAGNOSTIC_OFF                                    PTP_PTP2=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRP_DDRP2=0

//******************** NEGATIVE DIAGNOSTIC SETTINGS ******************** 
#elif ( defined (NEGATIVE_ANALOG_DIAGNOSTIC_LOGIC) || defined (DIAGNOSTIC_SHORT_DIGITAL_PROTECTION) )
                                                          						
#define DIAGNOSTIC_PORT_BIT                               PTS_PTS1                                       
#define DIAGNOSTIC_TOGGLE_PORT                            PTS_PTS1^=1

#ifdef DIAGNOSTIC_STAND_ALONE
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Disable Bootloader */                                    WOMS_WOMS1 = 0;\
                                                            PERS_PERS1 = 0;\
/* Transmitter disabled */                                  SCI0CR2_TE = 0;\
/* Output buffer operates as push-pull output */            DDRP_DDRP2 = 0;\
                                                          }
#else
#define DIAGNOSTIC_SET_OUTPUT                             { DDRS_DDRS1 = 1;\
/* Output buffer operates as push-pull output */            WOMS_WOMS1 = 0;\
/* Shorting pin check: set data direction register  */      DDRS_DDRS0 = 0;\
                                                          }
#endif  // DIAGNOSTIC_STAND_ALONE
                                                          
#define DIAGNOSTIC_ON                                     PTS_PTS1=1                                                                                              
#define DIAGNOSTIC_OFF                                    PTS_PTS1=0                                      
#define DIAGNOSTIC_REMOVE_OUTPUT                          DDRS_DDRS1=0

//******************** DIAGNOSTIC LOGIC NOT DEFINED ******************** 
#else
  #error ("diagnostic logic not selected")
#endif  // POSITIVE_ANALOG_DIAGNOSTIC_LOGIC


//******************** DIAGNOSTIC SHORT SW PROTECTION ********************
#ifdef DIAGNOSTIC_SHORT_SW_PROTECTION
#ifdef DIAGNOSTIC_STAND_ALONE
#else
#define IS_DIAGNOSTIC_SHORTED                             PTIS_PTIS0                         
#endif  // DIAGNOSTIC_STAND_ALONE
#endif  // DIAGNOSTIC_SHORT_SW_PROTECTION

#define	DIAGNOSTIC_NEW_INTERRUPT_CAPTURE(u16TPM3Freq)     {TC4 = (u16)((u16)TC4+u16TPM3Freq);} /*PRQA S 3453 #This function-like macro has been tested and consolidated so it is not necessary to modify it */       // Next pwm isr in 50uS. Timer modul works with 5Mhz.

  #define DIAGNOSTIC_SET_SIGNAL(u16TPM3Freq, u16TPM3Duty)   {\
                                                            if ( TIE_C4I == (u16)0 )\
                                                            {\
                                                              INT0_Set_u16DiagPeriodCount(0);\
                                                              INT0_Set_u16DiagPeriodDuty(u16TPM3Duty);\
  /* Next tc4 isr after duty time */                          TC4 = (u16)((u16)TCNT+u16TPM3Freq);\
  /* Isr channel 4 is enabled */                              TIE_C4I = 1;\
                                                            }\
                                                            }

  #define DIAGNOSTIC_REMOVE_SIGNAL                          {\
                                                              DIAGNOSTIC_OFF;\
                                                              INT0_Set_u16DiagPeriodCount(0);\
                                                              INT0_Set_u16DiagPeriodDuty(0);\
  /* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC4;\
  /* Isr channel 4 is enabled */                              TIE_C4I = 0;\
                                                            }

  #define DIAGNOSTIC_UPDATE_DUTY_ERROR(u16TPM3Duty)         {\
                                                              INT0_Set_u16DiagPeriodDuty(u16TPM3Duty);\
                                                            }
                                                          
//******************** DIAGNOSTIC SHORT SW PROTECTION ********************                                                          

#endif  // DIAGNOSTIC_STATE

//******************** DIAGNOSTIC ********************

//RESET MICRO						
#define RESET_MICRO                                       CPMUARMCOP = 0                                  // Wrong watchdog writing -> reset!


        

//TACHO						
#define	TACHO_CAPTURE_REGISTER                            (TC2)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_TACHO_ENABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is enabled */                              TIE_C2I = 1;\
                                                          }

#define INTERRUPT_TACHO_DISABLE                           {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC2;\
/* Isr channel 2 is disabled */                             TIE_C2I = 0;\
                                                          }

#define	INTERRUPT_TACHO_CLEAR                             (void)TC2                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

//BEMF TACHO						
#define	BEMF_CAPTURE_REGISTER                             (TC1)                                           // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared

#define INTERRUPT_BEMF_ENABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC1;\
/* Isr channel 2 is enabled */                              TIE_C1I = 1;\
                                                          }

#define INTERRUPT_BEMF_DISABLE                            {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC1;\
/* Isr channel 2 is disabled */                             TIE_C1I = 0;\
                                                          }

#define	INTERRUPT_BEMF_CLEAR                              (void)TC1
                                                                                                          

//TIMER 1						
#define ENABLE_PWM_TIMER                                  {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00 */                               PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is enabled */                      PWME = 0x2A;\
                                                          }

#define DISABLE_PWM_TIMER                                 {\
/* reset counter to to $00 */                               PWMCNT01 = 0;\
/* reset counter to to $00g */                              PWMCNT23 = 0;\
/* reset counter to to $00 */                               PWMCNT45 = 0;\
/* Pulse width channel x is disabled */                     PWME &= (u8)~((u8)0x2A);\
                                                          }

#define INTERRUPT_PWM_ENABLE                              {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Next pwm isr in 50uS */                                  TC0 = (u16)(TCNT + PWM_NEXT_ISR);\
/* Channel 0 isr enabling */                                TIE_C0I = 1U;\
                                                          }

#define INTERRUPT_PWM_DISABLE                             {\
/* reset isr flag ( TFFCA = 1 ) !!! */                      (void)TC0;\
/* Channel 0 isr disabling */                               TIE_C0I = 0;\
/* Multi-Channel Sample Mode disabled */                    ATDCTL5_MULT = 0;\
                                                          }

#define	INTERRUPT_PWM_CLEAR                               (void)TC0                                       // N.B.
                                                                                                          // Since TFFCA bit in TSCR register is set, a read from an input capture or a write into an output compare
                                                                                                          // channel (0x0010–0x001F) will cause the corresponding channel flag CxF to be cleared
#define	INTERRUPT_PWM_NEW_CAPTURE                         TC0 = (u16)((u16)TC0+PWM_NEXT_ISR)              // Next pwm isr in 50uS. PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz

//PWM: Pulse Witdh Modulation (Timer1 Overflow)								
#define	BRAKING_ON_LOW                                    {PWMDTY01 = 0U; PWMDTY23 = 0U; PWMDTY45 = 0U;}  // Braking on low side Mos			

// ADC module
#define ADC_CLEAR_PENDING_CONVERTION_FLAG ATDSTAT0_SCF = 1                                                // This flag is set upon completion of a conversion sequence. This flag is cleared when one of the following occurs: A) Write “1” to SCF
#define ADC_READING_LENGHT                                ((u8)8)                                         // Weight lenght. Must be 2 multiple

#define I_PEAK_CONV                                       ((u8)0)
#define V_BUS_CONV                                        ((u8)1)
#define I_BATT_CONV_257HZ                                 ((u8)2) // AN2 -> fbw = 257 Hz, AN6 -> fbw = 27 Hz
#define T_AMB_TLE                                         ((u8)3)

#ifndef AN_INPUT
#define ECONOMY_CONV                                      ((u8)4)
#else
#define AN_CONV                                           ((u8)4)
#define PWM_TO_AN_CONV                                    ((u8)7)
#endif

#define T_MOS_CONV                                        ((u8)5)
#define I_BATT_CONV                                       ((u8)6) // AN2 -> fbw = 257 Hz, AN6 -> fbw = 27 Hz
#define DIAG_CONV                                         ((u8)7)  


#define TLE_TEMP_SENSOR_CHANNEL   CONVERT_AIN3                                       
#define ANALOG_INPUT_CHANNEL      CONVERT_AIN2                                       
#define BUS_VOLTAGE_CHANNEL       CONVERT_AIN1                                       
#define CURRENT_INPUT_CHANNEL     CONVERT_AIN0                                       
#define ECONOMY_INPUT_CHANNEL     CONVERT_AIN4  
#define DIAG_SHORT_MON_CHANNEL    CONVERT_AIN4       //Monitor for diag short circuit condition


//SCI
#define Serial_CR1      SCI0CR1
#define Serial_CR2      SCI0CR2
#define SCIxBD          SCI0BD
#define Serial_SR1      SCI0SR1
#define Serial_TxData   SCI0DRL
#define Serial_fTIE     SCI0CR2_TIE
#define Serial_fTDRE    SCI0SR1_TDRE
#define Serial_fRIE     SCI0CR2_RIE
#define Serial_fRDRF    SCI0SR1_RDRF 
#define Serial_maskRDRF SCI0SR1_RDRF_MASK
#define Serial_fOR      SCI0SR1_OR
#define Serial_fTC      SCI0SR1_TC
#define _SRS_LVD        CPMUFLG_LVRF
#define _SRS_ICG        CPMUFLG_UPOSC
#define _SRS_ILOP       CPMUFLG_ILAF     
#define _SRS_COP        0          
#define _SRS_PIN        0  
#define _SRS_POR        CPMUFLG_PORF

#endif    /* PCB*/

#endif
/*** (c) 2007 SPAL Automotive ****************** END OF FILE **************/


