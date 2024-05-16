// Copyright 2024 MLOptic

#ifndef SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGE_H_
#define SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGE_H_

#include <QObject>

#include "MLBusinessManageClass.h"
#include "MLBusinessManagePlugin.h"
#include "MLCamaraCommon.h"
#include "mlbusinessmanage_global.h"

/// <summary>
///  This file contains the declaration of the interface class MLBusinessManage.
/// The class provides a common interface to manage calibration process and
/// hardware control including FilterWheel, PI Motor and Camera.
/// </summary>

class ML_addInBase;

namespace ML {
namespace MLFilterWheel {
class MLFilterWheelCallback;
}  // namespace MLFilterWheel

namespace MLBusinessManage {

class MLBUSINESSMANAGE_EXPORT MLBusinessManage : public QObject,
                                                 public MLBusinessManagePlugin {
    Q_OBJECT
    /*
     *Use Q_INTERFACES() macro tells Qt's meta-object system about the interface
     *and tells the meta-object system "I want to use this interface
     */
    Q_INTERFACES(MLBusinessManagePlugin)
    /*
     * Use Q_ PLUGIN_ METADATA() macro export plug-in
     */
    Q_PLUGIN_METADATA(IID AbstractInterface_iid FILE "MLBusinessManage.json")

 public:
    MLBusinessManage(QObject* parent = nullptr);
    ~MLBusinessManage();

    /// <summary>
    /// Initialize hardware config profile of MLBusinessManage.
    /// </summary>
    /// <param name="path">hardware config file path</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetConfigPath(const char* path);

    /// <summary>
    /// Create a module for MLBusinessManage, which contains filter wheel,
    /// motors and cameras.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_CreateModule();

    /// <summary>
    /// Connect the module created by ML_CreateModule().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_ConnectModule();

    /// <summary>
    /// DisConnect the module created by ML_CreateModule().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_DisconnectModule();

    /// <summary>
    /// Check if all members in the module created by ML_CreateModule() are
    /// connected.
    /// </summary>
    virtual bool ML_IsModuleConnect();

    /// <summary>
    /// Check if all members in the module created by ML_CreateModule() are
    /// moving.
    /// </summary>
    virtual bool ML_IsModuleMotorsMoving();

    /// <summary>
    /// Wait for all members' movement finish in the module created by
    /// ML_CreateModule().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_WaitForMovingStop(int timeout);

    /// <summary>
    /// Obtain the serial number of the module.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual std::string ML_GetModuleSerialNumber();

    /// <summary>
    /// Obtain the name of the module.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual std::string ML_GetModuleName();

    /// <summary>
    /// Switch ND/XYZ filter wheel's channel by name asynchronously.
    /// </summary>
    /// <param name="filterName">the name of FilterWheel, from the
    /// config</param>
    /// <param name="channelName">the name of channel to switch</param>
    /// <param name="cb">callback function</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_MoveND_XYZFilterByNameAsync(
        std::string filterName, std::string channelName,
        ML::MLFilterWheel::MLFilterWheelCallback* cb = nullptr);

    /// <summary>
    /// Switch ND/XYZ filter wheel's channel by name synchronously.
    /// </summary>
    /// <param name="filterName">the name of FilterWheel, from the config
    /// file</param>
    /// <param name="channelName">the name of channel to switch</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_MoveND_XYZFilterByNameSync(std::string filterName,
                                                 std::string channelName);

    /// <summary>
    /// Obtain the channel name of ND/XYZ FilterWheel.
    /// </summary>
    /// <param name="filterName">the name of FilterWheel, from the config
    /// file</param>
    virtual std::string ML_GetND_XYZChannelName(std::string filterName);

    /// <summary>
    /// Switch RX filter wheel's channel by name and change RX axis degree
    /// asynchronously.
    /// </summary>
    /// <param name="filterName">the name of FilterWheel, from the
    /// config</param>
    /// <param name="channelName">the name of channel to switch</param>
    /// <param name="degree">the degree of axis</param>
    /// <param name="cb">callback function</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_MoveRXFilterByNameAsync(
        std::string filterName, std::string channelName, int degree,
        ML::MLFilterWheel::MLFilterWheelCallback* cb);

    /// <summary>
    /// Switch RX filter wheel's channel by name and change RX axis degree
    /// synchronously.
    /// </summary>
    /// <param name="filterName">the name of FilterWheel, from the
    /// config</param> <param name="channelName">the name of channel to
    /// switch</param> <param name="degree">the degree of axis</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_MoveRXFilterByNameSync(std::string filterName,
                                             std::string channelName,
                                             int degree);

    /// <summary>
    /// Obtain the channel's name of RXFilterWheel.
    /// </summary>
    /// <param name="filterName">the name of RXFilterWheel, from the config
    /// file</param>
    virtual std::string ML_GetRXFilterName(std::string filterName);

    /// <summary>
    /// Obtain the degree of RXFilterWheel's axis.
    /// </summary>
    /// <param name="filterName">the name of RXFilterWheel, from the config
    /// file</param>
    virtual int ML_GetRXFilterAxis(std::string filterName);

    /// <summary>
    /// Set Focus by vid asynchronously.
    /// </summary>
    /// <param name="vid">the vid to set (millimeter)</param>
    /// <param name="method">focus method, default: OneMotor</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetFocusAsync(
        double vid, ML::MLBusinessManage::FoucsMethod method =
                        ML::MLBusinessManage::FoucsMethod::OneMotor);

    /// <summary>
    /// Set Focus by vid synchronously.
    /// </summary>
    /// <param name="vid">the vid to set (millimeter)</param>
    /// <param name="method">focus method, default: OneMotor</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetFocusSync(
        double vid, ML::MLBusinessManage::FoucsMethod method =
                        ML::MLBusinessManage::FoucsMethod::OneMotor);

    /// <summary>
    /// Obtain vid (millimeter).
    /// </summary>
    /// <param name="method">focus method, default: OneMotor</param>
    virtual double ML_GetVID(ML::MLBusinessManage::FoucsMethod method =
                                 ML::MLBusinessManage::FoucsMethod::OneMotor);

    /// <summary>
    /// Set absolute motion position asynchronously.
    /// </summary>
    /// <param name="motionName">the name of Motion, from the config</param>
    /// <param name="pos">absolute motion position to set (millimeter)</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetPosistionAbsAsync(std::string motionName, double pos);

    /// <summary>
    /// Set absolute motion position synchronously.
    /// </summary>
    /// <param name="motionName">the name of Motion, from the config</param>
    /// <param name="pos">absolute motion position to set (millimeter)</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetPosistionAbsSync(std::string motionName, double pos);

    /// <summary>
    /// Set relative motion position asynchronously.
    /// </summary>
    /// <param name="motionName">the name of Motion, from the config</param>
    /// <param name="pos">relative motion position to set (millimeter)</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetPositionRelAsync(std::string motionName, double pos);

    /// <summary>
    /// Set relative motion position synchronously.
    /// </summary>
    /// <param name="motionName">the name of Motion, from the config</param>
    /// <param name="pos">relative motion position to set (millimeter)</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetPositionRelSync(std::string motionName, double pos);

    /// <summary>
    /// Obtain motion position (millimeter).
    /// </summary>
    /// <param name="motionName">the name of Motion, from the config</param>
    virtual double ML_GetMotionPosition(std::string motionName);

    /// <summary>
    /// Stop motion movement.
    /// </summary>
    /// <param name="motionName">the name of Motion, from the config</param>
    virtual Result ML_StopMotionMovement(std::string motionName);

    /// <summary>
    /// Set aperture.
    /// </summary>
    /// <param name="aperture>aperture to set</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetAperture(std::string aperture);

    /// <summary>
    /// Obtain aperture.
    /// </summary>
    virtual std::string ML_GetAperture();

    /// <summary>
    /// Set colorLight.
    /// </summary>
    /// <param name="colorLight>colorLight to set</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetColorLight(std::string colorLight);

    /// <summary>
    /// Obtain colorLight.
    /// </summary>
    virtual std::string ML_GetColorLight();

    /// <summary>
    /// Set exposure time for camera.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param> <param name="expTime">exposure time to set</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetExposureTime(std::string cameraName, double expTime);

    /// <summary>
    /// Obtain exposure time.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    virtual double ML_GetExposureTime(std::string cameraName);

    /// <summary>
    /// Set camera binning.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param> <param name="binning">binning to set</param> <returns>the
    /// result contains the message, code and status</returns>
    virtual Result ML_SetBinning(std::string cameraName,
                                 ML::CameraV2::Binning binning);

    /// <summary>
    /// Obtain camera binning.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    virtual ML::CameraV2::Binning ML_GetBinning(std::string cameraName);

    /// <summary>
    /// Set camera binning mode.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <param name="binningMode">binning mode to set</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetBinningMode(std::string cameraName,
                                     ML::CameraV2::BinningMode binningMode);

    /// <summary>
    /// Obtain camera binning mode.
    /// </summary>
    virtual ML::CameraV2::BinningMode ML_GetBinningMode(std::string cameraName);

    /// <summary>
    /// Set camera bit depth.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <param name="bitDepth">bit depth to set</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SetBitDepth(std::string cameraName,
                                  ML::CameraV2::MLPixelFormat bitDepth);

    /// <summary>
    /// Obtain camera bit depth.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    virtual ML::CameraV2::MLPixelFormat ML_GetBitDepth(std::string cameraName);

    /// <summary>
    /// Obtain image gray level by ROI.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <param name="rect">roi to set</param>
    virtual double ML_GetGrayLevel(std::string cameraName, cv::Rect rect);

    /// <summary>
    /// Obtain image gray level by image pixels.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <param name="pixelConut">pixel conut to get gray level</param>
    virtual double ML_GetGrayLevel(std::string cameraName, int pixelConut);

    /// <summary>
    /// Auto expsoure asynchronously.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <param name="initialTime">initial time of auto exposure</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_AutoExposureAsync(std::string cameraName,
                                        int initialTime = 100000);

    /// <summary>
    /// Auto expsoure synchronously.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <param name="initialTime">initial time of auto exposure</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_AutoExposureSync(std::string cameraName,
                                       int initialTime = 100000);

    /// <summary>
    /// Capture Image asynchronously.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_CaptureImageAsync(std::string cameraName);

    /// <summary>
    /// Capture Image synchronously.
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_CaptureImageSync(std::string cameraName);

    /// <summary>
    /// Obtain image after calling ML_CaptureImageAsync() or
    /// ML_CaptureImageSync().
    /// </summary>
    /// <param name="cameraName">the name field of Camera, from the
    /// config</param>
    virtual cv::Mat ML_GetImage(std::string cameraName);

    /// <summary>
    /// Obtain CaptureData.
    /// </summary>
    virtual ML::MLBusinessManage::CaptureData ML_GetCaptureData();

    /// <summary>
    /// Obtain map of CaptureData for color camera.
    /// </summary>
    /// <returns>a map of CaptureData;
    /// string: the color filter's channel, contains X, Y, Z, Empty,
    /// etc;</returns>
    virtual std::map<std::string, ML::MLBusinessManage::CaptureData>
    ML_GetColorCameraCaptureData();

    /// <summary>
    /// Obtain a module pointer by name.
    /// </summary>
    /// <param name="name">the Name field of module, from the config
    /// file</param>
    virtual ML_addInBase* ML_GetOneModuleByName(std::string name);

    // calibration
    /// <summary>
    /// Set map of CaptureData to do calibration process.
    /// </summary>
    /// <param name="data">a map of CaptureData;
    /// string: the color filter's channel, contains X, Y, Z, Empty,
    /// etc;</param>
    virtual Result ML_SetCaptureDataMap(
        std::map<std::string, ML::MLBusinessManage::CaptureData> data);

    /// <summary>
    /// Load flat field data.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_LoadFFCData();

    /// <summary>
    /// Load color shift data.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_LoadColorShiftData();

    /// <summary>
    /// Load distortion data.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_LoadDistortionData();

    /// <summary>
    /// Load Exposure data.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_LoadExpsoureData();

    /// <summary>
    /// Load four color data.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_LoadFourColorData();

    /// <summary>
    /// Load luminance data.
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_LoadLuminanceData();

    /// <summary>
    /// Perform flat field correction, after calling ML_LoadFFCData().
    /// </summary>
    /// <param name="length">The length of ROI in the middle of the light image
    /// if the value=0</param>
    /// <param name="value">The default value is 0</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_DoFFCCorrect(int length = 100, int value = 0);

    /// <summary>
    /// Perform color shift correction, after calling ML_LoadColorShiftData().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_DoColorShiftCorrect();

    /// <summary>
    /// Perform distrotion correction, after calling ML_LoadDistortionData().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_DoDistortionCorrect();

    /// <summary>
    /// Perform exposure correct, after calling ML_LoadExposureData().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_DoExposureCorrect();

    /// <summary>
    /// Perform four color calculation, after calling ML_LoadFourColorData().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_DoFourColorCalculation();

    /// <summary>
    /// Perform luminance calculation, after calling ML_LoadLuminanceData().
    /// </summary>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_DoLuminanceCalculation();

    /// <summary>
    /// Obtain calibration data after flat field, color shift, distortion,four
    /// color and luminance process.
    /// </summary>
    /// <returns>a map of calibration data;
    /// CalibrationEnum: calibration type;
    /// string: the color filter's channel, contains X, Y, Z, Empty,
    /// etc;</returns>
    virtual std::map<ML::MLBusinessManage::CalibrationEnum,
                     std::map<std::string, cv::Mat>>
    ML_GetCalibrationData();

    /// <summary>
    /// Save images after calibration process.
    /// </summary>
    /// <param name="save_path">the path to save images</param>
    /// <param name="prefix">the prefix of the images</param>
    /// <param name="rect">the rect to save images</param>
    /// <param name="saveRaw">save Raw images</param>
    /// <param name="saveCalibration">save all calibration images</param>
    /// <returns>the result contains the message, code and status</returns>
    virtual Result ML_SaveCalibrationImage(std::string save_path,
                                           std::string prefix = "",
                                           cv::Rect rect = cv::Rect(),
                                           bool saveRaw = true,
                                           bool saveResult = true,
                                           bool saveCalibration = false);

    /// <summary>
    /// Clear calibration data map before start next calibration process.
    /// </summary>
    virtual Result ML_ClearCalibrationDataMap();

    /// <summary>
    /// Capture FFC image for sdk configuration.
    /// </summary>
    /// <param name="config">the config setting to capture image</param>
    /// <param name="cameraName">the camera to capture image</param>
    /// <param name="imageName">the name to save image</param>
    /// <param name="times">capture image by times, and calculate average
    /// image</param> 
    /// <returns>the result contains the message, code and
    /// status</returns>
    virtual Result ML_CaptureSDKFFCImage(
        ML::MLBusinessManage::ProcessPathConfig config, std::string cameraName,
        std::string imageName, int times = 5);

 private:
    Result CreateFilterWheel(
        ML::MLFilterWheel::MLNDFilterConfiguation configuration);

    Result CreateRXFilterWheel(
        ML::MLFilterWheel::MLRXFilterConfiguation configuration);

    Result CreateMotion(ML::MLBusinessManage::MotionConfig config);

    Result CreateCamera(ML::MLBusinessManage::CameraConfig config);

    Result ConnectFilterWheel();

    Result ConnectRXFilterWheel();

    Result ConnectMotion();

    Result ConnectCamera();

    Result DisConnectFilterWheel();

    Result DisConnectRXFilterWheel();

    Result DisConnectMotion();

    Result DisConnectCamera();

    bool IsFilterWheelConnect();

    bool IsRXFilterWheelConnect();

    bool IsMotionConnect();

    bool IsCameraConnect();

    bool IsFilterWheelMoving();

    bool IsRXFilterWheelMoving();

    bool IsMotionMoving();

    std::string tranCalibrationEnumToString(
        ML::MLBusinessManage::CalibrationEnum caliEnum);

    Result WriteMetaDatatoJson(std::string filename,
                               ML::MLBusinessManage::CaptureData data);

    std::string transBinningToString(ML::CameraV2::Binning binning);

    std::string transMLPixelFormatToString(ML::CameraV2::MLPixelFormat format);

    cv::Mat CalculateAverageImage(std::vector<cv::Mat> image_list);

 signals:
    void connectStatus(bool);
    void filterWheelStatus(bool);
    void RXFilterWheelStatus(bool);
    void motionStatus(bool);

 private:
    std::map<std::string, ML_addInBase*> m_addInBase;
    std::string m_ConfigPath;
    std::map<std::string, ML::MLBusinessManage::CaptureData> m_CaptureDataMap;
    std::map<ML::MLBusinessManage::CalibrationEnum,
             std::map<std::string, cv::Mat>>
        m_CalibrationDataMap;
    ML::MLBusinessManage::PreCalibrationData m_preCalibrationData;
    std::map<std::string, std::string> m_Filter;
    std::map<std::string, std::string> m_RXFilter;
    std::string m_Aperture;
    std::string m_ColorLight;
};
}  // namespace MLBusinessManage
}  // namespace ML

#endif  // SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGE_H_
