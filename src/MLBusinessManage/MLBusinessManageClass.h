// Copyright 2024 MLOptic

#ifndef SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGECLASS_H_
#define SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGECLASS_H_

#include <map>
#include <opencv2/opencv.hpp>
#include <string>

#include "MLFilterWheelClass.h"
#include "MLCamaraCommon.h"

namespace ML {
namespace MLBusinessManage {
struct MotionConfig {
    std::string SerialNumber;
    bool Enable;
    std::string Type;
    std::string Name;
    std::string ConnectAddress;
    std::string ConnectType;
    std::string Port;
    int DeviceID;
    int Axis;
    std::string AxisName;
    std::string HomingMethod;
    double Speed;
    bool IsReverse;
    double SoftwareLimitMax;
    double SoftwareLimitMin;
    double ReferencePosistion;
    double FocalLength;
    double FocalPlanesObjectSpace;
};

struct CameraConfig {
    std::string SerialNumber;
    bool Enable;
    bool ColourCamera;
    std::string ConnectAddress;
    std::string Type;
    std::string Name;
};

struct ModuleConfig {
    std::string SerialNumber;
    std::string Name;
    std::map<std::string, MotionConfig> MotionConfig_Map;
    std::map<std::string, CameraConfig> CameraConfig_Map;
    std::map<std::string, ML::MLFilterWheel::MLNDFilterConfiguation>
        NDFilterConfig_Map;
    std::map<std::string, ML::MLFilterWheel::MLRXFilterConfiguation>
        RXFilterConfig_Map;
};

struct CaptureData {
    std::string SerialNumber;
    std::string ModuleName;
    std::string Aperture;
    std::string ColorLight;
    std::string NDFilter;
    std::string ColorFilter;
    std::string RXFilter;
    int RXAxisDegree;
    double VID;
    double ExposureTime;
    cv::Mat Img;
    ML::CameraV2::Binning Binning;
    ML::CameraV2::MLPixelFormat BitDepth;
    CaptureData() {
        Aperture = "3mm";
        ColorLight = "";
        NDFilter = "ND0";
        ColorFilter = "X";
        RXFilter = "0d";
        RXAxisDegree = 0;
        VID = 0;
        ExposureTime = 100;
        Binning = ML::CameraV2::Binning::ONE_BY_ONE;
        BitDepth = ML::CameraV2::MLPixelFormat::MLMono12;
    }
};

enum class CalibrationEnum {
    Raw = 0,
    FFC = 1,
    ColorShift,
    Distortion,
    ExposureCorrect,
    FourColor,
    Lumiance,
    Result
};

struct PreCalibrationData {
    // X, Y, Z color filter map
    std::map<std::string, cv::Mat> LightFFCMap;
    // X, Y, Z color filter map
    std::map<std::string, cv::Mat> DarkFFCMap;
    // X, Y, Z color filter map
    std::map<std::string, cv::Point2d> PointMap;
    cv::Mat CameraMatrix;
    cv::Mat Coefficient;
    cv::Mat RMatrix;
    // X, Y, Z color filter map
    std::map<std::string, cv::Mat> LuminanceKMap;
};

struct FolderRule {
    std::string Rule;
    std::string Suffix;
};

struct ProcessPathConfig {
    std::string InputPath;
    std::string NDFilter;
    std::string ColorFilter;
    std::vector<std::string> ColorFilterArr;
    std::string ColorLight;
    std::string RxStr;
    std::string Aperture;
};

enum class FoucsMethod {
    OneMotor = 1
};
}  // namespace MLBusinessManage
}  // namespace ML

#endif  // SRC_MLBUSINESSMANAGE_MLBUSINESSMANAGECLASS_H_
