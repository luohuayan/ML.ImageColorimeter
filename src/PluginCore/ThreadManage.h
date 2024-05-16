#pragma once
#include <qtconcurrentrun.h>
#include "service/Result.h"
#include <QFutureWatcher> 
#include <QMutex>
#include <QObject>
#include <QWaitCondition>
#include "plugincore_global.h"
class PLUGINCORE_EXPORT ThreadManage : public QObject
{
    Q_OBJECT

  public:
    ThreadManage(int threadCountMax = 2);
    ~ThreadManage();

    bool reset(int count);

    template <typename T, typename Class> 
    Result waitRun(Class *object, T (Class::*fn)());

    template <typename T, typename Class, typename Param1, typename Arg1>
    Result waitRun(Class *object, T (Class::*fn)(Param1), const Arg1 &arg1);

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
    Result waitRun(Class *object, T (Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2);

    template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2,typename Param3, typename Arg3>
    Result waitRun(Class *object, T (Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3);
    
    Result result();

  private:
    bool before();
    bool after();

  private slots:
    void threadFinished();

  private:
    QList<QFutureWatcher<Result> *> m_watchers;
    int m_count;
    QMutex m_mutexWait;
    QMutex m_mutex;
    int m_currentN;
    QWaitCondition m_waitCondition;

    int m_total;
};

template <typename T, typename Class> 
Result ThreadManage::waitRun(Class *object, T (Class::*fn)())
{
    if (!before())
    {
        return Result(false, "Thread try lock failed.");
    }

    QFuture<Result> future = QtConcurrent::run(object, fn);
    m_watchers.at(m_currentN)->setFuture(future);

    if (!after())
    {
        return Result(false, "Thread failed.");
    }
    return Result();
}

template <typename T, typename Class, typename Param1, typename Arg1>
Result ThreadManage::waitRun(Class *object, T (Class::*fn)(Param1), const Arg1 &arg1)
{
    if(!before()){
        return Result(false, "Thread try lock failed.");
    }

    QFuture<Result> future = QtConcurrent::run(object, fn, arg1);    
    m_watchers.at(m_currentN)->setFuture(future);

    if (!after())
    {
        return Result(false, "Thread failed.");
    }
    return Result();
}

template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
Result ThreadManage::waitRun(Class *object, T (Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
{
    if (!before())
    {
        return Result(false, "Thread try lock failed.");
    }

    QFuture<Result> future = QtConcurrent::run(object, fn, arg1, arg2);
    m_watchers.at(m_currentN)->setFuture(future);

    if (!after())
    {
        return Result(false, "Thread failed.");
    }
    return Result();
}

template <typename T, typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3,
          typename Arg3>
Result ThreadManage::waitRun(Class *object, T (Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2,
                             const Arg3 &arg3)
{
    if (!before())
    {
        return Result(false, "Thread try lock failed.");
    }

    QFuture<Result> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
    m_watchers.at(m_currentN)->setFuture(future);

    if (!after())
    {
        return Result(false, "Thread failed.");
    }
    return Result();
}
