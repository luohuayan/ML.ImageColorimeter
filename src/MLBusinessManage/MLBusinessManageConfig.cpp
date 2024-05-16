// Copyright 2024 MLOptic

#include "MLBusinessManageConfig.h"

#include <io.h>
#include <qstring.h>
#include <qstringlist.h>

#include <fstream>
#include <list>
#include <mutex>

#include "LoggingWrapper.h"
#include "config.h"

using ML::MLBusinessManage::MLBusinessManageConfig;

MLBusinessManageConfig* MLBusinessManageConfig::m_instance = nullptr;

MLBusinessManageConfig::MLBusinessManageConfig(){};

MLBusinessManageConfig* MLBusinessManageConfig::instance() {
    if (!m_instance) {
        static std::mutex mutex;
        std::unique_lock<std::mutex> locker(mutex);
        if (!m_instance) {
            m_instance = new MLBusinessManageConfig();
        }
    }
    return m_instance;
}

MLBusinessManageConfig::~MLBusinessManageConfig() {}

bool ML::MLBusinessManage::MLBusinessManageConfig::LoadConfigPath(
    const char* path) {
    std::string message;
    try {
        std::vector<std::string> nameList = {".json"/*, ".csv"*/};
        for (auto it : nameList) {
            std::vector<std::string> fileList;
            std::string pathStr = path;
            int length = pathStr.length();
            if (!MLBusinessManageConfig::instance()->GetNeedFile(path, it,
                                                                 &fileList)) {
                message = "Load config files Failed";
                LoggingWrapper::getinstance()->Debug(
                    std::to_string(fileList.size()));
                LoggingWrapper::getinstance()->Error(message);
                return false;
            }
            if (it.find(".json") != std::string::npos) {
                for (auto file : fileList) {
                    json tempJson;
                    std::ifstream ifs(file.c_str(), std::ifstream::in);
                    if (ifs.fail()) {
                        LoggingWrapper::getinstance()->Error("load " + file +
                                                             "failed");
                        return false;
                    }
                    ifs >> tempJson;
                    std::string fileName = file.substr(length + 1);
                    std::string str = "";
                    if (fileName == "NDFilterConfig.json") {
                        str = "NDFilterConfig";
                    } else if (fileName == "RXFilterConfig.json") {
                        str = "RXFilterConfig";
                    } else if (fileName == "CameraConfig.json") {
                        str = "CameraConfig";
                    } else if (fileName == "MotionConfig.json") {
                        str = "MotionConfig";
                    } else if (fileName == "ModuleConfig.json") {
                        str = "ModuleConfig";
                    }
                    if (str != "") {
                        LoggingWrapper::getinstance()->Debug("load " + file +
                                                             " successfully");
                        m_JsonMap.insert(std::make_pair(str, tempJson));
                    }
                }
                message = "Load .json files Finish";
                LoggingWrapper::getinstance()->Info(message);
            } else if (it.find(".csv") != std::string::npos) {
                std::map<std::string, QVector<QStringList>> qvecMap;
                for (auto file : fileList) {
                    QFile qfile(QString::fromStdString(file));
                    if (!qfile.open(QIODevice::ReadOnly | QIODevice::Text)) {
                        return false;
                    }
                    QVector<QStringList> data;
                    QTextStream in(&qfile);
                    while (!in.atEnd()) {
                        QString line = in.readLine();
                        if (line.isEmpty()) {
                            continue;
                        }
                        QStringList list = line.split(",");
                        data.append(list);
                    }
                    std::string str = "";
                    std::string fileName = file.substr(length + 1);
                    if (fileName == "RX_Selection.csv") {
                        str = "RX_Selection";
                    }
                    if (str != "") {
                        m_CsvMap.insert(std::make_pair(str, data));
                    }
                }
                message = "Load .csv files Finish";
                LoggingWrapper::getinstance()->Info(message);
            }
        }
        return true;
    } catch (const std::exception e) {
        message = "Load Config files Exception";
        LoggingWrapper::getinstance()->Warn(message);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManageConfig::ReadModuleInfo() {
    ML::MLBusinessManage::ModuleConfig moduleConfig;
    std::string message;
    try {
        for (auto it : m_JsonMap) {
            if (it.first == "NDFilterConfig") {
                auto configs = m_JsonMap[it.first]["FilterMap"];
                for (auto config : configs.items()) {
                    ML::MLFilterWheel::MLNDFilterConfiguation configuration;
                    configuration.enable = config.value()["Enable"].get<bool>();
                    configuration.type =
                        config.value()["Type"].get<std::string>();
                    configuration.name =
                        config.value()["Name"].get<std::string>();
                    configuration.protocol =
                        config.value()["Protocol"].get<std::string>();
                    configuration.motorType =
                        config.value()["MotorType"].get<std::string>();
                    configuration.port =
                        config.value()["Port"].get<std::string>();
                    configuration.station =
                        config.value()["Station"].get<int>();
                    configuration.serial_info.baudrate =
                        config.value()["SerialInfo"]["BaudRate"].get<int>();
                    configuration.serial_info.bytesize =
                        config.value()["SerialInfo"]["ByteSize"].get<int>();
                    configuration.serial_info.parity =
                        config.value()["SerialInfo"]["Parity"]
                            .get<std::string>();
                    configuration.serial_info.stopbits =
                        config.value()["SerialInfo"]["StopBit"]
                            .get<std::string>();
                    configuration.serial_info.flowcontrol =
                        config.value()["SerialInfo"]["FlowControl"]
                            .get<std::string>();
                    for (auto pos : config.value()["PositionList"]) {
                        configuration.positionName_List.insert(
                            std::make_pair(pos["Name"].get<std::string>(),
                                           pos["Channel"].get<int>()));
                        configuration.positionEnum_List.insert(std::make_pair(
                            transStringToEnum(pos["Enum"].get<std::string>()),
                            pos["Channel"].get<int>()));
                    }
                    configuration.MLIOCommand_enable =
                        config.value()["IOCommand"]["Enable"].get<bool>();
                    configuration.MLIOCommand.start =
                        config.value()["IOCommand"]["Start"].get<int>();
                    configuration.MLIOCommand.zHome =
                        config.value()["IOCommand"]["ZHome"].get<int>();
                    configuration.MLIOCommand.stop =
                        config.value()["IOCommand"]["Stop"].get<int>();
                    configuration.MLIOCommand.alarmRst =
                        config.value()["IOCommand"]["AlarmRst"].get<int>();
                    moduleConfig.NDFilterConfig_Map.insert(
                        std::make_pair(configuration.name, configuration));
                }
                message = "Read NDFilter Config Finish";
                LoggingWrapper::getinstance()->Info(message);
            } else if (it.first == "RXFilterConfig") {
                auto configs = m_JsonMap[it.first]["FilterMap"];
                for (auto config : configs.items()) {
                    ML::MLFilterWheel::MLRXFilterConfiguation configuration;
                    configuration.enable = config.value()["Enable"].get<bool>();
                    configuration.type =
                        config.value()["Type"].get<std::string>();
                    configuration.name =
                        config.value()["Name"].get<std::string>();
                    configuration.protocol =
                        config.value()["Protocol"].get<std::string>();
                    configuration.motorType =
                        config.value()["MotorType"].get<std::string>();
                    configuration.port =
                        config.value()["Port"].get<std::string>();
                    configuration.station =
                        config.value()["Station"].get<int>();
                    configuration.serial_info.baudrate =
                        config.value()["SerialInfo"]["BaudRate"].get<int>();
                    configuration.serial_info.bytesize =
                        config.value()["SerialInfo"]["ByteSize"].get<int>();
                    configuration.serial_info.parity =
                        config.value()["SerialInfo"]["Parity"]
                            .get<std::string>();
                    configuration.serial_info.stopbits =
                        config.value()["SerialInfo"]["StopBit"]
                            .get<std::string>();
                    configuration.serial_info.flowcontrol =
                        config.value()["SerialInfo"]["FlowControl"]
                            .get<std::string>();
                    for (auto pos : config.value()["PositionList"]) {
                        configuration.positionName_List.insert(
                            std::make_pair(pos["Name"].get<std::string>(),
                                           pos["Channel"].get<int>()));
                    }
                    configuration.MLIOCommand_enable =
                        config.value()["IOCommand"]["Enable"].get<bool>();
                    configuration.MLIOCommand.start =
                        config.value()["IOCommand"]["Start"].get<int>();
                    configuration.MLIOCommand.zHome =
                        config.value()["IOCommand"]["ZHome"].get<int>();
                    configuration.MLIOCommand.stop =
                        config.value()["IOCommand"]["Stop"].get<int>();
                    configuration.MLIOCommand.alarmRst =
                        config.value()["IOCommand"]["AlarmRst"].get<int>();
                    configuration.axis_info.enable =
                        config.value()["Axis"]["Enable"].get<bool>();
                    configuration.axis_info.name =
                        config.value()["Axis"]["Name"].get<std::string>();
                    configuration.axis_info.motorType =
                        config.value()["Axis"]["MotorType"].get<std::string>();
                    configuration.axis_info.station =
                        config.value()["Axis"]["Station"].get<int>();
                    configuration.axis_info.MLIOCommand_enable =
                        config.value()["Axis"]["IOCommand"]["Enable"]
                            .get<bool>();
                    configuration.axis_info.mlIOCommand.start =
                        config.value()["Axis"]["IOCommand"]["Start"].get<int>();
                    configuration.axis_info.mlIOCommand.zHome =
                        config.value()["Axis"]["IOCommand"]["ZHome"].get<int>();
                    configuration.axis_info.mlIOCommand.stop =
                        config.value()["Axis"]["IOCommand"]["Stop"].get<int>();
                    configuration.axis_info.mlIOCommand.alarmRst =
                        config.value()["Axis"]["IOCommand"]["AlarmRst"]
                            .get<int>();
                    configuration.axis_info.PulsePerCycle =
                        config.value()["Axis"]["PulsePerCycle"].get<int>();
                    configuration.axis_info.Min =
                        config.value()["Axis"]["Min"].get<int>();
                    configuration.axis_info.Max =
                        config.value()["Axis"]["Max"].get<int>();
                    moduleConfig.RXFilterConfig_Map.insert(
                        std::make_pair(configuration.name, configuration));
                }
                message = "Read RXFilter Config Finish";
                LoggingWrapper::getinstance()->Info(message);
            } else if (it.first == "CameraConfig") {
                ML::MLBusinessManage::CameraConfig cameraConfig;
                auto configs = m_JsonMap[it.first]["CameraMap"];
                for (auto config : configs.items()) {
                    cameraConfig.SerialNumber =
                        config.value()["SerialNumber"].get<std::string>();
                    cameraConfig.Enable = config.value()["Enable"].get<bool>();
                    cameraConfig.ColourCamera = config.value()["ColourCamera"].get<bool>();
                    cameraConfig.Type =
                        config.value()["Type"].get<std::string>();
                    cameraConfig.Name =
                        config.value()["Name"].get<std::string>();
                    cameraConfig.ConnectAddress =
                        config.value()["ConnectAddress"].get<std::string>();
                    moduleConfig.CameraConfig_Map.insert(
                        std::make_pair(cameraConfig.Name, cameraConfig));
                }
                message = "Read Camera Config Finish";
                LoggingWrapper::getinstance()->Info(message);
            } else if (it.first == "MotionConfig") {
                auto configs = m_JsonMap[it.first]["MotionMap"];
                for (auto config : configs.items()) {
                    ML::MLBusinessManage::MotionConfig motionConfig;
                    motionConfig.SerialNumber =
                        config.value()["SerialNumber"].get<std::string>();
                    motionConfig.Enable = config.value()["Enable"].get<bool>();
                    motionConfig.Type =
                        config.value()["Type"].get<std::string>();
                    motionConfig.Name =
                        config.value()["Name"].get<std::string>();
                    motionConfig.ConnectAddress =
                        config.value()["ConnectAddress"].get<std::string>();
                    motionConfig.ConnectType =
                        config.value()["ConnectType"].get<std::string>();
                    motionConfig.Port =
                        config.value()["Port"].get<std::string>();
                    motionConfig.DeviceID =
                        config.value()["DeviceID"].get<int>();
                    motionConfig.Axis = config.value()["Axis"].get<int>();
                    motionConfig.AxisName =
                        config.value()["AxisName"].get<std::string>();
                    motionConfig.HomingMethod =
                        config.value()["HomingMethod"].get<std::string>();
                    motionConfig.FocalLength =
                        config.value()["FocalLength"].get<double>();
                    motionConfig.FocalPlanesObjectSpace =
                        config.value()["FocalPlanesObjectSpace"].get<double>();
                    motionConfig.SoftwareLimitMax =
                        config.value()["SoftwareLimitMax"].get<double>();
                    motionConfig.SoftwareLimitMin =
                        config.value()["SoftwareLimitMin"].get<double>();
                    motionConfig.ReferencePosistion =
                        config.value()["ReferencePosition"].get<double>();
                    motionConfig.IsReverse =
                        config.value()["IsReverse"].get<bool>();
                    motionConfig.Speed = config.value()["Speed"].get<double>();
                    moduleConfig.MotionConfig_Map.insert(
                        std::make_pair(motionConfig.Name, motionConfig));
                }
                message = "Read Motion Config Finish";
                LoggingWrapper::getinstance()->Info(message);
            } else if (it.first == "ModuleConfig") {
                moduleConfig.Name =
                    m_JsonMap[it.first]["Name"].get<std::string>();
                moduleConfig.SerialNumber =
                    m_JsonMap[it.first]["SerialNumber"].get<std::string>();
                message = "Read Module Config Finish";
                LoggingWrapper::getinstance()->Info(message);
            }
        }
        m_ModuleConfig = moduleConfig;
        message = "Read All Config Finish";
        LoggingWrapper::getinstance()->Info(message);
        return true;
    } catch (const std::exception e) {
        message = "Read Config Exception";
        LoggingWrapper::getinstance()->Info(message);
        return false;
    }
}

ML::MLBusinessManage::ModuleConfig
ML::MLBusinessManage::MLBusinessManageConfig::GetModuleInfo() {
    return m_ModuleConfig;
}

bool ML::MLBusinessManage::MLBusinessManageConfig::LoadFolderRule(
    std::string path) {
    try {
        CConfigFile* config = new CConfigFile(path.c_str());
        std::vector<std::string> sessionList = {"FFC", "ColorShift",
                                                "Distortion", "FourColor"};
        for (auto session : sessionList) {
            ML::MLBusinessManage::FolderRule rule;
            std::string folderRule;
            std::string suffix;
            config->GetValue(session.c_str(), "Folder_Rule", folderRule);
            config->GetValue(session.c_str(), "Suffix", suffix);
            rule.Rule = folderRule;
            rule.Suffix = suffix;
            m_allFolderRule.insert(std::make_pair(session, rule));
        }
        delete config;
        std::string message = "Load Folder Rule Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return true;
    } catch (const std::exception e) {
        std::string message = "Load Folder Rule Exception";
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

std::string ML::MLBusinessManage::MLBusinessManageConfig::ProcessPath(
    std::string session, ML::MLBusinessManage::ProcessPathConfig config) {
    std::string message;
    std::string resultPath = "";
    try {
        if (m_allFolderRule.find(session) != m_allFolderRule.end()) {
            QStringList qlist =
                QString::fromStdString(m_allFolderRule[session].Rule)
                    .split('/');
            resultPath = session;
            for (auto it : qlist) {
                std::string tempStr;
                if (it.contains("Aperture")) {
                    resultPath = resultPath + "/" + config.Aperture;
                } else if (it.contains("ColorFilter")) {
                    resultPath = resultPath + "/" + config.ColorFilter;
                } else if (it.contains("NDFilter")) {
                    resultPath = resultPath + "/" + config.NDFilter;
                } else if (it.contains("ColorLight")) {
                    resultPath = resultPath + "/" + config.ColorLight;
                }
            }
            if (m_allFolderRule[session].Suffix != "") {
                resultPath = resultPath + "/" + m_allFolderRule[session].Suffix;
            }
            message = "Process Path Successfully";
            LoggingWrapper::getinstance()->Info(message);
            return resultPath;
        }
    } catch (const std::exception e) {
        message = "Process Path Exception";
        LoggingWrapper::getinstance()->Error(message);
        return std::string();
    }
}

cv::Mat ML::MLBusinessManage::MLBusinessManageConfig::ReadJsonFileToMat(
    const char* filename, std::string objectName) {
    try {
        json tempJson;
        std::ifstream ifs(filename, std::ifstream::in);
        if (ifs.fail()) {
            return cv::Mat();
        }
        ifs >> tempJson;
        int rows, cols;
        int index = -1;
        int len = tempJson[objectName].size();
        rows = tempJson[objectName].size();
        cols = tempJson[objectName][0].size();
        cv::Mat m = cv::Mat(rows, cols, CV_32F);
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                float a = tempJson[objectName][i][j].get<float>();
                m.at<float>(i, j) = tempJson[objectName][i][j].get<float>();
            }
        }
        return m;
    } catch (const std::exception e) {
        return cv::Mat();
    }
}

bool ML::MLBusinessManage::MLBusinessManageConfig::GetNeedFile(
    std::string path, std::string fileType,
    std::vector<std::string>* fileList) {
    intptr_t file_handle = 0;
    struct _finddata_t file_info;
    std::vector<std::string> file;
    std::string temp;
    if ((file_handle = _findfirst(
             temp.assign(path).append("/*" + fileType).c_str(), &file_info)) !=
        -1) {
        do {
            fileList->push_back(
                temp.assign(path).append("/").append(file_info.name));
        } while (_findnext(file_handle, &file_info) == 0);
        _findclose(file_handle);
        return true;
    }
    return false;
}

ML::MLFilterWheel::MLFilterEnum
ML::MLBusinessManage::MLBusinessManageConfig::transStringToEnum(
    std::string str) {
    std::string result;
    result.resize(str.size());
    transform(str.begin(), str.end(), result.begin(), ::tolower);
    if (result == "x") {
        return ML::MLFilterWheel::MLFilterEnum::X;
    } else if (result == "y") {
        return ML::MLFilterWheel::MLFilterEnum::Y;
    } else if (result == "z") {
        return ML::MLFilterWheel::MLFilterEnum::Z;
    } else if (result == "nd0") {
        return ML::MLFilterWheel::MLFilterEnum::ND0;
    } else if (result == "nd1") {
        return ML::MLFilterWheel::MLFilterEnum::ND1;
    } else if (result == "nd2") {
        return ML::MLFilterWheel::MLFilterEnum::ND2;
    } else if (result == "nd3") {
        return ML::MLFilterWheel::MLFilterEnum::ND3;
    } else if (result == "nd4") {
        return ML::MLFilterWheel::MLFilterEnum::ND4;
    } else if (result == "clear") {
        return ML::MLFilterWheel::MLFilterEnum::Clear;
    } else if (result == "block") {
        return ML::MLFilterWheel::MLFilterEnum::Block;
    } else {
        return ML::MLFilterWheel::MLFilterEnum::Unknown;
    }
}
