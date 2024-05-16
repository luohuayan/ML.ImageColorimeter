#include "PluginCore.h"
#include <QDir>
#include <QDebug>
#include <QCoreApplication>
#include <fstream>
#include <string>
#include "json.hpp"
using json = nlohmann::json;
json m_JsonControl;
PluginCore* PluginCore::m_PluginCore = nullptr;


bool cmp(const std::pair<std::string, int> a, std::pair<std::string, int>b) {
    return a.second > b.second;
}

PluginCore::PluginCore()
{
}

PluginCore::~PluginCore()
{
    for (int i = m_sortAllPlugin.size() - 1; i >= 0 ; i--)
    {
        for (auto it = m_ProjectPluginList[m_sortAllPlugin[i]].begin(); it != m_ProjectPluginList[m_sortAllPlugin[i]].end(); it++)
        {
            delete it.value();
        }
    }

    for (int i = 0; i < m_PluginLoader.size(); i++)
    {
        delete m_PluginLoader[i];
    }
}

void PluginCore::loadPluginDiscard()
{
    QDir pluginsDir(QCoreApplication::applicationDirPath());
    qDebug() << qApp->applicationDirPath() << endl;
    /*
    * Traverse the files under the exe directory. If the instantiation succeeds, 
    * use qobject_ Cast () tests whether the plug-in implements the given interface.
    */
    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {

        //-- Use QPluginLoader to load plug-ins in the application
        //QPluginLoader 加载插件名
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));

        //Filter non-plugin
        if (!pluginLoader.load()) {
            qDebug() << fileName + " is not a plugin" << endl;
            continue;
        }

        //获取插件josn文件中信息
        QList<QString> jsonContent;
        QString jsonPath = QCoreApplication::applicationDirPath() + "/" + fileName.replace(fileName.indexOf("dll"), 3, "json");
        if (!ReadFromJson(jsonPath, jsonContent) || jsonContent[1] != "true") continue;
        QString abstractObjectName = jsonContent[0];
        m_pluginPath.insert(abstractObjectName, QCoreApplication::applicationDirPath() + "/" + fileName.replace(fileName.indexOf("json"), 4, "dll"));

        //--Another way: Use qobject_ Cast () tests whether the plug-in implements the given interface
       //Polymorphic creation of subclass objects
        QObject* plugin = pluginLoader.instance();
        

        //Convert it to an interface pointer, and then judge that it is not empty, which means normal loading
        if (plugin)
        {

            QMap<QString, QObject*> validPlugin;
            validPlugin.insert("", plugin);
            m_ProjectPluginList.insert(abstractObjectName, validPlugin);

            QMap<QString, bool> isHavePlugin;
            isHavePlugin.insert("", true);
            m_isHavePlugin.insert(abstractObjectName, isHavePlugin);
        }
    }
}

void PluginCore::loadPlugin()
{
    std::vector<std::string> allPluginPath;
    SortDllFromJson(allPluginPath);
    for (int i = 0; i < allPluginPath.size(); i++)
    {
        //-- Use QPluginLoader to load plug-ins in the application
        //QPluginLoader 加载插件名
        QString name = QString::fromStdString(allPluginPath[i]);
        QPluginLoader* pluginLoader = new QPluginLoader(QString::fromStdString(allPluginPath[i]));
        m_PluginLoader.push_back(pluginLoader); //when close to release
        //Filter non-plugin
        if (!pluginLoader->load()) {
            qDebug() << QString::fromStdString(allPluginPath[i]) + " is not a plugin" << endl;
            qDebug() << pluginLoader->errorString();
            continue;
        }

        //获取插件josn文件中信息
        QList<QString> jsonContent;
        QString jsonPath = QString::fromStdString(allPluginPath[i]).replace(QString::fromStdString(allPluginPath[i]).indexOf(".dll"), 4, ".json");
        if (!ReadFromJson(jsonPath, jsonContent) || jsonContent[1] != "true") continue;
        QString abstractObjectName = jsonContent[0];
        m_pluginPath.insert(abstractObjectName, QString::fromStdString(allPluginPath[i]));

        //--Another way: Use qobject_ Cast () tests whether the plug-in implements the given interface
       //Polymorphic creation of subclass objects
        QObject* plugin = pluginLoader->instance();

        //Convert it to an interface pointer, and then judge that it is not empty, which means normal loading
        if (plugin)
        {

            QMap<QString, QObject*> validPlugin;
            validPlugin.insert("", plugin);
            m_ProjectPluginList.insert(abstractObjectName, validPlugin);

            QMap<QString, bool> isHavePlugin;
            isHavePlugin.insert("", true);
            m_isHavePlugin.insert(abstractObjectName, isHavePlugin);
        }
    }
}

bool PluginCore::CreatePlugin(QString pluginAbstractName, QString pluginDescribe)
{
    QString pluginPath = "";
    QHash<QString, QString>::iterator it = m_pluginPath.begin();
    while (it != m_pluginPath.end()) {
        if (it.key() == pluginAbstractName)
        {
            pluginPath = it.value();
            break;
        }
        ++it;
    }

    if (pluginPath != "")
    {
        if (!PluginCopy(pluginPath, pluginDescribe))
        {
            return false;
        }

        //-- Use QPluginLoader to load plug-ins in the application
       //QPluginLoader 加载插件名
        QPluginLoader* pluginLoader = new QPluginLoader(pluginPath);
        m_PluginLoader.push_back(pluginLoader); //when close to release
        //Filter non-plugin
        if (!pluginLoader->load()) {
            return false;
        }

        //--Another way: Use qobject_ Cast () tests whether the plug-in implements the given interface
       //获取 QObject 示例；
        QObject* plugin = pluginLoader->instance();

        //Convert it to an interface pointer, and then judge that it is not empty, which means normal loading
        if (plugin)
        {

            if (m_ProjectPluginList[pluginAbstractName].count(pluginDescribe) < 1)
            {
                m_ProjectPluginList[pluginAbstractName].insert(pluginDescribe, plugin);
                m_isHavePlugin[pluginAbstractName].insert(pluginDescribe, true);
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

bool PluginCore::PluginCopy(QString& srcPath, QString pluginDescribe)
{
    QString src_path = srcPath.mid(0, srcPath.lastIndexOf("/") + 1);
    QString pluginName = srcPath.mid(srcPath.lastIndexOf("/") + 1);
    QString dest_path = src_path + "PluginTemporaryFile/";
    QString destPluginName = pluginName.replace(pluginName.indexOf(".dll"), 4, pluginDescribe + ".dll");

    QDir targetDir(dest_path);
    if (!targetDir.exists())//如果目标目录不存在，则进行创建
    {
        if (!targetDir.mkdir(targetDir.absolutePath()))
        {
            return false;
        }
    }
    //源数文件 包含路径 文件名
    QString sCurrentPath = srcPath;
    //备份文件 包含路径 文件名
    QString sBachUpPath = dest_path + destPluginName;
    srcPath = sBachUpPath;

    QFile currenFile(sCurrentPath);
    currenFile.copy(sBachUpPath);//copy函数不能直接创建文件夹,所有需要先把文件夹创建出来再进行拷贝

    return true;
}

bool PluginCore::deleteTemporaryFolder(QString path)
{
    if (path.isEmpty())
    {
        return false;
    }
    QDir dir(path);
    if (!dir.exists())
    {
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach(QFileInfo file, fileList)
    { //遍历文件信息
        if (file.isFile())
        { // 是文件，删除
            file.dir().remove(file.fileName());
        }
        else
        { // 递归调用函数，删除子文件夹
            deleteTemporaryFolder(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 这时候文件夹已经空了，再删除文件夹本身
}

bool PluginCore::ReadFromJson(QString jsonPath, QList<QString>& JsonContent)
{
    std::string path = jsonPath.toStdString();
    std::ifstream ifs(path.c_str(), std::fstream::in);

    if (ifs.fail())
    {
        return false;
        //throw std::runtime_error("Unable to open PeriscopeConfig File.");
    }
    ifs >> m_JsonControl;

    JsonContent.push_back(QString::fromStdString(m_JsonControl["Name"].get<std::string>()));
    JsonContent.push_back(QString::fromStdString(m_JsonControl["PluginApplication"].get<std::string>()));
    JsonContent.push_back(QString::fromStdString(m_JsonControl["Version"].get<std::string>()));
    JsonContent.push_back(QString::fromStdString(m_JsonControl["Vendor"].get<std::string>()));
    JsonContent.push_back(QString::fromStdString(m_JsonControl["Copyright"].get<std::string>()));
    JsonContent.push_back(QString::fromStdString(m_JsonControl["License"].get<std::string>()));
    JsonContent.push_back(QString::fromStdString(m_JsonControl["Description"].get<std::string>()));
    JsonContent.push_back(QString::fromStdString(m_JsonControl["Url"].get<std::string>()));
    ifs.close();
    return true;
}

bool PluginCore::SortDllFromJson(std::vector<std::string>& allPluginPath)
{
    std::map<std::string, int> pluginPriority; //key:pluginName, value:Priority
    std::map<std::string, std::string> GetPluginPathByJson; //key:pluginName, value:pluginDLLPath
    QDir pluginsDir(QCoreApplication::applicationDirPath());
    qDebug() << qApp->applicationDirPath() << endl;

    GetPluginPathByPluginName.clear();
    //Find the Json file of the plug-in and save the corresponding plug-in path
    foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
        QString JsonPath = pluginsDir.absoluteFilePath(fileName);
        if (fileName.contains(".json"))
        {
            std::string path = JsonPath.toStdString();
            std::ifstream ifs(path.c_str(), std::fstream::in);
            if (ifs.fail())
            {
                ifs.close();
                return false;
                //throw std::runtime_error("Unable to open PeriscopeConfig File.");
            }
            ifs >> m_JsonControl;
            if (m_JsonControl["PluginApplication"].empty())
            {
                ifs.close();
                continue;
            }

            std::string pluginPath = JsonPath.toStdString();
            GetPluginPathByPluginName.insert(std::pair<std::string, std::string>(m_JsonControl["Name"].get<std::string>(), pluginPath));
            int pos = pluginPath.rfind(".json");
            pluginPath.replace(pos, 5, ".dll");
            GetPluginPathByJson.insert(std::pair<std::string, std::string>(m_JsonControl["Name"].get<std::string>(), pluginPath));
            pluginPriority[m_JsonControl["Name"].get<std::string>()] = 0;
            ifs.close();
        }
    }

    //foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
    //    QString JsonPath = pluginsDir.absoluteFilePath(fileName);
    //    if (fileName.contains(".json"))
    //    {
    //        std::string path = JsonPath.toStdString();
    //        std::ifstream ifs(path.c_str(), std::fstream::in);
    //        if (ifs.fail())
    //        {
    //            ifs.close();
    //            return false;
    //            //throw std::runtime_error("Unable to open PeriscopeConfig File.");
    //        }
    //        ifs >> m_JsonControl;
    //        if (m_JsonControl["PluginApplication"].empty())
    //        {
    //            ifs.close();
    //            continue;
    //        }

    //        auto list = m_JsonControl["Dependencies"];
    //        for (auto& pluginName : list.items())
    //        {
    //            std::string plugin_name = pluginName.value()["Name"].get<std::string>();
    //            if (pluginPriority.count(plugin_name) == 1)
    //                pluginPriority[plugin_name] += 1;
    //        }
    //        ifs.close();
    //    }
    //}

    for (auto it = GetPluginPathByPluginName.begin(); it != GetPluginPathByPluginName.end(); it++)
        RecursiveStatisticsPluginNumber(it->second, pluginPriority);

    //sort pluginName
    std::vector<std::pair<std::string, int>>v(pluginPriority.begin(), pluginPriority.end());
    sort(v.begin(), v.end(), cmp);
    m_sortAllPlugin.clear();
    for (int i = 0; i < v.size(); i++)
    {
        m_sortAllPlugin.push_back(QString::fromStdString(v[i].first));
        allPluginPath.push_back(GetPluginPathByJson[v[i].first]);
    } 

    return true;
}

void PluginCore::RecursiveStatisticsPluginNumber(std::string jsonPath, std::map<std::string, int>& pluginPriority)
{
    std::ifstream ifs(jsonPath.c_str(), std::fstream::in);
    if (ifs.fail())
    {
        ifs.close();
        return;
        //throw std::runtime_error("Unable to open PeriscopeConfig File.");
    }
    ifs >> m_JsonControl;
    if (m_JsonControl["PluginApplication"].empty())
    {
        ifs.close();
        return;
    }

    std::string currentPluginName = m_JsonControl["Name"].get<std::string>();

    auto list = m_JsonControl["Dependencies"];
    for (auto& pluginName : list.items())
    {
        std::string plugin_name = pluginName.value()["Name"].get<std::string>();
        RecursiveStatisticsPluginNumber(GetPluginPathByPluginName[plugin_name], pluginPriority);
        if (pluginPriority.count(plugin_name) == 1)
            pluginPriority[plugin_name] += 1;
    }
    ifs.close();
}

PluginCore* PluginCore::getinstance()
{
    if (m_PluginCore == NULL)
    {
        m_PluginCore = new PluginCore();
        QString path = QCoreApplication::applicationDirPath();
        path = path + "/PluginTemporaryFile/";
        m_PluginCore->deleteTemporaryFolder(path);

        m_PluginCore->loadPlugin();
        //if (m_PluginCore->m_pluginPath.size())
        //{
        //    /*QString path = m_PluginCore->m_pluginPath.begin().value();
        //    path = path.mid(0, path.lastIndexOf("/") + 1) + "PluginTemporaryFile/";*/
        //    
        //}
       
    }
    return m_PluginCore;
}

std::vector<QString> PluginCore::GetPluginAllDescribe(QString pluginName)
{
    std::vector<QString> pluginAllDescribe;
    for (auto it = m_ProjectPluginList[pluginName].begin(); it != m_ProjectPluginList[pluginName].end(); it++)
    {
        pluginAllDescribe.push_back(it.key());
    }
    return pluginAllDescribe;
}
