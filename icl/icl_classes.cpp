///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// 文件名称: icl_classes.cpp
// 功能描述: 通用基础类库
// 最后修改: 2005-12-06
///////////////////////////////////////////////////////////////////////////////

#include "icl_classes.h"
#include "icl_sysutils.h"
#include "boost/regex.hpp"

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
// 全局对象

CLogger Logger;  // 日志对象 (注: 应用程序须自行初始化日志文件名)

///////////////////////////////////////////////////////////////////////////////
// 类型定义

union Int64Rec
{
    int64 value;
    struct {
        int32 lo;
        int32 hi;
    } ints;
};

///////////////////////////////////////////////////////////////////////////////
// class CException

CException::CException(const char *sErrorMsg, const char *sFileName, int nLineNo)
{
    if (sErrorMsg)
        m_strErrorMsg = sErrorMsg;
    if (sFileName)
        m_strFileName = sFileName;
    m_nLineNo = nLineNo;
}

CException::CException(const CException& src)
{
    m_strErrorMsg = src.m_strErrorMsg;
    m_strFileName = src.m_strFileName;
    m_nLineNo = src.m_nLineNo;
}

//-----------------------------------------------------------------------------
// 描述: 返回用于 Log 的字符串
//-----------------------------------------------------------------------------
string CException::MakeLogMsg() const
{
    string strText;

    strText = m_strErrorMsg;

    if (!m_strFileName.empty() && m_nLineNo != EMPTY_LINE_NO)
        strText = strText + " (" + m_strFileName + ":" + IntToStr(m_nLineNo) + ")";

    return strText;
}

///////////////////////////////////////////////////////////////////////////////
// class CLogger

//-----------------------------------------------------------------------------
// 描述: 将字符串写入文件
//-----------------------------------------------------------------------------
void CLogger::WriteToFile(const string& strString)
{
    if (m_strFileName.empty()) return;

    ofstream ofs(m_strFileName.c_str(), ios::out | ios::app);
    ofs << strString;
    ofs.close();
}

/*
//-----------------------------------------------------------------------------
// 描述: 将自定义文本写入日志
//-----------------------------------------------------------------------------
void CLogger::Write(const char* sFormatString, ...)
{
    string strText;
    uint nProcessId, nThreadId;

    va_list argList;
    va_start(argList, sFormatString);
    FormatStringV(strText, sFormatString, argList);
    va_end(argList);

#ifdef ICL_WIN32
    nProcessId = GetCurrentProcessId();
    nThreadId = GetCurrentThreadId();
#endif
#ifdef ICL_LINUX
    nProcessId = getpid();
    nThreadId = pthread_self();
#endif

    strText = FormatString("[%s](%d|%u)<%s>\r\n",
        CDateTime::CurrentDateTime().DateTimeString().c_str(),
        nProcessId, nThreadId, strText.c_str());
    WriteToFile(strText);
}
*/

//-----------------------------------------------------------------------------
// 描述: 将异常信息写入日志
//-----------------------------------------------------------------------------
void CLogger::Write(const CException& e)
{
	string strText=e.MakeLogMsg();
    uint nProcessId, nThreadId;
	#ifdef ICL_WIN32
		nProcessId = GetCurrentProcessId();
		nThreadId = GetCurrentThreadId();
	#endif
	#ifdef ICL_LINUX
		nProcessId = getpid();
		nThreadId = pthread_self();
	#endif

	strText = FormatString("[%s](%d|%u)<%s>\r\n",
		CDateTime::CurrentDateTime().DateTimeString().c_str(),
		nProcessId, nThreadId, strText.c_str());
    WriteToFile(strText);
}

///////////////////////////////////////////////////////////////////////////////
// class CBuffer

CBuffer::CBuffer()
{
    m_pBuffer = NULL;
    m_nSize = 0;
	m_strBufCharSet = "";
}

CBuffer::CBuffer(const CBuffer& src)
{
    m_pBuffer = NULL;
    m_nSize = 0;
	m_strBufCharSet = "";
    (*this) = src;
}

CBuffer::CBuffer(int nSize) 
{
    m_pBuffer = NULL;
    m_nSize = 0;
	m_strBufCharSet = "";
    SetSize(nSize);
}

CBuffer::CBuffer(const void *pBuffer, int nSize)
{
    m_pBuffer = NULL;
    m_nSize = 0;
	m_strBufCharSet = "";
    Assign(pBuffer, nSize);
}

CBuffer::~CBuffer() 
{ 
    if (m_pBuffer)
        free(m_pBuffer);
}

CBuffer& CBuffer::operator = (const CBuffer& rhs)
{
    if (this == &rhs) return *this;

    SetSize(rhs.GetSize());
    if (m_nSize > 0)
        memmove(m_pBuffer, rhs.m_pBuffer, m_nSize);
	m_strBufCharSet = rhs.m_strBufCharSet;
    return *this;
}

//-----------------------------------------------------------------------------
// 描述: 设置缓存大小
// 参数:
//   nSize       - 新缓冲区大小
//   bZeroExtBuf - 若新缓冲区比旧缓冲区大，是否将多余的空间用'\0'填充
// 备注:
//   新的缓存会保留原有内容
//-----------------------------------------------------------------------------
void CBuffer::SetSize(int nSize, bool bZeroExtBuf)
{
    if (nSize <= 0)
    {
        if (m_pBuffer) free(m_pBuffer);
        m_pBuffer = NULL;
        m_nSize = 0;
    }
    else if (nSize != m_nSize)
    {
        void *pNewBuf;

        // 如果 m_pBuffer == NULL，则 realloc 相当于 malloc。
		pNewBuf = realloc(m_pBuffer, nSize + 1);  // 多分配一个字节用于 c_str()!
        
        if (pNewBuf)
        {
            if (bZeroExtBuf && (nSize > m_nSize))
                ZeroBuffer(((char*)pNewBuf) + m_nSize, nSize - m_nSize);
            m_pBuffer = pNewBuf;
            m_nSize = nSize;
        }
        else
        {
            throw CException(IEM_OUT_OF_MEMORY, __FILE__, __LINE__);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 返回 C 风格的字符串 (末端附加结束符 '\0')
//-----------------------------------------------------------------------------
char* CBuffer::c_str() const
{
    if (m_nSize <= 0 || !m_pBuffer)
        return "";
    else
    {
        ((char*)m_pBuffer)[m_nSize] = 0;
        return (char*)m_pBuffer;
    }
}

//-----------------------------------------------------------------------------
// 描述: 将 pBuffer 中的 nSize 个字节复制到 *this 中，并将大小设置为 nSize
//-----------------------------------------------------------------------------
void CBuffer::Assign(const void *pBuffer, int nSize)
{
    SetSize(nSize);
    if (m_nSize > 0)
        memmove(m_pBuffer, pBuffer, m_nSize);
}

///////////////////////////////////////////////////////////////////////////////
// class CDateTime


CDateTime::CDateTime(const string& strTime)
{
	//?????????????????????????????????????????????????|(.*)-(.*)-(.*)\s+(.*):(.*):(.*)\s+(.*)||(.*),(.*)-(.*)-(.*)\\s+(.*):(.*):(.*)\\s+(.*)
	boost::regex re("(.*)-(.*)-(.*)\\s+(.*):(.*):(.*)\\s+(.*)",boost::regbase::normal | boost::regbase::icase);
	string strTemp = strTime;
	StringArray strVec;
	int nYear, nMonth, nDay, nHour, nMinute, nSecond;
	string strTimeFlag = "";
	boost::regex_split(back_inserter(strVec),strTemp,re);	
	if(strVec.size() == 7)
	{
		if(IsInt(strVec.at(0)))
		{
			nDay = StrToInt(strVec.at(0));	
		}
		else
		{
			nDay = 1;	
		}
		if(IsInt(strVec.at(1)))
		{
			nMonth = StrToInt(strVec.at(1));	
		}
		else
		{
			string strTemp = strVec.at(1);	
			if(strTemp.at(strTemp.length()-1) == '.')
			{
				strTemp = strTemp.substr(0,strTemp.length()-1);
			}
			if(StartSameText(strTemp,"January",false))
			{
				nMonth = 1;	
			}
			else if(StartSameText(strTemp,"February",false))
			{
				nMonth = 2;	
			}
			else if(StartSameText(strTemp,"March",false))
			{
				nMonth = 3;	
			}
			else if(StartSameText(strTemp,"April",false))
			{
				nMonth = 4;	
			}
			else if(StartSameText(strTemp,"May",false))
			{
				nMonth = 5;	
			}
			else if(StartSameText(strTemp,"June",false))
			{
				nMonth = 6;	
			}
			else if(StartSameText(strTemp,"Jul",false))
			{
				nMonth = 7;	
			}
			else if(StartSameText(strTemp,"August",false))
			{
				nMonth = 8;	
			}
			else if(StartSameText(strTemp,"September",false))
			{
				nMonth = 9;	
			}
			else if(StartSameText(strTemp,"October",false))
			{
				nMonth = 10;	
			}
			else if(StartSameText(strTemp,"November",false))
			{
				nMonth = 11;	
			}
			else if(StartSameText(strTemp,"December",false))
			{
				nMonth = 12;	
			}
			else
			{
				nMonth = 1;		
			}
		}
		if(IsInt(strVec.at(2)))
		{
			nYear = StrToInt(strVec.at(2));	
		}
		else
		{
			nYear = 1970;	
		}
		if(IsInt(strVec.at(3)))
		{
			nHour = StrToInt(strVec.at(3));	
		}
		else
		{
			nHour=0;	
		}
		if(IsInt(strVec.at(4)))
		{
			nMinute = StrToInt(strVec.at(4));	
		}
		else
		{
			nMinute = 0;	
		}
		if(IsInt(strVec.at(5)))
		{
			nSecond = StrToInt(strVec.at(5));
		}
		else
		{
			nSecond = 0;	
		}
		strTimeFlag = strVec.at(6);
		EncodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	}
	else
	{
		boost::regex rere(".*,(.*)-(.*)-(.*)\\s+(.*):(.*):(.*)\\s+(.*)",boost::regbase::normal | boost::regbase::icase);
		strTemp = strTime;
		strVec.clear();
		boost::regex_split(back_inserter(strVec),strTemp,re);	
		if(strVec.size() == 7)
		{
			if(IsInt(strVec.at(0)))
			{
				nDay = StrToInt(strVec.at(0));	
			}
			else
			{
				nDay = 1;	
			}
			if(IsInt(strVec.at(1)))
			{
				nMonth = StrToInt(strVec.at(1));	
			}
			else
			{
				string strTemp = strVec.at(1);	
				if(strTemp.at(strTemp.length()-1) == '.')
				{
					strTemp = strTemp.substr(0,strTemp.length()-1);
				}
				if(StartSameText(strTemp,"January",false))
				{
					nMonth = 1;	
				}
				else if(StartSameText(strTemp,"February",false))
				{
					nMonth = 2;	
				}
				else if(StartSameText(strTemp,"March",false))
				{
					nMonth = 3;	
				}
				else if(StartSameText(strTemp,"April",false))
				{
					nMonth = 4;	
				}
				else if(StartSameText(strTemp,"May",false))
				{
					nMonth = 5;	
				}
				else if(StartSameText(strTemp,"June",false))
				{
					nMonth = 6;	
				}
				else if(StartSameText(strTemp,"Jul",false))
				{
					nMonth = 7;	
				}
				else if(StartSameText(strTemp,"August",false))
				{
					nMonth = 8;	
				}
				else if(StartSameText(strTemp,"September",false))
				{
					nMonth = 9;	
				}
				else if(StartSameText(strTemp,"October",false))
				{
					nMonth = 10;	
				}
				else if(StartSameText(strTemp,"November",false))
				{
					nMonth = 11;	
				}
				else if(StartSameText(strTemp,"December",false))
				{
					nMonth = 12;	
				}
				else
				{
					nMonth = 1;		
				}
			}
			if(IsInt(strVec.at(2)))
			{
				nYear = StrToInt(strVec.at(2));	
			}
			else
			{
				nYear = 1970;	
			}
			if(IsInt(strVec.at(3)))
			{
				nHour = StrToInt(strVec.at(3));	
			}
			else
			{
				nHour=0;	
			}
			if(IsInt(strVec.at(4)))
			{
				nMinute = StrToInt(strVec.at(4));	
			}
			else
			{
				nMinute = 0;	
			}
			if(IsInt(strVec.at(5)))
			{
				nSecond = StrToInt(strVec.at(5));
			}
			else
			{
				nSecond = 0;	
			}
			strTimeFlag = strVec.at(6);
			EncodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
		}
		else 
		{
			 throw CException(IEM_INVALID_DATETIME_STR, __FILE__, __LINE__);
		}
	}
}

//-----------------------------------------------------------------------------
// 描述: 返回当前时间 (从1970-01-01 00:00:00 算起的秒数, UTC时间)
//-----------------------------------------------------------------------------
CDateTime CDateTime::CurrentDateTime()
{
    return CDateTime(time(NULL));
}

//-----------------------------------------------------------------------------
// 描述: 返回当前时区的当前时间戳 (从1970-01-01 00:00:00 算起的秒数)
//-----------------------------------------------------------------------------
time_t CDateTime::CurrentZoneTimeStamp()
{
    struct timeb t;
    ftime(&t);
    return t.time - t.timezone * 60;
}

//-----------------------------------------------------------------------------
// 描述: 将字符串转换成 CDateTime
// 注意: strDateTime 的格式必须为 YYYY-MM-DD HH:MM:SS
//-----------------------------------------------------------------------------
CDateTime& CDateTime::operator = (const string& strDateTime)
{
    int nYear, nMonth, nDay, nHour, nMinute, nSecond;

    if (strDateTime.length() == 19)
    {
        nYear = StrToInt(strDateTime.substr(0, 4), 0);
        nMonth = StrToInt(strDateTime.substr(5, 2), 0);
        nDay = StrToInt(strDateTime.substr(8, 2), 0);
        nHour = StrToInt(strDateTime.substr(11, 2), 0);
        nMinute = StrToInt(strDateTime.substr(14, 2), 0);
        nSecond = StrToInt(strDateTime.substr(17, 2), 0);

        EncodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
        return *this;
    }
    else
    {
        throw CException(IEM_INVALID_DATETIME_STR, __FILE__, __LINE__);
    }
}

//-----------------------------------------------------------------------------
// 描述: 日期时间编码，并存入 *this
//-----------------------------------------------------------------------------
void CDateTime::EncodeDateTime(int nYear, int nMonth, int nDay, 
    int nHour, int nMinute, int nSecond)
{
    struct tm tm;

    tm.tm_year = nYear - 1900;
    tm.tm_mon = nMonth - 1;
    tm.tm_mday = nDay;
    tm.tm_hour = nHour;
    tm.tm_min = nMinute;
    tm.tm_sec = nSecond;
    
    m_tTime = mktime(&tm);
}

//-----------------------------------------------------------------------------
// 描述: 日期时间解码，并存入各参数
//-----------------------------------------------------------------------------
void CDateTime::DecodeDateTime(int& nYear, int& nMonth, int& nDay, 
    int& nHour, int& nMinute, int& nSecond) const
{
    struct tm tm;

#ifdef ICL_WIN32
    struct tm *ptm = localtime(&m_tTime);    // TODO: 此处存在重入隐患！
    if (ptm) tm = *ptm;
#endif
#ifdef ICL_LINUX
    localtime_r(&m_tTime, &tm);
#endif

    nYear = tm.tm_year + 1900;
    nMonth = tm.tm_mon + 1;
    nDay = tm.tm_mday;
    nHour = tm.tm_hour;
    nMinute = tm.tm_min;
    nSecond = tm.tm_sec;
}

//-----------------------------------------------------------------------------
// 描述: 返回日期字符串
// 参数:
//   chDateSep - 日期分隔符
// 格式:
//   YYYY-MM-DD
//-----------------------------------------------------------------------------
string CDateTime::DateString(char chDateSep) const
{
    char sDate[11];
    int nYear, nMonth, nDay, nHour, nMinute, nSecond;

    DecodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
    sprintf(sDate, "%04d%c%02d%c%02d", nYear, chDateSep, nMonth, chDateSep, nDay);

    return string(sDate);
}
    
//-----------------------------------------------------------------------------
// 描述: 返回日期时间字符串
// 参数:
//   chDateSep - 日期分隔符
//   chTimeSep - 时间分隔符
// 格式:
//   YYYY-MM-DD HH:MM:SS
//-----------------------------------------------------------------------------
string CDateTime::DateTimeString(char chDateSep, char chTimeSep) const
{
    char sDateTime[20];
    int nYear, nMonth, nDay, nHour, nMinute, nSecond;

    DecodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
    sprintf(sDateTime, "%04d%c%02d%c%02d %02d%c%02d%c%02d", 
        nYear, chDateSep, nMonth, chDateSep, nDay,
        nHour, chTimeSep, nMinute, chTimeSep, nSecond);

    return string(sDateTime);
}

string CDateTime::DateTimeString(const string& strSep) const
{
    char sDateTime[20];
    int nYear, nMonth, nDay, nHour, nMinute, nSecond;

    DecodeDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
    if(strSep.length() <= 0)
    {
        sprintf(sDateTime, "%04d%02d%02d%02d%02d%02d",nYear,nMonth,nDay,nHour, nMinute, nSecond);
    }
    else
        sprintf(sDateTime, "%04d%s%02d%s%02d%s%02d%s%02d%s%02d",
            nYear, strSep.c_str(), nMonth, strSep.c_str(), nDay, strSep.c_str(),
            nHour, strSep.c_str(), nMinute, strSep.c_str(), nSecond);

    return string(sDateTime);
}

///////////////////////////////////////////////////////////////////////////////
// class CCriticalSection

CCriticalSection::CCriticalSection()
{
#ifdef ICL_WIN32
    InitializeCriticalSection(&m_Lock);
#endif
#ifdef ICL_LINUX
    pthread_mutexattr_t attr;

    // 锁属性说明:
    // PTHREAD_MUTEX_TIMED_NP:
    //   普通锁。同一线程内必须成对调用 Lock 和 Unlock。不可连续调用多次 Lock，否则会死锁。
    // PTHREAD_MUTEX_RECURSIVE_NP:
    //   嵌套锁。线程内可以嵌套调用 Lock，第一次生效，之后必须调用相同次数的 Unlock 方可解锁。
    // PTHREAD_MUTEX_ERRORCHECK_NP:
    //   检错锁。如果同一线程嵌套调用 Lock 则产生错误。
    // PTHREAD_MUTEX_ADAPTIVE_NP:
    //   适应锁。
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&m_Lock, &attr);
    pthread_mutexattr_destroy(&attr);
#endif
}

CCriticalSection::~CCriticalSection()
{
#ifdef ICL_WIN32
    DeleteCriticalSection(&m_Lock);
#endif
#ifdef ICL_LINUX
    // 如果在未解锁的情况下 destroy，此函数会返回错误 EBUSY。
    // 在 linux 下，即使此函数返回错误，也不会有资源泄漏。
    pthread_mutex_destroy(&m_Lock);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 加锁
//-----------------------------------------------------------------------------
void CCriticalSection::Lock()
{
#ifdef ICL_WIN32
    EnterCriticalSection(&m_Lock);
#endif
#ifdef ICL_LINUX
    pthread_mutex_lock(&m_Lock);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 解锁
//-----------------------------------------------------------------------------
void CCriticalSection::Unlock()
{
#ifdef ICL_WIN32
    LeaveCriticalSection(&m_Lock);
#endif
#ifdef ICL_LINUX
    pthread_mutex_unlock(&m_Lock);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 尝试加锁 (若已经处于加锁状态则立即返回)
// 返回:
//   true   - 加锁成功
//   false  - 失败，此锁已经处于加锁状态
//-----------------------------------------------------------------------------
bool CCriticalSection::TryLock()
{
#ifdef ICL_WIN32
    return TryEnterCriticalSection(&m_Lock);
#endif
#ifdef ICL_LINUX
    return pthread_mutex_trylock(&m_Lock) != EBUSY;
#endif
}

///////////////////////////////////////////////////////////////////////////////
// class CSeqAllocator

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   nStartId - 起始序列号
//-----------------------------------------------------------------------------
CSeqAllocator::CSeqAllocator(uint nStartId)
{
    m_nCurrentId = nStartId;
}

//-----------------------------------------------------------------------------
// 描述: 返回一个新分配的ID
//-----------------------------------------------------------------------------
uint CSeqAllocator::AllocId()
{
    CAutoSynchronizer Syncher(m_Lock);
    return m_nCurrentId++;
}

///////////////////////////////////////////////////////////////////////////////
// class CStream

int64 CStream::GetSize()
{
    int64 nPos, nResult;

    nPos = Seek(0, SO_CURRENT);
    nResult = Seek(0, SO_END);
    Seek(nPos, SO_BEGINNING);

    return nResult;
}

///////////////////////////////////////////////////////////////////////////////
// class CMemoryStream

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   nMemoryDelta - 内存增长步长 (字节数，必须是 2 的 N 次方)
//-----------------------------------------------------------------------------
CMemoryStream::CMemoryStream(int64 nMemoryDelta) :
    m_pMemory(NULL),
    m_nCapacity(0),
    m_nSize(0),
    m_nPosition(0)
{
    SetMemoryDelta(nMemoryDelta);
}

CMemoryStream::~CMemoryStream()
{
    Clear();
}

void CMemoryStream::SetMemoryDelta(int64 nNewMemoryDelta)
{
    if (nNewMemoryDelta != DEFAULT_MEMORY_DELTA)
    {
        if (nNewMemoryDelta < MIN_MEMORY_DELTA)
            nNewMemoryDelta = MIN_MEMORY_DELTA;

        // 保证 nNewMemoryDelta 是2的N次方
        for (int i = sizeof(int64) * 8 - 1; i >= 0; i--)
            if (((1 << i) & nNewMemoryDelta) != 0)
            {
                nNewMemoryDelta &= (1 << i);
                break;
            }
    }

    m_nMemoryDelta = nNewMemoryDelta;
}

void CMemoryStream::SetPointer(char* pMemory, int64 nSize)
{
    m_pMemory = pMemory;
    m_nSize = nSize;
}

void CMemoryStream::SetCapacity(int64 nNewCapacity)
{
    SetPointer(Realloc(nNewCapacity), m_nSize);
    m_nCapacity = nNewCapacity;
}

char* CMemoryStream::Realloc(int64& nNewCapacity)
{
    char* pResult;

    if (nNewCapacity > 0 && nNewCapacity != m_nSize)
        nNewCapacity = (nNewCapacity + (m_nMemoryDelta - 1)) & ~(m_nMemoryDelta - 1);
    
    pResult = m_pMemory;
    if (nNewCapacity != m_nCapacity)
    {
        if (nNewCapacity == 0)
        {
            free(m_pMemory);
            pResult = NULL;
        }
        else
        {
            if (m_nCapacity == 0)
                pResult = (char*)malloc(nNewCapacity);
            else
                pResult = (char*)realloc(m_pMemory, nNewCapacity);

            if (!pResult)
                throw CException(IEM_OUT_OF_MEMORY, __FILE__, __LINE__);
        }
    }

    return pResult;
}

//-----------------------------------------------------------------------------
// 描述: 读内存流
//-----------------------------------------------------------------------------
int CMemoryStream::Read(void *pBuffer, int nCount)
{
    int nResult = 0;

    if (m_nPosition >= 0 && nCount >= 0)
    {
        nResult = m_nSize - m_nPosition;
        if (nResult > 0)
        {
            if (nResult > nCount) nResult = nCount;
            memmove(pBuffer, m_pMemory + (uint)m_nPosition, nResult);
            m_nPosition += nResult;
        }
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 写内存流
//-----------------------------------------------------------------------------
int CMemoryStream::Write(const void *pBuffer, int nCount)
{
    int nResult = 0;
    int64 nPos;

    if (m_nPosition >= 0 && nCount >= 0)
    {
        nPos = m_nPosition + nCount;
        if (nPos > 0)
        {
            if (nPos > m_nSize)
            {
                if (nPos > m_nCapacity)
                    SetCapacity(nPos);
                m_nSize = nPos;
            }
            memmove(m_pMemory + (uint)m_nPosition, pBuffer, nCount);
            m_nPosition = nPos;
            nResult = nCount;
        }
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 内存流指针定位
//-----------------------------------------------------------------------------
int64 CMemoryStream::Seek(int64 nOffset, SeekOrigin nSeekOrigin)
{
    switch (nSeekOrigin)
    {
    case SO_BEGINNING:
        m_nPosition = nOffset;
        break;
    case SO_CURRENT:
        m_nPosition += nOffset;
        break;
    case SO_END:
        m_nPosition = m_nSize + nOffset;
        break;
    }

    return m_nPosition;
}

//-----------------------------------------------------------------------------
// 描述: 设置内存流大小
//-----------------------------------------------------------------------------
void CMemoryStream::SetSize(int64 nSize)
{
    int64 nOldPos = m_nPosition;

    SetCapacity(nSize);
    m_nSize = nSize;
    if (nOldPos > nSize) Seek(0, SO_END);
}

//-----------------------------------------------------------------------------
// 描述: 将其它流读入到内存流中
//-----------------------------------------------------------------------------
void CMemoryStream::LoadFromStream(CStream& Stream)
{
    int64 nCount;

    Stream.SetPosition(0);
    nCount = Stream.GetSize();
    SetSize(nCount);
    if (nCount != 0)
        Stream.Read(m_pMemory, nCount);
}

//-----------------------------------------------------------------------------
// 描述: 将文件读入到内存流中
//-----------------------------------------------------------------------------
void CMemoryStream::LoadFromFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
    LoadFromStream(FileStream);
}

//-----------------------------------------------------------------------------
// 描述: 将内存流保存到其它流中
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToStream(CStream& Stream)
{
    if (m_nSize != 0)
        Stream.Write(m_pMemory, m_nSize);
}

//-----------------------------------------------------------------------------
// 描述: 将内存流保存到文件中
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_CREATE);
    SaveToStream(FileStream);
}

//-----------------------------------------------------------------------------
// 描述: 清空内存流
//-----------------------------------------------------------------------------
void CMemoryStream::Clear()
{
    SetCapacity(0);
    m_nSize = 0;
    m_nPosition = 0;
}

///////////////////////////////////////////////////////////////////////////////
// class CFileStream

//-----------------------------------------------------------------------------
// 描述: 构造函数
// 参数:
//   strFileName - 文件名
//   nMode       - 文件流打开方式
//   nRights     - 文件存取权限
//-----------------------------------------------------------------------------
CFileStream::CFileStream(const string& strFileName, uint nMode, uint nRights)
{
    if (nMode == FM_CREATE)
    {
        m_nHandle = FileCreate(strFileName, nRights);
        if (m_nHandle < 0)
        {
            throw CException(FormatString(IEM_CANNOT_CREATE_FILE,
                strFileName.c_str(), SysErrorMessage(errno).c_str()).c_str(), __FILE__, __LINE__);
        }
    }
    else
    {
        m_nHandle = FileOpen(strFileName, nMode);
        if (m_nHandle < 0)
        {
            throw CException(FormatString(IEM_CANNOT_OPEN_FILE, 
                strFileName.c_str(), SysErrorMessage(errno).c_str()).c_str(), __FILE__, __LINE__);
        }
    }
}

//-----------------------------------------------------------------------------
// 描述: 析构函数
//-----------------------------------------------------------------------------
CFileStream::~CFileStream()
{
    if (m_nHandle >= 0) FileClose(m_nHandle);
}

//-----------------------------------------------------------------------------
// 描述: 创建文件
//-----------------------------------------------------------------------------
int CFileStream::FileCreate(const string& strFileName, uint nRights)
{
#ifdef ICL_WIN32
    return (int)CreateFile(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
#endif
#ifdef ICL_LINUX
    umask(0);  // 防止 nRights 被 umask 值 遮掩
    return open(strFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, nRights);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 打开文件
//-----------------------------------------------------------------------------
int CFileStream::FileOpen(const string& strFileName, uint nMode)
{
#ifdef ICL_WIN32
    uint nAccessModes[3] = {
        GENERIC_READ,
        GENERIC_WRITE,
        GENERIC_READ | GENERIC_WRITE
    };
    uint nShareModes[5] = {
        0,
        0,
        FILE_SHARE_READ,
        FILE_SHARE_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE
    };

    int nFileHandle = -1;

    if ((nMode & 3) <= FM_OPEN_READ_WRITE &&
        (nMode & 0xF0) <= FM_SHARE_DENY_NONE)
        nFileHandle = (int)CreateFile(strFileName.c_str(), nAccessModes[nMode & 3],
            nShareModes[(nMode & 0xF0) >> 4], NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    return nFileHandle;
#endif
#ifdef ICL_LINUX
    byte nShareModes[4] = {
        0,          // none
        F_WRLCK,    // FM_SHARE_EXCLUSIVE
        F_RDLCK,    // FM_SHARE_DENY_WRITE
        0           // FM_SHARE_DENY_NONE
    };

    int nFileHandle = -1;
    byte nShareMode;

    if (FileExists(strFileName) &&
        (nMode & 0x03) <= FM_OPEN_READ_WRITE &&
        (nMode & 0xF0) <= FM_SHARE_DENY_NONE)
    {
        umask(0);  // 防止 nMode 被 umask 值 遮掩
        nFileHandle = open(strFileName.c_str(), (nMode & 0x03), DEFAULT_FILE_ACCESS_RIGHTS);
        if (nFileHandle != -1)
        {
            nShareMode = ((nMode & 0xF0) >> 4);
            if (nShareModes[nShareMode] != 0)
            {
                struct flock flk;

                flk.l_type = nShareModes[nShareMode];
                flk.l_whence = SEEK_SET;
                flk.l_start = 0;
                flk.l_len = 0;

                if (fcntl(nFileHandle, F_SETLK, &flk) < 0)
                {
                    FileClose(nFileHandle);
                    nFileHandle = -1;
                }
            }
        }
    }

    return nFileHandle;
#endif
}

//-----------------------------------------------------------------------------
// 描述: 关闭文件
//-----------------------------------------------------------------------------
void CFileStream::FileClose(int nHandle)
{
#ifdef ICL_WIN32
    CloseHandle((HANDLE)nHandle);
#endif
#ifdef ICL_LINUX
    close(nHandle);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 读文件
//-----------------------------------------------------------------------------
int CFileStream::FileRead(int nHandle, void *pBuffer, int nCount)
{
#ifdef ICL_WIN32
    unsigned long nResult;
    if (!ReadFile((HANDLE)nHandle, pBuffer, nCount, &nResult, NULL))
        nResult = -1;
    return nResult;
#endif
#ifdef ICL_LINUX
    return read(nHandle, pBuffer, nCount);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 写文件
//-----------------------------------------------------------------------------
int CFileStream::FileWrite(int nHandle, const void *pBuffer, int nCount)
{
#ifdef ICL_WIN32
    unsigned long nResult;
    if (!WriteFile((HANDLE)nHandle, pBuffer, nCount, &nResult, NULL))
        nResult = -1;
    return nResult;
#endif
#ifdef ICL_LINUX
    return write(nHandle, pBuffer, nCount);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 文件指针定位
//-----------------------------------------------------------------------------
int64 CFileStream::FileSeek(int nHandle, int64 nOffset, SeekOrigin nSeekOrigin)
{
#ifdef ICL_WIN32
    int64 nResult = nOffset;
    ((Int64Rec*)&nResult)->ints.lo = SetFilePointer(
        (HANDLE)nHandle, ((Int64Rec*)&nOffset)->ints.lo,
        (PLONG)&(((Int64Rec*)&nOffset)->ints.hi), nSeekOrigin);
    return nResult;
#endif
#ifdef ICL_LINUX
    return lseek(nHandle, nOffset, nSeekOrigin);
#endif
}

//-----------------------------------------------------------------------------
// 描述: 读文件流
//-----------------------------------------------------------------------------
int CFileStream::Read(void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileRead(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 写文件流
//-----------------------------------------------------------------------------
int CFileStream::Write(const void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileWrite(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 文件流指针定位
//-----------------------------------------------------------------------------
int64 CFileStream::Seek(int64 nOffset, SeekOrigin nSeekOrigin)
{
    return FileSeek(m_nHandle, nOffset, nSeekOrigin);
}

//-----------------------------------------------------------------------------
// 描述: 设置文件流大小
//-----------------------------------------------------------------------------
void CFileStream::SetSize(int64 nSize)
{
    bool bSuccess;
    Seek(nSize, SO_BEGINNING);
    
#ifdef ICL_WIN32
    bSuccess = SetEndOfFile((HANDLE)m_nHandle);
#endif
#ifdef ICL_LINUX
    bSuccess = (ftruncate(m_nHandle, GetPosition()) == 0);
#endif

    if (!bSuccess)
        throw CException(IEM_SET_FILE_STREAM_SIZE_ERR, __FILE__, __LINE__);
}

///////////////////////////////////////////////////////////////////////////////
// class CList

CList::CList() :
    m_pList(NULL),
    m_nCount(0),
    m_nCapacity(0)
{
}

CList::~CList()
{
    Clear();
}

void CList::Grow()
{
    int nDelta;

    if (m_nCapacity > 64)
        nDelta = m_nCapacity / 4;
    else if (m_nCapacity > 8)
        nDelta = 16;
    else
        nDelta = 4;

    SetCapacity(m_nCapacity + nDelta);
}

Pointer CList::Get(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException(IEM_LIST_INDEX_ERROR, __FILE__, __LINE__);

    return m_pList[nIndex];
}

void CList::Put(int nIndex, Pointer Item)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException(IEM_LIST_INDEX_ERROR, __FILE__, __LINE__);

    m_pList[nIndex] = Item;
}

void CList::SetCapacity(int nNewCapacity)
{
    if (nNewCapacity < m_nCount)
        throw CException(IEM_LIST_CAPACITY_ERROR, __FILE__, __LINE__);

    if (nNewCapacity != m_nCapacity)
    {
        Pointer *p;
        p = (Pointer*)realloc(m_pList, nNewCapacity * sizeof(Pointer));
        if (p || nNewCapacity == 0)
        {
            m_pList = p;
            m_nCapacity = nNewCapacity;
        }
        else
        {
            throw CException(IEM_OUT_OF_MEMORY, __FILE__, __LINE__);
        }
    }
}

void CList::SetCount(int nNewCount)
{
    if (nNewCount < 0)
        throw CException(IEM_LIST_COUNT_ERROR, __FILE__, __LINE__);

    if (nNewCount > m_nCapacity)
        SetCapacity(nNewCount);
    if (nNewCount > m_nCount)
        ZeroBuffer(&m_pList[m_nCount], (nNewCount - m_nCount) * sizeof(Pointer));
    else
        for (int i = m_nCount - 1; i >= nNewCount; i--) Delete(i);

    m_nCount = nNewCount;
}

//-----------------------------------------------------------------------------
// 描述: 向列表中添加元素
//-----------------------------------------------------------------------------
void CList::Add(Pointer Item)
{
    if (m_nCount == m_nCapacity) Grow();
    m_pList[m_nCount] = Item;
    m_nCount++;
}

//-----------------------------------------------------------------------------
// 描述: 向列表中插入元素
// 参数:
//   nIndex - 插入位置下标号(0-based)
//-----------------------------------------------------------------------------
void CList::Insert(int nIndex, Pointer Item)
{
    if (nIndex < 0 || nIndex > m_nCount)
        throw CException(IEM_LIST_INDEX_ERROR, __FILE__, __LINE__);

    if (m_nCount == m_nCapacity) Grow();
    if (nIndex < m_nCount)
        MoveBuffer(&m_pList[nIndex], &m_pList[nIndex + 1], (m_nCount - nIndex) * sizeof(Pointer));
    m_pList[nIndex] = Item;
    m_nCount++;
}

//-----------------------------------------------------------------------------
// 描述: 从列表中删除元素
// 参数:
//   nIndex - 下标号(0-based)
//-----------------------------------------------------------------------------
void CList::Delete(int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException(IEM_LIST_INDEX_ERROR, __FILE__, __LINE__);

    m_nCount--;
    if (nIndex < m_nCount)
        MoveBuffer(&m_pList[nIndex + 1], &m_pList[nIndex], (m_nCount - nIndex) * sizeof(Pointer));
}

//-----------------------------------------------------------------------------
// 描述: 从列表中删除元素
// 返回: 被删除元素在列表中的下标号(0-based)，若未找到，则返回 -1.
//-----------------------------------------------------------------------------
int CList::Remove(Pointer Item)
{
    int nResult;

    nResult = IndexOf(Item);
    if (nResult >= 0)
        Delete(nResult);

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 从列表中删除元素
// 返回: 被删除的元素值，若未找到，则返回 NULL.
//-----------------------------------------------------------------------------
Pointer CList::Extract(Pointer Item)
{
    int i;
    Pointer pResult = NULL;

    i = IndexOf(Item);
    if (i >= 0)
    {
        pResult = Item;
        m_pList[i] = NULL;
        Delete(i);
    }

    return pResult;
}

//-----------------------------------------------------------------------------
// 描述: 移动一个元素到新的位置
//-----------------------------------------------------------------------------
void CList::Move(int nCurIndex, int nNewIndex)
{
    Pointer pItem;

    if (nCurIndex != nNewIndex)
    {
        if (nNewIndex < 0 || nNewIndex >= m_nCount)
            throw CException(IEM_LIST_INDEX_ERROR, __FILE__, __LINE__);

        pItem = Get(nCurIndex);
        m_pList[nCurIndex] = NULL;
        Delete(nCurIndex);
        Insert(nNewIndex, NULL);
        m_pList[nNewIndex] = pItem;
    }
}

//-----------------------------------------------------------------------------
// 描述: 改变列表的大小
//-----------------------------------------------------------------------------
void CList::Resize(int nCount)
{
    SetCount(nCount);
}

//-----------------------------------------------------------------------------
// 描述: 清空列表
//-----------------------------------------------------------------------------
void CList::Clear()
{
    SetCount(0);
    SetCapacity(0);
}

//-----------------------------------------------------------------------------
// 描述: 返回列表中的首个元素 (若列表为空则抛出异常)
//-----------------------------------------------------------------------------
Pointer CList::First()
{
    return Get(0);
}

//-----------------------------------------------------------------------------
// 描述: 返回列表中的最后元素 (若列表为空则抛出异常)
//-----------------------------------------------------------------------------
Pointer CList::Last()
{
    return Get(m_nCount - 1);
}

//-----------------------------------------------------------------------------
// 描述: 返回元素在列表中的下标号 (若未找到则返回 -1)
//-----------------------------------------------------------------------------
int CList::IndexOf(Pointer Item)
{
    int nResult = 0;

    while (nResult < m_nCount && m_pList[nResult] != Item) nResult++;
    if (nResult == m_nCount)
        nResult = -1;

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 返回列表中元素总数
//-----------------------------------------------------------------------------
int CList::Count() const
{
    return m_nCount;
}

//-----------------------------------------------------------------------------
// 描述: 存取列表中任意元素
//-----------------------------------------------------------------------------
Pointer& CList::operator [] (int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException(IEM_LIST_INDEX_ERROR, __FILE__, __LINE__);

    return m_pList[nIndex];
}

///////////////////////////////////////////////////////////////////////////////

} // namespace icl
