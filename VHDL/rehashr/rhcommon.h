/*
 Copyright (c) 2003, Dominik Reichl <dominik.reichl@t-online.de>
 All rights reserved.

 LICENSE TERMS

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.
 * Neither the name of ReichlSoft nor the names of its contributors may be used
   to endorse or promote products derived from this software without specific
   prior written permission.

 DISCLAIMER

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE
 FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef ___REHASH_COMMON_H___
#define ___REHASH_COMMON_H___

// Here you can manually set the flag for compiling a debug version
#undef RH_DEBUG_VERSION

#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <iostream>
#include <memory.h>
#include <string.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

#ifndef __cplusplus
	#error You need a CPlusPlus compiler to compile the ReHash project.
#endif

#define RH_PRODUCT_NAME "ReHash"
#define RH_PRODUCT_FULL "ReHash - Console-Based Hash Calculator"
#define RH_VERSION_STR  "0.2"

// Must be at least 1 greater than the number of implemented hashes
#define RH_MAX_ALGORITHMS 128

#define RH_SUCCESS         0
#define RH_NULLPOINTER     1
#define RH_NO_ARGS         2
#define RH_NO_PATH         3
#define RH_FILE_NOT_FOUND  4
#define RH_BUFFER_OVERFLOW 5
#define RH_FIND_DENIED     6
#define RH_INTERNAL_ERR    7

#define RH_MAX_PATH 1024

// RH_MAX_STD_BUFFER should be at least as big as RH_MAX_PATH
#define RH_MAX_STD_BUFFER 1024

///////////////////////////////////////////////////////////////////////////////
// The following endian detection test is based on the endian detection
// tests in Dr Brian Gladmans source codes.
// For his original header see the header in the sha1.cpp/sha1.h or
// sha2.cpp/sha2.h source files of the ReHash source code package
// (the SHA implementations are based on his source codes)

#undef RH_LITTLE_ENDIAN
#undef RH_BIG_ENDIAN

// Platform-specific includes
#if defined(__FreeBSD__) || defined(__OpenBSD__)
#include <sys/endian.h>
#elif defined(BSD) && (BSD >= 199103)
#include <machine/endian.h>
#elif defined(__GNUC__) || defined(__GNU_LIBRARY__)
#include <endian.h>
#include <byteswap.h>
#elif defined(linux)
#include <endian.h>
#endif

#if defined(__alpha__) || defined(__alpha) || defined(i386)       || \
    defined(__i386__)  || defined(_M_I86)  || defined(_M_IX86)    || \
    defined(__OS2__)   || defined(sun386)  || defined(__TURBOC__) || \
    defined(vax)       || defined(vms)     || defined(VMS)        || \
    defined(__VMS)
#define RH_LITTLE_ENDIAN
#endif

#if defined(AMIGA)    || defined(applec)  || defined(__AS400__)  || \
    defined(_CRAY)    || defined(__hppa)  || defined(__hp9000)   || \
    defined(ibm370)   || defined(mc68000) || defined(m68k)       || \
    defined(__MRC__)  || defined(__MVS__) || defined(__MWERKS__) || \
    defined(sparc)    || defined(__sparc) || defined(SYMANTEC_C) || \
    defined(__TANDEM) || defined(THINK_C) || defined(__VMCMS__)
#define RH_BIG_ENDIAN
#endif

#if ((!defined(RH_LITTLE_ENDIAN)) && (!defined(RH_BIG_ENDIAN)))

#if defined(LITTLE_ENDIAN) || defined(BIG_ENDIAN)
#if defined(LITTLE_ENDIAN) && !defined(BIG_ENDIAN)
#define RH_LITTLE_ENDIAN
#elif !defined(LITTLE_ENDIAN) &&  defined(BIG_ENDIAN)
#define RH_BIG_ENDIAN
#elif defined(BYTE_ORDER) && (BYTE_ORDER == LITTLE_ENDIAN)
#define RH_LITTLE_ENDIAN
#elif defined(BYTE_ORDER) && (BYTE_ORDER == BIG_ENDIAN)
#define RH_BIG_ENDIAN
#endif

#elif defined(_LITTLE_ENDIAN) || defined(_BIG_ENDIAN)
#if defined(_LITTLE_ENDIAN) && !defined(_BIG_ENDIAN)
#define RH_LITTLE_ENDIAN
#elif !defined(_LITTLE_ENDIAN) &&  defined(_BIG_ENDIAN)
#define RH_BIG_ENDIAN
#elif defined(_BYTE_ORDER) && (_BYTE_ORDER == _LITTLE_ENDIAN)
#define RH_LITTLE_ENDIAN
#elif defined(_BYTE_ORDER) && (_BYTE_ORDER == _BIG_ENDIAN)
#define RH_BIG_ENDIAN
#endif

#elif defined(__LITTLE_ENDIAN__) || defined(__BIG_ENDIAN__)
#if defined(__LITTLE_ENDIAN__) && !defined(__BIG_ENDIAN__)
#define RH_LITTLE_ENDIAN
#elif !defined(__LITTLE_ENDIAN__) &&  defined(__BIG_ENDIAN__)
#define RH_BIG_ENDIAN
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __LITTLE_ENDIAN__)
#define RH_LITTLE_ENDIAN
#elif defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __BIG_ENDIAN__)
#define RH_BIG_ENDIAN
#endif

#elif (('1234' >> 24) == '1')
#define RH_LITTLE_ENDIAN
#elif (('4321' >> 24) == '1')
#define RH_BIG_ENDIAN
#else
#error Unknown platform byte order. Set it manually.
#endif

#endif

// End of endian detection code
///////////////////////////////////////////////////////////////////////////////

#define RH_TARGET_SYSTEM_WINDOWS 1
#define RH_TARGET_SYSTEM_LINUX   2

#if (defined(__FreeBSD__) || defined(__OpenBSD__))
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_LINUX
#elif defined(BSD)
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_LINUX
#elif defined(linux)
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_LINUX
#elif defined(__Linux__)
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_LINUX
#elif defined(WIN32)
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_WINDOWS
#elif defined(_WIN32)
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_WINDOWS
#elif defined(MSC_VER)
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_WINDOWS
#elif defined(_MSC_VER)
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_WINDOWS
#else
	#define RH_TARGET_SYSTEM RH_TARGET_SYSTEM_LINUX
#endif

#undef UWORD8
#undef UWORD16
#undef UWORD32
#undef UWORD64
#undef SFX_32
#undef SFX_64

#pragma pack(1)

#if (defined(__int8) || defined(_MSC_VER))
typedef unsigned __int8 UWORD8;
#elif (UCHAR_MAX == 0xFF)
typedef unsigned char UWORD8;
#else
#error Cannot define an 8-bit unsigned integer type.
#endif

#if (defined(__int16) || defined(_MSC_VER))
typedef unsigned __int16 UWORD16;
#elif (USHRT_MAX == 0xFFFF)
typedef unsigned short UWORD16;
#else
#error Cannot define an 16-bit unsigned integer type.
#endif

#if (defined(__int32) || defined(_MSC_VER))
typedef unsigned __int32 UWORD32;
#define SFX_32
#elif (UINT_MAX == 0xFFFFFFFF)
typedef unsigned int UWORD32;
#define SFX_32
#elif (ULONG_MAX == 0xFFFFFFFF)
typedef unsigned long UWORD32;
#define SFX_32
#else
#error Cannot define an 32-bit unsigned integer type.
#endif

#if (defined(__int64) || defined(_MSC_VER))
typedef unsigned __int64 UWORD64;
#define SFX_64
#elif (UINT_MAX == 0xFFFFFFFFFFFFFFFF)
typedef unsigned int UWORD64;
#define SFX_64
#elif (ULONG_MAX == 0xFFFFFFFFFFFFFFFF)
typedef unsigned long UWORD64;
#define SFX_64 ul
#else
typedef unsigned long long UWORD64;
#define SFX_64 ull
#endif

#ifdef _MSC_VER
#define CONST32(n) (n##ui32)
#define CONST64(n) (n##ui64)
#else
// #define CONST32(n) (n##SFX_32)
#define CONST32(n) (n)
// #define CONST64(n) (n##SFX_64)
#define CONST64(n) (n)
#endif

// Set the preferred integer type. This is the size of a register of the
// processor, but at least 32-bit.
#if (ULONG_MAX >= 0xFFFFFFFF)
typedef unsigned long UINTPREF;
typedef long INTPREF;
#elif (UINT_MAX >= 0xFFFFFFFF)
typedef unsigned int UINTPREF;
typedef int INTPREF;
#else
#error Cannot define the preferred integer type.
#endif

#pragma pack()

#if (RH_TARGET_SYSTEM == RH_TARGET_SYSTEM_WINDOWS)

#include <windows.h>
#include <direct.h>
#define SZ_DIR_CHAR '\\'
#define SZ_DIR_STR  "\\"

#ifndef chdir
#define chdir _chdir
#define getcwd _getcwd
#endif

#else // RH_TARGET_SYSTEM != RH_TARGET_SYSTEM_WINDOWS

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/io.h>
#include <unistd.h>
#include <fnmatch.h>
#include <fcntl.h>
#include <dirent.h>

#define SZ_DIR_CHAR '/'
#define SZ_DIR_STR  "/"

#endif

///////////////////////////////////////////////////////////////////////////////
// Thanks to Tom St Denis for all the STORE and LOAD macros

#ifdef RH_LITTLE_ENDIAN

#define STORE32H(x, y) \
	{ (y)[0] = (UWORD8)(((x)>>24)&255); (y)[1] = (UWORD8)(((x)>>16)&255); \
	(y)[2] = (UWORD8)(((x)>>8)&255); (y)[3] = (UWORD8)((x)&255); }
#define STORE16H(x, y) \
	{ (y)[0] = (UWORD8)(((x)>>8)&255); (y)[1] = (UWORD8)((x)&255); }
#define LOAD32H(x, y) \
	{ x = ((UWORD32)((y)[0] & 255)<<24) | \
	((UWORD32)((y)[1] & 255)<<16) | \
	((UWORD32)((y)[2] & 255)<<8)  | \
	((UWORD32)((y)[3] & 255)); }
#define STORE64H(x, y) \
	{ (y)[0] = (UWORD8)(((x)>>56)&255); (y)[1] = (UWORD8)(((x)>>48)&255); \
	(y)[2] = (UWORD8)(((x)>>40)&255); (y)[3] = (UWORD8)(((x)>>32)&255); \
	(y)[4] = (UWORD8)(((x)>>24)&255); (y)[5] = (UWORD8)(((x)>>16)&255); \
	(y)[6] = (UWORD8)(((x)>>8)&255); (y)[7] = (UWORD8)((x)&255); }
#define LOAD64H(x, y) \
	{ x = (((UWORD64)((y)[0] & 255))<<56)|(((UWORD64)((y)[1] & 255))<<48) | \
	(((UWORD64)((y)[2] & 255))<<40)|(((UWORD64)((y)[3] & 255))<<32) | \
	(((UWORD64)((y)[4] & 255))<<24)|(((UWORD64)((y)[5] & 255))<<16) | \
	(((UWORD64)((y)[6] & 255))<<8)|(((UWORD64)((y)[7] & 255))); }
#define STORE32L(x, y) \
	{ UWORD32 __t = (x); memcpy(y, &__t, 4); }
#define LOAD32L(x, y) \
	memcpy(&(x), y, 4);
#define STORE64L(x, y) \
	{ (y)[7] = (UWORD8)(((x)>>56)&255); (y)[6] = (UWORD8)(((x)>>48)&255); \
	(y)[5] = (UWORD8)(((x)>>40)&255); (y)[4] = (UWORD8)(((x)>>32)&255); \
	(y)[3] = (UWORD8)(((x)>>24)&255); (y)[2] = (UWORD8)(((x)>>16)&255); \
	(y)[1] = (UWORD8)(((x)>>8)&255); (y)[0] = (UWORD8)((x)&255); }
#define LOAD64L(x, y) \
	{ x = (((UWORD64)((y)[7] & 255))<<56)|(((UWORD64)((y)[6] & 255))<<48)| \
	(((UWORD64)((y)[5] & 255))<<40)|(((UWORD64)((y)[4] & 255))<<32)| \
	(((UWORD64)((y)[3] & 255))<<24)|(((UWORD64)((y)[2] & 255))<<16)| \
	(((UWORD64)((y)[1] & 255))<<8)|(((UWORD64)((y)[0] & 255))); }

#elif defined(RH_BIG_ENDIAN)

#define STORE32H(x, y) \
     { UWORD32 __t = (x); memcpy(y, &__t, 4); }
#define STORE16H(x, y) \
	{ UWORD16 __t = (x); memcpy(y, &__t, 2); }
#define LOAD32H(x, y)         \
	memcpy(&(x), y, 4);
#define STORE64H(x, y) \
	{ (y)[7] = (UWORD8)(((x)>>56)&255); (y)[6] = (UWORD8)(((x)>>48)&255); \
	(y)[5] = (UWORD8)(((x)>>40)&255); (y)[4] = (UWORD8)(((x)>>32)&255); \
	(y)[3] = (UWORD8)(((x)>>24)&255); (y)[2] = (UWORD8)(((x)>>16)&255); \
	(y)[1] = (UWORD8)(((x)>>8)&255); (y)[0] = (UWORD8)((x)&255); }
#define LOAD64H(x, y) \
	{ x = (((UWORD64)((y)[7] & 255))<<56)|(((UWORD64)((y)[6] & 255))<<48)| \
	(((UWORD64)((y)[5] & 255))<<40)|(((UWORD64)((y)[4] & 255))<<32)| \
	(((UWORD64)((y)[3] & 255))<<24)|(((UWORD64)((y)[2] & 255))<<16)| \
	(((UWORD64)((y)[1] & 255))<<8)|(((UWORD64)((y)[0] & 255))); }
#define STORE32L(x, y) \
	{ (y)[0] = (UWORD8)(((x)>>24)&255); (y)[1] = (UWORD8)(((x)>>16)&255); \
	(y)[2] = (UWORD8)(((x)>>8)&255); (y)[3] = (UWORD8)((x)&255); }
#define LOAD32L(x, y) \
	{ x = ((UWORD32)((y)[0] & 255)<<24) | \
	((UWORD32)((y)[1] & 255)<<16) | \
	((UWORD32)((y)[2] & 255)<<8)  | \
	((UWORD32)((y)[3] & 255)); }
#define STORE64L(x, y) \
	{ (y)[0] = (UWORD8)(((x)>>56)&255); (y)[1] = (UWORD8)(((x)>>48)&255); \
	(y)[2] = (UWORD8)(((x)>>40)&255); (y)[3] = (UWORD8)(((x)>>32)&255); \
	(y)[4] = (UWORD8)(((x)>>24)&255); (y)[5] = (UWORD8)(((x)>>16)&255); \
	(y)[6] = (UWORD8)(((x)>>8)&255); (y)[7] = (UWORD8)((x)&255); }
#define LOAD64L(x, y) \
	{ x = (((UWORD64)((y)[0] & 255))<<56)|(((UWORD64)((y)[1] & 255))<<48) | \
	(((UWORD64)((y)[2] & 255))<<40)|(((UWORD64)((y)[3] & 255))<<32) | \
	(((UWORD64)((y)[4] & 255))<<24)|(((UWORD64)((y)[5] & 255))<<16) | \
	(((UWORD64)((y)[6] & 255))<<8)|(((UWORD64)((y)[7] & 255))); }
#else
#error Endian undefined. Cannot compile.
#endif

#ifndef strlwr
#define strlwr _strlwr
#endif
#ifndef stricmp
#if (RH_TARGET_SYSTEM != RH_TARGET_SYSTEM_WINDOWS)
#define stricmp strcasecmp
#else
#define stricmp _stricmp
#endif
#endif
#ifndef strnicmpex
#if (RH_TARGET_SYSTEM != RH_TARGET_SYSTEM_WINDOWS)
#define strnicmpex strncasecmp
#else
#define strnicmpex _strnicmp
#endif
#if (RH_TARGET_SYSTEM != RH_TARGET_SYSTEM_WINDOWS)
#define lowcase(c)	tolower(c)
#else
#define lowcase(c)	tolower(c)
#endif

#endif
#ifndef max
#define max(a,b) ((a > b) ? a : b)
#define min(a,b) ((a > b) ? b : a)
#endif

#ifndef ROL32
#if defined(_MSC_VER)
#define ROL32(x,n) _lrotl((x), (n))
#define ROR32(x,n) _lrotr((x), (n))
#else
#define ROL32(x,n) (((x) << (n)) | ((x) >> (32 - (n))))
#define ROR32(x,n) (((x) >> (n)) | ((x) << (32 - (n))))
#endif
#endif

#ifndef ROL64
#define ROL64(x,n) (((x) << (n)) | ((x) >> (64 - (n))))
#define ROR64(x,n) (((x) >> (n)) | ((x) << (64 - (n))))
#endif

#if defined(_MSC_VER)
#pragma intrinsic(_lrotl, _lrotr, abs)
#pragma intrinsic(memcmp, memcpy, memset, strcat, strcmp, strcpy, strlen)
#endif

#if (defined(_DEBUG) || defined(RH_DEBUG_VERSION))
#define RH_ASSERT(x0) { if(!(x0)) { \
	printf("Debug assertion failed in file "); \
	printf((const char *)__FILE__); \
	printf(" in line %d.", __LINE__); rhNewLine(); } }
#define RH_TRACE0(s0) { fprintf(stderr,s0); fprintf(stderr,rhGetNewLine()); }
#define RH_TRACE1(s0,s1) { fprintf(stderr,s0,s1); fprintf(stderr,rhGetNewLine()); }
#define RH_TRACE2(s0,s1,s2) { fprintf(stderr,s0,s1,s2); fprintf(stderr,rhGetNewLine()); }
#else
#define RH_ASSERT(x)
#define RH_TRACE0(s0)
#define RH_TRACE1(s0,s1)
#define RH_TRACE2(s0,s1,s2)
#endif

void rhSetNewLine(bool bWindows);
char *rhGetNewLine();
void rhNewLine();

#endif // ___REHASH_COMMON_H___
