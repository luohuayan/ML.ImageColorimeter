
#include "RecipeEngineWidget.h"

RecipeEngineWidget::RecipeEngineWidget(DispatchRecipeModule *dispatchModule, QWidget *parent)
    : QQuickWidget(parent), m_recipeEngineThread(dispatchModule)
{
    ui.setupUi(this);
    this->setSource(QUrl("qrc:/qml/RecipeGui.qml"));
    this->setResizeMode(QQuickWidget::SizeRootObjectToView);
    this->rootContext()->setContextProperty("recipewidget", this);
    this->rootContext()->setContextProperty("_recipeDataModules", &m_modules);

    QObject *rootObject = this->rootObject();

    // signal from QML
    connect(rootObject, SIGNAL(executeModule(QString)), this, SLOT(execution(QString)));
    connect(rootObject, SIGNAL(startLoadingModules()), this, SLOT(onStartLoadingModules()));

    // signal from c++
    connect(dispatchModule, &DispatchRecipeModule::dispatchResponseToRecipeGui, this, &RecipeEngineWidget::onResponse);
    //connect(Core::ICore::instance(), &Core::ICore::emgStopBtnClicked, this, &RecipeEngineWidget::emgStopRecipe);

    if (operatorMode)
    {
        readFixRecipeFile();
    }
}

RecipeEngineWidget::~RecipeEngineWidget()
{
    // future.cancel();
}

bool RecipeEngineWidget::getPause()
{
    return isPause;
}
void RecipeEngineWidget::setPause(bool val)
{
    if (isPause && !val)
    {
        isPause = val;
        m_recipeEngineThread.waitObj.wakeOne();
    }
    isPause = val;
    RecipeWrapperRegistry::Instance()->notifyPauseStatus(val);
    ;
}
bool RecipeEngineWidget::getIsOperator()
{
    return operatorMode;
}

bool RecipeEngineWidget::getStatus()
{
    return m_recipeEngineThread.isRunning();
}
void RecipeEngineWidget::setStatus(bool s)
{
    RecipeWrapperRegistry::Instance()->notifyStopStatus(!s);
    emit statusChanged();
    if (isPause)
    {
        m_recipeEngineThread.waitObj.wakeOne();
        isPause = false;
    }
}

QString RecipeEngineWidget::getFileName()
{
    return m_recipeEngineThread.CurrentFile;
}

void RecipeEngineWidget::setFileName(QString newName)
{
    m_recipeEngineThread.CurrentFile = newName;
    emit fileNameChanged();
}

bool RecipeEngineWidget::getMode()
{
    return m_mode;
}

void RecipeEngineWidget::setMode(bool newmode)
{
    m_mode = newmode;
}

QString RecipeEngineWidget::urlToNativeFilePath(const QUrl &url) const
{
    return url.toLocalFile();
}

void RecipeEngineWidget::saveDUTID(QString value)
{
    saveCurrentContextVar(value);
    emit doneQml();
}

void RecipeEngineWidget::saveCurrentContextVar(QString value)
{
    Session::getInstance()->save(m_recipeEngineThread.CurrentContextVar, value);
    m_recipeEngineThread.CurrentContextVar = "";
}

void RecipeEngineWidget::readFile()
{
    QFile file(m_recipeEngineThread.CurrentFile);
    if (!file.open(QIODevice::ReadOnly))
    {
        // LoggingWrapper::getinstance()->debug("Could not open file");
        return;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        line = line.trimmed();
        if (line != "" && line.at(0) != "#") // Todo remove all white spaces and trim
        {
            m_modules.addModule(line);
        }
    }
    file.close();
}

inline bool RecipeEngineWidget::parseString(int &index, const QString &msg, QStringList &fields, bool isParameter)
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

void RecipeEngineWidget::updateContextVar(QString msg)
{
    QStringList msgLst = msg.split("%");
    if (msgLst.size() == 3)
    {
        saveCurrentContextVar(msgLst[2]);
    }
}

void RecipeEngineWidget::onStartLoadingModules()
{
    m_modules.clear();
    readFile();
    emit finishedLoadingModulesQml();
}
bool RecipeEngineWidget::saveResults(const QString &path)
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
        // LoggingWrapper::getinstance()->debug("Could not save contextVariables");
        return false;
    }
    // LoggingWrapper::getinstance()->debug("Results saved");
    return true;
}

void RecipeEngineWidget::execution(QString msg)
{
    m_recipeEngineThread.SetMessageCommand(msg);
    qDebug() << "set message: " << QThread::currentThread() << msg;
    if (m_recipeEngineThread.isRunning() == false)
    {
        m_recipeEngineThread.start();
    }
    else
    {
        m_recipeEngineThread.WaitNewMessageCommand.wakeAll();
    }
}

void RecipeEngineWidget::onResponse(CORE::WrapperStatus s, QString msg)
{
    QString str = "=====Recipe engine===== " + msg;
    switch (s)
    {
    case CORE::Wrapper_Init:
        break;
    case CORE::Wrapper_Start:
        break;
    case CORE::Wrapper_Done:
        // LoggingWrapper::getinstance()->debug(str.toStdString());
        if (str.contains("%updateVar:%"))
        {
            updateContextVar(msg);
        }
        emit doneQml();
        break;
    case CORE::Wrapper_Error:
        // LoggingWrapper::getinstance()->debug(str.toStdString());
        if (m_mode)
        {
            emit userResponseQml("Error.qml", msg, false);
        }
        else
        {
            emit errorQml(false, msg);
        }
        break;
    case CORE::Wrapper_Metrics_Cal_Error:
        if (m_mode)
        {
            emit userResponseQml("Error.qml", msg, false);
        }
        else
        {
            emit errorQml(false, msg);
        }
        break;
    case CORE::Wrapper_AutoAOI_Error:
        break;
    case CORE::Wrapper_Image_Invalid:
        break;
    case CORE::Wrapper_Session_Get_DUT_Info:
        emit userResponseQml("SessionDialogDut.qml",
                             "Enter Serial Number:", !m_recipeEngineThread.CurrentContextVar.isEmpty());
        break;
    case CORE::Wrapper_Session_start:
        emit userResponseQml("SessionDialogStart.qml", "Please mount Dut onto hexapod", false);
        break;
    case CORE::Wrapper_Session_stop:
        emit userResponseQml("SessionDialogStop.qml", "Please remove Dut from hexapod", false);
        break;
    case CORE::Wrapper_Error_fatal:
        if (m_mode)
        {
            emit userResponseQml("Fatal.qml", msg, false);
        }
        else
        {
            emit errorQml(true, msg);
        }
        break;
    case CORE::Wrapper_Test_Result:
        if (m_mode)
        {
            emit userResponseQml("TestResult.qml", msg, false);
            break;
        }
        else
        {
            if (str.contains("%updateVar:%"))
            {
                updateContextVar(msg);
            }
            emit doneQml();
            break;
        }
    case CORE::Wrapper_Session_save:
        bool success = saveResults(msg);
        if (!success)
        {
            if (m_mode)
            {
                emit userResponseQml("Error.qml", "Could not save data", false);
            }
            else
            {
                emit errorQml(false, "Could not save data");
            }
        }
        else
        {
            emit doneQml();
        }
        break;
    }
}

void RecipeEngineWidget::emgStopRecipe()
{
    emit emgStop();
}

void RecipeEngineWidget::readFixRecipeFile()
{
    //QString fieeContent = LoginData::instance()->getRecipeFileContent();
    //QStringList lst = fieeContent.split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
    //for each (QString line in lst)
    //{
    //    line = line.trimmed();
    //    if (line != "" && line.at(0) != "#") // Todo remove all white spaces and trim
    //    {
    //        m_modules.addModule(line);
    //    }
    //}
    //emit finishedLoadingModulesQml();
}

void RecipeEngineWidget::recipeStarted()
{ /* LoggingWrapper::getinstance()->debug("Recipe Started"); */
    m_recipeEngineThread.setAbort(false);
    m_recipeEngineThread.WaitNewMessageCommand.wakeAll();
}
void RecipeEngineWidget::recipeEnded()
{ /*LoggingWrapper::getinstance()->debug("Recipe Ended");*/
    m_recipeEngineThread.setAbort(true);
    m_recipeEngineThread.WaitNewMessageCommand.wakeAll();
}
void RecipeEngineWidget::recipeStopped()
{ /*LoggingWrapper::getinstance()->debug("Recipe Stopped");*/
}
