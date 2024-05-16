#include "MLSystemWrapper.h"

void MLSystemWrapper::Invoke(QString cmd, QString param)
{
    if (cmd == "WaitForUser")
    {
        if (param == "/session/get_dut_info")
        {
            emit SendStatusSignal(CORE::Wrapper_Session_Get_DUT_Info, "Start");
        }
        else if (param == "/session/start")
        {
            emit SendStatusSignal(CORE::Wrapper_Session_start, "Start");
        }
        else if (param == "/session/stop")
        {
            emit SendStatusSignal(CORE::Wrapper_Session_stop, "Start");
        }
    }
    else if (cmd == "LoadDutConfiguration")
    {
    }
    else if (cmd == "DUTType")
    {
        if (param == "/DPA")
        {
            emit SelectDUTTypeSignal(0);
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, "");
        }
        else if (param == "/WaveGuide")
        {
            emit SelectDUTTypeSignal(1);
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, "");
        }
        else if (param == "/FullSystem")
        {
            emit SelectDUTTypeSignal(2);
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, "");
        }
        else
        {
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Error, "no such DUT type");
        }
    }
    else if (cmd == "Sleep")
    {
        MLSystemWrapper::delay(param.toInt());
    }
    else if (cmd == "Save")
    {
        QStringList p = param.split("_");
        if (p.size() != 2)
        {
            emit SendStatusSignal(CORE::Wrapper_Error, "Invalid folder parameter.Did not save data");
        }
        else
        {
            QString pathfolder = QString::fromStdString("./" /*m_fileConfig.outputDir*/) + p[1];
            if (!QDir().exists(pathfolder))
            {
                if (!QDir().mkdir(pathfolder))
                {
                    pathfolder = QDir::homePath();
                    // LoggingWrapper::getinstance()->debug("Invalid path. Saving to home directory");
                }
            }
            emit SendStatusSignal(CORE::Wrapper_Session_save, pathfolder);
        }
    }
    else if (cmd == "InternalLight")
    {
        if (param == "on" || param == "off")
        {
            emit SendInternalLightSignal(param);
            std::string msg = "Internal Light turned " + param.toStdString() + ".";
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Done, QString::fromStdString(msg));
        }
        else
        {
            emit SendStatusSignal(CORE::WrapperStatus::Wrapper_Error, "Please provide a param of either on or off");
        }
    }
    else if (cmd == "StartRecipe")
    {
        initTestinfo();
        emit SendStatusSignal(CORE::Wrapper_Done, "");
    }
}

MLSystemWrapper::~MLSystemWrapper()
{
}

MLSystemWrapper::MLSystemWrapper(QObject *parent) : QObject(parent)
{
    // m_fileConfig=FileConfig::GetInstance().ReadInfo();
}

CORE::WrapperStatus MLSystemWrapper::GetStatus()
{
    return m_Status;
}

void MLSystemWrapper::delay(int ms)
{
    QThread::msleep(ms);
    emit SendStatusSignal(CORE::Wrapper_Done, "");
}

void MLSystemWrapper::initTestinfo()
{
    // init Appraiser
    // QString appraiser = LoginData::instance()->getUserName();
    // MetricsDataBase::getInstance()->updateOneFieldValue("Appraiser", "testinfo", appraiser);
    // init Date
    QDate current_date = QDate::currentDate();
    QString currentDate = current_date.toString("yyyyMMdd");
    //MetricsData::instance()->setRecipeStartDate(currentDate);
    // init Time
    QTime current_time = QTime::currentTime();
    QString timeStr = current_time.toString("hh:mm:ss");
    //MetricsData::instance()->setRecipeStartTime(timeStr);
}
