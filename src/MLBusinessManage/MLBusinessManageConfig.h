// Copyright 2024 MLOptic

#ifndef SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGECONFIG_H_
#define SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGECONFIG_H_

#include <QDir>
#include <QFile>
#include <QTextStream>
#include <map>
#include <vector>

#include "MLBusinessManageClass.h"
#include "json.hpp"
#include "mlbusinessmanage_global.h"

namespace ML {
namespace MLBusinessManage {
using json = nlohmann::json;

class MLBUSINESSMANAGE_EXPORT MLBusinessManageConfig {
 public:
    static MLBusinessManageConfig* instance();
    ~MLBusinessManageConfig();

    bool LoadConfigPath(const char* path);

    bool ReadModuleInfo();
    ML::MLBusinessManage::ModuleConfig GetModuleInfo();

    bool LoadFolderRule(std::string path);

    std::string ProcessPath(std::string session, ML::MLBusinessManage::ProcessPathConfig config);

    cv::Mat ReadJsonFileToMat(const char* filename, std::string objectName);

    //std::map<std::string, ML::MLBusinessManage::FolderRule> GetFolderRule();

 private:
    MLBusinessManageConfig();

    bool GetNeedFile(std::string path, std::string fileType,
                     std::vector<std::string>* fileList);

    ML::MLFilterWheel::MLFilterEnum transStringToEnum(
        std::string str);

 private:
    static MLBusinessManageConfig* m_instance;
    std::map<std::string, json> m_JsonMap;
    std::map<std::string, QVector<QStringList>> m_CsvMap;
    ML::MLBusinessManage::ModuleConfig m_ModuleConfig;
    std::map<std::string, ML::MLBusinessManage::FolderRule> m_allFolderRule;
};
}  // namespace MLBusinessManage
}  // namespace ML

#endif  // SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGECONFIG_H_
