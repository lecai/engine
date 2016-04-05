//
//  Log.hpp
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#ifndef Log_hpp
#define Log_hpp

#include "Std.h"

namespace GameHub {
    
    
    enum { MaxLogBufferMessageSize = 2048 };
    
    enum LogMessageType
    {
        LogMessage_Text    = 0x00000, // No prefix, no newline.
        LogMessage_Warning = 0x20000, // "Warning: %s\n". For unexpected conditions handled gracefully.
        LogMessage_Error   = 0x30000, // "Error: %s\n". For runtime errors resulting in API fail (but not crash).
        LogMessage_Assert  = 0x40000, // "Assert: %s\n". Used for condition leading to crash; debug only.
    };
    
    void FormatLog(LogMessageType type, char* buffer, unsigned bufferSize,
                   const char* fmt, va_list argList);
    
    void LogMessage(LogMessageType type, const char* fmt, ...);
}

#ifndef GM_BUILD_DEBUG
#define GH_DEBUG_ERROR(cond, ...)                                   ((void)0)
#define GH_DEBUG_WARN(cond, ...)                                    ((void)0)
#define GH_DEBUG_ASSERT(cond, ...)                                  ((void)0)
#else
#define GH_DEBUG_Error(cond, ...)                                   do { if (!(cond)) { GameHub::LogMessage(GameHub::LogMessage_Error, __VA_ARGS__); } } while (0)
#define GH_DEBUG_WARN(cond, ...)                                    do { if (!(cond)) { GameHub::LogMessage(GameHub::LogMessage_Warning, __VA_ARGS__); } } while (0)
#define GH_DEBUG_ASSERT(cond, ...)                                  do { if (!(cond)) { GameHub::LogMessage(GameHub::LogMessage_Assert, __VA_ARGS__); GH_DEBUG_BREAK; } } while (0)
#endif

#define GH_INFO(...)                                                do { GameHub::LogMessage(GameHub::LogMessage_Text, __VA_ARGS__); } while (0)

#endif /* Log_hpp */
