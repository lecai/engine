//
//  BaseTickMgr.h
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#ifndef BaseTickMgr_h
#define BaseTickMgr_h
#include "Types.h"

namespace GameHub
{
    class GH_EXPORT CTick
    {
        friend class CTickMgr;
    public:
        CTick();
        virtual ~CTick();
        
        virtual void OnTick();
        void UnRegister();
        bool IsRegistered();
        virtual const char* GetTickName();
    protected:
        void*			m_pTickContext;
    };
    
    class ITickMgr
    {
    public:
        virtual void RegisterTick(uint32 uInterval, CTick* pTick)=0;
        virtual void UnRegisterTick(CTick* pTick)=0;
        virtual uint32 PushTickOnce(uint32 nInterval)=0;
        virtual void ReRegisterTick(CTick* pTick)=0;
    };
    
    extern "C" GH_EXPORT ITickMgr* GetTickMgr();
}

#endif /* BaseTickMgr_h */
