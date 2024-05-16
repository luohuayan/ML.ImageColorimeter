// Copyright 2023 MLOptic
#pragma once
// #include "ml_addinmanger_global.h"
#include <QObject>
#include <map>
#include <list>
#include <string>
#include <opencv2/opencv.hpp>
#include "pluginException.h"

typedef enum PluginType {
    typeDataIO,  // !< base type for data input and output
                 // (cameras, AD-converter,
                // display windows...)
    typeActuator,       /*!< base type for actuators and motors */
    typeAlgo,           /*!< base type for algorithm plugin */
    typeDataIO_Grabber,  // !< subtype of dataIO for
                        // cameras (grabbers), use this
                        //  type in combination with typeDataIO (OR-combination)
    typeDataIO_ADDA,  // !< subtype of dataIO for AD and DA-converters, use this
                       // type in combination with typeDataIO (OR-combination)
    typeDataIO_RawIO,  // !< subtype of dataIO for further input-output-devices
                       //  (like display windows), use this type in combination
                       //  with typeDataIO (OR-combination)
    typeFilterWheel,  // base type for NDFilter and XYZFilter
    typeRxWheel       // base type for Rx Filter
} pluginType;

/*
 *   Plugin information, including version, type, id, author, etc. of the plugin
 */
typedef struct PluginInfo {
    pluginType plugin_type;   // plugin type
    std::string version;      // pugin version
    std::string author;       // plugin author
    std::string description;  // a brief description of the plugin
    std::string licenseInfo;  // a detailed description of the plugin license
    std::string pluginName;   // pugin name
    int pluginID;             // plugin id
} pluginInfo;

/*
 * Information about the plugin instance, including the ID and name of the
 * plugin instance A plugin can have multiple plugin instances
 */
typedef struct PluginInstanceInfo {
    int pluginInstanceID;
    std::string pluginInstanceName;
} pluginInstanceInfo;

typedef enum PluginName {
    filterWheel,
    rxFilterWheel,
    motor,
    camera
} pluginName;

// device infomation
typedef struct DeviceInfo {
    std::string name;
    std::string sn;
    std::string type;
} deviceInfo;

class ADDINMANGER_API ML_addInBase;
class ADDINMANGER_API ML_addInInterfaceBase : public QObject {
    Q_OBJECT

 public:
    ML_addInInterfaceBase();
    ~ML_addInInterfaceBase();

 public:
    // get Plugin Info
    pluginInfo getPluginInfo(void);

    // set plugin ID
    void setPluginID(int pluginID);

    //! returns the plugin path
    std::string getPluginPath(void);

    // set plugin path
    void setPluginPath(std::string pluginPath);

    // create plugin instance
    virtual ML_addInBase* createPluginInstance(
        std::string pluginInstanceName = "") = 0;

    // close plugin instance
    virtual pluginException closePluginInstance(
        ML_addInBase* pluginInstance) = 0;

    // get plugin instance
    std::map<int, ML_addInBase*> getPluginInstance(
        std::string pluginInstanceName = "");
    ML_addInBase* getPluginInstance(int pluginInstanceID);

    // Obtain a list of base classes for plugin functionality
    std::list<ML_addInBase*> getAddInBaseList();

    // Is the plugin instance in a new thread
    bool isPluginInNewThread(ML_addInBase* base);

    /***********Future development***************/
    // GUI support
    // bool hasGuiSupport()
 protected:
    std::string m_pluginPath;
    pluginInfo m_pluginInfo;
    std::list<ML_addInBase*> pluginInstanceList;
};

// class QThread;
class ADDINMANGER_API ML_addInBase : public QObject {
    Q_OBJECT

 public:
    ML_addInBase();
    ~ML_addInBase();

 public:
    // sets the interface of this instance to base
    void setBasePlugin(ML_addInInterfaceBase* base);

    // returns the interface of this instance
    ML_addInInterfaceBase* getBasePlugin();

    // get plugin instance name
    std::string getPluginInstanceName();

    //! creates new thread for the class instance and moves this instance to the
    //! new thread
    pluginException MoveToThread(void);

    // is plugin instance in new thread
    bool isPluginInstanceInNewThread();

 protected:
    // std::string m_pInstanceName;

 private:
    QThread* m_thread = nullptr;
    ML_addInInterfaceBase* m_addInInterfaceBasePtr;
    bool m_isPluginInstanceInNewThread;
};

namespace ML {
namespace MLFilterWheel {
class MLNDFilterWheel;
class MLRXFilterWheel;
class MLFilterWheelCallback;
struct MLNDFilterConfiguation;
struct MLRXFilterConfiguation;
enum MLFilterEnum;
enum MLFilterStatus;
}
}  // namespace ML
class ADDINMANGER_API FilterWheelBase : public ML_addInBase {
    Q_OBJECT

 public:
    FilterWheelBase();
    ~FilterWheelBase();

 public:
    virtual pluginException SetFilterWheelConfig(
        ML::MLFilterWheel::MLNDFilterConfiguation* config) = 0;

    virtual void Subscribe() = 0;

    virtual void UnSubscribe() = 0;

    virtual pluginException ML_OpenSerial(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_OpenSerialSync() = 0;

    virtual pluginException ML_CloseSerial() = 0;

    virtual pluginException ML_IsOpen() = 0;

    virtual pluginException ML_IsMoving() = 0;

    virtual pluginException ML_StopMove() = 0;

    virtual pluginException ML_ResetFilter(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_ResetFilterSync() = 0;

    virtual pluginException ML_MoveByIndex(
        int index,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByIndexSync(int index) = 0;

    virtual pluginException ML_MoveByEnum(
        ML::MLFilterWheel::MLFilterEnum filter_enum,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByEnumSync(
        ML::MLFilterWheel::MLFilterEnum filter_enum) = 0;

    virtual pluginException ML_MoveByName(
        std::string name,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByNameSync(std::string name) = 0;

    virtual int ML_GetFilterIndex() = 0;

    virtual ML::MLFilterWheel::MLFilterEnum ML_GetFilterEnum() = 0;

    virtual std::string ML_GetFilterName() = 0;

    virtual ML::MLFilterWheel::MLFilterStatus ML_GetStatus() = 0;

    virtual pluginException ML_WaitForFinish(int timeout = 20000) = 0;
};

class ADDINMANGER_API RxFilterWheelBase : public ML_addInBase {
    Q_OBJECT

 public:
    RxFilterWheelBase();
    ~RxFilterWheelBase();

 public:
    virtual pluginException SetFilterWheelConfig(
        ML::MLFilterWheel::MLRXFilterConfiguation* config) = 0;

    virtual void Subscribe() = 0;

    virtual void UnSubscribe() = 0;

    virtual pluginException ML_OpenSerial(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_OpenSerialSync() = 0;

    virtual pluginException ML_CloseSerial() = 0;

    virtual pluginException ML_IsOpen() = 0;

    virtual pluginException ML_IsMoving() = 0;

    virtual pluginException ML_StopMove() = 0;

    virtual pluginException ML_ResetFilter(
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_ResetFilterSync() = 0;

    virtual pluginException ML_MoveByIndex(
        int index,
        int axis = 0,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByIndexSync(int index, int axis = 0) = 0;

    virtual pluginException ML_MoveByName(
        std::string name,
        int axis = 0,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr) = 0;

    virtual pluginException ML_MoveByNameSync(std::string name,
                                              int axis = 0) = 0;

    virtual int ML_GetFilterIndex() = 0;

    virtual std::string ML_GetFilterName() = 0;

    virtual int ML_GetAxisDegree() = 0;

    virtual ML::MLFilterWheel::MLFilterStatus ML_GetStatus() = 0;

    virtual pluginException ML_WaitForFinish(int timeout = 20000) = 0;
};

namespace ML {
namespace Motion {
enum MLMotionState;
enum MLMotionEvent;
class MLMotionCallback;
struct MotorConfigInfo;
}  // namespace Motion
}  // namespace ML

class ADDINMANGER_API ActuatorBase : public ML_addInBase {
    Q_OBJECT

 public:
    ActuatorBase();
    ~ActuatorBase();

 public:
    // virtual pluginException InitialConfig(std::string configPath) = 0;
    virtual pluginException InitialPluginConfig(
        ML::Motion::MotorConfigInfo* motorConfig) = 0;
    virtual pluginException Connect() = 0;
    virtual pluginException Disconnect() = 0;
    virtual bool IsConnected() = 0;
    virtual pluginException SetEnable(bool enable) = 0;
    virtual bool IsEnabled() = 0;
    virtual pluginException MoveRelAsync(
        std::map<std::string, double> axispos) = 0;
    virtual pluginException MoveAbsAsync(
        std::map<std::string, double> axispos) = 0;
    virtual pluginException MoveRel(std::map<std::string, double> axispos) = 0;
    virtual pluginException MoveAbs(std::map<std::string, double> axispos) = 0;
    virtual pluginException StopMove() = 0;
    virtual pluginException Homing() = 0;
    virtual bool IsHome() = 0;
    virtual pluginException SetSpeed(int speedX) = 0;
    virtual int GetSpeed() = 0;
    virtual bool IsMoving() = 0;
    virtual pluginException GetPosition(
        std::map<std::string, double>* axispos) = 0;
    virtual ML::Motion::MLMotionState GetState() = 0;
    virtual pluginException Subscribe(
        ML::Motion::MLMotionEvent event,
                           ML::Motion::MLMotionCallback* callback) = 0;
    virtual pluginException Unsubscribe(
        ML::Motion::MLMotionEvent event,
                             ML::Motion::MLMotionCallback* callback) = 0;
};


namespace ML {
namespace CameraV2 {
class MLCameraCallback;
enum class MLPixelFormat;
enum AutoExposureStatus;
enum class MLExposureMode;
enum class MLCameraState;
enum class MLCameraEvent;
enum Binning;
enum BinningMode;
enum TriggerSource;
struct CameraConfigInfo;
}  // namespace CameraV2
}  // namespace ML

class ADDINMANGER_API GrabberBase : public ML_addInBase {
    Q_OBJECT

 public:
    GrabberBase();
    ~GrabberBase();

 public:
    virtual pluginException InitialPluginConfig(
        ML::CameraV2::CameraConfigInfo* motorConfig) = 0;
    virtual pluginException InitializeAEConfig(const char* configfilePath,
                                               std::string cameraNameFromJson,
                                               int init_Time = 1000,
                                               bool isSetInitTime = false) = 0;
    virtual pluginException Open() = 0;
    // virtual pluginException OpenBySN(const char* SN) = 0;
    virtual pluginException Close() = 0;
    virtual bool IsOpened() = 0;
    virtual pluginException GrabOne() = 0;
    virtual pluginException StartGrabbingAsync() = 0;
    virtual pluginException StopGrabbing() = 0;
    virtual bool IsGrabbing() = 0;
    virtual int GetWidth() = 0;
    virtual int GetHeight() = 0;
    virtual pluginException SetFrameRate(double rate) = 0;
    virtual double GetFrameRate() = 0;
    virtual pluginException SetFormatType(
        ML::CameraV2::MLPixelFormat format) = 0;
    virtual ML::CameraV2::MLPixelFormat GetFormatType() = 0;
    virtual ML::CameraV2::AutoExposureStatus SetMLExposureAuto() = 0;
    virtual pluginException SetExposureMode(
        ML::CameraV2::MLExposureMode mode) = 0;
    virtual ML::CameraV2::MLExposureMode GetExposureMode() = 0;
    virtual pluginException SetExposureTime(double time) = 0;
    virtual double GetExposureTime() = 0;
    virtual pluginException GetExposureTimeRange(double& min, double& max) = 0;
    virtual pluginException SetGain(double value) = 0;
    virtual double GetGain() = 0;
    virtual pluginException GetGainRange(double& min, double& max) = 0;
    virtual pluginException SetAdjustExposureTime(double time) = 0;
    virtual ML::CameraV2::MLCameraState GetState() = 0;
    virtual pluginException FlipX(bool flip) = 0;
    virtual pluginException FlipY(bool flip) = 0;
    virtual pluginException Subscribe(
        ML::CameraV2::MLCameraEvent event,
        ML::CameraV2::MLCameraCallback* callback) = 0;
    virtual pluginException Unsubscribe(
        ML::CameraV2::MLCameraEvent event,
        ML::CameraV2::MLCameraCallback* callback) = 0;
    virtual cv::Mat GetImage() = 0;
    virtual pluginException StartCalcGrayLevel() = 0;
    virtual pluginException StopCalcGrayLevel() = 0;
    virtual pluginException Rotate(bool rotate) = 0;
    virtual pluginException SetBinning(ML::CameraV2::Binning binning) = 0;
    virtual pluginException SetBinningMode(
        ML::CameraV2::BinningMode binningMode) = 0;
    virtual ML::CameraV2::Binning GetBinning() = 0;
    virtual ML::CameraV2::BinningMode GetBinningMode() = 0;
    virtual pluginException SetTemperature(int temperature) = 0;
    virtual int GetTemperature() = 0;
    virtual pluginException SetTriggerSource(
        ML::CameraV2::TriggerSource source) = 0;

 public:
    std::string serial_number_;
    std::string cameraNameFromJson;
    int init_Time_;
    bool isSetInitTime;
};
// Plug-in identifier
#define AbstractInterface_iid "ML addInInterfaceBase"
// Register plug-in interface with Qt meta-object system
Q_DECLARE_INTERFACE(ML_addInInterfaceBase, AbstractInterface_iid)
