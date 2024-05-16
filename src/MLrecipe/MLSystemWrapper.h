#pragma once

#include <filesystem>

#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include <QtCore/QCoreApplication>

//#include "coreplugin/metricsdata.h"
#include "../PluginCore/service/ml.h"
// #include "FileConfig.h"

class MLSystemWrapper : public QObject
{
    Q_OBJECT

  public:
    MLSystemWrapper(QObject *parent = nullptr);
    ~MLSystemWrapper();

    void Invoke(QString cmd, QString param);
    CORE::WrapperStatus GetStatus();

  signals:
    void SendStatusSignal(CORE::WrapperStatus, QString);
    void SelectDUTTypeSignal(int);
    void SendInternalLightSignal(QString);

  private:
    CORE::WrapperStatus m_Status;
    void delay(int ms);
    // fileConfig m_fileConfig;
    void initTestinfo();
};
