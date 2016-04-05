#pragma once

#include "CNetAddr.h"
#include "INetWork.h"
#include "CAcceptorEx.h"
#include "CConnectorEx.h"
//#include "Object/TUniqueObjMgr.inl"
#include "TNetWork.h"

namespace PPE
{
	template<typename AcceptorExCallBack>
	TNetWork<AcceptorExCallBack>* TNetWork<AcceptorExCallBack>::m_pNetWork = NULL;
	
	template<typename AcceptorExCallBack>
	TNetWork<AcceptorExCallBack>& TNetWork<AcceptorExCallBack>::GetSingleton()
	{
		if(NULL == m_pNetWork)
		{
			m_pNetWork = PPE_NEW_T(TNetWork<AcceptorExCallBack>, MEMCATEGORY_GENERAL, "")();
		}
		return * m_pNetWork;
	}

	template<typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::ReleaseSingleton()
	{
		SAFE_RELEASE(m_pNetWork);
	}

	template< typename AcceptorExCallBack>
	TNetWork<AcceptorExCallBack>::TNetWork(void)
		:m_uAcceptorExCount(0)
		,m_uConnectorExCount(0)
		,m_pListener(NULL)
		,m_pEventReactor(NULL)
		,m_uGcTime(0)
        ,m_uCheckPingTickTime(0)

	{
		BegineGc(1000 * 5); // 10秒GC一次
		m_pEventReactor = GetEventReactor();
	}

	template< typename AcceptorExCallBack>
	TNetWork<AcceptorExCallBack>::~TNetWork(void)
	{

	}

	template< typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::EnumAcceptorExInShutDown(void* Obj, size_t stId, void* pContext)
	{
		static_cast<CAcceptorEx*>(Obj)->ShutDown();
	}

	template< typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::Release(void)
	{
		// 停止GC
		StopGc();
		// listener停止服务
		SAFE_RELEASE(m_pListener);

		// 所有的connector停止服务
		Map_ConnectorExs::iterator ConnectoriterBegin = m_mapConnectorExs.begin();
		Map_ConnectorExs::iterator ConnectoriterEnd = m_mapConnectorExs.end();
		for(;ConnectoriterBegin!=ConnectoriterEnd;++ConnectoriterBegin)
		{
			CConnectorEx* pConnectorEx = ConnectoriterBegin->second;
			pConnectorEx->ShutDown();
			SAFE_RELEASE(pConnectorEx);
		}
		m_mapConnectorExs.clear();

		// 所有的Acceptro停止服务
		//Map_AcceptorExs::iterator AcceptoriterBegin = m_mapAcceptorExs.begin();
		//Map_AcceptorExs::iterator AcceptoriterEnd = m_mapAcceptorExs.end();
		//for(;AcceptoriterBegin!=AcceptoriterEnd;++AcceptoriterBegin)
		//{
		//	CAcceptorEx* pAcceptorEx = AcceptoriterBegin->second;
		//	pAcceptorEx->ShutDown();
		//	SAFE_RELEASE(pAcceptorEx);
		//}
		//m_mapAcceptorExs.clear();

		m_mapAcceptorExs.EnumEachObj(this->EnumAcceptorExInShutDown, NULL);
		m_mapAcceptorExs.DeleteAllObj();

		// 所有的signal停止服务
		while(!m_quSystemSignals.empty())
		{
			ISystemSignal* pSystemSignal = m_quSystemSignals.front();
			SAFE_RELEASE(pSystemSignal);
			m_quSystemSignals.pop();
		}

		// 释放所有未gc的connector, acceptor
		OnTick();

		// 干掉EventReactor
		SAFE_RELEASE(m_pEventReactor);
		PPE_DELETE_T( this, TNetWork<AcceptorExCallBack>, MEMCATEGORY_GENERAL);
	}

	template< typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::BegineGc(uint32 uGcTime)
	{
		if(uGcTime==m_uGcTime)
			return;

		if(m_uGcTime>0)
		{
			GetTickMgr()->UnRegisterTick(this);
		}
		m_uGcTime =  uGcTime;
		GetTickMgr()->RegisterTick(uGcTime, this);
	}

	template< typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::StopGc()
	{
		if(0==m_uGcTime)
			return;
		
		GetTickMgr()->UnRegisterTick(this);
		m_uGcTime =  0;
	}

	template< typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::OnAccept(IEventReactor* pReactor, SOCKET Socket)
	{
		TNetWork<AcceptorExCallBack>::GetSingleton().NewAcceptor(pReactor,Socket);
	}

	template<typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::NewAcceptor(IEventReactor* pReactor, SOCKET Socket)
	{
		uint32 uId = GetAcceptorId();
		IAcceptor* pAcceptor = GetAcceptor(Socket, pReactor);
		CAcceptorEx* pAcceptorEx  = PPE_NEW_T(CAcceptorEx, MEMCATEGORY_GENERAL, "")( uId, pAcceptor);

		pAcceptorEx->SetCallbackFunc(&AcceptorExCallBack::AcceptorExOnDisconnected,
														&AcceptorExCallBack::AcceptorExOnSomeDataSend, 
														&AcceptorExCallBack::AcceptorExOnSomeDataRecv,
														m_uCheckPingTickTime);

		//m_mapAcceptorExs.insert(std::make_pair(uId, pAcceptorEx));
		m_mapAcceptorExs.AddNewObj(uId, pAcceptorEx);
		AcceptorExCallBack::CreateNewAcceptorEx(uId, pAcceptorEx);
	}

	template<typename AcceptorExCallBack>
	void TNetWork<AcceptorExCallBack>::SetCallBackSignal(uint32 uSignal, OnSignal pFunc, void* pContext, bool bLoop)
	{
		ISystemSignal* pSystemSignal = GetSystemSignal(m_pEventReactor);
		pSystemSignal->SetCallBackSignal(uSignal,pFunc, pContext, bLoop);
		m_quSystemSignals.push(pSystemSignal);
	}

	template< typename AcceptorExCallBack>
	bool TNetWork<AcceptorExCallBack>::BeginListen(const char* szNetAddr, uint16 uPort, uint32 uCheckPingTickTime)
	{
		PPE_ASSERT(NULL==m_pListener);
		m_pListener = GetListener(m_pEventReactor);
		CNetAddr addr(szNetAddr, uPort);
		bool bRes=m_pListener->Listen(addr, &TNetWork<AcceptorExCallBack>::OnAccept);
		PPE_ASSERT(true==bRes);
        m_uCheckPingTickTime = uCheckPingTickTime;
		return true;
	}

	template<typename AcceptorExCallBack>
	void TNetWork< AcceptorExCallBack>::EndListen()
	{
		PPE_ASSERT(NULL!=m_pListener);
		SAFE_RELEASE(m_pListener);
	}

	template<typename AcceptorExCallBack>
	uint32 TNetWork<AcceptorExCallBack>::GetConnectorId()
	{
		return ++m_uConnectorExCount;
	}

	template<typename AcceptorExCallBack>
	uint32 TNetWork< AcceptorExCallBack>::GetAcceptorId()
	{
		return ++m_uAcceptorExCount;
	}

	template<typename AcceptorExCallBack>
	uint32 TNetWork<AcceptorExCallBack>::Connect(const char* szNetAddr, uint16 uPort, 
																						ConnectorExFuncOnDisconnected pOnDisconnected, 
																						ConnectorExFuncOnConnectFailed pOnConnectFailed, 
																						ConnectorExFuncOnConnectted pOnConnectted, 
																						ConnectorExFuncOnSomeDataSend pOnSomeDataSend, 
																						ConnectorExFuncOnSomeDataRecv pOnSomeDataRecv,
																						ConnectorExFuncOnPingServer  pOnPingServer,
																						uint32 uPingTick /* = 45000 */, uint32 uTimeOut)
	{
		uint32 uId = GetConnectorId();
		
		IConnector* pConnector = GetConnector(m_pEventReactor);
		CConnectorEx* pConnectorEx = PPE_NEW_T(CConnectorEx, MEMCATEGORY_GENERAL, "")( uId, pConnector);
		
		pConnectorEx->SetCallbackFunc( pOnDisconnected,  pOnConnectFailed,  
																pOnConnectted,  pOnSomeDataSend,  
																pOnSomeDataRecv,pOnPingServer,
																uPingTick);

		CNetAddr addr(szNetAddr, uPort);
		timeval time;
		time.tv_sec = uTimeOut;
		time.tv_usec = 0;
		pConnector->Connect(addr, &time);
		m_mapConnectorExs.insert(std::make_pair(uId, pConnectorEx));
		return uId;
	}

	template< typename AcceptorExCallBack>
	uint32 TNetWork< AcceptorExCallBack>::GetConnectorExPingValue(uint32 uId)
	{
		CConnectorEx* pEx = FindConnectorEx(uId);
		if(pEx)
		{
			return pEx->GetPingValue();
		}else
		{
			return 9999999; // 表示无限大的延迟
		}
	}

	template< typename AcceptorExCallBack>
	void TNetWork< AcceptorExCallBack>::DispatchEvents()
	{
		m_pEventReactor->DispatchEvents();
	}

	template< typename AcceptorExCallBack>
	bool TNetWork< AcceptorExCallBack>::ShutDownConnectorEx(uint32 uId)
	{
		Map_ConnectorExs::iterator iter = m_mapConnectorExs.find(uId);

        if (m_mapConnectorExs.end()!=iter)
        {
            CConnectorEx* pConnectorEx = iter->second;

            pConnectorEx->ShutDown();

            m_quIdleConnectorExs.push(pConnectorEx);

            m_mapConnectorExs.erase(iter);
            return true;
        }else
        {
            return false;
        }
	}

	template< typename AcceptorExCallBack>
	bool TNetWork< AcceptorExCallBack>::ShutDownAcceptorEx(uint32 uId)
	{
		//Map_AcceptorExs::iterator iter = m_mapAcceptorExs.find(uId);
		//PPE_ASSERT(m_mapAcceptorExs.end()!=iter);
		//CAcceptorEx* pAcceptorEx = iter->second;

		CAcceptorEx* pAcceptorEx = m_mapAcceptorExs.GetObj(uId);

        if (pAcceptorEx)
        {
            pAcceptorEx->ShutDown();

            m_quIdleAcceptorExs.push(pAcceptorEx);
            //m_mapAcceptorExs.erase(iter);
            m_mapAcceptorExs.DeleteObj(uId, false);
            return true;
        }else
        {
            //PPE_ASSERT(false);
            return false;
        }
	}

	template< typename AcceptorExCallBack>
	void TNetWork< AcceptorExCallBack>::OnTick()
	{
		if(!m_quIdleAcceptorExs.empty())
		{
			while(!m_quIdleAcceptorExs.empty())
			{
				CAcceptorEx* pAcceptorEx = m_quIdleAcceptorExs.front();
				pAcceptorEx->Release();
				m_quIdleAcceptorExs.pop();
			}
		}

		if(!m_quIdleConnectorExs.empty())
		{
			while(!m_quIdleConnectorExs.empty())
			{
				CConnectorEx* pConnectorEx = m_quIdleConnectorExs.front();
				pConnectorEx->Release();
				m_quIdleConnectorExs.pop();
			}
		}
	}

	template< typename AcceptorExCallBack>
	void TNetWork< AcceptorExCallBack>::EnumAcceptorEx(void* Obj, size_t stId, void* pContext)
	{
		EnumContex* pContex = static_cast<EnumContex*>(pContext);
		pContex->pFunc(static_cast<CAcceptorEx*>(Obj), pContex->pContext);
	}

	template< typename AcceptorExCallBack>
	void TNetWork< AcceptorExCallBack>::EnumAcceptorEx(EnumAcceptorExCallbackFunc pFunc, void* pContext)
	{

		EnumContex contex;
		contex.pContext = pContext;
		contex.pFunc = pFunc;
		
		m_mapAcceptorExs.EnumEachObj( this->EnumAcceptorEx , &contex);
		//Map_AcceptorExs::iterator iterBegin = m_mapAcceptorExs.begin();
		//Map_AcceptorExs::iterator iterEnd = m_mapAcceptorExs.end();
		//for( ; iterBegin != iterEnd; )
		//{
		//	Map_AcceptorExs::iterator iterTemp = iterBegin;
		//	++iterBegin;
		//	CAcceptorEx* pAcceptorEx = iterTemp->second;
		//	if( pFunc(pAcceptorEx, pContext) )
		//	{
		//		break;
		//	}
		//}
	}

	template< typename AcceptorExCallBack>
	CAcceptorEx*	TNetWork< AcceptorExCallBack>::FindAcceptorEx(uint32 uId)
	{
		//Map_AcceptorExs::iterator iter = m_mapAcceptorExs.find(uId);
		//if(m_mapAcceptorExs.end()==iter)
		//{
		//	return NULL;
		//}else
		//{
		//	return iter->second;
		//}
		if(uId == 0)
			return NULL;

		return m_mapAcceptorExs.GetObj(uId);
	}

	template< typename AcceptorExCallBack>
	CConnectorEx*	TNetWork< AcceptorExCallBack>::FindConnectorEx(uint32 uId)
	{
		Map_ConnectorExs::iterator iter = m_mapConnectorExs.find(uId);
		if(m_mapConnectorExs.end()==iter)
		{
			return NULL;
		}else
		{
			return iter->second;
		}
	}

	template< typename AcceptorExCallBack>
	PipeResult TNetWork< AcceptorExCallBack>::ConnectorExSendData(uint32 uId, const void* pData, uint32 uSize)
	{
		CConnectorEx* pConnectorEx = FindConnectorEx(uId);
		if(pConnectorEx)
		{
			PipeResult eRes = pConnectorEx->Send(pData, uSize);
			PPE_ASSERT( ePR_OK == eRes );
			return  ePR_OK;
		}
		return ePR_Disconnected;
	}

	template< typename AcceptorExCallBack>
	PipeResult TNetWork< AcceptorExCallBack>::AcceptorExSendData(uint32 uId, const void* pData, uint32 uSize)
	{
		CAcceptorEx* pAcceptorEx = FindAcceptorEx(uId);
		if(pAcceptorEx)
		{
			PipeResult eRes =  pAcceptorEx->Send(pData, uSize); 
			return  ePR_OK;
		}
		return ePR_Disconnected;
	}

	template< typename AcceptorExCallBack>
	PipeResult TNetWork< AcceptorExCallBack>::AcceptorExSendData(uint32 uId, const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2)
	{
		CAcceptorEx* pAcceptorEx = FindAcceptorEx(uId);
		if(pAcceptorEx)
		{
			PipeResult eRes =  pAcceptorEx->Send(pData1,  uSize1, pData2,  uSize2); 
			return  ePR_OK;
		}
		return ePR_Disconnected;
	}

	template< typename AcceptorExCallBack>
	PipeResult TNetWork< AcceptorExCallBack>::AcceptorExSendData(uint32 uId, const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2, const void* pData3, uint32 uSize3)
	{
		CAcceptorEx* pAcceptorEx = FindAcceptorEx(uId);
		if(pAcceptorEx)
		{
			PipeResult eRes =  pAcceptorEx->Send(pData1,  uSize1, pData2,  uSize2,  pData3,  uSize3); 
			return  ePR_OK;
		}
		return ePR_Disconnected;
	}
}

