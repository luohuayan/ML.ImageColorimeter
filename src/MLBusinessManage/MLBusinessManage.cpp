#include "MLBusinessManage.h"

#include <Windows.h>
#include <math.h>

#include <fstream>

#include "CameraPluginCommon.h"
#include "LoggingWrapper.h"
#include "MLBusinessManageConfig.h"
#include "MLColorShift.h"
#include "MLDistortionCorrection.h"
#include "MLExposureTimeCalibration.h"
#include "MLFFC.h"
#include "MLFilesystem.h"
#include "MLFilterWheelCallback.h"
#include "MLFourColor.h"
#include "ML_AddinManger.h"
#include "ML_addInInterface.h"
#include "MotorPluginCommon.h"

using ML::MLBusinessManage::MLBusinessManage;

static HMODULE getSelfModuleHandle() {
    MEMORY_BASIC_INFORMATION mbi;
    return ((::VirtualQuery(getSelfModuleHandle, &mbi, sizeof(mbi)) != 0)
                ? (HMODULE)mbi.AllocationBase
                : NULL);
}

static std::string getRunningPath(const UINT appPathSize = MAX_PATH) {
    char curDir[MAX_PATH] = {0};
    if (GetModuleFileNameA(getSelfModuleHandle(), curDir, appPathSize)) {
        std::string curdirStr(curDir);
        int idx = curdirStr.find_last_of("\\");
        std::string dllPath = curdirStr.substr(0, idx);
        return dllPath;
    }
    return "./";
}

MLBusinessManage::MLBusinessManage(QObject* parent) : QObject(parent) {
    m_ConfigPath = "";
    m_Aperture = "3mm";
    m_ColorLight = "";
    LoggingWrapper::getinstance()->Info("Create MLBusinessManage instance");
}

ML::MLBusinessManage::MLBusinessManage::~MLBusinessManage() {
    ML_DisconnectModule();
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetConfigPath(
    const char* path) {
    std::string message;
    char* ch;
    int length = strlen(path);
    ch = new char[length + 1];
    strcpy(ch, path);
    m_ConfigPath = ch;
    std::replace(m_ConfigPath.begin(), m_ConfigPath.end(), '/', '\\');
    if (!MLBusinessManageConfig::instance()->LoadConfigPath(
            m_ConfigPath.c_str())) {
        message = "Load MLBusinessManage Config Failed";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
    if (!MLBusinessManageConfig::instance()->ReadModuleInfo()) {
        message = "Read MLBusinessManage Config Failed";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }

    std::string pathIni = m_ConfigPath;
    pathIni += "\\CalConfig\\config.ini";
    if (!MLBusinessManageConfig::instance()->LoadFolderRule(pathIni)) {
        message = "Load Calibration Config Folder Rule Failed";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
    message = "Set MLBusinessManage Config Successfully";
    LoggingWrapper::getinstance()->Info(message);
    return Result(true, message);
}

Result ML::MLBusinessManage::MLBusinessManage::ML_CreateModule() {
    std::string message;
    try {
        std::string path = getRunningPath();
        pluginException ex = ML_AddinManger::getinstance().loadPluginPath(path);
        message = "Loading AddinManger plugin";
        LoggingWrapper::getinstance()->Info(message);

        ML::MLBusinessManage::ModuleConfig config =
            MLBusinessManageConfig::instance()->GetModuleInfo();

        for (auto it : config.NDFilterConfig_Map) {
            Result rt = CreateFilterWheel(it.second);
            if (!rt.success) {
                message = "Create " + it.second.name + " FilterWheel Failed";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }

        for (auto it : config.RXFilterConfig_Map) {
            Result rt = CreateRXFilterWheel(it.second);
            if (!rt.success) {
                message = "Create " + it.second.name + " RXFilterWheel Failed";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }

        for (auto it : config.MotionConfig_Map) {
            Result rt = CreateMotion(it.second);
            if (!rt.success) {
                message = "Create " + it.second.Name + " Motion Failed";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }

        for (auto it : config.CameraConfig_Map) {
            Result rt = CreateCamera(it.second);
            if (!rt.success) {
                message = "Create " + it.second.Name + " Camera Failed ";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }

        message = "Create MLBusinessManage Module Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Loading AddinManger plugin Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_ConnectModule() {
    try {
        if (ML_IsModuleConnect()) {
            std::string message = "MLBusinessManage Module Already Connected";
            LoggingWrapper::getinstance()->Debug(message);
            return Result(true, message);
        }
        bool isConnectFilterWheel = ConnectFilterWheel().success;
        emit filterWheelStatus(isConnectFilterWheel);
        bool isConnectRXFilterWheel = ConnectRXFilterWheel().success;
        emit RXFilterWheelStatus(isConnectRXFilterWheel);
        bool isConnectMotion = ConnectMotion().success;
        emit motionStatus(isConnectMotion);
        bool isConnectCamera = ConnectCamera().success;
        bool status = isConnectFilterWheel && isConnectRXFilterWheel &&
                      isConnectMotion && isConnectCamera;
        emit connectStatus(status);
        if (status) {
            std::string message =
                "Connect MLBusinessManage Module Successfully";
            LoggingWrapper::getinstance()->Info(message);
            return Result(true, message);
        } else {
            std::string message = "Connect MLBusinessManage Module Failed";
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
    } catch (const std::exception e) {
        std::string ex = e.what();
        std::string message =
            "Connect MLBusinessManage Module Exception: " + ex;
        emit connectStatus(false);
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_DisconnectModule() {
    std::string message = "";
    try {
        if (!ML_IsModuleConnect()) {
            pluginException ex =
                ML_AddinManger::getinstance().closeAllPluginInstances();
            if (!ex.getStatusFlag()) {
                message = "Close All Plugin Instances Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            message = "MLBusinessManage Module Is DisConnected";
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
        bool isConnectFilterWheel = DisConnectFilterWheel().success;
        bool isConnectRXFilterWheel = DisConnectRXFilterWheel().success;
        bool isConnectMotion = DisConnectMotion().success;
        bool isConnectCamera = DisConnectCamera().success;
        bool status = isConnectFilterWheel && isConnectRXFilterWheel &&
                      isConnectMotion && isConnectCamera;
        if (status) {
            pluginException ex =
                ML_AddinManger::getinstance().closeAllPluginInstances();
            if (!ex.getStatusFlag()) {
                message = "Close All Plugin Instances Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            for (auto addInBase : m_addInBase) {
                if (addInBase.second != nullptr) {
                    delete addInBase.second;
                    addInBase.second = nullptr;
                }
            }
            m_addInBase.clear();
            message = "DisConnect MLBusinessManage Module Successfully";
            LoggingWrapper::getinstance()->Info(message);
            return Result(true, message);
        } else {
            message = "DisConnect MLBusinessManage Module Failed";
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "DisConnect MLBusinessManage Module Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

bool ML::MLBusinessManage::MLBusinessManage::ML_IsModuleConnect() {
    try {
        bool isConnect = false;
        bool isFilterWheelConnect = IsFilterWheelConnect();
        bool isRXFilterWheelConnect = IsRXFilterWheelConnect();
        bool isMotionConnect = IsMotionConnect();
        bool isCameraConnect = IsCameraConnect();
        isConnect = isFilterWheelConnect && isRXFilterWheelConnect &&
                    isMotionConnect && isCameraConnect;
        return isConnect;
    } catch (const std::exception e) {
        std::string ex = e.what();
        LoggingWrapper::getinstance()->Error(
            "Ask MLBusinessManage Module Connect Status Exception: " + ex);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManage::ML_IsModuleMotorsMoving() {
    try {
        bool isMoving = false;
        if (!ML_IsModuleConnect()) {
            std::string message = "MLBusinessManage Module Is DisConnected";
            LoggingWrapper::getinstance()->Error(message);
            return false;
        }
        isMoving = IsFilterWheelMoving() && IsRXFilterWheelMoving() &&
                   IsMotionMoving();
        return isMoving;
    } catch (const std::exception e) {
        std::string ex = e.what();
        LoggingWrapper::getinstance()->Error(
            "Ask MLBusinessManage Module Moving Status Exception: " + ex);
        return false;
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_WaitForMovingStop(
    int timeout) {
    std::string message;
    try {
        if (!ML_IsModuleConnect()) {
            message = "MLBusinessManage Module Is DisConnected";
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
        int count = 0;
        while (ML_IsModuleMotorsMoving()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
            count++;
            if (count * 200 >= timeout) {
                message =
                    "MLBusinessManage Module Wait For Moving Stop Time Out";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(false, message);
            }
        }
        message = "MLBusinessManage Module Wait For Moving Stop Finish";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message =
            "MLBusinessManage Module Wait For Moving Stop Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

std::string ML::MLBusinessManage::MLBusinessManage::ML_GetModuleSerialNumber() {
    return MLBusinessManageConfig::instance()->GetModuleInfo().SerialNumber;
}

std::string ML::MLBusinessManage::MLBusinessManage::ML_GetModuleName() {
    return MLBusinessManageConfig::instance()->GetModuleInfo().Name;
}

Result ML::MLBusinessManage::MLBusinessManage::ML_MoveND_XYZFilterByNameAsync(
    std::string filterName, std::string channelName,
    ML::MLFilterWheel::MLFilterWheelCallback* cb) {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .NDFilterConfig_Map) {
            if (filterName == filter.first) {
                if (!filter.second.enable) {
                    m_Filter[filterName] = channelName;
                    message = filter.second.name + " " + filter.second.type +
                              " Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                pluginException ex = static_cast<FilterWheelBase*>(
                                         m_addInBase[filter.second.name])
                                         ->ML_MoveByName(channelName, cb);
                if (!ex.getStatusFlag()) {
                    message = "Move " + filter.second.name + " " +
                              filter.second.type + "to " + channelName +
                              " Failed, " + ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = "Move " + filter.second.name + " " +
                          filter.second.type + "to " + channelName +
                          " Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + filterName + " ND_XYZFilterWheel";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Move ND_XYZFilter Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_MoveND_XYZFilterByNameSync(
    std::string filterName, std::string channelName) {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .NDFilterConfig_Map) {
            if (filterName == filter.first) {
                if (!filter.second.enable) {
                    m_Filter[filterName] = channelName;
                    message = filter.second.name + " " + filter.second.type +
                              " Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                pluginException ex = static_cast<FilterWheelBase*>(
                                         m_addInBase[filter.second.name])
                                         ->ML_MoveByNameSync(channelName);
                if (!ex.getStatusFlag()) {
                    message = "Move " + filter.second.name + " " +
                              filter.second.type + "to " + channelName +
                              " Synchronously Failed, " + ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = "Move " + filter.second.name + " " +
                          filter.second.type + "to " + channelName +
                          " Synchronously Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + filterName + " ND_XYZFilterWheel";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Move ND_XYZFilter Synchronously Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

std::string ML::MLBusinessManage::MLBusinessManage::ML_GetND_XYZChannelName(
    std::string filterName) {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .NDFilterConfig_Map) {
            if (filterName == filter.first) {
                if (!filter.second.enable) {
                    message = filter.second.name + " " + filter.second.type +
                              " Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return m_Filter[filterName];
                }
                std::string channelName = static_cast<FilterWheelBase*>(
                                              m_addInBase[filter.second.name])
                                              ->ML_GetFilterName();
                message = "Get " + filter.second.name + " " +
                          filter.second.type + " Channel Name: " + channelName +
                          " Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return channelName;
            }
        }
        message = "Can not find the " + filterName + " ND_XYZFilterWheel";
        LoggingWrapper::getinstance()->Error(message);
        return std::string();
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Get ND_XYZFilter Channel Name Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return "Exception";
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_MoveRXFilterByNameAsync(
    std::string filterName, std::string channelName, int degree,
    ML::MLFilterWheel::MLFilterWheelCallback* cb) {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            if (filterName == filter.first) {
                if (!filter.second.enable) {
                    m_Filter[filterName] = channelName;
                    message = filter.second.name + " " + filter.second.type +
                              " Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                pluginException ex =
                    static_cast<RxFilterWheelBase*>(
                        m_addInBase[filter.second.name])
                        ->ML_MoveByName(channelName, degree, cb);
                if (!ex.getStatusFlag()) {
                    message = "Move " + filter.second.name + " " +
                              filter.second.type + "to " + channelName +
                              " Failed, " + ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = "Move " + filter.second.name + " " +
                          filter.second.type + "to " + channelName +
                          " Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + filterName + " RXFilterWheel";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Move RXFilterWheel Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_MoveRXFilterByNameSync(
    std::string filterName, std::string channelName, int degree) {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            if (filterName == filter.first) {
                if (!filter.second.enable) {
                    m_Filter[filterName] = channelName;
                    message = filter.second.name + " " + filter.second.type +
                              " Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                pluginException ex =
                    static_cast<RxFilterWheelBase*>(
                        m_addInBase[filter.second.name])
                        ->ML_MoveByNameSync(channelName, degree);
                if (!ex.getStatusFlag()) {
                    message = "Move " + filter.second.name + " " +
                              filter.second.type + " to " + channelName +
                              " Synchronously Failed, " + ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = "Move " + filter.second.name + " " +
                          filter.second.type + " to " + channelName +
                          " Synchronously Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + filterName + " RXFilterWheel";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Move RXFilterWheel Synchronously Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

std::string ML::MLBusinessManage::MLBusinessManage::ML_GetRXFilterName(
    std::string filterName) {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            if (filterName == filter.first) {
                if (!filter.second.enable) {
                    message = filter.second.name + " " + filter.second.type +
                              " Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return m_RXFilter[filterName];
                }
                std::string channelName = static_cast<RxFilterWheelBase*>(
                                              m_addInBase[filter.second.name])
                                              ->ML_GetFilterName();
                message = "Get " + filter.second.name + " " +
                          filter.second.type + " Channel Name: " + channelName +
                          " Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return channelName;
            }
        }
        message = "Can not find the " + filterName + " RXFilterWheel";
        LoggingWrapper::getinstance()->Error(message);
        return std::string();
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Get RXFilterWheel Channel Name Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return "Exception";
    }
}

int ML::MLBusinessManage::MLBusinessManage::ML_GetRXFilterAxis(
    std::string filterName) {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            if (filterName == filter.first) {
                if (!filter.second.enable) {
                    message = filter.second.name + " " + filter.second.type +
                              " Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return INT_MAX;
                }
                int degree = static_cast<RxFilterWheelBase*>(
                                 m_addInBase[filter.second.name])
                                 ->ML_GetAxisDegree();
                message =
                    "Get " + filter.second.name + " " + filter.second.type +
                    " Axis Degree: " + std::to_string(degree) + " Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return degree;
            }
        }
        message = "Can not find the " + filterName + " RXFilterWheel";
        LoggingWrapper::getinstance()->Error(message);
        return INT_MAX;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Get RXFilterWheel Axis Degree Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return INT_MAX;
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetFocusAsync(
    double vid, ML::MLBusinessManage::FoucsMethod method) {
    std::string message;
    try {
        if (method == ML::MLBusinessManage::FoucsMethod::OneMotor) {
            for (auto motion : MLBusinessManageConfig::instance()
                                   ->GetModuleInfo()
                                   .MotionConfig_Map) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                double pos;
                pos = motion.second.FocalLength /
                          (vid + motion.second.FocalPlanesObjectSpace) +
                      motion.second.ReferencePosistion;
                if (motion.second.IsReverse) {
                    pos = -motion.second.FocalLength /
                              (vid + motion.second.FocalPlanesObjectSpace) +
                          motion.second.ReferencePosistion;
                }

                std::map<std::string, double> posMap;
                posMap[motion.second.AxisName] = static_cast<int>(pos * 1000);
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->MoveAbsAsync(posMap);
                if (!ex.getStatusFlag()) {
                    message = "Asynchronously Set Focus to " +
                              std::to_string(vid) + " Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                } else {
                    break;
                }
            }
            message = "Asynchronously Set Focus to " + std::to_string(vid) +
                      " Successfully";
            LoggingWrapper::getinstance()->Info(message);
            return Result(true, message);
        }
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Asynchronously Set Focus Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetFocusSync(
    double vid, ML::MLBusinessManage::FoucsMethod method) {
    std::string message;
    try {
        if (method == ML::MLBusinessManage::FoucsMethod::OneMotor) {
            for (auto motion : MLBusinessManageConfig::instance()
                                   ->GetModuleInfo()
                                   .MotionConfig_Map) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                double pos;
                pos = motion.second.FocalLength /
                          (vid + motion.second.FocalPlanesObjectSpace) +
                      motion.second.ReferencePosistion;
                if (motion.second.IsReverse) {
                    pos = -motion.second.FocalLength /
                              (vid + motion.second.FocalPlanesObjectSpace) +
                          motion.second.ReferencePosistion;
                }
                // switch vid to motion pos

                std::map<std::string, double> posMap;
                posMap[motion.second.AxisName] = pos * 1000.0;
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->MoveAbs(posMap);
                if (!ex.getStatusFlag()) {
                    message = "Synchronously Set Focus to " +
                              std::to_string(vid) + " Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                } else {
                    break;
                }
            }
            message = "Synchronously Set Focus to " + std::to_string(vid) +
                      " Successfully";
            LoggingWrapper::getinstance()->Info(message);
            return Result(true, message);
        }
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Synchronously Set Focus Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

double ML::MLBusinessManage::MLBusinessManage::ML_GetVID(
    ML::MLBusinessManage::FoucsMethod method) {
    std::string message;
    try {
        if (method == ML::MLBusinessManage::FoucsMethod::OneMotor) {
            for (auto motion : MLBusinessManageConfig::instance()
                                   ->GetModuleInfo()
                                   .MotionConfig_Map) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return DBL_MAX;
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return DBL_MAX;
                }
                // switch motion pos to vid
                std::map<std::string, double> posMap;
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->GetPosition(&posMap);
                if (!ex.getStatusFlag()) {
                    message = "MoveAbsAsync " + motion.second.Name + " " +
                              motion.second.Type + " Motion Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return DBL_MAX;
                }
                double vid;
                vid = motion.second.FocalLength /
                          (posMap[motion.second.AxisName] / 1000.0 -
                           motion.second.ReferencePosistion) -
                      motion.second.FocalPlanesObjectSpace;
                if (motion.second.IsReverse) {
                    vid = -motion.second.FocalLength /
                              (posMap[motion.second.AxisName] / 1000.0 -
                               motion.second.ReferencePosistion) -
                          motion.second.FocalPlanesObjectSpace;
                }
                message = "Obtain " + motion.second.Name + " " +
                          motion.second.Type + " Motion Position Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return vid;
            }
        }
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Obtain Motion Position Failed: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return DBL_MAX;
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetPosistionAbsAsync(
    std::string motionName, double pos) {
    std::string message;
    try {
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (motion.second.Name == motionName) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                if (pos > motion.second.SoftwareLimitMax ||
                    pos < motion.second.SoftwareLimitMin) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Position Out of Software Limit";
                    LoggingWrapper::getinstance()->Debug(message);
                    return Result(false, message);
                }
                std::map<std::string, double> posMap;
                posMap[motion.second.AxisName] = pos * 1000.0;
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->MoveAbsAsync(posMap);
                if (!ex.getStatusFlag()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion MoveAbsAsync Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion MoveAbsAsync Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + motionName + " Motion";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = motionName + " Motion MoveAbsAsync Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetPosistionAbsSync(
    std::string motionName, double pos) {
    std::string message;
    try {
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (motion.second.Name == motionName) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                if (pos > motion.second.SoftwareLimitMax ||
                    pos < motion.second.SoftwareLimitMin) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Position Out of Software Limit";
                    LoggingWrapper::getinstance()->Debug(message);
                    return Result(false, message);
                }
                std::map<std::string, double> posMap;
                posMap[motion.second.AxisName] = pos * 1000.0;
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->MoveAbs(posMap);
                if (!ex.getStatusFlag()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion MoveAbsSync Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion MoveAbsSync Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + motionName + " Motion";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = motionName + "Motion MoveAbsSync Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetPositionRelAsync(
    std::string motionName, double pos) {
    std::string message;
    try {
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (motion.second.Name == motionName) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                double relPos = pos + ML_GetMotionPosition(motion.second.Name);
                if (relPos > motion.second.SoftwareLimitMax ||
                    relPos < motion.second.SoftwareLimitMin) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Position Out of Software Limit";
                    LoggingWrapper::getinstance()->Debug(message);
                    return Result(false, message);
                }
                std::map<std::string, double> posMap;
                posMap[motion.second.AxisName] = pos * 1000.0;
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->MoveRelAsync(posMap);
                if (!ex.getStatusFlag()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion MoveRelAsync Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion MoveRelAsync Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + motionName + " Motion";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = motionName + "Motion MoveRelAsync Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetPositionRelSync(
    std::string motionName, double pos) {
    std::string message;
    try {
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (motion.second.Name == motionName) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                double relPos = pos + ML_GetMotionPosition(motion.second.Name);
                if (relPos > motion.second.SoftwareLimitMax ||
                    relPos < motion.second.SoftwareLimitMin) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Position Out of Software Limit";
                    LoggingWrapper::getinstance()->Debug(message);
                    return Result(false, message);
                }
                std::map<std::string, double> posMap;
                posMap[motion.second.AxisName] = pos * 1000.0;
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->MoveRel(posMap);
                if (!ex.getStatusFlag()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion MoveRelSync Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion MoveRelSync Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + motionName + " Motion";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = motionName + " Motion MoveRelSync Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

double ML::MLBusinessManage::MLBusinessManage::ML_GetMotionPosition(
    std::string motionName) {
    std::string message;
    try {
        double pos;
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (motion.second.Name == motionName) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return DBL_MAX;
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return DBL_MAX;
                }
                if (motion.second.Name == motionName) {
                    std::map<std::string, double> posMap;
                    pluginException ex = static_cast<ActuatorBase*>(
                                             m_addInBase[motion.second.Name])
                                             ->GetPosition(&posMap);
                    if (!ex.getStatusFlag()) {
                        message = "Obtain " + motion.second.Name + " " +
                                  motion.second.Type +
                                  " Motion Position Failed, " +
                                  ex.getExceptionMsg();
                        LoggingWrapper::getinstance()->Error(message);
                        return DBL_MAX;
                    }
                    pos = posMap[motion.second.AxisName] / 1000.0;
                    message = "Obtain " + motion.second.Name + " " +
                              motion.second.Type +
                              " Motion Position Successfully, position = " +
                              std::to_string(pos) + " millimeter";
                    LoggingWrapper::getinstance()->Info(message);
                    return pos;
                }
            }
        }
        message = "Can not find the " + motionName + " Motion";
        LoggingWrapper::getinstance()->Error(message);
        return DBL_MAX;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Obtain Motion Position Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return DBL_MAX;
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_StopMotionMovement(
    std::string motionName) {
    std::string message;
    try {
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (motion.second.Name == motionName) {
                if (!motion.second.Enable) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                         ->IsConnected()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                        ->StopMove();
                if (!ex.getStatusFlag()) {
                    message = motion.second.Name + " " + motion.second.Type +
                              " Motion StopMove Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion StopMove Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + motionName + " Motion";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "StopMove All Motions Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetAperture(
    std::string aperture) {
    m_Aperture = aperture;
    return Result(true, "");
}

std::string ML::MLBusinessManage::MLBusinessManage::ML_GetAperture() {
    return m_Aperture;
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetColorLight(
    std::string colorLight) {
    transform(colorLight.begin(), colorLight.end(), colorLight.begin(),
              ::toupper);
    m_ColorLight = colorLight;
    return Result(true, "");
}

std::string ML::MLBusinessManage::MLBusinessManage::ML_GetColorLight() {
    return m_ColorLight;
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetExposureTime(
    std::string cameraName, double expTime) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->SetExposureTime(expTime * 1000.0);
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Set Exposure Time Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Set Exposure Time to " +
                          std::to_string(expTime) + " millisecond Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Set Exposure Time to " +
                  std::to_string(expTime) + " Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

double ML::MLBusinessManage::MLBusinessManage::ML_GetExposureTime(
    std::string cameraName) {
    std::string message;
    try {
        double expTime;
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return DBL_MAX;
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return DBL_MAX;
                }
                expTime =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->GetExposureTime();
                expTime = expTime / 1000.0;
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Get Exposure Time to " +
                          std::to_string(expTime) + " millisecond Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return expTime;
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return DBL_MAX;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Get Exposure Time Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return DBL_MAX;
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetBinning(
    std::string cameraName, ML::CameraV2::Binning binning) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->SetBinning(binning);
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Set Binnin Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Set Binnin Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Set Binning Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

ML::CameraV2::Binning ML::MLBusinessManage::MLBusinessManage::ML_GetBinning(
    std::string cameraName) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return ML::CameraV2::Binning();
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return ML::CameraV2::Binning();
                }
                ML::CameraV2::Binning binning =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->GetBinning();
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Get Binnin Successfully, binning = " +
                          std::to_string(binning);
                LoggingWrapper::getinstance()->Info(message);
                return binning;
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return ML::CameraV2::Binning();
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Get Binning Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return ML::CameraV2::Binning();
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetBinningMode(
    std::string cameraName, ML::CameraV2::BinningMode binningMode) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->SetBinningMode(binningMode);
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Set BinninMode Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Set BinninMode Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Set BinningMode Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

ML::CameraV2::BinningMode
ML::MLBusinessManage::MLBusinessManage::ML_GetBinningMode(
    std::string cameraName) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return ML::CameraV2::BinningMode();
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return ML::CameraV2::BinningMode();
                }
                ML::CameraV2::BinningMode binningMode =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->GetBinningMode();
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Get Binnin Successfully, binning = " +
                          std::to_string(binningMode);
                LoggingWrapper::getinstance()->Info(message);
                return binningMode;
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return ML::CameraV2::BinningMode();
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Get BinningMode Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return ML::CameraV2::BinningMode();
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetBitDepth(
    std::string cameraName, ML::CameraV2::MLPixelFormat bitDepth) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->SetFormatType(bitDepth);
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Set BitDepth Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Set BitDepth Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Set BitDepth Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

ML::CameraV2::MLPixelFormat
ML::MLBusinessManage::MLBusinessManage::ML_GetBitDepth(std::string cameraName) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return ML::CameraV2::MLPixelFormat();
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return ML::CameraV2::MLPixelFormat();
                }
                ML::CameraV2::MLPixelFormat bitDepth =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->GetFormatType();
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Get BitDepth Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return bitDepth;
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return ML::CameraV2::MLPixelFormat();
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Get BinningMode Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return ML::CameraV2::MLPixelFormat();
    }
}

double ML::MLBusinessManage::MLBusinessManage::ML_GetGrayLevel(
    std::string cameraName, cv::Rect rect) {
    return 0.0;
}

double ML::MLBusinessManage::MLBusinessManage::ML_GetGrayLevel(
    std::string cameraName, int pixelConut) {
    return 0.0;
}

Result ML::MLBusinessManage::MLBusinessManage::ML_AutoExposureAsync(
    std::string cameraName, int initialTime) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = cameraName + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[cameraName])
                         ->IsOpened()) {
                    message = cameraName + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[cameraName])
                        ->InitializeAEConfig(m_ConfigPath.c_str(), cameraName,
                                             initialTime, true);
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Initialize AE config Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                ML::CameraV2::AutoExposureStatus status =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->SetMLExposureAuto();
                if (status ==
                        ML::CameraV2::AutoExposureStatus ::AE_too_bright_ ||
                    status == ML::CameraV2::AutoExposureStatus::AE_time_out) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure Warning, status code: " +
                              to_string(status);
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                } else if (status ==
                           ML::CameraV2::AutoExposureStatus::AE_normal_) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure Successfully";
                    LoggingWrapper::getinstance()->Info(message);
                    return Result(true, message);
                } else if (status ==
                           ML::CameraV2::AutoExposureStatus::AE_too_dark_) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure too dark";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                } else if (status == ML::CameraV2::AutoExposureStatus::
                                         AE_max_iterations_reached_) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure max iterations reached";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                } else {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure error, status code: " +
                              to_string(status);
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Auto exposure Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_AutoExposureSync(
    std::string cameraName, int initialTime) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = cameraName + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[cameraName])
                         ->IsOpened()) {
                    message = cameraName + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[cameraName])
                        ->InitializeAEConfig(m_ConfigPath.c_str(), cameraName,
                                             initialTime, true);
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Initialize AE config Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                ML::CameraV2::AutoExposureStatus status =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->SetMLExposureAuto();
                if (status ==
                        ML::CameraV2::AutoExposureStatus ::AE_too_bright_ ||
                    status == ML::CameraV2::AutoExposureStatus::AE_time_out) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure Warning, status code: " +
                              to_string(status);
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                } else if (status ==
                           ML::CameraV2::AutoExposureStatus::AE_normal_) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure Successfully";
                    LoggingWrapper::getinstance()->Info(message);
                    return Result(true, message);
                } else if (status ==
                           ML::CameraV2::AutoExposureStatus::AE_too_dark_) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure too dark";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                } else if (status == ML::CameraV2::AutoExposureStatus::
                                         AE_max_iterations_reached_) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure max iterations reached";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                } else {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Auto exposure error, status code: " +
                              to_string(status);
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Auto exposure Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_CaptureImageAsync(
    std::string cameraName) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->GrabOne();
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Capture Image Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Capture Image Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Capture Image Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_CaptureImageSync(
    std::string cameraName) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(true, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                pluginException ex =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->GrabOne();
                if (!ex.getStatusFlag()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Capture Image synchronously Failed, " +
                              ex.getExceptionMsg();
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Capture Image synchronously Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message =
            cameraName + " Camera Capture Image synchronously Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

cv::Mat ML::MLBusinessManage::MLBusinessManage::ML_GetImage(
    std::string cameraName) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return cv::Mat();
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return cv::Mat();
                }
                cv::Mat img =
                    static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                        ->GetImage();
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Get Image Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return img;
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return cv::Mat();
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Get Image Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return cv::Mat();
    }
}

ML::MLBusinessManage::CaptureData
ML::MLBusinessManage::MLBusinessManage::ML_GetCaptureData() {
    std::string message;
    try {
        ML::MLBusinessManage::CaptureData captureData;
        ML::MLBusinessManage::ModuleConfig moduleConfig =
            MLBusinessManageConfig::instance()->GetModuleInfo();
        captureData.SerialNumber = moduleConfig.SerialNumber;
        captureData.ModuleName = moduleConfig.Name;
        captureData.Aperture = ML_GetAperture();
        captureData.ColorLight = ML_GetColorLight();
        captureData.VID = ML_GetVID();
        for (auto filter : moduleConfig.NDFilterConfig_Map) {
            Sleep(150);
            std::string channel = ML_GetND_XYZChannelName(filter.second.name);
            std::string lowerName;
            lowerName.resize(filter.second.name.size());
            transform(filter.second.name.begin(), filter.second.name.end(),
                      lowerName.begin(), ::tolower);
            if (lowerName.find("nd") != std::string::npos) {
                captureData.NDFilter = channel;
            } else if (lowerName.find("xyz") != std::string::npos) {
                captureData.ColorFilter = channel;
            } else if (lowerName.find("color") != std::string::npos) {
                captureData.ColorFilter = channel;
            }
        }
        for (auto rxfilter : moduleConfig.RXFilterConfig_Map) {
            std::string channel = ML_GetRXFilterName(rxfilter.second.name);
            int degree = ML_GetRXFilterAxis(rxfilter.second.name);
            captureData.RXFilter = channel;
            captureData.RXAxisDegree = degree;
            break;
        }

        for (auto camera : moduleConfig.CameraConfig_Map) {
            ML::CameraV2::Binning binning = ML_GetBinning(camera.second.Name);
            ML::CameraV2::MLPixelFormat bitdepth =
                ML_GetBitDepth(camera.second.Name);
            double expTime = ML_GetExposureTime(camera.second.Name);
            cv::Mat img = ML_GetImage(camera.second.Name);
            captureData.Img = img;
            captureData.ExposureTime = expTime;
            captureData.Binning = binning;
            captureData.BitDepth = bitdepth;
            break;
        }
        message = "Get CaptureData Finish";
        LoggingWrapper::getinstance()->Info(message);
        return captureData;

    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Get CaptureData Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return ML::MLBusinessManage::CaptureData();
    }
}

std::map<std::string, ML::MLBusinessManage::CaptureData>
ML::MLBusinessManage::MLBusinessManage::ML_GetColorCameraCaptureData() {
    std::string message;
    try {
        std::map<std::string, ML::MLBusinessManage::CaptureData> captureMap;
        ML::CameraV2::Binning binning = ML::CameraV2::Binning::ONE_BY_ONE;
        ML::CameraV2::MLPixelFormat bitdepth =
            ML::CameraV2::MLPixelFormat::MLMono12;
        double expTime = 0.0;
        cv::Mat img;
        ML::MLBusinessManage::ModuleConfig moduleConfig =
            MLBusinessManageConfig::instance()->GetModuleInfo();
        for (auto camera : moduleConfig.CameraConfig_Map) {
            binning = ML_GetBinning(camera.second.Name);
            bitdepth = ML_GetBitDepth(camera.second.Name);
            expTime = ML_GetExposureTime(camera.second.Name);
            img = ML_GetImage(camera.second.Name);
            break;
        }
        std::string aperture = ML_GetAperture();
        std::string colorLight = ML_GetColorLight();
        double vid = ML_GetVID();
        std::string ndfilter;
        for (auto filter : moduleConfig.NDFilterConfig_Map) {
            std::string channel = ML_GetND_XYZChannelName(filter.second.name);
            std::string lowerName;
            lowerName.resize(filter.second.name.size());
            transform(filter.second.name.begin(), filter.second.name.end(),
                      lowerName.begin(), ::tolower);
            if (lowerName.find("nd") != std::string::npos) {
                ndfilter = channel;
                break;
            }
        }
        std::string rxchannel;
        int degree = 0;
        for (auto rxfilter : moduleConfig.RXFilterConfig_Map) {
            rxchannel = ML_GetRXFilterName(rxfilter.second.name);
            degree = ML_GetRXFilterAxis(rxfilter.second.name);
            break;
        }
        if (img.channels() == 3) {
            vector<cv::Mat> vecImg;
            cv::split(img, vecImg);
            std::map<int, std::string> channelMap = {
                {0, "X"}, {1, "Y"}, {2, "Z"}};
            for (int i = 0; i < vecImg.size(); i++) {
                ML::MLBusinessManage::CaptureData captureData;
                captureData.SerialNumber = moduleConfig.SerialNumber;
                captureData.ModuleName = moduleConfig.Name;
                captureData.Aperture = aperture;
                captureData.ColorLight = colorLight;
                captureData.NDFilter = ndfilter;
                captureData.ColorFilter = channelMap[i];
                captureData.RXFilter = rxchannel;
                captureData.RXAxisDegree = degree;
                captureData.VID = vid;
                captureData.ExposureTime = expTime;
                captureData.Binning = binning;
                captureData.BitDepth = bitdepth;
                captureData.Img = vecImg[i];
                captureMap.insert(
                    std::make_pair(captureData.ColorFilter, captureData));
            }
            message = "Get Color Camera CaptureData Map Finish";
            LoggingWrapper::getinstance()->Info(message);
            return captureMap;
        }
        message =
            "Get Color Camera CaptureData Map Failed, Not a three channel image from camera";
        LoggingWrapper::getinstance()->Error(message);
        return std::map<std::string, ML::MLBusinessManage::CaptureData>();
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Get Color Camera CaptureData Map Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return std::map<std::string, ML::MLBusinessManage::CaptureData>();
    }
}

ML_addInBase* ML::MLBusinessManage::MLBusinessManage::ML_GetOneModuleByName(
    std::string name) {
    std::string message;
    try {
        if (m_addInBase.find(name) != m_addInBase.end()) {
            message = "Get " + name + " AddInBase Pointer Successfully";
            LoggingWrapper::getinstance()->Info(message);
            return m_addInBase[name];
        }
        message = "Not Found " + name + " AddInBase Pointer";
        LoggingWrapper::getinstance()->Debug(message);
        return nullptr;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Get " + name + " AddInBase Pointer Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return nullptr;
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SetCaptureDataMap(
    std::map<std::string, ML::MLBusinessManage::CaptureData> data) {
    std::string message;
    try {
        m_CaptureDataMap.clear();
        m_CaptureDataMap = std::move(data);
        std::map<std::string, cv::Mat> capMat;
        for (auto it : m_CaptureDataMap) {
            capMat.insert(std::make_pair(it.first, it.second.Img.clone()));
        }
        if (m_CalibrationDataMap.count(
                ML::MLBusinessManage::CalibrationEnum::Raw) != 0) {
            m_CalibrationDataMap[ML::MLBusinessManage::CalibrationEnum::Raw] =
                capMat;
        } else {
            m_CalibrationDataMap.insert(std::make_pair(
                ML::MLBusinessManage::CalibrationEnum::Raw, capMat));
        }
        if (m_CalibrationDataMap.count(
                ML::MLBusinessManage::CalibrationEnum::Result) != 0) {
            m_CalibrationDataMap
                [ML::MLBusinessManage::CalibrationEnum::Result] = capMat;
        } else {
            m_CalibrationDataMap.insert(std::make_pair(
                ML::MLBusinessManage::CalibrationEnum::Result, capMat));
        }

        return Result(true, "");
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Set CaptureData Map Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(true, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_LoadFFCData() {
    std::string message;
    try {
        ML::MLBusinessManage::PreCalibrationData preData;
        for (auto it : m_CaptureDataMap) {
            ML::MLBusinessManage::ProcessPathConfig config;
            config.Aperture = it.second.Aperture;
            config.ColorFilter = it.second.ColorFilter;
            config.ColorLight = it.second.ColorLight;
            config.NDFilter = it.second.NDFilter;
            config.InputPath = m_ConfigPath + "\\CalConfig\\";
            cv::Mat dark = cv::imread(config.InputPath + "dark.tif", -1);
            LoggingWrapper::getinstance()->Debug(
                "dark image: " + config.InputPath + "dark.tif");
            std::string outPath =
                MLBusinessManageConfig::instance()->ProcessPath("FFC", config);
            cv::Mat ffcImg = cv::imread(config.InputPath + outPath, -1);
            LoggingWrapper::getinstance()->Debug(
                "light image: " + config.InputPath + outPath);
            preData.LightFFCMap.insert(
                std::make_pair(config.ColorFilter, ffcImg));
            preData.DarkFFCMap.insert(std::make_pair(config.ColorFilter, dark));
            message = "Load Flat Field Data For " + config.ColorFilter;
            LoggingWrapper::getinstance()->Info(message);
        }
        m_preCalibrationData.LightFFCMap = preData.LightFFCMap;
        m_preCalibrationData.DarkFFCMap = preData.DarkFFCMap;
        message = "Load Flat Field Data Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Load Flat Field Data Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_LoadColorShiftData() {
    std::string message;
    try {
        ML::MLBusinessManage::PreCalibrationData preData;
        for (auto it : m_CaptureDataMap) {
            ML::MLBusinessManage::ProcessPathConfig config;
            config.Aperture = it.second.Aperture;
            config.ColorFilter = it.second.ColorFilter;
            config.ColorLight = it.second.ColorLight;
            config.NDFilter = it.second.NDFilter;
            config.InputPath = m_ConfigPath;
            config.InputPath = config.InputPath + "\\CalConfig\\";
            std::string outPath =
                MLBusinessManageConfig::instance()->ProcessPath("ColorShift",
                                                                config);
            cv::Mat mat = MLBusinessManageConfig::instance()->ReadJsonFileToMat(
                (config.InputPath + outPath).c_str(), "ColorShift");
            cv::Point2d point{mat.at<float>(0, 0), mat.at<float>(0, 1)};
            LoggingWrapper::getinstance()->Info(
                "ColorShift point x: " + std::to_string(point.x) +
                ", point y: " + std::to_string(point.y));
            preData.PointMap.insert(std::make_pair(config.ColorFilter, point));
            message = "Load Color Shift Data For " + config.ColorFilter;
            LoggingWrapper::getinstance()->Info(message);
        }
        m_preCalibrationData.PointMap = preData.PointMap;
        message = "Load Color Shift Data Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Load Color Shift Data Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_LoadDistortionData() {
    std::string message;
    try {
        ML::MLBusinessManage::PreCalibrationData preData;
        for (auto it : m_CaptureDataMap) {
            ML::MLBusinessManage::ProcessPathConfig config;
            config.Aperture = it.second.Aperture;
            config.ColorFilter = it.second.ColorFilter;
            config.ColorLight = it.second.ColorLight;
            config.NDFilter = it.second.NDFilter;
            config.InputPath = m_ConfigPath;
            config.InputPath = config.InputPath + "\\CalConfig\\";
            std::string outPath =
                MLBusinessManageConfig::instance()->ProcessPath("Distortion",
                                                                config);
            cv::Mat cameraMatrix =
                MLBusinessManageConfig::instance()->ReadJsonFileToMat(
                    (config.InputPath + outPath).c_str(), "CameraMatrix");
            std::string mstr = "camera matrix: ";
            for (int i = 0; i < cameraMatrix.cols; i++) {
                for (int j = 0; j < cameraMatrix.rows; j++) {
                    mstr = mstr + std::to_string(cameraMatrix.at<float>(j, i)) +
                           ",";
                }
            }
            LoggingWrapper::getinstance()->Debug("camera matrix: " + mstr);
            cv::Mat coefficient =
                MLBusinessManageConfig::instance()->ReadJsonFileToMat(
                    (config.InputPath + outPath).c_str(), "Coefficient");
            std::string cstr = "coefficient: ";
            for (int i = 0; i < coefficient.cols; i++) {
                for (int j = 0; j < coefficient.rows; j++) {
                    cstr = cstr + std::to_string(coefficient.at<float>(j, i)) +
                           ",";
                }
            }
            LoggingWrapper::getinstance()->Debug("coefficient: " + cstr);
            message = "Load Distortion Data";
            preData.CameraMatrix = cameraMatrix;
            preData.Coefficient = coefficient;
            LoggingWrapper::getinstance()->Info(message);
            break;
        }
        m_preCalibrationData.CameraMatrix = preData.CameraMatrix;
        m_preCalibrationData.Coefficient = preData.Coefficient;
        message = "Load Distortion Data Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Load Distortion Data Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_LoadExpsoureData() {
    return Result();
}

Result ML::MLBusinessManage::MLBusinessManage::ML_LoadFourColorData() {
    std::string message;
    try {
        ML::MLBusinessManage::PreCalibrationData preData;
        for (auto it : m_CaptureDataMap) {
            ML::MLBusinessManage::ProcessPathConfig config;
            config.Aperture = it.second.Aperture;
            config.ColorFilter = it.second.ColorFilter;
            config.ColorLight = it.second.ColorLight;
            config.NDFilter = it.second.NDFilter;
            config.InputPath = m_ConfigPath;
            config.InputPath = config.InputPath + "\\CalConfig\\";
            std::string outPath =
                MLBusinessManageConfig::instance()->ProcessPath("FourColor",
                                                                config);
            cv::Mat RMatrix =
                MLBusinessManageConfig::instance()->ReadJsonFileToMat(
                    (config.InputPath + outPath).c_str(), "RMatrix");
            std::string mstr = "R matrix: ";
            for (int i = 0; i < RMatrix.cols; i++) {
                for (int j = 0; j < RMatrix.rows; j++) {
                    mstr = mstr + std::to_string(RMatrix.at<float>(i, j)) + ",";
                }
            }
            LoggingWrapper::getinstance()->Debug("RMatrix: " + mstr);
            message = "Load FourColor RMatrix";
            preData.RMatrix = RMatrix;
            LoggingWrapper::getinstance()->Info(message);
        }
        m_preCalibrationData.RMatrix = preData.RMatrix;
        message = "Load FourColor RMatrix Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Load FourColor Data Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_LoadLuminanceData() {
    return Result();
}

Result ML::MLBusinessManage::MLBusinessManage::ML_DoFFCCorrect(int length,
                                                               int value) {
    std::string message;
    try {
        for (auto it : m_CalibrationDataMap
                 [ML::MLBusinessManage::CalibrationEnum::Result]) {
            if (!it.second.empty()) {
                FFCProcessor ffcProcessor;
                ffcProcessor.setFFCImage(
                    m_preCalibrationData.LightFFCMap[it.first],
                    m_preCalibrationData.DarkFFCMap[it.first], length, value);
                LoggingWrapper::getinstance()->Debug("Set ffc image finish");
                LoggingWrapper::getinstance()->Debug(
                    "binning: " +
                    std::to_string(pow(
                        2,
                        static_cast<int>(m_CaptureDataMap[it.first].Binning))));
                LoggingWrapper::getinstance()->Debug(
                    "exposure time: " +
                    std::to_string(m_CaptureDataMap[it.first].ExposureTime));
                ImageData prodata;
                prodata.img = it.second.clone() -
                              m_preCalibrationData.DarkFFCMap[it.first];
                prodata.bin = pow(
                    2, static_cast<int>(m_CaptureDataMap[it.first].Binning));
                prodata.expt = m_CaptureDataMap[it.first].ExposureTime;
                ImageData newdata = ffcProcessor.process(prodata);
                LoggingWrapper::getinstance()->Debug("process finish");

                m_CalibrationDataMap[ML::MLBusinessManage::CalibrationEnum::FFC]
                                    [it.first] = newdata.img.clone();
                m_CalibrationDataMap
                    [ML::MLBusinessManage::CalibrationEnum::Result][it.first] =
                        newdata.img.clone();

                message =
                    "Do Flat Field Correct For " + it.first + " ColorFilter";
                LoggingWrapper::getinstance()->Info(message);
            }
        }
        message = "Do Flat Field Correct Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Do Flat Field Correct Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_DoColorShiftCorrect() {
    std::string message;
    try {
        for (auto it : m_CalibrationDataMap
                 [ML::MLBusinessManage::CalibrationEnum::Result]) {
            if (!it.second.empty()) {
                ColorShiftProcessor processor;
                cv::Point2d point =
                    m_preCalibrationData
                        .PointMap[m_CaptureDataMap[it.first].ColorFilter];
                processor.setOffset(point);
                ImageData prodata;
                prodata.img = it.second.clone();
                prodata.bin = pow(
                    2, static_cast<int>(m_CaptureDataMap[it.first].Binning));
                prodata.expt = m_CaptureDataMap[it.first].ExposureTime;
                prodata = processor.process(prodata);

                m_CalibrationDataMap
                    [ML::MLBusinessManage::CalibrationEnum::ColorShift]
                    [it.first] = prodata.img.clone();
                m_CalibrationDataMap
                    [ML::MLBusinessManage::CalibrationEnum::Result][it.first] =
                        prodata.img.clone();
                message =
                    "Do Color Shift Correct For " + it.first + " ColorFilter";
                LoggingWrapper::getinstance()->Info(message);
            }
        }
        message = "Do Color Shift Correct Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Do Color Shift Correct Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_DoDistortionCorrect() {
    std::string message;
    try {
        for (auto it : m_CalibrationDataMap
                 [ML::MLBusinessManage::CalibrationEnum::Result]) {
            if (!it.second.empty()) {
                DistortionProcessor processor;
                processor.SetCameraMatrix(m_preCalibrationData.CameraMatrix);
                processor.SetCoefficient(m_preCalibrationData.Coefficient);
                ImageData prodata;
                prodata.img = it.second.clone();
                prodata.bin = pow(
                    2, static_cast<int>(m_CaptureDataMap[it.first].Binning));
                prodata.expt = m_CaptureDataMap[it.first].ExposureTime;
                prodata = processor.process(prodata);

                m_CalibrationDataMap
                    [ML::MLBusinessManage::CalibrationEnum::Distortion]
                    [it.first] = prodata.img.clone();
                m_CalibrationDataMap
                    [ML::MLBusinessManage::CalibrationEnum::Result][it.first] =
                        prodata.img.clone();

                message =
                    "Do Distrotion Correct For " + it.first + " ColorFilter";
                LoggingWrapper::getinstance()->Info(message);
            }
        }
        message = "Do Distrotion Correct Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Do Distrotion Correct Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_DoExposureCorrect() {
    std::string message;
    try {
        for (auto it : m_CalibrationDataMap
                 [ML::MLBusinessManage::CalibrationEnum::Result]) {
            if (!it.second.empty()) {
                ExposureTimeProcessor processor;
                ImageData prodata;
                prodata.img = it.second.clone();
                prodata.bin = pow(
                    2, static_cast<int>(m_CaptureDataMap[it.first].Binning));
                prodata.expt = m_CaptureDataMap[it.first].ExposureTime;
                processor.process(prodata);

                m_CalibrationDataMap
                    [ML::MLBusinessManage::CalibrationEnum::ExposureCorrect]
                    [it.first] = prodata.img.clone();
                m_CalibrationDataMap
                    [ML::MLBusinessManage::CalibrationEnum::Result][it.first] =
                        prodata.img.clone();

                message =
                    "Do Exposure Correct For " + it.first + " ColorFilter";
                LoggingWrapper::getinstance()->Info(message);
            }
        }
        message = "Do Exposure Correct Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Do Exposure Correct Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_DoFourColorCalculation() {
    std::string message;
    try {
        MLFourColor processor;
        processor.setRMatrix(m_preCalibrationData.RMatrix);
        std::map<std::string, cv::Mat> fcMap;
        std::map<std::string, cv::Mat> resultMap;
        for (auto it : m_CalibrationDataMap
                 [ML::MLBusinessManage::CalibrationEnum::Result]) {
            if (!it.second.empty()) {
                fcMap.insert(std::make_pair(it.first, it.second.clone()));
            }
        }
        resultMap = processor.fourColorProcess(fcMap);
        m_CalibrationDataMap[ML::MLBusinessManage::CalibrationEnum::FourColor] =
            resultMap;
        m_CalibrationDataMap[ML::MLBusinessManage::CalibrationEnum::Result] =
            resultMap;
        message = "Do Four Color Calculation Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Do Four Color Calculation Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_DoLuminanceCalculation() {
    return Result();
}

std::map<ML::MLBusinessManage::CalibrationEnum, std::map<std::string, cv::Mat>>
ML::MLBusinessManage::MLBusinessManage::ML_GetCalibrationData() {
    return m_CalibrationDataMap;
}

Result ML::MLBusinessManage::MLBusinessManage::ML_SaveCalibrationImage(
    std::string save_path, std::string prefix, cv::Rect rect, bool saveRaw,
    bool saveResult, bool saveCalibration) {
    std::string message;
    try {
        std::string filepath = save_path;
        if (filepath.back() != '\\' && filepath.back() != '/') {
            filepath += '\\';
        }
        if (!prefix.empty()) {
            filepath += prefix;
            if (filepath.back() != '\\' && filepath.back() != '/')
                filepath += '_';
        }
        filesystem::path folder = filesystem::path(filepath);
        if (!filesystem::is_directory(folder))
            folder = folder.parent_path();
        if (!filesystem::exists(folder)) {
            if (filesystem::create_directories(folder) == false) {
                message =
                    "Create directories for path: " + filepath + " Failed";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }

        if (saveRaw) {
            std::string enumStr = tranCalibrationEnumToString(
                ML::MLBusinessManage::CalibrationEnum::Raw);
            std::string ndFilter = "ND0";
            for (auto data : m_CalibrationDataMap
                     [ML::MLBusinessManage::CalibrationEnum::Raw]) {
                if (!data.second.empty()) {
                    if (data.first != "xx" && data.first != "yy") {
                        ndFilter = m_CaptureDataMap[data.first].NDFilter;
                    }
                    std::string imgPath;
                    std::string jsonPath;
                    if (m_CaptureDataMap[data.first].ColorLight == "") {
                        imgPath = filepath + ndFilter + "_" + data.first + "_" +
                                  enumStr + ".tif";
                        jsonPath = filepath + ndFilter + "_" + data.first +
                                   "_metadata.json";
                    } else {
                        imgPath = filepath + ndFilter + "_" +
                                  m_CaptureDataMap[data.first].ColorLight +
                                  "_" + data.first + "_" + enumStr + ".tif";
                        jsonPath = filepath + ndFilter + "_" +
                                   m_CaptureDataMap[data.first].ColorLight +
                                   "_" + data.first + "_metadata.json";
                    }
                    WriteMetaDatatoJson(jsonPath, m_CaptureDataMap[data.first]);
                    cv::Mat img = data.second.clone();
                    if (!rect.empty()) {
                        img = img(rect);
                    }
                    cv::imwrite(imgPath, img);
                    message = "Save Raw Image Successfully, path: " + imgPath;
                    LoggingWrapper::getinstance()->Info(message);
                }
            }
            message = "Save All Raws Image Successfully";
            LoggingWrapper::getinstance()->Info(message);
        }
        if (saveResult) {
            std::string ndFilter = "ND0";
            for (auto data : m_CalibrationDataMap
                     [ML::MLBusinessManage::CalibrationEnum::FourColor]) {
                std::string enumStr = tranCalibrationEnumToString(
                    ML::MLBusinessManage::CalibrationEnum::FourColor);
                if (data.first == "X" || data.first == "Z") {
                    continue;
                }
                if (!data.second.empty()) {
                    if (data.first != "xx" && data.first != "yy") {
                        ndFilter = m_CaptureDataMap[data.first].NDFilter;
                    }
                    std::string imgPath;
                    if (m_CaptureDataMap[data.first].ColorLight == "") {
                        imgPath = filepath + ndFilter + "_" + data.first + "_" +
                                  enumStr + ".tif";
                    } else {
                        imgPath = filepath + ndFilter + "_" +
                                  m_CaptureDataMap[data.first].ColorLight +
                                  "_" + data.first + "_" + enumStr + ".tif";
                    }
                    cv::Mat img = data.second.clone();
                    if (!rect.empty()) {
                        img = img(rect);
                    }
                    cv::imwrite(imgPath, img);
                    message = "Save" + enumStr +
                              "Image Successfully, path : " + imgPath;
                    LoggingWrapper::getinstance()->Info(message);
                }
            }
            message = "Save All Calibration Images Successfully";
            LoggingWrapper::getinstance()->Info(message);
        }
        if (saveCalibration) {
            std::string ndFilter = "ND0";
            for (auto cali : m_CalibrationDataMap) {
                /*if (cali.first ==
                    ML::MLBusinessManage::CalibrationEnum::Result) {
                    continue;
                }*/
                if (cali.first != ML::MLBusinessManage::CalibrationEnum::Raw &&
                    cali.first != ML::MLBusinessManage::CalibrationEnum::
                                      ExposureCorrect) {
                    std::string enumStr =
                        tranCalibrationEnumToString(cali.first);
                    for (auto data : cali.second) {
                        if (!data.second.empty()) {
                            if (data.first != "xx" && data.first != "yy") {
                                ndFilter =
                                    m_CaptureDataMap[data.first].NDFilter;
                            }
                            std::string imgPath;
                            if (m_CaptureDataMap[data.first].ColorLight == "") {
                                imgPath = filepath + ndFilter + "_" +
                                          data.first + "_" + enumStr + ".tif";
                            } else {
                                imgPath =
                                    filepath + ndFilter + "_" +
                                    m_CaptureDataMap[data.first].ColorLight +
                                    "_" + data.first + "_" + enumStr + ".tif";
                            }
                            cv::Mat img = data.second.clone();
                            if (!rect.empty()) {
                                img = img(rect);
                            }
                            cv::imwrite(imgPath, img);
                            message = "Save" + enumStr +
                                      "Image Successfully, path : " + imgPath;
                            LoggingWrapper::getinstance()->Info(message);
                        }
                    }
                }
            }
            message = "Save All Calibration Images Successfully";
            LoggingWrapper::getinstance()->Info(message);
        }
        message = "Save All Images Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Save Images Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ML_ClearCalibrationDataMap() {
    m_CalibrationDataMap.clear();
    return Result(true, "");
}

Result ML::MLBusinessManage::MLBusinessManage::ML_CaptureSDKFFCImage(
    ML::MLBusinessManage::ProcessPathConfig config, std::string cameraName,
    std::string imageName, int times) {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (camera.second.Name == cameraName) {
                if (!camera.second.Enable) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Enable";
                    LoggingWrapper::getinstance()->Warn(message);
                    return Result(false, message);
                }
                if (!static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                         ->IsOpened()) {
                    message = camera.second.Name + " " + camera.second.Type +
                              " Camera Is Not Connected";
                    LoggingWrapper::getinstance()->Error(message);
                    return Result(false, message);
                }
                std::vector<cv::Mat> imgList;
                for (int i = 0; i < times; i++) {
                    pluginException ex = static_cast<GrabberBase*>(
                                             m_addInBase[camera.second.Name])
                                             ->GrabOne();
                    if (!ex.getStatusFlag()) {
                        message = camera.second.Name + " " +
                                  camera.second.Type +
                                  " Camera Capture SDK FFC Image Failed";
                        LoggingWrapper::getinstance()->Error(message);
                        return Result(false, message);
                    }
                    cv::Mat img = static_cast<GrabberBase*>(
                                      m_addInBase[camera.second.Name])
                                      ->GetImage();
                    imgList.push_back(img);
                }
                cv::Mat avgImg = CalculateAverageImage(imgList);
                std::string partPath;
                partPath = MLBusinessManageConfig::instance()->ProcessPath(
                    "FFC", config);
                cv::imwrite(config.InputPath + "\\" + partPath, avgImg);
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Capture SDK FFC Image for NDFilter: " +
                          config.NDFilter +
                          " , ColorFilter: " + config.ColorFilter +
                          "Successfully";
                LoggingWrapper::getinstance()->Info(message);
                return Result(true, message);
            }
        }
        message = "Can not find the " + cameraName + " Camera";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = cameraName + " Camera Capture SDK FFC Image Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::CreateFilterWheel(
    ML::MLFilterWheel::MLNDFilterConfiguation configuration) {
    std::string message;
    try {
        ML_addInBase* addInBase =
            ML_AddinManger::getinstance().newPluginInstance(
                pluginType::typeFilterWheel, pluginName::filterWheel,
                configuration.name);
        pluginException ex =
            static_cast<FilterWheelBase*>(addInBase)->SetFilterWheelConfig(
                &configuration);
        if (!ex.getStatusFlag()) {
            message = "Create " + configuration.name + " FilterWheel Failed, " +
                      ex.getExceptionMsg();
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
        if (m_addInBase.count(configuration.name) == 0) {
            m_addInBase.insert(std::make_pair(configuration.name, addInBase));
        } else {
            m_addInBase[configuration.name] = addInBase;
        }
        message = "Create " + configuration.name + " FilterWheel Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        message = "Create " + configuration.name +
                  " FilterWheel Exception: " + e.what();
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::CreateRXFilterWheel(
    ML::MLFilterWheel::MLRXFilterConfiguation configuration) {
    std::string message;
    try {
        ML_addInBase* addInBase =
            ML_AddinManger::getinstance().newPluginInstance(
                pluginType::typeRxWheel, pluginName::rxFilterWheel,
                configuration.name);
        pluginException ex =
            static_cast<RxFilterWheelBase*>(addInBase)->SetFilterWheelConfig(
                &configuration);
        if (!ex.getStatusFlag()) {
            message = "Create " + configuration.name +
                      " RXFilterWheel Failed, " + ex.getExceptionMsg();
            LoggingWrapper::getinstance()->Info(message);
            return Result(false, message);
        }
        if (m_addInBase.count(configuration.name) == 0) {
            m_addInBase.insert(std::make_pair(configuration.name, addInBase));
        } else {
            m_addInBase[configuration.name] = addInBase;
        }
        message =
            "Create " + configuration.name + " RXFilterWheel Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        message = "Create " + configuration.name +
                  " RXFilterWheel Exception: " + e.what();
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::CreateMotion(
    ML::MLBusinessManage::MotionConfig config) {
    std::string message;
    pluginException ex;
    try {
        ML_addInBase* addInBase =
            ML_AddinManger::getinstance().newPluginInstance(
                pluginType::typeActuator, pluginName::motor, config.Name);
        ML::Motion::MotorConfigInfo info;
        info.sn = config.ConnectAddress;
        info.type = config.Type;
        std::map<std::string, std::string> axis;
        axis.insert(std::make_pair("AxisName", config.AxisName));
        info.axisInfo.push_back(axis);
        LoggingWrapper::getinstance()->Debug("PI sn: " + info.sn);
        LoggingWrapper::getinstance()->Debug("PI Type: " + info.type);
        ex = static_cast<ActuatorBase*>(addInBase)->InitialPluginConfig(&info);
        if (!ex.getStatusFlag()) {
            message = "Create " + config.Name + " " + config.Type +
                      " Failed, " + ex.getExceptionMsg();
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
        if (m_addInBase.count(config.Name) == 0) {
            m_addInBase.insert(std::make_pair(config.Name, addInBase));
        } else {
            m_addInBase[config.Name] = addInBase;
        }
        message =
            "Create " + config.Name + " " + config.Type + " Successfully ";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        message = "Create " + config.Name + " " + config.Type +
                  " Exception: " + e.what();
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::CreateCamera(
    ML::MLBusinessManage::CameraConfig config) {
    std::string message;
    try {
        ML_addInBase* addInBase =
            ML_AddinManger::getinstance().newPluginInstance(
                pluginType::typeDataIO_Grabber, pluginName::camera,
                config.Name);
        ML::CameraV2::CameraConfigInfo info;
        info.sn = config.ConnectAddress;
        info.type = config.Type;
        pluginException ex =
            static_cast<GrabberBase*>(addInBase)->InitialPluginConfig(&info);
        if (!ex.getStatusFlag()) {
            message = "Create " + config.Name + " " + config.Type +
                      " Failed, " + ex.getExceptionMsg();
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
        if (m_addInBase.count(config.Name) == 0) {
            m_addInBase.insert(std::make_pair(config.Name, addInBase));
        } else {
            m_addInBase[config.Name] = addInBase;
        }
        message =
            "Create " + config.Name + " " + config.Type + " Successfully ";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        message = "Create " + config.Name + " " + config.Type +
                  " Exception: " + e.what();
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ConnectFilterWheel() {
    std::string message;
    try {
        ML::MLBusinessManage::ModuleConfig config =
            MLBusinessManageConfig::instance()->GetModuleInfo();
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .NDFilterConfig_Map) {
            if (!filter.second.enable) {
                message = filter.second.name + " FilterWheel Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message = "Can not find " + filter.second.name + " FilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            pluginException ex =
                static_cast<FilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_OpenSerialSync();
            if (!ex.getStatusFlag()) {
                message = "Connect " + filter.second.name +
                          " FilterWheel Failed, " + ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "Connect All FilterWheel Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Connect All FilterWheel Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ConnectRXFilterWheel() {
    std::string message = "";
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            if (!filter.second.enable) {
                message = filter.second.name + " RXFilterWheel Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message =
                    "Can not find " + filter.second.name + " RXFilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            pluginException ex =
                static_cast<RxFilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_OpenSerialSync();
            if (!ex.getStatusFlag()) {
                message = "Connect " + filter.second.name +
                          " RXFilterWheel Failed, " + ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "Connect All RXFilterWheel Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Connect All RXFilterWheel Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ConnectMotion() {
    std::string message = "";
    try {
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (!motion.second.Enable) {
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(motion.second.Name) == 0) {
                message = "Can not find " + motion.second.Name + " Motion";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            LoggingWrapper::getinstance()->Debug(
                "try to connect " + motion.second.Name + " " +
                motion.second.Type + " motion");
            pluginException ex =
                static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                    ->Connect();
            if (!ex.getStatusFlag()) {
                message = "Connect " + motion.second.Name + " " +
                          motion.second.Type + " Motion Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            double pos = motion.second.ReferencePosistion;
            std::map<std::string, double> posMap;
            posMap[motion.second.AxisName] = pos * 1000.0;
            ex = static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                     ->MoveAbs(posMap);
            if (!ex.getStatusFlag()) {
                message = "Synchronously Set Focus to ReferencePosistion: " +
                          std::to_string(pos) + " Failed";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "Connect All Motions Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Connect All Motions Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    } catch (out_of_range) {
        message = "out of range";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::ConnectCamera() {
    std::string message = "";
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (!camera.second.Enable) {
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(camera.second.Name) == 0) {
                message = "Can not find " + camera.second.Name + " Camera";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            pluginException ex =
                static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                    ->Open();
            if (!ex.getStatusFlag()) {
                message = "Connect " + camera.second.Name + " " +
                          camera.second.Type + " Camera Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "Connect All Cameras Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Connect All Cameras Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::DisConnectFilterWheel() {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .NDFilterConfig_Map) {
            if (!filter.second.enable) {
                message = filter.second.name + " FilterWheel Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message = "Can not find " + filter.second.name + " FilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            pluginException ex =
                static_cast<FilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_CloseSerial();
            if (!ex.getStatusFlag()) {
                message = "DisConnect " + filter.second.name +
                          " FilterWheel Failed, " + ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            ex = ML_AddinManger::getinstance().closePluginInstance(
                pluginType::typeFilterWheel, pluginName::filterWheel,
                filter.second.name);
            if (!ex.getStatusFlag()) {
                message = "Close " + filter.second.name +
                          " FilterWheel Plugin Instance Failed";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "DisConnect All FilterWheel Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "DisConnect All FilterWheel Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::DisConnectRXFilterWheel() {
    std::string message;
    try {
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            if (!filter.second.enable) {
                message = filter.second.name + " RXFilterFilter Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message =
                    "Can not find " + filter.second.name + " RXFilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            pluginException ex =
                static_cast<RxFilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_CloseSerial();
            if (!ex.getStatusFlag()) {
                message = "DisConnect " + filter.second.name +
                          " RXFilterFilter Failed, " + ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            ex = ML_AddinManger::getinstance().closePluginInstance(
                pluginType::typeRxWheel, pluginName::rxFilterWheel,
                filter.second.name);
            if (!ex.getStatusFlag()) {
                message = "Close " + filter.second.name +
                          " RXFilterWheel Plugin Instance Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "DisConnect All RXFilterWheel Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "DisConnect All RXFilterWheel Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::DisConnectMotion() {
    std::string message;
    try {
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (!motion.second.Enable) {
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(motion.second.Name) == 0) {
                message = "Can not find " + motion.second.Name + " Motion";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            pluginException ex =
                static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                    ->Disconnect();
            if (!ex.getStatusFlag()) {
                message = "Connect " + motion.second.Name + " " +
                          motion.second.Type + " Motion Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            ex = ML_AddinManger::getinstance().closePluginInstance(
                pluginType::typeActuator, pluginName::motor,
                motion.second.Name);
            if (!ex.getStatusFlag()) {
                message = "Close " + motion.second.Name +
                          " Motion Plugin Instance Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "DisConnect All Motions Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "DisConnect All Motions Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

Result ML::MLBusinessManage::MLBusinessManage::DisConnectCamera() {
    std::string message;
    try {
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (!camera.second.Enable) {
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                return Result(true, message);
            }
            if (m_addInBase.count(camera.second.Name) == 0) {
                message = "Can not find " + camera.second.Name + " Camera";
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            pluginException ex =
                static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                    ->Close();
            if (!ex.getStatusFlag()) {
                message = "Connect " + camera.second.Name + " " +
                          camera.second.Type + " Camera Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
            ex = ML_AddinManger::getinstance().closePluginInstance(
                pluginType::typeDataIO_Grabber, pluginName::camera,
                camera.second.Name);
            if (!ex.getStatusFlag()) {
                message = "Close " + camera.second.Name +
                          " Camera Plugin Instance Failed, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                return Result(false, message);
            }
        }
        message = "DisConnect All Cameras Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "DisConnect All Cameras Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

bool ML::MLBusinessManage::MLBusinessManage::IsFilterWheelConnect() {
    std::string message;
    try {
        bool isConnect = false;
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .NDFilterConfig_Map) {
            if (!filter.second.enable) {
                message = filter.second.name + " FilterWheel Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                isConnect = true;
                return isConnect;
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message = "Can not find " + filter.second.name + " FilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                return false;
            }
            pluginException ex =
                static_cast<FilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_IsOpen();
            isConnect = ex.getStatusFlag();
            if (!ex.getStatusFlag()) {
                message = filter.second.name +
                          " FilterWheel Is DisConnected, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                isConnect = false;
                return isConnect;
            }
            message = "Ask All FilterWheel Connect Status, status = " +
                      std::to_string(isConnect);
            LoggingWrapper::getinstance()->Info(message);
            return isConnect;
        }
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Ask All FilterWheel Connect Status Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManage::IsRXFilterWheelConnect() {
    std::string message;
    try {
        bool isConnect = false;
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            if (!filter.second.enable) {
                message = filter.second.name + " RXFilterWheel Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                isConnect = true;
                return isConnect;
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message =
                    "Can not find " + filter.second.name + " RXFilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                return false;
            }
            pluginException ex =
                static_cast<RxFilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_IsOpen();
            isConnect = ex.getStatusFlag();
            if (!ex.getStatusFlag()) {
                message = filter.second.name +
                          " RXFilterWheel Is DisConnected, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                isConnect = false;
                return isConnect;
            }
            message = "Ask All RXFilterWheel Connect Status, status = " +
                      std::to_string(isConnect);
            LoggingWrapper::getinstance()->Info(message);
            return isConnect;
        }
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Ask All RXFilterWheel Connect Status Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManage::IsMotionConnect() {
    std::string message;
    try {
        bool _isConnect = true;
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            if (!motion.second.Enable) {
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                _isConnect = true;
                continue;
            }
            if (m_addInBase.count(motion.second.Name) == 0) {
                message = "Can not find " + motion.second.Name + " Motion";
                LoggingWrapper::getinstance()->Error(message);
                return false;
            }
            bool isConnect =
                static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                    ->IsConnected();
            _isConnect = _isConnect && isConnect;
        }
        message = "Ask All Motions Connecting Status, status = " +
                  std::to_string(_isConnect);
        LoggingWrapper::getinstance()->Info(message);
        return _isConnect;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Ask All Motions Connecting Status Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManage::IsCameraConnect() {
    std::string message;
    try {
        bool _isConnect = true;
        for (auto camera : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .CameraConfig_Map) {
            if (!camera.second.Enable) {
                message = camera.second.Name + " " + camera.second.Type +
                          " Camera Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                _isConnect = true;
                continue;
            }
            if (m_addInBase.count(camera.second.Name) == 0) {
                message = "Can not find " + camera.second.Name + " Camera";
                LoggingWrapper::getinstance()->Error(message);
                return false;
            }
            bool isConnect =
                static_cast<GrabberBase*>(m_addInBase[camera.second.Name])
                    ->IsOpened();
            _isConnect = _isConnect && isConnect;
        }
        message = "Ask All Cameras Connecting Status, status = " +
                  std::to_string(_isConnect);
        LoggingWrapper::getinstance()->Info(message);
        return _isConnect;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Ask All Cameras Connecting Status Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManage::IsFilterWheelMoving() {
    std::string message;
    try {
        bool _isMoving = false;
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .NDFilterConfig_Map) {
            bool isMoving = false;
            if (!filter.second.enable) {
                message = filter.second.name + " FilterWheel Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                isMoving = false;
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message =
                    "Can not find " + filter.second.name + " RXFilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                isMoving = false;
                _isMoving = _isMoving || isMoving;
                continue;
            }
            pluginException ex =
                static_cast<FilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_IsMoving();
            isMoving = ex.getStatusFlag();
            if (!ex.getStatusFlag()) {
                message = filter.second.name +
                          " FilterWheel Is DisConnected, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                isMoving = false;
            }
            _isMoving = _isMoving || isMoving;
        }
        return _isMoving;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Ask All FilterWheel Moving Status Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManage::IsRXFilterWheelMoving() {
    std::string message;
    try {
        bool _isMoving = false;
        for (auto filter : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .RXFilterConfig_Map) {
            bool isMoving = false;
            if (!filter.second.enable) {
                message = filter.second.name + " RXFilterWheel Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                isMoving = false;
            }
            if (m_addInBase.count(filter.second.name) == 0) {
                message =
                    "Can not find " + filter.second.name + " RXFilterWheel";
                LoggingWrapper::getinstance()->Error(message);
                isMoving = false;
                _isMoving = _isMoving || isMoving;
                continue;
            }
            pluginException ex =
                static_cast<RxFilterWheelBase*>(m_addInBase[filter.second.name])
                    ->ML_IsMoving();
            isMoving = ex.getStatusFlag();
            if (!ex.getStatusFlag()) {
                message = filter.second.name +
                          " RXFilterWheel Is DisConnected, " +
                          ex.getExceptionMsg();
                LoggingWrapper::getinstance()->Error(message);
                isMoving = false;
            }
            _isMoving = _isMoving || isMoving;
        }
        return _isMoving;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Ask All RXFilterWheel Moving Status Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

bool ML::MLBusinessManage::MLBusinessManage::IsMotionMoving() {
    std::string message;
    try {
        bool _isMoving = false;
        for (auto motion : MLBusinessManageConfig::instance()
                               ->GetModuleInfo()
                               .MotionConfig_Map) {
            bool isMoving = false;
            if (!motion.second.Enable) {
                message = motion.second.Name + " " + motion.second.Type +
                          " Motion Is Not Enable";
                LoggingWrapper::getinstance()->Warn(message);
                isMoving = false;
            }
            if (m_addInBase.count(motion.second.Name) == 0) {
                message = "Can not find " + motion.second.Name + " Motion";
                LoggingWrapper::getinstance()->Error(message);
                isMoving = false;
                _isMoving = _isMoving || isMoving;
                continue;
            }
            isMoving =
                static_cast<ActuatorBase*>(m_addInBase[motion.second.Name])
                    ->IsMoving();
            _isMoving = _isMoving || isMoving;
        }
        return _isMoving;
    } catch (const std::exception e) {
        std::string ex = e.what();
        message = "Ask All Motions Moving Status Exception: " + ex;
        LoggingWrapper::getinstance()->Error(message);
        return false;
    }
}

std::string ML::MLBusinessManage::MLBusinessManage::tranCalibrationEnumToString(
    ML::MLBusinessManage::CalibrationEnum caliEnum) {
    std::string str;
    if (caliEnum == ML::MLBusinessManage::CalibrationEnum::Raw) {
        str = "Raw";
    } else if (caliEnum == ML::MLBusinessManage::CalibrationEnum::FFC) {
        str = "FFC";
    } else if (caliEnum == ML::MLBusinessManage::CalibrationEnum::ColorShift) {
        str = "ColorShift";
    } else if (caliEnum == ML::MLBusinessManage::CalibrationEnum::Distortion) {
        str = "Distortion";
    } else if (caliEnum == ML::MLBusinessManage::CalibrationEnum::FourColor) {
        str = "FourColor";
    } else if (caliEnum ==
               ML::MLBusinessManage::CalibrationEnum::ExposureCorrect) {
        str = "ExposureCorrect";
    } else if (caliEnum == ML::MLBusinessManage::CalibrationEnum::Lumiance) {
        str = "Luminance";
    } else if (caliEnum == ML::MLBusinessManage::CalibrationEnum::Result) {
        str = "Result";
    } else {
        str = "";
    }
    return str;
}

Result ML::MLBusinessManage::MLBusinessManage::WriteMetaDatatoJson(
    std::string filename, ML::MLBusinessManage::CaptureData data) {
    std::string message;
    try {
        nlohmann::json object;
        object["SerialNumber"] = data.SerialNumber;
        object["ModuleName"] = data.ModuleName;
        object["Aperture"] = data.Aperture;
        // object["ColorLight"] = data.ColorLight;
        object["NDFilter"] = data.NDFilter;
        object["ColorFilter"] = data.ColorFilter;
        // object["RXFilter"] = data.RXFilter;
        // object["RXAxisDegree"] = data.RXAxisDegree;
        object["VID"] = std::to_string(data.VID);
        object["ExposureTime"] = data.ExposureTime;
        object["Binning"] = transBinningToString(data.Binning);
        object["BitDepth"] = transMLPixelFormatToString(data.BitDepth);

        std::ofstream ofs(filename, std::ios_base::trunc);
        if (!ofs.is_open()) {
            message = "Open file: " + filename + " failed";
            LoggingWrapper::getinstance()->Error(message);
            return Result(false, message);
        }
        ofs << object.dump(4);
        ofs.close();
        message = "Write MetaData to Json file: " + filename + " Successfully";
        LoggingWrapper::getinstance()->Info(message);
        return Result(true, message);

    } catch (const std::exception e) {
        message = "Write MetaData to Json file Exception";
        LoggingWrapper::getinstance()->Error(message);
        return Result(false, message);
    }
}

std::string ML::MLBusinessManage::MLBusinessManage::transBinningToString(
    ML::CameraV2::Binning binning) {
    std::string str;
    switch (binning) {
        case ML::CameraV2::ONE_BY_ONE:
            str = "ONE_BY_ONE";
            break;
        case ML::CameraV2::TWO_BY_TWO:
            str = "TWO_BY_TWO";
            break;
        case ML::CameraV2::FOUR_BY_FOUR:
            str = "FOUR_BY_FOUR";
            break;
        case ML::CameraV2::EIGHT_BY_EIGHT:
            str = "EIGHT_BY_EIGHT";
            break;
        default:
            str = "ONE_BY_ONE";
            break;
    }
    return str;
}

std::string ML::MLBusinessManage::MLBusinessManage::transMLPixelFormatToString(
    ML::CameraV2::MLPixelFormat format) {
    std::string str;
    switch (format) {
        case ML::CameraV2::MLPixelFormat::MLMono8:
            str = "MLMono8";
            break;
        case ML::CameraV2::MLPixelFormat::MLMono10:
            str = "MLMono10";
            break;
        case ML::CameraV2::MLPixelFormat::MLMono12:
            str = "MLMono12";
            break;
        case ML::CameraV2::MLPixelFormat::MLMono16:
            str = "MLMono16";
            break;
        case ML::CameraV2::MLPixelFormat::MLRGB24:
            str = "MLRGB24";
            break;
        case ML::CameraV2::MLPixelFormat::MLBayer:
            str = "MLBayer";
            break;
        case ML::CameraV2::MLPixelFormat::MLBayerGB8:
            str = "MLBayerGB8";
            break;
        case ML::CameraV2::MLPixelFormat::MLBayerGB10:
            str = "MLBayerGB10";
            break;
        case ML::CameraV2::MLPixelFormat::MLBayerGB12:
            str = "MLBayerGB12";
            break;
        default:
            str = "MLMono12";
            break;
    }
    return str;
}

cv::Mat ML::MLBusinessManage::MLBusinessManage::CalculateAverageImage(
    std::vector<cv::Mat> image_list) {
    cv::Mat avgMat;
    if (image_list.size() > 0) {
        cv::Mat sumMat = cv::Mat::zeros(image_list[0].size(), CV_32FC1);
        avgMat = cv::Mat::zeros(image_list[0].size(), CV_32FC1);
        int num = image_list.size();
        for (int i = 0; i < num; i++) {
            cv::Mat temp;
            image_list[i].convertTo(temp, CV_32FC1);
            sumMat = sumMat + temp;
        }
        avgMat = sumMat / num;
    }
    cv::Mat avg16;
    avgMat.convertTo(avg16, CV_16UC1);
    return avg16;
}
