#pragma once

#include "EventReactor.h"
#include "NetAddr.h"
#include "Socket.h"
#include "INetWork.h"
#include "event.h"

namespace Net
{
	enum eListenerState
	{
		eLS_UnListen=0,
		eLS_Listened,
	};
	
	class NETWORK_API CListener : public IListener
	{
	public:
		CListener(IEventReactor* pReactor);
		virtual ~CListener(void);

		bool Listen(CNetAddr& addr, ListenerFuncOnAccept pFunc);
		IEventReactor*  GetReactor();

		void ShutDown();
		void Release();

		bool IsListened();

	private:

		void SetState(eListenerState eState);

		bool RegisterToReactor();
		bool UnRegisterFromReactor();

		static void lcb_Accept(int Socket, short nEventMask, void* arg);
		void HandleInput(int Socket, short nEventMask, void *arg);

	private:
		ListenerFuncOnAccept                m_pFuncOnAccept;
		eListenerState                      m_eState;
		CNetAddr                            m_ListenAddress;
		IEventReactor*                      m_pEventReactor;
		CSocket                             m_Socket;
		event								m_event;
	};
}