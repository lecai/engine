#pragma once

#include "NetInc.h"
#include "INetWork.h"


#include "event.h"


namespace Net
{
	class CEventReactor : public IEventReactor
	{
	public:
		CEventReactor(void);
		virtual ~CEventReactor(void);
        
		bool Register(IReactorHandler* pHandler);
		bool UnRegister(IReactorHandler* pHandler);
        
		void*	GetEventBase();
        
		void Init();
		void DispatchEvents();
		void	Release();
	private:
		uint32	m_uReactorHandlerCounter;
		event_base * m_pEventBase;
	};
}

