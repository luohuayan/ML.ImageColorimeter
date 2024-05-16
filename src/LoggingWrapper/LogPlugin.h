/*************************************************
Author:jason.huang
Date:2023.1.21
Version:0.0.1
Description:Provide a log interface to display the log
**************************************************/

#ifndef LOGPLUGIN_H
#define LOGPLUGIN_H
#include "loggingwrapper_global.h"
#include <QObject>
#include <QWidget>
#include <QtPlugin>

class LOGGINGWRAPPER_EXPORT LogPlugin
{
public:
    virtual ~LogPlugin() {}

    virtual void Info(std::string message) = 0;
    virtual void Warn(std::string message) = 0;
    virtual void Error(std::string message) = 0;

    virtual QWidget* GetLogWidget() = 0;
    virtual void SetParent(QWidget* widget) = 0;
};
//Plug-in identifier
#define AbstractInterface_iid "log plugin"
//Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(LogPlugin, AbstractInterface_iid)

#endif
