/*****************************************************
      start12.h - datastructures for startup
 ----------------------------------------------------
   Copyright (c) Metrowerks, Basel, Switzerland
               All rights reserved
                  Do not modify!
 *****************************************************/

#ifndef START12_H
#define START12_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(__DPAGE__) || defined(__EPAGE__) || defined(__PPAGE__) || defined(__LARGE__) || defined(__FAR_DATA)
#define FAR_DATA

typedef struct _Range {
  unsigned char * __far beg; int size;      /* [beg..beg+size] */
} _Range;

typedef struct _Copy {
    int size; unsigned char * __far dest;
} _Copy;

#else

typedef struct _Range {
  unsigned char * beg; int size;      /* [beg..beg+size] */
} _Range;

typedef struct _Copy {
    int size; unsigned char * dest;
} _Copy;

#endif

typedef void (*_PFunc)(void);

typedef struct _LibInit {
    _PFunc  *startup;      /* address of startup desc */
} _LibInit;

typedef struct _Cpp {
    _PFunc  initFunc;      /* address of init function */
} _Cpp;

#pragma DATA_SEG __NEAR_SEG STARTUP_DATA /* _startupData can be accessed using 16 bit accesses. This is needed because it contains the stack top, and without stack, far data cannot be accessed */

#ifdef __ELF_OBJECT_FILE_FORMAT__

/* ELF/DWARF object file format */

/* attention: the linker scans the debug information for these structures */
/* to obtain the available fields and their sizes. */
/* So do not change the names in this file. */
#if defined(__LARGE__)
#define PTR16 __near
#else
#define PTR16 /* default */
#endif

extern struct _tagStartup {
#ifndef __NO_FLAGS_OFFSET
     unsigned char   flags;
#endif
#ifndef __NO_MAIN_OFFSET
     _PFunc          main;            /* top level procedure of user program */
#endif
#ifndef __NO_STACKOFFSET_OFFSET
     Word            stackOffset;     /* 16bit, initial value of the stack pointer */
#endif
     unsigned int    nofZeroOuts;     /* number of zero out ranges */
     _Range *PTR16   pZeroOut;        /* vector of ranges with nofZeroOuts elements */
#if defined(__BANKED_COPY_DOWN)
     _Copy *__pptr    toCopyDownBeg;   /* rom-address where copydown-data begins */
#else
     _Copy *PTR16    toCopyDownBeg;   /* rom-address where copydown-data begins */
#endif
#if 0 /* switch on to implement ROM libraries */
     unsigned int    nofLibInits;     /* number of library startup descriptors */
     _LibInit *PTR16 libInits;        /* vector of pointers to library startup descriptors */
#endif
#ifdef __cplusplus
     unsigned int    nofInitBodies;   /* number of init functions for C++ constructors */
     _Cpp *PTR16     initBodies;      /* vector of function pointers to init functions for C++ constructors */
     unsigned int    nofFiniBodies;   /* number of fini functions for C++ destructors */
     _Cpp *PTR16     finiBodies;      /* vector of function pointers to fini functions for C++ destructors */
#endif
} _startupData;


#else

/* HIWARE object file format */

extern struct _tagStartup {
   unsigned      flags;
  _PFunc         main;            /* top procedure of user program */
   unsigned      dataPage;        /* page where data allocation begins */
   long          stackOffset;
   int           nofZeroOuts;
  _Range         *pZeroOut;       /* pZeroOut is a vector of ranges with nofZeroOuts elements */
   long          toCopyDownBeg;   /* rom-address where copydown-data begins */
  _PFunc         *mInits;         /* mInits is a vector of function pointers, terminated by 0 */
  struct sTag    **libInits;      /* libInits is a vector of pointers to startup descriptors, terminated by 0x0000FFFF */
} _startupData;

#endif

#pragma DATA_SEG DEFAULT


#include "push.sgm"
#include "non_bank.sgm"                 /* the _Startup function must be in non banked memory */
#ifdef __cplusplus
extern "C"
#endif

/* execution begins in _Startup */
#if defined(__SET_RESET_VECTOR__)
void __near __interrupt 0 _Startup(void);
#else
void _Startup(void);
#endif


/*--------------------------------------------------------------------*/
#ifdef __cplusplus
 }
#endif

#include "default.sgm"
#include "pop.sgm"

#endif
