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
// 提前声明

class CHandlerWorkerThread;
class CThreadHandler;

///////////////////////////////////////////////////////////////////////////////
// 常量定义

// 工作者线程的默认休息时间(毫秒)
const int DEF_THREAD_SHEEP_MSECS = 150;

///////////////////////////////////////////////////////////////////////////////
// class CHandlerWorkerThread - 用于 CThreadHandler 的工作者线程

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
// class CThreadHandler - 线程处理器

class CThreadHandler
{
friend class CHandlerWorkerThread;

private:
    CHandlerWorkerThread *m_pWorkerThread;  // 工作者线程
protected:
    bool m_bThreadNeedSleep;                // 线程是否需要休息(提供给Handle()函数修改)
    double m_fThreadSleepSeconds;           // 线程每次休息的时间(秒)

protected:
    // 处理器工作函数，由线程循环执行
    // 返回:
    //   true  - 线程无需休息，马上执行下一轮循环
    //   false - 线程需稍做休息
    virtual void Handle() = 0;
public:
    CThreadHandler();
	virtual ~CThreadHandler();
	HANDLE GegThreadHandle();

    // 启/停工作者线程
    void StartWorkerThread();
    void StopWorkerThread();
};

///////////////////////////////////////////////////////////////////////////////

#endif // _THD_HANDLER_H_
