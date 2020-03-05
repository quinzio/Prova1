/**************** (c) SPAL Automotive *********************************

PROJECT  : 
COMPILER : METROWERKS

MODULE  :  RedirTable.c

VERSION: 1.3b
DATA:	19/02/2016 
AUTHOR:	Tondelli

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
DESCRIPTION :   Redirection Table for configuration features             

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
MODIFICATIONS : 1.3b aggiunta SW_SUBVERSION

******************************************************************************/



#include "RedirTable.h"



#ifdef REDIR_TABLE
  
//#pragma CONST_SEG DFLASH


static const volatile unsigned int PtrRedirTableDim @PTR_REDIR_TABLE_START_ADDR = REDIR_TABLE_START_ADDR;
static const volatile unsigned int RedirTableDim @REDIR_TABLE_START_ADDR  = RED_TABLE_SIZE;

static const volatile unsigned int PtrEEProductDataStruct @PTR_EEDATA_STRUCT_START_ADDR = EEDATA_STRUCT_START_ADDR;
static const volatile unsigned int EEProductDataStructDim @EEDATA_STRUCT_START_ADDR = EEDATA_STRUCT_SIZE; 


static const volatile EEProductDataStruct_t EEProductDataStruct @(EEDATA_STRUCT_START_ADDR + sizeof(EEProductDataStructDim)) = {
INTERFACE_TYPE,
{CUSTOMER,PROJECT,SW_VERSION,SW_SUBVERSION},
FAN_MODEL,
APPLICATION,
PCB,
BATTERY_VOLTAGE,
SHUNT_RESISTOR_PER_10000,
PEAK_DETECTOR_GAIN_PER_10,
DEFAULT_DATAMATRIX,
DEFAULT_DATE,
DEFAULT_HW_VERSION,
DEFAULT_INTERFACE,
};


//const volatile RedirElemt_t RedirectingTable[2] @0x04d0U;

static const volatile RedirElemt_t RedirectingTable[RED_TABLE_LENGTH] @(REDIR_TABLE_START_ADDR + sizeof(RedirTableDim)) = {

InsertElement(EEProductDataStruct.TypeOfInterface),          //0
InsertArray(EEProductDataStruct.FwVersion),                  //1
InsertElement(EEProductDataStruct.FanModel),                 //2
InsertElement(EEProductDataStruct.Application),              //3
InsertElement(EEProductDataStruct.PCBVersion),               //4
InsertElement(EEProductDataStruct.BatteryVoltage),           //5
InsertElement(EEProductDataStruct.ShuntResistorX1000),       //6
InsertElement(EEProductDataStruct.PeakDetectorGainX10),      //7
InsertArray(EEProductDataStruct.DataMatrix),                 //8
InsertArray(EEProductDataStruct.Date),                       //9
InsertArray(EEProductDataStruct.HwVersion),                  //10
InsertElement(EEProductDataStruct.Interfaceversion),         //11
InsertNullElement,                                           //12
InsertNullElement                                            //13
};

#endif
