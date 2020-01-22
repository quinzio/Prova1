/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  PBATT1_Power.c

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file PBATT1_Power.c
    \version see \ref FMW_VERSION 
    \brief Basic functions for Pbatt management.
    \details Routines for Pbatt operation and variables related initialization.\n
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
#include "VBA1_Vbatt.h"
#include "CUR1_Current.h"
#include "main.h"           /* Per strDebug */
#include "PBATT1_Power.h"

/*PRQA S 0380,0857 -- */

/*PRQA S 0292,3108 EOF #Necessary for Doxygen syntax*/

/* Private Variables -------------------------------------------------------- */
static structPower structPowerMeas;

/* Private functions -------------------------------------------------------- */
static void Power_Set_structPowerMeas_u32PbattPU(u32 value);

/* Public functions -------------------------------------------------------- */

/*-----------------------------------------------------------------------------*/
/**
    \fn void Power_Set_structPowerMeas_u16VbattPU(u16 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static variable structPowerMeas.u16VbattPU
                            \par Used Variables
    \ref structPowerMeas.u16VbattPU \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void Power_Set_structPowerMeas_u16VbattPU(u16 value) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{
   structPowerMeas.u16VbattPU = value;
}


/*-----------------------------------------------------------------------------*/
/**
    \fn u16 Power_Get_structPowerMeas_u16VbattPU(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static variable structPowerMeas.u16VbattPU
                            \par Used Variables
    \ref structPowerMeas.u16VbattPU \n
    \return \b The value of static variable structPowerMeas.u16VbattPU.
    \note None.
    \warning None.
*/
u16 Power_Get_structPowerMeas_u16VbattPU(void)
{
   return(structPowerMeas.u16VbattPU);
}


/*-----------------------------------------------------------------------------*/
/**
    \fn void Power_Set_structPowerMeas_u16IbattPU(u16 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static variable structPowerMeas.u16IbattPU
                            \par Used Variables
    \ref structPowerMeas.u16IbattPU \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
void Power_Set_structPowerMeas_u16IbattPU(u16 value)
{
   structPowerMeas.u16IbattPU = value;
}


/*-----------------------------------------------------------------------------*/
/**
    \fn u16 Power_Get_structPowerMeas_u16IbattPU(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static variable structPowerMeas.u16IbattPU
                            \par Used Variables
    \ref structPowerMeas.u16IbattPU \n
    \return \b The value of static variable structPowerMeas.u16IbattPU.
    \note None.
    \warning None.
*/
u16 Power_Get_structPowerMeas_u16IbattPU(void)
{
   return(structPowerMeas.u16IbattPU);
}


/*-----------------------------------------------------------------------------*/
/**
    \fn static void Power_Set_structPowerMeas_u32PbattPU(u16 value)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Sets the value of static variable structPowerMeas.u32PbattPU
                            \par Used Variables
    \ref structPowerMeas.u32PbattPU \n
    \return \b void no value return.
    \note None.
    \warning None.
*/
static void Power_Set_structPowerMeas_u32PbattPU(u32 value)
{
   structPowerMeas.u32PbattPU = value;
}


/*-----------------------------------------------------------------------------*/
/**
    \fn u16 Power_Get_structPowerMeas_u32PbattPU(void)
    \author Ruosi Gabriele \n <em>Reviewer</em>

    \date 11/04/2019
                            \par Starting SW requirement
    <a href="file:///@REQUIREMENT_PATH/@REQUIREMENT_FILE"> Software Requirement Specification </a>
    \brief 	Gets the value of static variable structPowerMeas.u32PbattPU
                            \par Used Variables
    \ref structPowerMeas.u32PbattPU \n
    \return \b The value of static variable structPowerMeas.u32PbattPU.
    \note None.
    \warning None.
*/
u32 Power_Get_structPowerMeas_u32PbattPU(void)
{
   return(structPowerMeas.u32PbattPU);
}


void Power_InitPowerStruct (void)
{
  (void)Vbatt_GetBusVoltage();  /* Per inizializzare structPowerMeas.u16VbattPU */
  Power_Set_structPowerMeas_u16IbattPU(0);
	Power_Set_structPowerMeas_u32PbattPU(0);
}

void Power_UpdatePowerStruct (void) /*PRQA S 1532 #rcma-1.5.0 warning message: this function is correctly defined here */
{  
  u16 structPowerMeas_u16IbattPU_tmp;
  u16 structPowerMeas_u16VbattPU_tmp;

	(void)Vbatt_GetBusVoltage();
	(void)Curr_GetBattCurrent();
	
  structPowerMeas_u16IbattPU_tmp = Power_Get_structPowerMeas_u16IbattPU();
  structPowerMeas_u16VbattPU_tmp = Power_Get_structPowerMeas_u16VbattPU();

  Power_Set_structPowerMeas_u32PbattPU((u32)((u32)structPowerMeas_u16VbattPU_tmp*structPowerMeas_u16IbattPU_tmp));
}


/*** (c) 2015 SPAL Automotive ****************** END OF FILE **************/









