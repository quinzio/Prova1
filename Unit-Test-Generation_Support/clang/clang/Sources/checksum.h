/******************************************************************************
  FILE        : checksum.h 
  PURPOSE     : checksum calculation interface
  LANGUAGE    : ANSI-C
  -----------------------------------------------------------------------------
  HISTORY 
    19 sep 2002       Created.
 ******************************************************************************/

#ifndef _CHECKSUM_H_
#define _CHECKSUM_H_

#ifdef __HIWARE__
/* HIWARE target compiler */
#include <hidef.h>  /* for FALSE/TRUE */
#include <libdefs.h>

#if defined(_CHECKSUM_USE_FAR_PTR) && !defined(_CHECKSUM_QUALI)
#define _CHECKSUM_QUALI __far
#elif !defined(_CHECKSUM_QUALI)
#define _CHECKSUM_QUALI LIBDEF_VOID_DPTRQ
#endif

#else /* __HIWARE__ */

/* any host compiler (only used to generate the tables in checksum.c) */
#define LIBDEF_VOID_DPTRQ /* nothing */
typedef int Bool;
#define TRUE 1
#define FALSE 0
#endif /* __HIWARE__ */

typedef const LIBDEF_MemByte *_CHECKSUM_QUALI     _CHECKSUM_ConstMemBytePtr; /* pointer to a constant byte in memory */




/* notes: the 2 or 4 byte xor or add checksums do have problems with not actually allocated memory at the area boundaries */
/* for example, for "const char something@0x7777=0x12;" these algorithms are using a undefined byte at 0x7776. */
/* therefore it is not recommended to use them unless you do always allocate things in a multiple of this size */

#ifdef _CHECKSUM_CHECKSUMS_  /* internal test switch only. Generates redundant checks. */
#define _CHECKSUM_CRC_CCITT 1
#define _CHECKSUM_CRC_8     1
#define _CHECKSUM_CRC_16    1
#define _CHECKSUM_CRC_32    1
#define _CHECKSUM_ADD_BYTE  1
#define _CHECKSUM_ADD_WORD  0 /* careful: this one only works when all the addresses/sizes are a multiple of 2 */
#define _CHECKSUM_ADD_LONG  0 /* careful: this one only works when all the addresses/sizes are a multiple of 4 */
#define _CHECKSUM_XOR_BYTE  1
#define _CHECKSUM_XOR_WORD  0 /* careful: this one only works when all the addresses/sizes are a multiple of 2 */
#define _CHECKSUM_XOR_LONG  0 /* careful: this one only works when all the addresses/sizes are a multiple of 4 */
#else

#ifndef _CHECKSUM_CRC_CCITT  
#define _CHECKSUM_CRC_CCITT  0
#endif

#ifndef _CHECKSUM_CRC_8
#define _CHECKSUM_CRC_8   1	 /* used by default*/
#endif

#ifndef _CHECKSUM_CRC_16
#define _CHECKSUM_CRC_16  0
#endif

#ifndef _CHECKSUM_CRC_32
#define _CHECKSUM_CRC_32  0
#endif

#ifndef _CHECKSUM_ADD_BYTE
#define _CHECKSUM_ADD_BYTE 0
#endif

#ifndef _CHECKSUM_ADD_WORD  /* careful: this one only works when all the addresses/sizes are  a multiple of 2 */
#define _CHECKSUM_ADD_WORD 0
#endif

#ifndef _CHECKSUM_ADD_LONG  /* careful: this one only works when all the addresses/sizes are  a multiple of 4 */
#define _CHECKSUM_ADD_LONG 0
#endif

#ifndef _CHECKSUM_XOR_BYTE
#define _CHECKSUM_XOR_BYTE 0
#endif

#ifndef _CHECKSUM_XOR_WORD   /* careful: this one only works when all the addresses/sizes are  a multiple of 2 */
#define _CHECKSUM_XOR_WORD 0
#endif

#ifndef _CHECKSUM_XOR_LONG    /* careful: this one only works when all the addresses/sizes are  a multiple of 4 */
#define _CHECKSUM_XOR_LONG 0
#endif

#endif

#ifndef __FEED_COP /* define this macro accordingly, if you are using a COP (computer operates properly) and the checksum computation takes too long. */
#define __FEED_COP _FEED_COP();
#endif 

/* special types for checksum calculation with known sizes */
typedef unsigned char  _CheckSum1ByteType;
typedef unsigned short _CheckSum2ByteType;
typedef unsigned long  _CheckSum4ByteType;

#define DEFAULT_CRC8_CHECKSUM  ((_CheckSum1ByteType)(/*(1 << 8) +*/ (1 << 7) + (1 << 4) + (1 << 3) + (1 << 1) + (1 << 0)))
#define DEFAULT_CRC16_CHECKSUM ((_CheckSum2ByteType)(/*(1 << 16) +*/ (1 << 15) + (1 << 2) + (1 << 0)))  /* G(x)=X**16+X**15+X**2+1 */
#define DEFAULT_CRC32_CHECKSUM ((_CheckSum4ByteType)0x04C11DB7)  /* G(x)=x**26+x**23+x**22+x**16+x**12+x**11+x**10+x**8+x**7+x**5+x**4+x**2+x**1+1=0 */

/* calculates one byte step of the CRC CCITT computation */
/*_CheckSum2ByteType CheckSumByteCRC_CCITT(_CheckSum2ByteType crc, _CheckSum1ByteType val);*/


// Add this macro to the startup data structure to enable automatic checksums
#define _CHECKSUM_STARTUP_ENTRY     \
        struct __Checksum* _CHECKSUM_QUALI checkSum; \
        int nofCheckSums;            \

// call the checksum function like this (or just use the macro):
#define __CHECKSUM_IS_OK  (_Checksum_Check(_startupData.checkSum, _startupData.nofCheckSums))

/* check all the checksums */
/* returns TRUE (1) when the checksums are OK, FALSE otherwise */
/*int _Checksum_Check(const struct __Checksum* _CHECKSUM_QUALI check, int num);*/ /*NOT USED FUNCTION*/

//To calculate single checksum for multiple memory areas

struct __ChecksumArea {
  _CHECKSUM_ConstMemBytePtr start;
  unsigned int len;
};

/* _Checksum_CheckAreaCRC_CCITT: CRC CCITT checksum of multiple mem areas */
_CheckSum2ByteType _Checksum_CheckAreasCRC_CCITT(const struct __ChecksumArea * areas,int num,_CheckSum2ByteType init);


#endif /* _CHECKSUM_H_ */
