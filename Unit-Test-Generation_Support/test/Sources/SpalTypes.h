/************************* (c) 2019  SPAL Automotive *************************
PROJECT  :  JLR - MLA

MODULE   :  SpalTypes.h

VERSION  :  1.1

AUTHORS  :  Ianni Fabrizio

******************************************************************************/

#ifndef SPALTYPES_H
#define SPALTYPES_H


/*--------------------------STANDARD TYPE DEFINITION-------------------------*/
/**
    \typedef tPointerFcn;   
      \details Definition of a new type: poitner to function.
*/
typedef void (*tPointerFcn)(void); 

/**
    \typedef u8;   
      \details Unsigned 8 bit type definition done to fast variables identification and declaration.
    \typedef s8;   
      \details Signed 8 bit type definition done to fast variables identification and declaration.
    \typedef u16;   
      \details Unsigned 16 bit type definition done to fast variables identification and declaration.
    \typedef s16;   
      \details Signed 16 bit type definition done to fast variables identification and declaration.
    \typedef u32;   
      \details Unsigned 32 bit type definition done to fast variables identification and declaration.
    \typedef s32;   
      \details Signed 32 bit type definition done to fast variables identification and declaration.
    \typedef BOOL;   
      \details Starting from an \ref u8 type (u8 8 bit) a boolean variable is created and it's possible value are:
      - \ref TRUE
      - \ref FALSE
*/

#ifdef _CANTATA_
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
#else
typedef unsigned char u8;   /* unsigned 8 bit type definition */
typedef signed char s8;		  /* signed 8 bit type definition */
typedef unsigned int u16;	  /* unsigned 16 bit type definition */
typedef signed int s16;		  /* signed 16 bit type definition */
typedef unsigned long u32;	/* unsigned 32 bit type definition */
typedef signed long s32;	  /* signed 32 bit type definition */
#endif
typedef u8 BOOL;            /* Boolean value*/
typedef float float32_t;    /* float32_t value*/

#define U8_MAX    ((u8)255)
#define S8_MAX    ((s8)127)
#define S8_MIN    ((s8)-128)
#define U16_MAX   ((u16)65535)
#define S16_MAX   ((s16)32767)
#define S16_MIN   ((s16)-32768)
#define U24_MAX   ((u32)16777215)
#define U32_MAX   ((u32)4294967295u)
#define S32_MAX   ((s32)2147483647)
#define S32_MIN   ((s32)-S32_MAX-1)

/* STANDARD CONSTANT DEFINITION */
/* #ifdef    FALSE */
/* #undef    FALSE */
/* #endif          */

/* #ifdef    TRUE  */
/* #undef    TRUE  */
/* #endif          */

/* #define   FALSE   ((BOOL)(0x00)) */
/* #define   TRUE    ((BOOL)(0x01)) */

#define   ON         (u8)1
#define   OFF        (u8)0

/*--------------------------STANDARD MACRO DEFINITION------------------------*/
/*PRQA S 3453 ++  #These functions-like macro have been tested and consolidated so it is not necessary to modify them */
#define DIM_ARRAY(x) {(sizeof(x) / sizeof(x[0]));} /* Nbr of elements in array x[] */
#define RADQUAD(x) (u8)((float32_t)((float32_t)(((float32_t)(-0.6)*(x)*(x)) + ((float32_t)140.4*(x)))/(float32_t)1000) + ((float32_t)1.6277) + ((float32_t)0.5))    /* (x^0.5) = (-0,0006x2 + 0,1404x + 1,6277) + 0.5 */
                                                                                                                                                                    /* N.B.: lo 0.5 finale serve per la giusta approssimazione ad intero. */
/*--------------------------------BIT ACCESSES-------------------------------*/
#define SetBit(VAR,Place)         {( (VAR) |= (u16)((u16)1<<(Place)) );}
#define u8ClrBit(VAR,Place)       {((VAR) = (u8)(VAR)&(~((u8)1<<(u8)(Place))) );}
#define u16ClrBit(VAR,Place)      {((VAR) = (u16)(VAR)&(~((u16)1<<(u8)(Place))));}
#define AffBit(VAR,Place,Value)   {((Value) ? \
                                   (VAR |= (1<<Place)) : \
                                   (VAR &= (~(1<<Place))));}
#define MskBit(Dest,Msk,Src)      {( Dest = (Msk & Src) | ((~Msk) & Dest) );}
#define ValBit(VAR,Place)         ((VAR) & (u16)((u16)1<<(Place)))
#define BitTest(Mask, Addr)       ((Addr) & ((u8)Mask))
#define BitSet(Mask, Addr)        {((Addr) |= (u8)Mask);}
#define BitClear(Mask, Addr)      {((Addr) &= ~((u8)Mask));}
/*#define u16BitTest(Mask, Addr)    ((Addr) & ((u16)Mask))*/
#define u16BitSet(Mask, Addr)     {((Addr) |= (u16)Mask);}
#define u16BitClear(Mask, Addr)   {((Addr) &= ~((u16)Mask));}
/*PRQA S 3453 -- */
#define Nop()                     { asm nop; }

#endif  /* SPALTYPES_H */

/*** (c) 2016 SPAL Automotive ****************** END OF FILE **************/
