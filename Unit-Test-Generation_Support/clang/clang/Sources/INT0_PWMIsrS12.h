/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  INT0_PWMIsrS12.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file INT0_PWMIsrS12.h
    \version see \ref FMW_VERSION 
    \brief PWM interrupt routines prototypes.
    \details This routines manages PWM interrupts.\n
    \note none
    \warning none
*/

#ifndef INT0_PWMIsrS1_H
#define INT0_PWMIsrS1_H  


/* Private Constants ------------------------------------------------------- */
#define SIN_3TH_ARM_OFFSET_VALUE_BIT      (u8)64
#define SIN_3TH_ARM_CENTER_VALUE_BIT      (u16)256
#define MODULATION_PRESCALER_SHIFT        8
#define MODULATION_SCALER_SHIFT           1

#define PHASE_OFFSET_120_DEG_16BIT        (u16)21845
#define PHASE_OFFSET_240_DEG_16BIT        (u16)43690


/* Public Constants -------------------------------------------------------- */
/* PWM Frequency */
#define CENTER_ALIGNED                      2u   /* Definitions of PWM patterns type */
#define EDGE_ALIGNED                        1u
#define PATTERN_TYPE                        CENTER_ALIGNED
#define PWM_PRSC                            (u8)0
#define PWM_MODULO                          (u16)512             /* PWM frequency set to 19.607kHz. 20MHz/(2*510) = 19.607kHz */
#define PWM_NEXT_ISR                        ((u16)PWM_MODULO/2u) /* PWM timer works with 20Mhz clock whilst timer modul works with 5Mhz */

#if  (PATTERN_TYPE == CENTER_ALIGNED)
    #define PWM_FREQ                        (u16)((MTC_CLOCK *2u) / (u32)(PATTERN_TYPE * PWM_MODULO *(u32)((u32)2<<PWM_PRSC))) 
    /* Resolution: 1Hz */
#else
    #define PWM_FREQ                        (u16)((MTC_CLOCK *2u) / (u32)(PATTERN_TYPE * PWM_MODULO *(u32)((u32)2<<PWM_PRSC))) 
    /* Resolution: 1Hz */
#endif

#define SINEFREQ_FREQ_RESOL_PU              (u16)((float32_t)((float32_t)((float32_t)U32_MAX/(float32_t)PWM_FREQ)*(float32_t)BASE_FREQUENCY_HZ)/(float32_t)FREQUENCY_RES_FOR_PU_BIT)

/* Alignment u32Phase */
#if ( ( PCB == PCB211A_HY ) || ( PCB == PCB211A ) || ( PCB == PCB221A ) || ( PCB == PCB221B ) || ( PCB == PCB224A) || ( PCB == PCB224B) || ( PCB == PCB228B ) || ( PCB == PCB237E ) || ( PCB == PCB250A ) || ( PCB == PCB262A ) )
  #define LUT_VF_ZC_REF_HEX       (u16)17408     /* LUT zero crossing reference for phase voltage in hex format for assembler pwm isr in CW rotation */
  #define LUT_VF_ZC_REF_HEX_BWD   (u16)39168     /* LUT zero crossing reference for phase voltage in hex format for assembler pwm isr in CCW rotation */
#elif ( ( PCB == PCB211B ) || ( PCB == PCB213A ) || ( PCB == PCB213B )  || ( PCB == PCB213C ) || ( PCB == PCB220A ) || ( PCB == PCB220B ) || ( PCB == PCB220C ) || ( PCB == PCB213D ) || ( PCB == PCB224C ) || ( PCB == PCB224D ) || ( PCB == PCB225A ) || ( PCB == PCB225B ) || ( PCB == PCB225C ) || ( PCB == PCB233C ) || ( PCB == PCB228A ) || ( PCB == PCB230A ) || ( PCB == PCB240A ) || ( PCB == PCB236B ) || ( PCB == PCB258A ) )
  #define LUT_VF_ZC_REF_HEX       (u16)39168     /* LUT zero crossing reference for phase voltage in hex format for assembler pwm isr in CW rotation */
  #define LUT_VF_ZC_REF_HEX_BWD   (u16)17408     /* LUT zero crossing reference for phase voltage in hex format for assembler pwm isr in CCW rotation */
#endif  /*FAN_TYPE*/

/* Battery degradation */
#if ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) )
  #define IBATT_INT_GAIN_SHIFT                    (u16)8
  #define IBATT_INT_GAIN_FACT                     (u16)((u16)1<<IBATT_INT_GAIN_SHIFT)
  
  #define IBATT_REG_CONV_FACT                     (float32_t)((float32_t)((float32_t)0.00488*(float32_t)FREQUENCY_RES_FOR_PU_BIT)/(float32_t)((float32_t)BASE_FREQUENCY_HZ*SHUNT_RESISTOR*OP_AMP_GAIN))
  #define IBATT_REG_COMP_GAIN                     (float32_t)((float32_t)((float32_t)2*(float32_t)V_BUS_REFERENCE_VOLT)/(float32_t)((float32_t)3*VAPP_MAX_VALUE_VOLT*(float32_t)((float32_t)E_REF_1000RPM/(float32_t)((float32_t)((float32_t)6.283*(float32_t)1000)/(float32_t)60))))

  #define IBATT_RISE_TIME_SEC                     (float32_t)0.01                                   /* [s]  => 10 ms (come da specifica BMW). */
  #define IBATT_REG_BANDWIDTH_ON_HZ               (float32_t)((float32_t)0.35/IBATT_RISE_TIME_SEC)  /* [Hz] => 35 % del tempo di salita nella risposta a gradino di un sistema del 1° ordine. */
  
  #define IBATT_MAXIMUM_DELTA                     (float32_t)46     /* [A] => 1000 W / 13 V = 76 A => Ibatt_degradated(9 V) = 30 A => IBATT_MAXIMUM_DELTA = (76 - 30) [A] = 46 [A].                */
  #define IBATT_MAXIMUM_DERIVATIVE                (float32_t)400    /* [A/s]  N.B.: con riferimento alla specifica FAAR WEE di BMW, questo parametro era settato a 200 A/s. Con JLR non si hanno   */
                                                                    /* vincoli in tal senso e IBATT_MAXIMUM_DERIVATIVE e' stato posto a 400 A/s, cosi' da avere una costante RC del                */
                                                                    /* filtro passa-basso sulla Vmon pari a 0.115 s. Tale filtro e' utilizzato per rallentare la compensazione del bus             */
                                                                    /* DC, sia nell'ambito del controllo della derivata di corrente (a salire) per la routine di BATTERY_DEGRADATION, sia          */
                                                                    /* nell'ambito della stabilizzazione del ripple sulla corrente di fase con l'inserimento della CANCELLAZIONE D'IMPULSO.        */

  #define IBATT_REG_OFF_TIME_CONST                (float32_t)((float32_t)IBATT_MAXIMUM_DELTA/IBATT_MAXIMUM_DERIVATIVE)              /* [s] <=> Costante L/R del sistema equivalente. */
  #define IBATT_REG_BANDWIDTH_OFF_HZ              (float32_t)((float32_t)1/(float32_t)((float32_t)6.283*IBATT_REG_OFF_TIME_CONST))  /* fbw = (1 / 2pi(L/R)). */
      
  #define IBATT_PROP_GAIN_ON                      (float32_t)((float32_t)6.283*IBATT_REG_COMP_GAIN*Ls_REF*IBATT_REG_BANDWIDTH_ON_HZ)
  #define IBATT_PROP_GAIN_ON_BIT                  (u8)((float32_t)IBATT_PROP_GAIN_ON*IBATT_REG_CONV_FACT)
  #define IBATT_INT_GAIN_ON                       (float32_t)((float32_t)((float32_t)IBATT_PROP_GAIN_ON*Rs_REF)/Ls_REF)
  #define IBATT_INT_GAIN_ON_BIT                   (u8)((float32_t)((float32_t)IBATT_INT_GAIN_ON*IBATT_REG_CONV_FACT*(float32_t)0.0001)*(float32_t)IBATT_INT_GAIN_FACT)
    
  #define IBATT_REG_OFF_INTEGRATION_TIME          (float32_t)0.001  /* [s] => Per questione di risoluzione sui guadagni (integrale in particolare), l'integrazione in uscita si ha ogni ms. */
  #define IBATT_REG_OFF_INTEGRATION_TIME_BIT      (u8)((float32_t)IBATT_REG_OFF_INTEGRATION_TIME/(float32_t)0.0001)  
  #define IBATT_PROP_GAIN_OFF                     (float32_t)((float32_t)6.283*IBATT_REG_COMP_GAIN*Ls_REF*IBATT_REG_BANDWIDTH_OFF_HZ)
  #define IBATT_PROP_GAIN_OFF_BIT                 (u8)((float32_t)IBATT_PROP_GAIN_OFF*IBATT_REG_CONV_FACT)
  #define IBATT_INT_GAIN_OFF                      (float32_t)((float32_t)((float32_t)IBATT_PROP_GAIN_OFF*Rs_REF)/Ls_REF)
  #define IBATT_INT_GAIN_OFF_BIT                  (u8)((float32_t)((float32_t)IBATT_INT_GAIN_OFF*IBATT_REG_CONV_FACT*IBATT_REG_OFF_INTEGRATION_TIME)*(float32_t)IBATT_INT_GAIN_FACT)  
  
  #define IBATT_VS_VMON_LOW_LIMIT                 (float32_t)8.2    /* [V] */
  #define IBATT_VS_VMON_LOW_LIMIT_BIT             (u16)((float32_t)((float32_t)((float32_t)((float32_t)((float32_t)IBATT_VS_VMON_LOW_LIMIT - V_DROP_VDHS_VOLT - V_GAMMA_VDH_RVP_DIODE)*V_MON_RESISTOR_DOWN)/(float32_t)((float32_t)V_MON_RESISTOR_DOWN+V_MON_RESISTOR_UP))*(float32_t)1000)/(float32_t)4.88)
  #define IBATT_VS_VMON_HIGH_LIMIT                (float32_t)12.2   /* [V] */
  #define IBATT_VS_VMON_HIGH_LIMIT_BIT            (u16)((float32_t)((float32_t)((float32_t)((float32_t)((float32_t)IBATT_VS_VMON_HIGH_LIMIT - V_DROP_VDHS_VOLT - V_GAMMA_VDH_RVP_DIODE)*V_MON_RESISTOR_DOWN)/(float32_t)((float32_t)V_MON_RESISTOR_DOWN+V_MON_RESISTOR_UP))*(float32_t)1000)/(float32_t)4.88)

  #define IBATT_VS_VMON_SETPOINT_LOW              77  /* [A] */
  #define IBATT_VS_VMON_SETPOINT_LOW_BIT          (u16)((float32_t)((float32_t)((float32_t)IBATT_VS_VMON_SETPOINT_LOW*SHUNT_RESISTOR*OP_AMP_GAIN)/(float32_t)0.00488))
  #define IBATT_VS_VMON_SETPOINT_HIGH             82  /* [A] */
  #define IBATT_VS_VMON_SETPOINT_HIGH_BIT         (u16)((float32_t)((float32_t)((float32_t)IBATT_VS_VMON_SETPOINT_HIGH*SHUNT_RESISTOR*OP_AMP_GAIN)/(float32_t)0.00488))    
  #define IBATT_VS_VMON_COEFF_SHIFT               (u8)4
  #define IBATT_VS_VMON_COEFF_SCALER              (u8)((u8)1<<IBATT_VS_VMON_COEFF_SHIFT)
  #define IBATT_VS_VMON_COEFF                     (u16)((float32_t)((float32_t)((float32_t)IBATT_VS_VMON_SETPOINT_HIGH_BIT - (float32_t)IBATT_VS_VMON_SETPOINT_LOW_BIT)*(float32_t)IBATT_VS_VMON_COEFF_SCALER) / (float32_t)((float32_t)IBATT_VS_VMON_HIGH_LIMIT_BIT - (float32_t)IBATT_VS_VMON_LOW_LIMIT_BIT))

  #define IBATT_LEVEL_GAIN_CHANGE                 3   /* [A] <=> errore di regolazione sulla Ibatt entro i 3 A. */
  #define IBATT_LEVEL_GAIN_CHANGE_BIT             (s16)((float32_t)((float32_t)((float32_t)IBATT_LEVEL_GAIN_CHANGE*SHUNT_RESISTOR*OP_AMP_GAIN)/(float32_t)0.00488))
  
  #define VBATT_FILT_SCALER_BIT                   (u8)5
  #define VBATT_FILT_SCALER_FACT                  (u8)((u8)1<<VBATT_FILT_SCALER_BIT)                            /* Max risoluzione con variabili s16!!              */
  #define VBATT_STEP_FREQ_CONST                   (u16)((u32)((u32)1000*FILTER_RES_FACT)/TIMEBASE_MSEC)         /* Base-tempi 8 ms.                                 */
  #define VBATT_FILTER_BANDWIDTH                  (float32_t)IBATT_REG_BANDWIDTH_OFF_HZ                         /* [Hz] (Stessa banda del regolatore PI in uscita). */
  #define VBATT_RC_FREQ_CONST                     (u16)((float32_t)FILTER_RES_FACT*6.283*VBATT_FILTER_BANDWIDTH)
  #define VBATT_FILT_HYST_VOLT                    1                 /* [V] */
  #define VBATT_FILT_HYST_SCALED                  (u16)((float32_t)((float32_t)((float32_t)((float32_t)((float32_t)((float32_t)VBATT_FILT_HYST_VOLT - V_DROP_VDHS_VOLT - V_GAMMA_VDH_RVP_DIODE)*V_MON_RESISTOR_DOWN)/(float32_t)((float32_t)V_MON_RESISTOR_DOWN+V_MON_RESISTOR_UP))*(float32_t)1000)/(float32_t)4.88)*(float32_t)VBATT_FILT_SCALER_FACT)  
  #define VBATT_CHANGE_COMP_LOW_LEVEL             (float32_t)0.5    /* [V] */
  #define VBATT_CHANGE_COMP_LOW_LEVEL_BIT         (s16)((float32_t)((float32_t)((float32_t)((float32_t)((float32_t)VBATT_CHANGE_COMP_LOW_LEVEL - V_DROP_VDHS_VOLT - V_GAMMA_VDH_RVP_DIODE)*V_MON_RESISTOR_DOWN)/(float32_t)((float32_t)V_MON_RESISTOR_DOWN+V_MON_RESISTOR_UP))*(float32_t)1000)/(float32_t)4.88)
  #define VBATT_CHANGE_COMP_HIGH_LEVEL            (float32_t)1      /* [V] */
  #define VBATT_CHANGE_COMP_HIGH_LEVEL_BIT        (s16)((float32_t)((float32_t)((float32_t)((float32_t)((float32_t)VBATT_CHANGE_COMP_HIGH_LEVEL - V_DROP_VDHS_VOLT - V_GAMMA_VDH_RVP_DIODE)*V_MON_RESISTOR_DOWN)/(float32_t)((float32_t)V_MON_RESISTOR_DOWN+V_MON_RESISTOR_UP))*(float32_t)1000)/(float32_t)4.88)
#endif  /* ( defined(BATTERY_DEGRADATION) || defined(VBATT_FILTERING) || defined(PULSE_ERASING) ) */

#ifdef GROUND_BALANCE
  #define GROUND_BALANCE_TIME_SEC                (float32_t)0.05
  #define GROUND_BALANCE_TIME_BIT                (u16)((float32_t)GROUND_BALANCE_TIME_SEC/BASE_INTERRUPT_STEP)
  #define GROUND_BALANCE_DUTY_SHIFT              (u16)7
  #define GROUND_BALANCE_DUTY_SCALER             (u16)((u16)1<<GROUND_BALANCE_DUTY_SHIFT)
  #define GROUND_BALANCE_DUTY_PERCENT            10  /* [%] */
  #define GROUND_BALANCE_DUTY_STEP               (float32_t)((float32_t)((float32_t)GROUND_BALANCE_DUTY_PERCENT*(float32_t)512)/(float32_t)((float32_t)100*(float32_t)GROUND_BALANCE_TIME_BIT))
  #define GROUND_BALANCE_DUTY_STEP_SCALED        (u16)((float32_t)GROUND_BALANCE_DUTY_STEP*(float32_t)GROUND_BALANCE_DUTY_SCALER)
#endif  /* GROUND_BALANCE */

#ifdef PULSE_ERASING
  #define PWM_DUTY_MIN                    0
  #define PWM_DUTY_MAX                    512
  #define PULSE_CANC_DUTY_LOW_uS          5   /* us */
  #define PULSE_CANC_DUTY_LOW_BIT         (u16)((float32_t)((float32_t)PULSE_CANC_DUTY_LOW_uS*(float32_t)PWM_DUTY_MAX)/(float32_t)50)    /* Calcolo del duty su 50 us di interrupt. */
  #define PULSE_CANC_DUTY_HIGH_uS         (float32_t)((float32_t)50-(float32_t)PULSE_CANC_DUTY_LOW_uS)  /* us */
  #define PULSE_CANC_DUTY_HIGH_BIT        (u16)((float32_t)((float32_t)PULSE_CANC_DUTY_HIGH_uS*(float32_t)PWM_DUTY_MAX)/(float32_t)50)   /* Calcolo del duty su 50 us di interrupt. */
  #define VBATT_PULSE_CANC_ENABLE_VOLT    (float32_t)15.5   /* [V] */
  #define VBATT_PULSE_CANC_ENABLE_BIT     (u16)((float32_t)((float32_t)((float32_t)((float32_t)((float32_t)VBATT_PULSE_CANC_ENABLE_VOLT - V_DROP_VDHS_VOLT - V_GAMMA_VDH_RVP_DIODE)*V_MON_RESISTOR_DOWN)/(float32_t)((float32_t)V_MON_RESISTOR_DOWN+V_MON_RESISTOR_UP))*(float32_t)1000)/(float32_t)4.88)

  #define PULSE_CANC_MOD_TEMP_LOW_DEG     (s16)120  /* [°C] */
  #define PULSE_CANC_MOD_TEMP_LOW_PU      (s16)((float32_t)((float32_t)PULSE_CANC_MOD_TEMP_LOW_DEG*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)

  #define PULSE_CANC_MOD_TEMP_HIGH_DEG    (s16)130  /* [°C] */
  #define PULSE_CANC_MOD_TEMP_HIGH_PU     (s16)((float32_t)((float32_t)PULSE_CANC_MOD_TEMP_HIGH_DEG*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)
  
  /* #define PULSE_CANC_MOD_INDEX_HYST_LOW   (u16)((float32_t)512*1.03) */ /* Su u16SineMag (indice di modulazione teorico - non compensato), +3 % di PWM_MODULO. */
  /* #define PULSE_CANC_MOD_INDEX_HYST_HIGH  (u16)((float32_t)512*1.09) */ /* Su u16SineMag (indice di modulazione teorico - non compensato), +9 % di PWM_MODULO. */
#endif  /* PULSE_ERASING */

/* Overmodulation limits */
#ifndef OVER_MODULATION
  #define SINEMAGSCALED_MAX           (u16)512
#else
  #define SINEMAGSCALED_MAX           (u16)700
#endif /* OVER_MODULATION */

/* Public type definition -------------------------------------------------- */

/* Public Variables -------------------------------------------------------- */
#pragma DATA_SEG SHORT _DATA_ZEROPAGE

  #ifdef BATTERY_DEGRADATION
    typedef struct 
    {
      u8 u8KpBatt;                /* Guadagno proporzionale del regolatore sulla Ibatt.   */
      u8 u8KiBatt;                /* Guadagno integrale del regolatore sulla Ibatt.       */
      u16 u16IbattSp;             /* Setpoint del regolatore sulla Ibatt.                 */
      s16 s16IbattProp;           /* Componente proporzionale del regolatore sulla Ibatt. */
      u16 u16IbattInt;            /* Componente integrale del regolatore sulla Ibatt.     */
      u16 u16IbattRegOut;         /* Uscita del regolatore sulla Ibatt.                   */
    } structIbattPIreg;
    
    extern volatile structIbattPIreg IbattReg;
    
    typedef enum
    {
      GAIN_HIGH,                  /* Stato logico <=> banda larga del regolatore PI sulla Ibatt.   */
      GAIN_LOW                    /* Stato logico <=> banda stretta del regolatore PI sulla Ibatt. */
    } enumChangeGain;
    
    typedef struct
    {
      enumChangeGain enumGain;    /* Stato logico (dei guadagni) del regolatore PI sulla Ibatt.                                          */
      u8 u8IbattRegCount;         /* Timer per l'aggiornamento del regolatore PI sulla Ibatt.                                            */
      u8 u8IbattRegCountMax;      /* Step di integrazione del regolatore PI sulla Ibatt.                                                 */
      BOOL BOOLComputeSetpoint;   /* Flag per alternare (calcolo setpoint + macchina a stati) / aggiornamento regolatore PI sulla Ibatt. */
      BOOL BOOLChangeVbattComp;   /* Flag per segnalare il cambio di logica sulla compensazione di Vbatt.                                */
      BOOL BOOLForceGainFlag;     /* Flag per forzare al default la macchina a stati del regolatore PI sulla Ibatt.                      */
    } structIbattLogic;
    
    extern volatile structIbattLogic IbattStateMachine; 
  #endif  /* BATTERY_DEGRADATION */
  
  
#pragma DATA_SEG DEFAULT

/* Public Functions prototypes --------------------------------------------- */
__interrupt void INT0_dpwmmin_modulation(void);

#ifdef LIN_INPUT
  void INT0_EnablePWMISRComputation(void);
  void INT0_DisablePWMISRComputation(void);
#endif  /* LIN_INPUT */

void INT0_PWM_FailSafeMonitor(void);
void INT0_PWM_FailSafeMonitor_rst(void);
void INT0_Set_u16IBattOffset(u16 u16Value);
u16  INT0_Get_u16IBattOffset(void);
void INT0_Set_strTempDrive_s16TempAmbReadPU(s16 value);
s16  INT0_Get_strTempDrive_s16TempAmbReadPU(void);

#ifdef MOSFET_OVERTEMPERATURE_PROTECTION
void INT0_Set_strTempDrive_s16TempMOSReadPU(s16 value);
s16  INT0_Get_strTempDrive_s16TempMOSReadPU(void);
#endif

#endif  /* INT0_PWMIsrS1_H */

/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/
