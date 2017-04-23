///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// �ļ�����: icl_classes.cpp
// ��������: ͨ�û������
// ����޸�: 2005-12-06
///////////////////////////////////////////////////////////////////////////////

#include "icl_classes.h"
#include "icl_sysutils.h"
#include "boost/regex.hpp"

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
// ȫ�ֶ���

CLogger Logger;  // ��־���� (ע: Ӧ�ó��������г�ʼ����־�ļ���)

///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

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
// ����: �������� Log ���ַ���
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
// ����: ���ַ���д���ļ�
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
// ����: ���Զ����ı�д����־
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
// ����: ���쳣��Ϣд����־
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
// ����: ���û����С
// ����:
//   nSize       - �»�������С
//   bZeroExtBuf - ���»������Ⱦɻ��������Ƿ񽫶���Ŀռ���'\0'���
// ��ע:
//   �µĻ���ᱣ��ԭ������
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

        // ��� m_pBuffer == NULL���� realloc �൱�� malloc��
		pNewBuf = realloc(m_pBuffer, nSize + 1);  // �����һ���ֽ����� c_str()!
        
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
// ����: ���� C �����ַ��� (ĩ�˸��ӽ����� '\0')
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
// ����: �� pBuffer �е� nSize ���ֽڸ��Ƶ� *this �У�������С����Ϊ nSize
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
// ����: ���ص�ǰʱ�� (��1970-01-01 00:00:00 ���������, UTCʱ��)
//-----------------------------------------------------------------------------
CDateTime CDateTime::CurrentDateTime()
{
    return CDateTime(time(NULL));
}

//-----------------------------------------------------------------------------
// ����: ���ص�ǰʱ���ĵ�ǰʱ��� (��1970-01-01 00:00:00 ���������)
//-----------------------------------------------------------------------------
time_t CDateTime::CurrentZoneTimeStamp()
{
    struct timeb t;
    ftime(&t);
    return t.time - t.timezone * 60;
}

//-----------------------------------------------------------------------------
// ����: ���ַ���ת���� CDateTime
// ע��: strDateTime �ĸ�ʽ����Ϊ YYYY-MM-DD HH:MM:SS
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
// ����: ����ʱ����룬������ *this
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
// ����: ����ʱ����룬�����������
//-----------------------------------------------------------------------------
void CDateTime::DecodeDateTime(int& nYear, int& nMonth, int& nDay, 
    int& nHour, int& nMinute, int& nSecond) const
{
    struct tm tm;

#ifdef ICL_WIN32
    struct tm *ptm = localtime(&m_tTime);    // TODO: �˴���������������
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
// ����: ���������ַ���
// ����:
//   chDateSep - ���ڷָ���
// ��ʽ:
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
// ����: ��������ʱ���ַ���
// ����:
//   chDateSep - ���ڷָ���
//   chTimeSep - ʱ��ָ���
// ��ʽ:
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

    // ������˵��:
    // PTHREAD_MUTEX_TIMED_NP:
    //   ��ͨ����ͬһ�߳��ڱ���ɶԵ��� Lock �� Unlock�������������ö�� Lock�������������
    // PTHREAD_MUTEX_RECURSIVE_NP:
    //   Ƕ�������߳��ڿ���Ƕ�׵��� Lock����һ����Ч��֮����������ͬ������ Unlock ���ɽ�����
    // PTHREAD_MUTEX_ERRORCHECK_NP:
    //   ����������ͬһ�߳�Ƕ�׵��� Lock ���������
    // PTHREAD_MUTEX_ADAPTIVE_NP:
    //   ��Ӧ����
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
    // �����δ����������� destroy���˺����᷵�ش��� EBUSY��
    // �� linux �£���ʹ�˺������ش���Ҳ��������Դй©��
    pthread_mutex_destroy(&m_Lock);
#endif
}

//-----------------------------------------------------------------------------
// ����: ����
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
// ����: ����
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
// ����: ���Լ��� (���Ѿ����ڼ���״̬����������)
// ����:
//   true   - �����ɹ�
//   false  - ʧ�ܣ������Ѿ����ڼ���״̬
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
// ����: ���캯��
// ����:
//   nStartId - ��ʼ���к�
//-----------------------------------------------------------------------------
CSeqAllocator::CSeqAllocator(uint nStartId)
{
    m_nCurrentId = nStartId;
}

//-----------------------------------------------------------------------------
// ����: ����һ���·����ID
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
// ����: ���캯��
// ����:
//   nMemoryDelta - �ڴ��������� (�ֽ����������� 2 �� N �η�)
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

        // ��֤ nNewMemoryDelta ��2��N�η�
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
// ����: ���ڴ���
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
// ����: д�ڴ���
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
// ����: �ڴ���ָ�붨λ
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
// ����: �����ڴ�����С
//-----------------------------------------------------------------------------
void CMemoryStream::SetSize(int64 nSize)
{
    int64 nOldPos = m_nPosition;

    SetCapacity(nSize);
    m_nSize = nSize;
    if (nOldPos > nSize) Seek(0, SO_END);
}

//-----------------------------------------------------------------------------
// ����: �����������뵽�ڴ�����
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
// ����: ���ļ����뵽�ڴ�����
//-----------------------------------------------------------------------------
void CMemoryStream::LoadFromFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_OPEN_READ | FM_SHARE_DENY_WRITE);
    LoadFromStream(FileStream);
}

//-----------------------------------------------------------------------------
// ����: ���ڴ������浽��������
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToStream(CStream& Stream)
{
    if (m_nSize != 0)
        Stream.Write(m_pMemory, m_nSize);
}

//-----------------------------------------------------------------------------
// ����: ���ڴ������浽�ļ���
//-----------------------------------------------------------------------------
void CMemoryStream::SaveToFile(const string& strFileName)
{
    CFileStream FileStream(strFileName, FM_CREATE);
    SaveToStream(FileStream);
}

//-----------------------------------------------------------------------------
// ����: ����ڴ���
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
// ����: ���캯��
// ����:
//   strFileName - �ļ���
//   nMode       - �ļ����򿪷�ʽ
//   nRights     - �ļ���ȡȨ��
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
// ����: ��������
//-----------------------------------------------------------------------------
CFileStream::~CFileStream()
{
    if (m_nHandle >= 0) FileClose(m_nHandle);
}

//-----------------------------------------------------------------------------
// ����: �����ļ�
//-----------------------------------------------------------------------------
int CFileStream::FileCreate(const string& strFileName, uint nRights)
{
#ifdef ICL_WIN32
    return (int)CreateFile(strFileName.c_str(), GENERIC_READ | GENERIC_WRITE,
        0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
#endif
#ifdef ICL_LINUX
    umask(0);  // ��ֹ nRights �� umask ֵ ����
    return open(strFileName.c_str(), O_RDWR | O_CREAT | O_TRUNC, nRights);
#endif
}

//-----------------------------------------------------------------------------
// ����: ���ļ�
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
        umask(0);  // ��ֹ nMode �� umask ֵ ����
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
// ����: �ر��ļ�
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
// ����: ���ļ�
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
// ����: д�ļ�
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
// ����: �ļ�ָ�붨λ
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
// ����: ���ļ���
//-----------------------------------------------------------------------------
int CFileStream::Read(void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileRead(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: д�ļ���
//-----------------------------------------------------------------------------
int CFileStream::Write(const void *pBuffer, int nCount)
{
    int nResult;

    nResult = FileWrite(m_nHandle, pBuffer, nCount);
    if (nResult == -1) nResult = 0;

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: �ļ���ָ�붨λ
//-----------------------------------------------------------------------------
int64 CFileStream::Seek(int64 nOffset, SeekOrigin nSeekOrigin)
{
    return FileSeek(m_nHandle, nOffset, nSeekOrigin);
}

//-----------------------------------------------------------------------------
// ����: �����ļ�����С
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
// ����: ���б������Ԫ��
//-----------------------------------------------------------------------------
void CList::Add(Pointer Item)
{
    if (m_nCount == m_nCapacity) Grow();
    m_pList[m_nCount] = Item;
    m_nCount++;
}

//-----------------------------------------------------------------------------
// ����: ���б��в���Ԫ��
// ����:
//   nIndex - ����λ���±��(0-based)
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
// ����: ���б���ɾ��Ԫ��
// ����:
//   nIndex - �±��(0-based)
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
// ����: ���б���ɾ��Ԫ��
// ����: ��ɾ��Ԫ�����б��е��±��(0-based)����δ�ҵ����򷵻� -1.
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
// ����: ���б���ɾ��Ԫ��
// ����: ��ɾ����Ԫ��ֵ����δ�ҵ����򷵻� NULL.
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
// ����: �ƶ�һ��Ԫ�ص��µ�λ��
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
// ����: �ı��б�Ĵ�С
//-----------------------------------------------------------------------------
void CList::Resize(int nCount)
{
    SetCount(nCount);
}

//-----------------------------------------------------------------------------
// ����: ����б�
//-----------------------------------------------------------------------------
void CList::Clear()
{
    SetCount(0);
    SetCapacity(0);
}

//-----------------------------------------------------------------------------
// ����: �����б��е��׸�Ԫ�� (���б�Ϊ�����׳��쳣)
//-----------------------------------------------------------------------------
Pointer CList::First()
{
    return Get(0);
}

//-----------------------------------------------------------------------------
// ����: �����б��е����Ԫ�� (���б�Ϊ�����׳��쳣)
//-----------------------------------------------------------------------------
Pointer CList::Last()
{
    return Get(m_nCount - 1);
}

//-----------------------------------------------------------------------------
// ����: ����Ԫ�����б��е��±�� (��δ�ҵ��򷵻� -1)
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
// ����: �����б���Ԫ������
//-----------------------------------------------------------------------------
int CList::Count() const
{
    return m_nCount;
}

//-----------------------------------------------------------------------------
// ����: ��ȡ�б�������Ԫ��
//-----------------------------------------------------------------------------
Pointer& CList::operator [] (int nIndex)
{
    if (nIndex < 0 || nIndex >= m_nCount)
        throw CException(IEM_LIST_INDEX_ERROR, __FILE__, __LINE__);

    return m_pList[nIndex];
}

///////////////////////////////////////////////////////////////////////////////

} // namespace icl
