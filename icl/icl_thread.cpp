///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// 文件名称: icl_thread.cpp
// 功能描述: 线程类
// 最后修改: 2005-12-06
///////////////////////////////////////////////////////////////////////////////

#include "icl_thread.h"
#include "icl_sysutils.h"
#include "icl_classes.h"

namespace icl
{

///////////////////////////////////////////////////////////////////////////////
// class CThreadImpl

CThreadImpl::CThreadImpl(CThread *pThread) :
    m_Thread(*pThread),
    m_nThreadId(0),
    m_bFinished(false),
    m_nTermElapsedSecs(0),
    m_bFreeOnTerminate(false),
    m_bTerminated(false),
    m_nReturnValue(0)
{
}

void CThreadImpl::Execute()
{
    m_Thread.Execute();
}

void CThreadImpl::BeforeTerminate()
{
    m_Thread.BeforeTerminate();
}

void CThreadImpl::BeforeKill()
{
    m_Thread.BeforeKill();
}

//-----------------------------------------------------------------------------
// 描述: 如果线程已运行，则抛出异常
//-----------------------------------------------------------------------------
void CThreadImpl::CheckNotRunning()
{
    if (m_nThreadId != 0)
        throw CException(IEM_THREAD_ALREADY_RUNNING, __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 通知线程退出
//-----------------------------------------------------------------------------
void CThreadImpl::Terminate()
{
    try { BeforeTerminate(); } catch (CException& e){}

    if (!m_bTerminated) m_nTermElapsedSecs = time(NULL);
    m_bTerminated = true;
}

//-----------------------------------------------------------------------------
// 描述: 取得从调用 Terminate 到当前共经过多少时间(秒)
//-----------------------------------------------------------------------------
int CThreadImpl::GetTermElapsedSecs() const
{
    int nResult = 0;

    // 如果已经通知退出，但线程还活着
    if (m_bTerminated && m_nThreadId != 0)
    {
        nResult = time(NULL) - m_nTermElapsedSecs;
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// 描述: 设置是否 Terminate
//-----------------------------------------------------------------------------
void CThreadImpl::SetTerminated(bool bValue)
{
    if (bValue != m_bTerminated)
    {
        if (bValue)
            Terminate();
        else
        {
            m_bTerminated = false;
            m_nTermElapsedSecs = 0;
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CWin32ThreadImpl

#ifdef ICL_WIN32

//-----------------------------------------------------------------------------
// 描述: 线程执行函数
// 参数:
//   pArg - 线程参数，此处指向 CWin32ThreadImpl 对象
//-----------------------------------------------------------------------------
DWORD WINAPI ThreadExecProc(void *pArg)
{
    CWin32ThreadImpl *pThreadImpl = (CWin32ThreadImpl*)pArg;
    int nReturnValue = 0;

    {
        // 对象 AutoFinalizer 进行自动化善后工作
        struct CAutoFinalizer
        {
            CWin32ThreadImpl *m_pThreadImpl;
            CAutoFinalizer(CWin32ThreadImpl *pThreadImpl) { m_pThreadImpl = pThreadImpl; }
            ~CAutoFinalizer()
            {
                m_pThreadImpl->SetFinished(true);
                //m_pThreadImpl->SetThreadId(0);//为什么要设置等于0，设置后，就不在外部杀死线程
                if (m_pThreadImpl->GetFreeOnTerminate()) delete m_pThreadImpl->GetThread();
            }
        } AutoFinalizer(pThreadImpl);

        if (!pThreadImpl->m_bTerminated)
        {
            try { pThreadImpl->Execute(); } catch (CException& e) { ; }

            // 记下线程返回值
            nReturnValue = pThreadImpl->m_nReturnValue;
        }
    }
    
    ExitThread(nReturnValue);
    return NULL;
}

//-----------------------------------------------------------------------------
// 描述: 构造函数
//-----------------------------------------------------------------------------
CWin32ThreadImpl::CWin32ThreadImpl(CThread *pThread) :
    CThreadImpl(pThread),
    m_nHandle(0),
    m_nPriority(THREAD_PRI_NORMAL)
{
}

//-----------------------------------------------------------------------------
// 描述: 析构函数
//-----------------------------------------------------------------------------
CWin32ThreadImpl::~CWin32ThreadImpl()
{
    if (m_nThreadId != 0 && !m_bFinished)
    {
        Terminate();
        WaitFor();
    }

    if (m_nThreadId != 0)
        CloseHandle(m_nHandle);
}

//-----------------------------------------------------------------------------
// 描述: 线程错误处理
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::CheckThreadError(bool bSuccess)
{
    if (!bSuccess)
        throw CException(SysErrorMessage(GetLastError()).c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 创建线程并执行
// 注意: 此成员方法在对象声明周期中只可调用一次。
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Run()
{
    CheckNotRunning();

    m_nHandle = CreateThread(NULL, 0, ThreadExecProc, (LPVOID)this, 0, (LPDWORD)&m_nThreadId);
    CheckThreadError(m_nHandle != 0);

    // 设置线程优先级
    if (m_nPriority != THREAD_PRI_NORMAL)
        SetPriority(m_nPriority);
}

//-----------------------------------------------------------------------------
// 描述: 通知线程退出
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Terminate()
{
    CThreadImpl::Terminate();
}

//-----------------------------------------------------------------------------
// 描述: 强行杀死线程
// 注意: 
//   1. 调用此函数后，对线程类对象的一切操作皆不可用(Terminate(); WaitFor(); delete pThread; 等)。
//   2. 线程被杀死后，用户所管理的某些重要资源可能未能得到释放，比如锁资源 (还未来得及解锁
//      便被杀了)，所以重要资源的释放工作必须在 BeforeKill 中进行。
//   3. Win32 下强杀线程，线程执行过程中的栈对象不会析构。故 Kill() 会显式释放 CThread 对象。
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Kill()
{
    if (m_nThreadId != 0)
    {
        // 用户须在此处释放重要资源，比如释放锁资源
        try { BeforeKill(); } catch (CException& e) {}

        m_nThreadId = 0;
        TerminateThread(m_nHandle, 0);

        // Win32 下强杀线程，不会析构栈对象。故此处必须显式释放 CThread 对象。
        delete (CThread*)&m_Thread;
    }
}

//-----------------------------------------------------------------------------
// 描述: 等待线程退出
// 返回: 线程返回值
//-----------------------------------------------------------------------------
int CWin32ThreadImpl::WaitFor()
{
    if (m_nThreadId != 0)
    {
        WaitForSingleObject(m_nHandle, INFINITE);
        GetExitCodeThread(m_nHandle, (LPDWORD)&m_nReturnValue);
    }

    return m_nReturnValue;
}

//-----------------------------------------------------------------------------
// 描述: 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
// 参数:
//   fSeconds - 睡眠的秒数，可为小数，可精确到毫秒
// 注意:
//   由于将睡眠时间分成了若干份，每次睡眠时间的小误差累加起来将扩大总误差。
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Sleep(double fSeconds)
{
    const double SLEEP_INTERVAL = 0.5;      // 每次睡眠的时间(秒)
    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        NanoSleep(fOnceSecs, true);
    }
}

//-----------------------------------------------------------------------------
// 描述: 设置线程的优先级
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::SetPriority(int nValue)
{
    int nPriorities[7] = {
        THREAD_PRIORITY_IDLE,
        THREAD_PRIORITY_LOWEST,
        THREAD_PRIORITY_BELOW_NORMAL,
        THREAD_PRIORITY_NORMAL,
        THREAD_PRIORITY_ABOVE_NORMAL,
        THREAD_PRIORITY_HIGHEST,
        THREAD_PRIORITY_TIME_CRITICAL
    };

    nValue = (int)TrimValue((int)nValue, 0, 6);
    m_nPriority = nValue;
    if (m_nThreadId != 0)
        SetThreadPriority(m_nHandle, nPriorities[nValue]);
}

#endif

///////////////////////////////////////////////////////////////////////////////
// class CLinuxThreadImpl

#ifdef ICL_LINUX

//-----------------------------------------------------------------------------
// 描述: 线程执行函数
// 参数:
//   pArg - 线程参数，此处指向 CLinuxThreadImpl 对象
//-----------------------------------------------------------------------------
void* ThreadExecProc(void *pArg)
{
    CLinuxThreadImpl *pThreadImpl = (CLinuxThreadImpl*)pArg;
    int nReturnValue = 0;

    {
        // 对象 AutoFinalizer 进行自动化善后工作
        struct CAutoFinalizer
        {
            CLinuxThreadImpl *m_pThreadImpl;
            CAutoFinalizer(CLinuxThreadImpl *pThreadImpl) { m_pThreadImpl = pThreadImpl; }
            ~CAutoFinalizer()
            {
                m_pThreadImpl->SetFinished(true);
                m_pThreadImpl->SetThreadId(0);
                if (m_pThreadImpl->GetFreeOnTerminate()) delete m_pThreadImpl->GetThread();
            }
        } AutoFinalizer(pThreadImpl);

        // 线程对 cancel 信号的响应方式有三种: (1)不响应 (2)推迟到取消点再响应 (3)尽量立即响应。
        // 此处设置线程为第(3)种方式，即可马上被 cancel 信号终止。
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        if (!pThreadImpl->m_bTerminated)
        {
            // 线程在执行过程中若收到 cancel 信号，会抛出一个异常(究竟是什么类型的异常？)，
            // 此异常千万不可去阻拦它( try{}catch(...){} )，系统能侦测出此异常是否被彻底阻拦，
            // 若是，则会发出 SIGABRT 信号，并在终端输出 "FATAL: exception not rethrown"。
            // 所以此处的策略是只阻拦 CException 异常(在ISE中所有异常皆从 CException 继承)。
            // 在 pThread->Execute() 的执行过程中，用户应该注意如下事项:
            // 1. 请参照此处的做法去拦截异常，而切不可阻拦所有类型的异常( 即catch(...) );
            // 2. 不可抛出 CException 及其子类之外的异常。假如抛出一个整数( 如 throw 5; )，
            //    系统会因为没有此异常的处理程序而调用 abort。(尽管如此，ThreadFinalProc
            //    仍会象 pthread_cleanup_push 所承诺的那样被执行到。)
            try { pThreadImpl->Execute(); } catch (CException& e) { ; }

            // 记下线程返回值
            nReturnValue = pThreadImpl->m_nReturnValue;
        }

        // 屏蔽 cancel 信号，出了当前 scope 后将不可被强行终止
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    }
    
    pthread_exit((void*)nReturnValue);
    return NULL;
}

//-----------------------------------------------------------------------------
// 描述: 构造函数
//-----------------------------------------------------------------------------
CLinuxThreadImpl::CLinuxThreadImpl(CThread *pThread) :
    CThreadImpl(pThread),
    m_nPolicy(THREAD_POL_DEFAULT),
    m_nPriority(THREAD_PRI_DEFAULT)
{
}

//-----------------------------------------------------------------------------
// 描述: 析构函数
//-----------------------------------------------------------------------------
CLinuxThreadImpl::~CLinuxThreadImpl()
{
    if (m_nThreadId != 0 && !m_bFinished)
    {
        Terminate();
        WaitFor();
    }

    if (m_nThreadId != 0)
        pthread_detach(m_nThreadId);
}

//-----------------------------------------------------------------------------
// 描述: 线程错误处理
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::CheckThreadError(int nErrorCode)
{
    if (nErrorCode != 0)
        throw CException(SysErrorMessage(nErrorCode).c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// 描述: 创建线程并执行
// 注意: 此成员方法在对象声明周期中只可调用一次。
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Run()
{
    CheckNotRunning();

    // 创建线程
    CheckThreadError(pthread_create((pthread_t*)&m_nThreadId, NULL, ThreadExecProc, (void*)this));

    // 设置线程调度策略
    if (m_nPolicy != THREAD_POL_DEFAULT)
        SetPolicy(m_nPolicy);
    // 设置线程优先级
    if (m_nPriority != THREAD_PRI_DEFAULT)
        SetPriority(m_nPriority);
}

//-----------------------------------------------------------------------------
// 描述: 通知线程退出
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Terminate()
{
    CThreadImpl::Terminate();
}

//-----------------------------------------------------------------------------
// 描述: 强行杀死线程
// 注意: 
//   1. 调用此函数后，对线程类对象的一切操作皆不可用(Terminate(); WaitFor(); delete pThread; 等)。
//   2. 在杀死线程前，m_bFreeOnTerminate 会自动设为 true，以便对象能自动释放。
//   3. 线程被杀死后，用户所管理的某些重要资源可能未能得到释放，比如锁资源 (还未来得及解锁
//      便被杀了)，所以重要资源的释放工作必须在 DoKill 中进行。
//   4. 线程在收到 cancel 信号后，在销毁自己时，本质上是抛出一个异常，让异常来干扰线程函数
//      的执行流程。所以，线程函数中的所有栈对象依然按照正常的顺序析构。
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Kill()
{
    if (m_nThreadId != 0)
    {
        // 用户须在此处释放重要资源，比如释放锁资源
        try { BeforeKill(); } catch (CException& e) {}

        pthread_t nThreadId = m_nThreadId;
        m_nThreadId = 0;

        SetFreeOnTerminate(true);
        pthread_cancel(nThreadId);
    }
}

//-----------------------------------------------------------------------------
// 描述: 等待线程退出
// 返回: 线程返回值
//-----------------------------------------------------------------------------
int CLinuxThreadImpl::WaitFor()
{
    pthread_t nThreadId = m_nThreadId;

    if (m_nThreadId != 0)
    {
        m_nThreadId = 0;
        CheckThreadError(pthread_join(nThreadId, (void**)&m_nReturnValue));
    }
    
    return m_nReturnValue;
}

//-----------------------------------------------------------------------------
// 描述: 进入睡眠状态 (睡眠过程中会检测 m_bTerminated 的状态)
// 参数:
//   fSeconds - 睡眠的秒数，可为小数，可精确到毫秒
// 注意:
//   由于将睡眠时间分成了若干份，每次睡眠时间的小误差累加起来将扩大总误差。
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Sleep(double fSeconds)
{
    const double SLEEP_INTERVAL = 0.5;      // 每次睡眠的时间(秒)
    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        NanoSleep(fOnceSecs, true);
    }
}

//-----------------------------------------------------------------------------
// 描述: 设置线程的调度策略
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::SetPolicy(int nValue)
{
    if (nValue != THREAD_POL_DEFAULT &&
        nValue != THREAD_POL_RR &&
        nValue != THREAD_POL_FIFO)
    {
        nValue = THREAD_POL_DEFAULT;
    }

    m_nPolicy = nValue;

    if (m_nThreadId != 0)
    {
        struct sched_param param;
        param.sched_priority = m_nPriority;
        pthread_setschedparam(m_nThreadId, m_nPolicy, &param);
    }
}

//-----------------------------------------------------------------------------
// 描述: 设置线程的优先级
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::SetPriority(int nValue)
{
    if (nValue < THREAD_PRI_MIN || nValue > THREAD_PRI_MAX)
        nValue = THREAD_PRI_DEFAULT;

    m_nPriority = nValue;

    if (m_nThreadId != 0)
    {
        struct sched_param param;
        param.sched_priority = m_nPriority;
        pthread_setschedparam(m_nThreadId, m_nPolicy, &param);
    }
}

#endif

///////////////////////////////////////////////////////////////////////////////

} // namespace icl
