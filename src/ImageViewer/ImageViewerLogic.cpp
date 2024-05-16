#include "ImageViewerLogic.h"
#include "MLImageTransformHelper.h"
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include <math.h>

#include <QFileDialog>
#include <QMutexLocker>

#include "StateMachine.h"


ImageViewerLogic::ImageViewerLogic(QObject* parent/*=nullptr*/) : QObject(parent)
{
}

ImageViewerLogic::~ImageViewerLogic()
{}

void ImageViewerLogic::setImage(cv::Mat cvImg)
{
	QMutexLocker locker(&m_mutex);
	
	m_cvFrame = cvImg.clone();
    m_QImage = ImageHelper::cvMat2QImage(cvImg);

}
void ImageViewerLogic::setYxyImage(cv::Mat& Y, cv::Mat& x, cv::Mat& y)
{
	m_Yxy.clear();
	m_Yxy.append(Y);
	m_Yxy.append(x);
	m_Yxy.append(y);
}
void ImageViewerLogic::setXYZImage(cv::Mat& X, cv::Mat& Y, cv::Mat& Z)
{
	QMutexLocker locker(&m_mutex);

	m_XYZImg.imgX = X.clone();
	m_XYZImg.imgY = Y.clone();
	m_XYZImg.imgZ = Z.clone();

}

void ImageViewerLogic::setImageList(QImage image)
{
	QMutexLocker locker(&m_mutex);
    if (m_Data.size() > 3)
    {
        m_Data.pop_front();
    }
    else
    {
        m_Data.push_back(image);
    }
}

void ImageViewerLogic::setMatList(cv::Mat image)
{
	QMutexLocker locker(&m_mutex);

    if (m_matData.size() > 3)
    {
        m_matData.pop_front();
    }
    else
    {
        m_matData.push_back(image);
    }
}

void ImageViewerLogic::setImageModel(ImageModel model)
{
	m_ImageModel = model;
}

cv::Mat ImageViewerLogic::GetCvMat()
{
	QMutexLocker local_lock(&m_mutex);
	cv::Mat res;
	if (!m_matData.isEmpty())
	{
		try
		{
			res = m_matData.front();
			m_matData.pop_front();
		}
		catch (...)
		{

		}
	}
	return res;
}

QImage ImageViewerLogic::GetQImage()
{
	QMutexLocker local_lock(&m_mutex);
	QImage res;
	if (!m_Data.isEmpty())
	{
		try
		{
			res = m_Data.front();
			m_Data.pop_front();
		}
		catch (...)
		{

		}
	}
	return res;
}
void ImageViewerLogic::onThreadStarted()
{


}

void ImageViewerLogic::onROIchanged(QString name)
{
	emit sigROIChanged(name);
}

void ImageViewerLogic::onUpdateImage()
{
	//qDebug() << "ImageViewerLogic::onUpdateImage StartTime:" << QDateTime::currentDateTime().toString("hh:mm:ss zzz");

	if (m_ImageModel == ImageViewerShowContinuous)
	{
        m_cvFrame = GetCvMat();
		m_QImage = GetQImage();
	}
    emit sigUpdateImage(m_QImage, false);

	//if (!m_CVImg.empty()) 
	//{
	//	m_cvFrame = m_CVImg.clone();
	//}
	//else return;}
	//else if (StateMachine::instance()->getShowMode() == ImageViewerShowModeRawY)
	//{
	//	m_cvFrame = m_XYZImg.imgY.clone();
	//}
	//
	//if (m_cvFrame.empty())
	//{
	//	return;
	//}

	//MLPixelFormat pixFormat;
	//if (m_cvFrame.type() == CV_16U)
	//{
	//	pixFormat = MLPixelFormat::MLMono12;
	//	//m_cvFrame = m_cvFrame.clone() * 0x10;
	//}
	//else if (m_cvFrame.type() == CV_8U)
	//{
	//	pixFormat = MLPixelFormat::MLMono8;
	//}
	//else
	//{
	//	qDebug() << "ImageViewerLogic::onUpdateImage" << m_cvFrame.type() << endl;
	//}

	//QImage img(m_cvFrame.cols, m_cvFrame.rows, QImage::Format_RGB32);
	////if (pseudoColor)
	////	img = ImageHelper::cvMatGray2RGBImage(m_cvFrame, m_CVRGB, MLPixelFormat::MLMono12);
	//
   //QImage img = ImageHelper::cvMat2QImage(m_CVImg);
}

void ImageViewerLogic::onUpdateRGBImage(bool isRGB)
{
    if (m_cvFrame.empty() || m_cvFrame.channels() != 1)
    {
        return;
    }
	QImage img;
	if(isRGB)
        img = ImageHelper::cvMatGray2RGBImage(m_cvFrame, m_CVRGB, MLPixelFormat::MLMono12);
	else
        img = m_QImage;
	emit sigUpdateImage(img, isRGB);
}

void ImageViewerLogic::onLinCutChanged(QPoint start, QPoint end)
{
	QDateTime startTime = QDateTime::currentDateTime();
    if (m_cvFrame.empty())
	{
		return;
	}
	if (end.x() == start.x())
	{
		return;
	}
	if (start.x() > end.x())
	{
		QPoint temp = start;
		start = end;
		end = temp;
	}

	//check range
	start.setX(start.x() < 0 ? 0 : start.x());
	start.setY(start.y() < 0 ? 0 : start.y());
	start.setX(start.x() >= m_cvFrame.cols ? m_cvFrame.cols-1 : start.x());
	start.setY(start.y() >= m_cvFrame.rows ? m_cvFrame.rows-1 : start.y());

	end.setX(end.x() < 0 ? 0 : end.x());
	end.setY(end.y() < 0 ? 0 : end.y());
	end.setX(end.x() >= m_cvFrame.cols ? m_cvFrame.cols-1 : end.x());
	end.setY(end.y() >= m_cvFrame.rows ? m_cvFrame.rows-1 : end.y());


	m_CutLineAxisXDatas.clear();
	m_CutLineAxisYGrayDatas.clear();
	m_CutLineAxisYRedDatas.clear();
	m_CutLineAxisYGreenDatas.clear();
	m_CutLineAxisYBlueDatas.clear();
	
	double k = double(end.y() - start.y()) / double(end.x() - start.x());
	int x = start.x();
	int count = end.x() - x;
	int step = count / 50;
	step = 1;
	int lastGray = -1;
	int nextGray = -1;
	while(x <= end.x())
	{
		int col = x;
		int y = round(start.y() + (x - start.x()) * k);
		int row = y;
		
		float fAxisx = abs(sqrt(x * x + y * y));
		m_CutLineAxisXDatas.append(fAxisx);
		
		float gray = 0;
  //      cv::Mat targetFrame;
		//if (m_cvFrame.channels() == 3)
		//{
  //          std::vector<cv::Mat> channels;
  //          cv::split(m_cvFrame, channels);
  //          int targetChannel = GetBrightestChannel();
  //          targetFrame = channels[targetChannel];
  //      } 
		//else if (m_cvFrame.channels() == 1) 
		//{
  //          targetFrame = m_cvFrame;
  //      }

		if (m_cvFrame.type() == CV_16U)
		{
            ushort pixelValue = m_cvFrame.at<ushort>(row, col);
            gray = pixelValue /*/ 16.0*/;
		}
		else if (m_cvFrame.type() == CV_8U)
		{
            uchar pixelValue = m_cvFrame.at<uchar>(row, col);
            gray = pixelValue;
		}
		else if (m_cvFrame.type() == CV_32F) 
		{
            gray = m_cvFrame.at<float>(row, col);
        }

		//
		if ((gray != lastGray) /*|| (gray != nextGray) */|| (x == end.x()))
		{
			
			m_CutLineAxisYGrayDatas.append(QPointF(fAxisx, gray));
			if (m_CVRGB.empty())
			{
				m_CutLineAxisYRedDatas.append(QPointF(fAxisx, gray));
				m_CutLineAxisYGreenDatas.append(QPointF(fAxisx, gray));
				m_CutLineAxisYBlueDatas.append(QPointF(fAxisx, gray));
			}
			else
			{
				m_CutLineAxisYRedDatas.append(QPointF(fAxisx, m_CVRGB.at<uchar>(row, 3 * col)));
				m_CutLineAxisYRedDatas.append(QPointF(fAxisx, m_CVRGB.at<uchar>(row, 3 * col + 1)));
				m_CutLineAxisYRedDatas.append(QPointF(fAxisx, m_CVRGB.at<uchar>(row, 3 * col + 2)));
			}
			lastGray = gray;
		}
		if (x == end.x())
		{
			x++;
		}
		else if (x + step <= end.x())
		{
			x += step;
		}
		else
		{
			x = end.x();
		}
	}

	QDateTime endTime = QDateTime::currentDateTime();
	/*qDebug() << __FUNCTION__ << "endTime=" << endTime.toString("hh:mm:ss.zzz");
	qDebug() << __FUNCTION__ << startTime.msecsTo(endTime);*/

	emit sigUpdateCutLine(m_CutLineAxisXDatas, m_CutLineAxisYGrayDatas, m_CutLineAxisYRedDatas, m_CutLineAxisYGreenDatas, m_CutLineAxisYBlueDatas);

}

void ImageViewerLogic::onSaveImageClicked(QString filename)
{
	if (!m_cvFrame.empty())
	{
		cv::imwrite(filename.toStdString(), m_cvFrame);
	}
}

void ImageViewerLogic::onMousePosChanged(QPoint pixmapItemPos)
{
	int row = pixmapItemPos.y();
	int col = pixmapItemPos.x();

	if (m_cvFrame.empty())
	{
		return;
	}

    if (row < 0 || row >= m_cvFrame.rows || col < 0 || col >= m_cvFrame.cols)
	{
		return;
	}

	float gray = -1;
	float x = 0.0f;
	float y = 0.0f;
	float Y = 0.0f;
 /*   cv::Mat targetFrame;
	if (m_cvFrame.channels() == 3) 
	{
        std::vector<cv::Mat> channels;
        cv::split(m_cvFrame, channels);
        int targetChannel = GetBrightestChannel();
        targetFrame = channels[targetChannel];
    }
	else if (m_cvFrame.channels() == 1) 
	{
        targetFrame = m_cvFrame;
    }*/

	 if (m_cvFrame.type() == CV_16U) 
	 {
        ushort pixelValue = m_cvFrame.at<ushort>(row, col);
        gray = pixelValue;
	 }
	 else if (m_cvFrame.type() == CV_8U) 
	 {
         uchar pixelValue = m_cvFrame.at<uchar>(row, col);
         gray = pixelValue;
     } else if (m_cvFrame.type() == CV_32F)
	 {
         gray = m_cvFrame.at<float>(row, col);
     }
     if (m_Yxy.size() > 0) {
         if (row >= 0 && row < m_Yxy[0].rows && col >= 0 &&
             col < m_Yxy[0].cols) {
             Y = m_Yxy[0].at<float>(row, col);
             x = m_Yxy[1].at<float>(row, col);
             y = m_Yxy[2].at<float>(row, col);
         }
     }

	StateMachine::instance()->setGrayValue(gray);
	StateMachine::instance()->setxyYValue(x, y, Y);
    emit sigUpdateGrayValue(pixmapItemPos);
	//qDebug() << __FUNCTION__ << QDateTime::currentDateTime().toString("hh:mm:ss zzz ") << row << col << gray;
}
