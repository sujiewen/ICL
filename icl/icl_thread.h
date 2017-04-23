///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
// icl_thread.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _ICL_THREAD_H_
#define _ICL_THREAD_H_

#include "icl_config.h"

#ifdef ICL_WIN32
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#endif

#ifdef ICL_LINUX
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#endif

#include "icl_types.h"
#include "icl_errmsgs.h"

using namespace std;

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
/* 说明

一、Win32平台下和Linux平台下线程的主要区别:

    1. Win32线程拥有Handle和ThreadId，而Linux线程只有ThreadId。
    2. Win32线程只有ThreadPriority，而Linux线程有ThreadPolicy和ThreadPriority。
    3. Win32线程在强行杀死时，线程执行过程中的栈对象不会析构，而Linux线程则会。

*/
///////////////////////////////////////////////////////////////////////////////
// 类型定义

#ifdef ICL_WIN32
// 线程优先级
enum {
    THREAD_PRI_IDLE         = 0,
    THREAD_PRI_LOWEST       = 1,
    THREAD_PRI_NORMAL       = 2,
    THREAD_PRI_HIGHER       = 3,
    THREAD_PRI_HIGHEST      = 4,
    THREAD_PRI_TIMECRITICAL = 5
};
#endif

#ifdef ICL_LINUX
// 线程调度策略
enum {
    THREAD_POL_DEFAULT      = SCHED_OTHER,
    THREAD_POL_RR           = SCHED_RR,
    THREAD_POL_FIFO         = SCHED_FIFO
};

// 线程优先级
enum {
    THREAD_PRI_DEFAULT     = 0,
    THREAD_PRI_MIN         = 0,
    THREAD_PRI_MAX         = 99,
    THREAD_PRI_HIGH        = 80
};
#endif

///////////////////////////////////////////////////////////////////////////////
// class CThreadImpl - 平台线程实现基类

class CThreadImpl
{
friend class CThread;

protected:
    CThread& m_Thread;              // 相关联的 CThread 对象
    int m_nThreadId;                // 线程ID
    bool m_bFinished;               // 线程是否已完成了线程函数的执行
    int m_nTermElapsedSecs;         // 从调用 Terminate 到当前共经过多少时间(秒)
    bool m_bFreeOnTerminate;        // 线程退出时是否同时释放类对象
    bool m_bTerminated;             // 是否应退出的标志
    int m_nReturnValue;             // 线程返回值 (可在 Execute 函数中修改此值，函数 WaitFor 返回此值)

protected:
    void Execute();
    void BeforeTerminate();
    void BeforeKill();

    void CheckNotRunning();
public:
    CThreadImpl(CThread *pThread);
    virtual ~CThreadImpl() {}

    virtual void Run() = 0;
    virtual void Terminate();
    virtual void Kill() = 0;
    virtual int WaitFor() = 0;
    virtual void Sleep(double fSeconds) = 0;

    // 属性 (getter)
    CThread* GetThread() { return (CThread*)&m_Thread; }
    int GetThreadId() const { return m_nThreadId; }
    int GetTerminated() const { return m_bTerminated; }
    int GetReturnValue() const { return m_nReturnValue; }
    bool GetFreeOnTerminate() const { return m_bFreeOnTerminate; }
    int GetTermElapsedSecs() const;
    // 属性 (setter)
    void SetThreadId(int nValue) { m_nThreadId = nValue; }
    void SetFinished(bool bValue) { m_bFinished = bValue; }
    void SetTerminated(bool bValue);
    void SetReturnValue(int nValue) { m_nReturnValue = nValue; }
    void SetFreeOnTerminate(bool bValue) { m_bFreeOnTerminate = bValue; }
};

///////////////////////////////////////////////////////////////////////////////
// class CWin32ThreadImpl - Win32平台线程实现类

#ifdef ICL_WIN32
class CWin32ThreadImpl : public CThreadImpl
{
friend DWORD WINAPI ThreadExecProc(void *pArg);

protected:
    HANDLE m_nHandle;               // 线程句柄
    int m_nPriority;                // 线程优先级
    
private:    
    void CheckThreadError(bool bSuccess);

public:
    CWin32ThreadImpl(CThread *pThread);
    virtual ~CWin32ThreadImpl();

    virtual void Run();
    virtual void Terminate();
    virtual void Kill();
    virtual int WaitFor();
    virtual void Sleep(double fSeconds);

    int GetPriority() const { return m_nPriority; }
	void SetPriority(int nValue);
	HANDLE GetHandle() const {return m_nHandle;}
};
#endif

///////////////////////////////////////////////////////////////////////////////
// class CLinuxThreadImpl - Linux平台线程实现类

#ifdef ICL_LINUX
class CLinuxThreadImpl : public CThreadImpl
{
friend void* ThreadExecProc(void *pArg);

protected:
    int m_nPolicy;                  // 线程调度策略 (THREAD_POLICY_XXX)
    int m_nPriority;                // 线程优先级 (0..99)

private:    
    void CheckThreadError(int nErrorCode);
    
public:
    CLinuxThreadImpl(CThread *pTreahd);
    virtual ~CLinuxThreadImpl();

    virtual void Run();
    virtual void Terminate();
    virtual void Kill();
    virtual int WaitFor();
    virtual void Sleep(double fSeconds);

    int GetPolicy() const { return m_nPolicy; }
    int GetPriority() const { return m_nPriority; }
    void SetPolicy(int nValue);
    void SetPriority(int nValue);
};
#endif

///////////////////////////////////////////////////////////////////////////////
// class CThread - 线程类

class CThread
{
friend class CThreadImpl;

private:
#ifdef ICL_WIN32
    CWin32ThreadImpl m_ThreadImpl;
#endif
#ifdef ICL_LINUX
    CLinuxThreadImpl m_ThreadImpl;
#endif

protected:
    // 线程的执行函数，子类必须重写。
    virtual void Execute() {}

    // 执行 Terminate() 前的附加操作。
    // 注: 由于 Terminate() 属于自愿退出机制，为了能让线程能尽快退出，除了
    // m_bTerminated 标志被设为 true 之外，有时还应当补充一些附加的操作以
    // 便能让线程尽快从阻塞操作中解脱出来。
    virtual void BeforeTerminate() {}

    // 执行 Kill() 前的附加操作。
    // 注: 线程被杀死后，用户所管理的某些重要资源可能未能得到释放，比如锁资源
    // (还未来得及解锁便被杀了)，所以重要资源的释放工作必须在 BeforeKill 中进行。
    virtual void BeforeKill() {}
public:
    CThread() : m_ThreadImpl(this) {}
    virtual ~CThread() {}

    // 创建并执行线程。
    // 注: 此成员方法在对象声明周期中只可调用一次。
    void Run() { m_ThreadImpl.Run(); }

    // 通知线程退出 (自愿退出机制)
    // 注: 若线程由于某些阻塞式操作迟迟不退出，可调用 Kill() 强行退出。
    void Terminate() { m_ThreadImpl.Terminate(); }

    // 强行杀死线程 (强行退出机制)
    void Kill() { m_ThreadImpl.Kill(); }

    // 等待线程退出
    int WaitFor() { return m_ThreadImpl.WaitFor(); }

    // 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
    // 注: 此函数必须由线程自己调用方可生效。
    void Sleep(double fSeconds) { m_ThreadImpl.Sleep(fSeconds); }

    // 属性 (getter)
    int GetThreadId() const { return m_ThreadImpl.GetThreadId(); }
    int GetTerminated() const { return m_ThreadImpl.GetTerminated(); }
    int GetReturnValue() const { return m_ThreadImpl.GetReturnValue(); }
    bool GetFreeOnTerminate() const { return m_ThreadImpl.GetFreeOnTerminate(); }
    int GetTermElapsedSecs() const { return m_ThreadImpl.GetTermElapsedSecs(); }
#ifdef ICL_WIN32
   int GetPriority() const { return m_ThreadImpl.GetPriority(); }
   HANDLE GetHandle() const {return m_ThreadImpl.GetHandle();}
#endif
#ifdef ICL_LINUX
    int GetPolicy() const { return m_ThreadImpl.GetPolicy(); }
    int GetPriority() const { return m_ThreadImpl.GetPriority(); }
#endif
    // 属性 (setter)
    void SetTerminated(bool bValue) { m_ThreadImpl.SetTerminated(bValue); }
    void SetReturnValue(int nValue) { m_ThreadImpl.SetReturnValue(nValue); }
    void SetFreeOnTerminate(bool bValue) { m_ThreadImpl.SetFreeOnTerminate(bValue); }
#ifdef ICL_WIN32
    void SetPriority(int nValue) { m_ThreadImpl.SetPriority(nValue); }
#endif
#ifdef ICL_LINUX
    void SetPolicy(int nValue) { m_ThreadImpl.SetPolicy(nValue); }
    void SetPriority(int nValue) { m_ThreadImpl.SetPriority(nValue); }
#endif
};

///////////////////////////////////////////////////////////////////////////////

} // namespace icl

#endif // _ICL_THREAD_H_
