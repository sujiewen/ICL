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
/* ˵��

һ��Win32ƽ̨�º�Linuxƽ̨���̵߳���Ҫ����:

    1. Win32�߳�ӵ��Handle��ThreadId����Linux�߳�ֻ��ThreadId��
    2. Win32�߳�ֻ��ThreadPriority����Linux�߳���ThreadPolicy��ThreadPriority��
    3. Win32�߳���ǿ��ɱ��ʱ���߳�ִ�й����е�ջ���󲻻���������Linux�߳���ᡣ

*/
///////////////////////////////////////////////////////////////////////////////
// ���Ͷ���

#ifdef ICL_WIN32
// �߳����ȼ�
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
// �̵߳��Ȳ���
enum {
    THREAD_POL_DEFAULT      = SCHED_OTHER,
    THREAD_POL_RR           = SCHED_RR,
    THREAD_POL_FIFO         = SCHED_FIFO
};

// �߳����ȼ�
enum {
    THREAD_PRI_DEFAULT     = 0,
    THREAD_PRI_MIN         = 0,
    THREAD_PRI_MAX         = 99,
    THREAD_PRI_HIGH        = 80
};
#endif

///////////////////////////////////////////////////////////////////////////////
// class CThreadImpl - ƽ̨�߳�ʵ�ֻ���

class CThreadImpl
{
friend class CThread;

protected:
    CThread& m_Thread;              // ������� CThread ����
    int m_nThreadId;                // �߳�ID
    bool m_bFinished;               // �߳��Ƿ���������̺߳�����ִ��
    int m_nTermElapsedSecs;         // �ӵ��� Terminate ����ǰ����������ʱ��(��)
    bool m_bFreeOnTerminate;        // �߳��˳�ʱ�Ƿ�ͬʱ�ͷ������
    bool m_bTerminated;             // �Ƿ�Ӧ�˳��ı�־
    int m_nReturnValue;             // �̷߳���ֵ (���� Execute �������޸Ĵ�ֵ������ WaitFor ���ش�ֵ)

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

    // ���� (getter)
    CThread* GetThread() { return (CThread*)&m_Thread; }
    int GetThreadId() const { return m_nThreadId; }
    int GetTerminated() const { return m_bTerminated; }
    int GetReturnValue() const { return m_nReturnValue; }
    bool GetFreeOnTerminate() const { return m_bFreeOnTerminate; }
    int GetTermElapsedSecs() const;
    // ���� (setter)
    void SetThreadId(int nValue) { m_nThreadId = nValue; }
    void SetFinished(bool bValue) { m_bFinished = bValue; }
    void SetTerminated(bool bValue);
    void SetReturnValue(int nValue) { m_nReturnValue = nValue; }
    void SetFreeOnTerminate(bool bValue) { m_bFreeOnTerminate = bValue; }
};

///////////////////////////////////////////////////////////////////////////////
// class CWin32ThreadImpl - Win32ƽ̨�߳�ʵ����

#ifdef ICL_WIN32
class CWin32ThreadImpl : public CThreadImpl
{
friend DWORD WINAPI ThreadExecProc(void *pArg);

protected:
    HANDLE m_nHandle;               // �߳̾��
    int m_nPriority;                // �߳����ȼ�
    
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
// class CLinuxThreadImpl - Linuxƽ̨�߳�ʵ����

#ifdef ICL_LINUX
class CLinuxThreadImpl : public CThreadImpl
{
friend void* ThreadExecProc(void *pArg);

protected:
    int m_nPolicy;                  // �̵߳��Ȳ��� (THREAD_POLICY_XXX)
    int m_nPriority;                // �߳����ȼ� (0..99)

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
// class CThread - �߳���

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
    // �̵߳�ִ�к��������������д��
    virtual void Execute() {}

    // ִ�� Terminate() ǰ�ĸ��Ӳ�����
    // ע: ���� Terminate() ������Ը�˳����ƣ�Ϊ�������߳��ܾ����˳�������
    // m_bTerminated ��־����Ϊ true ֮�⣬��ʱ��Ӧ������һЩ���ӵĲ�����
    // �������߳̾�������������н��ѳ�����
    virtual void BeforeTerminate() {}

    // ִ�� Kill() ǰ�ĸ��Ӳ�����
    // ע: �̱߳�ɱ�����û��������ĳЩ��Ҫ��Դ����δ�ܵõ��ͷţ���������Դ
    // (��δ���ü������㱻ɱ��)��������Ҫ��Դ���ͷŹ��������� BeforeKill �н��С�
    virtual void BeforeKill() {}
public:
    CThread() : m_ThreadImpl(this) {}
    virtual ~CThread() {}

    // ������ִ���̡߳�
    // ע: �˳�Ա�����ڶ�������������ֻ�ɵ���һ�Ρ�
    void Run() { m_ThreadImpl.Run(); }

    // ֪ͨ�߳��˳� (��Ը�˳�����)
    // ע: ���߳�����ĳЩ����ʽ�����ٳٲ��˳����ɵ��� Kill() ǿ���˳���
    void Terminate() { m_ThreadImpl.Terminate(); }

    // ǿ��ɱ���߳� (ǿ���˳�����)
    void Kill() { m_ThreadImpl.Kill(); }

    // �ȴ��߳��˳�
    int WaitFor() { return m_ThreadImpl.WaitFor(); }

    // ����˯��״̬ (˯�߹����л��� m_bTerminated ��״̬)
    // ע: �˺����������߳��Լ����÷�����Ч��
    void Sleep(double fSeconds) { m_ThreadImpl.Sleep(fSeconds); }

    // ���� (getter)
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
    // ���� (setter)
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
