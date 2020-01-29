/**************** (c) SPAL Automotive *********************************

PROJECT  : 
COMPILER : METROWERKS

MODULE  :  RedirTable.h

VERSION: 1.3
DATA:	19/02/2016 
AUTHOR:	Tondelli

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
DESCRIPTION :   Redirection Table for configuration features             

-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-
MODIFICATIONS :

******************************************************************************/

#ifndef REDIR_TABLE_H
#define REDIR_TABLE_H  

//*************************************Custom header inclusions*************************************************************
#include "SPAL_Setup.h"
#include MICRO_REGISTERS
#include SPAL_DEF
//**************************************************************************************************************************

//*************************************Typedef*********************************************************************************
typedef struct{
  const volatile u8 *ptrData;                      //oppure meglio byte* ??????
  //u8 NumOfBytes;  
  const volatile u8 DataType;
  //DataType_t DataType; 
  const volatile u8 ArrayLength;
}RedirElemt_t;

typedef struct{
  u8   TypeOfInterface;
  u8   FwVersion[4];
  u8   FanModel;
  u8   Application;
  u8   PCBVersion;
  u8   BatteryVoltage;
  unsigned int    ShuntResistorX1000;
	unsigned int    PeakDetectorGainX10;
	u8   DataMatrix[12];
	u8   Date[3];
	u8   HwVersion[3];
	u8   Interfaceversion;

}EEProductDataStruct_t;

//*********************************************************************************************************************************

//*************************************Custom settings**********************************************************************

#define PTR_REDIR_TABLE_START_ADDR   (unsigned int)(0x0480) //locazione che contiene il ptr alla zona di memoria da cui parte la redir table
#define REDIR_TABLE_START_ADDR       (unsigned int)(0x0484) //indirizzo di memoria da cui parte la tabella di reindirizzamento (in realta'
                                            //prima c'e' la dimensione di questa contenuta in 2 bytes)
#define RED_TABLE_LENGTH             (unsigned int)14   //elementi

#define PTR_EEDATA_STRUCT_START_ADDR      (unsigned int)(PTR_REDIR_TABLE_START_ADDR + 2u)
#define EEDATA_STRUCT_START_ADDR          (unsigned int)(REDIR_TABLE_START_ADDR + sizeof(RedirTableDim) + (RED_TABLE_LENGTH * sizeof(RedirElemt_t)))
#define EEDATA_STRUCT_SIZE                (unsigned int)(sizeof(EEProductDataStruct_t))  

//****************************************************************************************************************************

//************************************************Private Macros********************************************************************
#define RED_TABLE_SIZE     (unsigned int)(RED_TABLE_LENGTH * sizeof(RedirElemt_t))

#define InsertNullElement        {(const volatile u8 *) 0,NULL_VAL,0}
#define InsertElement(data)      {(const volatile u8 *)&data,ELEMENT|sizeof(data),1}
#define InsertArray(data)        {(const volatile u8 *)&data[0],ARRAY|sizeof(data[0]),(sizeof(data) / sizeof(data[0])) }

//adibisco i primi 5 bit alla lunghezza del dato da leggere, e rimanenti 3 al tipo di dato (elemento o vettore) 
//quindi la massima dimensione del dato da leggere e' 2^5 = 32 bytes
#define SHIFT_DATA_TYPE      (u16)5
#define ELEMENT      (((u16)0)<<SHIFT_DATA_TYPE)
#define ARRAY        (((u16)1)<<SHIFT_DATA_TYPE)
#define MATRIX       (((u16)2)<<SHIFT_DATA_TYPE)
#define NULL_VAL     ((u8)(((u16)7)<<SHIFT_DATA_TYPE))

//*********************************************************************************************************************************

//************************************************Init value**********************************************************************
#define DEFAULT_DATAMATRIX       {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}
#define DEFAULT_DATE             {0x00,0x00,0x00}
#define DEFAULT_HW_VERSION       {0x00,0x00,0x00} 
#define DEFAULT_INTERFACE        0x00

//*********************************************************************************************************************************












#endif