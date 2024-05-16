#include "RecipeEngineThread.h"
#include <QDebug>
#include "../PluginCore/service/ml.h"
RecipeEngineThread::RecipeEngineThread(DispatchRecipeModule *dispatchModule) : QThread()
{
    this->setObjectName("Recipe engine");
    m_dispatchModule = dispatchModule;
}

RecipeEngineThread::~RecipeEngineThread()
{
    setAbort(true);
    if (this->wait(5000) == false)
    {
        this->terminate();
        this->wait();
    }
}

bool RecipeEngineThread::getIsOperator()
{
    return operatorMode;
}

QString RecipeEngineThread::getFileName()
{
    return CurrentFile;
}

void RecipeEngineThread::setFileName(QString newName)
{
    CurrentFile = newName;
    emit fileNameChanged();
}

bool RecipeEngineThread::getMode()
{
    return m_mode;
}

void RecipeEngineThread::setMode(bool newmode)
{
    m_mode = newmode;
}

bool RecipeEngineThread::getAbort()
{
    QMutexLocker locker(&m_mutexAbort);
    return m_abort;
}

void RecipeEngineThread::setAbort(bool value)
{
    QMutexLocker locker(&m_mutexAbort);
    m_abort = value;
}

void RecipeEngineThread::saveCurrentContextVar(QString value)
{
    Session::getInstance()->save(CurrentContextVar, value);
    CurrentContextVar = "";
}

void RecipeEngineThread::SetMessageCommand(QString value)
{
    QMutexLocker locker(&m_mutex);
    m_messageCommand.enqueue(value);
}

QString RecipeEngineThread::GetMessageCommand()
{
    QMutexLocker locker(&m_mutex);
    return m_messageCommand.dequeue();
}

bool RecipeEngineThread::IsEmptyMessageCommand()
{
    QMutexLocker locker(&m_mutex);
    return m_messageCommand.isEmpty();
}

inline bool RecipeEngineThread::parseString(int &index, const QString &msg, QStringList &fields, bool isParameter)
{
    int start = index, balance = 0;
    QString token;

    while (index < msg.size())
    {
        if (msg.at(index) == ':' && !isParameter)
        {
            break;
        }
        if (msg.at(index) == '$')
        {
            if (balance == 0)
            {
                token.append(msg.mid(start, index - start));
                start = index;
                balance += 1;
            }
            else
            {
                auto a = Session::getInstance()->get(msg.mid(start, index - start + 1));
                start = index + 1;
                token.append(a);
                balance -= 1;
            }
        }
        index++;
    }
    token.append(msg.mid(start, index - start));
    if (token != "" && balance == 0)
        fields << token;
    index++;
    return balance != 0 && !token.isEmpty();
}

void RecipeEngineThread::updateContextVar(QString msg)
{
    QStringList msgLst = msg.split("%");
    if (msgLst.size() == 3)
    {
        saveCurrentContextVar(msgLst[2]);
    }
}

inline bool RecipeEngineThread::parseModule(const QString &msg, QStringList &fields)
{
    Session::getInstance()->loadReservedWords(); // params
    int index = 0;
    if (msg.at(0) == '$') // Equation
    {
        index++;
        while (msg.at(index) != '$')
        {
            index++;
        }
        CurrentContextVar = msg.mid(0, index + 1); // save variable
        index += 2;                                // skip equalsign
    }

    parseString(index, msg, fields); // model
    parseString(index, msg, fields); // command
    parseString(index, msg, fields, true);
    Session::getInstance()->loadReservedWords(); // params
    return true;
}

void RecipeEngineThread::run()
{
    setAbort(false);
    qDebug() << "Start run: " << QThread::currentThread();

    while (getAbort() == false)
    {
        if (IsEmptyMessageCommand())
        {
            const QMutexLocker locker(&m_mutex);
            WaitNewMessageCommand.wait(&m_mutex);
            continue;
        }
        QStringList fields;
        parseModule(GetMessageCommand(), fields);
        if (fields.size() == 1)
        {
            if (CurrentContextVar != "")
            {
                saveCurrentContextVar(fields[0]);
                emit m_dispatchModule->dispatchResponseToRecipeGui(CORE::Wrapper_Done, "");
            }
            else
            {
                emit m_dispatchModule->dispatchResponseToRecipeGui(CORE::Wrapper_Error, "Invalid input format");
            }
        }
        else if (fields.size() == 2)
        {
            m_dispatchModule->dispatch(fields[0], fields[1]); // model, cmd
        }
        else if (fields.size() == 3)
        {
            m_dispatchModule->dispatch(fields[0], fields[1], fields[2]); // model, cmd, params
        }
        else
        {
            emit m_dispatchModule->dispatchResponseToRecipeGui(CORE::Wrapper_Error, "Invalid input format");
        }
    }
}

bool RecipeEngineThread::saveResults(const QString &path)
{
    // saving recipeResults                                          //TODO update reciperesults module with saving
    auto recipeData = m_modules.getData();
    QString filename = path + "/recipeResult.csv";
    if (QFile::exists(filename))
    {
        QFile::remove(filename);
    }

    QFile file(filename);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << "module"
               << ","
               << "result"
               << ","
               << "error" << endl;
        for (auto i : recipeData)
        {
            stream << i.name << "," << i.result << "," << i.error << endl;
        }
        file.close();
    }
    else
    {
        // LoggingWrapper::getinstance()->debug("Could not save recipeResults");
        return false;
    }

    // saving contextVariables
    auto recipeContext = Session::getInstance()->getData();
    filename = path + "/recipeContext.csv";
    if (QFile::exists(filename))
    {
        QFile::remove(filename);
    }

    file.setFileName(filename);
    if (file.open(QIODevice::ReadWrite))
    {
        QTextStream stream(&file);
        stream << "ContextVariables"
               << ","
               << "Values" << endl;
        for (auto i : recipeContext.keys())
        {
            stream << i << "," << recipeContext[i] << endl;
        }
        file.close();
    }
    else
    {
        return false;
    }
    return true;
}