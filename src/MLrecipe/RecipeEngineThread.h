#ifndef RECIPE_ENGINE_THREAD_WIDGET_H
#define RECIPE_ENGINE_THREAD_WIDGET_H

#include <QDir>
#include <QMutex>
#include <QObject>
#include <QQueue>
#include <QStandardpaths>
#include <QThread>
#include <QVariant>
#include <QWaitCondition>
#include <QWidget>

#include "irecipewrapper.h"
#include "recipewrapperregistry.h"
#include "DispatchRecipeModule.h"
#include "RecipeDataModules.h"
#include "Session.h"
class RecipeEngineThread : public QThread
{
    Q_OBJECT

  public:
    void setAbort(bool value);
    Q_INVOKABLE void saveCurrentContextVar(QString value);
    Q_INVOKABLE bool saveResults(const QString &path);

    RecipeEngineThread(DispatchRecipeModule *dispatchModule);
    ~RecipeEngineThread();
    QString getFileName();
    void setFileName(QString newName);
    bool getMode();
    void setMode(bool newmode);

    bool getAbort();

    void SetMessageCommand(QString value);
    QString GetMessageCommand();
    bool IsEmptyMessageCommand();
    bool getIsOperator();
    void run();

    QWaitCondition waitObj;
    QMutex mutex;
    QWaitCondition WaitNewMessageCommand;
    QString CurrentFile;       // stores current module opened file
    QString CurrentContextVar; // stores current custom context variable

  signals:
    void finishedLoadingModulesQml();
    void moduleFailedQml(QString msg);
    void modulePassedQml();
    void doneQml();
    void errorQml(bool fatal, QString msg);
    void fileNameChanged();
    void modeChanged();
    void emgStop();
    void operateChanged();

  private:
    bool operatorMode = false;
    bool parseModule(const QString &, QStringList &fields);
    bool parseString(int &index, const QString &, QStringList &fields, bool isParameter = false);
    DispatchRecipeModule *m_dispatchModule;
    RecipeDataModules m_modules;

    bool m_mode = true;      // stores current recipe mode
    int m_moduleDelay = 100; // number of seconds to delay befor executing next module
    void updateContextVar(QString value);
    bool isPause = false;

    QQueue<QString> m_messageCommand;
    QMutex m_mutex;

    bool m_abort;
    QMutex m_mutexAbort;
    QMutex m_mutexNewMessage;
};
#endif
