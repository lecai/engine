#pragma  once


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
//#include <WinSock2.h>
//#include <Windows.h>

#ifdef NETWORK_EXPORTS
#define NETWORK_API __declspec(dllexport)
#else
#define NETWORK_API __declspec(dllimport)
#endif

#ifdef NETWORK_STATIC
#undef NETWORK_API
#define NETWORK_API
#endif

#else
#define NETWORK_API
#endif

#include <iostream>
#include <sstream>


#ifdef WIN32
#define PpeGetLastError		WSAGetLastError
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
//#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
//#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE
#define SHUT_RDWR               SD_BOTH
#define SHUT_RD					SD_RECEIVE      
#define SHUT_WR                 SD_SEND

#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <cerrno>


#define PpeGetLastError() errno
#define closesocket			close
#define ioctlsocket			ioctl
#define SOCKET			int
typedef struct linger 		LINGER;
#define SOCKET_ERROR		-1
#define INVALID_SOCKET		-1
#define SD_SEND			SHUT_WR

#endif

#define INTER_FACE struct


enum EPipeDisconnectReason
{
	ePDR_REMOTE,	
	ePDR_ERROR
	//ePDR_LOCAL=0 保持旧的一致,不能先加
};

enum EPipeConnFailedReason
{
	ePCFR_UNREACH,
	ePCFR_INVALIDADDR,
	ePCFR_REFUSED,
	ePCFR_RESET,
	ePCFR_LOCALADDRINUSE,
	ePCFR_NOBUFFER,
	ePCFR_TIMEDOUT
};