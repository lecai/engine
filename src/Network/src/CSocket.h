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
		//��ʼ��socket
		virtual bool	Open ();
		//�ر�socket
		virtual void	Close ();
		//shutdown ֮���ٹر�socket
		void	Shutdown ();
		void	ShutdownRead ();
		void	ShutdownWrite ();
		//�ѱ�socket���óɷ�����
		void	SetNonblocking();
		//�ѱ�socket��һ����ַ
		uint32	Bind(const CNetAddr& Address);

		//ȡ��ַ
		bool	GetLocalAddress(CNetAddr& Address) const;
		bool	GetRemoteAddress(CNetAddr& Address) const;    

		//ȡ��ϵͳsocket �ļ�������
		SOCKET	GetSystemSocket () const			{return m_Socket;}
		//����socket �ļ�������
		void	SetSystemSocket (SOCKET Socket)	{m_Socket=Socket;}

		
		//ȡ��socket������Ϣ
		int		GetSocketError()const;

		//-----------------����tools--------------------
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