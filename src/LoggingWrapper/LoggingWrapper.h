#ifndef LOGGING_WRAPPER_H_
#define LOGGING_WRAPPER_H_

#include <QDockWidget>
#include <QHBoxLayout>
#include <QObject>
#include <QVBoxLayout>
#include <QWidget>
#include <QtPlugin>

#include "LogPlugin.h"
#include "loggingwrapper_global.h"
#include "qtmessagelogwidget.h"
// Inheriting QObject is to use Qt's meta-object system
class LOGGINGWRAPPER_EXPORT LoggingWrapper : public QObject {
    Q_OBJECT
    ///*
    //*Use Q_INTERFACES() macro tells Qt's meta-object system about the
    //interface and tells the meta-object system "I want to use this interface
    //*/
    // Q_INTERFACES(LogPlugin)
    ///*
    //* Use Q_ PLUGIN_ METADATA() macro export plug-in
    //*/
    // Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "LoggingWrapper.json")
 public:
    LoggingWrapper(QObject* parent = nullptr);
    ~LoggingWrapper();

 public:
    static LoggingWrapper* getinstance();
    QWidget* GetLogWidget();
    void SetParent(QWidget* widget);            // set widget attachment
    void SetLayoutV(QVBoxLayout* qVBoxLayout);  // set QVBoxLayout
    void SetLayoutH(QHBoxLayout* qHBoxLayout);  // set QHBoxLayout
    void SetDockWidget(QDockWidget* qDockWidget);

    void Info(std::string message);
    void Warn(std::string message);
    void Error(std::string message);
    void Debug(std::string message);

 private:
    LogWidget* loggingwidget = nullptr;
    static LoggingWrapper* m_loggingwrapper;

    bool m_isSetParent;
};

#endif