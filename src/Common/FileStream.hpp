//
//  FileStream.hpp
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#ifndef FileStream_hpp
#define FileStream_hpp

#include "Types.h"
#include <map>
#include <string>
#ifdef __linux__
#include <stdio.h>
#endif
using namespace std;
namespace GameHub
{
    
    class FileStream
    {
    public:
        static string GetBinaryPath();
        ~FileStream();
        bool Open(const char* filename);
        void Close();
        unsigned int GetFileSize();
        bool GetBuffer(char* bufferInout);
    private:
        typedef map<string, FILE*>  FileOpenMap;
        static FileOpenMap         m_sFileOpenMap;
        FILE*           m_pHandle;
        string          m_Filename;
    };
    
    
}

#endif /* FileStream_hpp */
