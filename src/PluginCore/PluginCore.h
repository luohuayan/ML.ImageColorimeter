#ifndef PLUGINCORE_H
#define PLUGINCORE_H

#include "plugincore_global.h"
#include <QObject>
#include <QVector>
#include <QHash>
#include <QPluginLoader>
#include <QMap>

#define TRANS(name) #name

class PLUGINCORE_EXPORT PluginCore : public QObject
{
    Q_OBJECT
private:
    PluginCore();
   
    void loadPlugin();

    void loadPluginDiscard();

    bool SortDllFromJson(std::vector<std::string>& allPluginPath);
    
    bool ReadFromJson(QString jsonPath, QList<QString>& JsonContent);

    bool CreatePlugin(QString pluginAbstractName, QString pluginDescribe = "");

    bool PluginCopy(QString& srcPath, QString pluginDescribe);
    bool deleteTemporaryFolder(QString path); //Delete plugin Temporary folders and files

    void RecursiveStatisticsPluginNumber(std::string currentPluginName, std::map<std::string, int>& pluginPriority); //递归统计插件数量

public:
    ~PluginCore();
    static PluginCore* getinstance();

    template<class T>
    T* GetPlugin(QString pluginAbstractObjectName, QString pluginDescribe = "")
    {
        bool isHasKey = false;
        QMap<QString, QMap<QString, QObject*>>::iterator it = m_ProjectPluginList.begin();
        while (it != m_ProjectPluginList.end()) {
            if (it.key() == pluginAbstractObjectName)
            {
                if (pluginDescribe != "")
                {
                    if (m_isHavePlugin[pluginAbstractObjectName][pluginDescribe])
                    {
                        isHasKey = true;
                        break;
                    }

                    if (!CreatePlugin(pluginAbstractObjectName, pluginDescribe))
                    {
                        break;
                    }
                    
                }

                isHasKey = true;
                break;
            }
            ++it;
        }
        T* pluginAbstract = NULL;
        if (isHasKey)
        {
               pluginAbstract = qobject_cast<T*>(m_ProjectPluginList[pluginAbstractObjectName][pluginDescribe]);
        }
        return pluginAbstract;
    }

    /*template<class T1>
    QString GetPluginDescribe(T1* pluginObj)
    {
        QObject* obj = dynamic_cast<QObject*>(pluginObj);
        for (auto it = m_ProjectPluginList.begin(); it != m_ProjectPluginList.end(); it++)
        {
            for (auto gt = it.value().begin(); gt != it.value().end(); gt++)
            {
                if (obj == gt.value()) return gt.key();
            }
        }
        return "NULL";
    }*/

    std::vector<QString> GetPluginAllDescribe(QString pluginName);
private:
    //QVector<QObject*> m_validPluginList;
    QMap<QString, QMap<QString, QObject*>> m_ProjectPluginList;
    QHash<QString, QString> m_pluginPath;
    QMap<QString, QMap<QString, bool>> m_isHavePlugin;
    static PluginCore* m_PluginCore;
    std::vector<QPluginLoader*> m_PluginLoader;
    std::vector<QString> m_sortAllPlugin;
    std::map<std::string, std::string> GetPluginPathByPluginName;
};

#endif