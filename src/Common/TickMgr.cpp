//
//  TickMgr.cpp
//  Common
//
//  Created by lecai on 16/4/5.
//  Copyright © 2016年 lecai. All rights reserved.
//

#include "TickMgr.hpp"
#include <list>

namespace GameHub
{
    CTick::CTick():m_pTickContext(NULL)
    {
    }
    
    CTick::~CTick()
    {
        if(m_pTickContext)
        {
            UnRegister();
        }
    }
    
    void CTick::UnRegister()
    {
        GetTickMgr()->UnRegisterTick(this);
    }
    
    void CTick::OnTick()
    {
        
    }
    
    const char* CTick::GetTickName()
    {
        return "";
    }
    
    bool CTick::IsRegistered()
    {
        return NULL != m_pTickContext;
    }
    
    
    ///--------------------------------
    CTickMgr::CTickMgr():m_nTotalCount(0), m_nTotalTickSize(0)
    {
        m_SlotsPointer[0] = reinterpret_cast<SlotCd*>(&m_oSlots1);
        m_SlotsPointer[1] = &m_oSlots2;
        m_SlotsPointer[2] = &m_oSlots3;
        m_SlotsPointer[3] = &m_oSlots4;
        m_SlotsPointer[4] = &m_oSlots5;
    }
    
    void CTickMgr::GetTickPos(uint32 nNextTickTime, uint32 nTimeLeft, uint32 &nSlot, uint32 &nIndex)
    {
        if(nTimeLeft <  eTeSIZE_256)
        {
            nSlot = 0;
            nIndex = nNextTickTime & eTeMASK_255;
        }
        else if(nTimeLeft < 1 << (eTeSIZE_8+eTeSIZE_6))
        {
            nSlot = 1;
            nIndex = nNextTickTime >> eTeSIZE_8 & eTeMASK_63;
        }
        else if(nTimeLeft < 1 <<(eTeSIZE_8+eTeSIZE_6*2))
        {
            nSlot = 2;
            nIndex = (nNextTickTime >> (eTeSIZE_8 + eTeSIZE_6 )) & eTeMASK_63;
        }
        else if(nTimeLeft < 1 <<(eTeSIZE_8+eTeSIZE_6*3))
        {
            nSlot = 3;
            nIndex = (nNextTickTime >> (eTeSIZE_8 + eTeSIZE_6*2 )) & eTeMASK_63;
        }
        else if(nTimeLeft <  0xFFFFFFFF)
        {
            nSlot = 4;
            nIndex = (nNextTickTime >> (eTeSIZE_8 + eTeSIZE_6*3 )) & eTeMASK_63;
        }
    }
    
    void CTickMgr::RegisterTick(uint32 uInterval, CTick* pTick)
    {
        if(false != pTick->IsRegistered())
            return;
        
        ++m_nTotalTickSize;
        
        uint32 nIndex=0;
        uint32 nSlot=0;
        
        TickContext* pContext = GH_NEW_T(TickContext, MEMCATEGORY_GENERAL, "TickContext");
        pContext->m_uInterval = uInterval;
        pContext->m_bTerminate = false;
        pContext->m_uNextTickTime = m_nTotalCount + uInterval;
        pContext->m_pTick = pTick;
        pTick->m_pTickContext = pContext;
        
        GetTickPos(pContext->m_uNextTickTime, uInterval, nSlot, nIndex);
        m_SlotsPointer[nSlot]->m_oSlots[nIndex].push_back(pContext);
        //printf("Re1 nNextTime =%d, nInterval = %d, nSlot = %d, nIndex = %d, nextcalltick = %u \n", nNextTime, pTick->GetInterval(), nSlot, nIndex, pTick->GetNextCallTick());
    }
    
    void CTickMgr::RegisterTickAgain(TickContext* pContext)
    {
        uint32 nIndex=0;
        uint32 nSlot=0;
        pContext->m_uNextTickTime  += pContext->m_uInterval;
        
        GetTickPos(pContext->m_uNextTickTime, pContext->m_uInterval, nSlot, nIndex);
        m_SlotsPointer[nSlot]->m_oSlots[nIndex].push_back(pContext);
    }
    
    void CTickMgr::ReRegisterTick(CTick* pTick)
    {
        if(pTick->IsRegistered())
        {
            TickContext* pContext = static_cast<TickContext*>(pTick->m_pTickContext);
            pContext->m_bTerminate = true;
            pTick->m_pTickContext = NULL;
            
            RegisterTick(pContext->m_uInterval, pTick);
        }
    }
    
    void CTickMgr::UnRegisterTick(CTick* pTick)
    {
        if(pTick->IsRegistered())
        {
            TickContext* pContext = static_cast<TickContext*>(pTick->m_pTickContext);
            pContext->m_bTerminate = true;
            pTick->m_pTickContext = NULL;
        }
    }
    
    void CTickMgr::MoveRegistedTick(TickContext * pTick)
    {
        uint32 nIndex=0;
        uint32 nSlot=0;
        uint32 nNextTickTime = pTick->m_uNextTickTime;
        uint32 nInterval = nNextTickTime < m_nTotalCount ? 0 : nNextTickTime - m_nTotalCount;
        
        GetTickPos(nNextTickTime, nInterval, nSlot, nIndex);
        m_SlotsPointer[nSlot]->m_oSlots[nIndex].push_back(pTick);
    }
    
    void CTickMgr::CascadeTimers(SlotCd& slotcd)
    {
        CTickContextSlot& slot = slotcd.m_oSlots[slotcd.m_nIndex];
        while(!slot.empty())
        {
            TickContext* pTickContext = slot.front();
            slot.pop_front();
            MoveRegistedTick(pTickContext);
        }
        slotcd.m_nIndex = (slotcd.m_nIndex + 1)&eTeMASK_63;
    }
    
    
    uint32 CTickMgr::PushTickOnce(uint32 nInterval)
    {
        uint32 uTotoalTime = 0;
        for(uint32 nIndex = 0; nIndex < nInterval; ++nIndex)
        {
            if(0==m_oSlots1.m_nIndex)
            {
                int n = 1;
                do {
                    CascadeTimers(*m_SlotsPointer[n]);
                } while (m_SlotsPointer[n]->m_nIndex == 1 && ++n < eTeSIZE_5);
            }
            
            CTickContextSlot& slot = m_oSlots1.m_oSlots[m_oSlots1.m_nIndex];
            
            while(!slot.empty())
            {
                TickContext * pTickContext = slot.front();
                if( pTickContext->m_bTerminate )
                {
                    GH_DELETE_T( pTickContext, TickContext, MEMCATEGORY_GENERAL);
                    pTickContext = NULL;
                    --m_nTotalTickSize;
                }
                else
                {
                    
                    CTick* pTick = pTickContext->m_pTick;
                    pTick->OnTick();
                    if(pTickContext->m_bTerminate)
                    {
                        GH_DELETE_T( pTickContext, TickContext, MEMCATEGORY_GENERAL);
                        pTickContext = NULL;
                        --m_nTotalTickSize;
                    }
                    else
                    {
                        RegisterTickAgain(pTickContext);
                    }
                }
                slot.pop_front();
            }
            
            ++m_nTotalCount;
            m_oSlots1.m_nIndex = (m_oSlots1.m_nIndex +1)&eTeMASK_255;
        }
        return uTotoalTime;
    }
    
    ITickMgr* GetTickMgr()
    {
        static CTickMgr mgr;
        return &mgr;
    }
}