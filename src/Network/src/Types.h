//
//  Types.h
//  TestClient
//
//  Created by chopdown on 13-1-11.
//  Copyright (c) 2013年 chopdown. All rights reserved.
//

#ifndef TestClient_Types_h
#define TestClient_Types_h

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
#if (defined(__ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__) || defined(__IPHONE_OS_VERSION_MIN_REQUIRED))
#define GH_OS_IPHONE
#else
# define GH_OS_DARWIN
# define GH_OS_MAC
#endif
#elif defined(_XBOX)
# include <xtl.h>
// Xbox360 and XBox both share _XBOX definition
#if (_XBOX_VER >= 200)
# define GH_OS_XBOX360
#endif
#elif (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
# define GH_OS_WIN32
#elif (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
# define GH_OS_WIN32
#elif defined(__MWERKS__) && defined(__INTEL__)
# define GH_OS_WIN32
#elif defined(__linux__) || defined(__linux)
# define GH_OS_LINUX
#elif defined(__PPU__)
# define GH_OS_PS3
#elif defined(RVL)
# define GH_OS_WII
#elif defined(NN_PLATFORM_CTR)
# define GH_OS_3DS
#else
# define GH_OS_OTHER
#endif

#if defined(ANDROID)
# define GH_OS_ANDROID
#endif


// ***** CPU Architecture
//
//---------------------------------------------------------------------------
// GameHub supports the following CPUs: (GH_CPU_x)
//
//    X86        - x86 (IA-32)
//    X86_64     - x86_64 (aGH64)
//    PPC        - PowerPC
//    PPC64      - PowerPC64
//    MIPS       - MIPS
//    OTHER      - CPU for which no special support is present or needed


#if defined(__x86_64__) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__)
#  define GH_CPU_X86_64
#  define GH_64BIT_POINTERS
#elif defined(__i386__) || defined(GH_OS_WIN32) || defined(GH_OS_XBOX)
#  define GH_CPU_X86
#elif defined(__powerpc64__) || defined(GH_OS_PS3) || defined(GH_OS_XBOX360) || defined(GH_OS_WII)
#  define GH_CPU_PPC64
// Note: PS3, x360 and WII don't use 64-bit pointers.
#elif defined(__ppc__)
#  define GH_CPU_PPC
#elif defined(__mips__) || defined(__MIPSEL__)
#  define GH_CPU_MIPS
#elif defined(__arm__)
#  define GH_CPU_ARM
#else
#  define GH_CPU_OTHER
#endif



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
# define GH_CC_INTEL       __INTEL_COMPILER

#elif defined(_MSC_VER)
// MSVC 5.0                     = 1100
// MSVC 6.0                     = 1200
// MSVC 7.0 (VC2002)            = 1300
// MSVC 7.1 (VC2012)            = 1310
// MSVC 8.0 (VC2005)            = 1400
# define GH_CC_MSVC        _MSC_VER

#elif defined(__MWERKS__)
// Metrowerks C/C++ 2.0         = 0x2000
// Metrowerks C/C++ 2.2         = 0x2200
# define GH_CC_MWERKS      __MWERKS__

#elif defined(__BORLANDC__) || defined(__TURBOC__)
// Borland C++ 5.0              = 0x500
// Borland C++ 5.02             = 0x520
// C++ Builder 1.0              = 0x520
// C++ Builder 3.0              = 0x530
// C++ Builder 4.0              = 0x540
// Borland C++ 5.5              = 0x550
// C++ Builder 5.0              = 0x550
// Borland C++ 5.51             = 0x551
# define GH_CC_BORLAND     __BORLANDC__

// SNC must come before GNUC because
// the SNC compiler defines GNUC as well
#elif defined(__SNC__)
# define GH_CC_SNC

#elif defined(__GNUC__)
# define GH_CC_GNU

#elif defined(__CC_ARM)
# define GH_CC_ARM

#else
# error "GH does not support this Compiler"
#endif

// ***** Standard Includes
//
//---------------------------------------------------------------------------
#include    <stddef.h>
#include    <limits.h>
#include    <float.h>

// *****  Definitions
//
//---------------------------------------------------------------------------
// Byte order
#define GH_LITTLE_ENDIAN       1
#define GH_BIG_ENDIAN          2

namespace GameHub {
    

    
#if defined(GH_OS_WIN32) || defined(GH_OS_XBOX360)
    // 8 bit Integer (Byte)
    typedef char                int8;
    typedef unsigned char       uint8;
    
    // 16 bit Integer (Word)
    typedef short               int16;
    typedef unsigned short      uint16;
    
    
    typedef long                int32;
    typedef unsigned long       uint32;
    
    // 64 bit Integer (QWord)
    typedef __int64             int64;
    typedef unsigned __int64    uint64;
    
    
#elif defined(GH_OS_MAC) || defined(GH_OS_IPHONE)
    
    typedef int          int8  __attribute__((__mode__ (__QI__)));
    typedef unsigned int uint8  __attribute__((__mode__ (__QI__)));
    
    typedef int          int16 __attribute__((__mode__ (__HI__)));
    typedef unsigned int uint16 __attribute__((__mode__ (__HI__)));
    
    typedef long          int32; // match <CarbonCore/MacTypes.h>
    typedef unsigned long uint32;
    
    typedef int          int64 __attribute__((__mode__ (__DI__)));
    typedef unsigned int uint64 __attribute__((__mode__ (__DI__)));
    
#elif defined(GH_CC_GNU) || defined(GH_CC_SNC)
    
    typedef int          int8  __attribute__((__mode__ (__QI__)));
    typedef unsigned int uint8  __attribute__((__mode__ (__QI__)));

    
    typedef int          int16 __attribute__((__mode__ (__HI__)));
    typedef unsigned int uint16 __attribute__((__mode__ (__HI__)));
    
    typedef int          int32 __attribute__((__mode__ (__SI__)));
    typedef unsigned int uint32 __attribute__((__mode__ (__SI__)));
    
    typedef int          int64 __attribute__((__mode__ (__DI__)));
    typedef unsigned int uint64 __attribute__((__mode__ (__DI__)));
    
    
#elif defined(GH_OS_WII) || defined(GH_OS_3DS)
    
    // 8 bit Integer (Byte)
    typedef s8              int8;
    typedef u8              uint8;
    
    // 16 bit Integer
    typedef s16             int16;
    typedef u16             uint16;
    
    // 32 bit Integer
    typedef s32             int32;
    typedef u32             uint32;
    
    // 64 bit Integer
    typedef s64             int64;
    typedef u64             uint64;
    
#else
#include <sys/types.h>
    
    // 8 bit Integer (Byte)
    typedef int8_t              int8;
    typedef uint8_t             uint8;
    
    // 16 bit Integer
    typedef int16_t             int16;
    typedef uint16_t            uint16;
    
    // 32 bit Integer
    typedef int32_t             int32;
    typedef uint32_t            uint32;
    
    // 64 bit Integer
    typedef int64_t             int64;
    typedef uint64_t            uint64;
    
#endif
    

    
    //
    // ***** BaseTypes Namespace
    
    // BaseTypes namespace is explicitly declared to allow base types to be used
    // by customers directly without other contents of GameHub namespace.
    
    namespace BaseTypes
    {
        using GameHub::uint16;
        using GameHub::int16;
        using GameHub::uint32;
        using GameHub::int32;
        using GameHub::uint64;
        using GameHub::int64;
    } // BaseTypes
    
} // GameHub

#endif
