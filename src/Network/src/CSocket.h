#pragma once
#include "NetWorkInclude.h"
#include "CNetAddr.h"
#include "event.h"

namespace PPE
{
	class NETWORK_API CSocket
	{
	public:
		CSocket(SOCKET Socket);
		CSocket(int32 nType=SOCK_STREAM, int32 nProtocolFamily=AF_INET, int32 nProtocol=0);
		virtual ~CSocket();
		//初始化socket
		virtual bool	Open ();
		//关闭socket
		virtual void	Close ();
		//shutdown 之后再关闭socket
		void	Shutdown ();
		void	ShutdownRead ();
		void	ShutdownWrite ();
		//把本socket设置成非阻塞
		void	SetNonblocking();
		//把本socket绑定一个地址
		uint32	Bind(const CNetAddr& Address);

		//取地址
		bool	GetLocalAddress(CNetAddr& Address) const;
		bool	GetRemoteAddress(CNetAddr& Address) const;    

		//取得系统socket 文件描述符
		SOCKET	GetSystemSocket () const			{return m_Socket;}
		//设置socket 文件描述符
		void	SetSystemSocket (SOCKET Socket)	{m_Socket=Socket;}

		
		//取得socket错误信息
		int		GetSocketError()const;

		//-----------------辅助tools--------------------
		static SOCKET CreateSocket(int32 Type=SOCK_STREAM, int32 ProtocolFamily=AF_INET, int32 Protocol=0);
		static SOCKET CreateBindedSocket(const CNetAddr& address);
		static void	MakeSocketNonblocking(SOCKET Socket);
		static void Address2sockaddr_in(sockaddr_in& saiAddress,const CNetAddr& Address);

	protected:
		SOCKET m_Socket;

	private:
		int32 m_nType;
		int32 m_nProtocolFamily;
		int32 m_nProtocol;
	};
}