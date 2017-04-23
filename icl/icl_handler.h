///////////////////////////////////////////////////////////////////////////////
// P2SP Client Daemon (P2SP_CD)
// thd_handler.h
///////////////////////////////////////////////////////////////////////////////

#ifndef _THD_HANDLER_H_
#define _THD_HANDLER_H_

#include "config.h"

#include "icl_classes.h"
#include "icl_thread.h"

using namespace icl;

///////////////////////////////////////////////////////////////////////////////
// ��ǰ����

class CHandlerWorkerThread;
class CThreadHandler;

///////////////////////////////////////////////////////////////////////////////
// ��������

// �������̵߳�Ĭ����Ϣʱ��(����)
const int DEF_THREAD_SHEEP_MSECS = 150;

///////////////////////////////////////////////////////////////////////////////
// class CHandlerWorkerThread - ���� CThreadHandler �Ĺ������߳�

class CHandlerWorkerThread : public CThread
{
private:
    CThreadHandler& m_Handler;
protected:
    virtual void Execute();
public:
    CHandlerWorkerThread(CThreadHandler *pHandler) :
        m_Handler(*pHandler) {}
    virtual ~CHandlerWorkerThread() {}
};


///////////////////////////////////////////////////////////////////////////////
// class CThreadHandler - �̴߳�����

class CThreadHandler
{
friend class CHandlerWorkerThread;

private:
    CHandlerWorkerThread *m_pWorkerThread;  // �������߳�
protected:
    bool m_bThreadNeedSleep;                // �߳��Ƿ���Ҫ��Ϣ(�ṩ��Handle()�����޸�)
    double m_fThreadSleepSeconds;           // �߳�ÿ����Ϣ��ʱ��(��)

protected:
    // �������������������߳�ѭ��ִ��
    // ����:
    //   true  - �߳�������Ϣ������ִ����һ��ѭ��
    //   false - �߳���������Ϣ
    virtual void Handle() = 0;
public:
    CThreadHandler();
	virtual ~CThreadHandler();
	HANDLE GegThreadHandle();

    // ��/ͣ�������߳�
    void StartWorkerThread();
    void StopWorkerThread();
};

///////////////////////////////////////////////////////////////////////////////

#endif // _THD_HANDLER_H_
