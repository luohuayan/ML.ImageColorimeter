#include "ThreadManage.h"
#include <QFuture>

ThreadManage::ThreadManage(int threadCountMax)
{
    for (int i = 0; i < threadCountMax; ++i)
    {
        QFutureWatcher<Result> *watcher = new QFutureWatcher<Result>();
        m_watchers.append(watcher);
        connect(watcher, SIGNAL(finished()), this, SLOT(threadFinished()));
    }
}

bool ThreadManage::before()
{
    m_mutex.lock();
    if (m_currentN == m_count - 1)
    {
        if (!m_mutexWait.tryLock())
        {
            m_mutex.unlock();
            return false;
        }
    }
    return true;
}

bool ThreadManage::after()
{
    if (m_currentN == m_count - 1)
    {
        m_mutex.unlock();
        m_waitCondition.wait(&m_mutexWait);
        m_mutexWait.unlock();
    }else{
        m_mutex.unlock();
    }
    ++m_currentN;
    return true;
}

Result ThreadManage::result()
{
    std::string errorInfo = "";
    for (int i = 0; i < m_total; ++i)
    {
        Result ret = m_watchers.at(i)->result();
        if (!ret.success){
            if(i == 0){
                errorInfo = ret.errorMsg;
            }else{
                errorInfo += " " + ret.errorMsg;
            }
        }
    }
    if (errorInfo != "")
    {
        return Result(false, errorInfo);
    }
    return Result();
}

ThreadManage::~ThreadManage()
{
    int count = m_watchers.size();
    for (int i = 0; i < count; ++i)
    {
        QFutureWatcher<Result> *watcher = m_watchers.at(i);
        delete watcher;
        watcher = nullptr;
    }
    m_watchers.clear();
}

bool ThreadManage::reset(int count)
{
    if (!m_mutexWait.tryLock())
    {
        return false;
    }
    m_count = count;
    m_currentN = 0;
    m_mutexWait.unlock();
    m_total = count;
    return true;
}

void ThreadManage::threadFinished()
{
    QMutexLocker locker(&m_mutex);
    --m_count;
    if(m_count <= 0){
        m_waitCondition.wakeOne();
    }
}
