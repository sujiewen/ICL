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
// class CException - 异常基类

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

    // 返回用于 Log 的字符串 
    string MakeLogMsg() const;
};

///////////////////////////////////////////////////////////////////////////////
// class CLogger - 日志类

class CLogger
{
private:
    string m_strFileName;   // 日志文件名

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
// class CBuffer - 缓存类

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
// class CDateTime - 日期时间类

class CDateTime
{
private:
    time_t m_tTime;     // (从1970-01-01 00:00:00 算起的秒数，UTC时间)

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
// class CAutoInvokable/CAutoInvoker - 自动被调对象/自动调用者
//
// 说明:
// 1. 这两个类联合使用，可以起到和 "智能指针" 类似的作用，即利用栈对象自动销毁的特性，在栈
//    对象的生命周期中自动调用 CAutoInvokable::InvokeInitialize() 和 InvokeFinalize()。
//    此二类一般使用在重要资源的对称性操作场合(比如加锁/解锁)。
// 2. 使用者需继承 CAutoInvokable 类，重写 InvokeInitialize() 和 InvokeFinalize()
//    函数。并在需要调用的地方定义 CAutoInvoker 的栈对象。

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
// class CCriticalSection - 线程临界区互斥类
//
// 说明:
// 1. 此类用于多线程环境下临界区互斥，基本操作有 Lock、Unlock 和 TryLock；
// 2. 线程内允许嵌套调用 Lock，嵌套调用后必须调用相同次数的 Unlock 才可解锁；

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

    // 加锁
    void Lock();
    // 解锁
    void Unlock();
    // 尝试加锁 (若已经处于加锁状态则立即返回 false)
    bool TryLock();
};

///////////////////////////////////////////////////////////////////////////////
// class CAutoSynchronizer - 线程自动互斥类
//
// 说明:
// 1. 此类利用C++的栈对象自动销毁的特性，在多线程环境下进行局部范围临界区互斥；
// 2. 使用方法: 在需要互斥的范围中以局部变量方式定义此类对象即可；
// 
// 使用范例:
//   假设已定义: CCriticalSection m_Lock;
//   自动加锁和解锁:
//   {
//       CAutoSynchronizer Syncher(m_Lock);
//       //...
//   }

typedef CAutoInvoker CAutoSynchronizer;

///////////////////////////////////////////////////////////////////////////////
// class CSeqAllocator - 整数序列号分配器类
//
// 说明:
// 1. 此类以线程安全方式生成一个不断递增的整数序列，用户可以指定序列的起始值；
// 2. 此类一般用于数据包的顺序号控制；

class CSeqAllocator
{
private:
    CCriticalSection m_Lock;
    uint m_nCurrentId;

public:
    explicit CSeqAllocator(uint nStartId = 0);

    // 返回一个新分配的ID
    uint AllocId();
};

///////////////////////////////////////////////////////////////////////////////
// class CStream - 流 基类

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
// class CMemoryStream - 内存流类

class CMemoryStream : public CStream
{
public:
    enum { DEFAULT_MEMORY_DELTA = 1024 };    // 缺省内存增长步长 (字节数，必须是 2 的 N 次方)
    enum { MIN_MEMORY_DELTA = 256 };         // 最小内存增长步长

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
// class CFileStream - 文件流类

// 文件流打开方式 (uint nMode)
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

// 缺省文件存取权限 (nRights)
#ifdef ICL_WIN32
enum { DEFAULT_FILE_ACCESS_RIGHTS = 0 };
#endif
#ifdef ICL_LINUX
enum { DEFAULT_FILE_ACCESS_RIGHTS = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH };
#endif

class CFileStream : public CStream
{
private:
    int m_nHandle;      // 文件句柄

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
// class CList  - 列表类
// 
// 说明:
// 1. 此类的实现原理与 Delphi::TList 完全相同；
// 2. 此列表类有如下优点:
//    a. 公有方法简单明确 (STL虽无比强大但稍显晦涩)；
//    b. 支持下标随机存取各个元素 (STL::list不支持)；
//    c. 支持快速获取列表长度 (STL::list不支持)；
//    d. 支持尾部快速增删元素；
// 3. 此列表类有如下缺点:
//    a. 不支持头部和中部的快速增删元素；
//    b. 只支持单一类型元素(Pointer类型)；

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
// class IEventHandler/CEvent - C++事件支持类

// 空事件参数定义
class CNullSender {};
class CNullParam {};

// 事件处理器接口
template<class SenderType, class ParamType>
class IEventHandler
{
public:
    virtual ~IEventHandler() {}
    virtual void HandleEvent(const SenderType& Sender, const ParamType& Param) = 0;
};

// 事件类
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
// 全局对象

extern CLogger Logger;  // 日志对象

///////////////////////////////////////////////////////////////////////////////

} // namespace icl

#endif // _ICL_CLASSES_H_
