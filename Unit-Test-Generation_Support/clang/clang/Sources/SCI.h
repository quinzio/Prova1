/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SCI.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

/** 
    \file SCI.h
    \version see \ref FMW_VERSION 
    \brief Basic communication functions prototypes.
    \details Routines for basic comunication features; here are stored all the devices driver, not the protocol.\n
    \note none
    \warning none
*/


#ifndef __SCI
#define __SCI

/* --------------------------- Public Constants ---------------------------- */

#define SCI_ErrValue 0x0f                         /*Error bits in SR register*/

typedef enum{										/*Parameters for Control Register1*/
	SCI_DEFAULT_PARAM1  = (u8) 0x00,     
	SCI_PARITY_ON       = (u8) 0x01,
	SCI_ILT_TYPE        = (u8) 0x04,
	SCI_WAKEUP_ADDR     = (u8) 0x08,
	SCI_WORDLENGTH_9    = (u8) 0x10,
	SCI_RSRC            = (u8) 0x20,
	SCI_SWAI            = (u8) 0x40,
	SCI_LOOPS           = (u8) 0x80
	}SCI_Type_Param1;

typedef enum {                             /*Parameters for Control Register2*/
	SCI_MUTE_ENABLE     = (u8) 0x02, 
	SCI_BREAK_ENABLE    = (u8) 0x01,
	SCI_DEFAULT_PARAM2  = (u8) 0x00
	}SCI_Type_Param2;

typedef enum {
	SCI_BD_38400	= (u16) 0x0020,    /*parameters for baudrate register*/
	SCI_BD_19200 =  (u16) 0x0041,
	SCI_BD_9600  =  (u16) 0x0082
	}SCI_Baudrate_Type;

typedef enum {                           /*Parameters for enabling interrupts*/
        SCI_IDLE_LINE               = (u8) 0x10,
        SCI_RECEIVE_OVERRUN	        = (u8) 0x20,
        SCI_TRANSMIT_REGISTER_READY = (u8) 0x80,
        SCI_FRAME_TRANSMITTED       = (u8) 0x40,
        SCI_PARITY_ERROR            = (u8) 0x01
	}SCI_IT_Type;

typedef enum {                                 /*Parameters for enabling mode*/
	SCI_TX_ENABLE		= (u8) 0x08,
	SCI_RX_ENABLE		= (u8) 0x04
	     }SCI_Mode_Type;

typedef enum {                                                  /*Error Types*/
	SCI_BUFFER_ONGOING  = (u8) 0xff,
	SCI_STRING_ONGOING  = (u8) 0xfe,
	SCI_NOISE_ERR		    = (u8) 0x04,
	SCI_OVERRUN_ERR 		= (u8) 0x08,
	SCI_FRAMING_ERR     = (u8) 0x02,
	SCI_PARITY_ERR      = (u8) 0x01,
	SCI_RECEIVE_OK      = (u8) 0x00,
	SCI_RX_DATA_EMPTY   = (u8) 0xfd
	    }SCI_RxError_t;


/* ------------------------ Public type definition ------------------------- */


/* --------------------------- Public Variables ---------------------------- */


/* ---------------------- Public Functions prototypes ---------------------- */

#ifdef ENABLE_RS232_DEBUG
void strcatu16(const u8 *PtrtoString, u16 x);
#endif
void SCI_Init(void);
void SCI_PutByte(u8 Tx_DATA);
BOOL SCI_IsTransmitCompleted(void);
#ifdef ENABLE_RS232_DEBUG
void SCI_PutString(const u8 *PtrtoString);
#endif
#ifdef ENABLE_RS232_DEBUG
void Transmission_ISR(void);
void Reception_ISR(void);
u8  Get_First_RxBuf(void);
#endif


#ifdef SCI_POLLING_RX
#ifdef ENABLE_RS232_DEBUG
SCI_RxError_t SCI_GetString(u8 *PtrtoString);
#endif
#else 
#ifdef SCI_ITDRV_WITHOUTBUF_RX
void SCI_GetString(u8 *, u8); 
#endif
#endif

#ifdef ENABLE_RS232_DEBUG
SCI_RxError_t SCI_IsReceptionCompleted(void);
#endif

#if defined (ENABLE_RS232_DEBUG)
__interrupt void Vsci0ISR(void);
#endif

#endif 

