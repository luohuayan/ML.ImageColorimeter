#pragma once
#include <qobject.h>
#include "MLCamaraCommon.h"
#include <QImage>

class MLCameraShow : public QObject, public ML::CameraV2::MLCameraCallback {

    Q_OBJECT
public:
    MLCameraShow();
    ~MLCameraShow();
public:
    //virtual void NotifyCameraStateChanged(ML::MLCameraState old_state, ML::MLCameraState new_state);
    //virtual void NotifyCameraFrameReceived(MLFrame* frame_ptr, void* buffer_ptr);
    //virtual void NotifyCameraGrayLevel(int gray_level);
    //virtual void NotifyCameraFrameRate(double fps);

    virtual void NotifyCameraStateChanged(ML::CameraV2::MLCameraState old_state, ML::CameraV2::MLCameraState new_state);
    virtual void NotifyCameraFrameReceived(cv::Mat,ML::CameraV2::MLPixelFormat format);
    virtual void NotifyCameraGrayLevel(int gray_level);

private:
   bool MLIamgeTransform(QImage& Dst, cv::Mat& Src, ML::CameraV2::MLPixelFormat format);
   int GetBrightestChannel(cv::Mat m_cvFrame);

signals:
    void SendCameraFrameReceiveSignal(cv::Mat m_mat, QImage m_image);
    void SendCameraGrayLevelSignal(int grayLevel);
    void SendCameraFpsSignal(double fps);
    void cameraStatus(bool status,int cameraId);
};

