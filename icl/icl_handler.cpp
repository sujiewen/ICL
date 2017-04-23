///////////////////////////////////////////////////////////////////////////////
// P2SP Client Daemon (P2SP_CD)
//
// 文件名称: thd_handler.cpp
// 功能描述: 线程处理器
// 最后修改: 2005-12-20
///////////////////////////////////////////////////////////////////////////////

#include "icl_handler.h"

///////////////////////////////////////////////////////////////////////////////
// class CHandlerWorkerThread

void CHandlerWorkerThread::Execute()
{
    while (!GetTerminated())
    {
        try
        {
            m_Handler.m_bThreadNeedSleep = true;
            m_Handler.Handle();
        }
        catch (CException& e)
        {
            m_Handler.m_bThreadNeedSleep = true;
            Logger.Write(e);
        }

        if (m_Handler.m_bThreadNeedSleep)
            this->Sleep(m_Handler.m_fThreadSleepSeconds);
    }
}

///////////////////////////////////////////////////////////////////////////////
// class CThreadHandler

CThreadHandler::CThreadHandler() :
    m_pWorkerThread(NULL),
    m_bThreadNeedSleep(true),
    m_fThreadSleepSeconds((double)DEF_THREAD_SHEEP_MSECS / 1000)
{
}

CThreadHandler::~CThreadHandler()
{
    StopWorkerThread();
}

HANDLE CThreadHandler::GegThreadHandle()
{
	return m_pWorkerThread->GetHandle();
}

//-----------------------------------------------------------------------------
// 描述: 启动工作者线程
//-----------------------------------------------------------------------------
void CThreadHandler::StartWorkerThread()
{
    if (!m_pWorkerThread)
    {
        m_pWorkerThread = new CHandlerWorkerThread(this);
        m_pWorkerThread->Run();
    }
}

//-----------------------------------------------------------------------------
// 描述: 停止工作者线程
//-----------------------------------------------------------------------------
void CThreadHandler::StopWorkerThread()
{
    printf("停止线程开始\n");
    if (m_pWorkerThread)
    {
        m_pWorkerThread->Terminate();
        m_pWorkerThread->WaitFor();
        delete m_pWorkerThread;
        m_pWorkerThread = NULL;
    }
    printf("停止线程结束\n");
}

///////////////////////////////////////////////////////////////////////////////

