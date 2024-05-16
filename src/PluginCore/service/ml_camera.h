#ifndef COMMON_MLCAMERA_H_
#define COMMON_MLCAMERA_H_

#include"ml.h"

// FixMe: added the following line 
#include <opencv2/core.hpp>

using namespace ML;

class MLCameraCallback 
{
public:

    virtual void NotifyCameraStateChanged(MLCameraState old_state, MLCameraState new_state) = 0;

    virtual void NotifyCameraFrameReceived(MLFrame* frame_ptr, void* buffer_ptr) = 0;

    virtual void NotifyCameraGrayLevel(int gray_level) = 0;

};


class MLCamera
{
public:

    virtual ~MLCamera() {};

public:

    virtual MLResult Open() = 0;

    virtual MLResult OpenBySN(const char* SN) = 0;

    virtual void Close() = 0;

    virtual bool IsOpened() = 0;

    virtual MLCameraInfo GetCameraInfo() = 0;

    virtual MLResult GrabOne() = 0;

    virtual void StartGrabbingAsync() = 0;

    virtual void StopGrabbing() = 0;

    virtual bool IsGrabbing() = 0;

    virtual int GetWidth() = 0;

    virtual int GetHeight() = 0;

    virtual void SetFrameRate(double rate) = 0;

    virtual double GetFrameRate() = 0;

    virtual void SetFormatType(MLPixelFormat format) = 0;

    virtual MLPixelFormat GetFormatType() = 0;

    virtual void SetMLExposureAuto() = 0;

    virtual void SetExposureMode(MLExposureMode mode) = 0;

    //  set exposure time of the camera, when the exposure mode is auto, this method does
    //  not have any effect on exposure time, need to set exposure mode Off first
    virtual void SetExposureTime(double time) = 0;

    virtual double GetExposureTime() = 0;

    virtual void GetExposureTimeRange(double& min, double& max) = 0;

    virtual void SetGain(double value) = 0;

    virtual double GetGain() = 0;

    virtual void GetGainRange(double& min, double& max) = 0;

    //  Set the exposure time to be an integer multiple of the AR glasses screen refresh
    //  rate��the camera can capture the complete image data in units of a AR glass refresh rate
    virtual void SetAdjustExposureTime(double time) = 0;

    virtual MLCameraState GetState() = 0;

    virtual void FlipX(bool flip) = 0;

    virtual void FlipY(bool flip) = 0;

    virtual void Subscribe(MLCameraEvent event, MLCameraCallback* callback) = 0;

    virtual void Unsubscribe(MLCameraEvent event, MLCameraCallback* callback) = 0;

    virtual cv::Mat GetImage() = 0;

    virtual void StartCalcGrayLevel() = 0;

    virtual void StopCalcGrayLevel() = 0;

    virtual void Rotate(bool rotate) = 0;
public:
    bool disconnect_;

    std::string serial_number_;

    int index_;

    bool pixel_fix_;
};

#endif // !COMMON_MLCAMER_H_
