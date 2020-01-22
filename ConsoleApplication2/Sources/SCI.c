/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SCI.c

VERSION  :  1.2

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file SCI.c
    \version see \ref FMW_VERSION 
    \brief Basic communication functions.
    \details Routines for basic comunication features; here are stored all the devices driver, not the protocol.\n
    \note none
    \warning none
*/
#ifdef _CANTATA_
#include "cantata.h"
#endif
#include <string.h>
#include "SPAL_Setup.h"
#include "SpalTypes.h"
#include "SpalBaseSystem.h"
#include MAC0_Register
#include MICRO_REGISTERS
#include SPAL_DEF
#include "SCI.h"
#include "main.h"           /* Per strDebug */


/* ---------------------------- Private Constants --------------------------- */

/* ------------------------------ Private Const ----------------------------- */

/* ---------------------------- Public Variables ---------------------------- */

/* ---------------------------- Private Variables --------------------------- */
static volatile u8 SCI_Error;
#ifdef ENABLE_RS232_DEBUG
static volatile u8 Rx_Index;
#endif
static volatile u8 SCI_Tx_Buff;
static volatile u8 SCI_Tx_String;
static volatile u16 SCI_Rx_Buff;
static volatile u8 SCI_Rx_String;
static volatile u8 SCI_Pol_Stat;  

#ifdef SCI_ITDRV_WITHOUTBUF_TX
static const volatile u8 *PtrToSCIBuffTx; 
#endif
#ifdef SCI_ITDRV_WITHOUTBUF_RX
static volatile u8 SCI_MY_DATA;
static volatile u8 *PtrToSCIBuffRx;
static u16 BufEnd;  
#endif 

static void SCI_Mode(SCI_Mode_Type SCI_Mode_Param);
static void SCI_Select_Baudrate(SCI_Baudrate_Type Baudrate_Presc);
static void SCI_FillTxData(u8 TxData);

#ifdef ENABLE_RS232_DEBUG
/*-----------------------------------------------------------------------------
ROUTINE NAME : strcatu16

INPUT/OUTPUT : None/None

DESCRIPTION  : Setup for SCI

COMMENTS     :
-----------------------------------------------------------------------------*/
void strcatu16(const u8 *PtrtoString, u16 x )
{
  char b[6], *s = b + 5;

  *s-- = '\0';
  do
  {
          *s-- = (char)(x % 10 + '0');
          x /= 10;
  } while( x );
  
  (void) strcat((char*)PtrtoString, s+1);
}
#endif


/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_Init

INPUT/OUTPUT : SCI_Type_Param1/SCI_Type_Param2

DESCRIPTION  : Setup for SCI

COMMENTS     :
-----------------------------------------------------------------------------*/
void SCI_Init(void)
{
  /* SCIC1: LOOPS=0,SCISWAI=0,RSRC=0,M=0,WAKE=0,ILT=0,PE=0,PT=0 */
  Serial_CR1 = (u8)SCI_DEFAULT_PARAM1;                      /*Sets SCI in the required mode*/
  /* SCIC2: TIE=0,TCIE=0,RIE=0,ILIE=0,TE=0,RE=0,RWU=0,SBK=0 */

  Serial_CR2 = (u8)SCI_DEFAULT_PARAM2;

  /* SCI0SR2: TXDIR=1 */
  //SCI0SR2 |= (SCI0SR2_TXDIR_MASK | SCI0SR2_TXPOL_MASK);   /* Use this if TX is plugged at pwm pin.                 */
  SCI0SR2 |= SCI0SR2_TXPOL_MASK;                            /* Use this if TX is plugged directly on micro pin TXD0. */

  SCI_Pol_Stat  = 0;               /*Initializes globalvariables*/
  SCI_Error     = 0;
  SCI_Tx_Buff   = 0;
  SCI_Tx_String = 0;
  SCI_Rx_Buff   = 0;
  SCI_Rx_String = 0;

  SCI_Select_Baudrate(SCI_BD_19200);
  SCI_Mode(SCI_TX_ENABLE);
  SCI_Mode(SCI_RX_ENABLE);
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_Mode

INPUT/OUTPUT : SCI_TX_ENABLE or SCI_RX_ENABLE/None

DESCRIPTION  : Selects the transmitter or receiver mode for SCI

COMMENTS     : None
-----------------------------------------------------------------------------*/
static void SCI_Mode(SCI_Mode_Type SCI_Mode_Param)
{
    Serial_CR2 |= (u8)SCI_Mode_Param;          /* Selects Transmitter or Receiver mode */
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_Select_Baudrate

INPUT/OUTPUT : SCI_Baudrate_Type/None

DESCRIPTION     : Selects possible baudrate for transmission and reception

COMMENTS        : Values are to be precalculated by the user
-----------------------------------------------------------------------------*/
static void SCI_Select_Baudrate(SCI_Baudrate_Type Baudrate_Presc)
{
  SCIxBD = (u16)Baudrate_Presc;                /* Loads the lowest value in SCI1BDL */
}

/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_PutByte

INPUT/OUTPUT : Tx_DATA/None

DESCRIPTION     : Transmits a single data byte

COMMENTS        : None
-----------------------------------------------------------------------------*/
void SCI_PutByte (u8 Tx_DATA)
{
  u8 temp;

  #ifdef SCI_POLLING_TX
    temp = Serial_SR1;                         /* Loads the byte to be transmitted */
    SCI_FillTxData(Tx_DATA);
  #else
    #ifdef SCI_ITDRV_WITHOUTBUF_TX	
      SCI_Tx_Buff = 1;
      SCI_Tx_String = 0;
      temp = Serial_SR1;                       /* Loads the byte to be transmitted */
      SCI_FillTxData(Tx_DATA);
      Serial_fTIE = 1;                         /* Enable INterrupt */
    #endif  /* SCI_ITDRV_WITHOUTBUF_TX */
  #endif  /* SCI_POLLING_TX */
}


/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_IsTransmitCompleted

INPUT/OUTPUT : None/BOOL

DESCRIPTION  : Checks if the transmission is completed or not

COMMENTS     : Must be called after Put_Byte or Put_Buffer or Put_String
-----------------------------------------------------------------------------*/
BOOL SCI_IsTransmitCompleted(void)
{
  BOOL ret;

  #if defined SCI_POLLING_TX
    if( Serial_fTC != 0u )                          /* Checks if transmission is completed */
    {
        ret = (BOOL)TRUE;
    }
    else 
    {
        ret = (BOOL)FALSE;
    }
  #else
    #if defined SCI_ITDRV_WITHOUTBUF_TX 
      if (SCI_Tx_String == 1)
      {
        if( (*PtrToSCIBuffTx == '\0') && (Serial_fTDRE) ) 
        {
          return (TRUE);
        }
        else
        {
          return (FALSE);
        }
      }
      else if (SCI_Tx_Buff == 0)
      {
        return (TRUE);
      }
      else
      {
        return (FALSE);
      }
    #else
      return(TRUE);                          /* Dummy return statement */
    #endif  /* SCI_ITDRV_WITHOUTBUF_TX */
  #endif  /* SCI_POLLING_TX */
  
  return(ret);
}

#ifdef ENABLE_RS232_DEBUG
/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_PutString

INPUT/OUTPUT : *PtrtoString/None

DESCRIPTION  : Transmits the user string in polling mode and starts transmission in interrupt driven mode

COMMENTS     : None
-----------------------------------------------------------------------------*/
void SCI_PutString(const u8 *PtrtoString)
{   
    u8 Val = *PtrtoString, temp;

#ifdef SCI_POLLING_TX 
    while (Val != '\0')                      /* Checks for the end of string */
    {
        Val= *PtrtoString;
        temp = Serial_SR1;        
        SCI_FillTxData(*PtrtoString);        /* Copies the character to be transmitted */
        while(!(SCI_IsTransmitCompleted()));
                                             /* Waits for the completion of the transmission */
        PtrtoString++;                       /* Moves the pointer to the next location */
    }
  #else
    #if defined SCI_ITDRV_WITHOUTBUF_TX 
        PtrToSCIBuffTx = PtrtoString;        /* Copies the adress of the user string into */
                                             /* the global variable                       */
        SCI_Tx_String = 1;                   /* Copies default value to recognize string  */ 
                                             /* transmission in the interrupt subroutine  */
        SCI_Tx_Buff = 0;
        temp = Serial_SR1;
        SCI_FillTxData(*PtrToSCIBuffTx);     /* Transmits first character to generate interrupt */
        Serial_fTIE = 1;                     /* Enable Interrupt */

        Serial_fTDRE = 1;                    /* Put TDRE to zero */

    #endif  /* SCI_ITDRV_WITHOUTBUF_TX */
  #endif  /* SCI_POLLING_TX */
} 
#endif  /* ENABLE_RS232_DEBUG */




#ifdef SCI_POLLING_RX

#ifdef ENABLE_RS232_DEBUG
/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_GetString

INPUT/OUTPUT : *PtrtoString/Error Status

DESCRIPTION     : Receives a string,Stores it in the user buffer and returns
                  the error status to the user
COMMENTS        : Reception stops as soon as an error occurs
-----------------------------------------------------------------------------*/
SCI_RxError_t SCI_GetString(u8 *PtrtoString)
{
    u8 Temp = 0x00, Val = 0xff;

    for (;((Val != '\0')&&(Temp == 0));PtrtoString++)
    {
      Temp |= (u8)(Serial_SR1 & SCI_ErrValue);
      while (!Serial_fRDRF);                                /* waits for the received byte                              */
      Temp |= (u8)(Serial_SR1 & SCI_ErrValue);              /* Temp stores any error that occurred during the reception */
      *PtrtoString = Serial_TxData;                         /* Copies the received character into the user string       */
      Val = *PtrtoString;
    }
    return(Temp);                                           /* Returns the error status to the user */
}
#endif /* ENABLE_RS232_DEBUG */
#endif /* SCI_POLLING_RX */

#ifdef SCI_ITDRV_WITHOUTBUF_RX

/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_GetString

INPUT/OUTPUT : *PtrtoString/None

DESCRIPTION     : Starts the reception of a string in the interrupt subroutine

COMMENTS        : Any data received before calling this function is ignored
-----------------------------------------------------------------------------*/
void SCI_GetString(u8 *PtrtoString, u8 BufLength) 
{
    BufEnd = (u16)PtrtoString + BufLength - 1;
    PtrToSCIBuffRx = PtrtoString;               /*Copies the user buffer to global buffer*/
    Rx_Index = 0;
    SCI_Rx_String = 1;                          /*For String recognition in ISR*/ 
    SCI_Error = 0x00;
    Serial_fRDRF = 1;                           /* Clear Flag */
    Serial_fRIE = 1;                            /* Enable Interrupt */
}
#endif


#ifdef ENABLE_RS232_DEBUG
/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_IsReceptionCompleted

INPUT/OUTPUT : None/Error Status

DESCRIPTION     : Returns the error status occurred during the reception in the
                  interrupt subroutine if the reception is completed.
COMMENTS        : Must be called after SCI_GetBuffer and SCI_GetString in 
                  Interrupt Driven mode to get the reception status.
                  Must be called before SCI_GetByte in interrupt Driven mode.
-----------------------------------------------------------------------------*/
SCI_RxError_t SCI_IsReceptionCompleted (void)  
{   
#if defined SCI_POLLING_RX
    if( Serial_fRDRF ) 
    {
        return ((u8)(Serial_SR1 & SCI_ErrValue)); 
    }
    else
    {
        return (SCI_RX_DATA_EMPTY);    /*Returns the error status to the user*/
    }
#else
#if defined SCI_ITDRV_WITHOUTBUF_RX
    SCI_Pol_Stat = 0;

/*  if ((SCI_Rx_String == 1) && (*(PtrToSCIBuffRx) != '\0') && (SCI_Error == 0)) */
    if ((SCI_Rx_String == 1) && (SCI_Error == 0))
                                        /* String reception request undergoing */
    {
        return(SCI_STRING_ONGOING);
    }  
    else if ((SCI_Error == 0) && (SCI_Rx_Buff > 1)) 
                                        /* Buffer Reception request undergoing */
    {
        return(SCI_BUFFER_ONGOING) ;
    }
/*  else if ((SCI_Rx_Buff == 0) && (SCI_Rx_String == 0) && (SCI_Pol_Stat == 0)) */ /*Single Byte not received*/
/*  {                                                                           */
/*    return (SCI_RX_DATA_EMPTY);                                               */
/*  }                                                                           */
    else                                             /*Reception is completed*/
    {
        SCI_Pol_Stat = 0; 
        SCI_Rx_Buff = 0;
        SCI_Rx_String = 0;
        return (SCI_Error);
    }
#else 
        return (SCI_RX_DATA_EMPTY);                  /*Dummy return statement*/
#endif
#endif
}
#endif

/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_FillTxData

INPUT/OUTPUT : Data/void

DESCRIPTION  : SCI fill the trasmit buffer

COMMENTS     :
-----------------------------------------------------------------------------*/

static void SCI_FillTxData(u8 TxData) {
    Serial_TxData = TxData;
    Serial_fTDRE = 1; //HCS12: reset TDRE to start a new char transmission
}

#ifdef ENABLE_RS232_DEBUG
/*-----------------------------------------------------------------------------
ROUTINE NAME : SCI_FillTxData

INPUT/OUTPUT : void/Rx_Buf Index to read next command

DESCRIPTION  : SCI fill the trasmit buffer

COMMENTS     :
-----------------------------------------------------------------------------*/
u8 Get_First_RxBuf(void) {
    if (Rx_Index > 1) {
        return (Rx_Index-1);
    }
    else return 0;
}
#endif



#if defined (ENABLE_RS232_DEBUG)
__interrupt void Vsci0ISR(void) {
  #ifdef SCI_ITDRV_WITHOUTBUF_TX 
    Reception_ISR();
    Transmission_ISR();
  #endif
}


/*-----------------------------------------------------------------------------
ROUTINE NAME : Transmission_ISR

INPUT/OUTPUT : void/void

DESCRIPTION  : called by HCS08 transmission interrupt

COMMENTS     :
-----------------------------------------------------------------------------*/
void Transmission_ISR(void) 
{
/******************************************************************************
Transmission in Interrupt Driven without Buffer mode
******************************************************************************/

#ifdef SCI_ITDRV_WITHOUTBUF_TX 
    if((Serial_fTC) && (Serial_fTIE))/*Checks if transmit complete flag is set*/
    {
        if ((SCI_Tx_String == 1) && (*PtrToSCIBuffTx != '\0')) 
                                   /*Checks if complete string is transmitted*/
        {
            PtrToSCIBuffTx++;
            SCI_FillTxData(*PtrToSCIBuffTx);
            if (*PtrToSCIBuffTx == '\0')
            {
              Serial_fTIE = 0;  /*Disable IT if complete string is transmitted*/
            }
        }
        else if (SCI_Tx_Buff > 1)   /*Checks if all the bytes are transmitted*/
        {
            SCI_Tx_Buff--; 
            PtrToSCIBuffTx++;            /*Moves the pointer to next location*/
            SCI_FillTxData(*PtrToSCIBuffTx);                /*Transmits a single byte*/
        }
        else if(SCI_Tx_Buff == 1)
        {
            SCI_Tx_Buff--;
            Serial_fTIE = 0;/*Disable IT if complete string is transmitted*/
        }
    }
    else if ((Serial_fTDRE) && (Serial_fTIE))    /*Checks if TDRE Flag is set*/
    {
        if ((SCI_Tx_String == 1)&&(*PtrToSCIBuffTx != '\0')) /*Checks if complete 
                                                        string is transmitted*/
        {
           PtrToSCIBuffTx++;
           SCI_FillTxData(*PtrToSCIBuffTx);
           if (*PtrToSCIBuffTx == '\0')
           {
              Nop();
              Serial_fTIE = 0; /*Disable IT if complete string is transmitted*/
              Nop();
           }

        }
        else if (SCI_Tx_Buff > 1)   /*Checks if all the bytes are transmitted*/
        {
            SCI_Tx_Buff--; 
            PtrToSCIBuffTx++;            /*Moves the pointer to next location*/
            SCI_FillTxData(*PtrToSCIBuffTx);                /*Transmits a single byte*/
        }
        else if(SCI_Tx_Buff == 1)
        {
          SCI_Tx_Buff--;
          Serial_fTIE = 0; /*Disable IT if complete string is transmitted*/
        }
    }
#endif
}
/*-----------------------------------------------------------------------------
ROUTINE NAME : Reception_ISR

INPUT/OUTPUT : void/void

DESCRIPTION  : called by HCS08 reception interrupt

COMMENTS     :
-----------------------------------------------------------------------------*/
void Reception_ISR(void) 
{
/******************************************************************************
Reception in Interrupt Driven Without Buffer mode
******************************************************************************/

#ifdef SCI_ITDRV_WITHOUTBUF_RX 
    u8 temp;
    if(Serial_fOR)          /*If Interrupt is generated due to overrun error*/
    {
        SCI_Error = (u8)(Serial_SR1 & SCI_ErrValue);
        temp = Serial_TxData;
        SCI_Pol_Stat = 2;
    }
    else if (Serial_fRDRF)
    {
/*      if((SCI_Rx_String == 1) && (*PtrToSCIBuffRx != '\0') && (SCI_Error == 0)) */
        if((SCI_Rx_String == 1) && (SCI_Error == 0))     /*If String Reception*/
        {
            SCI_Error = (u8)(Serial_SR1 & SCI_ErrValue);
            *PtrToSCIBuffRx = Serial_TxData;

            if ( (*PtrToSCIBuffRx == '#') || ((u16)PtrToSCIBuffRx >= BufEnd) )
            {
                Serial_fRIE = 0;                            /* Disable interrupt */
                SCI_Rx_String = 0;                          /* String received   */
            }
            else
            {
                if (*PtrToSCIBuffRx != '#') Rx_Index++;     /* Punta al carattere di comando                                                */
                                                            /* es: P# puntera' a P, se invece arriva a BufEnd puntera' all'ultimo carattere */
                                                            /* che non e' piu' '#'                                                          */
                PtrToSCIBuffRx++;
            }
            SCI_Pol_Stat = 2;
        }
        else if ((SCI_Rx_Buff > 1) && (SCI_Error == 0))
        {
            SCI_Error = (u8)(Serial_SR1 & SCI_ErrValue) ;  
            *PtrToSCIBuffRx = Serial_TxData;
            SCI_Rx_Buff--;  
            SCI_Pol_Stat = 2;                       /*If a buffer is received*/
            PtrToSCIBuffRx++;  
            if(SCI_Rx_Buff == 1)
            {
                Serial_fRDRF = 0;       /* Disable interrupt */
                temp = Serial_SR1;
                temp = Serial_TxData;
            }
        }
      else if ((SCI_Rx_Buff == 0) && (SCI_Rx_String == 0))  
      /*If interrupt is generated before calling SCI_GetBuffer/SCI_GetString*/
      {  
        SCI_Error = (u8)(Serial_SR1 & SCI_ErrValue);  
        SCI_MY_DATA = Serial_TxData;
        SCI_Pol_Stat = 1;
      }
      else
      {   
        SCI_Pol_Stat = 1;
        temp = Serial_TxData;
      }
    }
#endif 
}
#endif


/*** (c) 2010 SPAL Automotive ****************** END OF FILE **************/

