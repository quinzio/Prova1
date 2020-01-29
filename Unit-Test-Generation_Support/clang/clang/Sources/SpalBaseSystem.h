/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SpalBaseSystem.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifndef SPALBASESYSTEM_H
#define SPALBASESYSTEM_H


/* Sistema base */
 /* Grandezze primarie */
  /* Tensione */
  #if ( BATTERY_VOLTAGE == V_BATT_12V )
    #define V_BUS_REFERENCE_VOLT                  13  /* [V] */
  #else
    #define V_BUS_REFERENCE_VOLT                  26  /* [V] */
  #endif  /* BATTERY_VOLTAGE */
  #define V_BUS_REFERENCE_BIT                     (u16)((float32_t)((float32_t)((float32_t)V_BUS_REFERENCE_VOLT-V_GAMMA_VDH_RVP_DIODE-V_DROP_VDHS_VOLT)*V_MON_RESISTOR_DOWN*(float32_t)1000)/(float32_t)((float32_t)4.88*(V_MON_RESISTOR_UP+V_MON_RESISTOR_DOWN)))

  #define BASE_VOLTAGE_VOLT                       (float32_t)((float32_t)((float32_t)BASE_VCC_VOLT/(float32_t)V_MON_GAIN) + V_DROP_VDHS_VOLT + V_GAMMA_VDH_RVP_DIODE)  /* 36.483 V */
  #define BASE_VOLTAGE_CONV_FACT                  (float32_t)((float32_t)BASE_VCC_VOLT / (float32_t)((float32_t)BASE_VCC_VOLT + (float32_t)((float32_t)V_MON_GAIN*(float32_t)((float32_t)V_DROP_VDHS_VOLT + V_GAMMA_VDH_RVP_DIODE))))
  #define BASE_VOLTAGE_CONV_FACT_RES_INT          (u16)((float32_t)((float32_t)BASE_VOLTAGE_CONV_FACT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/(float32_t)AD_MAXIMUM_RES_BIT)
  #define BASE_VOLTAGE_CONV_FACT_RES_FLOAT        (float32_t)((float32_t)((float32_t)BASE_VOLTAGE_CONV_FACT*(float32_t)VOLTAGE_RES_FOR_PU_BIT)/(float32_t)AD_MAXIMUM_RES_BIT)
  #define BASE_VOLTAGE_CONV_FACT_RES              (((float32_t)((float32_t)BASE_VOLTAGE_CONV_FACT_RES_FLOAT - (float32_t)BASE_VOLTAGE_CONV_FACT_RES_INT) >= (float32_t)0.5 ) ? (u16)(BASE_VOLTAGE_CONV_FACT_RES_INT+(u16)1) : (u16)BASE_VOLTAGE_CONV_FACT_RES_INT )

  /* Corrente */
  #define BASE_CURRENT_AMP                        (float32_t)((float32_t)((float32_t)BASE_VCC_VOLT - (OP_AMP_GAIN*BASE_OP_AMP_OFFSET))/(float32_t)(OP_AMP_GAIN*SHUNT_RESISTOR))    /* 266.218 A */
  #define BASE_CURRENT_CONV_FACT                  (float32_t)((float32_t)BASE_VCC_VOLT / (float32_t)((float32_t)BASE_CURRENT_AMP* OP_AMP_GAIN * SHUNT_RESISTOR))
  #define BASE_CURRENT_CONV_FACT_RES_INT          (u16)((float32_t)((float32_t)BASE_CURRENT_CONV_FACT*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)AD_MAXIMUM_RES_BIT)  
  #define BASE_CURRENT_CONV_FACT_RES_FLOAT        (float32_t)((float32_t)((float32_t)BASE_CURRENT_CONV_FACT*(float32_t)CURRENT_RES_FOR_PU_BIT)/(float32_t)AD_MAXIMUM_RES_BIT)
  #define BASE_CURRENT_CONV_FACT_RES              (((float32_t)((float32_t)BASE_CURRENT_CONV_FACT_RES_FLOAT - (float32_t)BASE_CURRENT_CONV_FACT_RES_INT) >= (float32_t)0.5 ) ? (u16)(BASE_CURRENT_CONV_FACT_RES_INT+(u16)1) : (u16)BASE_CURRENT_CONV_FACT_RES_INT )

  /* Velocita' */
  #define BASE_SPEED_RPM                  RPM_MAX_SPEED  /* [rpm] */
  #define BASE_FREQUENCY_HZ               (u16)((u32)((u32)BASE_SPEED_RPM*POLE_PAIR_NUM)/60u)
  #define BASE_ELECTRICAL_PULSE           (float32_t)((float32_t)BASE_FREQUENCY_HZ*(float32_t)6.283)
  #define BASE_MECHANICAL_PULSE           (float32_t)((float32_t)BASE_ELECTRICAL_PULSE/(float32_t)POLE_PAIR_NUM)	

  /* Temperatura */
  #define BASE_TEMPERATURE_DEG            (s16)180   /* [°C] */
  #define TLE_BASE_TEMPERATURE_DEG        BASE_TEMPERATURE_DEG  /* (s16)1024 */  /* [°C] */
  #define MOS_BASE_TEMPERATURE_DEG        BASE_TEMPERATURE_DEG  /* (s16)1236 */  /* 1024  1.207 [°C] */

 /* Grandezze derivate */
  /* Angolo elettrico (Delta) */
  #define BASE_ANGLE_DEG                  (u16)360    /* [°el] */
  #define BASE_ANGLE_RAD                  (float32_t)(((float32_t)BASE_ANGLE_DEG * (float32_t)3.141) / (float32_t)180)  /* 2pi [rad_el] */
  #define BASE_OFFSET_ANGLE_DEG           84.375      /* [°el] */
  #define BASE_OFFSET_ANGLE_PU            (float32_t)((float32_t)BASE_OFFSET_ANGLE_DEG / (float32_t)BASE_ANGLE_DEG)
  #define BASE_OFFSET_ANGLE_RES           (u32)(BASE_OFFSET_ANGLE_DEG * ANGLE_RES_FOR_PU_BIT)
  #define BASE_OFFSET_ANGLE_PU_RES        (u16)((float32_t)BASE_OFFSET_ANGLE_PU * (float32_t)ANGLE_RES_FOR_PU_BIT)
  
  /* Potenza assorbita */
  #define BASE_BATT_POWER_WATT            (float32_t)(BASE_VOLTAGE_VOLT*BASE_CURRENT_AMP)  
  
  /* Fattori di risoluzione */
  #define VOLTAGE_RES_FOR_PU_SHIFT        (u32)15
  #define VOLTAGE_RES_FOR_PU_BIT          (u16)((u32)((u32)1<<VOLTAGE_RES_FOR_PU_SHIFT) - (u32)1)
  #define CURRENT_RES_FOR_PU_SHIFT        (u32)15
  #define CURRENT_RES_FOR_PU_BIT          (u16)((u32)((u32)1<<CURRENT_RES_FOR_PU_SHIFT) - (u32)1)
  #define FREQUENCY_RES_FOR_PU_SHIFT      (u32)15
  #define FREQUENCY_RES_FOR_PU_BIT        (u16)((u32)((u32)1<<FREQUENCY_RES_FOR_PU_SHIFT) - (u32)1)
  #define TEMPERATURE_RES_FOR_PU_SHIFT    (u32)15
  #define TEMPERATURE_RES_FOR_PU_BIT      (u16)((u32)((u32)1<<TEMPERATURE_RES_FOR_PU_SHIFT) - (u32)1)
  #define ANGLE_RES_FOR_PU_SHIFT          (u32)15
  #define ANGLE_RES_FOR_PU_BIT            (u16)((u32)((u32)1<<ANGLE_RES_FOR_PU_SHIFT) - (u32)1)


/* Parametri motore + carico in PU */
  /* Rs in PU */
  #define SERIES_RESISTANCE_RES_FOR_PU_SHIFT     (u16)((u16)VOLTAGE_RES_FOR_PU_SHIFT-CURRENT_RES_FOR_PU_SHIFT)
  #define SERIES_RESISTANCE_RES_FOR_PU_BIT       (u16)((u16)VOLTAGE_RES_FOR_PU_BIT/CURRENT_RES_FOR_PU_BIT)    
  #define BASE_SERIES_RESISTANCE                 (float32_t)((float32_t)BASE_VOLTAGE_VOLT /(float32_t) BASE_CURRENT_AMP)  /* 0.137 Ohm */
  #define SERIES_RESISTANCE_PU                   (float32_t)((float32_t)((float32_t)((float32_t)Rs_REF+(float32_t)R_MOS_REF)*(float32_t)SERIES_RESISTANCE_RES_FOR_PU_BIT)/(float32_t)BASE_SERIES_RESISTANCE) /* 0.058 */

  /* Ls in PU */
  #define SERIES_INDUCTANCE_RES_FOR_PU_SHIFT     (u16)((u16)ANGLE_RES_FOR_PU_SHIFT+VOLTAGE_RES_FOR_PU_SHIFT-FREQUENCY_RES_FOR_PU_SHIFT-CURRENT_RES_FOR_PU_SHIFT)
  #define SERIES_INDUCTANCE_RES_FOR_PU_BIT       (u16)((u32)((u32)ANGLE_RES_FOR_PU_BIT*VOLTAGE_RES_FOR_PU_BIT)/(u32)((u32)FREQUENCY_RES_FOR_PU_BIT*CURRENT_RES_FOR_PU_BIT))  
  #define BASE_SERIES_INDUCTANCE                 (float32_t)((float32_t)((float32_t)BASE_ANGLE_RAD * (float32_t)BASE_VOLTAGE_VOLT) / (float32_t)((float32_t)BASE_ELECTRICAL_PULSE * (float32_t)BASE_CURRENT_AMP)) /* 403e-6 H */
  #define SERIES_INDUCTANCE_PU                   (float32_t)((float32_t)((float32_t)Ls_REF*(float32_t)SERIES_INDUCTANCE_RES_FOR_PU_BIT)/(float32_t)BASE_SERIES_INDUCTANCE) /* 0.088 */

  /* Ke in PU */
  #define RL_TIME_CONSTANT                       (float32_t)((float32_t)Ls_REF / (float32_t)(Rs_REF + R_MOS_REF))
  #define Ke_BASE_CONSTANT                       (float32_t)((float32_t)BASE_VOLTAGE_VOLT/BASE_MECHANICAL_PULSE)
  #define Ke_CONSTANT_PU                         (float32_t)((float32_t)((float32_t)E_REF_1000RPM/(float32_t)((float32_t)((float32_t)6.283*(float32_t)1000)/(float32_t)60))/Ke_BASE_CONSTANT)
  #define BASE_INERTIA_CONSTANT                  (float32_t)((float32_t)((float32_t)1.5*Ke_BASE_CONSTANT*BASE_CURRENT_AMP*RL_TIME_CONSTANT)/(float32_t)BASE_MECHANICAL_PULSE)
  #define BEMF_TEMP_COMP_OFFSET                  (float32_t)1.04875  /* Adimensionale (quindi gia' in PU) */
  #define BEMF_TEMP_COMP_COEFF                   (float32_t)0.00195  /* [1/°C] */
  #define BEMF_TEMP_COMP_COEFF_INV               (float32_t)((float32_t)1/BEMF_TEMP_COMP_COEFF) /* [°C] */
  #define BEMF_TEMP_COMP_COEFF_INV_PU            (float32_t)((float32_t)((float32_t)BEMF_TEMP_COMP_COEFF_INV*(float32_t)TEMPERATURE_RES_FOR_PU_BIT)/(float32_t)TLE_BASE_TEMPERATURE_DEG)
  #define BEMF_TEMP_CORR_OFFSET_PU               (s32)(BEMF_TEMP_COMP_OFFSET*BEMF_TEMP_COMP_COEFF_INV_PU)
  #define BEMF_TEMP_CORR_DIVIDER_PU              (s32)BEMF_TEMP_COMP_COEFF_INV_PU

  /* J in PU */
  #define INERTIA_CONSTANT_PU                    (float32_t)((float32_t)FAN_ROT_INERTIA/BASE_INERTIA_CONSTANT)

#endif  /* SPALBASESYSTEM_H */

/*** (c) 2016 SPAL Automotive ****************** END OF FILE **************/
