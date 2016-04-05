//
//  FileStream.cpp
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#include "FileStream.hpp"
#include <assert.h>

using namespace GameHub;

FileStream::FileOpenMap FileStream::m_sFileOpenMap = FileStream::FileOpenMap();
string FileStream::GetBinaryPath()
{
    char path[256] = "";
    FILE* file = fopen("/proc/self/cmdline", "r");
    fgets(path, 256, file);
    fclose(file);
    return path;
}
FileStream::~FileStream()
{
    Close();
}
bool FileStream::Open(const char *filename)
{
    m_pHandle = fopen(filename, "r");
    if (m_pHandle)
    {
        m_Filename = filename;
        m_sFileOpenMap[m_Filename] = m_pHandle;
    }
    return m_pHandle != NULL;
}
void FileStream::Close()
{
    if(m_pHandle)
    {
        fclose(m_pHandle);
        FileOpenMap::iterator itr = m_sFileOpenMap.find(m_Filename);
        m_sFileOpenMap.erase(itr);
    }
    m_pHandle = NULL;
}
unsigned int FileStream::GetFileSize()
{
    if (m_pHandle)
    {
        fseek(m_pHandle,0,SEEK_END);
        unsigned long size = ftell(m_pHandle);
        fseek(m_pHandle, 0,SEEK_SET);
        return size;
    }
    else
        return 0;
}

bool FileStream::GetBuffer(char *bufferInout)
{
    if (m_pHandle)
    {
        unsigned int filesize = GetFileSize();
        unsigned long readsize = fread(bufferInout, 1, filesize,m_pHandle);
        return readsize = filesize;
    }
    return false;
}