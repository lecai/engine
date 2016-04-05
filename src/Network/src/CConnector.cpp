#include "CConnector.h"


namespace PPE
{
	IConnector* GetConnector(IEventReactor* pReactor)
	{
		return PPE_NEW_T(CConnector, MEMCATEGORY_GENERAL, "")( pReactor);
	}

	CConnector::CConnector(IEventReactor* pReactor)
		:m_pReactor(pReactor)
		,m_pStBufEv(NULL)
		,m_uMaxInBufferSize(eBS_MaxInBufferSize)
		,m_uMaxOutBufferSize(eBS_MaxOutBufferSize)
		,m_eState(eCS_Disconnected)
		,m_pFuncOnDisconnected(NULL)
		,m_pFuncOnConnectFailed(NULL)
		,m_pFuncOnConnectted(NULL)
		,m_pFuncOnSomeDataSend(NULL)
		,m_pFuncOnSomeDataRecv(NULL)
	{
	}
	
	CConnector::~CConnector(void)
	{
		PPE_ASSERT(true==IsDisconnected());
	}

	void	CConnector::GetRemoteIpAddress(char* szBuf, uint32 uBufSize)
	{
		PPE_ASSERT(uBufSize >= 16);
		strncpy( szBuf, m_Addr.GetAddress(), 16 );
	}

	bool CConnector::Connect(const CNetAddr& addr, const timeval * time /* = NULL */)
	{
#ifdef WIN32
		PPE_INFO_2(PPE_FormatStr("connect begin : %s , %d ", addr.GetAddress(), addr.GetPort()), LML_CRITICAL);
#else
		PpeSqlLogEx(elc_Common, 0, "connect begin : %s , %d ", addr.GetAddress(), addr.GetPort());
#endif

		PPE_ASSERT(NULL==m_pStBufEv);
		EPipeConnFailedReason eReason;
		m_Socket.Open();
		sockaddr_in saiAddress;
		m_Addr.Copy(addr);
		CSocket::Address2sockaddr_in(saiAddress,addr);
		m_Socket.SetNonblocking();
		int nResult=connect(m_Socket.GetSystemSocket(),reinterpret_cast<sockaddr*>(&saiAddress),sizeof(sockaddr));
		PPE_ASSERT(SOCKET_ERROR == nResult);

		int nError = PpeGetLastError();
		switch(nError)
		{ 
#if defined(__linux__)
			case EINPROGRESS:
#elif defined(WIN32)
			case EWOULDBLOCK:
#endif
				nResult=1;
				break;
#ifdef _WIN32
			case ENOBUFS:
				nResult=2;
				eReason=ePCFR_NOBUFFER;
				PPE_INFO_2(PPE_FormatStr("connect return error ENOBUFS : %d", ENOBUFS), LML_CRITICAL);
				return false;
#endif
			case EADDRINUSE:
				nResult=2;
				eReason=ePCFR_LOCALADDRINUSE;
#ifdef WIN32
				PPE_INFO_2(PPE_FormatStr("connect return error EADDRINUSE : %d", EADDRINUSE), LML_CRITICAL);
#else
				PpeSqlLogEx(elc_Common, 0, "connect return error EADDRINUSE : %d", EADDRINUSE);
#endif
				return false;
			case ECONNREFUSED:
				nResult=2;
				eReason=ePCFR_REFUSED;
#ifdef WIN32
				PPE_INFO_2(PPE_FormatStr("connect return error ECONNREFUSED : %d", ECONNREFUSED), LML_CRITICAL);
#else
				PpeSqlLogEx(elc_Common, 0, "connect return error ECONNREFUSED : %d", ECONNREFUSED);
#endif
				return false;
			case ETIMEDOUT:
				nResult=2;
				eReason=ePCFR_TIMEDOUT;
#ifdef WIN32
				PPE_INFO_2(PPE_FormatStr("connect return error ETIMEDOUT : %d", ETIMEDOUT), LML_CRITICAL);
#else
				PpeSqlLogEx(elc_Common, 0, "connect return error ETIMEDOUT : %d", ETIMEDOUT);
#endif
				return false;
			case ENETUNREACH:
				nResult=2;
				eReason=ePCFR_UNREACH;
#ifdef WIN32
				PPE_INFO_2(PPE_FormatStr("connect return error ENETUNREACH : %d", ENETUNREACH), LML_CRITICAL);
#else
				PpeSqlLogEx(elc_Common, 0, "connect return error ENETUNREACH : %d", ENETUNREACH);
#endif
				return false;
			case ECONNRESET:
				nResult=2;
				eReason=ePCFR_RESET;
#ifdef WIN32
				PPE_INFO_2(PPE_FormatStr("connect return error ECONNRESET : %d", ECONNRESET), LML_CRITICAL);
#else
				PpeSqlLogEx(elc_Common, 0, "connect return error ECONNRESET : %d", ECONNRESET);
#endif
				return false;
			default:
				{
#ifdef WIN32
					PPE_INFO_2(PPE_FormatStr("connect return error  unkown : %d", nError), LML_CRITICAL);
#else
					PpeSqlLogEx(elc_Common, 0, "connect return error  unkown : %d", nError);
#endif
					GenExp("connect failed!!");
				}
		}

		//还没有成功，但是没有不可以忽略的错误
		//注册CEventPipe的event，event触发时回调 CConnector::LibeventCb
		event_set(&m_ConnectEvent, (int)m_Socket.GetSystemSocket(), EV_WRITE, CConnector::lcb_OnConnectResult, this);
		event_base_set (static_cast<event_base *>(GetReactor()->GetEventBase()), &m_ConnectEvent);
		event_add(&m_ConnectEvent, time);
		SetState(eCS_Connecting);

		return true;
	}

	void CConnector::SetCallbackFunc(ConnectorFuncOnDisconnected pOnDisconnected, ConnectorFuncOnConnectFailed pOnConnectFailed, ConnectorFuncOnConnectted pOnConnectted, ConnectorFuncOnSomeDataSend pOnSomeDataSend, ConnectorFuncOnSomeDataRecv pOnSomeDataRecv)
	{
		m_pFuncOnDisconnected = pOnDisconnected;
		m_pFuncOnConnectFailed = pOnConnectFailed;
		m_pFuncOnConnectted = pOnConnectted;
		m_pFuncOnSomeDataSend = pOnSomeDataSend;
		m_pFuncOnSomeDataRecv = pOnSomeDataRecv;
	}

	void	CConnector::lcb_OnConnectResult(int32 Socket, int16 nEventMask, void *arg)
	{
		CConnector* pConnector = static_cast<CConnector *>(arg);
		pConnector->HandleInput(Socket,nEventMask,NULL);
	}

	void CConnector::HandleInput(int32 Socket, int16 nEventMask, void* arg)
	{
		switch(nEventMask)
		{
			// connect成功
		case EV_WRITE:
			{
#ifdef WIN32
				PPE_INFO_2(PPE_FormatStr("OnConnectted %s : %d ", m_Addr.GetAddress(), m_Addr.GetPort()), LML_CRITICAL);
#else
				PpeSqlLog(elc_Common, 0, "OnConnectted");
#endif
				OnConnectted();
				return;
			}break;
			// connect超时
		case EV_TIMEOUT:
			{
#ifdef WIN32
				PPE_INFO_2(PPE_FormatStr("connect %s : %d EV_TIMEOUT", m_Addr.GetAddress(), m_Addr.GetPort()), LML_CRITICAL);
#else
				PpeSqlLog(elc_Common, 0, "connect EV_TIMEOUT");
#endif
				PPE_ASSERT(IsConnecting());
				ShutDown();
				m_pFuncOnConnectFailed(this);
			}break;
		default:
			{
				GenExp("connect failed, unkown error!!!");
				// 获取soket的详细错误类型，告诉handler
			}break;
		}
	}

	void		CConnector::OnConnectted()
	{
		GetReactor()->Register(this);
		m_pFuncOnConnectted(this);
	}

	bool		CConnector::RegisterToReactor()
	{
		m_pStBufEv = bufferevent_new(static_cast<int>(m_Socket.GetSystemSocket()), 
			CConnector::lcb_OnPipeRead,		
			CConnector::lcb_OnPipeWrite,		
			CConnector::lcb_OnPipeError,this);

		if(NULL==m_pStBufEv)
			GenExp("bufferevent_new failed!!");

		bufferevent_setwatermark(m_pStBufEv,EV_READ,0,m_uMaxInBufferSize);
		bufferevent_setwatermark(m_pStBufEv,EV_WRITE,0,m_uMaxOutBufferSize);
		bufferevent_base_set(static_cast<event_base *>(GetReactor()->GetEventBase()), m_pStBufEv);

		int32 nRes = bufferevent_enable(m_pStBufEv, EV_READ | EV_WRITE);
		PPE_ASSERT(0==nRes);
		SetState(eCS_Connected);
		return true;
	}

	bool		CConnector::UnRegisterFromReactor()
	{
		PPE_ASSERT(NULL!=m_pStBufEv);
		bufferevent_disable(m_pStBufEv, EV_READ | EV_WRITE);
		return true;
	}

	void	CConnector::ShutDown()
	{
		if(IsConnecting())
		{
#ifdef WIN32
			PPE_INFO_2(PPE_FormatStr("ShutDown In Connecting: %s : %d", m_Addr.GetAddress(), m_Addr.GetPort()), LML_CRITICAL);
#else
			PpeSqlLog(elc_Common, 0, "ShutDown In Connecting");
#endif
			event_del(&m_ConnectEvent);
		}
		else if(IsConnected())
		{
#ifdef WIN32
			PPE_INFO_2(PPE_FormatStr("ShutDown In Connected: %s : %d", m_Addr.GetAddress(), m_Addr.GetPort()), LML_CRITICAL);
#else
			PpeSqlLog(elc_Common, 0, "ShutDown In Connected");
#endif
			GetReactor()->UnRegister(this);
		}
		m_Socket.Shutdown();
		SetState(eCS_Disconnected);
	}

	void		CConnector::Release()
	{
		ShutDown();
		if(m_pStBufEv)
		{
			bufferevent_free(m_pStBufEv);
			m_pStBufEv = NULL;
		}
		PPE_DELETE_T( this, CConnector, MEMCATEGORY_GENERAL);
	}

	void	 CConnector::lcb_OnPipeRead(struct bufferevent* bev, void* arg)
	{
		CConnector* pConnector = static_cast<CConnector *>(arg);
		pConnector->m_pFuncOnSomeDataRecv(pConnector);
	}

	void	CConnector::lcb_OnPipeWrite(bufferevent* bev, void* arg)
	{
		CConnector* pConnector = static_cast<CConnector *>(arg);
		pConnector->m_pFuncOnSomeDataSend(pConnector);
	}

	void	CConnector::lcb_OnPipeError(bufferevent* bev, int16 nWhat, void* arg)
	{
		//由于libevent错误类型较少，再这里获取socket错误
		//并进行相应处理
		CConnector* pConnector = static_cast<CConnector *>(arg);

#ifdef WIN32
		PPE_INFO_2(PPE_FormatStr("%s, %d lcb_OnPipeError With PpeGetLastError %d", pConnector->m_Addr.GetAddress(), pConnector->m_Addr.GetPort(), PpeGetLastError()), LML_CRITICAL);
#else
		PpeSqlLogEx(elc_Common, 0, "lcb_OnPipeError With PpeGetLastError %d", PpeGetLastError());
#endif
		pConnector->ShutDown();
		
		if(nWhat&EVBUFFER_EOF)
		{
#ifdef WIN32
			PPE_INFO_2(PPE_FormatStr("%s, %d lcb_OnPipeError EVBUFFER_EOF %d", pConnector->m_Addr.GetAddress(), pConnector->m_Addr.GetPort() , nWhat), LML_CRITICAL);
#else
			PpeSqlLogEx(elc_Common, 0, "lcb_OnPipeError EVBUFFER_EOF %d", nWhat);
#endif
			pConnector->m_pFuncOnDisconnected(pConnector);
			return;
		}

		if(nWhat&EVBUFFER_ERROR)
		{
#ifdef WIN32
			PPE_INFO_2(PPE_FormatStr("%s, %d lcb_OnPipeError EVBUFFER_ERROR %d", pConnector->m_Addr.GetAddress(), pConnector->m_Addr.GetPort() , nWhat), LML_CRITICAL);
#else
			PpeSqlLogEx(elc_Common, 0, "lcb_OnPipeError EVBUFFER_ERROR %d", nWhat);
#endif
			pConnector->m_pFuncOnDisconnected(pConnector);
			return;
		}
		PPE_ASSERT(false);
	}

	void*	CConnector::GetContext(void) const
	{
		return m_pContext;
	}

	void		CConnector::SetContext(void* pContext)
	{
		m_pContext = pContext;
	}

	PipeResult CConnector::Send(const void* pData, uint32 uSize)
	{
		if(!IsConnected())
			return ePR_Disconnected;

		if(m_pStBufEv->output->off+uSize > m_uMaxOutBufferSize)
		{
#ifdef WIN32
			PPE_INFO_2(PPE_FormatStr("%s %d CConnector send buffer out of size", m_Addr.GetAddress(), m_Addr.GetPort() ), LML_CRITICAL);
#else
			PpeSqlLog(elc_Common, 0, "CConnector send buffer out of size");
#endif
			ShutDown();
			m_pFuncOnDisconnected(this);
			return ePR_OutPipeBuf;
		}

		int32 nRes = bufferevent_write(m_pStBufEv,  pData, uSize);
		PPE_ASSERT(0==nRes);
		return ePR_OK;
	}
	 
	void*	CConnector::GetRecvData()const
	{
		PPE_ASSERT(NULL!=m_pStBufEv);
		return m_pStBufEv->input->buffer;
	}

	uint32	CConnector::GetRecvDataSize()
	{
		PPE_ASSERT(NULL!=m_pStBufEv);
		return static_cast<uint32>(m_pStBufEv->input->off);
	}

	uint32	CConnector::GetSendDataSize()
	{
		PPE_ASSERT(NULL!=m_pStBufEv);
		return static_cast<uint32>(m_pStBufEv->output->off);
	}

	void		CConnector::PopRecvData(uint32 uSize)
	{
		PPE_ASSERT(NULL!=m_pStBufEv);
		PPE_ASSERT(m_pStBufEv->input->off >= uSize);
		PPE_ASSERT(uSize>=0);
		if(uSize>0)
			evbuffer_drain(m_pStBufEv->input,uSize);
	}

	void		CConnector::SetMaxSendBufSize(uint32 uSize)
	{
		PPE_ASSERT(uSize>0);
		m_uMaxOutBufferSize=uSize;
		if(IsConnected())
		{
			PPE_ASSERT(NULL!=m_pStBufEv);
			bufferevent_setwatermark(m_pStBufEv,EV_WRITE,0,m_uMaxOutBufferSize);
		}
	}

	uint32	CConnector::GetMaxSendBufSize()
	{
		return m_uMaxOutBufferSize;
	}

	void	CConnector::SetMaxRecvBufSize(uint32 uSize)
	{
		PPE_ASSERT(uSize>0);
		m_uMaxInBufferSize=uSize;
		if(IsConnected())
		{
			PPE_ASSERT(NULL!=m_pStBufEv);
			bufferevent_setwatermark(m_pStBufEv,EV_READ,0,m_uMaxInBufferSize);
		}
	}

	uint32	CConnector::GetMaxRecvBufSize()
	{
		return m_uMaxInBufferSize;
	}

	bool		CConnector::IsConnected()
	{
		return m_eState == eCS_Connected;
	}

	bool	CConnector::IsConnecting()
	{
		return m_eState == eCS_Connecting;
	}

	bool CConnector::IsDisconnected()
	{
		return m_eState == eCS_Disconnected;
	}

	IEventReactor* CConnector::GetReactor()
	{
		return m_pReactor;
	}


	CConnector::eConnectorState CConnector::GetState()
	{
		return m_eState;
	}
	
	void CConnector::SetState(eConnectorState eState)
	{
		m_eState = eState;
	}


	void CConnector::ProcessSocketError()
	{
		switch(m_Socket.GetSocketError())
		{
		case ePCFR_UNREACH:
		case ePCFR_REFUSED:
		case ePCFR_RESET:
		case ePCFR_LOCALADDRINUSE:
		case ePCFR_NOBUFFER:
		case ePCFR_TIMEDOUT:
			m_pFuncOnDisconnected(this);
			break;
		default:
			GenExp("unknown socket error!!!");
		}
	}

}
