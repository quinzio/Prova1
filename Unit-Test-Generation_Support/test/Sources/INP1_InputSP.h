/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INP1_InputSP.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INP1_InputSP.h
    \version see \ref FMW_VERSION 
    \brief Basic pwm input function prototypes.
    \details Routines for pwm input operation and variables related managing.\n
    \note none
    \warning none
*/

#ifndef INPUT_SP_H
#define INPUT_SP_H


/* Public Constants -------------------------------------------------------- */
/* Estremi e coefficiente angolare della FdT */
#define K_SETPOINT_PU_FLOAT                 (float32_t)((float32_t)FREQUENCY_RES_FOR_PU_BIT/(float32_t)255)
#define K_SETPOINT_PU_INT                   (u16)((float32_t)FREQUENCY_RES_FOR_PU_BIT/(float32_t)255)
#define K_SETPOINT_PU                       ( ( K_SETPOINT_PU_FLOAT > (float32_t)K_SETPOINT_PU_INT ) ? (K_SETPOINT_PU_INT+(u16)1) : K_SETPOINT_PU_INT )

#define SP_MIN_SPEED_VALUE_FLOAT            (float32_t)((float32_t)((float32_t)((float32_t)EL_FREQ_MIN*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)/(float32_t)K_SETPOINT_PU_FLOAT)
#define SP_MIN_SPEED_VALUE_INT              (u16)((float32_t)((float32_t)((float32_t)EL_FREQ_MIN*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)/(float32_t)K_SETPOINT_PU_FLOAT)
#define SP_MIN_SPEED_VALUE                  ( ( ( SP_MIN_SPEED_VALUE_FLOAT > (float32_t)SP_MIN_SPEED_VALUE_INT ) && ( (u16)SP_MIN_SPEED_VALUE_FLOAT <= (u16)254 ) )  ? (SP_MIN_SPEED_VALUE_INT+(u16)1) : SP_MIN_SPEED_VALUE_INT )

#define SP_MAX_SPEED_VALUE_FLOAT            (float32_t)((float32_t)((float32_t)((float32_t)EL_FREQ_NOMINAL*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)/(float32_t)K_SETPOINT_PU_FLOAT)
#define SP_MAX_SPEED_VALUE_INT              (u16)((float32_t)((float32_t)((float32_t)EL_FREQ_NOMINAL*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)BASE_FREQUENCY_HZ)/(float32_t)K_SETPOINT_PU_FLOAT)
#define SP_MAX_SPEED_VALUE                  ( ( ( SP_MAX_SPEED_VALUE_FLOAT > (float32_t)SP_MAX_SPEED_VALUE_INT ) && ( (u16)SP_MAX_SPEED_VALUE_FLOAT <= (u16)254 ) )  ? (SP_MAX_SPEED_VALUE_INT+(u16)1) : SP_MAX_SPEED_VALUE_INT )

#define SP_EMERGENCY_SPEED                  SP_MAX_SPEED_VALUE  /* 255 */

#if( FAN_TYPE == BLOWER )

  #define	PWM_IN_0  (u8)42    /* Duty  20% */
  #define	PWM_IN_1  (u8)92    /* Duty  40% */
  #define	PWM_IN_2  (u8)145   /* Duty  60% */
  #define	PWM_IN_3  (u8)197   /* Duty  80% */
  #define	PWM_IN_4  (u8)222   /* Duty  90% */
  #define	PWM_IN_5  (u8)243   /* Duty  95% */

  #define	K0_0      (u8)(((u16)(((u32)RPM_MIN*POLE_PAIR_NUM*128)/60) + ((u16)I_FEEDBACK_LIM*K1_BLOWER_SET_POINT)) / K0_BLOWER_SET_POINT)    /* Duty 20% */
  #define	K0_1      (u8)87    /* Duty  40% */
  #define	K0_2      (u8)114   /* Duty  60% */
  #define	K0_3      (u8)157   /* Duty  80% */
  #define	K0_4      (u8)185   /* Duty  90% */
  #define	K0_5      (u8)245   /* Duty  95% */

  #define K0_SLOPE_1  	(u16)(((u16)(K0_1 - K0_0)*256)/((u8)(PWM_IN_1 - PWM_IN_0)))
  #define K0_SLOPE_2  	(u16)(((u16)(K0_2 - K0_1)*256)/((u8)(PWM_IN_2 - PWM_IN_1)))
  #define K0_SLOPE_3  	(u16)(((u16)(K0_3 - K0_2)*256)/((u8)(PWM_IN_3 - PWM_IN_2)))
  #define K0_SLOPE_4  	(u16)(((u16)(K0_4 - K0_3)*256)/((u8)(PWM_IN_4 - PWM_IN_3)))
  #define K0_SLOPE_5  	(u16)(((u16)(K0_5 - K0_4)*256)/((u8)(PWM_IN_5 - PWM_IN_4)))

  #define K0_BLOWER_SET_POINT                 (u8)255
  #define K1_BLOWER_SET_POINT                 (u8)135 

  #define K1_BLOWER_SET_POINT_PU              (u8)((u8)K1_BLOWER_SET_POINT/BASE_CURRENT_CONV_FACT_RES)
  #define K0_MINIMUM_BLOWER_SET_POINT         EL_FREQ_MIN_PU_RES_BIT

  /*Define here speed max and speed min for balance blower test*/
  /*Basing on blower test mode requiremnets, Balance test must be performed at constant speed*/
  #define BLOWER_BALANCE_RPM_MAX              (u16)3700
  #define BLOWER_BALANCE_FREQ_MAX             (float32_t)((float32_t)BLOWER_BALANCE_RPM_MAX*POLE_PAIR_NUM/60)
  #define BLOWER_BALANCE_RPM_MIN              (u16)1100
  #define BLOWER_BALANCE_FREQ_MIN             (float32_t)((float32_t)BLOWER_BALANCE_RPM_MAX*POLE_PAIR_NUM/60)  
  
  #define SP_MAX_SPEED_VALUE_BALANCE          SP_MAX_SPEED_VALUE
  #define SP_MIN_SPEED_VALUE_BALANCE          (u8)((u32)((u32)BLOWER_BALANCE_RPM_MIN*SP_MAX_SPEED_VALUE_BALANCE)/BLOWER_BALANCE_RPM_MAX) 

  #define BLOWER_BALANCE_MAX_FREQ_PU          (u16)((float32_t)((float32_t)BLOWER_BALANCE_FREQ_MAX*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)                   
  #define BLOWER_BALANCE_MIN_FREQ_PU          (u16)((float32_t)((float32_t)BLOWER_BALANCE_FREQ_MIN*FREQUENCY_RES_FOR_PU_BIT)/BASE_FREQUENCY_HZ)
   
  #define K1_BALANCE_BLOWER_SET_POINT         (u16)((u16)(BLOWER_BALANCE_MAX_FREQ_PU-BLOWER_BALANCE_MIN_FREQ_PU)/(u16)(SP_MAX_SPEED_VALUE_BALANCE-SP_MIN_SPEED_VALUE_BALANCE))
  #define K2_BALANCE_BLOWER_SET_POINT         BLOWER_BALANCE_MIN_FREQ_PU 

#else

  #define SP_MAX_SPEED_VALUE_BALANCE          SP_MAX_SPEED_VALUE
  #define SP_MIN_SPEED_VALUE_BALANCE          SP_MIN_SPEED_VALUE

#endif  /* FAN_TYPE */

/* PWM_TO_ANALOG-input parameters */
#ifdef PWM_TO_ANALOG_IN
  #if ( FAN_TYPE == AXIAL )
    #if (PCB==PCB220C)
      #define PWM_TO_ANALOG_TURN_OFF_BIT       ((u16)61)   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        ((u16)102)  /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    ((u16)211)  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    ((u16)635)  /* Duty 92% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    ((u16)650)  /* Duty 94% */
    #elif (PCB==PCB220A) 
     #define PWM_TO_ANALOG_TURN_OFF_BIT        ((u16)83)   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        ((u16)143)  /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    ((u16)279)  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    ((u16)828)  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    ((u16)843)  /* Duty 95% */
    #elif (PCB == PCB225C)
      #define PWM_TO_ANALOG_TURN_OFF_BIT       ((u16)47)   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        ((u16)98)   /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    ((u16)206)  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    ((u16)654)  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    ((u16)662)  /* Duty 95% */
    #elif (PCB == PCB233C)
      #define PWM_TO_ANALOG_TURN_OFF_BIT       ((u16)47)   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        ((u16)98)   /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    ((u16)206)  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    ((u16)654)  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    ((u16)662)  /* Duty 95% */
    #elif (PCB == PCB258A)
      #define PWM_TO_ANALOG_TURN_OFF_BIT       ((u16)47)   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        ((u16)98)   /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    ((u16)206)  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    ((u16)654)  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    ((u16)662)  /* Duty 95% */
    #elif (PCB == PCB262A)
      #define PWM_TO_ANALOG_TURN_OFF_BIT       ((u16)47)   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        ((u16)98)   /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    ((u16)206)  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    ((u16)654)  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    ((u16)662)  /* Duty 95% */
    #else
      #error("define PWM_TO_ANALOG map")
    #endif /* PCB */

    #define PWM_TO_AN_SP_STOP_SPEED 0
    #define PWM_TO_AN_SP_MIN_SPEED_VALUE SP_MIN_SPEED_VALUE
    #define PWM_TO_AN_SP_N_TO_MAX_SPEED_VALUE (u16)245
    #define PWM_TO_AN_SP_MAX_SPEED_VALUE SP_MAX_SPEED_VALUE

    #define PWM_TO_ANALOG_FDT_SLOPE_SHIFT (u16)6 
    #define PWM_TO_AN_FDT_SLOPE (((u16)(PWM_TO_AN_SP_N_TO_MAX_SPEED_VALUE-PWM_TO_AN_SP_MIN_SPEED_VALUE)*((u16)1<<PWM_TO_ANALOG_FDT_SLOPE_SHIFT))/(PWM_TO_ANALOG_N_TO_MAX_SP_BIT-PWM_TO_ANALOG_TURN_MIN_SP_BIT))

  #elif ( FAN_TYPE == BLOWER )

    #if (PCB==PCB220C)
      #define PWM_TO_ANALOG_TURN_OFF_BIT       (u16)61   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        (u16)102  /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    (u16)211  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    (u16)645  /* Duty 92% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    (u16)655  /* Duty 94% */
    #elif (PCB==PCB220A) 
      #define PWM_TO_ANALOG_TURN_OFF_BIT       (u16)83   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        (u16)143  /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    (u16)279  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    (u16)828  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    (u16)843  /* Duty 95% */
    #elif (PCB == PCB225C)
      #define PWM_TO_ANALOG_TURN_OFF_BIT       (u16)47   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        (u16)98   /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    (u16)206  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    (u16)654  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    (u16)662  /* Duty 95% */
    #elif (PCB == PCB233C)  
      #define PWM_TO_ANALOG_TURN_OFF_BIT       (u16)47   /* Duty 3%  */
      #define PWM_TO_ANALOG_TURN_ON_BIT        (u16)98   /* Duty 7%  */
      #define PWM_TO_ANALOG_TURN_MIN_SP_BIT    (u16)206  /* Duty 20% */
      #define PWM_TO_ANALOG_MAX_TO_N_SP_BIT    (u16)654  /* Duty 93% */
      #define PWM_TO_ANALOG_N_TO_MAX_SP_BIT    (u16)662  /* Duty 95% */
    #else
      #error("define PWM_TO_ANALOG map")        
    #endif /* PCB */

    #define PWM_TO_AN_SP_STOP_SPEED 0
    #define PWM_TO_AN_SP_MIN_SPEED_VALUE 42
    #define PWM_TO_AN_SP_N_TO_MAX_SPEED_VALUE 243
    #define PWM_TO_AN_SP_MAX_SPEED_VALUE 255

    #define PWM_TO_ANALOG_FDT_SLOPE_SHIFT 6 
    #define PWM_TO_AN_FDT_SLOPE (((u16)(PWM_TO_AN_SP_N_TO_MAX_SPEED_VALUE-PWM_TO_AN_SP_MIN_SPEED_VALUE)*(1<<PWM_TO_ANALOG_FDT_SLOPE_SHIFT))/(PWM_TO_ANALOG_N_TO_MAX_SP_BIT-PWM_TO_ANALOG_TURN_MIN_SP_BIT))

  #endif  /* FAN_TYPE */
#endif /* PWM_TO_ANALOG_IN */
  
/* ECONOMY-input parameters */
  #define ECONOMY_SPEED_RPM   (u16)1550
  #define ECONOMY_SPEED       (u8)(((u32)ECONOMY_SPEED_RPM*POLE_PAIR_NUM*128)/(60*K_SETPOINT))
  #define ECONOMY_IS_OFF_THRESHOLD  6			/* 6 V */
  #define ECONOMY_IS_ON_THRESHOLD   8			/* 8 V */
  #define ECONOMY_IS_OFF_THRESHOLD_BIT (ECONOMY_IS_OFF_THRESHOLD*DIAG_ALPHA_SHORT_ECON)
  #define ECONOMY_IS_ON_THRESHOLD_BIT  (ECONOMY_IS_ON_THRESHOLD*DIAG_ALPHA_SHORT_ECON)

/* PWM-input parameters */
  #define SP_0PERCENT_DUTY    (u8)0
  #define SP_100PERCENT_DUTY  (u8)255

  #define BUFFER_PWM_IN_POWER                 (u8)3
  #define DIM_BUFFER_PWM_IN                   (u8)((u8)1<<BUFFER_PWM_IN_POWER)

#ifdef PWM_TO_ANALOG_IN
  #define PWM_IN_DISCARDED_DUTY               (u8)2
  #define PWM_IN_PERIOD_EVALUATING_NUMBER     (u8)3
  #define PWM_IN_VALID_PERIOD_NUMBER          (u8)3        /* Number of good pwm input period requested to update speed set point */
  #define PWM_IN_SET_PERIOD_HZ                (u16)10      /* Pwm input period set in hz */
  #define PWM_IN_PERIOD_TOLERANCE_HZ          (u16)1
#else
  #define PWM_IN_DISCARDED_DUTY               (u8)3
  #define PWM_IN_PERIOD_EVALUATING_NUMBER     (u8)6
  #define PWM_IN_VALID_PERIOD_NUMBER          (u8)4
  #define PWM_IN_SET_PERIOD_HZ                (u16)297
  #define PWM_IN_PERIOD_TOLERANCE_HZ          (u16)252
#endif

  #define PWM_IN_SET_PERIOD_TIMER_COUNT       (u32)((u32)5000000/PWM_IN_SET_PERIOD_HZ)                                      /* Number of timer ticks into a pwm input period. */
  #define PWM_IN_MIN_PERIOD_TIMER_COUNT       (u32)((u32)5000000/(u16)(PWM_IN_SET_PERIOD_HZ+PWM_IN_PERIOD_TOLERANCE_HZ))
  #define PWM_IN_MAX_PERIOD_TIMER_COUNT       (u32)((u32)5000000/(u16)(PWM_IN_SET_PERIOD_HZ-PWM_IN_PERIOD_TOLERANCE_HZ))
  #define PWM_IN_VALID_DUTY_NUMBER            (u8)PWM_IN_VALID_PERIOD_NUMBER                                              /* Number of good pwm input duty cycle requested to update speed set point. */
  #define PWM_IN_DUTY_TOL_PERCENT             (u16)2                                                                      /* Pwm input period tolerance in hz.                                        */
  #define PWM_IN_DUTY_TOL_NUMBER              (u8)((u16)((u16)PWM_IN_DUTY_TOL_PERCENT*(u16)255)/(u16)100)                 /* Pwm input duty cycle tolerance in percent over 255.                      */

  #define PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER   (u8)8                                                                       /* Number of pwm input period to detect disappear */
  #define PWM_IN_DISAPPEAR_NORMAL_OP_NUM      (u8)((u8)((u16)((u16)125*PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER)/(u16)(PWM_IN_SET_PERIOD_HZ-PWM_IN_PERIOD_TOLERANCE_HZ))+(u16)2)  /* Formula is computed i this way: PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER * PWMinputPeriod / 8 ms */
  
  #define PWM_TO_QUIESCENT_NORMAL_OP_MSEC     (u16)2000       /* Maximum value is 2000 msec */
  #define PWM_TO_QUIESCENT_NORMAL_OP_NUM      (u8)((u16)PWM_TO_QUIESCENT_NORMAL_OP_MSEC/TIMEBASE_MSEC)  /* Formula is computed i this way: PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER * PWMinputPeriod / 8mS */

  #ifdef KEY_SHORT_LOGIC
    #define MAXIMUM_POSTIVE_LEVEL_COUNT         (u8)2
    #define KEY_CHECK_WITH_NO_PWM_TIME_MSEC     (u16)100  /* [ms] */
    #define KEY_CHECK_WITH_NO_PWM_TIME_NUM      (u8)((u16)KEY_CHECK_WITH_NO_PWM_TIME_MSEC/TIMEBASE_MSEC)  /* Formula is computed i this way: PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER * PWMinputPeriod / 8mS */
  #endif  /* KEY_SHORT_LOGIC */                                                                           /* +2 takes into account incorrelation between pwm input and main sw. If pwm input edge, base time isr and disappearing checking happen in this sequence, */

#ifdef PWM_TO_ANALOG_IN                                                                                 /* first, pwm input edge set u8NumNoDuty to zero then base time isr increment u8NumNoDuty to one and immediatilly after (since base time, pwm input and main flow are incorrelated) disappearing checking found mistake! */
  #define PWM_TO_ANALOG_MAX_PLAUSIBILITY_FAILS  (u8)2
  #define PWM_IN_PERIOD_TO_WAIT_NUMBER          (u8)(PWM_IN_PERIOD_EVALUATING_NUMBER+(u8)1)                                     /* Number of pwm input period to wait in order to check initial plausibility and operating mode */
#else
  #define PWM_IN_PERIOD_TO_WAIT_NUMBER          (u8)((u8)2*(PWM_IN_PERIOD_EVALUATING_NUMBER+(u8)1))
#endif 

  #define PWM_IN_INPUT_TEST_TIME_BASE         (u8)((u16)((u16)1000*PWM_IN_PERIOD_TO_WAIT_NUMBER)/(u16)((u16)8*(PWM_IN_SET_PERIOD_HZ-PWM_IN_PERIOD_TOLERANCE_HZ)))
                                                                                                                            /* Formula is computed i this way: 3 * (PWM_IN_PERIOD_EVALUATING_NUMBER+1) * pwm input period/8e-3 */ /* where 8e-3 is timer base time */
  #define PWM_IN_VALID_NORMAL_OPERATION_COUNT (u8)(PWM_IN_PERIOD_TO_WAIT_NUMBER/(u8)2)                                         /* Number of conseguitve normal operation InputSP_FindOperatingMode has to found in order to set  NORMAL_OPERATION mode. */

  #define PWM_IN_HALT_TO_EMERGENCY            (u8)(((u16)((float32_t)4.5*(float32_t)255))/100u)       /* 4.0 %  */
  #define PWM_IN_EMERGENCY_TO_HALT            (u8)(((u16)((float32_t)5.2*(float32_t)255))/100u)       /* 6.0 %  */

  #define PWM_IN_HALT_TO_MIN                  (u8)(((u16)((float32_t)15.5*(float32_t)255))/100u)      /* 16.0 % */
  #define PWM_IN_MIN_TO_HALT                  (u8)(((u16)((float32_t)12.5*(float32_t)255))/100u)      /* 12.0 % */

  #define PWM_IN_MIN_TO_NOMINAL               (u8)(((u16)((float32_t)15.5*(float32_t)255))/100u)    /* 16.0 % */
  #define PWM_IN_NOMINAL_TO_MIN               (u8)(((u16)((float32_t)15.5*(float32_t)255))/100u)    /* 16.0 % */

  #define PWM_IN_NOMINAL_TO_MAX               (u8)(((u16)((float32_t)89.5*(float32_t)255))/100u)    /* 90.0 % */
  #define PWM_IN_MAX_TO_NOMINAL               (u8)(((u16)((float32_t)89.3*(float32_t)255))/100u)      /* 90.0 % */
  
  /* #define PWM_IN_EMERGENCY_TO_MAX             ((u8)(((u16)(95.8*255))/100)) */ /* 95.0 % */
  /* #define PWM_IN_MAX_TO_EMERGENCY             ((u8)(((u16)98*255)/100))     */ /* 98.0 % */

  #define DISCRIMINATION_OVERFLOW_PWM_IN      (u16)0x8000U

#ifdef RVS
  #if defined(PWM_TO_ANALOG_IN)
    #define PWM_LIM_0                           (u8)((u16)98/4)                         /* 6%.  PWM TO ANALOG at 25°C */
    #define PWM_LIM_1                           (u8)((u16)210/4)                        /* 19%. PWM TO ANALOG at 25°C */
    #define PWM_LIM_2                           (u8)((u16)259/4)                        /* 25%. PWM TO ANALOG at 25°C */
    #define PWM_LIM_3                           (u8)((u16)349/4)                        /* 37%. PWM TO ANALOG at 25°C */
    #define PWM_LIM_4                           (u8)((u16)380/4)                        /* 42%. PWM TO ANALOG at 25°C */
    #define PWM_LIM_5                           (u8)((u16)600/4)                        /* 79%. PWM TO ANALOG at 25°C */
  #else
    #define PWM_LIM_0                           (u8)((u16)((u16)10*255)/100)            /* 6%.  6*255/100  */
    #define PWM_LIM_1                           (u8)((u16)((u16)15*255)/100)            /* 25%. 12*255/100 */
    #define PWM_LIM_2                           (u8)((u16)((u16)30*255)/100)            /* 35%. 20*255/100 */
    #define PWM_LIM_3                           (u8)((u16)((u16)35*255)/100)            /* 50%. 22*255/100 */
    #define PWM_LIM_4                           (u8)((u16)((u16)40*255)/100)            /* 55%. 30*255/100 */
    #define PWM_LIM_5                           (u8)((u16)((u16)95*255)/100)            /* 95%. 95*255/100 */
    #define PWM_LIM_6                           (u8)((u16)((u16)98*255)/100)            /* 98%. 95*255/100 */
  #endif /* PWM_TO_ANALOG_IN */

  #define I0_0                                SP_MIN_SPEED_VALUE
  #define	I0_1                                (u8)255           
  #define	I0_2                                (u8)128           
#endif  /* RVS */

#define PWM_IN_FREQ_PRODUCTION_LOW_HZ             (u16)1400                         /* Means 1400 Hz                                   */
#define PWM_IN_FREQ_PRODUCTION_HIGH_HZ            (u16)1600                         /* Means 1600 Hz                                   */
#define PWM_IN_DUTY_TORQUE_LOW_PERCENT            (u8)9                             /* Means  9% of duty. In order to satisfy TREQ043. */
#define PWM_IN_DUTY_TORQUE_HIGH_PERCENT           (u8)11                            /* Means 11% of duty. In order to satisfy TREQ043. */
#define PWM_IN_DUTY_BALAN_HALT_TO_MIN_PERCENT     (u8)20                            /* Means 20% of duty. In order to satisfy TREQ043. */
#define PWM_IN_DUTY_BALAN_MIN_TO_NOMINAL_PERCENT  (u8)25                            /* Means 25% of duty. In order to satisfy TREQ043. */
#define PWM_IN_DUTY_BALAN_NOMINAL_TO_MAX_PERCENT  (u8)75                            /* Means 75% of duty. In order to satisfy TREQ043. */
#define PWM_IN_DUTY_BALAN_MAX_TO_OFF_PERCENT      (u8)80                            /* Means 80% of duty. In order to satisfy TREQ043. */
#define PWM_IN_DUTY_DEBUG_LOW_PERCENT             (u8)89                            /* Means 89% of duty. In order to satisfy TREQ043. */
#define PWM_IN_DUTY_DEBUG_HIGH_PERCENT            (u8)91                            /* Means 91% of duty. In order to satisfy TREQ043. */

#define PWM_IN_FREQ_PRODUCTION_LOW_BIT            (u16)((u32)5000000/PWM_IN_FREQ_PRODUCTION_HIGH_HZ)                     
#define PWM_IN_FREQ_PRODUCTION_HIGH_BIT           (u16)((u32)5000000/PWM_IN_FREQ_PRODUCTION_LOW_HZ)                     
#define PWM_IN_DUTY_TORQUE_LOW_BIT                (u8)((u16)((u16)255*PWM_IN_DUTY_TORQUE_LOW_PERCENT)/100u)
#define PWM_IN_DUTY_TORQUE_HIGH_BIT               (u8)((u16)((u16)255*PWM_IN_DUTY_TORQUE_HIGH_PERCENT)/100u)
#define PWM_IN_DUTY_BALAN_HALT_TO_MIN_BIT         (u8)((u16)((u16)255*PWM_IN_DUTY_BALAN_HALT_TO_MIN_PERCENT)/100u)
#define PWM_IN_DUTY_BALAN_MIN_TO_NOM_BIT          (u8)((u16)((u16)255*PWM_IN_DUTY_BALAN_MIN_TO_NOMINAL_PERCENT)/100u)
#define PWM_IN_DUTY_BALAN_NOM_TO_MAX_BIT          (u8)((u16)((u16)255*PWM_IN_DUTY_BALAN_NOMINAL_TO_MAX_PERCENT)/100u)
#define PWM_IN_DUTY_BALAN_MAX_TO_OFF_BIT          (u8)((u16)((u16)255*PWM_IN_DUTY_BALAN_MAX_TO_OFF_PERCENT)/100u)
#define PWM_IN_DUTY_DEBUG_LOW_BIT                 (u8)((u16)((u16)255*PWM_IN_DUTY_DEBUG_LOW_PERCENT)/100u)
#define PWM_IN_DUTY_DEBUG_HIGH_BIT                (u8)((u16)((u16)255*PWM_IN_DUTY_DEBUG_HIGH_PERCENT)/100u)

#define PWM_IN_DISAPPEAR_TEST_OP_NUM              (u8)((u8)((u16)((u16)125*PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER)/PWM_IN_FREQ_PRODUCTION_LOW_HZ)+(u16)2)  /* Formula is computed i this way: PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER * PWMinputPeriod(testing phase) / 8 mS */
                                                                                                                                                      /* +2 takes into account incorrelation between pwm input and main sw. If pwm input edge, base time isr and disappearing checking happen in this sequence */
                                                                                                                                                      /* first, pwm input edge set u8NumNoDuty to zero then base time isr increment u8NumNoDuty to one and immediatilly after (since base time, pwm input and main flow are incorrelated) disappearing checking found mistake! */
/* ANALOG-input parameters */
#define MAX_ANALOG_AD_INPUT ((u16)1023)

#define ANALOG_RESISTOR_UP_PER_255          (u16)4590   /* Means 18 kOhm */
#define ANALOG_RESISTOR_DOWN_PER_255        (u16)3825   /* Means 15 kOhm */

#define ANALOG_TURN_OFF_VOLTAGE_PER_256     (u16)(384)                                          /* Means 1.50 V. */
#define ANALOG_TURN_ON_VOLTAGE_PER_256      (u16)(499)                                          /* Means 1.95 V. */
#define ANALOG_MAX_SP_VOLTAGE_PER_256       (u16)(2304)                                         /* Means 9    V. */
#define ANALOG_TURN_OFF_BIT		              (u16)((u32)((u32)ANALOG_TURN_OFF_VOLTAGE_PER_256*ANALOG_RESISTOR_DOWN_PER_255*4u)/(u32)(5u*(u32)((u32)ANALOG_RESISTOR_UP_PER_255+ANALOG_RESISTOR_DOWN_PER_255))) /* *4 multiplication takes care 10bit A/D convertion!!!! */
#define ANALOG_TURN_ON_BIT		              (u16)((u32)((u32)ANALOG_TURN_ON_VOLTAGE_PER_256*ANALOG_RESISTOR_DOWN_PER_255*4u)/(u32)(5u*(u32)((u32)ANALOG_RESISTOR_UP_PER_255+ANALOG_RESISTOR_DOWN_PER_255)))  /* *4 multiplication takes care 10bit A/D convertion!!!! */
#define ANALOG_MAX_SP_BIT		                (u16)((u32)((u32)ANALOG_MAX_SP_VOLTAGE_PER_256*ANALOG_RESISTOR_DOWN_PER_255*4u)/(u32)(5u*(u32)((u32)ANALOG_RESISTOR_UP_PER_255+ANALOG_RESISTOR_DOWN_PER_255)))   /* *4 multiplication takes care 10bit A/D convertion!!!! */

#define ANALOG_FDT_SLOPE_SHIFT              7
#define ANALOG_FDT_SLOPE                    (u8)(((u32)(SP_MAX_SPEED_VALUE - SP_MIN_SPEED_VALUE)<<ANALOG_FDT_SLOPE_SHIFT)/((u16)(ANALOG_MAX_SP_BIT - ANALOG_TURN_ON_BIT)))

#define ANALOG_TEST_FREQUENCY_HZ            (u8)10                                              /* Means 10Hz. */
#define ANALOG_PERIOD_TO_WAIT_NUMBER        (u8)8                                               /* Number of analog input period to wait in order to check initial operating mode */
#define ANALOG_INPUT_TEST_TIME_BASE         (u8)((u16)((u16)1000*ANALOG_PERIOD_TO_WAIT_NUMBER)/(u16)((u16)TIMEBASE_MSEC*ANALOG_TEST_FREQUENCY_HZ)) /* Formula is computed i this way: ANALOG_PERIOD_TO_WAIT_NUMBER * pwm input period/8e-3 where 8e-3 is timer base time */
#define ANALOG_VALID_NORMAL_OPERATION_COUNT (u8)(ANALOG_PERIOD_TO_WAIT_NUMBER/(u8)2)            /* Number of consecuitve normal operation InputSP_FindOperatingMode has to found in order to set NORMAL_OPERATION mode. */

#define ANALOG_PWM_LOW_VOLTAGE_PER_256      (u16)(768)                                          /* Means 3V. */
#define ANALOG_PWM_HIGH_VOLTAGE_PER_256     (u16)(1945)                                         /* Means Vbatt-5V = 12,6-5. Taking into account analog measure is compensated at 12,6V */
#define ANALOG_PWM_LOW_BIT		              (u16)((u32)((u32)ANALOG_PWM_LOW_VOLTAGE_PER_256*ANALOG_RESISTOR_DOWN_PER_255*4u)/(u32)(5u*(u32)((u32)ANALOG_RESISTOR_UP_PER_255+ANALOG_RESISTOR_DOWN_PER_255)))  /* *4 multiplication takes care 10bit A/D convertion!!!! */
#define ANALOG_PWM_HIGH_BIT		              (u16)((u32)((u32)ANALOG_PWM_HIGH_VOLTAGE_PER_256*ANALOG_RESISTOR_DOWN_PER_255*4u)/(u32)(5u*(u32)((u32)ANALOG_RESISTOR_UP_PER_255+ANALOG_RESISTOR_DOWN_PER_255))) /* *4 multiplication takes care 10bit A/D convertion!!!! */

#define ANALOG_PWM_IN_HALT_TO_EMERGENCY     (u8)4                                                   /* 14.5*255/100 */
#define ANALOG_PWM_IN_EMERGENCY_TO_HALT     (u8)5                                                   /* 8.5*255/100  */

#define ANALOG_PWM_IN_HALT_TO_MIN           (u8)37                                                  /* 14.5*255/100 */
#define ANALOG_PWM_IN_MIN_TO_HALT           (u8)21                                                  /* 8.5*255/100  */

#define ANALOG_PWM_IN_MIN_TO_NOMINAL        (u8)37                                                  /* 14.5*255/100 */
#define ANALOG_PWM_IN_NOMINAL_TO_MIN        (u8)37                                                  /* 14.5*255/100 */

#define ANALOG_PWM_IN_NOMINAL_TO_MAX        (u8)229                                                 /* 90*255/100   */
#define ANALOG_PWM_IN_MAX_TO_NOMINAL        (u8)229                                                 /* 89*255/100   */

#define ANALOG_PWM_IN_EMERGENCY_TO_MAX      (u8)244                                                 /* 95.6*255/100 */
#define ANALOG_PWM_IN_MAX_TO_EMERGENCY      (u8)247                                                 /* 96.8*255/100 */

#if defined(PWM_TO_ANALOG_IN)

  #define ANALOG_IN_FREQ_PRODUCTION_LOW_HZ            (u16)(PWM_IN_SET_PERIOD_HZ-PWM_IN_PERIOD_TOLERANCE_HZ)    /* Means 10 Hz */
  #define ANALOG_IN_FREQ_PRODUCTION_HIGH_HZ           (u16)(PWM_IN_SET_PERIOD_HZ+PWM_IN_PERIOD_TOLERANCE_HZ)    /* Means 15 Hz */

  #define ANALOG_IN_DUTY_TORQUE_LOW_PERCENT           (u8)8                                     /* Means  8% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_TORQUE_HIGH_PERCENT          (u8)12                                    /* Means 12% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_HALT_TO_MIN_PERCENT    (u8)20                                    /* Means 20% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_MIN_TO_NOMINAL_PERCENT (u8)25                                    /* Means 25% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_NOMINAL_TO_MAX_PERCENT (u8)75                                    /* Means 75% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_MAX_TO_OFF_PERCENT     (u8)80                                    /* Means 80% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_DEBUG_LOW_PERCENT            (u8)88                                    /* Means 90% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_DEBUG_HIGH_PERCENT           (u8)92                                    /* Means 95% of duty. In order to satisfy TREQ043. */

#else
  
  #define ANALOG_IN_FREQ_PRODUCTION_LOW_HZ            (u16)10                                   /* Means 10 Hz */
  #define ANALOG_IN_FREQ_PRODUCTION_HIGH_HZ           (u16)15                                   /* Means 15 Hz */
  
  #define ANALOG_IN_DUTY_TORQUE_LOW_PERCENT           (u8)10                                    /* Means 10% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_TORQUE_HIGH_PERCENT          (u8)15                                    /* Means 15% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_HALT_TO_MIN_PERCENT    (u8)20                                    /* Means 20% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_MIN_TO_NOMINAL_PERCENT (u8)25                                    /* Means 25% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_NOMINAL_TO_MAX_PERCENT (u8)75                                    /* Means 75% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_BALAN_MAX_TO_OFF_PERCENT     (u8)80                                    /* Means 80% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_DEBUG_LOW_PERCENT            (u8)90                                    /* Means 90% of duty. In order to satisfy TREQ043. */
  #define ANALOG_IN_DUTY_DEBUG_HIGH_PERCENT           (u8)95                                    /* Means 95% of duty. In order to satisfy TREQ043. */
 
#endif /*PWM_TO_ANALOG_IN*/

#define ANALOG_IN_FREQ_PRODUCTION_LOW_BIT           (u32)((u32)5000000/ANALOG_IN_FREQ_PRODUCTION_HIGH_HZ)
#define ANALOG_IN_FREQ_PROD_HIGH_BIT                (u32)((u32)5000000/ANALOG_IN_FREQ_PRODUCTION_LOW_HZ)
#define ANALOG_IN_DUTY_TORQUE_LOW_BIT               (u8)((u16)((u16)255*ANALOG_IN_DUTY_TORQUE_LOW_PERCENT)/(u16)100)
#define ANALOG_IN_DUTY_TORQUE_HIGH_BIT              (u8)((u16)((u16)255*ANALOG_IN_DUTY_TORQUE_HIGH_PERCENT)/(u16)100)
#define ANALOG_IN_DUTY_BAL_HALT_TO_MIN_BIT          (u8)((u16)((u16)255*ANALOG_IN_DUTY_BALAN_HALT_TO_MIN_PERCENT)/(u16)100)
#define ANALOG_IN_DUTY_BAL_MIN_TO_NOM_BIT           (u8)((u16)((u16)255*ANALOG_IN_DUTY_BALAN_MIN_TO_NOMINAL_PERCENT)/(u16)100)
#define ANALOG_IN_DUTY_BAL_NOM_TO_MAX_BIT           (u8)((u16)((u16)255*ANALOG_IN_DUTY_BALAN_NOMINAL_TO_MAX_PERCENT)/(u16)100)
#define ANALOG_IN_DUTY_BAL_MAX_TO_OFF_BIT           (u8)((u16)((u16)255*ANALOG_IN_DUTY_BALAN_MAX_TO_OFF_PERCENT)/(u16)100)
#define ANALOG_IN_DUTY_DEBUG_LOW_BIT                (u8)((u16)((u16)255*ANALOG_IN_DUTY_DEBUG_LOW_PERCENT)/(u16)100)
#define ANALOG_IN_DUTY_DEBUG_HIGH_BIT               (u8)((u16)((u16)255*ANALOG_IN_DUTY_DEBUG_HIGH_PERCENT)/(u16)100)

#define ANALOG_IN_DISAPPEAR_TEST_OP_NUM             (u8)((u8)((u16)((u16)125*PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER)/ANALOG_IN_FREQ_PRODUCTION_LOW_HZ)+(u16)2)  /* Formula is computed i this way: PWM_IN_PERIOD_TO_DISAPPEAR_NUMBER * PWMinputPeriod(testing phase) / 8 ms */
                                                                                                                                                           /* +2 takes into account incorrelation between pwm input and main sw. If pwm input edge, base time isr and disappearing checking happen in this sequence, */
                                                                                                                                                           /* first, pwm input edge set u8NumNoDuty to zero then base time isr increment u8NumNoDuty to one and immediatilly after (since base time, pwm input and main flow are incorrelated) disappearing checking found mistake! */
/* NTC-input parameters */
#define NTC_TURN_OFF_VOLTAGE_PER_256        (u16)783                                          /* 19 °C. From pspice simulation 3.06 V */
#define NTC_TURN_ON_VOLTAGE_PER_256         (u16)776                                          /* 20 °C. From pspice simulation 3.03 V */
#define NTC_MAX_SP_VOLTAGE_PER_256          (u16)545                                          /* 50 °C. From pspice simulation 2.13 V */
#define NTC_TURN_OFF_BIT                    (u16)((u32)((u32)NTC_TURN_OFF_VOLTAGE_PER_256*4u)/5u)
#define NTC_TURN_ON_BIT                     (u16)((u32)((u32)NTC_TURN_ON_VOLTAGE_PER_256*4u)/5u)
#define NTC_MAX_SP_BIT                      (u16)((u32)((u32)NTC_MAX_SP_VOLTAGE_PER_256*4u)/5u)

#define NTC_FDT_SLOPE_SHIFT                 (u8)7
#define NTC_FDT_SLOPE                       (u8)(((u32)(SP_MAX_SPEED_VALUE - SP_MIN_SPEED_VALUE)<<ANALOG_FDT_SLOPE_SHIFT)/((u16)(NTC_TURN_ON_BIT - NTC_MAX_SP_BIT)))

/* CheckTestPhase definitions */
/** 
    \def SP_IN_HALT_TO_MIN_TEST_MODE2
    \brief Set point value edge from halt to min speed for balancing test. 
    \details It take count on pwm or analog input.

    \def SP_IN_MIN_TO_NOMINAL_TEST_MODE2
    \brief Set point value edge from minimum to nominal speed for balancing test. 
    \details It take count on pwm or analog input.

    \def SP_IN_NOMINAL_TO_MAX_TEST_MODE2
    \brief Set point value edge from nominal to maximum speed for balancing test. 
    \details It take count on pwm or analog input.

    \def SP_IN_MAX_TO_OFF_TEST_MODE2
    \brief Set point value edge from maximum to stop speed for balancing test. 
    \details It take count on pwm or analog input.

    \def SP_IN_HALT_TO_MIN
    \brief Set point value edge from halt to min speed for normal operation. 
    \details It take count on pwm or analog input.

    \def SP_IN_MIN_TO_HALT
    \brief Set point value edge from min to halt speed for normal operation. 
    \details It take count on pwm or analog input.

    \def SP_IN_MIN_TO_NOMINAL
    \brief Set point value edge from minimum to nominal speed for normal operation. 
    \details It take count on pwm or analog input.

    \def SP_IN_NOMINAL_TO_MIN
    \brief Set point value edge from nominal to minimum speed for normal operation. 
    \details It take count on pwm or analog input.

    \def SP_IN_NOMINAL_TO_MAX
    \brief Set point value edge from nominal to maximum speed for normal operation. 
    \details It take count on pwm or analog input.

    \def SP_IN_MAX_TO_NOMINAL
    \brief Set point value edge from maximum to nominal speed for normal operation. 
    \details It take count on pwm or analog input.

    \def SP_IN_EMERGENCY_TO_MAX
    \brief Set point value edge from emergency to maximum speed for normal operation. 
    \details It take count on pwm or analog input.

    \def SP_IN_MAX_TO_EMERGENCY
    \brief Set point value edge from maximum to emergency speed for normal operation. 
    \details It take count on pwm or analog input.
*/
                                             
#if ( defined(PWM_INPUT) || defined(LIN_INPUT) )

  #define SP_IN_HALT_TO_MIN_TEST_MODE2        PWM_IN_DUTY_BALAN_HALT_TO_MIN_BIT
  #define SP_IN_MIN_TO_NOMINAL_TEST_MODE2     PWM_IN_DUTY_BALAN_MIN_TO_NOM_BIT
  #define SP_IN_NOMINAL_TO_MAX_TEST_MODE2     PWM_IN_DUTY_BALAN_NOM_TO_MAX_BIT
  #define SP_IN_MAX_TO_OFF_TEST_MODE2         PWM_IN_DUTY_BALAN_MAX_TO_OFF_BIT

  #define SP_IN_HALT_TO_EMERGENCY             PWM_IN_HALT_TO_EMERGENCY
  #define SP_IN_EMERGENCY_TO_HALT             PWM_IN_EMERGENCY_TO_HALT
  #define SP_IN_HALT_TO_MIN                   PWM_IN_HALT_TO_MIN
  #define SP_IN_MIN_TO_HALT                   PWM_IN_MIN_TO_HALT
  #define SP_IN_MIN_TO_NOMINAL                PWM_IN_MIN_TO_NOMINAL
  #define SP_IN_NOMINAL_TO_MIN                PWM_IN_NOMINAL_TO_MIN
  #define SP_IN_NOMINAL_TO_MAX                PWM_IN_NOMINAL_TO_MAX
  #define SP_IN_MAX_TO_NOMINAL                PWM_IN_MAX_TO_NOMINAL
  /*#define SP_IN_EMERGENCY_TO_MAX              PWM_IN_EMERGENCY_TO_MAX */
  /*#define SP_IN_MAX_TO_EMERGENCY              PWM_IN_MAX_TO_EMERGENCY */

  #define SP_IN_NOMINAL_L_TO_H                PWM_IN_NOMINAL_L_TO_H
  #define SP_IN_NOMINAL_H_TO_L                PWM_IN_NOMINAL_H_TO_L

  #define SP_IN_FREQ_PRODUCTION_LOW_BIT       PWM_IN_FREQ_PRODUCTION_LOW_BIT
  #define SP_IN_FREQ_PRODUCTION_HIGH_BIT      PWM_IN_FREQ_PRODUCTION_HIGH_BIT
  #define SP_IN_DUTY_TORQUE_LOW_BIT           PWM_IN_DUTY_TORQUE_LOW_BIT
  #define SP_IN_DUTY_TORQUE_HIGH_BIT          PWM_IN_DUTY_TORQUE_HIGH_BIT
  #define SP_IN_DUTY_DEBUG_LOW_BIT            PWM_IN_DUTY_DEBUG_LOW_BIT
  #define SP_IN_DUTY_DEBUG_HIGH_BIT           PWM_IN_DUTY_DEBUG_HIGH_BIT

#else
  #ifdef AN_INPUT

    #define SP_IN_HALT_TO_MIN_TEST_MODE2        ANALOG_IN_DUTY_BAL_HALT_TO_MIN_BIT
    #define SP_IN_MIN_TO_NOMINAL_TEST_MODE2     ANALOG_IN_DUTY_BAL_MIN_TO_NOM_BIT
    #define SP_IN_NOMINAL_TO_MAX_TEST_MODE2     ANALOG_IN_DUTY_BAL_NOM_TO_MAX_BIT
    #define SP_IN_MAX_TO_OFF_TEST_MODE2         ANALOG_IN_DUTY_BAL_MAX_TO_OFF_BIT

    #define SP_IN_HALT_TO_EMERGENCY             ANALOG_PWM_IN_HALT_TO_EMERGENCY
    #define SP_IN_EMERGENCY_TO_HALT             ANALOG_PWM_IN_EMERGENCY_TO_HALT
    #define SP_IN_HALT_TO_MIN                   ANALOG_PWM_IN_HALT_TO_MIN
    #define SP_IN_MIN_TO_HALT                   ANALOG_PWM_IN_MIN_TO_HALT
    #define SP_IN_MIN_TO_NOMINAL                ANALOG_PWM_IN_MIN_TO_NOMINAL
    #define SP_IN_NOMINAL_TO_MIN                ANALOG_PWM_IN_NOMINAL_TO_MIN
    #define SP_IN_NOMINAL_TO_MAX                ANALOG_PWM_IN_NOMINAL_TO_MAX
    #define SP_IN_MAX_TO_NOMINAL                ANALOG_PWM_IN_MAX_TO_NOMINAL
    #define SP_IN_EMERGENCY_TO_MAX              ANALOG_PWM_IN_EMERGENCY_TO_MAX
    #define SP_IN_MAX_TO_EMERGENCY              ANALOG_PWM_IN_MAX_TO_EMERGENCY

    #define SP_IN_FREQ_PRODUCTION_LOW_BIT       ANALOG_IN_FREQ_PRODUCTION_LOW_BIT
    #define SP_IN_FREQ_PRODUCTION_HIGH_BIT      ANALOG_IN_FREQ_PROD_HIGH_BIT
    #define SP_IN_DUTY_TORQUE_LOW_BIT           ANALOG_IN_DUTY_TORQUE_LOW_BIT
    #define SP_IN_DUTY_TORQUE_HIGH_BIT          ANALOG_IN_DUTY_TORQUE_HIGH_BIT
    #define SP_IN_DUTY_DEBUG_LOW_BIT            ANALOG_IN_DUTY_DEBUG_LOW_BIT
    #define SP_IN_DUTY_DEBUG_HIGH_BIT           ANALOG_IN_DUTY_DEBUG_HIGH_BIT

  #endif  /* AN_INPUT */
#endif  /* PWM_INPUT */

#define PWM_FDT_SLOPE_NORMAL_MODE           (u8)(((u16)(SP_MAX_SPEED_VALUE - SP_MIN_SPEED_VALUE)*128)/((u8)(SP_IN_NOMINAL_TO_MAX - SP_IN_NOMINAL_TO_MIN)))
#define PWM_FDT_SLOPE_TEST_MODE             (u8)(((u16)(SP_MAX_SPEED_VALUE - SP_MIN_SPEED_VALUE)*128)/((u8)(SP_IN_NOMINAL_TO_MAX_TEST_MODE2 - SP_IN_MIN_TO_NOMINAL_TEST_MODE2)))

/* Public type definition -------------------------------------------------- */
/** 
    \typedef OperatingMode_t
    \brief Used to define motor behaviour over input request. 
    \details Following table has applied:\n
    1. \ref enumOperatingMode = \ref BALAN_TEST if 1080Hz<Fpwm<1120 58%<duty<62% is recognize.\n
    2. \ref enumOperatingMode = \ref TORQUE_TEST if 1080Hz<Fpwm<1120 28%<duty<32% is recognize.\n
    3. \ref enumOperatingMode = \ref NORMAL_OPERATION otherwise.\n
*/
typedef enum {
  NORMAL_OPERATION,
  TORQUE_TEST,
  BALAN_TEST,
  DEBUG_TEST
} OperatingMode_t;

/** 
    \struct tPWMInValue
    \brief Used to turn out frequency and duty cycle pwm input value. 
    \details It gathers two value telling timer value and overflow amoung
    two consecutive pwm chosen edge.
*/
typedef struct UPWMInValue
  { 
    u16 u16Val;  /* !< Necessary to store the value of the timer module (TIM) register at the capture time */
    u8  u8Ovf;   /* !< Necessary to store the occurred overflow number in Timer 2 register. */
  }tPWMInValue;

/** 
    \typedef StatePwmIn_T
    \brief Used to define motor behaviour over input request. 
    \details Some customer requires different behaviour respect on input value
    These state take care of different input request and so motor state.
*/
typedef enum  {
  HALTED,          /*!< Input PWM in stop range */
  MIN_SPEED,       /*!< Input PWM in minimum speed range */
  NOMINAL_SPEED,   /*!< Input PWM in linear conversion range */
  MAX_SPEED,       /*!< Input PWM in maximum speed range */
  EMERGENCY        /*!< Input PWM in emergency range */
} StatePwmIn_T;

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE
  extern volatile tPWMInValue bufstrPWMInData[DIM_BUFFER_PWM_IN];
  extern u8 u8PWMindex;
  extern u8 u8NumOvfPWM;
  extern BOOL boolOVFVsPWMInput;
#pragma DATA_SEG DEFAULT
  extern volatile u8 u8NumNoDuty;
  #if (!defined(PWM_INPUT) && !defined(PWM_TO_ANALOG_IN))
  extern u8 u8PWMinDuty;
  #endif

/* Public Functions prototypes --------------------------------------------- */
u8   InputSP_Get_u8NumNoDuty(void);
u32  InputSP_Get_u32TempDuty(void);

void InputSP_Set_u8PWMinDuty(u8 u8Value);
u8   InputSP_Get_u8PWMinDuty(void);

void InputSP_InitInput(void);
OperatingMode_t InputSP_GetOperatingMode(void);
void InputSP_SetOperatingMode(OperatingMode_t OperatingMode);
u8 InputSP_GetInputSetPoint(void);
BOOL InputSP_ReadyToSleep(void);
BOOL InputSP_PwmDisappeared(void);
void InputSP_GetPWMDutyAndPeriod(void); 
void InputSP_FindOperatingMode(void);
BOOL InputSP_PerformPlausabilityCheck(void);
extern u8 (*InputSP_StatePwmInFunction[5])(u8 InRef);
#ifdef PWM_TO_ANALOG_IN
u8 InputSP_CheckPeriodCount(void);
void InputSP_CheckDutyAndSetMode(void);
#endif
#if ((defined(PWM_INPUT)) && !(defined(PWM_TO_ANALOG_IN)))
void InputSP_InitPlausabilityCheck(void);
#endif
#ifdef RVS 
  u8 InputSP_GetRevInputFdT(u8); 
#endif  /* RVS */
StatePwmIn_T InputSP_Get_enumPwmInState(void);

#endif  /* INPUT_SP_H */

/*** (c) 2011 SPAL Automotive ****************** END OF FILE **************/
