#include "CSocket.h"





namespace PPE
{
	CSocket::CSocket(SOCKET Socket)
		:m_Socket(Socket)
	{

	}

	CSocket::CSocket(int32 nType/*=SOCK_STREAM*/, int32 nProtocolFamily/*=AF_INET*/, int32 nProtocol/*=0*/)
		:m_Socket(INVALID_SOCKET),
		m_nType(nType),
		m_nProtocolFamily(nProtocolFamily),
		m_nProtocol(nProtocol)
	{		
	}

	CSocket::~CSocket()
	{
		Close();
	}


	//----------------------------------------------------------------
	bool CSocket::Open ()
	{   
		PPE_ASSERT( INVALID_SOCKET == m_Socket);
		m_Socket = CreateSocket(m_nType,m_nProtocolFamily,m_nProtocol);
		if(INVALID_SOCKET == m_Socket)
		{
			std::ostringstream strm;
			strm<<"CreateSocket failed with error code "<< PpeGetLastError() <<"."<< std::ends;
			GenExp(strm.str().c_str());
		}
		return true;
	}

	//-------------------------tools--------------------------------
	SOCKET CSocket::CreateSocket(int32 Type, int32 ProtocolFamily, int32 Protocol)
	{		
		SOCKET Socket = socket(ProtocolFamily, Type, Protocol);//����socket
		if (Socket == INVALID_SOCKET)
		{
			std::ostringstream strm;
			strm<<"socket failed with error code "<< PpeGetLastError() <<"."<<std::ends;
			GenExp(strm.str().c_str());
		}
		int iVal=1;
		if(SOCKET_ERROR==setsockopt(Socket,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&iVal),sizeof(iVal)))
		{
			std::ostringstream strm;
			strm<<"setsockopt failed with error code "<< PpeGetLastError() <<"."<<std::ends;
			GenExp(strm.str().c_str());
		}
		return Socket;
	}

	//-----------------------------------------------------------------
	void CSocket::Close ()
	{    
		if (m_Socket != INVALID_SOCKET)
		{
			closesocket(m_Socket);
			m_Socket=INVALID_SOCKET;
		}
	}

	//-----------------------------------------------------------------
	void CSocket::Shutdown ()
	{    
		if (m_Socket != INVALID_SOCKET)
		{
			shutdown(m_Socket,SHUT_RDWR);//�ر�����
			closesocket(m_Socket);
			m_Socket=INVALID_SOCKET;
		}
	}

	//-----------------------------------------------------------------
	void CSocket::ShutdownRead ()
	{    
		if (m_Socket != INVALID_SOCKET)
		{
			shutdown(m_Socket,SHUT_RD);
		}
	}

	//-----------------------------------------------------------------
	void CSocket::ShutdownWrite ()
	{    
		if (m_Socket != INVALID_SOCKET)
		{
			shutdown(m_Socket,SHUT_WR);
		}
	}

	//-----------------------------------------------------------------
	void CSocket::SetNonblocking()
	{
		//ArkAst(m_Socket != INVALID_SOCKET);
		MakeSocketNonblocking(m_Socket);
	}

	//-----------------------------------------------------------------
	bool CSocket::GetLocalAddress(CNetAddr& Addr)const
	{
		if(INVALID_SOCKET == m_Socket)	return false;
		sockaddr_in asiAddress;

#ifdef WIN32
		int32 nSize=0;
#else
		socklen_t nSize =0;
#endif
		nSize=sizeof(asiAddress);
		memset(&asiAddress,0,nSize);
		//int nErrorCode;
		if(getsockname(m_Socket,reinterpret_cast<sockaddr*>(&asiAddress),&nSize))
		{
			//nErrorCode=ArkGetLastError();
			return false;
		}
		Addr.SetPort(ntohs(asiAddress.sin_port));
		Addr.SetAddress(inet_ntoa(asiAddress.sin_addr));
		return true;
	}

	//-----------------------------------------------------------------
	bool CSocket::GetRemoteAddress(CNetAddr& Address) const
	{    
		if(INVALID_SOCKET == m_Socket)
			return false;
		sockaddr_in asiAddress;

#ifdef WIN32
		int32 nSize=0;
#else
		socklen_t nSize =0;
#endif
		nSize=sizeof(asiAddress);
		memset(&asiAddress,0,sizeof(asiAddress));
		if(getpeername(m_Socket,reinterpret_cast<sockaddr*>(&asiAddress),&nSize))
		{			
			return false;
		}
		Address.SetPort(ntohs(asiAddress.sin_port));
		Address.SetAddress(inet_ntoa(asiAddress.sin_addr));
		return true;
	}


	//-----------------------------------------------------------------
	int	CSocket::GetSocketError()const
	{
		int nError = PpeGetLastError();
		EPipeConnFailedReason eReason;
		switch(nError)
		{
#if defined(__linux__)
			case EINPROGRESS:
#elif defined(WIN32)
			case EWOULDBLOCK:
#endif
				break;
#ifdef _WIN32
			case ENOBUFS:
				eReason = ePCFR_NOBUFFER;
#endif
			case EADDRINUSE:
				eReason = ePCFR_LOCALADDRINUSE;
			case ECONNREFUSED:
				eReason = ePCFR_REFUSED;
			case ETIMEDOUT:
				eReason = ePCFR_TIMEDOUT;
			case ENETUNREACH:
				eReason = ePCFR_UNREACH;
			case ECONNRESET:
				eReason = ePCFR_RESET;
			default:
				GenExp("unknown failed!!");
		}
		return eReason;

	}

	//-----------------------------------------------------------------
	uint32 CSocket::Bind(const CNetAddr& Address)
	{
		//ArkAst(m_Socket != INVALID_SOCKET);

		sockaddr_in saiAddress;
		Address2sockaddr_in(saiAddress,Address);
		//bind
		if( bind(m_Socket,reinterpret_cast<sockaddr*>(&saiAddress),sizeof(sockaddr)) )
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
			return PpeGetLastError();
		}
		return 0;
	}


	//--------------------------------- *tools* -------------------------------------------------------------------
	void CSocket::Address2sockaddr_in(sockaddr_in& saiAddress, const CNetAddr& Address)
	{		
		memset(&saiAddress,0,sizeof(saiAddress));
		saiAddress.sin_addr.s_addr = inet_addr(Address.GetAddress());
		saiAddress.sin_port = htons(static_cast<u_short>(Address.GetPort()));
		saiAddress.sin_family = AF_INET;
	}

	//-----------------------------------------------------------------
	SOCKET CSocket::CreateBindedSocket(const CNetAddr& Address)
	{
		SOCKET Socket=CreateSocket();

		sockaddr_in saiAddress;
		Address2sockaddr_in(saiAddress,Address);
		//bind
		if( bind(Socket,reinterpret_cast<sockaddr*>(&saiAddress),sizeof(sockaddr)) )
		{
			closesocket(Socket);
			return PpeGetLastError();
		}
		return Socket;
	}

	//-----------------------------------------------------------------
	void CSocket::MakeSocketNonblocking(SOCKET Socket)
	{
		if(-1 == evutil_make_socket_nonblocking(static_cast<int>(Socket)))
		{
			closesocket(Socket);
			std::ostringstream strm;
			strm<<"ioctlsocket failed with error code "<< PpeGetLastError() <<"."<<std::ends;
			GenExp(strm.str().c_str());
		}
	}
}