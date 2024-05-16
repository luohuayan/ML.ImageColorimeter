#include "MLImageTransformHelper.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <QDebug>
#include <QDateTime>
namespace ImageHelper 
{
    QImage cvMatGray2RGBImage(const cv::Mat& srcMat, cv::Mat & dst8, MLPixelFormat pixFormat)
    {
        //To CV_8UC1
        if (srcMat.type() == CV_32F) 
        {
            double Min = 0.0, Max = 0.0;
            double* minp = &Min;
            double* maxp = &Max;
            minMaxIdx(srcMat, minp, maxp);
            if (Min != Max) {
                srcMat.convertTo(dst8, CV_8U, 255.0 / (Max - Min), -255.0 * Min / (Max - Min));
            }
        }
        else if (srcMat.type() == CV_16U)
		{
            srcMat.convertTo(dst8, CV_8UC1, 255.0 / 4095.0);
		}
		else if(srcMat.type() == CV_8U)
		{
			dst8 = srcMat.clone();
		}
        else
        {
            return QImage();
        }

        //to color
        cv::applyColorMap(dst8, dst8, cv::COLORMAP_JET);

        //BRG to RGB
		cv::cvtColor(dst8, dst8, cv::COLOR_BGR2RGB);

        //RGB888
        QImage img = QImage(static_cast<uchar*>(dst8.data), dst8.cols, dst8.rows, QImage::Format_RGB888);

		return img;
	}
    QImage cvMat2QImage(const cv::Mat& mat)
    {
        {
            if (mat.type() == CV_8UC1)
            {
                QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
                image.setColorCount(256);
                for (int i = 0; i < 256; i++)
                {
                    image.setColor(i, qRgb(i, i, i));
                }
                uchar* pSrc = mat.data;
                for (int row = 0; row < mat.rows; row++)
                {
                    uchar* pDest = image.scanLine(row);
                    memcpy(pDest, pSrc, static_cast<quint32>(mat.cols));
                    pSrc += static_cast<quint32>(mat.step);
                }
                return image;
            }
            else if (mat.type() == CV_16UC1)
            {
                cv::Mat dst8 = cv::Mat::zeros(mat.size(), CV_8U);
                double mymin, mymax;
                cv::minMaxIdx(mat, &mymin, &mymax);
                assert(mymax >= mymin);
                double delt;
                if (mymax == mymin)
                {
                    delt = 255;
                }
                else
                {
                    delt = 255.0 / (mymax - mymin);
                }

                for (int i = 0; i < mat.rows; i++)
                {
                    for (int j = 0; j < mat.cols; j++)
                    {
                        ushort s = mat.at<ushort>(i, j);
                        uchar d = (s - mymin) * delt;
                        dst8.at<uchar>(i, j) = d;
                    }
                }

                QImage image(dst8.cols, dst8.rows, QImage::Format_Indexed8);
                image.setColorCount(256);
                for (int i = 0; i < 256; i++)
                {
                    image.setColor(i, qRgb(i, i, i));
                }
                uchar* pSrc = dst8.data;
                for (int row = 0; row < dst8.rows; row++)
                {
                    uchar* pDest = image.scanLine(row);
                    memcpy(pDest, pSrc, static_cast<quint32>(dst8.cols));
                    pSrc += static_cast<quint32>(dst8.step);
                }
                return image;
            }
            else if (mat.type() == CV_8UC3)
            {
                const uchar* pSrc = const_cast<const uchar*>(mat.data);
                QImage image(pSrc, mat.cols, mat.rows,
                    static_cast<qint32>(mat.step), QImage::Format_RGB888);
                return image.rgbSwapped();
            }
            else if (mat.type() == CV_8UC4)
            {
                const uchar* pSrc = const_cast<const uchar*>(mat.data);
                QImage image(pSrc, mat.cols, mat.rows,
                    static_cast<qint32>(mat.step), QImage::Format_ARGB32);
                return image.copy();
            }
            else if (mat.type() == 22)
            {
                cv::Mat src;
                mat.convertTo(src, CV_8UC3);
                const uchar* pSrc = const_cast<const uchar*>(src.data);
                QImage image(pSrc, src.cols, src.rows,
                    static_cast<qint32>(src.step), QImage::Format_RGB888);
                return image.rgbSwapped();
            } 
            else if (mat.type() == CV_16UC3) 
            {
                cv::Mat dst8 = cv::Mat::zeros(mat.size(), CV_8UC3);

                double mymin, mymax;
                cv::minMaxIdx(mat, &mymin, &mymax);
                assert(mymax >= mymin);
                double delt;
                if (mymax == mymin) {
                    delt = 255;
                } else {
                    delt = 255.0 / (mymax - mymin);
                }

                for (int i = 0; i < mat.rows; i++) {
                    for (int j = 0; j < mat.cols; j++) {
                        cv::Vec3w s = mat.at<cv::Vec3w>(i, j);
                        cv::Vec3b d;
                        for (int k = 0; k < 3; k++) {
                            d[k] = static_cast<uchar>((s[k] - mymin) * delt);
                        }
                        dst8.at<cv::Vec3b>(i, j) = d;
                    }
                }

                QImage image(dst8.cols, dst8.rows, QImage::Format_RGB888);
                uchar* pSrc = dst8.data;
                for (int row = 0; row < dst8.rows; row++) {
                    uchar* pDest = image.scanLine(row);
                    memcpy(pDest, pSrc, static_cast<quint32>(dst8.cols) * 3);
                    pSrc += static_cast<quint32>(dst8.step);
                }
                return image.rgbSwapped();// BGR to RGB
            }
            else if (mat.type() == CV_32F)
            {
                double minVal, maxVal;
                cv::minMaxLoc(mat, &minVal, &maxVal);
                double alpha = 255.0 / (maxVal - minVal);
                double beta = -minVal * 255.0 / (maxVal - minVal);
                cv::Mat dst = cv::Mat::zeros(mat.size(), CV_8UC1);
                mat.convertTo(dst, CV_8U, alpha, beta);

                QImage image(dst.data, dst.cols, dst.rows, static_cast<int>(dst.step),
                    QImage::Format_Grayscale8);

                return image.copy();
            }
            else
            {
                return QImage();
            }
        }
        //cv::Mat dst8;
        //if (mat.type() == CV_16U)
        //{
        //    cv::Mat tmp;
        //    dst8 = cv::Mat::zeros(mat.size(), CV_8U);
        //    double minv = 0.0, maxv = 0.0;
        //    double* minp = &minv;
        //    double* maxp = &maxv;
        //    cv::minMaxIdx(mat, minp, maxp);
        //    if (maxv > 4095.1)
        //    {
        //        cv::Mat tmp = mat / 65535 * 255;
        //        tmp.convertTo(dst8, CV_8UC1);
        //    }
        //    else
        //    {
        //        cv::Mat tmp = mat / 4095.0 * 255;
        //        tmp.convertTo(dst8, CV_8UC1);
        //    }
        //}
        //else
        //{
        //    dst8 = mat.clone();
        //}
        //QImage img;
        //int chana = dst8.channels();
        //if (3 == chana)
        //{
        //    cv::cvtColor(dst8, dst8, cv::COLOR_BGR2RGB);
        //    img = QImage(static_cast<uchar*>(dst8.data), dst8.cols, dst8.rows, QImage::Format_RGB888);
        //}
        //else if (4 == chana)
        //{
        //    // argb
        //    img = QImage(static_cast<uchar*>(dst8.data), dst8.cols, dst8.rows, QImage::Format_ARGB32);
        //}
        //else
        //{
        //    img = QImage(dst8.cols, dst8.rows, QImage::Format_Grayscale8);
        //    uchar* matdata = dst8.data;
        //    for (int row = 0; row < dst8.rows; ++row)
        //    {
        //        uchar* rowdata = img.scanLine(row);
        //        memcpy(rowdata, matdata, dst8.cols);
        //        matdata += dst8.cols;
        //    }
        //}
        //return img;
    }
    cv::Mat convertTo3Channels(const cv::Mat& binImg)
    {
        if (binImg.type() == CV_8UC1)
        {
            cv::Mat three_channel = cv::Mat::zeros(binImg.rows, binImg.cols, CV_8UC3);
            std::vector<cv::Mat> channels;
            for (int i = 0; i < 3; i++)
            {
                channels.push_back(binImg);
            }
            merge(channels, three_channel);
            return three_channel;
        }
        else
        {
            return binImg;
        }
    }
}


