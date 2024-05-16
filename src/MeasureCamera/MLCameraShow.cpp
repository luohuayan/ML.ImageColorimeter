#include "MLCameraShow.h"
#include "MLImageTransformHelper.h"
#include <QDateTime>
#include <QThread>
#include "VmbTransform.h"

MLCameraShow::MLCameraShow() 
{ 
}
MLCameraShow::~MLCameraShow()
{
}

void MLCameraShow::NotifyCameraStateChanged(ML::CameraV2::MLCameraState old_state,ML::CameraV2::MLCameraState new_state) {
	
}

void MLCameraShow::NotifyCameraFrameReceived(cv::Mat cv_image, ML::CameraV2::MLPixelFormat format) {
    cv::Mat targetFrame;
    if (cv_image.channels() == 3) 
    {
        std::vector<cv::Mat> channels;
        cv::split(cv_image, channels);
        int targetChannel = GetBrightestChannel(cv_image);
        targetFrame = channels[targetChannel];
    }
    else 
    {
        targetFrame = cv_image;
    }
    QImage img = ImageHelper::cvMat2QImage(targetFrame);  

    // MLIamgeTransform(img, cv_image, format);
    emit SendCameraFrameReceiveSignal(targetFrame, img);
   
}

void MLCameraShow::NotifyCameraGrayLevel(int gray_level)
{ 
    int m_GrayLevel = gray_level;
    emit SendCameraGrayLevelSignal(m_GrayLevel);
}

//void MLCameraShow::NotifyCameraFrameRate(double fps)
//{
//    if (m_selCameraId == m_cameraId)
//    {
//        emit SendCameraFpsSignal(fps);
//    }
//}
//bool MLCameraShow::MLIamgeTransform(
//    QImage& Dst,
//                      cv::Mat& Src, ML::CameraV2::MLPixelFormat format) {
//    VmbError_t Result;
//    if (Dst.isNull()) {
//        return false;
//    }
//    VmbImage source_image;
//    VmbImage destination_image;
//    source_image.Size = sizeof(source_image);
//    destination_image.Size = sizeof(destination_image);
//    std::string pixel_in;
//    switch (format) {
//        case MLPixelFormat::MLMono8:
//        case MLPixelFormat::MLBayerGB8:
//            pixel_in = "Mono8";
//            break;
//        case MLPixelFormat::MLMono10:
//        case MLPixelFormat::MLBayerGB10:
//            pixel_in = "Mono10";
//            break;
//        case MLPixelFormat::MLMono12:
//        case MLPixelFormat::MLBayerGB12:
//            pixel_in = "Mono12";
//            break;
//        default:
//            pixel_in = "BGR24";
//            break;
//    }
//    Result = VmbSetImageInfoFromString(
//        pixel_in.c_str(), static_cast<VmbUint32_t>(pixel_in.size()),
//        static_cast<VmbUint32_t>(Src.rows), static_cast<VmbUint32_t>(Src.cols),
//        &source_image);
//    if (VmbErrorSuccess != Result) {
//        return false;
//    }
//
//    unsigned int dst_bits_per_pixel = Dst.depth();
//    std::string pixel_out;
//    switch (dst_bits_per_pixel) {
//        default:
//            return false;
//        case 8:
//            pixel_out = "Mono8";
//            break;
//        case 24:
//            pixel_out = "BGR24";
//            break;
//        case 32:
//            pixel_out = "BGRA32";
//            break;
//    }
//    Result = VmbSetImageInfoFromString(
//        pixel_out.c_str(), static_cast<VmbUint32_t>(pixel_out.size()),
//        static_cast<VmbUint32_t>(Dst.width()),
//        static_cast<VmbUint32_t>(Dst.height()), &destination_image);
//    source_image.Data = &Src;
//    destination_image.Data = Dst.bits();
//    Result = VmbImageTransform(&source_image, &destination_image, NULL, 0);
//    if (VmbErrorSuccess == Result) {
//        return true;
//    } else {
//        return false;
//    }
//}

int MLCameraShow::GetBrightestChannel(cv::Mat m_cvFrame) {
    int w = 100, h = 100;
    int imgWidth = m_cvFrame.cols;
    int imgHeight = m_cvFrame.rows;
    int x = (imgWidth - 100) / 2;
    int y = (imgHeight - 100) / 2;
    cv::Rect roi_rect(x, y, w, h);
    cv::Mat roi = m_cvFrame(roi_rect);

    cv::Scalar sum = cv::sum(roi);
    double sum_r = sum[2];
    double sum_g = sum[1];
    double sum_b = sum[0];

    if (sum_r > sum_g && sum_r > sum_b) {
        return 2;
    } else if (sum_g > sum_r && sum_g > sum_b) {
        return 1;
    } else {
        return 0;
    }
}
