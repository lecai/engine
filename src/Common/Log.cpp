//
//  Log.cpp
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#include "Log.hpp"

namespace GameHub {
    
    void FormatLog(LogMessageType type, char* buffer, unsigned bufferSize,
                   const char* fmt, va_list argList)
    {
        // Generate a format string.
        switch(type)
        {
            case LogMessage_Error:     GHstrcpy(buffer, bufferSize, "Error: ");     break;
            case LogMessage_Warning:   GHstrcpy(buffer, bufferSize, "Warning: ");   break;
            case LogMessage_Assert:    GHstrcpy(buffer, bufferSize, "Assert: ");    break;
            case LogMessage_Text:      buffer[0] = 0;
        }
        
        UPInt prefixLength = GHstrlen(buffer);
        char *buffer2      = buffer + prefixLength;
        GHvsprintf(buffer2, bufferSize - prefixLength, fmt, argList);
        
        
        GHstrcat(buffer, bufferSize, "\n");
    }
    
    void LogMessage(LogMessageType type, const char* fmt, ...)
    {
        char buffer[MaxLogBufferMessageSize];
        va_list argList;
        
        va_start(argList, fmt);
        
        FormatLog(type, buffer, MaxLogBufferMessageSize, fmt, argList);
        
#if defined(GH_OS_WIN32) || defined(GH_OS_XBOX) || defined(GH_OS_XBOX360)
        ::OutputDebugStringA(buffer);
#elif defined(GH_OS_WII)
        OSReport(buffer);
#elif defined(GH_OS_3DS)
        nn::dbg::detail::PutString(buffer);
#elif defined(GH_OS_ANDROID)
        __android_log_write(ANDROID_LOG_INFO,"GameHub",buffer);
#else
        fputs(buffer, stdout);
#endif
        va_end(argList);
        
    }
    
}