#include <sstream>
#include "CEventReactor.h"



namespace PPE
{
    using namespace std;

	IEventReactor* GetEventReactor()
	{
		IEventReactor*  pReactor = PPE_NEW CEventReactor();
		pReactor->Init();
		return pReactor;
	}

	CEventReactor::CEventReactor(void)
		:m_pEventBase(NULL)
		,m_uReactorHandlerCounter(0)
	{
	}

	CEventReactor::~CEventReactor(void)
	{
	}

	void*	CEventReactor::GetEventBase()
	{
		return m_pEventBase;
	}

	void CEventReactor::Init()
	{
#ifdef _WIN32
		WORD wVersion;
		WSADATA wsaData;
		wVersion = MAKEWORD(2, 2);
		int nResult= WSAStartup(wVersion, &wsaData);
		if(nResult)
		{
			std::stringstream strm;
			strm<<"WSAStartup failed with error code:"<<nResult<<std::ends;
			GenExp(strm.str().c_str());
		}
#endif

		m_pEventBase = event_base_new();
		if(!m_pEventBase)
		{
			stringstream strm;
			strm<<"CEventReactor Init error"<<std::endl;
			GenExp(strm.str().c_str());
		}
	}

	void CEventReactor::Release()
	{
		if (0 != m_uReactorHandlerCounter)
		{
			GenExp("0 != m_uReactorHandlerCounter");
		}

		event_base_free(m_pEventBase);
#ifdef _WIN32//WIN32
		int nResult= WSACleanup();
		if(nResult)
		{
			stringstream strm;
			strm<<"WSACleanup failed with error code:"<<nResult<<std::ends;
			GenExp(strm.str().c_str());
		}
#endif
		PPE_DELETE this;
	}

	void CEventReactor::DispatchEvents()
	{
		event_base_loop(m_pEventBase,EVLOOP_ONCE|EVLOOP_NONBLOCK);
	}

	bool CEventReactor::Register(IReactorHandler* pHandler)
	{
		m_uReactorHandlerCounter++;
		//cout<<"IReactorHandler	=	"<<pHandler<<"	×¢²á"<<endl;

		pHandler->RegisterToReactor();
		return true;
	}

	bool CEventReactor::UnRegister(IReactorHandler* pHandler)
	{
		PPE_ASSERT(0<m_uReactorHandlerCounter);
		m_uReactorHandlerCounter--;
		//cout<<"IReactorHandler	=	"<<pHandler<<"	×¢Ïú"<<endl;

		pHandler->UnRegisterFromReactor();
		return true;
	}
}
