#pragma once
#include "INetWork.h"

namespace PPE
{
    using namespace std;
    
	INTER_FACE IListener;
	INTER_FACE IConnector;
	INTER_FACE IAcceptor;
	

	class CAcceptorEx;
	class CConnectorEx;


	typedef void(*AcceptorExFuncOnDisconnected)(CAcceptorEx* pAcceptorEx);
	typedef void(*AcceptorExFuncOnSomeDataSend)(CAcceptorEx* pAcceptorEx);
	typedef void(*AcceptorExFuncOnSomeDataRecv)(CAcceptorEx* pAcceptorEx);
	typedef void(*AcceptorExFuncOnClientLost)(CAcceptorEx* pAcceptorEx);


	typedef void(*ConnectorExFuncOnDisconnected)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnConnectFailed)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnConnectted)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnSomeDataSend)(CConnectorEx* pConnectorEx);
	typedef void(*ConnectorExFuncOnSomeDataRecv)(CConnectorEx* pConnectorEx);
	
	typedef void(*ConnectorExFuncOnPingServer)(CConnectorEx* pConnectorEx);

	typedef bool(*EnumAcceptorExCallbackFunc)(CAcceptorEx* pConnectorEx, void* pContext);

	template<typename AcceptorExCallBack>
	class TNetWork : public CTick
	{
	public:
		TNetWork();
		virtual ~TNetWork();


		bool BeginListen(const char* szNetAddr, uint16 uPort, uint32 uCheckPingTickTime = 0);
		void EndListen();

		// 把connector的回调函数的设置放到这里，是因为链接不同的网络地址，使用的协议完全不一样
		// 而acceptor则不存在这个问题
		uint32 Connect(const char* szNetAddr, uint16 uPort,	ConnectorExFuncOnDisconnected		pOnDisconnected,
																							ConnectorExFuncOnConnectFailed		pOnConnectFailed,
																							ConnectorExFuncOnConnectted			pOnConnectted,
																							ConnectorExFuncOnSomeDataSend		pOnSomeDataSend,
																							ConnectorExFuncOnSomeDataRecv		pOnSomeDataRecv,
																							ConnectorExFuncOnPingServer			pOnPingServer,
																							uint32 uPingTick = 4500, uint32 uTimeOut = 30); // 返回的connector的id


		void	SetCallBackSignal(uint32 uSignal, OnSignal pFunc, void* pContext, bool bLoop = false);


		PipeResult ConnectorExSendData(uint32 uId, const void* pData, uint32 uSize);
		PipeResult AcceptorExSendData(uint32 uId, const void* pData, uint32 uSize);
		PipeResult AcceptorExSendData(uint32 uId, const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2);
		PipeResult AcceptorExSendData(uint32 uId, const void* pData1, uint32 uSize1, const void* pData2, uint32 uSize2, const void* pData3, uint32 uSize3);
				
		bool ShutDownConnectorEx(uint32 uId);
		bool ShutDownAcceptorEx(uint32 uId);
		void DispatchEvents();

		void BegineGc(uint32 uGcTime);
		void StopGc();

		void AcceptorNeedPing(bool bNeed);

		static TNetWork<AcceptorExCallBack>& GetSingleton();
		static void ReleaseSingleton();

		void EnumAcceptorEx(EnumAcceptorExCallbackFunc pFunc, void* pContext);

		uint32 GetConnectorExPingValue(uint32 uId);

		CAcceptorEx*	FindAcceptorEx(uint32 uId);
		CConnectorEx*	FindConnectorEx(uint32 uId);
	
	private:

		struct EnumContex
		{
			EnumAcceptorExCallbackFunc pFunc;
			void* pContext;
		};

		void OnTick();
		const char* GetTickName() { return "TNetWork Tick"; };

		// listenor
		static void OnAccept(IEventReactor* pReactor, SOCKET Socket);

		uint32 GetAcceptorId();
		uint32 GetConnectorId();

		void NewAcceptor(IEventReactor* pReactor, SOCKET Socket);


		static void EnumAcceptorExInShutDown(void* Obj, size_t stId, void* pContext);
		static void EnumAcceptorEx(void* Obj, size_t stId, void* pContext);


		void Release();


	private:
		uint32											m_uGcTime;
		uint32											m_uAcceptorExCount;
		uint32											m_uConnectorExCount;
        uint32                                          m_uCheckPingTickTime;

		IListener*										m_pListener;
		IEventReactor*								m_pEventReactor;

		typedef map<uint32, CConnectorEx*>::type		Map_ConnectorExs; 
		typedef TUniqueObjMgr<CAcceptorEx>			Map_AcceptorExs;
		typedef std::queue<ISystemSignal*>					Queue_SystemSignals;

		Map_ConnectorExs							m_mapConnectorExs;
		Map_AcceptorExs							m_mapAcceptorExs;
		Queue_SystemSignals						m_quSystemSignals;

		typedef std::queue<CConnectorEx*>	Queue_IdleConnectorExs;
		typedef std::queue<CAcceptorEx*>		Queue_IdleAcceptorExs;

		Queue_IdleConnectorExs	m_quIdleConnectorExs;
		Queue_IdleAcceptorExs	m_quIdleAcceptorExs;

		static TNetWork<AcceptorExCallBack>* m_pNetWork;
	};
}
