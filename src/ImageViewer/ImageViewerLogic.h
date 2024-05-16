#pragma once

#include <QObject>
#include <QMutex>
#include "imageviewer_global.h"
#include "ImageViewerWidget.h"

#include <opencv2/opencv.hpp>

class ImageViewerLogic  : public QObject
{
	Q_OBJECT

public:
	ImageViewerLogic(QObject *parent=nullptr);
	~ImageViewerLogic();

	void setImage(cv::Mat img);
	void setYxyImage(cv::Mat& Y, cv::Mat& x, cv::Mat& y);
	void setXYZImage(cv::Mat& X, cv::Mat& Y, cv::Mat& Z);
	void setImageList(QImage image);
	void setMatList(cv::Mat image);
	void setImageModel(ImageModel model);
	cv::Mat GetCvMat();
	QImage GetQImage();
 public slots:
	void onThreadStarted();
	void onROIchanged(QString);
	void onUpdateImage();
	void onUpdateRGBImage(bool isRGB);
	void onLinCutChanged(QPoint start, QPoint end);
	void onSaveImageClicked(QString filename);
	void onMousePosChanged(QPoint pixmapItemPos);
signals:
	void sigUpdateImage(QImage image, bool pseudo);
	void sigROIChanged(QString);
	void sigUpdateCutLine(QList<float>, QList<QPointF>, QList<QPointF>, QList<QPointF>, QList<QPointF>);
    void sigUpdateGrayValue(QPoint point);


private:
	//ImageViewerWidget* m_imageViewerWidget;
	QMutex m_mutex;
	XYZImage m_XYZImg;
	ImageModel m_ImageModel;
	cv::Mat m_CVImg;
	cv::Mat m_cvFrame;
	QImage m_QImage;
	cv::Mat m_CVRGB;
	bool pseudoColor = false;
	QList<QImage> m_Data;
	QList<cv::Mat> m_matData;
	QVector<cv::Mat> m_Yxy;
	QList<float> m_CutLineAxisXDatas;
	QList<QPointF> m_CutLineAxisYGrayDatas;
	QList<QPointF> m_CutLineAxisYRedDatas;
	QList<QPointF> m_CutLineAxisYGreenDatas;
	QList<QPointF> m_CutLineAxisYBlueDatas;
};
