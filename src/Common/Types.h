//
//  Types.h
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#ifndef Types_h
#define Types_h

// ****** Operating System
//
//---------------------------------------------------------------------------
// GameHub works on the following operating systems: (GH_OS_x)
//
//    WIN32    - Win32 (Windows 95/98/ME and Windows NT/2000/XP)
//    DARWIN   - Darwin OS (Mac OS X)
//    LINUX    - Linux
//    ANDROID  - Android
//    IPHONE   - iPhone
//    XBOX360  - Xbox 360 console
//    PS3      - Playstation 3 console
//    WII      - Wii console
//    3DS      - 3DS handheld console

#if (defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))) || defined(__MACOS__)
#   if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
#       define GH_OS_IPHONE
#       define __WIN32__ 0
#       define __ANDROID__ 0
#       define __OSX__ 1
#   else
#       define GH_OS_DARWIN
#       define GH_OS_MAC
#   endif
#elif defined(_XBOX)
#   include <xtl.h>
// Xbox360 and XBox both share _XBOX definition
#   if (_XBOX_VER >= 200)
#       define GH_OS_XBOX360
#   endif
#elif (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#   define GH_OS_WIN32
#   define __WIN32__ 1
#   define __ANDROID__ 0
#   define __OSX__ 0
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#   define GH_OS_WIN32
#   define __WIN32__ 1
#   define __ANDROID__ 0
#   define __OSX__ 0
#elif defined(__MWERKS__) && defined(__INTEL__)
#   define GH_OS_WIN32
#elif defined(__linux__) || defined(__linux)
#   define GH_OS_LINUX
#elif defined(__PPU__)
#   define GH_OS_PS3
#elif defined(RVL)
#   define GH_OS_WII
#elif defined(NN_PLATFORM_CTR)
#   define GH_OS_3DS
#else
#   define GH_OS_OTHER
#endif

#if defined(ANDROID)
#   define GH_OS_ANDROID
#   define __WIN32__ 0
#   define __ANDROID__ 1
#   define __OSX__ 0
#endif


// ***** CPU Architecture
//
//---------------------------------------------------------------------------
// GameHub supports the following CPUs: (GH_CPU_x)
//
//    X86        - x86 (IA-32)
//    X86_64     - x86_64 (amd64)
//    PPC        - PowerPC
//    PPC64      - PowerPC64
//    MIPS       - MIPS
//    OTHER      - CPU for which no special support is present or needed


#if defined(__x86_64__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#   define GH_CPU_X86_64
#   define GH_64BIT_POINTERS
#elif defined(__i386__) || defined(GH_OS_WIN32) || defined(GH_OS_XBOX)
#   define GH_CPU_X86
#elif defined(__powerpc64__) || defined(GH_OS_PS3) || defined(GH_OS_XBOX360) || defined(GH_OS_WII)
#   define GH_CPU_PPC64
// Note: PS3, x360 and WII don't use 64-bit pointers.
#elif defined(__ppc__)
#   define GH_CPU_PPC
#elif defined(__mips__) || defined(__MIPSEL__)
#   define GH_CPU_MIPS
#elif defined(__arm__)
#   define GH_CPU_ARM
#else
#   define GH_CPU_OTHER
#endif


// ***** Include Config.h
//
//---------------------------------------------------------------------------
//#ifndef GH_CONFIG_H
//#define GH_CONFIG_H "Config.h"
//#endif

// Include conditional compilation options file
// Needs to be after OS detection code
//#include GH_CONFIG_H


// ***** Compiler
//
//---------------------------------------------------------------------------
//  GameHub is compatible with the following compilers: (GH_CC_x)
//
//     MSVC     - Microsoft Visual C/C++
//     INTEL    - Intel C++ for Linux / Windows
//     GNU      - GNU C++
//     SNC      - SN Systems ProDG C/C++ (EDG)
//     MWERKS   - Metrowerks CodeWarrior
//     BORLAND  - Borland C++ / C++ Builder
//     ARM      - ARM C/C++

#if defined(__INTEL_COMPILER)
// Intel 4.0                    = 400
// Intel 5.0                    = 500
// Intel 6.0                    = 600
// Intel 8.0                    = 800
// Intel 9.0                    = 900
#   define GH_CC_INTEL       __INTEL_COMPILER

#elif defined(_MSC_VER)
// MSVC 5.0                     = 1100
// MSVC 6.0                     = 1200
// MSVC 7.0 (VC2002)            = 1300
// MSVC 7.1 (VC2012)            = 1310
// MSVC 8.0 (VC2005)            = 1400
#   define GH_CC_MSVC        _MSC_VER

#elif defined(__MWERKS__)
// Metrowerks C/C++ 2.0         = 0x2000
// Metrowerks C/C++ 2.2         = 0x2200
#   define GH_CC_MWERKS      __MWERKS__

#elif defined(__BORLANDC__) || defined(__TURBOC__)
// Borland C++ 5.0              = 0x500
// Borland C++ 5.02             = 0x520
// C++ Builder 1.0              = 0x520
// C++ Builder 3.0              = 0x530
// C++ Builder 4.0              = 0x540
// Borland C++ 5.5              = 0x550
// C++ Builder 5.0              = 0x550
// Borland C++ 5.51             = 0x551
#   define GH_CC_BORLAND     __BORLANDC__

// SNC must come before GNUC because
// the SNC compiler defines GNUC as well
#elif defined(__SNC__)
#   define GH_CC_SNC

#elif defined(__GNUC__)
#   define GH_CC_GNU

#elif defined(__CC_ARM)
#   define GH_CC_ARM

#else
#   error "GameHub does not support this Compiler"
#endif


// Ignore compiler warnings specific to SN compiler
#if defined(GH_CC_SNC)
#   pragma diag_suppress=68   // integer conversion resulted in a change of sign
#   pragma diag_suppress=112  // statement is unreachable
// (possibly due to SN disliking modifying a mutable object in a const function)
#   pragma diag_suppress=175  // expression has no effect
// (deliberately placed so it would NOT produce a warning...)
#   pragma diag_suppress=178  // variable was declared but never referenced
#   pragma diag_suppress=382  // extra ";" ignored
#   pragma diag_suppress=552  // variable was set but never used
#   pragma diag_suppress=613  // overloaded virtual function is only partially overridden in class
#   pragma diag_suppress=999  // function funcA is hidden by funcB -- virtual function override intended?

#   pragma diag_suppress=1011  // function funcA is hidden by funcB -- virtual function override intended?
#   ifdef GH_OS_PS3
#       pragma diag_suppress=1421 // trigraphs (in comments)
#   endif
#endif


// *** Linux Unicode - must come before Standard Includes

#ifdef GH_OS_LINUX
/* Use glibc unicode functions on linux. */
#   ifndef  _GNU_SOURCE
#       define _GNU_SOURCE
#   endif
#endif

// *** Symbian - fix defective header files

#ifdef GH_OS_SYMBIAN
#   include <_ansi.h>
#       undef _STRICT_ANSI
#   include <stdlib.h>
#endif

#ifdef GH_OS_WII
#   include <types.h>
#endif

#ifdef GH_OS_3DS
#   include <nn/types.h>
#endif

// ***** Standard Includes
//
//---------------------------------------------------------------------------
#include    <stddef.h>
#include    <limits.h>
#include    <float.h>

// This macro needs to be defined if it is necessary to avoid the use of Double.
// In that case Double in defined as float and thus extra #ifdef checks on
// overloads need to be done. Useful for platforms with poor/unavailable
// Double support

//# define GH_NO_DOUBLE


// *****  Definitions
//
//---------------------------------------------------------------------------
// Byte order
#define GH_LITTLE_ENDIAN       1
#define GH_BIG_ENDIAN          2

namespace GameHub {
    // Pointer-size integer
    typedef size_t              UPInt;
    typedef ptrdiff_t           SPInt;
    
} // namespace GameHub {

// **** Win32 or XBox
//-----------------------
#if defined(GH_OS_WIN32) || defined(GH_OS_XBOX360)

// ** Miscellaneous compiler definitions

// DLL linking options
//-----------------------
#   ifdef GH_BUILD_STATICLIB
// BUILD_STATICLIB means no DLL support, so disable symbol exports
#       define    GH_IMPORT
#       define    GH_EXPORT
#   else
#       define    GH_IMPORT   __declspec(dllimport)
#       define    GH_EXPORT   __declspec(dllexport)
#   endif // GH_BUILD_STATICLIB

#   define GH_EXPORTC      extern "C" GH_EXPORT

// Build is used for importing (must be defined to nothing)
#   define GH_BUILD

// Calling convention - goes after function return type but before function name
#   ifdef __cplusplus_cli
#       define GH_FASTCALL         __stdcall
#   else
#       define GH_FASTCALL         __fastcall
#   endif

#   define GH_STDCALL              __stdcall
#   define GH_CDECL                __cdecl

// Byte order
#   if defined(GH_OS_XBOX360)
#       define           GH_BIG_ENDIAN
#   else
#       define GH_BYTE_ORDER          GH_LITTLE_ENDIAN
#   endif

#   if defined(GH_CC_MSVC)
// Disable "inconsistent dll linkage" warning
#       pragma warning(disable : 4127)
// Disable "exception handling" warning
#       pragma warning(disable : 4530)
#       if (GH_CC_MSVC<1300)
// Disable "unreferenced inline function has been removed" warning
#           pragma warning(disable : 4514)
// Disable "function not inlined" warning
#           pragma warning(disable : 4710)
// Disable "_force_inline not inlined" warning
#           pragma warning(disable : 4714)
// Disable "debug variable name longer than 255 chars" warning
#           pragma warning(disable : 4786)
#       endif // (GH_CC_MSVC<1300)
#   endif // (GH_CC_MSVC)

// Assembly macros
#   if defined(GH_CC_MSVC)
#       define GH_ASM                _asm
#   else
#       define GH_ASM                asm
#   endif // (GH_CC_MSVC)

// Inline substitute - goes before function declaration
#   if defined(GH_CC_MSVC)
#       define GH_INLINE          __forceinline
#   else
#       define GH_INLINE          inline
#   endif  // GH_CC_MSVC


#   ifdef UNICODE
#       define GH_STR(str) L##str
#   else
#       define GH_STR(str) str
#   endif // UNICODE

#else

#   if (defined(BYTE_ORDER) && (BYTE_ORDER == BIG_ENDIAN))|| \
(defined(_BYTE_ORDER) && (_BYTE_ORDER == _BIG_ENDIAN))
#       define GH_BYTE_ORDER          GH_BIG_ENDIAN
#   elif (defined(GH_OS_PS3) || defined(__ARMEB__) || defined(GH_CPU_PPC) || defined(GH_CPU_PPC64))
#       define GH_BYTE_ORDER          GH_BIG_ENDIAN
#   elif defined (GH_OS_WII)
#       define GH_BYTE_ORDER          GH_BIG_ENDIAN

#   else
#       define GH_BYTE_ORDER          GH_LITTLE_ENDIAN

#   endif

// **** Standard systems

// ** Miscellaneous compiler definitions

// DLL linking options
#   define GH_IMPORT
#   define GH_EXPORT
#   define GH_EXPORTC              extern "C" GH_EXPORT

// Build is used for importing, defined to nothing
#   define GH_BUILD

// Assembly macros
#   define GH_ASM                  __asm__
#   define GH_ASM_PROC(procname)   GH_ASM
#   define GH_ASM_END              GH_ASM

// Inline substitute - goes before function declaration
#   define GH_INLINE               inline

// Calling convention - goes after function return type but before function name
#   define GH_FASTCALL
#   define GH_STDCALL
#   define GH_CDECL

#endif // defined(GH_OS_WIN32) || defined(GH_OS_XBOX) || defined(GH_OS_XBOX360) || defined(GH_OS_WINCE)

// ***** Type definitions for Win32
//---------------------------------------------------------------------------
namespace GameHub {
    
    typedef char                Char;
    
    // *** Type definitions for common systems
    
    typedef char                Char;
    
#if defined(GH_OS_WIN32) || defined(GH_OS_XBOX360)
    // 8 bit Integer (Byte)
    typedef char                SInt8;
    typedef unsigned char       UInt8;
    typedef SInt8               SByte;
    typedef UInt8               UByte;
    
    // 16 bit Integer (Word)
    typedef short               SInt16;
    typedef unsigned short      UInt16;
    
    // 32 bit Integer (DWord)
    // #if defined(GH_OS_LINUX) && defined(GH_CPU_X86_64)
    // typedef int                 SInt32;
    // typedef unsigned int        UInt32;
    // #else
    // typedef long                SInt32;
    // typedef unsigned long       UInt32;
    // #endif
    
    typedef long                SInt32;
    typedef unsigned long       UInt32;
    
    // 64 bit Integer (QWord)
    typedef __int64             SInt64;
    typedef unsigned __int64    UInt64;
    
    // Floating point
    //typedef float               float;
    
#ifdef GH_NO_DOUBLE
    typedef float               Double;
#else
    typedef double              Double;
#endif
    
#elif defined(GH_OS_MAC) || defined(GH_OS_IPHONE)
    
    typedef int          SInt8  __attribute__((__mode__ (__QI__)));
    typedef unsigned int UInt8  __attribute__((__mode__ (__QI__)));
    typedef int          SByte  __attribute__((__mode__ (__QI__)));
    typedef unsigned int UByte  __attribute__((__mode__ (__QI__)));
    
    typedef int          SInt16 __attribute__((__mode__ (__HI__)));
    typedef unsigned int UInt16 __attribute__((__mode__ (__HI__)));
    
    typedef long          SInt32; // match <CarbonCore/MacTypes.h>
    typedef unsigned long UInt32;
    
    typedef int          SInt64 __attribute__((__mode__ (__DI__)));
    typedef unsigned int UInt64 __attribute__((__mode__ (__DI__)));
    
#elif defined(GH_CC_GNU) || defined(GH_CC_SNC)
    
    typedef int          SInt8  __attribute__((__mode__ (__QI__)));
    typedef unsigned int UInt8  __attribute__((__mode__ (__QI__)));
    typedef int          SByte  __attribute__((__mode__ (__QI__)));
    typedef unsigned int UByte  __attribute__((__mode__ (__QI__)));
    
    typedef int          SInt16 __attribute__((__mode__ (__HI__)));
    typedef unsigned int UInt16 __attribute__((__mode__ (__HI__)));
    
    typedef int          SInt32 __attribute__((__mode__ (__SI__)));
    typedef unsigned int UInt32 __attribute__((__mode__ (__SI__)));
    
    typedef int          SInt64 __attribute__((__mode__ (__DI__)));
    typedef unsigned int UInt64 __attribute__((__mode__ (__DI__)));
    
    
#elif defined(GH_OS_WII) || defined(GH_OS_3DS)
    
    // 8 bit Integer (Byte)
    typedef s8              SInt8;
    typedef u8              UInt8;
    typedef s8              SByte;
    typedef u8              UByte;
    
    // 16 bit Integer
    typedef s16             SInt16;
    typedef u16             UInt16;
    
    // 32 bit Integer
    typedef s32             SInt32;
    typedef u32             UInt32;
    
    // 64 bit Integer
    typedef s64             SInt64;
    typedef u64             UInt64;
    
#else
#include <sys/types.h>
    
    // 8 bit Integer (Byte)
    typedef int8_t              SInt8;
    typedef uint8_t             UInt8;
    typedef int8_t              SByte;
    typedef uint8_t             UByte;
    
    // 16 bit Integer
    typedef int16_t             SInt16;
    typedef uint16_t            UInt16;
    
    // 32 bit Integer
    typedef int32_t             SInt32;
    typedef uint32_t            UInt32;
    
    // 64 bit Integer
    typedef int64_t             SInt64;
    typedef uint64_t            UInt64;
    
#endif
    
    // Floating point
    //typedef float               float;
    
#ifdef GH_NO_DOUBLE
    typedef float               Double;
#else
    typedef double              Double;
#endif // GH_NO_DOUBLE
    
    //
    // ***** BaseTypes Namespace
    
    // BaseTypes namespace is explicitly declared to allow base types to be used
    // by customers directly without other contents of GameHub namespace.
    
    namespace BaseTypes
    {
        using GameHub::UPInt;
        using GameHub::SPInt;
        using GameHub::UByte;
        using GameHub::SByte;
        using GameHub::UInt16;
        using GameHub::SInt16;
        using GameHub::UInt32;
        using GameHub::SInt32;
        using GameHub::UInt64;
        using GameHub::SInt64;
        using GameHub::Double;
        
        typedef UInt16 uint16;
        typedef SInt16 int16;
        typedef UInt32 uint32;
        typedef SInt32 int32;
        typedef UInt64 uint64;
        typedef SInt64 int64;
        typedef unsigned char uint8;
        typedef char   int8;
    } // BaseTypes
    
    using namespace BaseTypes;
    
} // GameHub


// ***** GH_DEBUG_BREAK, GH_ASSERT
//
//---------------------------------------------------------------------------
// If not in debug build, macros do nothing
//#ifndef GH_BUILD_DEBUG

//#   define GH_DEBUG_BREAK  ((void)0)
//#   define GH_ASSERT(p)    ((void)0)

//#else

// Microsoft Win32 specific debugging support
//#if (defined(GH_OS_WIN32) && !defined(__cplusplus_cli)) || defined(GH_OS_XBOX)
#   if defined(GH_OS_WIN32)
#       ifdef GH_CPU_X86
#           if defined(__cplusplus_cli)
#               define GH_DEBUG_BREAK do { __debugbreak(); } while(0)
#           elif defined(GH_CC_GNU)
#               define GH_DEBUG_BREAK do { GH_ASM("int $3\n\t"); } while(0)
#           else
#               define GH_DEBUG_BREAK do { GH_ASM int 3 } while (0)
#           endif
#       else
#           define GH_DEBUG_BREAK     do { __debugbreak(); } while(0)
#       endif
// Xbox360 specific debugging
#   elif defined(GH_OS_XBOX360)
#       define GH_DEBUG_BREAK         DebugBreak()
// Unix specific debugging support
#   elif defined(GH_CPU_X86)
#       define GH_DEBUG_BREAK     do { GH_ASM("int $3\n\t"); } while(0)
#   elif defined(GH_OS_3DS)
#       include <nn/dbg.h>
#       define GH_DEBUG_BREAK     do { nn::dbg::Break(nn::dbg::BREAK_REASON_USER); } while(0)
#   else
#       define GH_DEBUG_BREAK     do { *((int *) 0) = 1; } while(0)
#   endif

//#endif // GH_BUILD_DEBUG

// This will cause compiler breakpoint
#   define GH_ASSERT(p)       do { if (!(p))  { GH_DEBUG_BREAK; } } while(0)

// ***** Compiler Assert
//
//---------------------------------------------------------------------------
// Compile-time assert.  Thanks to Jon Jagger (http://www.jaggersoft.com) for this trick.
#define GH_COMPILER_ASSERT(x)   { int assertVar = 0; switch(assertVar){case 0: case x:;} }

// Handy macro to quiet compiler warnings about unused parameters/variables.
#if defined(GH_CC_GNU)
#   define     GH_UNUSED(a)          do {__typeof__ (&a) __attribute__ ((unused)) __tmp = &a; } while(0)
#   define     GH_UNUSED2(a,b)       GH_UNUSED(a); GH_UNUSED(b)
#   define     GH_UNUSED3(a,b,c)     GH_UNUSED2(a,c); GH_UNUSED(b)
#   define     GH_UNUSED4(a,b,c,d)   GH_UNUSED3(a,c,d); GH_UNUSED(b)
#else
#   define     GH_UNUSED(a)          (a)
#   define     GH_UNUSED2(a,b)       (a);(b)
#   define     GH_UNUSED3(a,b,c)     (a);(b);(c)
#   define     GH_UNUSED4(a,b,c,d)   (a);(b);(c);(d)
#endif

#define     GH_UNUSED1(a) GH_UNUSED(a)
#define     GH_UNUSED5(a1,a2,a3,a4,a5) GH_UNUSED4(a1,a2,a3,a4); GH_UNUSED(a5)
#define     GH_UNUSED6(a1,a2,a3,a4,a5,a6) GH_UNUSED4(a1,a2,a3,a4); GH_UNUSED2(a5,a6)
#define     GH_UNUSED7(a1,a2,a3,a4,a5,a6,a7) GH_UNUSED4(a1,a2,a3,a4); GH_UNUSED3(a5,a6,a7)


// ******** Variable range definitions
//
//---------------------------------------------------------------------------
#if defined(GH_CC_MSVC)
#   define GH_UINT64(x) x
#else
#   define GH_UINT64(x) x##LL
#endif

// 8 bit Integer ranges (Byte)
#define GH_MAX_SINT8           GameHub::SInt8(0x7F)                   //  127
#define GH_MIN_SINT8           GameHub::SInt8(0x80)                   // -128
#define GH_MAX_UINT8           GameHub::UInt8(0xFF)                   //  255
#define GH_MIN_UINT8           GameHub::UInt8(0x00)                   //  0

#define GH_MAX_SBYTE           GH_MAX_SINT8                             //  127
#define GH_MIN_SBYTE           GH_MIN_SINT8                             // -128
#define GH_MAX_UBYTE           GH_MAX_UINT8                             //  255
#define GH_MIN_UBYTE           GH_MIN_UINT8                             //  0

// 16 bit Integer ranges (Word)
#define GH_MAX_SINT16          GameHub::SInt16(SHRT_MAX)              //  32767
#define GH_MIN_SINT16          GameHub::SInt16(SHRT_MIN)              // -32768
#define GH_MAX_UINT16          GameHub::UInt16(USHRT_MAX)             //  65535
#define GH_MIN_UINT16          GameHub::UInt16(0)                     //  0

// 32 bit Integer ranges (Int)
#define GH_MAX_SINT32          GameHub::SInt32(INT_MAX)               //  2147483647
#define GH_MIN_SINT32          GameHub::SInt32(INT_MIN)               // -2147483648
#define GH_MAX_UINT32          GameHub::UInt32(UINT_MAX)              //  4294967295
#define GH_MIN_UINT32          GameHub::UInt32(0)                     //  0

// 64 bit Integer ranges (Long)
#define GH_MAX_SINT64          GameHub::SInt64(0x7FFFFFFFFFFFFFFF)    // -9223372036854775808
#define GH_MIN_SINT64          GameHub::SInt64(0x8000000000000000)    //  9223372036854775807
#define GH_MAX_UINT64          GameHub::UInt64(0xFFFFFFFFFFFFFFFF)    //  18446744073709551615
#define GH_MIN_UINT64          GameHub::UInt64(0)                     //  0

// Compiler specific
#define GH_MAX_SINT            int(GH_MAX_SINT32)                       //  2147483647
#define GH_MIN_SINT            int(GH_MIN_SINT32)                       // -2147483648
#define GH_MAX_UINT            unsigned(GH_MAX_UINT32)                  //  4294967295
#define GH_MIN_UINT            unsigned(GH_MIN_UINT32)                  //  0


#if defined(GH_64BIT_POINTERS)
#   define GH_MAX_SPINT           GH_MAX_SINT64
#   define GH_MIN_SPINT           GH_MIN_SINT64
#   define GH_MAX_UPINT           GH_MAX_UINT64
#   define GH_MIN_UPINT           GH_MIN_UINT64
#else
#   define GH_MAX_SPINT           GH_MAX_SINT
#   define GH_MIN_SPINT           GH_MIN_SINT
#   define GH_MAX_UPINT           GH_MAX_UINT
#   define GH_MIN_UPINT           GH_MIN_UINT
#endif

// ***** Floating point ranges
//
//---------------------------------------------------------------------------
// Minimum and maximum (positive) float values
#define GH_MIN_FLOAT           float(FLT_MIN)               // 1.175494351e-38F
#define GH_MAX_FLOAT           float(FLT_MAX)               // 3.402823466e+38F

#ifdef GH_NO_DOUBLE
// Minimum and maximum (positive) Double values
#   define GH_MIN_DOUBLE          GameHub::Double(FLT_MIN)   // 1.175494351e-38F
#   define GH_MAX_DOUBLE          GameHub::Double(FLT_MAX)   // 3.402823466e+38F
#else
// Minimum and maximum (positive) Double values
#   define GH_MIN_DOUBLE          GameHub::Double(DBL_MIN)   // 2.2250738585072014e-308
#   define GH_MAX_DOUBLE          GameHub::Double(DBL_MAX)   // 1.7976931348623158e+308
#endif


// ***** Flags
//
//---------------------------------------------------------------------------
#define GH_FLAG32(value)       (GameHub::UInt32(value))
#define GH_FLAG64(value)       (GameHub::UInt64(value))




namespace GameHub {
    
    template <typename T1, typename T2>
    struct Pair
    {
        typedef T1 FirstType;
        typedef T2 SecondType;
        
        Pair(const FirstType& f, const SecondType& s) : First(f), Second(s) {}
        
        T1 First;
        T2 Second;
    };
    
} //namespace GameHub {

#define GH_NEW  new
#define GH_DELETE  delete
#define GH_NEW_T(T, category, typedesc) new T
#define GH_DELETE_T(ptr, T, category) delete ptr;

#define SAFE_RELEASE(ptr) do{if(ptr){ptr->Release(); ptr=NULL;} }while(0)

#endif /* Types_h */
