//
//  TickMgr.hpp
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#ifndef TickMgr_hpp
#define TickMgr_hpp

#include "BaseTickMgr.h"
#include <list>

namespace GameHub
{
    class CTickMgr : public ITickMgr
    {
        enum TickEnum
        {
            eTeSIZE_256 = 256,
            eTeSIZE_64 = 64,
            eTeSIZE_6 = 6,
            eTeSIZE_8 = 8,
            eTeMASK_255 = eTeSIZE_256 - 1,
            eTeMASK_63 = eTeSIZE_64 - 1,
            eTeSIZE_5 = 5
        };
        
        struct TickContext
        {
            UInt64						m_uInterval;
            bool						m_bTerminate;
            UInt64						m_uNextTickTime;
            CTick*						m_pTick;
        };
        
        typedef std::list<TickContext*>		CTickContextSlot;
        
        struct SlotCd
        {
            SlotCd():m_nIndex(0) {};
            uint32						m_nIndex;
            CTickContextSlot		m_oSlots[eTeSIZE_64];
        };
        
        struct SlotRoot
        {
            SlotRoot():m_nIndex(0) {};
            uint32						m_nIndex;
            CTickContextSlot		m_oSlots[eTeSIZE_256];
        };
        
    public:
        CTickMgr();
        
        void RegisterTick(uint32 uInterval, CTick* pTick);
        void UnRegisterTick(CTick* pTick);
        void ReRegisterTick(CTick* pTick);
        
        uint32 PushTickOnce(uint32 nInterval);
        
    private:
        void RegisterTick(TickContext* pContext);
        
        void GetTickPos(uint32 nNextTickTime, uint32 nTimeLeft, uint32 &nSlot, uint32 &nIndex);
        void RegisterTickAgain(TickContext* pTick);
        void MoveRegistedTick(TickContext * pTick);
        void CascadeTimers(SlotCd& slotcd);
        
        
        uint32			m_nTotalCount;
        SlotCd*			m_SlotsPointer[eTeSIZE_5];
        
        SlotRoot		m_oSlots1;
        SlotCd			m_oSlots2;
        SlotCd			m_oSlots3;
        SlotCd			m_oSlots4;
        SlotCd			m_oSlots5;
        
        uint32			m_nTotalTickSize;
    };
    
}

#endif /* TickMgr_hpp */
