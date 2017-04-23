///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// �ļ�����: icl_socket.cpp
// ��������: ����������
// ����޸�: 2005-12-18
///////////////////////////////////////////////////////////////////////////////

#include "icl_socket.h"
#include "icl_sysutils.h"

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
// �����

static int bNetworkInitCount = 0;

//-----------------------------------------------------------------------------
// ����: �����ʼ��
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
// ����: ���������
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
// ����: ȡ�����Ĵ������
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
// ����: ���ش�����Ϣ
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
// ����: ȡ��������Ķ�Ӧ��Ϣ
//-----------------------------------------------------------------------------
string IclGetLastErrMsg()
{
    return IclGetErrorMsg(IclGetLastError());
}

//-----------------------------------------------------------------------------
// ����: �ر��׽���
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
// ����: ����IP(�����ֽ�˳��) -> ����IP
//-----------------------------------------------------------------------------
string IpToString(int nIp)
{
#pragma pack(1)
    union CIpUnion
    {
        int nValue;
        struct
        {
            unsigned char ch1;  // nValue������ֽ�
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
// ����: ����IP -> ����IP(�����ֽ�˳��)
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
// ����: ��� SockAddr �ṹ
//-----------------------------------------------------------------------------
void GetSocketAddr(SockAddr& SockAddr, uint nIpHostValue, int nPort)
{
    ZeroBuffer(&SockAddr, sizeof(SockAddr));
    SockAddr.sin_family = AF_INET;
    SockAddr.sin_addr.s_addr = htonl(nIpHostValue);
    SockAddr.sin_port = htons(nPort);
}

//-----------------------------------------------------------------------------
// ����: ȡ�ÿ��ж˿ں�
// ����:
//   nProto      - ����Э��(UDP,TCP)
//   nStartPort  - ��ʼ�˿ں�
//   nCheckTimes - ������
// ����:
//   ���ж˿ں� (��ʧ���򷵻� 0)
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
// ����: ȡ�ñ���IP�б�
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
// ����: ȡ�ñ���IP
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
    // �˴���Ӧ�� bValue != m_bBlockMode ���жϣ���Ϊ�ڲ�ͬ��ƽ̨�£�
    // �׽���������ʽ��ȱʡֵ��һ����
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
// ����: ���׽���
//-----------------------------------------------------------------------------
void CSocket::Bind(int nPort)
{
    SockAddr Addr;
    int nValue = 1;

    GetSocketAddr(Addr, INADDR_ANY, nPort);

    // ǿ�����°󶨣��������������ص�Ӱ��
    setsockopt(m_nHandle, SOL_SOCKET, SO_REUSEADDR, (char*)&nValue, sizeof(int));
    // ���׽���
    if (bind(m_nHandle, (struct sockaddr*)&Addr, sizeof(Addr)) < 0)
        throw CSocketException(IclGetLastErrMsg().c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ���׽���
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
// ����: �ر��׽���
//-----------------------------------------------------------------------------
void CSocket::Close()
{
    if (m_bActive) DoClose();
}

///////////////////////////////////////////////////////////////////////////////
// class CDtpConnection

//-----------------------------------------------------------------------------
// ����: �����ַ�����ֱ���յ�ָ���Ľ�����
// ����:
//   chTerminalChar - �������ַ�
//   nTimeoutSecs   - ָ����ʱʱ��(��)��������ָ��ʱ����δ������ȫ���������׳��쳣��
//                    �� nTimeoutSecs Ϊ 0�����ʾ�����г�ʱ��⡣
// ��ע: ����������ʧ�ܣ����׳��쳣��
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
// ����: ��������
//   nTimeoutSecs - ָ����ʱʱ��(��)��������ָ��ʱ����δ������ȫ���������׳��쳣��
//                  �� nTimeoutSecs Ϊ 0�����ʾ�����г�ʱ��⡣
// ��ע: 
//   1. �˴����÷�����ģʽ���Ա��ܼ�ʱ�˳���
//   2. ����������ʧ�ܣ����׳��쳣��
//-----------------------------------------------------------------------------
void CTcpConnection::SendBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)

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
        // �趨ÿ�εȴ�ʱ��
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

        // �����Ҫ��ʱ���
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
// ����: ��������
// ����:
//   nTimeoutSecs - ָ����ʱʱ��(��)��������ָ��ʱ����δ������ȫ���������׳��쳣��
//                  �� nTimeoutSecs Ϊ 0�����ʾ�����г�ʱ��⡣
// ��ע: 
//   1. �˴����÷�����ģʽ���Ա��ܼ�ʱ�˳���
//   2. ����������ʧ�ܣ����׳��쳣��
//-----------------------------------------------------------------------------
void CTcpConnection::RecvBuffer(void *pBuffer, int nSize, int nTimeoutSecs)
{
    const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)

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
        // �趨ÿ�εȴ�ʱ��
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

        // �����Ҫ��ʱ���
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
// ����: �������� (������)
// ����: ʵ�ʷ��ͳ�ȥ�������ֽ���
// ��ע: �������������׳��쳣��
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
// ����: �������� (������)
// ����: ʵ�ʽ��յ��������ֽ���
// ��ע: �������������׳��쳣��
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
// ����: �������� (������)
// ����: ʵ�ʷ��ͳ�ȥ�������ֽ���
// ��ע: �������������׳��쳣��
//-----------------------------------------------------------------------------
int CTcpConnection::WriteSjwBuffer(void *pBuffer, int nSize)
{
	const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)
	fd_set wds;
    struct timeval tv;
	int r=-1;
	int nResult=-1;
	// �趨ÿ�εȴ�ʱ��
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
// ����: �������� (������)
// ����: ʵ�ʽ��յ��������ֽ���
// ��ע: �������������׳��쳣��
//-----------------------------------------------------------------------------
int CTcpConnection::ReadSjwBuffer(void *pBuffer, int nSize)
{
	const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)
	fd_set fds;
    struct timeval tv;
	int r=-1;
	int nResult=-1;
	// �趨ÿ�εȴ�ʱ��
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
// ����: �Ͽ�����
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
// ����: ���ص�ǰ�Ƿ�Ϊ����״̬
//-----------------------------------------------------------------------------
bool CTcpConnection::GetConnected()
{
    CheckDisconnect(false);
    return m_Socket.GetActive();
}

///////////////////////////////////////////////////////////////////////////////
// class CTcpClient

//-----------------------------------------------------------------------------
// ����: ����TCP�������� (����ʽ)
// ��ע: ������ʧ�ܣ����׳��쳣��
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
// ����: ����TCP�������� (������ʽ)
// ����:
//   true  - ���ӽ����ɹ�
//   false - �ѿ�ʼ���ӣ���δ��������
// ��ע:
//   ��ʧ�ܣ����׳��쳣��
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
//������IsConnect�ж������Ƿ����
//-----------------------------------------------------------------------------
bool CTcpClient::IsConnect()
{
     fd_set rset, wset;                                                                     //�ļ���������fd_set
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
          //������ӹ����з����˴���
          if(getsockopt(nHandle, SOL_SOCKET, SO_ERROR, (char *)&nError, &nErrorLen) < 0 || nError)
              throw CException();
          //����������
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
// ����: ���������߳�
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
// ����: ֹͣ�����߳�
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
// ����: ����TCP������
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
// ����: �ر�TCP������
//-----------------------------------------------------------------------------
void CTcpServer::Close()
{
    if (GetActive())
    {
        // ��ֹͣ�̣߳�����socketʧЧ��select����ʧ�����100%CPU��
        StopListenerThread();
        m_Socket.Close();
    }
}

//-----------------------------------------------------------------------------
// ����: ����/�ر�TCP������
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
// ����: ����TCP�����������˿�
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
// ����: TCP��������������
//-----------------------------------------------------------------------------
void CTcpListenerThread::Execute()
{
    const int SELECT_WAIT_MSEC = 250;    // ÿ�εȴ�ʱ�� (����)

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
        // �趨ÿ�εȴ�ʱ��
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
