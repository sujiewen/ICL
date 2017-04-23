///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
// icl_classes.h
// Classes:
//   > CException
//   > CLogger
//   > CBuffer
//   > CDateTime
//   > CAutoInvokable
//   > CAutoInvoker
//   > CCriticalSection
//   > CAutoSynchronizer
//   > CSeqAllocator
//   > CStream
//   > CMemoryStream
//   > CFileStream
//   > CList
//   > IEventHandler
//   > CEvent
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_CLASSES_H_
#define _ICL_CLASSES_H_

#include "icl_config.h"

#ifdef ICL_WIN32
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <sys/timeb.h>
#include <iostream>
#include <fstream>
#include <string>
#include <windows.h>
#endif

#ifdef ICL_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <time.h>
#include <sys/timeb.h>
#include <sys/file.h>
#include <iostream>
#include <fstream>
#include <string>
#endif

#include "icl_types.h"
#include "icl_errmsgs.h"

using namespace std;

namespace icl
{

typedef void* Pointer;

///////////////////////////////////////////////////////////////////////////////
// class CException - �쳣����

class CException
{
public:
    enum { EMPTY_LINE_NO = -1 };
    
protected:
    string m_strErrorMsg;
    string m_strFileName;
    int m_nLineNo;    

public:
    CException(const char *sErrorMsg = NULL, const char *sFileName = NULL,
    int nLineNo = EMPTY_LINE_NO);
    CException(const CException& src);
    virtual ~CException() {}

    string ErrorMsg() const { return m_strErrorMsg; }
    string FileName() const { return m_strFileName; }
    int LineNo() const { return m_nLineNo; }

    // �������� Log ���ַ��� 
    string MakeLogMsg() const;
};

///////////////////////////////////////////////////////////////////////////////
// class CLogger - ��־��

class CLogger
{
private:
    string m_strFileName;   // ��־�ļ���

    void WriteToFile(const string& strString);
public:
    CLogger() {}
    virtual ~CLogger() {}

    void SetFileName(const string& strFileName)
        { m_strFileName = strFileName; }

    //void Write(const char* sFormatString, ...);
    void Write(const CException& e);
};

///////////////////////////////////////////////////////////////////////////////
// class CBuffer - ������

class CBuffer
{
protected:
    void *m_pBuffer;
    int m_nSize;
	string m_strBufCharSet;
public:
    CBuffer();
    CBuffer(const CBuffer& src);
    explicit CBuffer(int nSize);
    CBuffer(const void *pBuffer, int nSize);
    virtual ~CBuffer();
    CBuffer& operator = (const CBuffer& rhs);
    char& operator [] (int nIndex)
    {
        return ((char*)m_pBuffer)[nIndex];
    }
    operator char*() const 
	{
		if (m_nSize <= 0 || !m_pBuffer)
			return "";
		else
		{
			((char*)m_pBuffer)[m_nSize] = 0;
			return (char*)m_pBuffer;
		}
	}
    char* Data() const { return (char*)m_pBuffer; }
    char* c_str() const;
    void Assign(const void *pBuffer, int nSize);
    void SetSize(int nSize, bool bZeroExtBuf = false);
    int GetSize() const { return m_nSize; }
	string GetBufCharSet(){ return m_strBufCharSet;}
	void SetBufCharSet(const string& strBufCharSet)
	{
		m_strBufCharSet = strBufCharSet;	
	}
    
};

///////////////////////////////////////////////////////////////////////////////
// class CDateTime - ����ʱ����

class CDateTime
{
private:
    time_t m_tTime;     // (��1970-01-01 00:00:00 �����������UTCʱ��)

public:
    CDateTime()                     { m_tTime = 0; }
    CDateTime(const CDateTime& src) { m_tTime = src.m_tTime; }
    CDateTime(time_t src)           { m_tTime = src; }
	CDateTime(const string& strTime);

    static CDateTime CurrentDateTime();
    static time_t CurrentZoneTimeStamp();

    CDateTime& operator = (const CDateTime& rhs)
        { m_tTime = rhs.m_tTime; return *this; }
    CDateTime& operator = (const time_t rhs)
        { m_tTime = rhs; return *this; }
    CDateTime& operator = (const string& strDateTime);

    CDateTime operator + (const CDateTime& rhs) const { return m_tTime + rhs.m_tTime; }
    CDateTime operator + (time_t rhs) const { return m_tTime + rhs; }
    CDateTime operator - (const CDateTime& rhs) const { return m_tTime - rhs.m_tTime; }
    CDateTime operator - (time_t rhs) const { return m_tTime - rhs; }

    bool operator == (const CDateTime& rhs) const { return m_tTime == rhs.m_tTime; }
    bool operator != (const CDateTime& rhs) const { return m_tTime != rhs.m_tTime; }
    bool operator > (const CDateTime& rhs) const  { return m_tTime > rhs.m_tTime; }
    bool operator < (const CDateTime& rhs) const  { return m_tTime < rhs.m_tTime; }
    bool operator >= (const CDateTime& rhs) const { return m_tTime >= rhs.m_tTime; }
    bool operator <= (const CDateTime& rhs) const { return m_tTime <= rhs.m_tTime; }

    operator time_t() const { return m_tTime; }

    void EncodeDateTime(int nYear, int nMonth, int nDay, 
        int nHour = 0, int nMinute = 0, int nSecond = 0);
    void DecodeDateTime(int& nYear, int& nMonth, int& nDay, 
        int& nHour, int& nMinute, int& nSecond) const;

    string DateString(char chDateSep = '-') const;
    string DateTimeString(char chDateSep = '-', char chTimeSep = ':') const;
	string DateTimeString(const string& strSep) const;
};

///////////////////////////////////////////////////////////////////////////////
// class CAutoInvokable/CAutoInvoker - �Զ���������/�Զ�������
//
// ˵��:
// 1. ������������ʹ�ã������𵽺� "����ָ��" ���Ƶ����ã�������ջ�����Զ����ٵ����ԣ���ջ
//    ����������������Զ����� CAutoInvokable::InvokeInitialize() �� InvokeFinalize()��
//    �˶���һ��ʹ������Ҫ��Դ�ĶԳ��Բ�������(�������/����)��
// 2. ʹ������̳� CAutoInvokable �࣬��д InvokeInitialize() �� InvokeFinalize()
//    ������������Ҫ���õĵط����� CAutoInvoker ��ջ����

class CAutoInvokable
{
friend class CAutoInvoker;

protected:
    virtual void InvokeInitialize() {}
    virtual void InvokeFinalize() {}
};

class CAutoInvoker
{
private:
    CAutoInvokable *m_pObject;
public:
    explicit CAutoInvoker(CAutoInvokable& Object)
    { m_pObject = &Object; m_pObject->InvokeInitialize(); }

    virtual ~CAutoInvoker()
    { m_pObject->InvokeFinalize(); }
};

///////////////////////////////////////////////////////////////////////////////
// class CCriticalSection - �߳��ٽ���������
//
// ˵��:
// 1. �������ڶ��̻߳������ٽ������⣬���������� Lock��Unlock �� TryLock��
// 2. �߳�������Ƕ�׵��� Lock��Ƕ�׵��ú���������ͬ������ Unlock �ſɽ�����

class CCriticalSection : public CAutoInvokable
{
private:
#ifdef ICL_WIN32
    CRITICAL_SECTION m_Lock;
#endif
#ifdef ICL_LINUX
    pthread_mutex_t m_Lock;
#endif

protected:
    virtual void InvokeInitialize() { Lock(); }
    virtual void InvokeFinalize() { Unlock(); }

public:
    CCriticalSection();
    ~CCriticalSection();

    // ����
    void Lock();
    // ����
    void Unlock();
    // ���Լ��� (���Ѿ����ڼ���״̬���������� false)
    bool TryLock();
};

///////////////////////////////////////////////////////////////////////////////
// class CAutoSynchronizer - �߳��Զ�������
//
// ˵��:
// 1. ��������C++��ջ�����Զ����ٵ����ԣ��ڶ��̻߳����½��оֲ���Χ�ٽ������⣻
// 2. ʹ�÷���: ����Ҫ����ķ�Χ���Ծֲ�������ʽ���������󼴿ɣ�
// 
// ʹ�÷���:
//   �����Ѷ���: CCriticalSection m_Lock;
//   �Զ������ͽ���:
//   {
//       CAutoSynchronizer Syncher(m_Lock);
//       //...
//   }

typedef CAutoInvoker CAutoSynchronizer;

///////////////////////////////////////////////////////////////////////////////
// class CSeqAllocator - �������кŷ�������
//
// ˵��:
// 1. �������̰߳�ȫ��ʽ����һ�����ϵ������������У��û�����ָ�����е���ʼֵ��
// 2. ����һ���������ݰ���˳��ſ��ƣ�

class CSeqAllocator
{
private:
    CCriticalSection m_Lock;
    uint m_nCurrentId;

public:
    explicit CSeqAllocator(uint nStartId = 0);

    // ����һ���·����ID
    uint AllocId();
};

///////////////////////////////////////////////////////////////////////////////
// class CStream - �� ����

enum SeekOrigin {
    SO_BEGINNING    = 0,
    SO_CURRENT      = 1,
    SO_END          = 2
};

class CStream
{
public:
    virtual ~CStream() {}

    virtual int Read(void *pBuffer, int nCount) = 0;
    virtual int Write(const void *pBuffer, int nCount) = 0;
    virtual int64 Seek(int64 nOffset, SeekOrigin nSeekOrigin) = 0;

    int64 GetPosition() { return Seek(0, SO_CURRENT); }
    void SetPosition(int64 nPos) { Seek(nPos, SO_BEGINNING); }

    virtual int64 GetSize();
    virtual void SetSize(int64 nSize) {}
};

///////////////////////////////////////////////////////////////////////////////
// class CMemoryStream - �ڴ�����

class CMemoryStream : public CStream
{
public:
    enum { DEFAULT_MEMORY_DELTA = 1024 };    // ȱʡ�ڴ��������� (�ֽ����������� 2 �� N �η�)
    enum { MIN_MEMORY_DELTA = 256 };         // ��С�ڴ���������

private:
    char *m_pMemory;
    int64 m_nCapacity;
    int64 m_nSize;
    int64 m_nPosition;
    int64 m_nMemoryDelta;

    void SetMemoryDelta(int64 nNewMemoryDelta);
    void SetPointer(char* pMemory, int64 nSize);
    void SetCapacity(int64 nNewCapacity);
    char* Realloc(int64& nNewCapacity);
public:
    explicit CMemoryStream(int64 nMemoryDelta = DEFAULT_MEMORY_DELTA);
    virtual ~CMemoryStream();

    virtual int Read(void *pBuffer, int nCount);
    virtual int Write(const void *pBuffer, int nCount);
    virtual int64 Seek(int64 nOffset, SeekOrigin nSeekOrigin);
    virtual void SetSize(int64 nSize);
    void LoadFromStream(CStream& Stream);
    void LoadFromFile(const string& strFileName);
    void SaveToStream(CStream& Stream);
    void SaveToFile(const string& strFileName);
    void Clear();
    char* GetMemory() { return m_pMemory; }
};

///////////////////////////////////////////////////////////////////////////////
// class CFileStream - �ļ�����

// �ļ����򿪷�ʽ (uint nMode)
#ifdef ICL_WIN32
enum {
    FM_CREATE           = 0xFFFF,
    FM_OPEN_READ        = 0x0000,
    FM_OPEN_WRITE       = 0x0001,
    FM_OPEN_READ_WRITE  = 0x0002,

    FM_SHARE_EXCLUSIVE  = 0x0010,
    FM_SHARE_DENY_WRITE = 0x0020,
    FM_SHARE_DENY_NONE  = 0x0040
};
#endif
#ifdef ICL_LINUX
enum {
    FM_CREATE           = 0xFFFF,
    FM_OPEN_READ        = O_RDONLY,  // 0
    FM_OPEN_WRITE       = O_WRONLY,  // 1
    FM_OPEN_READ_WRITE  = O_RDWR,    // 2

    FM_SHARE_EXCLUSIVE  = 0x0010,
    FM_SHARE_DENY_WRITE = 0x0020,
    FM_SHARE_DENY_NONE  = 0x0030
};
#endif

// ȱʡ�ļ���ȡȨ�� (nRights)
#ifdef ICL_WIN32
enum { DEFAULT_FILE_ACCESS_RIGHTS = 0 };
#endif
#ifdef ICL_LINUX
enum { DEFAULT_FILE_ACCESS_RIGHTS = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH };
#endif

class CFileStream : public CStream
{
private:
    int m_nHandle;      // �ļ����

    int FileCreate(const string& strFileName, uint nRights);
    int FileOpen(const string& strFileName, uint nMode);
    void FileClose(int nHandle);
    int FileRead(int nHandle, void *pBuffer, int nCount);
    int FileWrite(int nHandle, const void *pBuffer, int nCount);
    int64 FileSeek(int nHandle, int64 nOffset, SeekOrigin nSeekOrigin);
public:
    CFileStream(const string& strFileName, uint nMode, uint nRights = DEFAULT_FILE_ACCESS_RIGHTS);
    virtual ~CFileStream();

    virtual int Read(void *pBuffer, int nCount);
    virtual int Write(const void *pBuffer, int nCount);
    virtual int64 Seek(int64 nOffset, SeekOrigin nSeekOrigin);
    virtual void SetSize(int64 nSize);
};

///////////////////////////////////////////////////////////////////////////////
// class CList  - �б���
// 
// ˵��:
// 1. �����ʵ��ԭ���� Delphi::TList ��ȫ��ͬ��
// 2. ���б����������ŵ�:
//    a. ���з�������ȷ (STL���ޱ�ǿ�����Ի�ɬ)��
//    b. ֧���±������ȡ����Ԫ�� (STL::list��֧��)��
//    c. ֧�ֿ��ٻ�ȡ�б��� (STL::list��֧��)��
//    d. ֧��β��������ɾԪ�أ�
// 3. ���б���������ȱ��:
//    a. ��֧��ͷ�����в��Ŀ�����ɾԪ�أ�
//    b. ֻ֧�ֵ�һ����Ԫ��(Pointer����)��

class CList
{
private:
    Pointer *m_pList;
    int m_nCount;
    int m_nCapacity;

protected:
    virtual void Grow();

    Pointer Get(int nIndex);
    void Put(int nIndex, Pointer Item);
    void SetCapacity(int nNewCapacity);
    void SetCount(int nNewCount);

public:
    CList();
    virtual ~CList();

    void Add(Pointer Item);
    void Insert(int nIndex, Pointer Item);
    void Delete(int nIndex);
    int Remove(Pointer Item);
    Pointer Extract(Pointer Item);
    void Move(int nCurIndex, int nNewIndex);
    void Resize(int nCount);
    void Clear();

    Pointer First();
    Pointer Last();
    int IndexOf(Pointer Item);
    int Count() const;
    inline bool IsEmpty() { return (Count() == 0); }

    Pointer& operator [] (int nIndex);
};

///////////////////////////////////////////////////////////////////////////////
// class IEventHandler/CEvent - C++�¼�֧����

// ���¼���������
class CNullSender {};
class CNullParam {};

// �¼��������ӿ�
template<class SenderType, class ParamType>
class IEventHandler
{
public:
    virtual ~IEventHandler() {}
    virtual void HandleEvent(const SenderType& Sender, const ParamType& Param) = 0;
};

// �¼���
template<class SenderType, class ParamType>
class CEvent
{
public:
    typedef IEventHandler<SenderType, ParamType> EventHanderType;
private:
    CList m_HandlerList;       // (EventHanderType*)[]
public:
    virtual ~CEvent() {}

    virtual void RegisterHandler(EventHanderType *pHandler)
    {
        if (pHandler && m_HandlerList.IndexOf(pHandler) == -1)
            m_HandlerList.Add(pHandler);
    }

    virtual void UnregisterHandler(EventHanderType *pHandler)
    {
        m_HandlerList.Remove(pHandler);
    }

    virtual void Invoke(const SenderType& Sender, const ParamType& Param)
    {
        for (register int i = 0; i < m_HandlerList.Count(); i++)
            ((EventHanderType*)m_HandlerList[i])->HandleEvent(Sender, Param);
    }
};

///////////////////////////////////////////////////////////////////////////////
// ȫ�ֶ���

extern CLogger Logger;  // ��־����

///////////////////////////////////////////////////////////////////////////////

} // namespace icl

#endif // _ICL_CLASSES_H_
