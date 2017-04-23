///////////////////////////////////////////////////////////////////////////////
// Iris Cross-Platform Library (ICL)
//
// �ļ�����: icl_thread.cpp
// ��������: �߳���
// ����޸�: 2005-12-06
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
// ����: ����߳������У����׳��쳣
//-----------------------------------------------------------------------------
void CThreadImpl::CheckNotRunning()
{
    if (m_nThreadId != 0)
        throw CException(IEM_THREAD_ALREADY_RUNNING, __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ�߳��˳�
//-----------------------------------------------------------------------------
void CThreadImpl::Terminate()
{
    try { BeforeTerminate(); } catch (CException& e){}

    if (!m_bTerminated) m_nTermElapsedSecs = time(NULL);
    m_bTerminated = true;
}

//-----------------------------------------------------------------------------
// ����: ȡ�ôӵ��� Terminate ����ǰ����������ʱ��(��)
//-----------------------------------------------------------------------------
int CThreadImpl::GetTermElapsedSecs() const
{
    int nResult = 0;

    // ����Ѿ�֪ͨ�˳������̻߳�����
    if (m_bTerminated && m_nThreadId != 0)
    {
        nResult = time(NULL) - m_nTermElapsedSecs;
    }

    return nResult;
}

//-----------------------------------------------------------------------------
// ����: �����Ƿ� Terminate
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
// ����: �߳�ִ�к���
// ����:
//   pArg - �̲߳������˴�ָ�� CWin32ThreadImpl ����
//-----------------------------------------------------------------------------
DWORD WINAPI ThreadExecProc(void *pArg)
{
    CWin32ThreadImpl *pThreadImpl = (CWin32ThreadImpl*)pArg;
    int nReturnValue = 0;

    {
        // ���� AutoFinalizer �����Զ����ƺ���
        struct CAutoFinalizer
        {
            CWin32ThreadImpl *m_pThreadImpl;
            CAutoFinalizer(CWin32ThreadImpl *pThreadImpl) { m_pThreadImpl = pThreadImpl; }
            ~CAutoFinalizer()
            {
                m_pThreadImpl->SetFinished(true);
                //m_pThreadImpl->SetThreadId(0);//ΪʲôҪ���õ���0�����ú󣬾Ͳ����ⲿɱ���߳�
                if (m_pThreadImpl->GetFreeOnTerminate()) delete m_pThreadImpl->GetThread();
            }
        } AutoFinalizer(pThreadImpl);

        if (!pThreadImpl->m_bTerminated)
        {
            try { pThreadImpl->Execute(); } catch (CException& e) { ; }

            // �����̷߳���ֵ
            nReturnValue = pThreadImpl->m_nReturnValue;
        }
    }
    
    ExitThread(nReturnValue);
    return NULL;
}

//-----------------------------------------------------------------------------
// ����: ���캯��
//-----------------------------------------------------------------------------
CWin32ThreadImpl::CWin32ThreadImpl(CThread *pThread) :
    CThreadImpl(pThread),
    m_nHandle(0),
    m_nPriority(THREAD_PRI_NORMAL)
{
}

//-----------------------------------------------------------------------------
// ����: ��������
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
// ����: �̴߳�����
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::CheckThreadError(bool bSuccess)
{
    if (!bSuccess)
        throw CException(SysErrorMessage(GetLastError()).c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: �����̲߳�ִ��
// ע��: �˳�Ա�����ڶ�������������ֻ�ɵ���һ�Ρ�
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Run()
{
    CheckNotRunning();

    m_nHandle = CreateThread(NULL, 0, ThreadExecProc, (LPVOID)this, 0, (LPDWORD)&m_nThreadId);
    CheckThreadError(m_nHandle != 0);

    // �����߳����ȼ�
    if (m_nPriority != THREAD_PRI_NORMAL)
        SetPriority(m_nPriority);
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ�߳��˳�
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Terminate()
{
    CThreadImpl::Terminate();
}

//-----------------------------------------------------------------------------
// ����: ǿ��ɱ���߳�
// ע��: 
//   1. ���ô˺����󣬶��߳�������һ�в����Բ�����(Terminate(); WaitFor(); delete pThread; ��)��
//   2. �̱߳�ɱ�����û��������ĳЩ��Ҫ��Դ����δ�ܵõ��ͷţ���������Դ (��δ���ü�����
//      �㱻ɱ��)��������Ҫ��Դ���ͷŹ��������� BeforeKill �н��С�
//   3. Win32 ��ǿɱ�̣߳��߳�ִ�й����е�ջ���󲻻��������� Kill() ����ʽ�ͷ� CThread ����
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Kill()
{
    if (m_nThreadId != 0)
    {
        // �û����ڴ˴��ͷ���Ҫ��Դ�������ͷ�����Դ
        try { BeforeKill(); } catch (CException& e) {}

        m_nThreadId = 0;
        TerminateThread(m_nHandle, 0);

        // Win32 ��ǿɱ�̣߳���������ջ���󡣹ʴ˴�������ʽ�ͷ� CThread ����
        delete (CThread*)&m_Thread;
    }
}

//-----------------------------------------------------------------------------
// ����: �ȴ��߳��˳�
// ����: �̷߳���ֵ
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
// ����: ����˯��״̬ (˯�߹����л��� m_bTerminated ��״̬)
// ����:
//   fSeconds - ˯�ߵ���������ΪС�����ɾ�ȷ������
// ע��:
//   ���ڽ�˯��ʱ��ֳ������ɷݣ�ÿ��˯��ʱ���С����ۼ���������������
//-----------------------------------------------------------------------------
void CWin32ThreadImpl::Sleep(double fSeconds)
{
    const double SLEEP_INTERVAL = 0.5;      // ÿ��˯�ߵ�ʱ��(��)
    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        NanoSleep(fOnceSecs, true);
    }
}

//-----------------------------------------------------------------------------
// ����: �����̵߳����ȼ�
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
// ����: �߳�ִ�к���
// ����:
//   pArg - �̲߳������˴�ָ�� CLinuxThreadImpl ����
//-----------------------------------------------------------------------------
void* ThreadExecProc(void *pArg)
{
    CLinuxThreadImpl *pThreadImpl = (CLinuxThreadImpl*)pArg;
    int nReturnValue = 0;

    {
        // ���� AutoFinalizer �����Զ����ƺ���
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

        // �̶߳� cancel �źŵ���Ӧ��ʽ������: (1)����Ӧ (2)�Ƴٵ�ȡ��������Ӧ (3)����������Ӧ��
        // �˴������߳�Ϊ��(3)�ַ�ʽ���������ϱ� cancel �ź���ֹ��
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

        if (!pThreadImpl->m_bTerminated)
        {
            // �߳���ִ�й��������յ� cancel �źţ����׳�һ���쳣(������ʲô���͵��쳣��)��
            // ���쳣ǧ�򲻿�ȥ������( try{}catch(...){} )��ϵͳ���������쳣�Ƿ񱻳���������
            // ���ǣ���ᷢ�� SIGABRT �źţ������ն���� "FATAL: exception not rethrown"��
            // ���Դ˴��Ĳ�����ֻ���� CException �쳣(��ISE�������쳣�Դ� CException �̳�)��
            // �� pThread->Execute() ��ִ�й����У��û�Ӧ��ע����������:
            // 1. ����մ˴�������ȥ�����쳣�����в��������������͵��쳣( ��catch(...) );
            // 2. �����׳� CException ��������֮����쳣�������׳�һ������( �� throw 5; )��
            //    ϵͳ����Ϊû�д��쳣�Ĵ����������� abort��(������ˣ�ThreadFinalProc
            //    �Ի��� pthread_cleanup_push ����ŵ��������ִ�е���)
            try { pThreadImpl->Execute(); } catch (CException& e) { ; }

            // �����̷߳���ֵ
            nReturnValue = pThreadImpl->m_nReturnValue;
        }

        // ���� cancel �źţ����˵�ǰ scope �󽫲��ɱ�ǿ����ֹ
        pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
    }
    
    pthread_exit((void*)nReturnValue);
    return NULL;
}

//-----------------------------------------------------------------------------
// ����: ���캯��
//-----------------------------------------------------------------------------
CLinuxThreadImpl::CLinuxThreadImpl(CThread *pThread) :
    CThreadImpl(pThread),
    m_nPolicy(THREAD_POL_DEFAULT),
    m_nPriority(THREAD_PRI_DEFAULT)
{
}

//-----------------------------------------------------------------------------
// ����: ��������
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
// ����: �̴߳�����
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::CheckThreadError(int nErrorCode)
{
    if (nErrorCode != 0)
        throw CException(SysErrorMessage(nErrorCode).c_str(), __FILE__, __LINE__);
}

//-----------------------------------------------------------------------------
// ����: �����̲߳�ִ��
// ע��: �˳�Ա�����ڶ�������������ֻ�ɵ���һ�Ρ�
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Run()
{
    CheckNotRunning();

    // �����߳�
    CheckThreadError(pthread_create((pthread_t*)&m_nThreadId, NULL, ThreadExecProc, (void*)this));

    // �����̵߳��Ȳ���
    if (m_nPolicy != THREAD_POL_DEFAULT)
        SetPolicy(m_nPolicy);
    // �����߳����ȼ�
    if (m_nPriority != THREAD_PRI_DEFAULT)
        SetPriority(m_nPriority);
}

//-----------------------------------------------------------------------------
// ����: ֪ͨ�߳��˳�
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Terminate()
{
    CThreadImpl::Terminate();
}

//-----------------------------------------------------------------------------
// ����: ǿ��ɱ���߳�
// ע��: 
//   1. ���ô˺����󣬶��߳�������һ�в����Բ�����(Terminate(); WaitFor(); delete pThread; ��)��
//   2. ��ɱ���߳�ǰ��m_bFreeOnTerminate ���Զ���Ϊ true���Ա�������Զ��ͷš�
//   3. �̱߳�ɱ�����û��������ĳЩ��Ҫ��Դ����δ�ܵõ��ͷţ���������Դ (��δ���ü�����
//      �㱻ɱ��)��������Ҫ��Դ���ͷŹ��������� DoKill �н��С�
//   4. �߳����յ� cancel �źź��������Լ�ʱ�����������׳�һ���쳣�����쳣�������̺߳���
//      ��ִ�����̡����ԣ��̺߳����е�����ջ������Ȼ����������˳��������
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Kill()
{
    if (m_nThreadId != 0)
    {
        // �û����ڴ˴��ͷ���Ҫ��Դ�������ͷ�����Դ
        try { BeforeKill(); } catch (CException& e) {}

        pthread_t nThreadId = m_nThreadId;
        m_nThreadId = 0;

        SetFreeOnTerminate(true);
        pthread_cancel(nThreadId);
    }
}

//-----------------------------------------------------------------------------
// ����: �ȴ��߳��˳�
// ����: �̷߳���ֵ
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
// ����: ����˯��״̬ (˯�߹����л��� m_bTerminated ��״̬)
// ����:
//   fSeconds - ˯�ߵ���������ΪС�����ɾ�ȷ������
// ע��:
//   ���ڽ�˯��ʱ��ֳ������ɷݣ�ÿ��˯��ʱ���С����ۼ���������������
//-----------------------------------------------------------------------------
void CLinuxThreadImpl::Sleep(double fSeconds)
{
    const double SLEEP_INTERVAL = 0.5;      // ÿ��˯�ߵ�ʱ��(��)
    double fOnceSecs;

    while (!GetTerminated() && fSeconds > 0)
    {
        fOnceSecs = (fSeconds >= SLEEP_INTERVAL ? SLEEP_INTERVAL : fSeconds);
        fSeconds -= fOnceSecs;

        NanoSleep(fOnceSecs, true);
    }
}

//-----------------------------------------------------------------------------
// ����: �����̵߳ĵ��Ȳ���
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
// ����: �����̵߳����ȼ�
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
