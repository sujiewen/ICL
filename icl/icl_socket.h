///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
// icl_socket.h
// Classes:
//   > CSocketException
//   > CSocket
//   > CUdpSocket
//   > CUdpClient
//   > CUdpServer
//   > CTcpSocket
//   > CDtpConnection
//   > CTcpConnection
//   > CTcpClient
//   > CTcpServer
//   > CListenerThread
//   > CUdpListenerThread
//   > CUdpListenerThreadPool
//   > CTcpListenerThread
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_SOCKET_H_
#define _ICL_SOCKET_H_

#include "icl_config.h"

#ifdef ICL_WIN32
#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#endif

#ifdef ICL_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if_arp.h> 
#include <net/if.h> 
#include <string>
#endif

#include "icl_classes.h"
#include "icl_thread.h"

using namespace icl;

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
// 提前声明

class CTcpServer;
class CListenerThread;
class CUdpListenerThread;
class CUdpListenerThreadPool;
class CTcpListenerThread;

///////////////////////////////////////////////////////////////////////////////
// 常量定义

#ifdef ICL_WIN32
const int IS_SD_RECV            = 0;
const int IS_SD_SEND            = 1;
const int IS_SD_BOTH            = 2;

const int IS_EINTR              = WSAEINTR;
const int IS_EBADF              = WSAEBADF;
const int IS_EACCES             = WSAEACCES;
const int IS_EFAULT             = WSAEFAULT;
const int IS_EINVAL             = WSAEINVAL;
const int IS_EMFILE             = WSAEMFILE;
const int IS_EWOULDBLOCK        = WSAEWOULDBLOCK;
const int IS_EINPROGRESS        = WSAEINPROGRESS;
const int IS_EALREADY           = WSAEALREADY;
const int IS_ENOTSOCK           = WSAENOTSOCK;
const int IS_EDESTADDRREQ       = WSAEDESTADDRREQ;
const int IS_EMSGSIZE           = WSAEMSGSIZE;
const int IS_EPROTOTYPE         = WSAEPROTOTYPE;
const int IS_ENOPROTOOPT        = WSAENOPROTOOPT;
const int IS_EPROTONOSUPPORT    = WSAEPROTONOSUPPORT;
const int IS_ESOCKTNOSUPPORT    = WSAESOCKTNOSUPPORT;

const int IS_EOPNOTSUPP         = WSAEOPNOTSUPP;
const int IS_EPFNOSUPPORT       = WSAEPFNOSUPPORT;
const int IS_EAFNOSUPPORT       = WSAEAFNOSUPPORT;
const int IS_EADDRINUSE         = WSAEADDRINUSE;
const int IS_EADDRNOTAVAIL      = WSAEADDRNOTAVAIL;
const int IS_ENETDOWN           = WSAENETDOWN;
const int IS_ENETUNREACH        = WSAENETUNREACH;
const int IS_ENETRESET          = WSAENETRESET;
const int IS_ECONNABORTED       = WSAECONNABORTED;
const int IS_ECONNRESET         = WSAECONNRESET;
const int IS_ENOBUFS            = WSAENOBUFS;
const int IS_EISCONN            = WSAEISCONN;
const int IS_ENOTCONN           = WSAENOTCONN;
const int IS_ESHUTDOWN          = WSAESHUTDOWN;
const int IS_ETOOMANYREFS       = WSAETOOMANYREFS;
const int IS_ETIMEDOUT          = WSAETIMEDOUT;
const int IS_ECONNREFUSED       = WSAECONNREFUSED;
const int IS_ELOOP              = WSAELOOP;
const int IS_ENAMETOOLONG       = WSAENAMETOOLONG;
const int IS_EHOSTDOWN          = WSAEHOSTDOWN;
const int IS_EHOSTUNREACH       = WSAEHOSTUNREACH;
const int IS_ENOTEMPTY          = WSAENOTEMPTY;
#endif

#ifdef ICL_LINUX
const int IS_SD_RECV            = SHUT_RD;
const int IS_SD_SEND            = SHUT_WR;
const int IS_SD_BOTH            = SHUT_RDWR;

const int IS_EINTR              = EINTR;
const int IS_EBADF              = EBADF;
const int IS_EACCES             = EACCES;
const int IS_EFAULT             = EFAULT;
const int IS_EINVAL             = EINVAL;
const int IS_EMFILE             = EMFILE;
const int IS_EWOULDBLOCK        = EWOULDBLOCK;
const int IS_EINPROGRESS        = EINPROGRESS;
const int IS_EALREADY           = EALREADY;
const int IS_ENOTSOCK           = ENOTSOCK;
const int IS_EDESTADDRREQ       = EDESTADDRREQ;
const int IS_EMSGSIZE           = EMSGSIZE;
const int IS_EPROTOTYPE         = EPROTOTYPE;
const int IS_ENOPROTOOPT        = ENOPROTOOPT;
const int IS_EPROTONOSUPPORT    = EPROTONOSUPPORT;
const int IS_ESOCKTNOSUPPORT    = ESOCKTNOSUPPORT;

const int IS_EOPNOTSUPP         = EOPNOTSUPP;
const int IS_EPFNOSUPPORT       = EPFNOSUPPORT;
const int IS_EAFNOSUPPORT       = EAFNOSUPPORT;
const int IS_EADDRINUSE         = EADDRINUSE;
const int IS_EADDRNOTAVAIL      = EADDRNOTAVAIL;
const int IS_ENETDOWN           = ENETDOWN;
const int IS_ENETUNREACH        = ENETUNREACH;
const int IS_ENETRESET          = ENETRESET;
const int IS_ECONNABORTED       = ECONNABORTED;
const int IS_ECONNRESET         = ECONNRESET;
const int IS_ENOBUFS            = ENOBUFS;
const int IS_EISCONN            = EISCONN;
const int IS_ENOTCONN           = ENOTCONN;
const int IS_ESHUTDOWN          = ESHUTDOWN;
const int IS_ETOOMANYREFS       = ETOOMANYREFS;
const int IS_ETIMEDOUT          = ETIMEDOUT;
const int IS_ECONNREFUSED       = ECONNREFUSED;
const int IS_ELOOP              = ELOOP;
const int IS_ENAMETOOLONG       = ENAMETOOLONG;
const int IS_EHOSTDOWN          = EHOSTDOWN;
const int IS_EHOSTUNREACH       = EHOSTUNREACH;
const int IS_ENOTEMPTY          = ENOTEMPTY;
#endif

///////////////////////////////////////////////////////////////////////////////
// 错误信息 (Iris Socket Error Message)

const char* const ISEM_ERROR             = "Socket Error #%d: %s";
const char* const ISEM_EINTR             = "Interrupted system call.";
const char* const ISEM_EBADF             = "Bad file number.";
const char* const ISEM_EACCES            = "Access denied.";
const char* const ISEM_EFAULT            = "Buffer fault.";
const char* const ISEM_EINVAL            = "Invalid argument.";
const char* const ISEM_EMFILE            = "Too many open files.";
const char* const ISEM_EWOULDBLOCK       = "Operation would block.";
const char* const ISEM_EINPROGRESS       = "Operation now in progress.";
const char* const ISEM_EALREADY          = "Operation already in progress.";
const char* const ISEM_ENOTSOCK          = "Socket operation on non-socket.";
const char* const ISEM_EDESTADDRREQ      = "Destination address required.";
const char* const ISEM_EMSGSIZE          = "Message too long.";
const char* const ISEM_EPROTOTYPE        = "Protocol wrong type for socket.";
const char* const ISEM_ENOPROTOOPT       = "Bad protocol option.";
const char* const ISEM_EPROTONOSUPPORT   = "Protocol not supported.";
const char* const ISEM_ESOCKTNOSUPPORT   = "Socket type not supported.";
const char* const ISEM_EOPNOTSUPP        = "Operation not supported on socket.";
const char* const ISEM_EPFNOSUPPORT      = "Protocol family not supported.";
const char* const ISEM_EAFNOSUPPORT      = "Address family not supported by protocol family.";
const char* const ISEM_EADDRINUSE        = "Address already in use.";
const char* const ISEM_EADDRNOTAVAIL     = "Cannot assign requested address.";
const char* const ISEM_ENETDOWN          = "Network is down.";
const char* const ISEM_ENETUNREACH       = "Network is unreachable.";
const char* const ISEM_ENETRESET         = "Net dropped connection or reset.";
const char* const ISEM_ECONNABORTED      = "Software caused connection abort.";
const char* const ISEM_ECONNRESET        = "Connection reset by peer.";
const char* const ISEM_ENOBUFS           = "No buffer space available.";
const char* const ISEM_EISCONN           = "Socket is already connected.";
const char* const ISEM_ENOTCONN          = "Socket is not connected.";
const char* const ISEM_ESHUTDOWN         = "Cannot send or receive after socket is closed.";
const char* const ISEM_ETOOMANYREFS      = "Too many references, cannot splice.";
const char* const ISEM_ETIMEDOUT         = "Connection timed out.";
const char* const ISEM_ECONNREFUSED      = "Connection refused.";
const char* const ISEM_ELOOP             = "Too many levels of symbolic links.";
const char* const ISEM_ENAMETOOLONG      = "File name too long.";
const char* const ISEM_EHOSTDOWN         = "Host is down.";
const char* const ISEM_EHOSTUNREACH      = "No route to host.";
const char* const ISEM_ENOTEMPTY         = "Directory not empty";

const char* const ISEM_SOCKETERROR       = "Socket error";
const char* const ISEM_TCPSENDTIMEOUT    = "TCP send timeout";
const char* const ISEM_TCPRECVTIMEOUT    = "TCP recv timeout";

///////////////////////////////////////////////////////////////////////////////
// 类型定义

#ifdef ICL_WIN32
typedef int socklen_t;
#endif

typedef struct sockaddr_in SockAddr;

// 网络协议类型(UDP|TCP)
enum NetProtoType {
    NPT_UDP,        // UDP
    NPT_TCP         // TCP
};

// 网络协议类型(TCP|UTP)
enum DtpProtoType {
    DPT_TCP,        // TCP
    DPT_UTP         // UTP (UDP Transfer Protocol)
};

// Peer地址信息
struct CPeerAddress
{
    uint nIp;       // IP (主机字节顺序)
    int nPort;      // 端口

    CPeerAddress() : nIp(0), nPort(0) {}
    CPeerAddress(uint _nIp, int _nPort)
        { nIp = _nIp;  nPort = _nPort; }
    bool operator == (const CPeerAddress& rhs) const
        { return (nIp == rhs.nIp && nPort == rhs.nPort); }
};

///////////////////////////////////////////////////////////////////////////////
// 杂项函数


// 网络初始化/结束化
bool NetworkInitialize();
void NetworkFinalize();

// 解决跨平台问题的函数
int IclGetLastError();
string IclGetErrorMsg(int nError);
string IclGetLastErrMsg();
void IclCloseSocket(int nHandle);

// 杂项函数
string IpToString(int nIp);
int StringToIp(const string& strString);
void GetSocketAddr(SockAddr& SockAddr, uint nIpHostValue, int nPort);
int GetFreePort(NetProtoType nProto, int nStartPort, int nCheckTimes);
void GetLocalIpList(StringArray& IpList);
string GetLocalIp();

///////////////////////////////////////////////////////////////////////////////
// class CSocketException - 网络异常类
//
// 注: 所有的网络操作异常都使用该类或它的派生类。

class CSocketException : public CException
{
public:
    CSocketException(
        const char *sErrorMsg = NULL,
        const char *sFileName = NULL,
        int nLineNo = EMPTY_LINE_NO) :
	    CException(sErrorMsg, sFileName, nLineNo) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CSocket - 套接字类

class CSocket
{
friend class CTcpServer;

protected:
    bool m_bActive;     // 套接字是否准备就绪
    int m_nHandle;      // 套接字句柄
    int m_nDomain;      // 套接字的协议家族 (PF_UNIX, PF_INET, PF_INET6, PF_IPX, ...)
    int m_nType;        // 套接字类型，必须指定 (SOCK_STREAM, SOCK_DGRAM, SOCK_RAW, SOCK_RDM, SOCK_SEQPACKET)
    int m_nProtocol;    // 套接字所用协议，可为0 (IPPROTO_IP, IPPROTO_UDP, IPPROTO_TCP, ...)
    bool m_bBlockMode;  // 是否为阻塞模式 (缺省为阻塞模式)

private:
    void DoSetBlockMode(int nHandle, bool bValue);
    void DoClose();

protected:
    void SetActive(bool bValue);
    void SetDomain(int nValue);
    void SetType(int nValue);
    void SetProtocol(int nValue);

    void Bind(int nPort);
public:
    CSocket();
    virtual ~CSocket();

    virtual void Open();
    virtual void Close();

    bool GetActive() { return m_bActive; }
    int GetHandle() { return m_nHandle; }
    bool GetBlockMode() { return m_bBlockMode; }
    void SetBlockMode(bool bValue);
    void SetHandle(int nValue);
};


///////////////////////////////////////////////////////////////////////////////
// class CTcpSocket - TCP 套接字类

class CTcpSocket : public CSocket
{
public:
    CTcpSocket()
    {
        m_nType = SOCK_STREAM;
        m_nProtocol = IPPROTO_TCP;
        m_bBlockMode = false;
    }
};

///////////////////////////////////////////////////////////////////////////////
// class CDtpConnection - DTP Connection 类 (Data Transfer Protocol, including TCP and UTP)

class CDtpConnection
{
protected:
    CPeerAddress m_nRemoteAddr;
public:
    CDtpConnection() {}
    virtual ~CDtpConnection() {}

    virtual bool GetConnected() = 0;
    virtual void Disconnect() = 0;
    // 循环非阻塞发送/接收
    virtual void SendBuffer(void *pBuffer, int nSize, int nTimeoutSecs = 0) = 0;
    virtual void RecvBuffer(void *pBuffer, int nSize, int nTimeoutSecs = 0) = 0;
    // 非阻塞发送/接收
    virtual int WriteBuffer(void *pBuffer, int nSize) = 0;
    virtual int ReadBuffer(void *pBuffer, int nSize) = 0;

    // 其它发送/接收方法
    virtual string RecvString(char chTerminalChar = '\0', int nTimeoutSecs = 0);

    CPeerAddress GetRemoteAddr() { return m_nRemoteAddr; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpConnection - TCP Connection 类

class CTcpConnection : public CDtpConnection
{
protected:
    CTcpSocket m_Socket;
    bool m_bClosedGracefully;

    void CheckDisconnect(bool bThrowException);
    void ResetConnection();
public:
    CTcpConnection();
    CTcpConnection(int nSocketHandle, const CPeerAddress& RemoteAddr);
    virtual ~CTcpConnection() {}

    virtual bool GetConnected();
    virtual void Disconnect();

    // 循环非阻塞发送/接收
    virtual void SendBuffer(void *pBuffer, int nSize, int nTimeoutSecs = 0);
    virtual void RecvBuffer(void *pBuffer, int nSize, int nTimeoutSecs = 0);
    // 非阻塞发送/接收
    virtual int WriteBuffer(void *pBuffer, int nSize);
    virtual int ReadBuffer(void *pBuffer, int nSize);
	int WriteSjwBuffer(void *pBuffer, int nSize);
    int ReadSjwBuffer(void *pBuffer, int nSize);

    CTcpSocket& GetSocket() { return m_Socket; }
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpClient - TCP Client 类

class CTcpClient : public CTcpConnection
{
public:
    // 阻塞式连接
    void Connect(const string& strIp, int nPort);
    // 非阻塞式连接
    bool ConnectNonBlock(const string& strIp, int nPort);
    // 非阻塞式连接是否完成
    bool IsConnect();
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpServer - TCP Server 类

class CTcpServer
{
public:
    enum { LISTEN_QUEUE_SIZE = 15 };   // TCP监听队列长度

private:
    CTcpSocket m_Socket;
    int m_nLocalPort;
    CTcpListenerThread *m_pListenerThread;
public:
    // OnConnectEvent 的事件参数类型
    struct CConnectEventParam
    {
        CTcpConnection *pConnection;
        CConnectEventParam(CTcpConnection *pConnectionA) : pConnection(pConnectionA) {}
    };

    CEvent<CTcpServer, CConnectEventParam> OnConnectEvent;
protected:
    virtual void StartListenerThread();
    virtual void StopListenerThread();
public:
    CTcpServer();
    virtual ~CTcpServer();

    virtual void Open();
    virtual void Close();

    bool GetActive() { return m_Socket.GetActive(); }
    int GetLocalPort() { return m_nLocalPort; }
    CTcpSocket& GetSocket() { return m_Socket; }
    void SetActive(bool bValue);
    void SetLocalPort(int nValue);

    // 事件函数: 收到连接 (注: pConnection 是堆对象，需使用者释放)
    virtual void OnConnect(CTcpConnection *pConnection)
    { OnConnectEvent.Invoke(*this, CConnectEventParam(pConnection)); }
};

///////////////////////////////////////////////////////////////////////////////
// class CListenerThread - 监听线程类

class CListenerThread : public CThread
{
protected:
    virtual void Execute() {}
public:
    CListenerThread()
    {
#ifdef ICL_WIN32
        SetPriority(THREAD_PRI_HIGHEST);
#endif
#ifdef ICL_LINUX
        SetPolicy(THREAD_POL_RR);
        SetPriority(THREAD_PRI_HIGH);
#endif
    }
    virtual ~CListenerThread() {}
};

///////////////////////////////////////////////////////////////////////////////
// class CTcpListenerThread - TCP服务器监听线程类

class CTcpListenerThread : public CListenerThread
{
private:
    CTcpServer *m_pTcpServer;
protected:
    virtual void Execute();
public:
    explicit CTcpListenerThread(CTcpServer *pTcpServer);
};

///////////////////////////////////////////////////////////////////////////////
// class CDtpConnectionWarp - 封装DTP连接，方便读写数据

class CDtpConnectionWarp
{
private:
    CDtpConnection* m_pDtpConnection;
    char* m_pReadBuffer;
    int m_ReadLength;
    char* m_pWriteBuffer;
    int m_WriteLength;

    int m_DoneReadLength;
    int m_DoneWriteLength;
public:
    CDtpConnectionWarp() {} ;
    virtual ~CDtpConnectionWarp() {};

    ///////////////////////////////////////////////////////////////////////////
    // 设置参数。如果不需要读取或者写出只需要将相应的长度设置为0。
    void SetParam(CDtpConnection* pDtpConnection,
        char* pReadBuffer, int ReadLength,
        char* pWriteBuffer, int WriteLength);

    ///////////////////////////////////////////////////////////////////////////
    // 接受数据，如果接收到预期的数据则返回true
    bool IsDoneReadBuffer();

    ///////////////////////////////////////////////////////////////////////////
    // 发送数据，如果数据发送完毕返回true
    bool IsDoneWriteBuffer();

    ///////////////////////////////////////////////////////////////////////////
    // 重新设置要读取的字节数（可能已经读取了一些数据）
    void AdjustReadLength(int ReadLength);

    ///////////////////////////////////////////////////////////////////////////
    // 重新设置要写出的字节数（可能已经写出了一些数据）
    void AdjustWriteLength(int WriteLength);

    CDtpConnection* GetDtpConnection() { return m_pDtpConnection; };
};

///////////////////////////////////////////////////////////////////////////////

} // namespace icl

#endif // _ICL_SOCKET_H_ 
