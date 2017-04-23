///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// 文件名称: icl_socket.cpp
// 功能描述: 网络基础类库
// 最后修改: 2005-12-18
///////////////////////////////////////////////////////////////////////////////

#include "icl_socket.h"
#include "icl_sysutils.h"

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
// 杂项函数

static int bNetworkInitCount = 0;

//-----------------------------------------------------------------------------
// 描述: 网络初始化
//-----------------------------------------------------------------------------
bool NetworkInitialize()
{
    bNetworkInitCount++;
    if (bNetworkInitCount > 1) return true;

#ifdef ICL_WIN32
    WSAData Wsd;
    return (WSAStartup(MAKEWORD(2, 2), &Wsd) == 0);
#endif
#ifdef ICL_LINUX
    return true;
#endif
}

//-----------------------------------------------------------------------------
// 描述: 网络结束化
//-----------------------------------------------------------------------------
void NetworkFinalize()
{
    if (bNetworkInitCount > 0)
        bNetworkInitCount--;
    if (bNetworkInitCount != 0) return;

#ifdef ICL_WIN32
    WSACleanup();
#endif
}

//-----------------------------------------------------------------------------
// 描述: 取得最后的错误代码
//-----------------------------------------------------------------------------
int IclGetLastError()
{
#ifdef ICL_WIN32
    return WSAGetLastError();
#endif
#ifdef ICL_LINUX
    return errno;
#endif
}

//-----------------------------------------------------------------------------
// 描述: 返回错误信息
//-----------------------------------------------------------------------------
string IclGetErrorMsg(int nError)
{
    string strResult;
    const char *p = "";

    switch (nError)
    {
    case IS_EINTR:              p = ISEM_EINTR;                break;
    case IS_EBADF:              p = ISEM_EBADF;                break;
    case IS_EACCES:             p = ISEM_EACCES;               break;
    case IS_EFAULT:             p = ISEM_EFAULT;               break;
    case IS_EINVAL:             p = ISEM_EINVAL;               break;
    case IS_EMFILE:             p = ISEM_EMFILE;               break;

    case IS_EWOULDBLOCK:        p = ISEM_EWOULDBLOCK;          break;
    case IS_EINPROGRESS:        p = ISEM_EINPROGRESS;          break;
    case IS_EALREADY:           p = ISEM_EALREADY;             break;
    case IS_ENOTSOCK:           p = ISEM_ENOTSOCK;             break;
    case IS_EDESTADDRREQ:       p = ISEM_EDESTADDRREQ;         break;
    case IS_EMSGSIZE:           p = ISEM_EMSGSIZE;             break;
    case IS_EPROTOTYPE:         p = ISEM_EPROTOTYPE;           break;
    case IS_ENOPROTOOPT:        p = ISEM_ENOPROTOOPT;          break;
    case IS_EPROTONOSUPPORT:    p = ISEM_EPROTONOSUPPORT;      break;
    case IS_ESOCKTNOSUPPORT:    p = ISEM_ESOCKTNOSUPPORT;      break;
    case IS_EOPNOTSUPP:         p = ISEM_EOPNOTSUPP;           break;
    case IS_EPFNOSUPPORT:       p = ISEM_EPFNOSUPPORT;         break;
    case IS_EAFNOSUPPORT:       p = ISEM_EAFNOSUPPORT;         break;
    case IS_EADDRINUSE:         p = ISEM_EADDRINUSE;           break;
    case IS_EADDRNOTAVAIL:      p = ISEM_EADDRNOTAVAIL;        break;
    case IS_ENETDOWN:           p = ISEM_ENETDOWN;             break;
    case IS_ENETUNREACH:        p = ISEM_ENETUNREACH;          break;
    case IS_ENETRESET:          p = ISEM_ENETRESET;            break;
    case IS_ECONNABORTED:       p = ISEM_ECONNABORTED;         break;
    case IS_ECONNRESET:         p = ISEM_ECONNRESET;           break;
    case IS_ENOBUFS:            p = ISEM_ENOBUFS;              break;
    case IS_EISCONN:            p = ISEM_EISCONN;              break;
    case IS_ENOTCONN:           p = ISEM_ENOTCONN;             break;
    case IS_ESHUTDOWN:          p = ISEM_ESHUTDOWN;            break;
    case IS_ETOOMANYREFS:       p = ISEM_ETOOMANYREFS;         break;
    case IS_ETIMEDOUT:          p = ISEM_ETIMEDOUT;            break;
    case IS_ECONNREFUSED:       p = ISEM_ECONNREFUSED;         break;
    case IS_ELOOP:              p = ISEM_ELOOP;                break;
    case IS_ENAMETOOLONG:       p = ISEM_ENAMETOOLONG;         break;
    case IS_EHOSTDOWN:          p = ISEM_EHOSTDOWN;            break;
    case IS_EHOSTUNREACH:       p = ISEM_EHOSTUNREACH;         break;
    case IS_ENOTEMPTY:          p = ISEM_ENOTEMPTY;            break;
    }
    
    FormatString(strResult, ISEM_ERROR, nError, p);
    return strResult;
}

//-----------------------------------------------------------------------------
// 描述: 取得最后错误的对应信息
//-----------------------------------------------------------------------------
string IclGetLastErrMsg()
{
    return IclGetErrorMsg(IclGetLastError());
}

//-----------------------------------------------------------------------------
// 描述: 关闭套接字
//-----------------------------------------------------------------------------
void IclCloseSocket(int nHandle)
{
#ifdef ICL_WIN32
    closesocket(nHandle);
#endif
#ifdef ICL_LINUX
    close(nHandle);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 整形IP(主机字节顺序) -> 串型IP
//-----------------------------------------------------------------------------
string IpToString(int nIp)
{
#pragma pack(1)
    union CIpUnion
    {
        int nValue;
        struct
        {
            unsigned char ch1;  // nValue的最低字节
            unsigned char ch2;
            unsigned char ch3;
            unsigned char ch4;
        } Bytes;
    } IpUnion;
#pragma pack()
    char strString[64];

    IpUnion.nValue = nIp;
    sprintf(strString, "%u.%u.%u.%u", IpUnion.Bytes.ch4, IpUnion.Bytes.ch3, 
        IpUnion.Bytes.ch2, IpUnion.Bytes.ch1);
    return &strString[0];
}

//-----------------------------------------------------------------------------
// 描述: 串型IP -> 整形IP(主机字节顺序)
//-----------------------------------------------------------------------------
int StringToIp(const string& strString)
{
#pragma pack(1)
    union CIpUnion
    {
        int nValue;
        struct
        {
            unsigned char ch1;
            unsigned char ch2;
            unsigned char ch3;
            unsigned char ch4;
        } Bytes;
    } IpUnion;
#pragma pack()
    IntegerArray IntList;

    SplitStringToInt(strString, '.', IntList);
    if (IntList.size() == 4)
    {
        IpUnion.Bytes.ch1 = IntList[3];
        IpUnion.Bytes.ch2 = IntList[2];
        IpUnion.Bytes.ch3 = IntList[1];
        IpUnion.Bytes.ch4 = IntList[0];
        return IpUnion.nValue;
    }
    else
        return 0;
}

//-----------------------------------------------------------------------------
// 描述: 填充 SockAddr 结构
//-----------------------------------------------------------------------------
void GetSocketAddr(SockAddr& SockAddr, uint nIpHostValue, int nPort)
{
    ZeroBuffer(&SockAddr, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = htonl(nIpHostValue);
    SockAddr.sin_port = htons(nPort);
}

//-----------------------------------------------------------------------------
// 描述: 取得空闲端口号
// 参数:
//   nProto      - 网络协议(UDP,TCP)
//   nStartPort  - 起始端口号
//   nCheckTimes - 检测次数
// 返回:
//   空闲端口号 (若失败则返回 0)
//-----------------------------------------------------------------------------
int GetFreePort(NetProtoType nProto, int nStartPort, int nCheckTimes)
{
    int i, s, nResult = 0;
    bool bSuccess;
    SockAddr Addr;

    if (!NetworkInitialize()) return 0;
    struct CAutoFinalizer {
        ~CAutoFinalizer() { NetworkFinalize(); }
    } AutoFinalizer;

    s = socket(PF_INET, (nProto == NPT_UDP? SOCK_DGRAM : SOCK_STREAM), IPPROTO_IP);
    if (s == -1) return 0;

    bSuccess = false;
    for (i = 0; i < nCheckTimes; i++)
    {
        nResult = nStartPort + i;
        GetSocketAddr(Addr, INADDR_ANY, nResult);
        if (bind(s, (struct sockaddr*)&Addr, sizeof(Addr)) != -1)
        {
            bSuccess = true;
            break;
        }
    }

    IclCloseSocket(s);
    if (!bSuccess) nResult = 0;
    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 取得本机IP列表
//-----------------------------------------------------------------------------
void GetLocalIpList(StringArray& IpList)
{
#ifdef ICL_WIN32
    char sHostName[250];
    hostent *pHostEnt;
    in_addr **addr_ptr;

    IpList.clear();
    gethostname(sHostName, sizeof(sHostName));
    pHostEnt = gethostbyname(sHostName);
    if (pHostEnt)
    {
        addr_ptr = (in_addr**)(pHostEnt->h_addr_list);
        int i = 0;
        while (addr_ptr[i])
        {
            uint nIp = ntohl( *(uint*)(addr_ptr[i]) );
            IpList.push_back(IpToString(nIp));
            i++;
        }
    }
#endif
#ifdef ICL_LINUX
    const int MAX_INTERFACES = 16;
    int nFd, nIntfCount;
    struct ifreq buf[MAX_INTERFACES];
    struct ifconf ifc;

    IpList.clear();
    if ((nFd = socket(AF_INET, SOCK_DGRAM, 0)) >= 0)
    {
        ifc.ifc_len = sizeof(buf);
        ifc.ifc_buf = (caddr_t) buf;
        if (!ioctl(nFd, SIOCGIFCONF, (char*)&ifc))
        {
            nIntfCount = ifc.ifc_len / sizeof(struct ifreq);
            for (int i = 0; i < nIntfCount; i++)
            {
                ioctl(nFd, SIOCGIFADDR, (char*)&buf[i]);
                uint nIp = ((struct sockaddr_in*)(&buf[i].ifr_addr))->sin_addr.s_addr;
                IpList.push_back(IpToString(ntohl(nIp)));
            }
        }
        close(nFd);
    }
#endif
}

//-----------------------------------------------------------------------------
// 描述: 取得本机IP
//-----------------------------------------------------------------------------
string GetLocalIp()
{
    StringArray IpList;
    string strResult;

    GetLocalIpList(IpList);
    if (!IpList.empty())
    {
        if (IpList.size() == 1)
            strResult = IpList[0];
        else
        {
            for (uint i = 0; i < IpList.size(); i++)
                if (IpList[i] != "127.0.0.1")
                {
                    strResult = IpList[i];
                    break;
                }

            if (strResult.length() == 0)
                strResult = IpList[0];
        }
    }

    return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// class CSocket

CSocket::CSocket() :
    m_bActive(false),
    m_nHandle(-1),
    m_nDomain(PF_INET),
    m_nType(SOCK_STREAM),
    m_nProtocol(IPPROTO_IP),
    m_bBlockMode(true)
{
}

CSocket::~CSocket()
{
    Close();
}

void CSocket::DoSetBlockMode(int nHandle, bool bValue)
{
#ifdef ICL_WIN32
    uint nNotBlock = (bValue? 0 : 1);
    if (ioctlsocket(nHandle, FIONBIO, (u_long*)&nNotBlock) < 0)
        throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
#endif
#ifdef ICL_LINUX
    int nFlag = fcntl(nHandle, F_GETFL);

    if (bValue)
        nFlag &= ~O_NONBLOCK;
    else
        nFlag |= O_NONBLOCK;

    if (fcntl(nHandle, F_SETFL, nFlag) < 0)
        throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
#endif
}

void CSocket::DoClose()
{
    shutdown(m_nHandle, IS_SD_BOTH);
    IclCloseSocket(m_nHandle);
    m_nHandle = -1;
    m_bActive = false;
}

void CSocket::SetActive(bool bValue)
{
    if (bValue != m_bActive)
    {
        if (bValue) Open();
        else Close();
    }
}

void CSocket::SetDomain(int nValue)
{
    if (nValue != m_nDomain)
    {
        if (GetActive()) Close();
        m_nDomain = nValue;
    }
}

void CSocket::SetType(int nValue)
{
    if (nValue != m_nType)
    {
        if (GetActive()) Close();
        m_nType = nValue;
    }
}

void CSocket::SetProtocol(int nValue)
{
    if (nValue != m_nProtocol)
    {
        if (GetActive()) Close();
        m_nProtocol = nValue;
    }
}

void CSocket::SetBlockMode(bool bValue)
{
    // 此处不应作 bValue != m_bBlockMode 的判断，因为在不同的平台下，
    // 套接字阻塞方式的缺省值不一样。
    if (m_bActive)
        DoSetBlockMode(m_nHandle, bValue);
    m_bBlockMode = bValue;
}

void CSocket::SetHandle(int nValue)
{
    if (nValue != m_nHandle)
    {
        if (GetActive()) Close();
        m_nHandle = nValue;
        if (m_nHandle != -1) 
            m_bActive = true;
    }
}

//-----------------------------------------------------------------------------
// 描述: 绑定套接字
//-----------------------------------------------------------------------------
void CSocket::Bind(int nPort)
{
    SockAddr Addr;
    int nValue = 1;

    GetSocketAddr(Addr, INADDR_ANY, nPort);

    // 强制重新绑定，而不受其它因素的影响
    setsockopt(m_nHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&nValue, sizeof(int));
    // 绑定套接字
    if (bind(m_nHandle, (struct sockaddr*)&Addr, sizeof(Addr)) < 0)
        throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 打开套接字
//-----------------------------------------------------------------------------
void CSocket::Open()
{
    if (!m_bActive)
    {
        try
        {
            int nHandle;
            nHandle = socket(m_nDomain, m_nType, m_nProtocol);
            if (nHandle == -1)
                throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
            m_bActive = (nHandle != -1);
            if (m_bActive)
            {
                m_nHandle = nHandle;
                SetBlockMode(m_bBlockMode);
            }
        }
        catch (CSocketException& e)
        {
            DoClose();
            throw e;
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 关闭套接字
//-----------------------------------------------------------------------------
void CSocket::Close()
{
    if (m_bActive) DoClose();
}

///////////////////////////////////////////////////////////////////////////////
// class CDtpConnection

//-----------------------------------------------------------------------------
// 描述: 接收字符串，直到收到指定的结束符
// 参数:
//   chTerminalChar - 结束符字符
//   nTimeoutSecs   - 指定超时时间(秒)，若超过指定时间仍未接收完全部数据则抛出异常。
//                    若 nTimeoutSecs 为 0，则表示不进行超时检测。
// 备注: 若接收数据失败，则抛出异常。
//-----------------------------------------------------------------------------
string CDtpConnection::RecvString(char chTerminalChar, int nTimeoutSecs)
{
    string strResult;
    char ch;

    while (true)
    {
        RecvBuffer(&ch, 1, nTimeoutSecs);
        if (ch == chTerminalChar) break;
        strResult += ch;
    }

    return strResult;
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpConnection

CTcpConnection::CTcpConnection()
{
    m_bClosedGracefully = false;
    m_Socket.SetBlockMode(false);
}

CTcpConnection::CTcpConnection(int nSocketHandle, const CPeerAddress& RemoteAddr)
{
    m_bClosedGracefully = false;

    m_Socket.SetHandle(nSocketHandle) ;
    m_Socket.SetBlockMode(false);
    m_nRemoteAddr = RemoteAddr;
}

void CTcpConnection::CheckDisconnect(bool bThrowException)
{
    bool bDisconnected;

    if (m_bClosedGracefully)
    {
        Disconnect();
        bDisconnected = true;
    }
    else
        bDisconnected = !m_Socket.GetActive();

    if (bDisconnected && bThrowException)
        throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
}

void CTcpConnection::ResetConnection()
{
    m_bClosedGracefully = false;
}

//-----------------------------------------------------------------------------
// 描述: 发送数据
//   nTimeoutSecs - 指定超时时间(秒)，若超过指定时间仍未接收完全部数据则抛出异常。
//                  若 nTimeoutSecs 为 0，则表示不进行超时检测。
// 备注: 
//   1. 此处采用非阻塞模式，以便能及时退出。
//   2. 若接收数据失败，则抛出异常。
//-----------------------------------------------------------------------------
void CTcpConnection::SendBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)

    fd_set fds;
    struct timeval tv;
    int nSocketHandle = m_Socket.GetHandle();
    int n, r, nRemain, nIndex;
    uint nStartTime, nElapsedSecs;

    if (nSize <= 0) return;
    if (!m_Socket.GetActive())
        throw CSocketException(ISEM_SOCKETERROR, __FILE__, __LINE__);

    nRemain = nSize;
    nIndex = 0;
    nStartTime = time(NULL);

    while(m_Socket.GetActive() && nRemain > 0)
    {
        // 设定每次等待时间
        tv.tv_sec = 0;
        tv.tv_usec = SELECT_WAIT_MSEC * 1000;

        FD_ZERO(&fds);
        FD_SET((uint)nSocketHandle, &fds);
        
        r = select(nSocketHandle + 1, NULL, &fds, NULL, &tv);
        if (r < 0)
        {
            if(IclGetLastError() == IS_EINTR) continue;
            else throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
        }

        if (r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &fds))
        {
            n = send(nSocketHandle, &((char*)pBuffer)[nIndex], nRemain, 0);
            if (n < 0)
            {
                if (IclGetLastError() != IS_EWOULDBLOCK)
                    m_bClosedGracefully = true;  // error
                else
                    n = 0;
            }
            CheckDisconnect(true);

            nIndex += n;
            nRemain -= n;
        }

        // 如果需要超时检测
        if (nTimeoutSecs > 0 && nRemain > 0)
        {
            nElapsedSecs = time(NULL) - nStartTime;
            if (nElapsedSecs >= (uint)nTimeoutSecs)
                throw CSocketException(ISEM_TCPSENDTIMEOUT, __FILE__, __LINE__);
        }
    }

    if (nRemain > 0)
        throw CSocketException(ISEM_SOCKETERROR, __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 接收数据
// 参数:
//   nTimeoutSecs - 指定超时时间(秒)，若超过指定时间仍未接收完全部数据则抛出异常。
//                  若 nTimeoutSecs 为 0，则表示不进行超时检测。
// 备注: 
//   1. 此处采用非阻塞模式，以便能及时退出。
//   2. 若接收数据失败，则抛出异常。
//-----------------------------------------------------------------------------
void CTcpConnection::RecvBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)

    fd_set fds;
    struct timeval tv;
    int nSocketHandle = m_Socket.GetHandle();
    int n, r, nRemain, nIndex;
    uint nStartTime, nElapsedSecs;

    if (nSize <= 0) return;
    if (!m_Socket.GetActive())
        throw CSocketException(ISEM_SOCKETERROR, __FILE__, __LINE__);

    nRemain = nSize;
    nIndex = 0;
    nStartTime = time(NULL);

    while(m_Socket.GetActive() && nRemain > 0)
    {
        // 设定每次等待时间
        tv.tv_sec = 0;
        tv.tv_usec = SELECT_WAIT_MSEC * 1000;

        FD_ZERO(&fds);
        FD_SET((uint)nSocketHandle, &fds);
        
        r = select(nSocketHandle + 1, &fds, NULL, NULL, &tv);
        if (r < 0)
        {
            if(IclGetLastError() == IS_EINTR) continue;
            else throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
        }

        if (r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &fds))
        {
            n = recv(nSocketHandle, &((char*)pBuffer)[nIndex], nRemain, 0);
            if (n < 0)
            {
                if (IclGetLastError() != IS_EWOULDBLOCK)
                    m_bClosedGracefully = true;  // error
                else
                    n = 0;
            }
            CheckDisconnect(true);

            nIndex += n;
            nRemain -= n;
        }

        // 如果需要超时检测
        if (nTimeoutSecs > 0 && nRemain > 0)
        {
            nElapsedSecs = time(NULL) - nStartTime;
            if (nElapsedSecs >= (uint)nTimeoutSecs)
                throw CSocketException(ISEM_TCPRECVTIMEOUT, __FILE__, __LINE__);
        }
    }

    if (nRemain > 0)
        throw CSocketException(ISEM_SOCKETERROR, __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 发送数据 (非阻塞)
// 返回: 实际发送出去的数据字节数
// 备注: 若发生错误，则抛出异常。
//-----------------------------------------------------------------------------
int CTcpConnection::WriteBuffer(void *pBuffer, int nSize)
{
    int nResult = send(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
    if (nResult < 0)
    {
        if (IclGetLastError() != IS_EWOULDBLOCK)
            m_bClosedGracefully = true;  // error
        else
            nResult = 0;
    }

    CheckDisconnect(true);
    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 接收数据 (非阻塞)
// 返回: 实际接收到的数据字节数
// 备注: 若发生错误，则抛出异常。
//-----------------------------------------------------------------------------
int CTcpConnection::ReadBuffer(void *pBuffer, int nSize)
{
    int nResult = recv(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
    if (nResult < 0)
    {
        if (IclGetLastError() != IS_EWOULDBLOCK)
            m_bClosedGracefully = true;  // error
        else
            nResult = 0;
    }
    CheckDisconnect(true);
    return nResult;
}


//-----------------------------------------------------------------------------
// 描述: 发送数据 (非阻塞)
// 返回: 实际发送出去的数据字节数
// 备注: 若发生错误，则抛出异常。
//-----------------------------------------------------------------------------
int CTcpConnection::WriteSjwBuffer(void *pBuffer, int nSize)
{
	const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)
	fd_set wds;
    struct timeval tv;
	int r=-1;
	int nResult=-1;
	// 设定每次等待时间
    tv.tv_sec = 0;
    tv.tv_usec =SELECT_WAIT_MSEC * 1000;
	
	uint nSocketHandle = m_Socket.GetHandle();
    FD_ZERO(&wds);
    FD_SET((uint)nSocketHandle, &wds);
        
    r = select(nSocketHandle + 1, NULL, &wds, NULL, &tv);
	if (r < 0)
	{
		if(IclGetLastError() == IS_EINTR) return -2;
		else throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
	}

	if (r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &wds))
	{
		nResult = send(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
		if (nResult < 0)
		{
			if (IclGetLastError() != IS_EWOULDBLOCK)
				m_bClosedGracefully = true;  // error
			else
				nResult = -1;
		}
		CheckDisconnect(true);
	}
    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 接收数据 (非阻塞)
// 返回: 实际接收到的数据字节数
// 备注: 若发生错误，则抛出异常。
//-----------------------------------------------------------------------------
int CTcpConnection::ReadSjwBuffer(void *pBuffer, int nSize)
{
	const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)
	fd_set fds;
    struct timeval tv;
	int r=-1;
	int nResult=-1;
	// 设定每次等待时间
    tv.tv_sec = 0;
    tv.tv_usec = SELECT_WAIT_MSEC * 1000;
	
	uint nSocketHandle = m_Socket.GetHandle();
    FD_ZERO(&fds);
    FD_SET((uint)nSocketHandle, &fds);
        
    r = select(nSocketHandle + 1, &fds, NULL, NULL, &tv);
    if (r < 0)
    {
        if(IclGetLastError() == IS_EINTR) return -2;
        else throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
    }
	else if( r > 0 && m_Socket.GetActive() && FD_ISSET(nSocketHandle, &fds))
	{
		nResult = recv(m_Socket.GetHandle(), (char*)pBuffer, nSize, 0);
		if (nResult < 0)
		{
			if (IclGetLastError() != IS_EWOULDBLOCK)
				m_bClosedGracefully = true;  // error
			else nResult =-1;
		}
		CheckDisconnect(true);
	}
    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 断开连接
//-----------------------------------------------------------------------------
void CTcpConnection::Disconnect()
{
    if (m_Socket.GetActive())
    {
        m_Socket.Close();
        m_bClosedGracefully = true;
    }
}

//-----------------------------------------------------------------------------
// 描述: 返回当前是否为连接状态
//-----------------------------------------------------------------------------
bool CTcpConnection::GetConnected()
{
    CheckDisconnect(false);
    return m_Socket.GetActive();
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpClient

//-----------------------------------------------------------------------------
// 描述: 发起TCP连接请求 (阻塞式)
// 备注: 若连接失败，则抛出异常。
//-----------------------------------------------------------------------------
void CTcpClient::Connect(const string& strIp, int nPort)
{
    if (GetConnected()) Disconnect();

    try
    {
        m_Socket.Open();
        if (m_Socket.GetActive())
        {
            SockAddr Addr;

            ResetConnection();
            GetSocketAddr(Addr, StringToIp(strIp), nPort);

            bool bOldBlockMode = m_Socket.GetBlockMode();
            m_Socket.SetBlockMode(true);

            if (connect(m_Socket.GetHandle(), (struct sockaddr*)&Addr, sizeof(Addr)) < 0)
                throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);

            m_Socket.SetBlockMode(bOldBlockMode);
            m_nRemoteAddr = CPeerAddress(ntohl(Addr.sin_addr.s_addr), nPort);
        }
    }
    catch (CSocketException& e)
    {
        m_Socket.Close();
        throw e;
    }
}

//-----------------------------------------------------------------------------
// 描述: 发起TCP连接请求 (非阻塞式)
// 返回:
//   true  - 连接建立成功
//   false - 已开始连接，尚未发生错误
// 备注:
//   若失败，则抛出异常。
//-----------------------------------------------------------------------------
bool CTcpClient::ConnectNonBlock(const string& strIp, int nPort)
{
    bool bResult = false;

    if (GetConnected()) Disconnect();
    try
    {
        m_Socket.Open();
        if (m_Socket.GetActive())
        {
            SockAddr Addr;
            int r;

            ResetConnection();
            GetSocketAddr(Addr, StringToIp(strIp), nPort);
            m_Socket.SetBlockMode(false);
            r = connect(m_Socket.GetHandle(), (struct sockaddr*)&Addr, sizeof(Addr));
            if (r == 0)
                bResult = true;
#ifdef ICL_WIN32
            else if (IclGetLastError() != IS_EWOULDBLOCK)
#endif
#ifdef ICL_LINUX
            else if (IclGetLastError() != IS_EINPROGRESS)
#endif
                throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);

            m_nRemoteAddr = CPeerAddress(ntohl(Addr.sin_addr.s_addr), nPort);
        }
    }
    catch (CSocketException& e)
    {
        m_Socket.Close();
        throw e;
    }

    return bResult;
}

//-----------------------------------------------------------------------------
//描述：IsConnect判断连接是否完成
//-----------------------------------------------------------------------------
bool CTcpClient::IsConnect()
{
     fd_set rset, wset;                                                                     //文件描述符集fd_set
     struct timeval tv;
     int r;
     int nHandle = GetSocket().GetHandle();
     tv.tv_sec = 0;  tv.tv_usec = 0;
     FD_ZERO(&rset);
     FD_SET((uint)nHandle, &rset);
     wset = rset;

     r = select(nHandle + 1, &rset,&wset,NULL,&tv);
     if(r > 0 && (FD_ISSET(nHandle, &rset) || FD_ISSET(nHandle, &wset)))
     {
          socklen_t nErrorLen = sizeof(int);
          int nError = 0;
          //如果连接过程中发生了错误
          if(getsockopt(nHandle, SOL_SOCKET, SO_ERROR, (char *)&nError, &nErrorLen) < 0 || nError)
              throw CException();
          //如果连接完成
          return true;
     }
     return false;
}



///////////////////////////////////////////////////////////////////////////////
// class CTcpServer

CTcpServer::CTcpServer() :
    m_nLocalPort(0),
    m_pListenerThread(NULL)
{
}

CTcpServer::~CTcpServer()
{
    Close();
}

//-----------------------------------------------------------------------------
// 描述: 启动监听线程
//-----------------------------------------------------------------------------
void CTcpServer::StartListenerThread()
{
    if (!m_pListenerThread)
    {
        m_pListenerThread = new CTcpListenerThread(this);
        m_pListenerThread->Run();
    }
}

//-----------------------------------------------------------------------------
// 描述: 停止监听线程
//-----------------------------------------------------------------------------
void CTcpServer::StopListenerThread()
{
    if (m_pListenerThread)
    {
        m_pListenerThread->Terminate();
        m_pListenerThread->WaitFor();
        delete m_pListenerThread;
        m_pListenerThread = NULL;
    }
}

//-----------------------------------------------------------------------------
// 描述: 开启TCP服务器
//-----------------------------------------------------------------------------
void CTcpServer::Open()
{
    try
    {
        if (!GetActive())
        {
            m_Socket.Open();
            m_Socket.Bind(m_nLocalPort);
            if (listen(m_Socket.GetHandle(), LISTEN_QUEUE_SIZE) < 0)
                throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
            StartListenerThread();
        }
    }
    catch (CSocketException& e)
    {
        Close();
        throw e;
    }
}

//-----------------------------------------------------------------------------
// 描述: 关闭TCP服务器
//-----------------------------------------------------------------------------
void CTcpServer::Close()
{
    if (GetActive())
    {
        // 先停止线程，以免socket失效后，select函数失灵造成100%CPU。
        StopListenerThread();
        m_Socket.Close();
    }
}

//-----------------------------------------------------------------------------
// 描述: 开启/关闭TCP服务器
//-----------------------------------------------------------------------------
void CTcpServer::SetActive(bool bValue)
{
    if (GetActive() != bValue)
    {
        if (bValue) Open();
        else Close();
    }
}

//-----------------------------------------------------------------------------
// 描述: 设置TCP服务器监听端口
//-----------------------------------------------------------------------------
void CTcpServer::SetLocalPort(int nValue)
{
    if (nValue != m_nLocalPort)
    {
        if (GetActive()) Close();
        m_nLocalPort = nValue;
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpListenerThread

CTcpListenerThread::CTcpListenerThread(CTcpServer *pTcpServer) :
    m_pTcpServer(pTcpServer)
{
    SetFreeOnTerminate(false);
}

//-----------------------------------------------------------------------------
// 描述: TCP服务器监听工作
//-----------------------------------------------------------------------------
void CTcpListenerThread::Execute()
{
    const int SELECT_WAIT_MSEC = 250;    // 每次等待时间 (毫秒)

    fd_set fds;
    struct timeval tv;
    SockAddr Addr;
    socklen_t nSockLen = sizeof(Addr);
    int nSocketHandle = m_pTcpServer->GetSocket().GetHandle();
    CPeerAddress RemoteAddr;
    int r, nAcceptHandle;

    while (!GetTerminated() && m_pTcpServer->GetActive())
    try
    {
        // 设定每次等待时间
        tv.tv_sec = 0;
        tv.tv_usec = SELECT_WAIT_MSEC * 1000;

        FD_ZERO(&fds);
        FD_SET((uint)nSocketHandle, &fds);

        r = select(nSocketHandle + 1, &fds, NULL, NULL, &tv);

        if (r > 0 && m_pTcpServer->GetActive() && FD_ISSET(nSocketHandle, &fds))
        {
            nAcceptHandle = accept(nSocketHandle, (struct sockaddr*)&Addr, &nSockLen);
            if (nAcceptHandle != -1)
            {
                RemoteAddr = CPeerAddress(ntohl(Addr.sin_addr.s_addr), ntohs(Addr.sin_port));
                m_pTcpServer->OnConnect(new CTcpConnection(nAcceptHandle, RemoteAddr));
            }
        }
    }
    catch (CException& e)
    { ; }
}

///////////////////////////////////////////////////////////////////////////////
// class CDtpConnectionWarp

void CDtpConnectionWarp::SetParam(CDtpConnection* pDtpConnection,
    char* pReadBuffer, int ReadLength,
    char* pWriteBuffer, int WriteLength)
{
    m_pDtpConnection = pDtpConnection;
    m_pReadBuffer = pReadBuffer;
    m_ReadLength = ReadLength;
    m_pWriteBuffer = pWriteBuffer;
    m_WriteLength = WriteLength;

    m_DoneReadLength = 0;
    m_DoneWriteLength = 0;
}

bool CDtpConnectionWarp::IsDoneReadBuffer()
{
    if (m_ReadLength && (m_pReadBuffer) && (m_DoneReadLength < m_ReadLength))
    {
        m_DoneReadLength += m_pDtpConnection->ReadBuffer(
            m_pReadBuffer + m_DoneReadLength,
            m_ReadLength - m_DoneReadLength);
    } else return true;
    
    if (m_ReadLength && (m_pReadBuffer) && (m_DoneReadLength < m_ReadLength))
        return false;
    else
        return true;
}

bool CDtpConnectionWarp::IsDoneWriteBuffer()
{
    if (m_WriteLength && (m_pWriteBuffer) && (m_DoneWriteLength < m_WriteLength))
    {
        m_DoneWriteLength += m_pDtpConnection->WriteBuffer(
            m_pWriteBuffer + m_DoneWriteLength,
            m_WriteLength - m_DoneWriteLength);
    } else return true;

    if (m_WriteLength && (m_pWriteBuffer) && (m_DoneWriteLength < m_WriteLength))
        return false;
    else
        return true;
}

void CDtpConnectionWarp::AdjustReadLength(int ReadLength)
{
    m_ReadLength = ReadLength;
}

void CDtpConnectionWarp::AdjustWriteLength(int WriteLength)
{
    m_WriteLength = WriteLength;
}

///////////////////////////////////////////////////////////////////////////////

} // namespace icl
