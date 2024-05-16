#ifndef RECIPE_ENGINE_WIDGET_H
#define RECIPE_ENGINE_WIDGET_H

#include <QDir>
#include <QFuture>
#include <QMutex>
#include <QObject>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickWidget>
#include <QStandardpaths>
#include <QVariant>
#include <QWaitCondition>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>
#include "irecipewrapper.h"
#include "recipewrapperregistry.h"
#include "ui_RecipeEngineWidget.h"

#include "DispatchRecipeModule.h"
#include "RecipeDataModules.h"
#include "Session.h"
#include "mlrecipe_global.h"
#include "RecipeEngineThread.h"

class RecipeEngineWidget : public QQuickWidget
{
    Q_OBJECT
    Q_PROPERTY(bool status READ getStatus WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(QString file READ getFileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(bool mode READ getMode WRITE setMode NOTIFY modeChanged)
    Q_PROPERTY(bool pause READ getPause WRITE setPause NOTIFY statusChanged)
    Q_PROPERTY(bool operatorMode READ getIsOperator NOTIFY operateChanged)

  public:
    Q_INVOKABLE QString urlToNativeFilePath(const QUrl &url) const;
    Q_INVOKABLE void saveCurrentContextVar(QString value);
    Q_INVOKABLE void saveDUTID(QString value);
    Q_INVOKABLE bool saveResults(const QString &path);
    Q_INVOKABLE void recipeStarted();
    Q_INVOKABLE void recipeEnded();
    Q_INVOKABLE void recipeStopped();

    RecipeEngineWidget(DispatchRecipeModule *m_dispatchModule, QWidget *parent = Q_NULLPTR);
    ~RecipeEngineWidget();
    bool getStatus();
    void setStatus(bool s);
    QString getFileName();
    void setFileName(QString newName);
    bool getMode();
    void setMode(bool newmode);

    bool getPause();
    void setPause(bool val);
    bool getIsOperator();

  signals:
    void finishedLoadingModulesQml();
    void moduleFailedQml(QString msg);
    void modulePassedQml();
    void userResponseQml(QString load, QString msg, bool retrieveContext);
    void doneQml();
    void errorQml(bool fatal, QString msg);
    void statusChanged();
    void fileNameChanged();
    void modeChanged();
    void emgStop();
    void operateChanged();
  public slots:
    void onStartLoadingModules();
    void onResponse(CORE::WrapperStatus, QString);
    void execution(QString);
    void emgStopRecipe();

  private:
    void readFixRecipeFile();
    bool operatorMode = false;
    void readFile();
    bool parseString(int &index, const QString &, QStringList &fields, bool isParameter = false);
    Ui::RecipeEngineWidget ui;
    RecipeDataModules m_modules;

    bool m_mode = true;      // stores current recipe mode
    int m_moduleDelay = 100; // number of seconds to delay befor executing next module
    void updateContextVar(QString value);
    bool isPause = false;
    RecipeEngineThread m_recipeEngineThread;
    QString m_msg;
};
#endif // !RECIPE_ENGINE_WIDGET_H
