#ifndef MLX_IMAGE_TRANSFORM_HELPER_H_
#define MLX_IMAGE_TRANSFORM_HELPER_H_
#include <QImage>
#include "ImageViewerStructure.h"
#include "MLCamaraCommon.h"
using namespace  ML::CameraV2;
namespace ImageHelper
{
	QImage cvMatGray2RGBImage(const cv::Mat& mat, cv::Mat& dst8, MLPixelFormat pixFormat = MLPixelFormat::MLMono16);
    QImage cvMat2QImage(const cv::Mat& mat);
	cv::Mat convertTo3Channels(const cv::Mat& binImg);
}
#endif // !MLX_IMAGE_TRANSFORM_HELPER_H_

