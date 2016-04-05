//
//  Std.h
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#ifndef Std_h
#define Std_h

#include "Types.h"
#include <stdarg.h> // for va_list args
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#if !defined(GH_OS_WINCE) && defined(GH_CC_MSVC) && (GH_CC_MSVC >= 1400)
#define GH_MSVC_SAFESTRING
#include <errno.h>
#endif

// Wide-char funcs
#if !defined(GH_OS_SYMBIAN) && !defined(GH_CC_RENESAS) && !defined(GH_OS_PS2)
#include <wchar.h>
#endif

#if !defined(GH_OS_SYMBIAN) && !defined(GH_CC_RENESAS) && !defined(GH_OS_PS2) && !defined(GH_CC_SNC)
#include <wctype.h>
#endif

namespace GameHub {
    
#if defined(GH_OS_WIN32)
    inline char* GH_CDECL GHitoa(int val, char *dest, UPInt destsize, int radix)
    {
#if defined(GH_MSVC_SAFESTRING)
        _itoa_s(val, dest, destsize, radix);
        return dest;
#else
        GH_UNUSED(destsize);
        return itoa(val, dest, radix);
#endif
    }
#else // GH_OS_WIN32
    inline char* GHitoa(int val, char* dest, unsigned int len, int radix)
    {
        if (val == 0)
        {
            if (len > 1)
            {
                dest[0] = '0';
                dest[1] = '\0';
            }
            return dest;
        }
        
        int cur = val;
        unsigned int i    = 0;
        unsigned int sign = 0;
        
        if (val < 0)
        {
            val = -val;
            sign = 1;
        }
        
        while ((val != 0) && (i < (len - 1 - sign)))
        {
            cur    = val % radix;
            val   /= radix;
            
            if (radix == 16)
            {
                switch(cur)
                {
                    case 10:
                        dest[i] = 'a';
                        break;
                    case 11:
                        dest[i] = 'b';
                        break;
                    case 12:
                        dest[i] = 'c';
                        break;
                    case 13:
                        dest[i] = 'd';
                        break;
                    case 14:
                        dest[i] = 'e';
                        break;
                    case 15:
                        dest[i] = 'f';
                        break;
                    default:
                        dest[i] = (char)('0' + cur);
                        break;
                }
            }
            else
            {
                dest[i] = (char)('0' + cur);
            }
            ++i;
        }
        
        if (sign)
        {
            dest[i++] = '-';
        }
        
        for (unsigned int j = 0; j < i / 2; ++j)
        {
            char tmp        = dest[j];
            dest[j]         = dest[i - 1 - j];
            dest[i - 1 - j] = tmp;
        }
        dest[i] = '\0';
        
        return dest;
    }
    
#endif
    
    
    // String functions
    
    inline UPInt GH_CDECL GHstrlen(const char* str)
    {
        return strlen(str);
    }
    
    inline char* GH_CDECL GHstrcpy(char* dest, UPInt destsize, const char* src)
    {
#if defined(GH_MSVC_SAFESTRING)
        strcpy_s(dest, destsize, src);
        return dest;
#else
        GH_UNUSED(destsize);
        return strcpy(dest, src);
#endif
    }
    
    inline char* GH_CDECL GHstrncpy(char* dest, UPInt destsize, const char* src, UPInt count)
    {
#if defined(GH_MSVC_SAFESTRING)
        strncpy_s(dest, destsize, src, count);
        return dest;
#else
        GH_UNUSED(destsize);
        return strncpy(dest, src, count);
#endif
    }
    
    inline char * GH_CDECL GHstrcat(char* dest, UPInt destsize, const char* src)
    {
#if defined(GH_MSVC_SAFESTRING)
        strcat_s(dest, destsize, src);
        return dest;
#else
        GH_UNUSED(destsize);
        return strcat(dest, src);
#endif
    }
    
    inline int GH_CDECL GHstrcmp(const char* dest, const char* src)
    {
        return strcmp(dest, src);
    }
    
    inline const char* GH_CDECL GHstrchr(const char* str, char c)
    {
        return strchr(str, c);
    }
    
    inline char* GH_CDECL GHstrchr(char* str, char c)
    {
        return strchr(str, c);
    }
    
    inline const char* GHstrrchr(const char* str, char c)
    {
        UPInt len = GHstrlen(str);
        for (UPInt i=len; i>0; i--)
            if (str[i]==c)
                return str+i;
        return 0;
    }
    
    inline const UByte* GH_CDECL GHmemrchr(const UByte* str, UPInt size, UByte c)
    {
        for (SPInt i = (SPInt)size - 1; i >= 0; i--)
        {
            if (str[i] == c)
                return str + i;
        }
        return 0;
    }
    
    inline char* GH_CDECL GHstrrchr(char* str, char c)
    {
        UPInt len = GHstrlen(str);
        for (UPInt i=len; i>0; i--)
            if (str[i]==c)
                return str+i;
        return 0;
    }
    
    
    double GH_CDECL GHstrtod(const char* string, char** tailptr);
    
    inline long GH_CDECL GHstrtol(const char* string, char** tailptr, int radix)
    {
        return strtol(string, tailptr, radix);
    }
    
    inline long GH_CDECL GHstrtoul(const char* string, char** tailptr, int radix)
    {
        return strtoul(string, tailptr, radix);
    }
    
    inline int GH_CDECL GHstrncmp(const char* ws1, const char* ws2, UPInt size)
    {
        return strncmp(ws1, ws2, size);
    }
    
    inline UInt64 GH_CDECL GHstrtouq(const char *nptr, char **endptr, int base)
    {
#if defined(GH_CC_MSVC) && !defined(GH_OS_WINCE)
        return _strtoui64(nptr, endptr, base);
#else
        return strtoull(nptr, endptr, base);
#endif
    }
    
    inline SInt64 GH_CDECL GHstrtoq(const char *nptr, char **endptr, int base)
    {
#if defined(GH_CC_MSVC) && !defined(GH_OS_WINCE)
        return _strtoi64(nptr, endptr, base);
#else
        return strtoll(nptr, endptr, base);
#endif
    }
    
    
    inline SInt64 GH_CDECL GHatoq(const char* string)
    {
#if defined(GH_CC_MSVC) && !defined(GH_OS_WINCE)
        return _atoi64(string);
#else
        return atoll(string);
#endif
    }
    
    inline UInt64 GH_CDECL GHatouq(const char* string)
    {
        return GHstrtouq(string, NULL, 10);
    }
    
    
    // Implemented in GStd.cpp in platform-specific manner.
    int GH_CDECL GHstricmp(const char* dest, const char* src);
    int GH_CDECL GHstrnicmp(const char* dest, const char* src, UPInt count);
    
    inline UPInt GH_CDECL GHsprintf(char *dest, UPInt destsize, const char* format, ...)
    {
        va_list argList;
        va_start(argList,format);
        UPInt ret;
#if defined(GH_CC_MSVC)
#if defined(GH_MSVC_SAFESTRING)
        ret = _vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
        GH_ASSERT(ret != -1);
#else
        GH_UNUSED(destsize);
        ret = _vsnprintf(dest, destsize - 1, format, argList); // -1 for space for the null character
        GH_ASSERT(ret != -1);
        dest[destsize-1] = 0;
#endif
#else
        GH_UNUSED(destsize);
        ret = vsprintf(dest, format, argList);
        GH_ASSERT(ret < destsize);
#endif
        va_end(argList);
        return ret;
    }
    
    inline UPInt GH_CDECL GHvsprintf(char *dest, UPInt destsize, const char * format, va_list argList)
    {
        UPInt ret;
#if defined(GH_CC_MSVC)
#if defined(GH_MSVC_SAFESTRING)
        dest[0] = '\0';
        int rv = vsnprintf_s(dest, destsize, _TRUNCATE, format, argList);
        if (rv == -1)
        {
            dest[destsize - 1] = '\0';
            ret = destsize - 1;
        }
        else
            ret = (UPInt)rv;
#else
        GH_UNUSED(destsize);
        int rv = _vsnprintf(dest, destsize - 1, format, argList);
        GH_ASSERT(rv != -1);
        ret = (UPInt)rv;
        dest[destsize-1] = 0;
#endif
#else
        GH_UNUSED(destsize);
        ret = (UPInt)vsprintf(dest, format, argList);
        GH_ASSERT(ret < destsize);
#endif
        return ret;
    }
    
    wchar_t* GH_CDECL GHwcscpy(wchar_t* dest, UPInt destsize, const wchar_t* src);
    wchar_t* GH_CDECL GHwcsncpy(wchar_t* dest, UPInt destsize, const wchar_t* src, UPInt count);
    wchar_t* GH_CDECL GHwcscat(wchar_t* dest, UPInt destsize, const wchar_t* src);
    UPInt    GH_CDECL GHwcslen(const wchar_t* str);
    int      GH_CDECL GHwcscmp(const wchar_t* a, const wchar_t* b);
    int      GH_CDECL GHwcsicmp(const wchar_t* a, const wchar_t* b);
    
    inline int GH_CDECL GHwcsicoll(const wchar_t* a, const wchar_t* b)
    {
#if defined(GH_OS_WIN32) || defined(GH_OS_XBOX) || defined(GH_OS_XBOX360) || defined(GH_OS_WII)
#if defined(GH_CC_MSVC) && (GH_CC_MSVC >= 1400)
        return ::_wcsicoll(a, b);
#else
        return ::wcsicoll(a, b);
#endif
#else
        // not supported, use regular wcsicmp
        return GHwcsicmp(a, b);
#endif
    }
    
    inline int GH_CDECL GHwcscoll(const wchar_t* a, const wchar_t* b)
    {
#if defined(GH_OS_WIN32) || defined(GH_OS_XBOX) || defined(GH_OS_XBOX360) || defined(GH_OS_PS3) || defined(GH_OS_WII) || defined(GH_OS_LINUX)
        return wcscoll(a, b);
#else
        // not supported, use regular wcscmp
        return GHwcscmp(a, b);
#endif
    }
    
#ifndef GH_NO_WCTYPE
    
    inline int GH_CDECL UnicodeCharIs(const UInt16* table, wchar_t charCode)
    {
        unsigned offset = table[charCode >> 8];
        if (offset == 0) return 0;
        if (offset == 1) return 1;
        return (table[offset + ((charCode >> 4) & 15)] & (1 << (charCode & 15))) != 0;
    }
    
    extern const UInt16 UnicodeAlnumBits[];
    extern const UInt16 UnicodeAlphaBits[];
    extern const UInt16 UnicodeDigitBits[];
    extern const UInt16 UnicodeSpaceBits[];
    extern const UInt16 UnicodeXDigitBits[];
    
    // Uncomment if necessary
    //extern const UInt16 UnicodeCntrlBits[];
    //extern const UInt16 UnicodeGraphBits[];
    //extern const UInt16 UnicodeLowerBits[];
    //extern const UInt16 UnicodePrintBits[];
    //extern const UInt16 UnicodePunctBits[];
    //extern const UInt16 UnicodeUpperBits[];
    
    inline int GH_CDECL GHiswalnum (wchar_t charCode) { return UnicodeCharIs(UnicodeAlnumBits,  charCode); }
    inline int GH_CDECL GHiswalpha (wchar_t charCode) { return UnicodeCharIs(UnicodeAlphaBits,  charCode); }
    inline int GH_CDECL GHiswdigit (wchar_t charCode) { return UnicodeCharIs(UnicodeDigitBits,  charCode); }
    inline int GH_CDECL GHiswspace (wchar_t charCode) { return UnicodeCharIs(UnicodeSpaceBits,  charCode); }
    inline int GH_CDECL GHiswxdigit(wchar_t charCode) { return UnicodeCharIs(UnicodeXDigitBits, charCode); }
    
    // Uncomment if necessary
    //inline int GH_CDECL GHiswcntrl (wchar_t charCode) { return UnicodeCharIs(UnicodeCntrlBits,  charCode); }
    //inline int GH_CDECL GHiswgraph (wchar_t charCode) { return UnicodeCharIs(UnicodeGraphBits,  charCode); }
    //inline int GH_CDECL GHiswlower (wchar_t charCode) { return UnicodeCharIs(UnicodeLowerBits,  charCode); }
    //inline int GH_CDECL GHiswprint (wchar_t charCode) { return UnicodeCharIs(UnicodePrintBits,  charCode); }
    //inline int GH_CDECL GHiswpunct (wchar_t charCode) { return UnicodeCharIs(UnicodePunctBits,  charCode); }
    //inline int GH_CDECL GHiswupper (wchar_t charCode) { return UnicodeCharIs(UnicodeUpperBits,  charCode); }
    
    int GHtowupper(wchar_t charCode);
    int GHtowlower(wchar_t charCode);
    
#else // GH_NO_WCTYPE
    
    inline int GH_CDECL GHiswspace(wchar_t c)
    {
#if defined(GH_OS_WII) || defined(GH_CC_SNC)
        return ((c) < 128 ? isspace((char)c) : 0);
#else
        return iswspace(c);
#endif
    }
    
    inline int GH_CDECL GHiswdigit(wchar_t c)
    {
#if defined(GH_OS_WII) || defined(GH_CC_SNC)
        return ((c) < 128 ? isdigit((char)c) : 0);
#else
        return iswdigit(c);
#endif
    }
    
    inline int GH_CDECL GHiswxdigit(wchar_t c)
    {
#if defined(GH_OS_WII) || defined(GH_CC_SNC)
        return ((c) < 128 ? isxdigit((char)c) : 0);
#else
        return iswxdigit(c);
#endif
    }
    
    inline int GH_CDECL GHiswalpha(wchar_t c)
    {
#if defined(GH_OS_WII) || defined(GH_CC_SNC)
        return ((c) < 128 ? isalpha((char)c) : 0);
#else
        return iswalpha(c);
#endif
    }
    
    inline int GH_CDECL GHiswalnum(wchar_t c)
    {
#if defined(GH_OS_WII) || defined(GH_CC_SNC)
        return ((c) < 128 ? isalnum((char)c) : 0);
#else
        return iswalnum(c);
#endif
    }
    
    inline wchar_t GH_CDECL GHtowlower(wchar_t c)
    {
#if (defined(GH_OS_PS3) && defined(GH_CC_SNC))
        return (wchar_t)tolower((char)c);
#else
        return (wchar_t)towlower(c);
#endif
    }
    
    inline wchar_t GHtowupper(wchar_t c)
    {
#if (defined(GH_OS_PS3) && defined(GH_CC_SNC))
        return (wchar_t)toupper((char)c);
#else
        return (wchar_t)towupper(c);
#endif
    }
    
#endif // GH_NO_WCTYPE
    
    // ASCII versions of tolower and toupper. Don't use "char"
    inline int GH_CDECL GHtolower(int c)
    {
        return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
    }
    
    inline int GH_CDECL GHtoupper(int c)
    {
        return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
    }
    
    
    
    inline double GH_CDECL GHwcstod(const wchar_t* string, wchar_t** tailptr)
    {
#if defined(GH_OS_OTHER)
        GH_UNUSED(tailptr);
        char buffer[64];
        char* tp = NULL;
        UPInt max = GHwcslen(string);
        if (max > 63) max = 63;
        unsigned char c = 0;
        for (UPInt i=0; i < max; i++)
        {
            c = (unsigned char)string[i];
            buffer[i] = ((c) < 128 ? (char)c : '!');
        }
        buffer[max] = 0;
        return GHstrtod(buffer, &tp);
#else
        return wcstod(string, tailptr);
#endif
    }
    
    inline long GH_CDECL GHwcstol(const wchar_t* string, wchar_t** tailptr, int radix)
    {
#if defined(GH_OS_OTHER)
        GH_UNUSED(tailptr);
        char buffer[64];
        char* tp = NULL;
        UPInt max = GHwcslen(string);
        if (max > 63) max = 63;
        unsigned char c = 0;
        for (UPInt i=0; i < max; i++)
        {
            c = (unsigned char)string[i];
            buffer[i] = ((c) < 128 ? (char)c : '!');
        }
        buffer[max] = 0;
        return strtol(buffer, &tp, radix);
#else
        return wcstol(string, tailptr, radix);
#endif
    }
    
} // Util

#endif /* Std_h */
